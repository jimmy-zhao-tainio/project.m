#include <stdio.h>
#include <lib.test/test.h>

#include "test-encode.h"

bool (*tests[]) (Test *test) =
{
        &test_encode_base64,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
