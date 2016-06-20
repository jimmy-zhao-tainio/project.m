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
        NetPollConnection connection = { 0 };

        TITLE ();
        CATCH (!(events = net_poll_events_create ()));
        CATCH (!net_poll_events_push_write (events, &connection));
        event = net_poll_events_next (events);
        CATCH (event.type != NET_POLL_INTERNAL_WRITE);
        CATCH (event.connection != &connection);
        net_poll_events_destroy (events);
        CATCH (error_count () != 0);
        PASS ();
}
