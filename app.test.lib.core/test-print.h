#ifndef test_print_h
#define test_print_h

#include <lib.test/test.h>

bool test_print (Test *test);
bool test_print_silent (Test *test);
bool test_print_log_begin_invalid_argument (Test *test);
bool test_print_log_begin_invalid_operation (Test *test);
bool test_print_log_begin_function_call (Test *test);
bool test_print_log_end_invalid_operation (Test *test);
bool test_print_log (Test *test);

#endif
