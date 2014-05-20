#ifndef test_compile_h
#define test_compile_h

#include <lib.test/test.h>

bool test_compile_create_null (Test *test);
bool test_compile_create_not_app_or_lib (Test *test);
bool test_compile_create_app_empty (Test *test);
bool test_compile_create_lib_empty (Test *test);
bool test_compile_create_no_och_files (Test *test);
bool test_compile_create_h_files (Test *test);
bool test_compile_create_c_files (Test *test);
bool test_compile_create_co_files (Test *test);
bool test_compile_create_o_files (Test *test);
bool test_compile_destroy_null (Test *test);
bool test_compile_prepare_null (Test *test);
bool test_compile_execute_null (Test *test);

#endif
