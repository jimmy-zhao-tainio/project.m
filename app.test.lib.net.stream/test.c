#include <stdio.h>
#include <lib.test/test.h>

#include "test-stream.h"

bool (*tests[]) (Test *test) =
{
        &test_stream_create_invalid_argument_1,
        &test_stream_create_invalid_argument_2,
        &test_stream_create_invalid_argument_3,
        &test_stream_create_invalid_argument_4,
        &test_stream_create_invalid_argument_5,
        &test_stream_create_function_call_1,
        &test_stream_create_function_call_2,
        &test_stream_create_function_call_3,
        &test_stream_create_function_call_4,
        &test_stream_create_function_call_5,
        &test_stream_create,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
