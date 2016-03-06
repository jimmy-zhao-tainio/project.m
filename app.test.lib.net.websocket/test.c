#include <stdio.h>
#include <lib.test/test.h>

#include "test-http.h"

bool (*tests[]) (Test *test) =
{
        &test_http_request_begin,
        &test_http_request_end,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
