#include <lib.net/net.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/tree.h>
#include <lib.core/threads.h>
#include <lib.core/hash.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <lib.net/epoll.h>
#include <lib.net/event-reader.h>

void event_reader (Thread *thread)
{
        Net *net_public = thread->argument;
        NetPrivate *net;
        EpollEvent event;
        int count;
        int i;

        if (net_public->type == NET_TYPE_SERVER) {
                net = &PRIVATE (NetPrivateServer, ((NetServer *)net_public))->net;
        }
        else {
                net = &PRIVATE (NetPrivateClient, ((NetClient *)net_public))->net;
        }
        if (!thread_signal_wait (&net->initialized_signal)) {
                error_code (FunctionCall, 1);
                return;
        }
        while (epoll_events_count (net->epoll, &count)) {
                for (i = 0; i < count; i++) {
                        event = epoll_event (net->epoll, i, true);
                        if (event.custom_event && event.custom_value == CUSTOM_EVENT_STOP) {
                                event_reader_stop (net, event);
                                return;
                        }
                        if (!event_reader_signal (net, event)) {
                                error_code (FunctionCall, 2);
                                break;
                        }
                }
        }
        event.custom_event = true;
        event.custom_value = CUSTOM_EVENT_STOP;
        event_reader_stop (net, event);
        event_reader_signal (net, event);
        error_code (FunctionCall, 3);
}

void event_reader_stop (NetPrivate *net, EpollEvent event)
{
        size_t i;

        for (i = 0; i < net->worker_count; i++) {
                if (!net->worker[i]) {
                        break;
                }
                event_reader_signal (net, event);
        }
}

bool event_reader_signal (NetPrivate *net, EpollEvent event)
{
        if (!thread_signal (&net->reader_signal)) {
                error_code (FunctionCall, 1);
                return false;
        }
        net->event = event;
        if (!thread_signal_wait (&net->worker_signal)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}
