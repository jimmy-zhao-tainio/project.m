#ifndef lib_net_client_epoll_h
#define lib_net_client_epoll_h

#include <lib.core/defines.h>
#include <sys/epoll.h>

typedef struct _NetClientEpoll      NetClientEpoll;
typedef struct _NetClientEpollEvent NetClientEpollEvent;

#define NET_CLIENT_EPOLL_EVENTS_LENGTH 64

struct _NetClientEpoll
{
        struct epoll_event events[NET_CLIENT_EPOLL_EVENTS_LENGTH];
        int file;
        int stop_event;
};

struct _NetClientEpollEvent
{
        void *pointer;
        bool connect;
        bool connect_error;
        bool stop;
};

NetClientEpoll *net_client_epoll_allocate   (void);
void            net_client_epoll_deallocate (NetClientEpoll *epoll);

bool                 net_client_epoll_events_count (NetClientEpoll *epoll, int *count);
NetClientEpollEvent  net_client_epoll_event        (NetClientEpoll *epoll, int index);

bool net_client_epoll_stop (NetClientEpoll *epoll);

bool net_client_epoll_monitor      (NetClientEpoll *epoll, int socket, void *pointer);
bool net_client_epoll_monitor_stop (NetClientEpoll *epoll, int socket);

#endif
