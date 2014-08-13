#ifndef test_app_arguments_validate_h
#define test_app_arguments_validate_h

#include <lib.test/test.h>

bool test_app_arguments_validate_boolean_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_integer_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_uint64_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_string_value_pointer_is_null (Test *test);
bool test_app_arguments_validate_ordinal_required_discontinuity (Test *test);
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

#endif
