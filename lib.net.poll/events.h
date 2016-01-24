#ifndef lib_net_poll_events_h
#define lib_net_poll_events_h

#include <lib.core/defines.h>
#include <lib.net.poll/queue.h>
#include <lib.net.poll/event.h>
#include <lib.net.poll/connection.h>
#include <sys/epoll.h>

typedef struct _NetPollEvents NetPollEvents;

#define NET_POLL_EPOLL_EVENTS 64

struct _NetPollEvents
{
        struct epoll_event events[NET_POLL_EPOLL_EVENTS];
        int event_index;
        int event_count;
        int file;
        int internal_event;
        NetPollQueue queue;
        bool queue_got_events;
};

NetPollEvents *net_poll_events_create (void);
void     net_poll_events_destroy      (NetPollEvents *events);
bool     net_poll_events_push_monitor (NetPollEvents *events, NetPollConnection *connection);
bool     net_poll_events_push_close   (NetPollEvents *events, NetPollConnection *connection);
bool     net_poll_events_push_write   (NetPollEvents *events, NetPollConnection *connection);
bool     net_poll_events_push_exit    (NetPollEvents *events);

NetPollEvent net_poll_events_next (NetPollEvents *events);

#endif
