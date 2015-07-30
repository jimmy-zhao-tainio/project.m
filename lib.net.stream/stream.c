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

static bool inner_create (NetStream *stream,
                          ThreadSignal *stop_signal,
                          NetStreamEpoll **epoll,
                          Thread **thread,
                          bool is_read);
static void inner_destroy (NetStream *stream, 
                           ThreadSignal *stop_signal, 
                           NetStreamEpoll *epoll,
                           Thread *thread);
static void read_worker (Thread *thread);
static void write_worker (Thread *thread);
static bool connection_read (NetStream *stream, NetStreamConnection *connection);

NetStream *net_stream_create (NetStreamOnRead on_read, 
                              NetStreamOnClose on_close,
                              NetStreamOnError on_error)
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
        if (!inner_create (stream,
                           &stream->stop_read_signal,
                           &stream->epoll_read,
                           &stream->thread_read,
                           true)) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!inner_create (stream,
                           &stream->stop_write_signal,
                           &stream->epoll_write,
                           &stream->thread_write,
                           false)) {
                net_stream_destroy (stream);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return stream;
}

void net_stream_destroy (NetStream *stream)
{
        if (!stream) {
                error (InvalidArgument);
        }
        inner_destroy (stream, 
                       &stream->stop_read_signal, 
                       stream->epoll_read,
                       stream->thread_read);
        inner_destroy (stream, 
                       &stream->stop_write_signal, 
                       stream->epoll_write,
                       stream->thread_write);
        memory_destroy (stream);
}

static bool inner_create (NetStream *stream,
                          ThreadSignal *stop_signal,
                          NetStreamEpoll **epoll,
                          Thread **thread,
                          bool is_read)
{
        if (!thread_signal_create (stop_signal)) {
                return false;
        }
        if (!(*epoll = net_stream_epoll_allocate ())) {
                return false;
        }
        if (!(*thread = thread_create (is_read ? 
                                       &read_worker :
                                       &write_worker, 
                                       stream))) {
                return false;
        }
        return true;
}

static void inner_destroy (NetStream *stream, 
                           ThreadSignal *stop_signal, 
                           NetStreamEpoll *epoll,
                           Thread *thread)
{
        (void)stream;
        if (stop_signal->initialized && 
            epoll && 
            thread) {
                if (!net_stream_epoll_stop (epoll)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                if (!thread_signal_wait (stop_signal)) {
                        error_code (FunctionCall, 2);
                }
        }
        if (thread) {
                if (!thread_wait (thread)) {
                        error_code (FunctionCall, 3);
                }
                thread_destroy (thread);
        }
        if (epoll) {
                net_stream_epoll_deallocate (epoll);
        }
        if (stop_signal->initialized) {
                thread_signal_destroy (stop_signal);
        }
}

bool net_stream_add (NetStream *stream, NetStreamConnection *connection)
{
        if (!net_stream_epoll_monitor_read (stream->epoll_read, 
                                            connection->socket, 
                                            connection)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!net_stream_epoll_monitor_write (stream->epoll_write, 
                                             connection->socket, 
                                             connection)) {
                if (!net_stream_epoll_monitor_stop (stream->epoll_read, 
                                                    connection->socket)) {
                        error_code (FunctionCall, 2);
                }
                error_code (FunctionCall, 3);
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
                        if (!net_stream_epoll_remonitor_write (stream->epoll_write, 
                                                               connection->socket,
                                                               connection)) {
                                error_code (FunctionCall, 1);
                                return;
                        }
                        if (!thread_signal_wait (&connection->write_signal)) {
                                error_code (FunctionCall, 2);
                                return;
                        }
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

static void read_worker (Thread *thread)
{
        NetStream *stream = thread->argument;
        NetStreamConnection *connection;
        NetStreamEpollEvent event;
        int events;
        int i;
        
        while (net_stream_epoll_events_count (stream->epoll_read, &events)) {
                for (i = 0; i < events; i++) {
                        event = net_stream_epoll_event (stream->epoll_read, i);
                        if (event.stop) {
                                thread_signal (&stream->stop_read_signal);
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

static void write_worker (Thread *thread)
{
        NetStream *stream = thread->argument;
        NetStreamConnection *connection;
        NetStreamEpollEvent event;
        int events;
        int i;
        
        while (net_stream_epoll_events_count (stream->epoll_write, &events)) {
                for (i = 0; i < events; i++) {
                        event = net_stream_epoll_event (stream->epoll_write, i);
                        if (event.stop) {
                                thread_signal (&stream->stop_write_signal);
                                thread_exit (thread);
                        }
                        connection = event.pointer;
                        if (event.write) {
                                if (!thread_signal (&connection->write_signal)) {
                                        error (FunctionCall);
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
