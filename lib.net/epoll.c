#include <lib.net/epoll.h>
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

static bool monitor_socket (Epoll *epoll, struct epoll_event *event, uint32_t events, int socket, void *pointer);

Epoll *epoll_allocate (void)
{
        Epoll *epoll;
        int events_length = 64;
        size_t events_bytes = (size_t)((size_t)events_length * (size_t)sizeof (struct epoll_event));
        struct epoll_event event = { 0 };
        
        if (!(epoll = memory_create (sizeof (Epoll)))) {
                error (FunctionCall);
                return NULL;
        }
        epoll->custom_event = -1;
        epoll->file = -1;
        epoll->events = NULL;
        epoll->server_event.data.ptr = NULL;
        if ((epoll->file = epoll_create1 (0)) == -1) {
                epoll_deallocate (epoll);
                error_code (SystemCall, 1);
                return NULL;
        }
        epoll->events_length = events_length;
        if (!(epoll->events = memory_create (events_bytes))) {
                epoll_deallocate (epoll);
                error_code (SystemCall, 2);
                return NULL;
        }
        if ((epoll->custom_event = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 3);
                epoll_deallocate (epoll);
                return NULL;
        }
        event.data.ptr = &epoll->custom_event;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (epoll->file, 
                       EPOLL_CTL_ADD, 
                       epoll->custom_event, &event) == -1) {
                epoll_deallocate (epoll);
                error_code (SystemCall, 4);
                return NULL;
        }
        return epoll;
}

void epoll_deallocate (Epoll *epoll)
{
        if (!epoll) {
                error (InvalidArgument);
                return;
        }
        if (epoll->custom_event != -1) {
                close (epoll->custom_event);
        }
        if (epoll->file != -1) {
                close (epoll->file);
        }
        if (epoll->events) {
                memory_destroy (epoll->events);
        }
        memory_destroy (epoll);
}

bool epoll_events_count (Epoll *epoll, int *count)
{
        while (true) {
                *count = epoll_wait (epoll->file,
                                     epoll->events,
                                     epoll->events_length,
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

static void print_events (uint32_t events);

EpollEvent epoll_event (Epoll *epoll, int index, bool print)
{
        EpollEvent event = { 0 };

        event.pointer = epoll->events[index].data.ptr;
        if (print) {
                print_events (epoll->events[index].events);
        }
        if (event.pointer == &epoll->custom_event) {
                // Read from epoll->custom_event.
                event.custom_event = true;
                if (eventfd_read (epoll->custom_event, &event.custom_value) == -1) {
                        event.custom_value = (uint64_t)-1;
                }
        }
        else if (event.pointer == epoll->server_event.data.ptr) {
                if (epoll->events[index].events & EPOLLERR ||
                    epoll->events[index].events & EPOLLHUP ||
                    epoll->events[index].events & EPOLLRDHUP ||
                    epoll->events[index].events & EPOLLOUT) {
                        event.server_error = true;
                }
                else if (epoll->events[index].events & EPOLLIN ||
                         epoll->events[index].events & EPOLLPRI) {
                        event.server_accept = true;
                }
        }
        else {
                if (epoll->events[index].events & EPOLLERR ||
                    epoll->events[index].events & EPOLLHUP ||
                    epoll->events[index].events & EPOLLRDHUP) {
                        event.connection_close = true;
                }
                else {
                        if (epoll->events[index].events & EPOLLIN ||
                            epoll->events[index].events & EPOLLPRI) {
                                event.connection_read = true;
                        }
                        if (epoll->events[index].events & EPOLLOUT) {
                                event.connection_write = true;
                        }
                }
        }
        return event;
}

static void print_events (uint32_t events)
{
        printf ("events:");
        if (events & EPOLLERR) {
                printf (" EPOLLERR");
        }
        if (events & EPOLLHUP) {
                printf (" EPOLLHUP");
        }
        if (events & EPOLLRDHUP) {
                printf (" EPOLLRDHUP");
        }
        if (events & EPOLLIN) {
                printf (" EPOLLIN");
        }
        if (events & EPOLLPRI) {
                printf (" EPOLLPRI");
        }
        if (events & EPOLLOUT) {
                printf (" EPOLLOUT");
        }
        printf ("\n"); fflush (stdout);
}

bool epoll_custom_event (Epoll *epoll, uint64_t custom_value)
{
        if (eventfd_write (epoll->custom_event, custom_value) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}

bool epoll_monitor_server (Epoll *epoll, int socket, void *pointer)
{
        return monitor_socket (epoll, 
                               &epoll->server_event, 
                               EPOLLIN | EPOLLET, 
                               socket, 
                               pointer);
}

bool epoll_monitor_connection (Epoll *epoll, int socket, void *pointer)
{
        struct epoll_event event;
        
        return monitor_socket (epoll, 
                               &event, 
                               EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET | EPOLLONESHOT, 
                               socket, 
                               pointer);
}

bool epoll_monitor_stop (Epoll *epoll, int socket)
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

static bool monitor_socket (Epoll *epoll, struct epoll_event *event, uint32_t events, int socket, void *pointer)
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
