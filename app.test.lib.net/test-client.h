#ifndef test_client_h
#define test_client_h

#include <lib.test/test.h>

bool test_client_create_invalid_argument_1 (Test *test);
bool test_client_create_invalid_argument_2 (Test *test);
bool test_client_create_invalid_argument_3 (Test *test);
bool test_client_create_invalid_argument_4 (Test *test);
bool test_client_create_function_call_1 (Test *test);
bool test_client_create_function_call_2 (Test *test);
bool test_client_create (Test *test);
bool test_client_stop_invalid_argument (Test *test);
bool test_client_connect_invalid_argument_1 (Test *test);
bool test_client_connect_invalid_argument_2 (Test *test);
bool test_client_connect_system_call_1 (Test *test);
bool test_client_on_connect (Test *test);

#endif
