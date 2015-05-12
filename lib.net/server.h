#ifndef lib_net_server_h
#define lib_net_server_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>

typedef struct _Server Server;
typedef struct _Client Client;

typedef Client *(*EventOpen) (Server *server, int socket);
typedef void (*EventClose) (Server *server, Client *client);
typedef void (*EventRead) (Server *server, Client *client, char *buffer, size_t length);
typedef void (*EventStop) (Server *server);

struct _Server
{
        Thread *thread;
        uint16_t port;
        int socket;

        EventOpen on_open;
        EventClose on_close;
        EventRead on_read;
        EventStop on_stop;
        void *private;
};

struct _Client 
{
        int socket;
};

Server *server_create (uint16_t port, 
                       EventOpen on_open, 
                       EventClose on_close, 
                       EventRead on_read, 
                       EventStop on_stop);
void server_destroy (Server *server);

#endif
