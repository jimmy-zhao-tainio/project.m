#ifndef test_pattern_path_h
#define test_pattern_path_h

#include <lib.test/test.h>

bool test_pattern_path_create_invalid_argument (Test *test);
bool test_pattern_path_create_function_call (Test *test);
bool test_pattern_path_create (Test *test);
bool test_pattern_path_create_branch_function_call_1 (Test *test);
bool test_pattern_path_create_branch_function_call_2 (Test *test);
bool test_pattern_path_create_branch (Test *test);
bool test_pattern_path_create_not_function_call (Test *test);
bool test_pattern_path_create_not_range (Test *test);
bool test_pattern_path_create_not_set (Test *test);
bool test_pattern_path_create_not_value (Test *test);
bool test_pattern_path_create_or_function_call_1 (Test *test);
bool test_pattern_path_create_or_function_call_2 (Test *test);
bool test_pattern_path_create_or_function_call_3 (Test *test);
bool test_pattern_path_create_or (Test *test);
bool test_pattern_path_create_repeat_function_call_1 (Test *test);
bool test_pattern_path_create_repeat_function_call_2 (Test *test);
bool test_pattern_path_create_repeat (Test *test);
bool test_pattern_path_create_range_function_call (Test *test);
bool test_pattern_path_create_range (Test *test);
bool test_pattern_path_create_set_function_call (Test *test);
bool test_pattern_path_create_set (Test *test);
bool test_pattern_path_create_value_function_call (Test *test);
bool test_pattern_path_create_value (Test *test);
bool test_pattern_path_destroy (Test *test);

#endif
