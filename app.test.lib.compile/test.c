#include <stdio.h>

#include <lib.test/test.h>

#include "test-compile.h"
#include "test-compile-project.h"

bool (*tests[]) (Test *test) =
{
	&test_compile_create_null,
	&test_compile_create_not_app_or_lib,
	&test_compile_create_app_empty,
	&test_compile_create_lib_empty,
	&test_compile_create_no_och_files,
	&test_compile_create_h_files,
	&test_compile_create_c_files,
	&test_compile_create_co_files,
	&test_compile_create_o_files,
	&test_compile_destroy_null,
	&test_compile_prepare_null,
	&test_compile_execute_null,
	&test_compile_project_create_null,
	&test_compile_project_create_invalid_path,
	&test_compile_project_create_not_project,
	&test_compile_project_create,
	&test_compile_project_prepare_null,
	&test_compile_project_prepare_empty,
	&test_compile_project_prepare_lib,
	&test_compile_project_prepare_lib_and_app,
	&test_compile_project_prepare_libs_and_app,
	&test_compile_project_prepare_libs_and_app_circle,
	&test_compile_project_prepare_app_indirect_lib,
	&test_compile_project_prepare_app_invalid_lib_include,
	&test_compile_project_destroy_null,
	&test_compile_project_execute_null,
	&test_compile_project_execute_lib,
	&test_compile_project_execute_app_twice,
	&test_compile_project_execute_lib_twice,
	&test_compile_project_execute_lib_and_app_twice,
	&test_compile_project_execute_lib_and_lib_twice,
	&test_compile_project_execute_2libs_and_1app,
	&test_compile_project_execute_3libs_and_2app,
	&test_compile_project_execute_with_directory_name_null,
	&test_compile_project_execute_with_directory_name_not_found,
	&test_compile_project_execute_with_directory_name_lib,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
