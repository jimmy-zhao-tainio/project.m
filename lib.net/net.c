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
#include <lib.net/private.h>

NetServer *net_server_create (NetAddress address,
                              NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error)
{
        NetPrivateServer *server;
        struct sockaddr_in c_address;
        int yes = 1;
        
        if (address.port == 0) {
                error_code (InvalidArgument, 5);
                return NULL;
        }
        if (!(server = memory_create (sizeof (NetPrivateServer)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        server->public.address = address;
        server->public.net.type = NET_TYPE_SERVER;
        server->public.net.on_connect = on_connect;
        server->public.net.on_close = on_close;
        server->public.net.on_read = on_read;
        server->public.net.on_error = on_error;
        server->socket = -1;
        if (!net_private_create (&server->net)) {
                net_destroy (&server->public.net);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if ((server->socket = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                net_destroy (&server->public.net);
                error_code (SystemCall, 1);
                return NULL;
        }
        if (setsockopt (server->socket, 
                        SOL_SOCKET, 
                        SO_REUSEADDR, 
                        &yes, 
                        sizeof (int)) == -1) {
                net_destroy (&server->public.net);
                error_code (SystemCall, 2);
                return NULL;
        }
        bzero (&c_address, sizeof (c_address));
        c_address.sin_family = AF_INET;
        c_address.sin_port = htons (server->public.address.port);
        if (inet_pton (AF_INET, server->public.address.ip, &c_address.sin_addr) != 1) {
                net_destroy (&server->public.net);
                error_code (SystemCall, 3);
                return NULL;
        }
        if (bind (server->socket, 
                  (struct sockaddr *)&c_address, 
                  sizeof (c_address)) == -1) {
                net_destroy (&server->public.net);
                error_code (SystemCall, 4);
                return NULL;
        }
        if (listen (server->socket, SOMAXCONN) == -1) {
                net_destroy (&server->public.net);
                error_code (SystemCall, 5);
                return NULL;
        }
        if (!epoll_monitor_server (server->net.epoll, server->socket, server)) {
                net_destroy (&server->public.net);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return &server->public;
}

NetClient *net_client_create (NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error)
{
        NetPrivateClient *client;

        if (!(client = memory_create (sizeof (NetPrivateClient)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        client->public.net.type = NET_TYPE_CLIENT;
        client->public.net.on_connect = on_connect;
        client->public.net.on_close = on_close;
        client->public.net.on_read = on_read;
        client->public.net.on_error = on_error;
        if (!net_private_create (&client->net)) {
                net_destroy (&client->public.net);
                error_code (FunctionCall, 2);
                return NULL;
        }
        return &client->public;
}

void net_destroy (Net *net)
{
        NetPrivateServer *server;
        NetPrivateClient *client;

        if (!net) {
                error (InvalidArgument);
                return;
        }
        if (net->type == NET_TYPE_SERVER) {
                server = PRIVATE (NetPrivateServer, ((NetServer *)net));
                if (server->net.epoll) {
                        (void)epoll_monitor_stop (server->net.epoll, server->socket);
                }
                if (server->socket != -1) {
                        close (server->socket);
                }
                net_private_destroy (&server->net);
                memory_destroy (server);
        }
        else {
                client = PRIVATE (NetPrivateClient, ((NetClient *)net));
                net_private_destroy (&client->net);
                memory_destroy (client);
        }
}

void net_close (Net *net, NetConnection connection)
{
        NetPrivate *private;

        if (!net) {
                error (InvalidArgument);
                return;
        }
        if (net->type == NET_TYPE_SERVER) {
                private = &PRIVATE (NetPrivateServer, ((NetServer *)net))->net;
        }
        else {
                private = &PRIVATE (NetPrivateClient, ((NetClient *)net))->net;
        }
        if (!thread_lock (&private->connections_lock)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (hash_find (private->connections, connection.object.id)) {
                // Signal implementation to cause closure.
        }
        if (!thread_unlock (&private->connections_lock)) {
                error_code (FunctionCall, 3);
        }
}

void net_write (Net *net, NetConnection connection, NetPackage package)
{
        (void)net;
        (void)connection;
        (void)package;
}

bool net_connect (Net *net, NetAddress address)
{
        (void)net; (void)address;
        return false;
}
