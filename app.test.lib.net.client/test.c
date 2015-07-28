#include <stdio.h>
#include <lib.test/test.h>

#include "test-client.h"
#include "test-client-on-connect.h"
#include "test-client-on-connect-error.h"

bool (*tests[]) (Test *test) =
{
        &test_client_create_invalid_argument_1,
        &test_client_create_invalid_argument_2,
        &test_client_create_invalid_argument_3,
        &test_client_create_function_call_1,
        &test_client_create_function_call_2,
        &test_client_create_function_call_3,
        &test_client_create,
        &test_client_destroy_invalid_argument,
        &test_client_on_connect,
        &test_client_on_connect_error,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
