#include <stdio.h>
#include <lib.test/test.h>

#include "test-http.h"

bool (*tests[]) (Test *test) =
{
        &test_http_request_begin,
        &test_http_request_end_1,
        &test_http_request_end_2,
        &test_http_set_method,
        &test_http_set_uri,
        &test_http_set_version,
        &test_http_set_headers,
        &test_http_get_header_1,
        &test_http_get_header_2,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
