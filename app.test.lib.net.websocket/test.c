#include <stdio.h>
#include <lib.test/test.h>

#include "test-http.h"

bool (*tests[]) (Test *test) =
{
        &test_http_request_begin,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
