#ifndef lib_net_client_h
#define lib_net_client_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>
#include <lib.net.client/client-epoll.h>

typedef struct _NetClient           NetClient;
typedef struct _NetClientConnection NetClientConnection;

typedef void (*NetClientOnConnect)      (NetClient *client, NetClientConnection *connection);
typedef void (*NetClientOnConnectError) (NetClient *client, NetClientConnection *connection);
typedef void (*NetClientOnError)        (NetClient *client);

struct _NetClient
{
        NetClientOnConnect on_connect;
        NetClientOnConnectError on_connect_error;
        NetClientOnError on_error;
        NetClientEpoll *epoll;
        Thread *thread;
        void *tag;
};

struct _NetClientConnection
{
        const char *ip;
        uint16_t port;
        int socket;
        void *pointer;
};

NetClient *net_client_create (NetClientOnConnect on_connect,
                              NetClientOnConnectError on_connect_error,
                              NetClientOnError on_error,
                              void *tag);
void net_client_connect (NetClient *client, NetClientConnection *connection);
void net_client_destroy (NetClient *client);

#endif
