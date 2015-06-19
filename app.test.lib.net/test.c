#include <stdio.h>
#include <lib.test/test.h>

#include "test-server.h"
#include "test-client.h"
#include "test-client-server-10MB.h"

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
        &test_server_on_connect,
        &test_client_create_invalid_argument_1,
        &test_client_create_invalid_argument_2,
        &test_client_create_invalid_argument_3,
        &test_client_create_invalid_argument_4,
        &test_client_create_function_call_1,
        &test_client_create_function_call_2,
        &test_client_create,
        &test_client_stop_invalid_argument,
        &test_client_connect_invalid_argument_1,
        &test_client_connect_invalid_argument_2,
        &test_client_connect_system_call_1,
        &test_client_on_connect,
        &test_client_server_10MB,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
