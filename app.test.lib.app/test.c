#include <stdio.h>

#include <lib.test/test.h>

#include "test-arguments.h"
#include "test-arguments-validate.h"
#include "test-arguments-usage.h"

bool (*tests[]) (Test *test) =
{
        &test_app_arguments_invalid_argument_1,
        &test_app_arguments_invalid_argument_2,
        &test_app_arguments_invalid_argument_3,
        &test_app_arguments_function_call_1,
        &test_app_arguments_function_call_2,
        &test_app_arguments_function_call_3,
        &test_app_arguments_function_call_4,
        &test_app_arguments_function_call_5,
	&test_app_arguments_named_duplicate,
	&test_app_arguments_named_missing_integer_value,
	&test_app_arguments_named_function_call_1,
	&test_app_arguments_named_invalid_integer_value,
	&test_app_arguments_named_missing_uint64_value,
	&test_app_arguments_named_function_call_2,
	&test_app_arguments_named_invalid_uint64_value,
	&test_app_arguments_named_missing_string_value,
	&test_app_arguments_named_unknown_argument,
	&test_app_arguments_ordinal_function_call_1,
	&test_app_arguments_ordinal_invalid_integer_value,
	&test_app_arguments_ordinal_function_call_2,
	&test_app_arguments_ordinal_invalid_uint64_value,
	&test_app_arguments_ordinal_unknown_argument,
	&test_app_arguments_missing_required_argument_1,
	&test_app_arguments_missing_required_argument_2,
	&test_app_arguments,
        &test_app_arguments_validate_boolean_value_pointer_is_null,
	&test_app_arguments_validate_integer_value_pointer_is_null,
	&test_app_arguments_validate_uint64_value_pointer_is_null,
	&test_app_arguments_validate_string_value_pointer_is_null,
	&test_app_arguments_validate_ordinal_required_discontinuity,
	&test_app_arguments_validate_missing_short_or_long_form,
	&test_app_arguments_validate_invalid_short_form_length,
	&test_app_arguments_validate_short_form_missing_single_dash,
	&test_app_arguments_validate_invalid_short_form_name_1,
	&test_app_arguments_validate_invalid_short_form_name_2,
	&test_app_arguments_validate_long_form_missing_double_dash,
	&test_app_arguments_validate_invalid_long_form_length,
	&test_app_arguments_validate_invalid_long_form_name_1,
	&test_app_arguments_validate_invalid_long_form_name_2,
	&test_app_arguments_validate_short_form_duplicate,
	&test_app_arguments_validate_long_form_duplicate,
        &test_app_arguments_usage_invalid_argument_1,
        &test_app_arguments_usage_invalid_argument_2,
        &test_app_arguments_usage_invalid_argument_3,
        &test_app_arguments_usage_function_call,
        &test_app_arguments_usage,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
