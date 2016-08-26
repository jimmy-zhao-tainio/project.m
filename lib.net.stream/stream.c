#include <lib.net.stream/stream.h>
#include <lib.net.stream/connection.h>
#include <lib.net.stream/connection-queue.h>
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
        if (!(stream->connection_queue = net_stream_connection_queue_create (stream))) {
                memory_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(stream->poll = net_poll_create (&poll_on_monitor,
                                              &poll_on_close,
                                              &poll_on_read,
                                              &poll_on_write))) {
                net_stream_connection_queue_destroy (stream->connection_queue);
                memory_destroy (stream);
                error_code (FunctionCall, 3);
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
        net_stream_connection_queue_destroy (stream->connection_queue);
        net_poll_destroy (stream->poll);
        memory_destroy (stream);
}

bool net_stream_add (NetStream *stream, int socket)
{
        NetStreamConnection *connection;

        if (!(connection = net_stream_connection_create (socket))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!net_poll_monitor (stream->poll, &connection->poll)) {
                net_stream_connection_destroy (connection);
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
        NetStreamTask task = {
                .type = NET_STREAM_TASK_WRITE,
                .value.write.buffer = buffer,
                .value.write.length = length
        };

        if (!net_stream_connection_queue_push (stream->connection_queue, connection, task)) {
                return false;
        }
        return true;
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
        net_stream_connection_destroy (connection);
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
