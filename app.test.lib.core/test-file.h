#ifndef test_file_h
#define test_file_h

#include <lib.test/test.h>

bool test_file_path_size_1 (Test *test);
bool test_file_path_size_2 (Test *test);
bool test_file_directory_current_path_function_call (Test *test);
bool test_file_directory_current_path_system_call (Test *test);
bool test_file_directory_current_path (Test *test);
bool test_file_path_is_valid (Test *test);
bool test_file_name_from_path_invalid_operation (Test *test);
bool test_file_name_from_path (Test *test);
bool test_file_directory_sub_path_invalid_operation (Test *test);
bool test_file_directory_sub_path_function_call (Test *test);
bool test_file_directory_sub_path (Test *test);
bool test_file_directory_open_invalid_operation (Test *test);
bool test_file_directory_open_system_call (Test *test);
bool test_file_directory_open_function_call_1 (Test *test);
bool test_file_directory_open_function_call_2 (Test *test);
bool test_file_directory_open_function_call_3 (Test *test);
bool test_file_directory_open (Test *test);
bool test_file_directory_open_current_1 (Test *test);
bool test_file_directory_open_current_2 (Test *test);
bool test_file_directory_close_invalid_argument (Test *test);
bool test_file_directory_close (Test *test);
bool test_file_directory_read_invalid_argument (Test *test);
bool test_file_directory_read_1 (Test *test);
bool test_file_directory_read_2 (Test *test);
bool test_file_directory_read_3 (Test *test);
bool test_file_directory_read_4 (Test *test);
bool test_file_directory_read_5 (Test *test);
bool test_file_directory_read_6 (Test *test);
bool test_file_directory_find_invalid_argument (Test *test);
bool test_file_directory_find (Test *test);
bool test_file_directory_add_fake_file_invalid_argument (Test *test);
bool test_file_directory_add_fake_file_function_call_1 (Test *test);
bool test_file_directory_add_fake_file_function_call_2 (Test *test);
bool test_file_open_fail (Test *test);
bool test_file_open_fail_multiple (Test *test);
bool test_file_open (Test *test);
bool test_file_close (Test *test);
bool test_file_readline_f1 (Test *test);
bool test_file_readline_f2 (Test *test);
bool test_file_readline_f3 (Test *test);
bool test_file_remove (Test *test);
bool test_file_exists (Test *test);

#endif
