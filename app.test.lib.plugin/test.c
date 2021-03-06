#include <stdio.h>
#include <lib.test/test.h>

#include "test-plugin.h"

bool (*tests[]) (Test *test) =
{
        &test_plugin_create_invalid_argument,
        &test_plugin_create_function_call_1,
        &test_plugin_create_function_call_2,
        &test_plugin_create_function_call_3,
        &test_plugin_create_function_call_4,
        &test_plugin_set_function_invalid_argument_1,
        &test_plugin_set_function_invalid_argument_2,
        &test_plugin_set_function_invalid_argument_3,
        &test_plugin_set_function_invalid_operation,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
