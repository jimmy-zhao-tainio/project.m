#ifndef lib_net_server_h
#define lib_net_server_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>
#include <lib.net.server/server-epoll.h>

typedef struct _NetServer NetServer;

typedef void (*NetServerOnConnect) (NetServer *server, int socket);
typedef void (*NetServerOnError)   (NetServer *server);

struct _NetServer
{
        const char *ip;
        uint16_t port;
        int socket;
        NetServerOnConnect on_connect;
        NetServerOnError on_error;
        NetServerEpoll *epoll;
        Thread *thread;
};

NetServer *net_server_create (const char *ip, 
                              uint16_t port,
                              NetServerOnConnect on_connect, 
                              NetServerOnError on_error);
void net_server_destroy (NetServer *server);

#endif
