#ifndef test_error_h
#define test_error_h

#include <lib.test/test.h>

bool test_error (Test *test);
bool test_error_add_invalid_error (Test *test);
bool test_error_add (Test *test);
bool test_error_reset (Test *test);
bool test_error_count (Test *test);
bool test_error_at (Test *test);

#endif
