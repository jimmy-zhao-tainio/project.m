#ifndef lib_net_private_structures_h
#define lib_net_private_structures_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.net/structures.h>

typedef struct _NetPrivate           NetPrivate;
typedef struct _NetPrivateServer     NetPrivateServer;
typedef struct _NetPrivateClient     NetPrivateClient;
typedef struct _NetPrivateConnection NetPrivateConnection;

struct _NetPrivate
{
        Hash *connections;
        ThreadLock connections_lock;
        Epoll *epoll;
        Thread *reader;
        ThreadSignal reader_signal;
        Thread **worker;
        ThreadSignal worker_signal;
        size_t worker_count;
        ThreadSignal initialized_signal;
        EpollEvent event;
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

#endif
