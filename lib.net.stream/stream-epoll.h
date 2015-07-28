#ifndef lib_net_stream_epoll_h
#define lib_net_stream_epoll_h

#include <lib.core/defines.h>
#include <sys/epoll.h>

typedef struct _NetStreamEpoll      NetStreamEpoll;
typedef struct _NetStreamEpollEvent NetStreamEpollEvent;

#define NET_STREAM_EPOLL_EVENTS_LENGTH 64

struct _NetStreamEpoll
{
        struct epoll_event events[NET_STREAM_EPOLL_EVENTS_LENGTH];
        int file;
        int stop_event;
};

struct _NetStreamEpollEvent
{
        void *pointer;
        bool read;
        bool write;
        bool close;
        bool stop;
};

NetStreamEpoll *net_stream_epoll_allocate   (void);
void            net_stream_epoll_deallocate (NetStreamEpoll *epoll);

bool                 net_stream_epoll_events_count (NetStreamEpoll *epoll, int *count);
NetStreamEpollEvent  net_stream_epoll_event        (NetStreamEpoll *epoll, int index);

bool net_stream_epoll_stop (NetStreamEpoll *epoll);

bool net_stream_epoll_monitor      (NetStreamEpoll *epoll, int socket, void *pointer, bool is_read, bool is_write);
bool net_stream_epoll_remonitor    (NetStreamEpoll *epoll, int socket, void *pointer, bool is_read, bool is_write);
bool net_stream_epoll_monitor_stop (NetStreamEpoll *epoll, int socket);

#endif
