#ifndef test_memory_h 
#define test_memory_h

#include <lib.test/test.h>

bool test_memory_create_invalid_argument (Test *test);
bool test_memory_create_overflow_1 (Test *test);
bool test_memory_create_overflow_2 (Test *test);
bool test_memory_create_overflow_3 (Test *test);
bool test_memory_create_commit_limit (Test *test);
bool test_memory_create_total_create_limit (Test *test);
bool test_memory_create_out_of_memory (Test *test);
bool test_memory_create_1 (Test *test);
bool test_memory_create_2 (Test *test);
bool test_memory_create_3 (Test *test);
bool test_memory_grow_invalid_argument_1 (Test *test);
bool test_memory_grow_invalid_argument_2 (Test *test);
bool test_memory_grow_invalid_operation (Test *test);
bool test_memory_grow_overflow_1 (Test *test);
bool test_memory_grow_overflow_2 (Test *test);
bool test_memory_grow_overflow_3 (Test *test);
bool test_memory_grow_commit_limit (Test *test);
bool test_memory_grow_total_create_limit (Test *test);
bool test_memory_grow_out_of_memory (Test *test);
bool test_memory_grow_1 (Test *test);
bool test_memory_grow_2 (Test *test);
bool test_memory_destroy_invalid_argument (Test *test);
bool test_memory_destroy (Test *test);
bool test_memory_total_create_size (Test *test);
bool test_memory_total_destroy_size (Test *test);
bool test_memory_size_invalid_argument (Test *test);
bool test_memory_size (Test *test);

#endif
