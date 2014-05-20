#ifndef test_tree_h
#define test_tree_h

#include <lib.test/test.h>

bool test_tree_create_function_call_1 (Test *test);
bool test_tree_create_function_call_2 (Test *test);
bool test_tree_create (Test *test);
bool test_tree_node_insert_function_call_1 (Test *test);
bool test_tree_node_insert_function_call_2 (Test *test);
bool test_tree_node_insert_invalid_operation (Test *test);
bool test_tree_node_insert (Test *test);
bool test_tree_insert_invalid_argument_1 (Test *test);
bool test_tree_insert_invalid_argument_2 (Test *test);
bool test_tree_insert_overflow (Test *test);
bool test_tree_insert_function_call_1 (Test *test);
bool test_tree_insert_function_call_2 (Test *test);
bool test_tree_insert_invalid_operation (Test *test);
bool test_tree_insert_duplicate (Test *test);
bool test_tree_insert_1 (Test *test);
bool test_tree_insert_2 (Test *test);
bool test_tree_insert_3 (Test *test);
bool test_tree_insert_4 (Test *test);
bool test_tree_insert_5 (Test *test);
bool test_tree_insert_6 (Test *test);
bool test_tree_search_invalid_argument_1 (Test *test);
bool test_tree_search_invalid_argument_2 (Test *test);
bool test_tree_search_1 (Test *test);
bool test_tree_search_2 (Test *test);
bool test_tree_search_3 (Test *test);
bool test_tree_search_4 (Test *test);
bool test_tree_search_5 (Test *test);
bool test_tree_search_6 (Test *test);
bool test_tree_search_7 (Test *test);
bool test_tree_iterator_path_size_1 (Test *test);
bool test_tree_iterator_path_size_2 (Test *test);
bool test_tree_iterator_create_invalid_argument (Test *test);
bool test_tree_iterator_create_function_call_1 (Test *test);
bool test_tree_iterator_create_function_call_2 (Test *test);
bool test_tree_iterator_create (Test *test);
bool test_tree_iterator_next_invalid_argument (Test *test);
bool test_tree_iterator_next_overflow (Test *test);
bool test_tree_iterator_next_1 (Test *test);
bool test_tree_iterator_next_2 (Test *test);
bool test_tree_iterator_next_3 (Test *test);
bool test_tree_iterator_destroy_invalid_argument (Test *test);
bool test_tree_iterator_destroy (Test *test);
bool test_tree_count_invalid_argument (Test *test);
bool test_tree_count (Test *test);

#endif
