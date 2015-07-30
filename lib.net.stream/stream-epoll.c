#include <lib.net.stream/stream-epoll.h>
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

NetStreamEpoll *net_stream_epoll_allocate (void)
{
        NetStreamEpoll *epoll;
        struct epoll_event event = { 0 };
        
        if (!(epoll = memory_create (sizeof (NetStreamEpoll)))) {
                error (FunctionCall);
                return NULL;
        }
        epoll->file = -1;
        if ((epoll->file = epoll_create1 (0)) == -1) {
                net_stream_epoll_deallocate (epoll);
                error_code (SystemCall, 1);
                return NULL;
        }
        if ((epoll->stop_event = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 2);
                net_stream_epoll_deallocate (epoll);
                return NULL;
        }
        event.data.ptr = &epoll->stop_event;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (epoll->file, 
                       EPOLL_CTL_ADD, 
                       epoll->stop_event, 
                       &event) == -1) {
                net_stream_epoll_deallocate (epoll);
                error_code (SystemCall, 3);
                return NULL;
        }
        return epoll;
}

void net_stream_epoll_deallocate (NetStreamEpoll *epoll)
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

bool net_stream_epoll_events_count (NetStreamEpoll *epoll, int *count)
{
        while (true) {
                *count = epoll_wait (epoll->file,
                                     epoll->events,
                                     NET_STREAM_EPOLL_EVENTS_LENGTH,
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

NetStreamEpollEvent net_stream_epoll_event (NetStreamEpoll *epoll, int index)
{
        NetStreamEpollEvent event = { 0 };
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
                        event.close = true;
                }
                if (epoll->events[index].events & EPOLLIN) {
                        event.read = true;
                }
                if (epoll->events[index].events & EPOLLOUT) {
                        event.write = true;
                }
        }
        return event;
}

bool net_stream_epoll_stop (NetStreamEpoll *epoll)
{
        if (eventfd_write (epoll->stop_event, 1) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}

bool net_stream_epoll_monitor_read (NetStreamEpoll *epoll, int socket, void *pointer)
{
        struct epoll_event event = { 0 };

        event.data.ptr = pointer;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (epoll->file,
                       EPOLL_CTL_ADD,
                       socket,
                       &event) == -1) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool net_stream_epoll_monitor_write (NetStreamEpoll *epoll, int socket, void *pointer)
{
        struct epoll_event event = { 0 };

        event.data.ptr = pointer;
        event.events = EPOLLET | EPOLLONESHOT;
        if (epoll_ctl (epoll->file,
                       EPOLL_CTL_ADD,
                       socket,
                       &event) == -1) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool net_stream_epoll_remonitor_write (NetStreamEpoll *epoll, int socket, void *pointer)
{
        struct epoll_event event = { 0 };

        event.data.ptr = pointer;
        event.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
        if (epoll_ctl (epoll->file,
                       EPOLL_CTL_MOD,
                       socket,
                       &event) == -1) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool net_stream_epoll_monitor_stop (NetStreamEpoll *epoll, int socket)
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
