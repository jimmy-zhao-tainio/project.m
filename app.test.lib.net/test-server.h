#ifndef test_server_h
#define test_server_h

#include <lib.test/test.h>

bool test_server_create_invalid_argument_1 (Test *test);
bool test_server_create_invalid_argument_2 (Test *test);
bool test_server_create_invalid_argument_3 (Test *test);
bool test_server_create_invalid_argument_4 (Test *test);
bool test_server_create_invalid_argument_5 (Test *test);
bool test_server_create_function_call_1 (Test *test);
bool test_server_create_function_call_2 (Test *test);
bool test_server_create (Test *test);
bool test_server_stop_invalid_argument (Test *test);
bool test_server_on_open (Test *test);

#endif
