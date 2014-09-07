#ifndef test_app_arguments_h
#define test_app_arguments_h

#include <lib.test/test.h>

bool test_app_arguments_invalid_argument_1 (Test *test);
bool test_app_arguments_invalid_argument_2 (Test *test);
bool test_app_arguments_invalid_argument_3 (Test *test);
bool test_app_arguments_function_call_1 (Test *test);
bool test_app_arguments_function_call_2 (Test *test);
bool test_app_arguments_function_call_3 (Test *test);
bool test_app_arguments_function_call_4 (Test *test);
bool test_app_arguments_function_call_5 (Test *test);
bool test_app_arguments_named_duplicate (Test *test);
bool test_app_arguments_named_missing_integer_value (Test *test);
bool test_app_arguments_named_function_call_1 (Test *test);
bool test_app_arguments_named_invalid_integer_value (Test *test);
bool test_app_arguments_named_missing_uint64_value (Test *test);
bool test_app_arguments_named_function_call_2 (Test *test);
bool test_app_arguments_named_invalid_uint64_value (Test *test);
bool test_app_arguments_named_missing_string_value (Test *test);
bool test_app_arguments_named_unknown_argument (Test *test);
bool test_app_arguments_ordinal_function_call_1 (Test *test);
bool test_app_arguments_ordinal_invalid_integer_value (Test *test);
bool test_app_arguments_ordinal_function_call_2 (Test *test);
bool test_app_arguments_ordinal_invalid_uint64_value (Test *test);
bool test_app_arguments_ordinal_unknown_argument (Test *test);
bool test_app_arguments_missing_required_argument_1 (Test *test);
bool test_app_arguments_missing_required_argument_2 (Test *test);
bool test_app_arguments (Test *test);

#endif
