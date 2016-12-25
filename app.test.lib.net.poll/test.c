#include <stdio.h>
#include <lib.test/test.h>

#include "test-poll-events.h"
#include "test-poll.h"
#include "test-poll-internal-loop.h"
#include "test-poll-monitor-1.h"
#include "test-poll-monitor-2.h"
#include "test-poll-close-1.h"
#include "test-poll-close-2.h"
#include "test-poll-on-close.h"
#include "test-poll-on-read-1.h"
#include "test-poll-on-read-2.h"
#include "test-poll-write-1.h"
#include "test-poll-write-2.h"
#include "test-poll-write-3.h"

bool (*tests[]) (Test *test) =
{
        &test_poll_events_create_function_call_1,
        &test_poll_events_create_function_call_2,
        &test_poll_events_create,
        &test_poll_events_internal_monitor,
        &test_poll_events_internal_close,
        &test_poll_events_internal_write,
        &test_poll_events_internal_loop,
        &test_poll_create_function_call_1,
        &test_poll_create_function_call_2,
        &test_poll_create_function_call_3,
        &test_poll_create,
        &test_poll_monitor_1,
        &test_poll_monitor_2,
        &test_poll_close_1,
        &test_poll_close_2,
        &test_poll_on_close,
        &test_poll_on_read_1,
        &test_poll_on_read_2,
        &test_poll_write_1,
        &test_poll_write_2,
        &test_poll_write_3,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
