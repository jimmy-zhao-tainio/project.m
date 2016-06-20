#include <lib.net.poll/events.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-internal-loop.h"

#define PUSH_POP_COUNT 1234

static ThreadSignal signal_push = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal signal_pop = THREAD_SIGNAL_INITIALIZER;
static void worker_push (Thread *thread);
static void worker_pop (Thread *thread);
static NetPollEvents *worker_events;
static NetPollConnection connections[PUSH_POP_COUNT];

bool test_poll_events_internal_loop (Test *test)
{
        Thread *thread_push;
        Thread *thread_pop;

        TITLE ();
        CATCH (!(worker_events = net_poll_events_create ()));
        CATCH (!(thread_push = thread_create (&worker_push, NULL)));
        CATCH (!(thread_pop = thread_create (&worker_pop, NULL)));
        thread_signal_wait (&signal_push);
        thread_signal_wait (&signal_pop);
        thread_wait (thread_push);
        thread_wait (thread_pop);
        thread_destroy (thread_push);
        thread_destroy (thread_pop);
        net_poll_events_destroy (worker_events);
        CATCH (error_count () != 0);
        PASS ();
}

static void worker_push (Thread *thread)
{
        int i;
        (void)thread;

        for (i = 0; i < PUSH_POP_COUNT; i++) {
                connections[i].closed = false;
                connections[i].socket = i;
                if (!net_poll_events_push_close (worker_events, &connections[i])) {
                        printf ("worker_push failed at %i\n", i);
                        fflush (stdout);
                        exit (-1);
                }
        }
        thread_signal (&signal_push);
}

static void worker_pop (Thread *thread)
{
        NetPollEvent event;
        int i;
        (void)thread;

        for (i = 0; i < PUSH_POP_COUNT; i++) {
                event = net_poll_events_next (worker_events);
                if (event.type == NET_POLL_EMPTY) {
                        printf ("worker_pop failed at %i (empty)\n", i);
                        fflush (stdout);
                        exit (-1);
                }
                if (event.connection->socket != i) {
                        printf ("worker_pop failed at %i\n", i); 
                        fflush (stdout); 
                        exit (-1);
                }
        }
        thread_signal (&signal_pop);
}
