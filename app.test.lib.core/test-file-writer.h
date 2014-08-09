#ifndef test_file_writer_h
#define test_file_writer_h

#include <lib.test/test.h>

bool test_file_writer_create_invalid_argument_1 (Test *test);
bool test_file_writer_create_invalid_argument_2 (Test *test);
bool test_file_writer_create_function_call_1 (Test *test);
bool test_file_writer_create_function_call_2 (Test *test);
bool test_file_writer_create_system_call (Test *test);
bool test_file_writer_create_append (Test *test);
bool test_file_writer_create_truncate (Test *test);
bool test_file_writer_write_invalid_argument_1 (Test *test);
bool test_file_writer_write_invalid_argument_2 (Test *test);
bool test_file_writer_write_invalid_argument_3 (Test *test);
bool test_file_writer_write_system_call (Test *test);
bool test_file_writer_write_args_invalid_argument_1 (Test *test);
bool test_file_writer_write_args_invalid_argument_2 (Test *test);
bool test_file_writer_write_args_system_call (Test *test);
bool test_file_writer_write_args (Test *test);

#endif
