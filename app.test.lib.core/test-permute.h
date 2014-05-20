#ifndef test_permute_h
#define test_permute_h

#include <lib.test/test.h>

bool test_permute_create_invalid_argument (Test *test);
bool test_permute_create_overflow_1 (Test *test);
bool test_permute_create_overflow_2 (Test *test);
bool test_permute_create_function_call_1 (Test *test);
bool test_permute_create_function_call_2 (Test *test);
bool test_permute_create (Test *test);
bool test_permute_destroy_invalid_argument (Test *test);
bool test_permute_destroy (Test *test);
bool test_permute_next_invalid_argument (Test *test);
bool test_permute_next_1 (Test *test);
bool test_permute_next_2 (Test *test);
bool test_permute_next_3 (Test *test);
bool test_permute_next_4 (Test *test);

#endif
