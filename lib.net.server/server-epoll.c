#include <lib.net.server/server-epoll.h>
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

static bool monitor_socket (NetServerEpoll *epoll, 
                            struct epoll_event *event,
                            uint32_t events, 
                            int socket, 
                            void *pointer);

NetServerEpoll *net_server_epoll_allocate (void)
{
        NetServerEpoll *epoll;
        struct epoll_event event = { 0 };
        
        if (!(epoll = memory_create (sizeof (NetServerEpoll)))) {
                error (FunctionCall);
                return NULL;
        }
        epoll->file = -1;
        epoll->server_event.data.ptr = NULL;
        if ((epoll->file = epoll_create1 (0)) == -1) {
                net_server_epoll_deallocate (epoll);
                error_code (SystemCall, 1);
                return NULL;
        }
        if ((epoll->stop_event = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 2);
                net_server_epoll_deallocate (epoll);
                return NULL;
        }
        event.data.ptr = &epoll->stop_event;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (epoll->file, 
                       EPOLL_CTL_ADD, 
                       epoll->stop_event, 
                       &event) == -1) {
                net_server_epoll_deallocate (epoll);
                error_code (SystemCall, 3);
                return NULL;
        }
        return epoll;
}

void net_server_epoll_deallocate (NetServerEpoll *epoll)
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

bool net_server_epoll_events_count (NetServerEpoll *epoll, int *count)
{
        while (true) {
                *count = epoll_wait (epoll->file,
                                     epoll->events,
                                     NET_SERVER_EPOLL_EVENTS_LENGTH,
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

NetServerEpollEvent net_server_epoll_event (NetServerEpoll *epoll, int index)
{
        NetServerEpollEvent event = { 0 };
        uint64_t not_used;

        event.pointer = epoll->events[index].data.ptr;
        if (event.pointer == &epoll->stop_event) {
                event.stop = true;
                if (eventfd_read (epoll->stop_event, &not_used) == -1) {
                        error (SystemCall);
                }
        }
        else if (event.pointer == epoll->server_event.data.ptr) {
                if (epoll->events[index].events & EPOLLERR ||
                    epoll->events[index].events & EPOLLHUP ||
                    epoll->events[index].events & EPOLLRDHUP ||
                    epoll->events[index].events & EPOLLOUT) {
                        event.error = true;
                }
                else if (epoll->events[index].events & EPOLLIN ||
                         epoll->events[index].events & EPOLLPRI) {
                        event.accept = true;
                }
        }
        return event;
}

bool net_server_epoll_stop (NetServerEpoll *epoll)
{
        if (eventfd_write (epoll->stop_event, 1) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}

bool net_server_epoll_monitor (NetServerEpoll *epoll, int socket, void *pointer)
{
        return monitor_socket (epoll, 
                               &epoll->server_event, 
                               EPOLLIN | EPOLLET, 
                               socket, 
                               pointer);
}

bool net_server_epoll_monitor_stop (NetServerEpoll *epoll, int socket)
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

static bool monitor_socket (NetServerEpoll *epoll, 
                            struct epoll_event *event, 
                            uint32_t events, 
                            int socket, 
                            void *pointer)
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
