#include <stdio.h>
#include <lib.test/test.h>

#include "test-stream.h"
#include "test-add.h"
#include "test-write.h"
#include "test-write-close.h"

bool (*tests[]) (Test *test) =
{
        &test_stream_create_function_call_1,
        &test_stream_create_function_call_2,
        &test_stream_create,
        &test_add,
        &test_write,
        &test_write_close,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
