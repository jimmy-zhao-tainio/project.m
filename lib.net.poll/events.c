#include <lib.net.poll/events.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/list.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define QUEUE_SIZE 1024

static bool read_events (NetPollEvents *events);
static NetPollEvent next_event (NetPollEvents *events);
static bool push_queue_event (NetPollEvents *events);

NetPollEvents *net_poll_events_create (void)
{
        NetPollEvents *events;
        struct epoll_event event = { 0 };
        
        if (!(events = memory_create (sizeof (NetPollEvents)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        events->file = -1;
        events->internal_event = -1;
        events->event_index = 0;
        events->event_count = 0;
        events->queue_got_events = false;
        if (!net_poll_queue_create (&events->queue, QUEUE_SIZE)) {
                memory_destroy (events);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if ((events->file = epoll_create1 (0)) == -1) {
                net_poll_events_destroy (events);
                error_code (SystemCall, 1);
                return NULL;
        }
        if ((events->internal_event = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 2);
                net_poll_events_destroy (events);
                return NULL;
        }
        event.data.ptr = &events->internal_event;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (events->file, 
                       EPOLL_CTL_ADD, 
                       events->internal_event, 
                       &event) == -1) {
                net_poll_events_destroy (events);
                error_code (SystemCall, 3);
                return NULL;
        }
        return events;
}

void net_poll_events_destroy (NetPollEvents *events)
{
        if (!events) {
                error (InvalidArgument);
                return;
        }
        net_poll_queue_destroy (&events->queue);
        if (events->internal_event != -1) {
                close (events->internal_event);
        }
        if (events->file != -1) {
                close (events->file);
        }
        memory_destroy (events);
}

bool net_poll_events_push_monitor (NetPollEvents *events, 
                                   NetPollConnection *connection)
{
        if (!net_poll_queue_push_monitor (&events->queue, connection)) {
                return false;
        }
        return push_queue_event (events);
}

bool net_poll_events_push_close (NetPollEvents *events, 
                                 NetPollConnection *connection)
{
        if (!net_poll_queue_push_close (&events->queue, connection)) {
                return false;
        }
        return push_queue_event (events);
}

bool net_poll_events_push_write (NetPollEvents *events, 
                                 NetPollConnection *connection)
{
        if (!net_poll_queue_push_write (&events->queue, connection)) {
                return false;
        }
        return push_queue_event (events);
}

bool net_poll_events_push_exit (NetPollEvents *events)
{
        NetPollEvent event = {
                .type = NET_POLL_EXIT
        };

        if (!net_poll_queue_push (&events->queue, event)) {
                return false;
        }
        return push_queue_event (events);
}

static bool push_queue_event (NetPollEvents *events)
{
        if (eventfd_write (events->internal_event, 1) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}

NetPollEvent net_poll_events_next (NetPollEvents *events)
{
        NetPollEvent event_exit = {
                .type = NET_POLL_EXIT
        };
        NetPollEvent event = {
                .type = NET_POLL_EMPTY
        };

        while (event.type == NET_POLL_EMPTY) {
                if (events->event_index == events->event_count) {
                        if (!read_events (events)) {
                                error_code (FunctionCall, 1);
                                return event_exit;
                        }
                }
                event = next_event (events);
        }
        return event;
}

static bool read_events (NetPollEvents *events)
{
        // Loop here to retry when interrupted by EINTR.
        while (true) {
                events->event_count = epoll_wait (events->file,
                                                  events->events,
                                                  NET_POLL_EPOLL_EVENTS,
                                                  -1);
                if (events->event_count < 0) {
                        if (errno == EINTR) {
                                error_code (SystemCall, errno);
                                continue;
                        }
                        return false;
                }
                if (events->event_count == 0) {
                        // The man page only mentions a return value of 0 for timeouts.
                        // We're not using timeouts so treat this as an error of some sort.
                        error_code (SystemCall, 1);
                        return false;
                }
                events->event_index = 0;
                return true;
        }
}

static NetPollEvent next_event (NetPollEvents *events)
{
        NetPollEvent event;
        uint64_t not_used;
        int index;
        
        index = events->event_index;
        if (events->events[index].data.ptr == &events->internal_event) {
                if (events->queue_got_events == false) {
                        if (eventfd_read (events->internal_event, &not_used) == -1) {
                                error (SystemCall);
                        }
                }
                event = net_poll_queue_pop (&events->queue);
                if (event.type == NET_POLL_EMPTY) {
                        events->queue_got_events = false;
                        events->event_index++;
                }
                else {
                        events->queue_got_events = true;
                }
                return event;
        }
        else {
                if (events->events[index].events & EPOLLERR ||
                    events->events[index].events & EPOLLHUP ||
                    events->events[index].events & EPOLLRDHUP) {
                        event.connection = events->events[index].data.ptr;
                        event.type = NET_POLL_CLOSE;
                }
                else if (events->events[index].events & EPOLLIN &&
                         events->events[index].events & EPOLLOUT) {
                        event.connection = events->events[index].data.ptr;
                        event.type = NET_POLL_READ | NET_POLL_WRITE;
                }
                else if (events->events[index].events & EPOLLIN) {
                        event.connection = events->events[index].data.ptr;
                        event.type = NET_POLL_READ;
                }
                else if (events->events[index].events & EPOLLOUT) {
                        event.connection = events->events[index].data.ptr;
                        event.type = NET_POLL_WRITE;
                }
                else {
                        event.type = NET_POLL_EMPTY;
                }
                events->event_index++;
                return event;
        }
}
