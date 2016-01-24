#include <lib.net.poll/queue.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-queue.h"

bool test_poll_queue_create_function_call_1 (Test *test)
{
        NetPollQueue queue;

        TITLE ();
        memory_commit_limit (sizeof (size_t) + (1000 * sizeof (NetPollEvent)) - 1);
        CATCH (net_poll_queue_create (&queue, 1000));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_poll_queue_create (Test *test)
{
        NetPollQueue queue;

        TITLE ();
        CATCH (!net_poll_queue_create (&queue, 1000));
        CATCH (error_count () != 0);
        net_poll_queue_destroy (&queue);
        PASS ();
}

bool test_poll_queue_1 (Test *test)
{
        NetPollQueue queue;
        NetPollEvent event;
        NetPollConnection connection = { 0 };
        
        TITLE ();
        CATCH (!net_poll_queue_create (&queue, 3));
        CATCH (!net_poll_queue_push_monitor (&queue, &connection));
        CATCH (!net_poll_queue_push_close (&queue, &connection));
        CATCH (!net_poll_queue_push_write (&queue, &connection));
        event = net_poll_queue_pop (&queue);
        CATCH (event.type != NET_POLL_INTERNAL_MONITOR);
        event = net_poll_queue_pop (&queue);
        CATCH (event.type != NET_POLL_INTERNAL_CLOSE);
        event = net_poll_queue_pop (&queue);
        CATCH (event.type != NET_POLL_INTERNAL_WRITE);
        event = net_poll_queue_pop (&queue);
        CATCH (event.type != NET_POLL_EMPTY);
        net_poll_queue_destroy (&queue);
        PASS ();
}

static ThreadSignal signal_push = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal signal_pop = THREAD_SIGNAL_INITIALIZER;
static void worker_push (Thread *thread);
static void worker_pop (Thread *thread);
static NetPollQueue worker_queue;
static int pushpop = 100000;

bool test_poll_queue_2 (Test *test)
{
        Thread *thread_push;
        Thread *thread_pop;
        
        TITLE ();
        CATCH (!net_poll_queue_create (&worker_queue, 1024));
        CATCH (!(thread_push = thread_create (&worker_push, NULL)));
        CATCH (!(thread_pop = thread_create (&worker_pop, NULL)));
        thread_signal_wait (&signal_push);
        thread_signal_wait (&signal_pop);
        thread_destroy (thread_push);
        thread_destroy (thread_pop);
        net_poll_queue_destroy (&worker_queue);
        PASS ();
}

static void worker_push (Thread *thread)
{
        NetPollConnection connection = { 0 };
        int i;

        (void)thread;
        for (i = 0; i < pushpop; i++) {
                if (!net_poll_queue_push_close (&worker_queue, &connection)) {
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
        int popped = 0;

        (void)thread;
        while (popped != pushpop) {
                event = net_poll_queue_pop (&worker_queue);
                if (event.type == NET_POLL_EMPTY) {
                        continue;
                }
                popped++;
        }
        thread_signal (&signal_pop);
}
