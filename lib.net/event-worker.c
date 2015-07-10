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
#include <lib.net/epoll.h>
#include <lib.net/event-worker.h>
#include <lib.net/event-reader.h>
#include <lib.net/private.h>
#include <lib.net/epoll-events.h>

void event_worker (Thread *thread)
{
        Net *net_public = thread->argument;
        NetPrivate *net;
        EpollEvent event;

        if (net_public->type == NET_TYPE_SERVER) {
                net = &PRIVATE (NetPrivateServer, ((NetServer *)net_public))->net;
        }
        else {
                net = &PRIVATE (NetPrivateClient, ((NetClient *)net_public))->net;
        }
        while (true) {
                if (!thread_signal_wait (&net->reader_signal)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                event = net->event;
                if (!thread_signal (&net->worker_signal)) {
                        error_code (FunctionCall, 2);
                        return;
                }
                if (event.is_custom_event && event.custom_event.event_number == EPOLL_EVENT_STOP) {
                        return;
                }
                if (event.server_error) {
                        event_server_error (net);
                }
                if (event.server_accept) {
                        event_server_accept (net);
                }
                if (event.connection_read) {
                        event_connection_read (net, event.pointer);
                }
                if (event.connection_write) {
                        event_connection_write (net, event.pointer);
                }
                if (event.connection_close) {
                        event_connection_close (net, event.pointer);
                }
        }
}

void event_server_error (NetPrivate *net)
{
        (void)net;
}

void event_server_accept (NetPrivate *net)
{
        NetPrivateServer *server = (NetPrivateServer *)net;
        int client_socket;

        while (true) {
                if ((client_socket = accept (server->socket, NULL, NULL)) == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                return;
                        }
                        error_code (SystemCall, errno);
                        continue;
                }
                if (!connection_add (net, client_socket)) {
                        close (client_socket);
                        error (FunctionCall);
                        continue;
                }
        }
}

bool event_connection_open (NetPrivate *net, NetPrivateConnection *connection)
{
        (void)net;
        (void)connection;
        return false;
        /*
        if (getsockopt (event.connection->socket,
                        SOL_SOCKET,
                        SO_ERROR,
                        &result,
                        &result_length) != 0) {
                close (event.connection->socket);
                event.connection_close = true;
                // Call OnClose?
                // Remove from connections?
        }
        else {
                event.connection->initialized = true;
                event.connection_open = true;
                event.connection_read = true;
        }
        */
}

void event_connection_read (NetPrivate *net, NetPrivateConnection *connection)
{
        if (!connection->initialized) {
                if (!event_connection_open (net, connection)) {
                        return;
                }
        }
}

void event_connection_write (NetPrivate *net, NetPrivateConnection *connection)
{
        if (!connection->initialized) {
                if (!event_connection_open (net, connection)) {
                        return;
                }
        }
}

void event_connection_close (NetPrivate *net, NetPrivateConnection *connection)
{
        connection_remove (net, connection);
}

/*
static NetConnection *connection_create (int socket)
{
        NetPrivateConnection *connection;

        if (!(connection = memory_create (sizeof (NetPrivateConnection)))) {
                error (FunctionCall);
                return NULL;
        }
        connection->public.object.id = object_id ();
        connection->public.pointer = NULL;
        connection->socket = socket;
        connection->initialized = false;
        return &connection->public;
}

void connection_destroy (NetConnection *connection_public)
{
        NetPrivateConnection *connection;

        if (!connection_public) {
                error (InvalidArgument);
                return;
        }
        connection = PRIVATE (NetPrivateConnection, connection_public);
        memory_destroy (connection);
}*/

bool connection_add (NetPrivate *net, int socket)
{
        NetPrivateConnection *connection;
        bool success = true;

        if (!(connection = memory_create (sizeof (NetPrivateConnection)))) {
                return false;
        }
        connection->public.object.id = object_id ();
        connection->public.pointer = NULL;
        connection->socket = socket;
        connection->initialized = false;
        if (!thread_lock (&net->connections_lock)) {
                memory_destroy (connection);
                error_code (FunctionCall, 1);
                return false;
        }
        if (!hash_insert (net->connections, &connection->public.object)) {
                memory_destroy (connection);
                error_code (FunctionCall, 2);
                success = false;
        }
        if (!thread_unlock (&net->connections_lock)) {
                memory_destroy (connection);
                error_code (FunctionCall, 3);
                success = false;
        }
        return success;
}

void connection_remove (NetPrivate *net, NetPrivateConnection *connection)
{
        if (!thread_lock (&net->connections_lock)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (hash_find (net->connections, connection->public.object.id)) {
                hash_remove (net->connections, &connection->public.object);
                if (close (connection->socket) == -1) {
                        error_code (SystemCall, errno);
                }
                memory_destroy (connection);
        }
        else {
                error (InvalidOperation);
        }
        if (!thread_unlock (&net->connections_lock)) {
                error_code (FunctionCall, 2);
                return;
        }
}
