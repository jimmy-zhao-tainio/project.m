#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-monitor-1.h"

static void on_monitor (NetPoll *poll, NetPollConnection *connection, bool success);
static ThreadSignal signal = THREAD_SIGNAL_INITIALIZER;
static bool monitor_success;

bool test_poll_monitor_1 (Test *test)
{
        NetPoll *poll;
        NetPollConnection connection = { .socket = -1 };

        TITLE ();
        monitor_success = false;
        CATCH (!(poll = net_poll_create (&on_monitor, NULL, NULL)));
        CATCH (!net_poll_monitor (poll, &connection));
        thread_signal_wait (&signal);
        CATCH (monitor_success);
        net_poll_destroy (poll);
        CATCH (error_count () == 0);
        PASS ();
}

static void on_monitor (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        (void)connection;
        monitor_success = success;
        thread_signal (&signal);
}
