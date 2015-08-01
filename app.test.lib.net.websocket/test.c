#include <stdio.h>
#include <lib.test/test.h>

#include "test-web-socket.h"

bool (*tests[]) (Test *test) =
{
        &test_web_socket_create_invalid_argument_1,
        &test_web_socket_create_invalid_argument_2,
        &test_web_socket_create_invalid_argument_3,
        &test_web_socket_create_invalid_argument_4,
        &test_web_socket_create_function_call_1,
        &test_web_socket_create_function_call_2,
        &test_web_socket_create_function_call_3,
        &test_web_socket_create_function_call_4,
        &test_web_socket_create,
        &test_web_socket_destroy_invalid_argument,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
