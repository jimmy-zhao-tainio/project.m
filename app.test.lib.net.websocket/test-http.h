#ifndef test_http_h
#define test_http_h

#include <lib.test/test.h>

bool test_http_request_begin (Test *test);
bool test_http_request_end_1 (Test *test);
bool test_http_request_end_2 (Test *test);
bool test_http_set_method (Test *test);
bool test_http_set_uri (Test *test);
bool test_http_set_version (Test *test);
bool test_http_set_headers (Test *test);
bool test_http_get_header_1 (Test *test);
bool test_http_get_header_2 (Test *test);

#endif
