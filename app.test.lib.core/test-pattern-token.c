#include <lib.core/pattern-token.h>
#include <lib.core/pattern-token-private.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/defines-private.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <stdlib.h>

#include "test-pattern-token.h"

bool test_pattern_tokens_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_pattern_tokens_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (0);
	CATCH (pattern_tokens_create (""));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_1 (Test *test)
{
	List *list;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("")));
	CATCH (list_count (list) != 0);
	pattern_tokens_destroy (list);
	PASS ();
}	

bool test_pattern_tokens_overflow (Test *test)
{
	TITLE ();
	size_t_private_max (sizeof (size_t) + sizeof (List));
	CATCH (pattern_tokens_create ("aaaaaaaabbbbbbbbc"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	PASS ();
}

bool test_pattern_tokens_2 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("("));
	PASS ();
}

bool test_pattern_tokens_3 (Test *test)
{
	List *list;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("()")));
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_parentheses_open_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List));
	CATCH (pattern_tokens_create ("("));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_parentheses_open (Test *test)
{
	List *list;
	PatternToken *token;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("()")));
	CATCH (list_count (list) == 0);
	token = list_first (list)->data;
	CATCH (token->type != PatternTokenTypeParenthesesOpen);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_parentheses_mismatch (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create (")"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternParenthesesMismatch);
	PASS ();
}

bool test_pattern_tokens_parentheses_close_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
				 sizeof (size_t) + sizeof (PatternToken) +
				 sizeof (size_t) + sizeof (ListNode) +
				 sizeof (size_t) + sizeof (PatternToken) - 
				 1);
	CATCH (pattern_tokens_create ("()"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_parentheses_close (Test *test)
{
	List *list;
	PatternToken *token;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("()")));
	CATCH (list_count (list) != 2);
	token = list_first (list)->next->data;
	CATCH (token->type != PatternTokenTypeParenthesesClose);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_not_missing_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("!"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternNotMissingOperand);
	PASS ();
}

bool test_pattern_tokens_not_invalid_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("!()"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternNotInvalidOperand);
	PASS ();
}

bool test_pattern_tokens_not_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
			     sizeof (size_t) + sizeof (PatternToken) -
			     1);
	CATCH (pattern_tokens_create ("!0"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_not (Test *test)
{
	List *list;
	PatternToken *token;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("!a")));
	CATCH (list_count (list) != 2);
	token = list_first (list)->data;
	CATCH (token->type != PatternTokenTypeNot);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_or_missing_left_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("|"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternOrMissingLeftOperand);
	PASS ();
}

bool test_pattern_tokens_or_invalid_left_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("(|)"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternOrInvalidLeftOperand);
	PASS ();
}

bool test_pattern_tokens_or_missing_right_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("0|"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternOrMissingRightOperand);
	PASS ();
}

bool test_pattern_tokens_or_invalid_right_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("(0|)"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternOrInvalidRightOperand);
	PASS ();
}

bool test_pattern_tokens_or_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
				 sizeof (size_t) + sizeof (PatternTokenValue) +
				 sizeof (size_t) + sizeof (ListNode) +
				 sizeof (size_t) + sizeof (PatternToken) - 
				 1);
	CATCH (pattern_tokens_create ("0|1"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_or (Test *test)
{
	List *list;
	PatternToken *token;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("0|1")));
	CATCH (list_count (list) != 3);
	token = list_first (list)->next->data;
	CATCH (token->type != PatternTokenTypeOr);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("{"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingOperand);
	PASS ();
}

bool test_pattern_tokens_repeat_invalid_operand (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("({"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatInvalidOperand);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_from (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingFrom);
	PASS ();
}

bool test_pattern_tokens_repeat_implicit_from (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{-12}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 0);
	CATCH (repeat->to != 12);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_invalid_from (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{X"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorPatternRepeatInvalidFrom);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_close_bracket_1 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingCloseBracket);
	PASS ();
}

bool test_pattern_tokens_repeat_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
				 sizeof (size_t) + sizeof (PatternTokenValue) +
				 sizeof (size_t) + sizeof (ListNode) +
				 sizeof (size_t) + sizeof (PatternTokenRepeat) -
				 1);
	CATCH (pattern_tokens_create ("a{1}"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_hyphen (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1+"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingHyphen);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_to (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1-"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingTo);
	PASS ();
}

bool test_pattern_tokens_repeat_implicit_to (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{12-}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 12);
	CATCH (repeat->to != SIZE_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_invalid_to (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1-X"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorPatternRepeatInvalidTo);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_close_bracket_2 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1-2"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingCloseBracket);
	PASS ();
}

bool test_pattern_tokens_repeat_missing_close_bracket_3 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("a{1-2)"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRepeatMissingCloseBracket);
	PASS ();
}

bool test_pattern_tokens_repeat_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) + 
				 sizeof (size_t) + sizeof (PatternTokenValue) +
				 sizeof (size_t) + sizeof (ListNode) +
				 sizeof (size_t) + sizeof (PatternTokenRepeat) -
				 1);
	CATCH (pattern_tokens_create ("a{1-2}"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_repeat_1 (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{12}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 12);
	CATCH (repeat->to != 12);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_2 (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{12-3456789}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 12);
	CATCH (repeat->to != 3456789);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_3 (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{-123}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 0);
	CATCH (repeat->to != 123);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_4 (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{123-}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 123);
	CATCH (repeat->to != SIZE_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_repeat_5 (Test *test)
{
	List *list;
	PatternTokenRepeat *repeat;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a{-}")));
	CATCH (list_count (list) != 2);
	repeat = list_first (list)->next->data;
	CATCH (repeat->token.type != PatternTokenTypeRepeat);
	CATCH (repeat->from != 0);
	CATCH (repeat->to != SIZE_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_missing_from (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("["));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingFrom);
	PASS ();
}

bool test_pattern_tokens_range_implicit_from (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[-a]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)0);
	CATCH (range->to != (unsigned char)'a');
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_invalid_from (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[!"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeInvalidFrom);
	PASS ();
}

bool test_pattern_tokens_range_missing_hyphen_1 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingHyphen);
	PASS ();
}

bool test_pattern_tokens_range_missing_hyphen_2 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0+"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingHyphen);
	PASS ();
}

bool test_pattern_tokens_range_missing_to (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0-"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingTo);
	PASS ();
}

bool test_pattern_tokens_range_implicit_to (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[a-]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)'a');
	CATCH (range->to != UCHAR_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_invalid_to (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0-!"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeInvalidTo);
	PASS ();
}

bool test_pattern_tokens_range_missing_close_bracket_1 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0-1"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingCloseBracket);
	PASS ();
}

bool test_pattern_tokens_range_missing_close_bracket_2 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("[0-1)"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternRangeMissingCloseBracket);
	PASS ();
}

bool test_pattern_tokens_range_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
				 sizeof (size_t) + sizeof (PatternTokenRange) -
				 1);
	CATCH (pattern_tokens_create ("[0-1]"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_range_1 (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[0-1]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)'0');
	CATCH (range->to != (unsigned char)'1');
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_2 (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[-\\xAA]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)0);
	CATCH (range->to != (unsigned char)0xAA);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_3 (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[\\xAA-]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)0xAA);
	CATCH (range->to != UCHAR_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_range_4 (Test *test)
{
	List *list;
	PatternTokenRange *range;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("[-]")));
	CATCH (list_count (list) != 1);
	range = list_first (list)->data;
	CATCH (range->token.type != PatternTokenTypeRange);
	CATCH (range->from != (unsigned char)0);
	CATCH (range->to != UCHAR_MAX);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_set_missing_value_1 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("<"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternSetMissingValue);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_pattern_tokens_set_value_invalid_1 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("<!"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternSetValueInvalid);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_pattern_tokens_set_missing_value_2 (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("<>"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternSetMissingValue);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_pattern_tokens_set_missing_closing_bracket (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("<a"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternSetMissingClosingBracket);
	PASS ();
}

bool test_pattern_tokens_set_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
			     sizeof (size_t) + sizeof (PatternTokenSet) -
			     1);
	CATCH (pattern_tokens_create ("<a>"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_pattern_tokens_set_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
			     sizeof (size_t) + sizeof (PatternTokenSet) +
			     sizeof (size_t) + 1 -
			     1);
	CATCH (pattern_tokens_create ("<a>"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_pattern_tokens_set_1 (Test *test)
{
	List *list;
	PatternTokenSet *set;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("<\\xAA>")));
	CATCH (list_count (list) != 1);
	set = list_first (list)->data;
	CATCH (set->token.type != PatternTokenTypeSet);
	CATCH (set->values_length != 1);
	CATCH (memory_size (set->values) != 1);
	CATCH (set->values[0] != (unsigned char)0xAA);
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_set_2 (Test *test)
{
	List *list;
	PatternTokenSet *set;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("<a\\xBBc>")));
	CATCH (list_count (list) != 1);
	set = list_first (list)->data;
	CATCH (set->token.type != PatternTokenTypeSet);
	CATCH (set->values_length != 3);
	CATCH (memory_size (set->values) != 3);
	CATCH (set->values[0] != 'a');
	CATCH (set->values[1] != (unsigned char)0xBB);
	CATCH (set->values[2] != 'c');
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_set_3 (Test *test)
{
	List *list;
	PatternTokenSet *set;
	PatternTokenValue *value_a;
	PatternTokenValue *value_d;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a<bc>d")));
	CATCH (list_count (list) != 3);
	value_a = list_first (list)->data;
	set = list_first (list)->next->data;
	value_d = list_first (list)->next->next->data;
	CATCH (value_a->value != 'a');
	CATCH (set->token.type != PatternTokenTypeSet);
	CATCH (set->values_length != 2);
	CATCH (memory_size (set->values) != 2);
	CATCH (set->values[0] != 'b');
	CATCH (set->values[1] != 'c');
	CATCH (value_d->value != 'd');
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_value_invalid (Test *test)
{
	TITLE ();
	CATCH (pattern_tokens_create ("\\z"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternValueInvalid);
	PASS ();
}

bool test_pattern_tokens_value_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (List) +
			     sizeof (size_t) + sizeof (PatternTokenValue) -
			     1);
	CATCH (pattern_tokens_create ("a"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}

bool test_pattern_tokens_value (Test *test)
{
	List *list;
	PatternTokenValue *value;

	TITLE ();
	CATCH (!(list = pattern_tokens_create ("a")));
	CATCH (list_count (list) != 1);
	value = list_first (list)->data;
	CATCH (value->token.type != PatternTokenTypeValue);
	CATCH (value->value != 'a');
	pattern_tokens_destroy (list);
	PASS ();
}

bool test_pattern_tokens_string_to_value (Test *test)
{
	char pattern[2];
	size_t i;
	unsigned char value;

	TITLE ();
	i = 1;
	pattern[0] = 'a';
	CATCH (pattern_token_string_to_value (pattern, 1, &i, &value));
	i = 0;
	pattern[0] = '\0';
	CATCH (pattern_token_string_to_value (pattern, 1, &i, &value));
	i = 0;
	pattern[0] = '(';
	CATCH (pattern_token_string_to_value (pattern, 1, &i, &value));
	i = 0;
	pattern[0] = '\\';
	pattern[1] = 'z';
	CATCH (pattern_token_string_to_value (pattern, 2, &i, &value));
	i = 0;
	pattern[0] = '\\';
	pattern[1] = 'n';
	CATCH (!pattern_token_string_to_value (pattern, 2, &i, &value));
	CATCH (value != '\n');
	CATCH (i != 2);
	i = 0;
	pattern[0] = 'a';
	CATCH (!pattern_token_string_to_value (pattern, 2, &i, &value));
	CATCH (value != 'a');
	CATCH (i != 1);
	PASS ();
}

bool test_pattern_tokens_escape_to_value (Test *test)
{
	size_t i;
	unsigned char value;

	TITLE ();
	i = 1;
	CATCH (pattern_token_escape_to_value ("\\z", 2, &i, &value));
	PASS ();
}

bool test_pattern_tokens_escape_to_value_B (Test *test)
{
	char B[10] = "\\B00001111";
	size_t i;
	unsigned char value;

	TITLE ();
	i = 0;
	CATCH (pattern_token_escape_to_value (B, 10 - 1, &i, &value));
	i = 0;
	B[9] = 'x';
	CATCH (pattern_token_escape_to_value (B, 10, &i, &value));
	i = 0;
	B[2] = '1'; B[3] = '1'; B[4] = '1'; B[5] = '1';
	B[6] = '1'; B[7] = '1'; B[8] = '1'; B[9] = '1';
	value = 0;
	CATCH (!pattern_token_escape_to_value (B, 10, &i, &value));
	CATCH (!(value & (1 << 0)));
	CATCH (!(value & (1 << 1)));
	CATCH (!(value & (1 << 2)));
	CATCH (!(value & (1 << 3)));
	CATCH (!(value & (1 << 4)));
	CATCH (!(value & (1 << 5)));
	CATCH (!(value & (1 << 6)));
	CATCH (!(value & (1 << 7)));
	CATCH (i != 10);
	i = 0;
	B[2] = '0'; B[3] = '0'; B[4] = '0'; B[5] = '0';
	B[6] = '0'; B[7] = '0'; B[8] = '0'; B[9] = '0';
	value = 2;
	CATCH (!pattern_token_escape_to_value (B, 10, &i, &value));
	CATCH ((value & (1 << 0)));
	CATCH ((value & (1 << 1)));
	CATCH ((value & (1 << 2)));
	CATCH ((value & (1 << 3)));
	CATCH ((value & (1 << 4)));
	CATCH ((value & (1 << 5)));
	CATCH ((value & (1 << 6)));
	CATCH ((value & (1 << 7)));
	CATCH (i != 10);
	i = 0;
	B[2] = '1'; B[3] = '0'; B[4] = '1'; B[5] = '0';
	B[6] = '1'; B[7] = '0'; B[8] = '1'; B[9] = '0';
	value = 0;
	CATCH (!pattern_token_escape_to_value (B, 10, &i, &value));
	CATCH ( (value & (1 << 0)));
	CATCH (!(value & (1 << 1)));
	CATCH ( (value & (1 << 2)));
	CATCH (!(value & (1 << 3)));
	CATCH ( (value & (1 << 4)));
	CATCH (!(value & (1 << 5)));
	CATCH ( (value & (1 << 6)));
	CATCH (!(value & (1 << 7)));
	CATCH (i != 10);
	i = 0;
	B[2] = '0'; B[3] = '1'; B[4] = '0'; B[5] = '1';
	B[6] = '0'; B[7] = '1'; B[8] = '0'; B[9] = '1';
	value = 0;
	CATCH (!pattern_token_escape_to_value (B, 10, &i, &value));
	CATCH (!(value & (1 << 0)));
	CATCH ( (value & (1 << 1)));
	CATCH (!(value & (1 << 2)));
	CATCH ( (value & (1 << 3)));
	CATCH (!(value & (1 << 4)));
	CATCH ( (value & (1 << 5)));
	CATCH (!(value & (1 << 6)));
	CATCH ( (value & (1 << 7)));
	CATCH (i != 10);
	PASS ();
}

bool test_pattern_tokens_escape_to_value_o (Test *test)
{
	char o[5] = "\\o123";
	size_t i;
	unsigned char value;

	TITLE ();
	i = 0;
	CATCH (pattern_token_escape_to_value (o, 5 - 1, &i, &value));
	i = 0;
	o[4] = '8';
	CATCH (pattern_token_escape_to_value (o, 5, &i, &value));
	i = 0;
	o[2] = '3';
	o[3] = '7';
	o[4] = '7';
	CATCH (!pattern_token_escape_to_value (o, 5, &i, &value));
	CATCH (value != 0377);
	CATCH (i != 5);
	PASS ();
	i = 0;
	o[2] = '0';
	o[3] = '0';
	o[4] = '0';
	CATCH (!pattern_token_escape_to_value (o, 5, &i, &value));
	CATCH (value != 0);
	CATCH (i != 5);
	PASS ();
	i = 0;
	o[2] = '2';
	o[3] = '2';
	o[4] = '2';
	CATCH (!pattern_token_escape_to_value (o, 5, &i, &value));
	CATCH (value != 0222);
	CATCH (i != 5);
	PASS ();
}

bool test_pattern_tokens_escape_to_value_x (Test *test)
{
	char x[4] = "\\x12";
	size_t i;
	unsigned char value;

	TITLE ();
	i = 0;
	CATCH (pattern_token_escape_to_value (x, 4 - 1, &i, &value));
	i = 0;
	x[3] = 'G';
	CATCH (pattern_token_escape_to_value (x, 4, &i, &value));
	i = 0;
	x[2] = 'F';
	x[3] = 'F';
	CATCH (!pattern_token_escape_to_value (x, 4, &i, &value));
	CATCH (value != 0xFF);
	CATCH (i != 4);
	PASS ();
	i = 0;
	x[2] = '0';
	x[3] = '0';
	CATCH (!pattern_token_escape_to_value (x, 4, &i, &value));
	CATCH (value != 0);
	CATCH (i != 4);
	PASS ();
	i = 0;
	x[2] = '2';
	x[3] = '2';
	CATCH (!pattern_token_escape_to_value (x, 4, &i, &value));
	CATCH (value != 0x22);
	CATCH (i != 4);
	PASS ();
}

bool test_pattern_tokens_escapable_to_byte (Test *test)
{
	unsigned char value = 0;

	TITLE ();
	CATCH (!pattern_token_escapable_to_byte ('(', &value));
	CATCH (value != (unsigned char)'(');
	CATCH (!pattern_token_escapable_to_byte (' ', &value));
	CATCH (value != (unsigned char)' ');
	CATCH (!pattern_token_escapable_to_byte ('\'', &value));
	CATCH (value != (unsigned char)'\'');
	CATCH (!pattern_token_escapable_to_byte ('\"', &value));
	CATCH (value != (unsigned char)'\"');
	CATCH (!pattern_token_escapable_to_byte ('\\', &value));
	CATCH (value != (unsigned char)'\\');
	CATCH (!pattern_token_escapable_to_byte ('a', &value));
	CATCH (value != (unsigned char)'\a');
	CATCH (!pattern_token_escapable_to_byte ('b', &value));
	CATCH (value != (unsigned char)'\b');
	CATCH (!pattern_token_escapable_to_byte ('f', &value));
	CATCH (value != (unsigned char)'\f');
	CATCH (!pattern_token_escapable_to_byte ('n', &value));
	CATCH (value != (unsigned char)'\n');
	CATCH (!pattern_token_escapable_to_byte ('r', &value));
	CATCH (value != (unsigned char)'\r');
	CATCH (!pattern_token_escapable_to_byte ('t', &value));
	CATCH (value != (unsigned char)'\t');
	CATCH (!pattern_token_escapable_to_byte ('v', &value));
	CATCH (value != (unsigned char)'\v');
	CATCH (pattern_token_escapable_to_byte ('z', &value));
	PASS ();
}

bool test_pattern_token_is_syntax (Test *test)
{
	char cs[] = { '(', ')', '!', '|', '[', ']', '{', '}', '\0' };
	size_t i;

	TITLE ();
	for (i = 0; cs[i] != '\0'; i++) {
		CATCH (!pattern_token_is_syntax (cs[i]));
	}
	CATCH (pattern_token_is_syntax ('z'));
	PASS ();
}

bool test_pattern_token_is_octal (Test *test)
{
	TITLE ();
	CATCH (pattern_token_is_octal ('8', '0', '0'));
	CATCH (pattern_token_is_octal ('0', '8', '0'));
	CATCH (pattern_token_is_octal ('0', '0', '8'));
	CATCH (pattern_token_is_octal ('4', '0', '0'));
	CATCH (!pattern_token_is_octal ('3', '7', '7'));
	PASS ();
}

bool test_pattern_token_is_hexadecimal (Test *test)
{
	TITLE ();
	CATCH (pattern_token_is_hexadecimal ('G', '0'));
	CATCH (pattern_token_is_hexadecimal ('0', 'G'));
	CATCH (!pattern_token_is_hexadecimal ('F', 'F'));
	PASS ();
}

bool test_pattern_token_is_binary (Test *test)
{
	TITLE ();
	CATCH (pattern_token_is_binary ('2', '0', '0', '0', '0', '0', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '2', '0', '0', '0', '0', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '2', '0', '0', '0', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '0', '2', '0', '0', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '0', '0', '2', '0', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '0', '0', '0', '2', '0', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '0', '0', '0', '0', '2', '0'));
	CATCH (pattern_token_is_binary ('0', '0', '0', '0', '0', '0', '0', '2'));
	CATCH (!pattern_token_is_binary ('0', '0', '0', '0', '0', '0', '0', '0'));
	CATCH (!pattern_token_is_binary ('1', '1', '1', '1', '1', '1', '1', '1'));
	PASS ();
}
