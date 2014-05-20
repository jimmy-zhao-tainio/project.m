#ifndef test_stopwatch_h
#define test_stopwatch_h

#include <lib.test/test.h>

bool test_stopwatch_start_invalid_argument (Test *test);
bool test_stopwatch_start_invalid_operation (Test *test);
bool test_stopwatch_stop_invalid_argument (Test *test);
bool test_stopwatch_stop_invalid_operation_1 (Test *test);
bool test_stopwatch_stop_invalid_operation_2 (Test *test);
bool test_stopwatch_reset_invalid_argument (Test *test);
bool test_stopwatch_reset (Test *test);
bool test_stopwatch_elapsed_time_invalid_argument_1 (Test *test);
bool test_stopwatch_elapsed_time_invalid_argument_2 (Test *test);
bool test_stopwatch_elapsed_time_invalid_operation_1 (Test *test);
bool test_stopwatch_elapsed_time_invalid_operation_2 (Test *test);
bool test_stopwatch_elapsed_time_invalid_operation_3 (Test *test);
bool test_stopwatch_elapsed_time (Test *test);

#endif
