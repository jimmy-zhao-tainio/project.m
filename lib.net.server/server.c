#include <lib.net.server/server.h>
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
static void worker_accept (NetServer *server);

NetServer *net_server_create (const char *ip, 
                              uint16_t port,
                              NetServerOnConnect on_connect, 
                              NetServerOnError on_error,
                              void *tag)
{
        NetServer *server;
        struct sockaddr_in c_address;
        int yes = 1;
        
        if (ip == NULL) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (port == 0) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_connect) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!on_error) {
                error_code (InvalidArgument, 4);
                return NULL;
        }
        if (!(server = memory_create (sizeof (NetServer)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        server->ip = ip;
        server->port = port;
        server->on_connect = on_connect;
        server->on_error = on_error;
        server->tag = tag;
        server->socket = -1;
        if ((server->socket = socket (AF_INET, 
                                      SOCK_STREAM | SOCK_NONBLOCK, 
                                      0)) == -1) {
                net_server_destroy (server);
                error_code (SystemCall, 1);
                return NULL;
        }
        if (setsockopt (server->socket, 
                        SOL_SOCKET, 
                        SO_REUSEADDR, 
                        &yes, 
                        sizeof (int)) == -1) {
                net_server_destroy (server);
                error_code (SystemCall, 2);
                return NULL;
        }
        bzero (&c_address, sizeof (c_address));
        c_address.sin_family = AF_INET;
        c_address.sin_port = htons (port);
        if (inet_pton (AF_INET, ip, &c_address.sin_addr) != 1) {
                net_server_destroy (server);
                error_code (SystemCall, 3);
                return NULL;
        }
        if (bind (server->socket, 
                  (struct sockaddr *)&c_address, 
                  sizeof (c_address)) == -1) {
                net_server_destroy (server);
                error_code (SystemCall, 4);
                return NULL;
        }
        if (listen (server->socket, SOMAXCONN) == -1) {
                net_server_destroy (server);
                error_code (SystemCall, 5);
                return NULL;
        }
        if (!(server->epoll = net_server_epoll_allocate ())) {
                net_server_destroy (server);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!net_server_epoll_monitor (server->epoll, server->socket, NULL)) {
                close (server->socket);
                server->socket = -1;
                net_server_destroy (server);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(server->thread = thread_create (&worker, server))) {
                net_server_destroy (server);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return server;
}

void net_server_destroy (NetServer *server)
{
        if (!server) {
                error (InvalidArgument);
                return;
        }
        if (server->epoll) {
                if (!net_server_epoll_stop (server->epoll)) {
                        error (FunctionCall);
                        return;
                }
        }
        if (server->thread) {
                thread_wait (server->thread);
                thread_destroy (server->thread);
        }
        if (server->epoll) {
                if (server->socket != -1) {
                        net_server_epoll_monitor_stop (server->epoll, 
                                                       server->socket);
                }
                net_server_epoll_deallocate (server->epoll);
        }
        if (server->socket != -1) {
                close (server->socket);
        }
        memory_destroy (server);
}

static void worker (Thread *thread)
{
        NetServer *server = thread->argument;
        NetServerEpollEvent event;
        int count;
        int i;

        while (net_server_epoll_events_count (server->epoll, &count)) {
                for (i = 0; i < count; i++) {
                        event = net_server_epoll_event (server->epoll, i);
                        if (event.accept) {
                                worker_accept (server);
                        }
                        else if (event.stop) {
                                return;
                        }
                        else if (event.error) {
                                server->on_error (server);
                                return;
                        }
                }
        }
        server->on_error (server);
        return;
}

static void worker_accept (NetServer *server)
{
        int client_socket;
        int flags;

        while (true) {
                if ((client_socket = accept (server->socket, NULL, NULL)) != -1) {
                        if ((flags = fcntl (client_socket, F_GETFL, 0)) == -1) {
                                close (client_socket);
                                error_code (SystemCall, errno);
                                continue;
                        }
                        if ((flags = fcntl (client_socket, 
                                            F_SETFL, 
                                            flags | O_NONBLOCK)) == -1) {
                                close (client_socket);
                                error_code (SystemCall, errno);
                                continue;
                        }
                        server->on_connect (server, client_socket);
                        continue;
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        return;
                }
                error_code (SystemCall, errno);
        }
}
