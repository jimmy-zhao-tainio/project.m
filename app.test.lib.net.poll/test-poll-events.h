#ifndef test_poll_events_h
#define test_poll_events_h

#include <lib.test/test.h>

bool test_poll_events_create_function_call_1 (Test *test);
bool test_poll_events_create_function_call_2 (Test *test);
bool test_poll_events_create (Test *test);
bool test_poll_events_internal_monitor (Test *test);
bool test_poll_events_internal_close (Test *test);
bool test_poll_events_internal_write (Test *test);

#endif
