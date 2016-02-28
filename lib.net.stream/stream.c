#include <lib.net.stream/stream.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <unistd.h>
#include <stdio.h>

static void poll_on_monitor (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             bool success);
static void poll_on_write   (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             unsigned char *buffer, 
                             size_t length);
static void poll_on_read    (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             unsigned char *buffer, 
                             size_t length);
static void poll_on_close   (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             bool success);
static void signal_close      (NetStreamConnection *connection);
static void signal_write      (NetStreamConnection *connection);
static bool signal_wait_write (NetStreamConnection *connection);

NetStream *net_stream_create (NetStreamOnAdd on_add,
                              NetStreamOnClose on_close, 
                              NetStreamOnRead on_read,
                              void *tag)
{
        NetStream *stream;

        if (!(stream = memory_create (sizeof (NetStream)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(stream->poll = net_poll_create (&poll_on_monitor, 
                                              &poll_on_close, 
                                              &poll_on_read, 
                                              &poll_on_write))) {
                memory_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        stream->on_add = on_add;
        stream->on_close = on_close;
        stream->on_read = on_read;
        stream->poll->pointer = stream;
        stream->tag = tag;
        return stream;
}

void net_stream_destroy (NetStream *stream)
{
        if (!stream) {
                error (InvalidArgument);
                return;
        }
        net_poll_destroy (stream->poll);
        memory_destroy (stream);
}

bool net_stream_add (NetStream *stream, int socket)
{
        NetStreamConnection *connection;

        if (!(connection = memory_create (sizeof (NetStreamConnection)))) {
                close (socket);
                error_code (FunctionCall, 1);
                return false;
        }
        connection->poll.socket = socket;
        connection->poll.pointer = connection;
        if (!thread_lock_create_full (&connection->lock, false)) {
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 2);
                return false;
        }
        if (!thread_signal_create (&connection->signal)) {
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 3);
                return false;
        }
        if (!net_poll_monitor (stream->poll, &connection->poll)) {
                thread_signal_destroy (&connection->signal);
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

bool net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       unsigned char *buffer, 
                       size_t length)
{
        if (!signal_wait_write (connection)) {
                return false;
        }
        if (!net_poll_write (stream->poll, &connection->poll, buffer, length)) {
                return false;
        }
        return true;
}

bool net_stream_close (NetStream *stream, NetStreamConnection *connection)
{
        if (!net_poll_close (stream->poll, &connection->poll)) {
                return false;
        }
        return true;
}

void net_stream_remove (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        thread_signal_destroy (&connection->signal);
        thread_lock_destroy (&connection->lock);
        memory_destroy (connection);
}

static void poll_on_monitor (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             bool success)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        if (!success) {
                close (poll_connection->socket);
                memory_destroy (connection);
                return;
        }
        stream->on_add (stream, connection);
}

static void poll_on_read (NetPoll *poll, 
                          NetPollConnection *poll_connection, 
                          unsigned char *buffer, 
                          size_t length)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        (void)stream;
        (void)buffer;
        (void)length;
        stream->on_read (stream, connection, buffer, length);
}

static void poll_on_write (NetPoll *poll, 
                           NetPollConnection *poll_connection, 
                           unsigned char *buffer, 
                           size_t length)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        (void)stream;
        (void)buffer;
        (void)length;
        signal_write (connection);
}

static void poll_on_close (NetPoll *poll, 
                           NetPollConnection *poll_connection,
                           bool success)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        (void)stream;
        (void)success;
        signal_close (connection);
        stream->on_close (stream, connection);
}

static void signal_close (NetStreamConnection *connection)
{
        thread_lock (&connection->lock);
        connection->closed = true;
        thread_signal (&connection->signal);
        thread_unlock (&connection->lock);
}

static void signal_write (NetStreamConnection *connection)
{
        thread_lock (&connection->lock);
        connection->writing = false;
        thread_signal (&connection->signal);
        thread_unlock (&connection->lock);
}

static bool signal_wait_write (NetStreamConnection *connection)
{
        bool closed = false;
        bool done = false;

        while (true) {
                thread_lock (&connection->lock);
                if (connection->closed) {
                        closed = true;
                }
                else if (connection->writing == false) {
                        connection->writing = true;
                        done = true;
                }
                thread_unlock (&connection->lock);
                if (closed) {
                        return false;
                }
                if (done) {
                        return true;
                }
                thread_signal_wait (&connection->signal);
        }
}
