#ifndef lib_net_net_server_epoll_h
#define lib_net_net_server_epoll_h

#include <lib.core/defines.h>
#include <sys/epoll.h>

typedef struct _NetServerEpoll            NetServerEpoll;
typedef struct _NetServerEpollEvent       NetServerEpollEvent;

#define NET_SERVER_EPOLL_EVENTS_LENGTH 64

struct _NetServerEpoll
{
        struct epoll_event events[NET_SERVER_EPOLL_EVENTS_LENGTH];
        int file;
        int stop_event;
        struct epoll_event server_event;
};

struct _NetServerEpollEvent
{
        void *pointer;
        bool error;
        bool accept;
        bool stop;
};

NetServerEpoll *net_server_epoll_allocate   (void);
void            net_server_epoll_deallocate (NetServerEpoll *epoll);

bool                 net_server_epoll_events_count (NetServerEpoll *epoll, int *count);
NetServerEpollEvent  net_server_epoll_event        (NetServerEpoll *epoll, int index);

bool net_server_epoll_stop (NetServerEpoll *epoll);

bool net_server_epoll_monitor      (NetServerEpoll *epoll, int socket, void *pointer);
bool net_server_epoll_monitor_stop (NetServerEpoll *epoll, int socket);

#endif
