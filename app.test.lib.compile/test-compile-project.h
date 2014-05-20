#ifndef test_compile_project_h
#define test_compile_project_h

#include <lib.test/test.h>

bool test_compile_project_create_null (Test *test);
bool test_compile_project_create_invalid_path (Test *test);
bool test_compile_project_create_not_project (Test *test);
bool test_compile_project_create (Test *test);
bool test_compile_project_prepare_null (Test *test);
bool test_compile_project_prepare_empty (Test *test);
bool test_compile_project_prepare_lib (Test *test);
bool test_compile_project_prepare_lib_and_app (Test *test);
bool test_compile_project_prepare_libs_and_app (Test *test);
bool test_compile_project_prepare_libs_and_app_circle (Test *test);
bool test_compile_project_prepare_app_indirect_lib (Test *test);
bool test_compile_project_prepare_app_invalid_lib_include (Test *test);
bool test_compile_project_destroy_null (Test *test);
bool test_compile_project_execute_null (Test *test);
bool test_compile_project_execute_lib (Test *test);
bool test_compile_project_execute_app_twice (Test *test);
bool test_compile_project_execute_lib_twice (Test *test);
bool test_compile_project_execute_lib_and_app_twice (Test *test);
bool test_compile_project_execute_lib_and_lib_twice (Test *test);
bool test_compile_project_execute_2libs_and_1app (Test *test);
bool test_compile_project_execute_3libs_and_2app (Test *test);
bool test_compile_project_execute_with_directory_name_null (Test *test);
bool test_compile_project_execute_with_directory_name_not_found (Test *test);
bool test_compile_project_execute_with_directory_name_lib (Test *test);

#endif
