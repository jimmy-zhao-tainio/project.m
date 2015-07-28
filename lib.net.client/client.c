#include <lib.net.client/client.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/tree.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

static void worker (Thread *thread);
static void worker_connect (NetClient *client, NetClientConnection *connection);
static void worker_connect_error (NetClient *client, NetClientConnection *connection);

NetClient *net_client_create (NetClientOnConnect on_connect, 
                              NetClientOnConnectError on_connect_error, 
                              NetClientOnError on_error)
{
        NetClient *client;
        if (!on_connect) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (!on_connect_error) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_error) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!(client = memory_create (sizeof (NetClient)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        client->on_connect = on_connect;
        client->on_connect_error = on_connect_error;
        client->on_error = on_error;
        if (!(client->epoll = net_client_epoll_allocate ())) {
                net_client_destroy (client);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(client->thread = thread_create (&worker, client))) {
                net_client_destroy (client);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return client;
}

void net_client_connect (NetClient *client, NetClientConnection *connection)
{
        struct sockaddr_in address;
        int result;
        int i;

        if (!client) {
                error_code (InvalidArgument, 1);
                return;
        }
        if (!connection) {
                error_code (InvalidArgument, 2);
                return;
        }
        if (inet_pton (AF_INET, connection->ip, &address.sin_addr) != 1) {
                error_code (SystemCall, 1);
                worker_connect_error (client, connection);
                return;
        }
        address.sin_family = AF_INET;
        address.sin_port = htons (connection->port);
        if ((connection->socket = socket (AF_INET, 
                                          SOCK_STREAM | SOCK_NONBLOCK, 
                                          0)) == -1) {
                error_code (SystemCall, 2);
                worker_connect_error (client, connection);
                return;
        }
        for (i = 0; i < 3; i++) {
                result = connect (connection->socket, 
                                  (struct sockaddr *)&address, 
                                  sizeof (struct sockaddr));
                if (result == 0) {
                        worker_connect (client, connection);
                        return;
                }
                if (result == -1 && (errno == EINPROGRESS || errno == EWOULDBLOCK)) {
                        /* Obscure epoll bug can cause EPOLLOUT+EPOLLHUP if epoll_ctl is
                         * done before a connect, so we do it after connect.
                         * To reproduce, just move this line before the for loop and run
                         * the test with 1-10 iterations.
                         */
                        if (!net_client_epoll_monitor (client->epoll, 
                                                       connection->socket, 
                                                       connection)) {
                                error_code (FunctionCall, 1);
                                worker_connect_error (client, connection);
                                return;
                        }
                        return;
                }
                if (result == -1 && errno == EINTR) {
                        continue;
                }
                break;
        }
        error_code (SystemCall, 2);
        worker_connect_error (client, connection);
        return;
}

void net_client_destroy (NetClient *client)
{
        if (!client) {
                error (InvalidArgument);
                return;
        }
        if (client->epoll) {
                if (!net_client_epoll_stop (client->epoll)) {
                        error (FunctionCall);
                        return;
                }
        }
        if (client->thread) {
                thread_wait (client->thread);
                thread_destroy (client->thread);
        }
        if (client->epoll) {
                net_client_epoll_deallocate (client->epoll);
        }
        memory_destroy (client);
}

static void worker (Thread *thread)
{
        NetClient *client = thread->argument;
        NetClientEpollEvent event;
        int count;
        int i;

        while (net_client_epoll_events_count (client->epoll, &count)) {
                for (i = 0; i < count; i++) {
                        event = net_client_epoll_event (client->epoll, i);
                        if (event.stop) {
                                thread_exit (thread);
                        }
                        else if (event.connect) {
                                worker_connect (client, event.pointer);
                        }
                        else {
                                worker_connect_error (client, event.pointer);
                        }
                }
        }
        client->on_error (client);
        thread_exit (thread);
}

static void worker_connect (NetClient *client, NetClientConnection *connection)
{
        int result;
        socklen_t result_length = sizeof (result);

        if (getsockopt (connection->socket,
                        SOL_SOCKET,
                        SO_ERROR,
                        &result,
                        &result_length) != 0) {
                worker_connect_error (client, connection);
        }
        else {
                client->on_connect (client, connection);
        }
}

static void worker_connect_error (NetClient *client, NetClientConnection *connection)
{
        close (connection->socket);
        client->on_connect_error (client, connection);
}
