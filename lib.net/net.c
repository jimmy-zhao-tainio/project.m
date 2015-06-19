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

typedef struct _NetPrivate           NetPrivate;
typedef struct _NetPrivateServer     NetPrivateServer;
typedef struct _NetPrivateClient     NetPrivateClient;
typedef struct _NetPrivateConnection NetPrivateConnection;

ThreadSignal nil_signal;

struct _NetPrivate
{
        Hash *connections;
        Epoll *epoll;
        Thread      *reader;
        ThreadSignal reader_signal;
        Thread     **worker;
        ThreadSignal worker_signal;
        size_t       worker_count;
        ThreadSignal initialized_signal;
        EpollEvent   event;
};

struct _NetPrivateServer
{
        NetPrivate net;
        NetServer public;
        int socket;
};

struct _NetPrivateClient
{
        NetPrivate net;
        NetClient public;
};

struct _NetPrivateConnection
{
        NetConnection public;
        int socket;
        bool initialized;
};

#define PRIVATE(type, object) ((type *)(((char *)(object)) - offsetof (type, public)))

static bool net_private_create  (NetPrivate *net);
static void net_private_destroy (NetPrivate *net);

/*
static NetConnection *connection_create  (int socket);
static void           connection_destroy (NetConnection *connection);*/

static void event_reader        (Thread *thread);
static void event_worker        (Thread *thread);
static void event_reader_stop   (NetPrivate *net, EpollEvent event);
static bool event_reader_signal (NetPrivate *net, EpollEvent event);

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
        if (setsockopt (server->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
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
        if (bind (server->socket, (struct sockaddr *)&c_address, sizeof (c_address)) == -1) {
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

void net_close (Net *net, NetConnection *connection)
{
        (void)net; (void)connection;
}

void net_write (Net *net, NetConnection *connection, NetPackage package)
{
        (void)net; (void)connection; (void)package;
}

bool net_connect (Net *net, NetAddress address)
{
        (void)net; (void)address;
        return false;
}

static bool net_private_create (NetPrivate *net)
{
        size_t i;

        net->reader_signal.initialized = false;
        net->worker_signal.initialized = false;
        net->initialized_signal.initialized = false;
        if (!(net->connections = hash_create ())) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(net->epoll = epoll_allocate ())) {
                error_code (FunctionCall, 2);
                return false;
        }
        if (!thread_signal_create (&net->initialized_signal)) {
                error_code (FunctionCall, 3);
                return false;
        }
        if (!thread_signal_create (&net->reader_signal)) {
                error_code (FunctionCall, 4);
                return false;
        }
        if (!thread_signal_create (&net->worker_signal)) {
                error_code (FunctionCall, 5);
                return false;
        }
        if (!(net->reader = thread_create (&event_reader, net))) {
                error_code (FunctionCall, 6);
                return false;
        }
        net->worker_count = 8;
        if (!(net->worker = memory_create (sizeof (Thread) * net->worker_count))) {
                error_code (FunctionCall, 7);
                return false;
        }
        for (i = 0; i < net->worker_count; i++) {
                if (!(net->worker[i] = thread_create (&event_worker, net))) {
                        if (!thread_signal (&net->initialized_signal)) {
                                error_code (FunctionCall, 8);
                                return false;
                        }
                        error_code (FunctionCall, 9);
                        return false;
                }
        }
        if (!thread_signal (&net->initialized_signal)) {
                error_code (FunctionCall, 10);
                return false;
        }
        return true;
}

static void net_private_destroy (NetPrivate *net)
{
        size_t i;

        if (net->reader) {
                if (net->epoll) {
                        epoll_stop (net->epoll);
                }
                thread_wait (net->reader);
                thread_destroy (net->reader);
        }
        if (net->worker) {
                for (i = 0; i < net->worker_count; i++) {
                        if (!net->worker[i]) {
                                break;
                        }
                        thread_wait (net->worker[i]);
                        thread_destroy (net->worker[i]);
                }
                memory_destroy (net->worker);
        }
        if (net->reader_signal.initialized) {
                thread_signal_destroy (&net->reader_signal);
        }
        if (net->worker_signal.initialized) {
                thread_signal_destroy (&net->worker_signal);
        }
        if (net->initialized_signal.initialized) {
                thread_signal_destroy (&net->initialized_signal);
        }
        if (net->epoll) {
                epoll_deallocate (net->epoll);
        }
        if (net->connections) {
                hash_destroy (net->connections);
        }
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

static void connection_destroy (NetConnection *connection_public)
{
        NetPrivateConnection *connection;

        if (!connection_public) {
                error (InvalidArgument);
                return;
        }
        connection = PRIVATE (NetPrivateConnection, connection_public);
        memory_destroy (connection);
}*/

static void event_reader (Thread *thread)
{
        Net *net_public = thread->argument;
        NetPrivate *net;
        EpollEvent event;
        int count;
        int i;

        if (net_public->type == NET_TYPE_SERVER) {
                net = &PRIVATE (NetPrivateServer, ((NetServer *)net_public))->net;
        }
        else {
                net = &PRIVATE (NetPrivateClient, ((NetClient *)net_public))->net;
        }
        if (!thread_signal_wait (&net->initialized_signal)) {
                error_code (FunctionCall, 1);
                return;
        }
        while (epoll_events_count (net->epoll, &count)) {
                for (i = 0; i < count; i++) {
                        event = epoll_event (net->epoll, i, true);
                        if (event.stop == true) {
                                event_reader_stop (net, event);
                                return;
                        }
                        if (!event_reader_signal (net, event)) {
                                error_code (FunctionCall, 2);
                                break;
                        }
                }
        }
        event.stop = true;
        event_reader_stop (net, event);
        event_reader_signal (net, event);
        error_code (FunctionCall, 3);
}

static void event_worker (Thread *thread)
{
        Net *net_public = thread->argument;
        NetPrivate *net;

        if (net_public->type == NET_TYPE_SERVER) {
                net = &PRIVATE (NetPrivateServer, ((NetServer *)net_public))->net;
        }
        else {
                net = &PRIVATE (NetPrivateClient, ((NetClient *)net_public))->net;
        }
        while (true) {
                
        }
        (void)net;
        /*
        if (!event.connection->initialized) {
                if (getsockopt (event.connection->socket,
                                SOL_SOCKET,
                                SO_ERROR,
                                &result,
                                &result_length) != 0) {
                        close (event.connection->socket);
                        event.connection_close = true;
                }
                else {
                        event.connection->initialized = true;
                        event.connection_open = true;
                        event.connection_read = true;
                }
        }*/
}

static void event_reader_stop (NetPrivate *net, EpollEvent event)
{
        size_t i;

        for (i = 0; i < net->worker_count; i++) {
                if (!net->worker[i]) {
                        break;
                }
                event_reader_signal (net, event);
        }
}

static bool event_reader_signal (NetPrivate *net, EpollEvent event)
{

        (void)net;
        (void)event;
        return false;
}
