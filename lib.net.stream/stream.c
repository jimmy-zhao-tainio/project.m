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
                             size_t length,
                             bool success);
static void poll_on_read    (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             unsigned char *buffer, 
                             size_t length);
static void poll_on_close   (NetPoll *poll, 
                             NetPollConnection *poll_connection, 
                             bool success);

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
        if (!thread_signal_create (&connection->close_signal)) {
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 3);
                return false;
        }
        if (!thread_signal_create (&connection->write_signal)) {
                thread_signal_destroy (&connection->close_signal);
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 4);
                return false;
        }
        if (!net_poll_monitor (stream->poll, &connection->poll)) {
                thread_signal_destroy (&connection->write_signal);
                thread_signal_destroy (&connection->close_signal);
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 5);
                return false;
        }
        return true;
}

bool net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       unsigned char *buffer, 
                       size_t length)
{
        if (!net_poll_write (stream->poll, &connection->poll, buffer, length)) {
                return false;
        }
        thread_signal_wait (&connection->write_signal);
        return connection->write_success;
}

bool net_stream_write_flags (NetStream *stream, 
                             NetStreamConnection *connection, 
                             unsigned char *buffer, 
                             size_t length,
                             NetPollFlag flags)
{
        if (!net_poll_write_flags (stream->poll, 
                                   &connection->poll, 
                                   buffer,
                                   length,
                                   flags)) {
                return false;
        }
        thread_signal_wait (&connection->write_signal);
        return connection->write_success;
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
        thread_signal_destroy (&connection->write_signal);
        thread_signal_destroy (&connection->close_signal);
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
                           size_t length,
                           bool success)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        (void)stream;
        (void)buffer;
        (void)length;
        (void)success;
 
        connection->write_success = success;       
        thread_signal (&connection->write_signal);
}

static void poll_on_close (NetPoll *poll, 
                           NetPollConnection *poll_connection,
                           bool success)
{
        NetStream *stream = poll->pointer;
        NetStreamConnection *connection = poll_connection->pointer;

        (void)stream;
        (void)success;
        thread_signal (&connection->close_signal);
        stream->on_close (stream, connection);
}
