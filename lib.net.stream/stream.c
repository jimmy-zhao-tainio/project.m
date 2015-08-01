#include <lib.core/memory.h>
#include <lib.core/error.h>
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
#include <lib.net.stream/stream.h>

static void worker (Thread *thread);
static bool connection_read (NetStream *stream, NetStreamConnection *connection);

NetStream *net_stream_create (NetStreamOnRead on_read, 
                              NetStreamOnClose on_close,
                              NetStreamOnError on_error,
                              void *tag)
{
        NetStream *stream;

        if (!on_read) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (!on_close) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_error) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!(stream = memory_create (sizeof (NetStream)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        stream->on_read = on_read;
        stream->on_close = on_close;
        stream->on_error = on_error;
        stream->tag = tag;
        if (!thread_signal_create (&stream->stop)) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(stream->epoll = net_stream_epoll_allocate ())) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(stream->thread = thread_create (&worker, stream))) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return stream;
}

void net_stream_destroy (NetStream *stream)
{
        if (!stream) {
                error (InvalidArgument);
        }
        if (stream->stop.initialized && 
            stream->epoll && 
            stream->thread) {
                if (!net_stream_epoll_stop (stream->epoll)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                if (!thread_signal_wait (&stream->stop)) {
                        error_code (FunctionCall, 2);
                }
        }
        if (stream->thread) {
                if (!thread_wait (stream->thread)) {
                        error_code (FunctionCall, 3);
                }
                thread_destroy (stream->thread);
        }
        if (stream->epoll) {
                net_stream_epoll_deallocate (stream->epoll);
        }
        if (stream->stop.initialized) {
                thread_signal_destroy (&stream->stop);
        }
        memory_destroy (stream);
}

bool net_stream_add (NetStream *stream, NetStreamConnection *connection)
{
        if (!net_stream_epoll_monitor (stream->epoll, 
                                       connection->socket, 
                                       connection)) {
                error_code (FunctionCall, 1);
                return false;
        }
        return true;
}

void net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       NetStreamPackage package)
{
        ssize_t count;
        size_t position = 0;

        (void)stream;
        while (true) {
                count = write (connection->socket, 
                               package.buffer + position,
                               package.length - position);
                if (count == -1 && errno == EINTR) {
                        continue;
                }
                if (count == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        usleep (1000);
                        continue;
                }
                if (count == -1 || count == 0) {
                        return;
                }
                position += (size_t)count;
                if (position == package.length) {
                        return;
                }
        }
}

static void worker (Thread *thread)
{
        NetStream *stream = thread->argument;
        NetStreamConnection *connection;
        NetStreamEpollEvent event;
        int events;
        int i;
        
        while (net_stream_epoll_events_count (stream->epoll, &events)) {
                for (i = 0; i < events; i++) {
                        event = net_stream_epoll_event (stream->epoll, i);
                        if (event.stop) {
                                thread_signal (&stream->stop);
                                thread_exit (thread);
                        }
                        connection = event.pointer;
                        if (event.close) {
                                stream->on_close (stream, connection);
                                continue;
                        }
                        if (event.read) {
                                if (!connection_read (stream, connection)) {
                                        continue;
                                }
                        }
                }
        }
}

static bool connection_read (NetStream *stream, NetStreamConnection *connection) 
{
        NetStreamPackage package;
        char buffer[512];
        ssize_t count;

        while (true) {
                count = read (connection->socket, buffer, sizeof (buffer));
                if (count == -1 && errno == EINTR) {
                        continue;
                }
                if (count == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        return true;
                }
                if (count == -1 || count == 0) {
                        stream->on_close (stream, connection);
                        return false;
                }
                package.buffer = buffer;
                package.length = (size_t)count;
                stream->on_read (stream, connection, package);
        }
}
