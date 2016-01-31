#include <lib.net.stream/stream.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <unistd.h>

static void poll_on_monitor (NetPoll *poll, NetPollConnection *poll_connection, bool success);
static void poll_on_write   (NetPoll *poll, NetPollConnection *poll_connection, unsigned char *buffer, size_t length);
static void poll_on_close   (NetPoll *poll, NetPollConnection *poll_connection, bool success);

NetStream *net_stream_create (NetStreamOnAdd on_add,
                              NetStreamOnClose on_close, 
                              NetStreamOnRead on_read)
{
        NetStream *stream;

        if (!(stream = memory_create (sizeof (NetStream)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(stream->poll = net_poll_create (&poll_on_monitor, 
                                              &poll_on_close, 
                                              NULL, 
                                              &poll_on_write))) {
                memory_destroy (stream);
                error_code (FunctionCall, 2);
                return NULL;
        }
        stream->on_add = on_add;
        stream->on_close = on_close;
        stream->on_read = on_read;
        stream->poll->pointer = stream;
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
        if (!net_poll_monitor (stream->poll, &connection->poll)) {
                close (socket);
                memory_destroy (connection);
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}

bool net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       unsigned char *buffer, 
                       size_t length)
{
        if (connection->is_writing) {
                
        }
        connection->is_writing = true;
        if (!net_poll_write (stream->poll, &connection->poll, buffer, length)) {
                connection->is_writing = false;
                return false;
        }
        return true;
}

static void poll_on_monitor (NetPoll *poll, NetPollConnection *poll_connection, bool success)
{
        NetStream *stream;
        NetStreamConnection *connection;

        stream = poll->pointer;
        connection = poll_connection->pointer;

        if (!success) {
                close (poll_connection->socket);
                memory_destroy (connection);
                return;
        }
        stream->on_add (stream, connection);
}

static void poll_on_write (NetPoll *poll, NetPollConnection *poll_connection, unsigned char *buffer, size_t length)
{
        NetStream *stream;
        NetStreamConnection *connection;

        stream = poll->pointer;
        connection = poll_connection->pointer;
 
        (void)buffer;
        (void)length;
        (void)stream;
        (void)connection;
}

static void poll_on_close (NetPoll *poll, NetPollConnection *poll_connection, bool success)
{
        NetStream *stream;
        NetStreamConnection *connection;

        stream = poll->pointer;
        connection = poll_connection->pointer;
 
        (void)stream;
        (void)connection;
        (void)success;
}
