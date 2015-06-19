#ifndef lib_net_h
#define lib_net_h

#include <lib.core/defines.h>
#include <lib.core/object.h>

typedef struct _Net           Net;
typedef struct _NetServer     NetServer;
typedef struct _NetClient     NetClient;
typedef struct _NetConnection NetConnection;
typedef struct _NetAddress    NetAddress;
typedef struct _NetPackage    NetPackage;

typedef void (*NetOnConnect) (Net *net, NetConnection *connection);
typedef void (*NetOnClose)   (Net *net, NetConnection *connection);
typedef void (*NetOnRead)    (Net *net, NetConnection *connection, NetPackage package);
typedef void (*NetOnError)   (Net *net);

typedef enum {
        NET_TYPE_SERVER,
        NET_TYPE_CLIENT
} NetType;

struct _Net
{
        NetType type;
        NetOnConnect on_connect;
        NetOnClose on_close;
        NetOnRead on_read;
        NetOnError on_error;
};

struct _NetConnection
{
        Object object;
        void *pointer;
};

struct _NetAddress
{
        const char *ip;
        uint16_t port;
};

struct _NetPackage
{
        char *buffer;
        size_t length;
};

struct _NetServer
{
        Net net;
        NetAddress address;
};

struct _NetClient
{
        Net net;
};

NetServer *net_server_create (NetAddress address,
                              NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error);
NetClient *net_client_create (NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error);
void net_destroy (Net *net);
void net_close   (Net *net, NetConnection *connection);
void net_write   (Net *net, NetConnection *connection, NetPackage package);
bool net_connect (Net *net, NetAddress address);

#endif
