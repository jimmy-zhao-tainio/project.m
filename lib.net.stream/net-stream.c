#include <lib.net/net.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/tree.h>
#include <lib.core/threads.h>
#include <lib.core/hash.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <lib.net.stream/net-stream.h>

static void worker (Thread *worker_thread);
static bool worker_read  (NetStream *stream, NetStreamConnection connection);
static bool worker_write (NetStream *stream, NetStreamConnection connection);
static bool connection_index (NetStreamThread *thread, NetStreamConnection *connection);

NetStream *net_stream_create (NetStreamOnClose on_close, 
                              NetStreamOnRead on_read,
                              NetStreamOnError on_error,
                              size_t threads_count,
                              size_t connections_per_thread)
{
        NetStream *stream;
        size_t i;

        if (threads_count < 2) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (threads_count % 2 != 0) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_close) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!on_read) {
                error_code (InvalidArgument, 4);
                return NULL;
        }
        if (!on_error) {
                error_code (InvalidArgument, 5);
                return NULL;
        }
        if (!(stream = memory_create (sizeof (NetStream)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        stream->on_close = on_close;
        stream->on_read = on_read;
        stream->on_error = on_error;
        stream->threads_count = threads_count;
        if (!(stream->connections = memory_create (sizeof (NetStreamConnection) * 
                                                   threads_count * 
                                                   connections_per_thread))) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(stream->threads = memory_create (sizeof (NetStreamThread) * threads_count))) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 3);
                return NULL;
        }
        for (i = 0; i < threads_count; i++) {
                stream->threads[i].stream = stream;
                stream->threads[i].connections = (stream->connections + (connections_per_thread * i));
                stream->threads[i].connections_size = connections_per_thread;
                stream->threads[i].connections_count = 0;
                if (!thread_lock_create (&stream->threads[i].lock)) {
                        net_stream_destroy (stream);
                        error_code (FunctionCall, 4);
                        return NULL;
                }
                if (!(stream->threads[i].thread = thread_create (&worker, &stream->threads[i]))) {
                        net_stream_destroy (stream);
                        error_code (FunctionCall, 5);
                        return NULL;
                }
        }
        return stream;
}

void net_stream_destroy (NetStream *stream)
{
        size_t i;

        if (!stream) {
                error (InvalidArgument);
        }
        if (stream->threads) {
                for (i = 0; i < stream->threads_count; i++) {
                        if (stream->threads[i].lock.initialized) {
                                (void)thread_lock_destroy (&stream->threads[i].lock);
                        }
                        if (!stream->threads[i].thread) {
                                continue;
                        }
                        if (!thread_set_exit (stream->threads[i].thread)) {
                                error_code (FunctionCall, 1);
                        }
                        if (!thread_wait (stream->threads[i].thread)) {
                                error_code (FunctionCall, 2);
                        }
                        thread_destroy (stream->threads[i].thread);
                }
                memory_destroy (stream->threads);
        }
        if (stream->connections) {
                memory_destroy (stream->connections);
        }
        memory_destroy (stream);
}

bool net_stream_add (NetStream *stream, int socket, void *pointer, NetStreamConnection *connection)
{
        size_t i;
        size_t available;
        size_t maximum = 0;
        NetStreamThread *thread = NULL;

        for (i = 0; i < stream->threads_count; i++) {
                if (!thread_lock (&stream->threads[i].lock)) {
                        error_code (FunctionCall, 1);
                        return false;
                }
                available = stream->threads[i].connections_size - stream->threads[i].connections_count;
                if (!thread_unlock (&stream->threads[i].lock)) {
                        error_code (FunctionCall, 2);
                        return false;
                }
                if (available > maximum) {
                        maximum = available;
                        thread = &stream->threads[i];
                }
        }
        if (maximum == 0) {
                return false;
        }
        if (!thread_lock (&thread->lock)) {
                error_code (FunctionCall, 3);
                return false;
        }
        connection->object.id = object_id ();
        connection->pointer = pointer;
        connection->socket = socket;
        connection->index = thread->connections_count;
        connection->thread = thread;
        thread->connections[thread->connections_count] = *connection;
        thread->connections_count += 1;
        if (!thread_unlock (&thread->lock)) {
                error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

void net_stream_remove (NetStream *stream, NetStreamConnection connection)
{
        NetStreamThread *thread = connection.thread;

        (void)stream;
        if (!thread_lock (&thread->lock)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (connection_index (thread, &connection)) {
                if (thread->connections_count > 1) {
                        thread->connections[connection.index] = thread->connections[thread->connections_count - 1];
                        thread->connections[connection.index].index = connection.index;
                }
                thread->connections_count--;
        }
        if (!thread_unlock (&thread->lock)) {
                error_code (FunctionCall, 2);
                return;
        }
}

void net_stream_write (NetStream *stream, NetStreamConnection connection, NetStreamPackage package)
{
        NetStreamThread *thread = connection.thread;
        bool loop = true;

        (void)stream;
        while (true) {
                if (!thread_lock (&thread->lock)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                if (connection_index (thread, &connection)) {
                        if (thread->connections[connection.index].write.length == 0) {
                                thread->connections[connection.index].write = package;
                                loop = false;
                        }
                }
                else {
                        loop = false;
                }
                if (!thread_unlock (&thread->lock)) {
                        error_code (FunctionCall, 2);
                        return;
                }
                if (loop) {
                        usleep (10);
                }
        }
}

static void worker (Thread *worker_thread)
{
        NetStreamThread *thread = (NetStreamThread *)worker_thread->argument;
        NetStream *stream = thread->stream;
        NetStreamConnection *connections = thread->connections;
        size_t connections_count = thread->connections_count;
        size_t i;

        while (true) {
                if (!thread_lock (&thread->lock)) {
                        error_code (FunctionCall, 1);
                        stream->on_error (stream);
                        return;
                }
                connections_count = thread->connections_count;
                for (i = 0; i < connections_count;) {
                        if (!worker_read (stream, connections[i])) {
                                net_stream_remove (stream, connections[i]);
                                stream->on_close (stream, connections[i]);
                                continue;
                        }
                        if (!worker_write (stream, connections[i])) {
                                net_stream_remove (stream, connections[i]);
                                stream->on_close (stream, connections[i]);
                                continue;
                        }
                        i++;
                }
                if (!thread_unlock (&thread->lock)) {
                        error_code (FunctionCall, 2);
                        stream->on_error (stream);
                        return;
                }
                if (thread_get_exit (worker_thread)) {
                        thread_exit (worker_thread);
                }
                usleep (10);
        }
}

static bool worker_read (NetStream *stream, NetStreamConnection connection) 
{
        NetStreamPackage package;
        char buffer[512];
        ssize_t count;

        count = read (connection.socket, buffer, sizeof (buffer));
        if (count == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                return true;
        }
        if (count == -1 || count == 0) {
                return false;
        }
        package.buffer = buffer;
        package.length = (size_t)count;
        stream->on_read (stream, connection, package);
        return true;
}

static bool worker_write (NetStream *stream, NetStreamConnection connection)
{
        ssize_t count;

        (void)stream;
        if (connection.write.length == 0) {
                return true;
        }
        count = write (connection.socket, 
                       connection.write.buffer + connection.write.position,
                       connection.write.length - connection.write.position);
        if (count == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                return true;
        }
        if (count == -1 || count == 0) {
                return false;
        }
        connection.write.position += (size_t)count;
        if (connection.write.position == connection.write.length) {
                connection.write.length = 0;
        }
        return true;
}

static bool connection_index (NetStreamThread *thread, NetStreamConnection *connection)
{
        size_t i;

        if (thread->connections[connection->index].object.id == connection->object.id) {
                return true;
        }
        if (connection->index >= thread->connections_count) {
                i = thread->connections_count;
        }
        else {
                i = connection->index + 1;
        }
        while (i --> 0) {
                if (thread->connections[i].object.id == connection->object.id) {
                        connection->index = i;
                        return true;
                }
        }
        return false;
}
