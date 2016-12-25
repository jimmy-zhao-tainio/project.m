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
        if (!queue_create (&connection->tasks,
                           32,
                           sizeof (NetStreamTask), 
                           QUEUE_SIZE_DYNAMIC)) {
                memory_destroy (connection);
                error_code (FunctionCall, 2);
                return NULL;
        }

        connection->poll.socket = socket;
        connection->poll.tag = connection;
        if (!thread_lock_create_full (&connection->lock, false)) {
                queue_destroy (&connection->tasks);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!thread_signal_create (&connection->close_signal)) {
                thread_lock_destroy (&connection->lock);
                queue_destroy (&connection->tasks);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 4);
                return NULL;
        }
        if (!thread_signal_create (&connection->write_signal)) {
                thread_signal_destroy (&connection->close_signal);
                thread_lock_destroy (&connection->lock);
                queue_destroy (&connection->tasks);
                memory_destroy (connection);
                close (socket);
                error_code (FunctionCall, 5);
                return NULL;
        }
        return connection;
}

void net_stream_connection_destroy (NetStreamConnection *connection)
{
        thread_signal_destroy (&connection->write_signal);
        thread_signal_destroy (&connection->close_signal);
        thread_lock_destroy (&connection->lock);
        queue_destroy (&connection->tasks);
        memory_destroy (connection);
}
