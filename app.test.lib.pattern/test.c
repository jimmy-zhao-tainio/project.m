#include <stdio.h>

#include <lib.test/test.h>

#include "test-pattern-whitespace.h"
#include "test-pattern-token.h"
#include "test-pattern-branch.h"
#include "test-pattern-path.h"
#include "test-pattern-path-match.h"
#include "test-pattern-search.h"

bool (*tests[]) (Test *test) =
{
        &test_pattern_whitespace_invalid_argument,
        &test_pattern_whitespace_overflow,
        &test_pattern_whitespace_1,
        &test_pattern_whitespace_2,
        &test_pattern_whitespace_3,
        &test_pattern_whitespace_function_call,
        &test_pattern_tokens_invalid_argument_1,
        &test_pattern_tokens_invalid_argument_2,
        &test_pattern_tokens_function_call,
        &test_pattern_tokens_1,
        &test_pattern_tokens_overflow,
        &test_pattern_tokens_2,
        &test_pattern_tokens_3,
        &test_pattern_tokens_parentheses_open_function_call,
        &test_pattern_tokens_parentheses_open,
        &test_pattern_tokens_parentheses_mismatch,
        &test_pattern_tokens_parentheses_close_parentheses_empty,
        &test_pattern_tokens_parentheses_close_function_call,
        &test_pattern_tokens_parentheses_close,
        &test_pattern_tokens_not_missing_operand,
        &test_pattern_tokens_not_invalid_operand,
        &test_pattern_tokens_not_function_call,
        &test_pattern_tokens_not,
        &test_pattern_tokens_or_missing_left_operand,
        &test_pattern_tokens_or_invalid_left_operand,
        &test_pattern_tokens_or_missing_right_operand,
        &test_pattern_tokens_or_invalid_right_operand,
        &test_pattern_tokens_or_function_call,
        &test_pattern_tokens_or,
        &test_pattern_tokens_repeat_missing_operand,
        &test_pattern_tokens_repeat_invalid_operand,
        &test_pattern_tokens_repeat_missing_from,
        &test_pattern_tokens_repeat_implicit_from,
        &test_pattern_tokens_repeat_invalid_from,
        &test_pattern_tokens_repeat_missing_close_bracket_1,
        &test_pattern_tokens_repeat_function_call_1,
        &test_pattern_tokens_repeat_missing_hyphen,
        &test_pattern_tokens_repeat_missing_to,
        &test_pattern_tokens_repeat_implicit_to,
        &test_pattern_tokens_repeat_invalid_to,
        &test_pattern_tokens_repeat_missing_close_bracket_2,
        &test_pattern_tokens_repeat_missing_close_bracket_3,
        &test_pattern_tokens_repeat_function_call_2,
        &test_pattern_tokens_repeat_1,
        &test_pattern_tokens_repeat_2,
        &test_pattern_tokens_repeat_3,
        &test_pattern_tokens_repeat_4,
        &test_pattern_tokens_repeat_5,
        &test_pattern_tokens_range_missing_from,
        &test_pattern_tokens_range_implicit_from,
        &test_pattern_tokens_range_invalid_from,
        &test_pattern_tokens_range_missing_hyphen_1,
        &test_pattern_tokens_range_missing_hyphen_2,
        &test_pattern_tokens_range_missing_to,
        &test_pattern_tokens_range_implicit_to,
        &test_pattern_tokens_range_invalid_to,
        &test_pattern_tokens_range_missing_close_bracket_1,
        &test_pattern_tokens_range_missing_close_bracket_2,
        &test_pattern_tokens_range_function_call,
        &test_pattern_tokens_range_1,
        &test_pattern_tokens_range_2,
        &test_pattern_tokens_range_3,
        &test_pattern_tokens_range_4,
        &test_pattern_tokens_set_missing_value_1,
        &test_pattern_tokens_set_value_invalid_1,
        &test_pattern_tokens_set_missing_value_2,
        &test_pattern_tokens_set_missing_closing_bracket,
        &test_pattern_tokens_set_function_call_1,
        &test_pattern_tokens_set_function_call_2,
        &test_pattern_tokens_set_1,
        &test_pattern_tokens_set_2,
        &test_pattern_tokens_set_3,
        &test_pattern_tokens_value_invalid,
        &test_pattern_tokens_value_function_call,
        &test_pattern_tokens_value,
        &test_pattern_tokens_string_to_value,
        &test_pattern_tokens_escape_to_value,
        &test_pattern_tokens_escape_to_value_B,
        &test_pattern_tokens_escape_to_value_o,
        &test_pattern_tokens_escape_to_value_x,
        &test_pattern_tokens_escapable_to_byte,
        &test_pattern_token_is_syntax,
        &test_pattern_token_is_octal,
        &test_pattern_token_is_hexadecimal,
        &test_pattern_token_is_binary,
        &test_pattern_branch_create_invalid_argument,
        &test_pattern_branch_create_function_call_1,
        &test_pattern_branch_create_function_call_2,
        &test_pattern_branch_create_invalid_operation,
        &test_pattern_branch_create,
        &test_pattern_branch_destroy_invalid_argument,
        &test_pattern_branch_create_parenthesize_main_branch_function_call_1,
        &test_pattern_branch_create_parenthesize_main_branch_function_call_2,
        &test_pattern_branch_create_parenthesize_main_branch_function_call_3,
        &test_pattern_branch_create_parenthesize_main_branch_function_call_4,
        &test_pattern_branch_create_parenthesize_main_branch,
        &test_pattern_branch_create_branch_create_function_call_1,
        &test_pattern_branch_create_branch_create_function_call_2,
        &test_pattern_branch_create_branch_create_function_call_3,
        &test_pattern_branch_create_branch_create_function_call_4,
        &test_pattern_branch_create_branch_create_function_call_5,
        &test_pattern_branch_create_part_branch_recursion_depth,
        &test_pattern_branch_create_part_branch_function_call_1,
        &test_pattern_branch_create_part_branch_function_call_2,
        &test_pattern_branch_create_part_not_function_call_1,
        &test_pattern_branch_create_part_not_function_call_2,
        &test_pattern_branch_create_part_or_function_call_1,
        &test_pattern_branch_create_part_or_function_call_2,
        &test_pattern_branch_create_part_or_function_call_3,
        &test_pattern_branch_create_part_or_function_call_4,
        &test_pattern_branch_create_part_or_function_call_5,
        &test_pattern_branch_create_part_repeat_function_call,
        &test_pattern_branch_create_part_range_function_call,
        &test_pattern_branch_create_part_set_function_call,
        &test_pattern_branch_create_part_value_function_call,
        &test_pattern_branch_create_1,
        &test_pattern_branch_create_2,
        &test_pattern_branch_create_3,
        &test_pattern_branch_create_4,
        &test_pattern_branch_create_5,
        &test_pattern_branch_create_6,
        &test_pattern_branch_create_7,
        &test_pattern_branch_create_8,
        &test_pattern_path_tracker_create_invalid_argument,
        &test_pattern_path_tracker_create_function_call_1,
        &test_pattern_path_tracker_create_function_call_2,
        &test_pattern_path_tracker_create_function_call_3,
        &test_pattern_path_tracker_create,
        &test_pattern_path_tracker_create_branch_function_call,
        &test_pattern_path_tracker_create_branch,
        &test_pattern_path_tracker_create_not_function_call,
        &test_pattern_path_tracker_create_not_range,
        &test_pattern_path_tracker_create_not_set,
        &test_pattern_path_tracker_create_not_value,
        &test_pattern_path_tracker_create_or_function_call_1,
        &test_pattern_path_tracker_create_or_function_call_2,
        &test_pattern_path_tracker_create_or_function_call_3,
        &test_pattern_path_tracker_create_or_function_call_4,
        &test_pattern_path_tracker_create_or,
        &test_pattern_path_tracker_create_repeat_function_call_1,
        &test_pattern_path_tracker_create_repeat_function_call_2,
        &test_pattern_path_tracker_create_repeat_function_call_3,
        &test_pattern_path_tracker_create_repeat,
        &test_pattern_path_tracker_create_range_function_call_1,
        &test_pattern_path_tracker_create_range_function_call_2,
        &test_pattern_path_tracker_create_range,
        &test_pattern_path_tracker_create_set_function_call_1,
        &test_pattern_path_tracker_create_set_function_call_2,
        &test_pattern_path_tracker_create_set,
        &test_pattern_path_tracker_create_value_function_call_1,
        &test_pattern_path_tracker_create_value_function_call_2,
        &test_pattern_path_tracker_create_value,
        &test_pattern_path_tracker_destroy,
        &test_pattern_path_match_1,
        &test_pattern_path_match_2,
        &test_pattern_path_match_3,
        &test_pattern_path_match_not_1,
        &test_pattern_path_match_not_2,
        &test_pattern_path_match_or,
        &test_pattern_path_match_repeat_1,
        &test_pattern_path_match_repeat_2,
        &test_pattern_path_match_repeat_3,
        &test_pattern_path_match_repeat_4,
        &test_pattern_path_match_repeat_5,
        &test_pattern_path_match_repeat_6,
        &test_pattern_path_match_range_1,
        &test_pattern_path_match_range_2,
        &test_pattern_path_match_set_2,
        &test_pattern_path_match_set_2,
        &test_pattern_search_create_function_call_1,
        &test_pattern_search_create_function_call_2,
        &test_pattern_search_create_function_call_3,
        &test_pattern_search_create_function_call_4,
        &test_pattern_search_create_function_call_5,
        &test_pattern_search_create,
        &test_pattern_search_next_1,
        &test_pattern_search_next_2,
        &test_pattern_search_next_3,
        &test_pattern_search_next_4,
        &test_pattern_search_next_5,
        &test_pattern_search_next_6,
        &test_pattern_search_next_7,
        &test_pattern_search_next_8,
        &test_pattern_search_next_9,
        NULL
};

int main (int argc, char **argv)
{
        return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
