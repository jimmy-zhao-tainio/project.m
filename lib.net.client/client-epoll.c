#include <lib.net.client/client-epoll.h>
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

static bool monitor_socket (NetClientEpoll *epoll, 
                            struct epoll_event *event,
                            uint32_t events, 
                            int socket, 
                            void *pointer);

NetClientEpoll *net_client_epoll_allocate (void)
{
        NetClientEpoll *epoll;
        struct epoll_event event = { 0 };
        
        if (!(epoll = memory_create (sizeof (NetClientEpoll)))) {
                error (FunctionCall);
                return NULL;
        }
        epoll->file = -1;
        if ((epoll->file = epoll_create1 (0)) == -1) {
                net_client_epoll_deallocate (epoll);
                error_code (SystemCall, 1);
                return NULL;
        }
        if ((epoll->stop_event = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 2);
                net_client_epoll_deallocate (epoll);
                return NULL;
        }
        event.data.ptr = &epoll->stop_event;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (epoll->file, 
                       EPOLL_CTL_ADD, 
                       epoll->stop_event, 
                       &event) == -1) {
                net_client_epoll_deallocate (epoll);
                error_code (SystemCall, 3);
                return NULL;
        }
        return epoll;
}

void net_client_epoll_deallocate (NetClientEpoll *epoll)
{
        if (!epoll) {
                error (InvalidArgument);
                return;
        }
        if (epoll->stop_event != -1) {
                close (epoll->stop_event);
        }
        if (epoll->file != -1) {
                close (epoll->file);
        }
        memory_destroy (epoll);
}

bool net_client_epoll_events_count (NetClientEpoll *epoll, int *count)
{
        while (true) {
                *count = epoll_wait (epoll->file,
                                     epoll->events,
                                     NET_CLIENT_EPOLL_EVENTS_LENGTH,
                                     -1);
                if (*count > 0) {
                        return true;
                }
                else if (*count < 0) {
                        if (errno == EINTR) {
                                error_code (SystemCall, errno);
                                continue;
                        }
                        error_code (SystemCall, errno);
                        return false;
                }
        }
}

NetClientEpollEvent net_client_epoll_event (NetClientEpoll *epoll, int index)
{
        NetClientEpollEvent event = { 0 };
        uint64_t not_used;

        event.pointer = epoll->events[index].data.ptr;
        if (event.pointer == &epoll->stop_event) {
                event.stop = true;
                if (eventfd_read (epoll->stop_event, &not_used) == -1) {
                        error (SystemCall);
                }
        }
        else {
                if (epoll->events[index].events & EPOLLERR ||
                    epoll->events[index].events & EPOLLHUP ||
                    epoll->events[index].events & EPOLLRDHUP) {
                        event.connect_error = true;
                }
                else {
                        event.connect = true;
                }
        }
        return event;
}

bool net_client_epoll_stop (NetClientEpoll *epoll)
{
        if (eventfd_write (epoll->stop_event, 1) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}

bool net_client_epoll_monitor (NetClientEpoll *epoll, int socket, void *pointer)
{
        struct epoll_event event = { 0 };

        return monitor_socket (epoll, 
                               &event, 
                               EPOLLOUT | EPOLLONESHOT, 
                               socket, 
                               pointer);
}

bool net_client_epoll_monitor_stop (NetClientEpoll *epoll, int socket)
{
        struct epoll_event event = { 0 };

        if (epoll_ctl (epoll->file,
                       EPOLL_CTL_DEL,
                       socket,
                       &event) == -1) {
                error (SystemCall);
                return false;
        }
        return true;
}

static bool monitor_socket (NetClientEpoll *epoll, struct epoll_event *event, uint32_t events, int socket, void *pointer)
{
        int flags;

        if ((flags = fcntl (socket, F_GETFL, 0)) == -1) {
                error_code (SystemCall, 1);
                return false;
        }
        if (fcntl (socket, F_SETFL, flags | O_NONBLOCK) == -1) {
                error_code (SystemCall, 2);
                return false;
        }
        event->data.ptr = pointer;
        event->events = events;
        if (epoll_ctl (epoll->file,
                       EPOLL_CTL_ADD,
                       socket,
                       event) == -1) {
                error_code (SystemCall, 3);
                return false;
        }
        return true;
}
