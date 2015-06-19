#ifndef test_threads_h
#define test_threads_h

#include <lib.test/test.h>

bool test_thread_create_invalid_argument (Test *test);
bool test_thread_create_function_call_1 (Test *test);
bool test_thread_create_1000 (Test *test);
bool test_thread_destroy_invalid_argument (Test *test);
bool test_thread_exit_invalid_argument (Test *test);
bool test_thread_wait_invalid_argument (Test *test);
bool test_thread_set_exit_invalid_argument (Test *test);
bool test_thread_get_exit_invalid_argument (Test *test);
bool test_thread_signal_create_invalid_argument (Test *test);
bool test_thread_signal_destroy_invalid_argument (Test *test);
bool test_thread_signal_invalid_argument (Test *test);
bool test_thread_signal_wait_invalid_argument (Test *test);
bool test_thread_lock_create_invalid_argument (Test *test);
bool test_thread_lock_destroy_invalid_argument (Test *test);
bool test_thread_lock_invalid_argument (Test *test);
bool test_thread_unlock_invalid_argument (Test *test);
bool test_thread_lock_recursive (Test *test);

#endif
