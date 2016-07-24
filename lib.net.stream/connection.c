#include <lib.net.stream/connection.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <unistd.h>
#include <stdio.h>

NetStreamConnection *net_stream_connection_create (int socket)
{
        NetStreamConnection *connection;

        if (!(connection = memory_create (sizeof (NetStreamConnection)))) {
                close (socket);
                error_code (FunctionCall, 1);
                return NULL;
        }
        connection->poll.socket = socket;
        connection->poll.pointer = connection;
        if (!thread_lock_create_full (&connection->lock, false)) {
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!thread_signal_create (&connection->close_signal)) {
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!thread_signal_create (&connection->write_signal)) {
                thread_signal_destroy (&connection->close_signal);
                thread_lock_destroy (&connection->lock);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return connection;
}

void net_stream_connection_destroy (NetStreamConnection *connection)
{
        thread_signal_destroy (&connection->write_signal);
        thread_signal_destroy (&connection->close_signal);
        thread_lock_destroy (&connection->lock);
        memory_destroy (connection);
}
