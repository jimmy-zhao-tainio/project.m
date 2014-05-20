#ifndef test_tainio_tree_h
#define test_tainio_tree_h

#include <lib.test/test.h>

bool test_tainio_tree_create_invalid_argument_1 (Test *test);
bool test_tainio_tree_create_invalid_argument_2 (Test *test);
bool test_tainio_tree_create_function_call_1 (Test *test);
bool test_tainio_tree_create_function_call_2 (Test *test);
bool test_tainio_tree_create (Test *test);
bool test_tainio_tree_destroy_invalid_argument (Test *test);
bool test_tainio_tree_next_invalid_argument_1 (Test *test);
bool test_tainio_tree_next_invalid_argument_2 (Test *test);
bool test_tainio_tree_next_invalid_operation (Test *test);
bool test_tainio_tree_next_overflow (Test *test);
bool test_tainio_tree_next_function_call_1 (Test *test);
bool test_tainio_tree_next_function_call_2 (Test *test);
bool test_tainio_tree_next_function_call_3 (Test *test);
bool test_tainio_tree_next_function_call_1_2 (Test *test);
bool test_tainio_tree_next (Test *test);

#endif
