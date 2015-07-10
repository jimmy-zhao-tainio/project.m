#ifndef lib_net_epoll_h
#define lib_net_epoll_h

#include <lib.core/defines.h>
#include <sys/epoll.h>

typedef struct _Epoll            Epoll;
typedef struct _EpollEvent       EpollEvent;
typedef struct _EpollCustomEvent EpollCustomEvent;

struct _EpollCustomEvent
{
        uint64_t event_number;
        union {
                void *pointer;
                uint64_t value;
        } argument;
};

struct _Epoll
{
        struct epoll_event *events;
        int events_length;
        int file;
        int custom_event;
        struct epoll_event server_event;
        EpollCustomEvent *custom_events;
        size_t custom_events_size;
        size_t custom_events_count;
};

struct _EpollEvent
{
        void *pointer;
        bool server_error;
        bool server_accept;
        bool connection_read;
        bool connection_write;
        bool connection_close;
        bool     is_custom_event;
        EpollCustomEvent custom_event;
};

Epoll *epoll_allocate   (void);
void   epoll_deallocate (Epoll *epoll);

bool        epoll_events_count (Epoll *epoll, int *count);
EpollEvent  epoll_event        (Epoll *epoll, int index);

bool epoll_custom_event (Epoll *epoll, EpollCustomEvent event);

bool epoll_monitor_server     (Epoll *epoll, int socket, void *pointer);
bool epoll_monitor_connection (Epoll *epoll, int socket, void *pointer);
bool epoll_monitor_stop       (Epoll *epoll, int socket);

#endif
