#ifndef test_random_h
#define test_random_h

#include <lib.test/test.h>

bool test_random_open_invalid_operation (Test *test);
bool test_random_bytes_invalid_operation (Test *test);
bool test_random_bytes_invalid_argument_1 (Test *test);
bool test_random_bytes_invalid_argument_2 (Test *test);
bool test_random_bytes (Test *test);
bool test_random_value (Test *test);
bool test_random_close_invalid_operation (Test *test);

#endif
