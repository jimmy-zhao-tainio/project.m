#include <lib.net.poll/events.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-events.h"

bool test_poll_events_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetPollEvents) - 1);
        CATCH (net_poll_events_create ());
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_poll_events_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetPollEvents) +
                             sizeof (size_t) + (sizeof (NetPollEvent) * 1024) - 
                             1);
        CATCH (net_poll_events_create ());
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_poll_events_create (Test *test)
{
        NetPollEvents *events;

        TITLE ();
        CATCH (!(events = net_poll_events_create ()));
        net_poll_events_destroy (events);
        CATCH (error_count () != 0);
        PASS ();
}

bool test_poll_events_internal_monitor (Test *test)
{
        NetPollEvents *events;
        NetPollEvent event;
        NetPollConnection connection = { 0 };

        TITLE ();
        CATCH (!(events = net_poll_events_create ()));
        CATCH (!net_poll_events_push_monitor (events, &connection));
        event = net_poll_events_next (events);
        CATCH (event.type != NET_POLL_INTERNAL_MONITOR);
        CATCH (event.connection != &connection);
        net_poll_events_destroy (events);
        CATCH (error_count () != 0);
        PASS ();
}

bool test_poll_events_internal_close (Test *test)
{
        NetPollEvents *events;
        NetPollEvent event;
        NetPollConnection connection = { 0 };

        TITLE ();
        CATCH (!(events = net_poll_events_create ()));
        CATCH (!net_poll_events_push_close (events, &connection));
        event = net_poll_events_next (events);
        CATCH (event.type != NET_POLL_INTERNAL_CLOSE);
        CATCH (event.connection != &connection);
        net_poll_events_destroy (events);
        CATCH (error_count () != 0);
        PASS ();
}

bool test_poll_events_internal_write (Test *test)
{
        NetPollEvents *events;
        NetPollEvent event;
        NetPollConnection connection = { .closed = false, .write.lock = THREAD_LOCK_INITIALIZER };

        TITLE ();
        CATCH (!(events = net_poll_events_create ()));
        CATCH (!net_poll_events_push_write (events, &connection));
        thread_unlock (&connection.write.lock);
        event = net_poll_events_next (events);
        CATCH (event.type != NET_POLL_INTERNAL_WRITE);
        CATCH (event.connection != &connection);
        net_poll_events_destroy (events);
        CATCH (error_count () != 0);
        PASS ();
}

static ThreadSignal signal_push = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal signal_pop = THREAD_SIGNAL_INITIALIZER;
static void worker_push (Thread *thread);
static void worker_pop (Thread *thread);
static NetPollEvents *worker_events;
static int pushpop = 1234;

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

int ipush;
int ipop;

static void worker_push (Thread *thread)
{
        NetPollConnection connections[pushpop];
        (void)thread;
        for (ipush = 0; ipush < pushpop; ipush++) {
                connections[ipush].closed = false;
                connections[ipush].socket = ipush;
                if (!net_poll_events_push_close (worker_events, &connections[ipush])) {
                        printf ("worker_push failed at %i\n", ipush); 
                        fflush (stdout); 
                        exit (-1);
                }
        }
        thread_signal (&signal_push);
}

static void worker_pop (Thread *thread)
{
        NetPollEvent event;

        (void)thread;
        for (ipop = 0; ipop < pushpop; ipop++) {
                event = net_poll_events_next (worker_events);
                if (event.type == NET_POLL_EMPTY) {
                        printf ("worker_pop failed at %i (empty)\n", ipop);
                        fflush (stdout);
                        exit (-1);
                }
                if (event.connection->socket != ipop) {
                        printf ("worker_pop failed at %i\n", ipop); 
                        fflush (stdout); 
                        exit (-1);
                }
        }
        thread_signal (&signal_pop);
}
