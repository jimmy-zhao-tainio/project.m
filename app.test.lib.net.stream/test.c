#include <stdio.h>
#include <lib.test/test.h>

#include "test-stream.h"
#include "test-stream-1.h"
#include "test-stream-10x512x10.h"
#include "test-stream-100x512x100.h"
#include "test-stream-1000x512x1000.h"

bool (*tests[]) (Test *test) =
{
        &test_stream_create_invalid_argument_1,
        &test_stream_create_invalid_argument_2,
        &test_stream_create_invalid_argument_3,
        &test_stream_create_function_call_1,
        &test_stream_create_function_call_2,
        &test_stream_create_function_call_3,
        &test_stream_create,
        &test_stream_1,
        &test_stream_10x512x10,
        &test_stream_100x512x100,
        &test_stream_1000x512x1000,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
