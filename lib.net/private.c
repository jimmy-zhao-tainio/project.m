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
#include <lib.net/private.h>
#include <lib.net/event-reader.h>
#include <lib.net/event-worker.h>
#include <lib.net/epoll-events.h>

bool net_private_create (NetPrivate *net)
{
        size_t i;

        net->reader_signal.initialized = false;
        net->worker_signal.initialized = false;
        net->initialized_signal.initialized = false;
        if (!(net->connections = hash_create ())) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(thread_lock_create (&net->connections_lock))) {
                error_code (FunctionCall, 2);
                return false;
        }
        if (!(net->epoll = epoll_allocate ())) {
                error_code (FunctionCall, 3);
                return false;
        }
        if (!thread_signal_create (&net->initialized_signal)) {
                error_code (FunctionCall, 4);
                return false;
        }
        if (!thread_signal_create (&net->reader_signal)) {
                error_code (FunctionCall, 5);
                return false;
        }
        if (!thread_signal_create (&net->worker_signal)) {
                error_code (FunctionCall, 6);
                return false;
        }
        if (!(net->reader = thread_create (&event_reader, net))) {
                error_code (FunctionCall, 7);
                return false;
        }
        net->worker_count = 8;
        if (!(net->worker = memory_create (sizeof (Thread) * net->worker_count))) {
                error_code (FunctionCall, 8);
                return false;
        }
        for (i = 0; i < net->worker_count; i++) {
                if (!(net->worker[i] = thread_create (&event_worker, net))) {
                        if (!thread_signal (&net->initialized_signal)) {
                                error_code (FunctionCall, 9);
                                return false;
                        }
                        error_code (FunctionCall, 10);
                        return false;
                }
        }
        if (!thread_signal (&net->initialized_signal)) {
                error_code (FunctionCall, 11);
                return false;
        }
        return true;
}

void net_private_destroy (NetPrivate *net)
{
        EpollCustomEvent event;
        size_t i;

        if (net->reader) {
                if (net->epoll) {
                        event.event_number = EPOLL_EVENT_STOP;
                        event.argument.pointer = NULL;
                        if (!epoll_custom_event (net->epoll, event)) {
                                error (FunctionCall);
                        }
                }
                thread_wait (net->reader);
                thread_destroy (net->reader);
        }
        if (net->worker) {
                for (i = 0; i < net->worker_count; i++) {
                        if (!net->worker[i]) {
                                break;
                        }
                        thread_wait (net->worker[i]);
                        thread_destroy (net->worker[i]);
                }
                memory_destroy (net->worker);
        }
        if (net->reader_signal.initialized) {
                thread_signal_destroy (&net->reader_signal);
        }
        if (net->worker_signal.initialized) {
                thread_signal_destroy (&net->worker_signal);
        }
        if (net->initialized_signal.initialized) {
                thread_signal_destroy (&net->initialized_signal);
        }
        if (net->epoll) {
                epoll_deallocate (net->epoll);
        }
        if (net->connections_lock.initialized) {
                thread_lock_destroy (&net->connections_lock);
        }
        if (net->connections) {
                hash_destroy (net->connections);
        }
}
