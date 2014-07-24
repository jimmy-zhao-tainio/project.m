#ifndef test_compile_h
#define test_compile_h

#include <lib.test/test.h>

bool test_app_arguments_invalid_argument_1 (Test *test);
bool test_app_arguments_invalid_argument_2 (Test *test);
bool test_app_arguments_invalid_argument_3 (Test *test);
bool test_app_arguments_function_call_1 (Test *test);
bool test_app_arguments_function_call_2 (Test *test);
bool test_app_arguments_function_call_3 (Test *test);
bool test_app_arguments_function_call_4 (Test *test);
bool test_app_arguments_validate_boolean_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_integer_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_string_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_missing_short_or_long_form (Test *test);
bool test_app_arguments_validate_invalid_short_form_length (Test *test);
bool test_app_arguments_validate_short_form_missing_single_dash (Test *test);
bool test_app_arguments_validate_invalid_short_form_name_1 (Test *test);
bool test_app_arguments_validate_invalid_short_form_name_2 (Test *test);
bool test_app_arguments_validate_long_form_missing_double_dash (Test *test);
bool test_app_arguments_validate_invalid_long_form_length (Test *test);
bool test_app_arguments_validate_invalid_long_form_name_1 (Test *test);
bool test_app_arguments_validate_invalid_long_form_name_2 (Test *test);
bool test_app_arguments_validate_short_form_duplicate (Test *test);
bool test_app_arguments_validate_long_form_duplicate (Test *test);
bool test_app_arguments_named_duplicate (Test *test);
bool test_app_arguments_named_missing_integer_value (Test *test);
bool test_app_arguments_named_function_call (Test *test);
bool test_app_arguments_named_invalid_integer_value (Test *test);
bool test_app_arguments_named_missing_string_value (Test *test);
bool test_app_arguments_named_unknown_argument (Test *test);
bool test_app_arguments_ordinal_function_call (Test *test);
bool test_app_arguments_ordinal_invalid_integer_value (Test *test);
bool test_app_arguments_ordinal_unknown_argument (Test *test);
bool test_app_arguments_missing_required_argument (Test *test);
bool test_app_arguments (Test *test);

#endif
