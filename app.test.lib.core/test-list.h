#ifndef test_list_h
#define test_list_h

#include <lib.test/test.h>

bool test_list_create (Test *test);
bool test_list_destroy_invalid_argument (Test *test);
bool test_list_destroy (Test *test);
bool test_list_insert_invalid_argument (Test *test);
bool test_list_insert_overflow (Test *test);
bool test_list_insert_function_call (Test *test);
bool test_list_insert_1 (Test *test);
bool test_list_insert_2 (Test *test);
bool test_list_insert_3 (Test *test);
bool test_list_append_invalid_argument (Test *test);
bool test_list_append_invalid_argument (Test *test);
bool test_list_append_overflow (Test *test);
bool test_list_append_function_call (Test *test);
bool test_list_append_1 (Test *test);
bool test_list_append_2 (Test *test);
bool test_list_append_3 (Test *test);
bool test_list_position_invalid_argument_1 (Test *test);
bool test_list_position_invalid_argument_2 (Test *test);
bool test_list_position_1 (Test *test);
bool test_list_position_2 (Test *test);
bool test_list_remove_invalid_argument (Test *test);
bool test_list_remove (Test *test);
bool test_list_first (Test *test);
bool test_list_last (Test *test);
bool test_list_count_invalid_argument (Test *test);
bool test_list_count (Test *test);
bool test_list_find_invalid_argument (Test *test);
bool test_list_find (Test *test);

#endif
