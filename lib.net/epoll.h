#ifndef lib_net_epoll_h
#define lib_net_epoll_h

#include <lib.core/defines.h>
#include <sys/epoll.h>

typedef struct _Epoll      Epoll;
typedef struct _EpollEvent EpollEvent;

struct _Epoll 
{
        struct epoll_event *events;
        int events_length;
        int file;
        int event_stop;
        struct epoll_event server_event;
};

struct _EpollEvent
{
        void *pointer;
        bool stop;
        bool server_error;
        bool server_accept;
        bool connection_read;
        bool connection_write;
        bool connection_close;
};

Epoll *epoll_allocate   (void);
void   epoll_deallocate (Epoll *epoll);

bool        epoll_events_count (Epoll *epoll, int *count);
EpollEvent  epoll_event        (Epoll *epoll, int index, bool print);

bool epoll_stop (Epoll *epoll);

bool epoll_monitor_server     (Epoll *epoll, int socket, void *pointer);
bool epoll_monitor_connection (Epoll *epoll, int socket, void *pointer);
bool epoll_monitor_stop       (Epoll *epoll, int socket);

#endif
