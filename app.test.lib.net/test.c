#include <stdio.h>
#include <lib.test/test.h>

#include "test-server.h"

bool (*tests[]) (Test *test) =
{
        &test_server_create_invalid_argument_1,
        &test_server_create_invalid_argument_2,
        &test_server_create_invalid_argument_3,
        &test_server_create_invalid_argument_4,
        &test_server_create_invalid_argument_5,
        &test_server_create_function_call_1,
        &test_server_create_function_call_2,
        &test_server_create,
        &test_server_stop_invalid_argument,
        &test_server_on_open,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
