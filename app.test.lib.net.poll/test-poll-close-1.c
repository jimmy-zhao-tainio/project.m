#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-close-1.h"

static void on_close (NetPoll *poll, NetPollConnection *connection, bool success);
static ThreadSignal signal = THREAD_SIGNAL_INITIALIZER;
static bool close_success;

static NetPollConnection connection = { .closed = false, .socket = -1, .pointer = NULL };

bool test_poll_close_1 (Test *test)
{
        NetPoll *poll;

        TITLE ();
        close_success = false;
        CATCH (!(poll = net_poll_create (NULL, &on_close, NULL, NULL)));
        CATCH (!net_poll_close (poll, &connection));
        thread_signal_wait (&signal);
        CATCH (close_success);
        net_poll_destroy (poll);
        CATCH (error_count () == 0);
        PASS ();
}

static void on_close (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        (void)connection;
        close_success = success;
        thread_signal (&signal);
}
