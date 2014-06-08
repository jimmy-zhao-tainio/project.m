#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/list.h>
#include <lib.core/convert.h>
#include <lib.core/ascii.h>
#include <lib.core/error.h>
#include <lib.core/pattern-token.h>
#include <lib.core/pattern-token-private.h>

static bool tokenize_create_try (const char *pattern, size_t length, List *tokens);
static bool tokenize_parentheses_open (size_t *i, PatternToken **token);
static bool tokenize_parentheses_close (size_t *i, PatternToken **token);
static bool tokenize_not (size_t *i, PatternToken **token);
static bool tokenize_or (const char *pattern, size_t length, size_t *i, PatternToken **token);
static bool tokenize_value (const char *pattern, size_t length, size_t *i, PatternToken **token);
static bool tokenize_set (const char *pattern, size_t length, size_t *i, PatternToken **token);
static bool tokenize_range (const char *pattern, size_t length, size_t *i, PatternToken **token);
static bool tokenize_repeat (const char *pattern, size_t length, size_t *i, PatternToken **token);

List *pattern_tokens_create (const char *pattern)
{
	List *tokens;
	size_t length;

	if (!pattern) {
		error (InvalidArgument);
		return NULL;
	}
	if (!(tokens = list_create ())) {
		error (FunctionCall);
		return NULL;
	}
	if ((length = string_length (pattern)) == 0) {
		return tokens;
	}
	if (!size_t_add (length, length, NULL)) {
		pattern_tokens_destroy (tokens);
		error (Overflow);
		return NULL;
	}
	if (!tokenize_create_try (pattern, length, tokens)) {
		pattern_tokens_destroy (tokens);
		return NULL;
	}
	return tokens;
}

void pattern_tokens_destroy (List *tokens)
{
	PatternToken *token;
	PatternTokenSet *set;
	ListNode *node;

	for (node = list_first (tokens); node; node = node->next) {
		token = node->data;
		if (token->type == PatternTokenTypeSet) {
			set = (PatternTokenSet *)token;
			if (set->values != NULL) {
				memory_destroy (set->values);
			}
		}
		memory_destroy (node->data);
	}
	list_destroy (tokens);
}

static bool tokenize_create_try (const char *pattern, size_t length, List *tokens)
{
	PatternToken *token = NULL;
	PatternToken *token_previous;
	size_t parentheses_open = 0;
	size_t i = 0;

	while (i < length) {
		switch (pattern[i]) {
		case '(':
			parentheses_open++;
			if (!tokenize_parentheses_open (&i, &token)) {
				return false;
			}
			break;
		case ')':
			if (parentheses_open == 0) {
				error (PatternParenthesesMismatch);
				return false;
			}
			parentheses_open--;
			if (!tokenize_parentheses_close (&i, &token)) {
				return false;
			}
			break;
		case '!':
			if (!tokenize_not (&i, &token)) {
				return false;
			}
			break;
		case '|':
			if (!tokenize_or (pattern, length, &i, &token)) {
				return false;
			}
			break;
		case '{':
			if (!tokenize_repeat (pattern, length, &i, &token)) {
				return false;
			}
			break;
		case '[':
			if (!tokenize_range (pattern, length, &i, &token)) {
				return false;
			}
			break;
		case '<':
			if (!tokenize_set (pattern, length, &i, &token)) {
				return false;
			}
			break;
		default:
			if (!tokenize_value (pattern, length, &i, &token)) {
				return false;
			}
			break;
		}
		if (list_count (tokens) != 0) {
			token_previous = list_last (tokens)->data;
			if (token_previous->type == PatternTokenTypeNot &&
			    token->type != PatternTokenTypeRange &&
			    token->type != PatternTokenTypeSet &&
			    token->type != PatternTokenTypeValue) {
				memory_destroy (token);
				error (PatternNotInvalidOperand);
				return false;
			}
		}
		if (!list_append (tokens, token)) {
			memory_destroy (token);
			error (FunctionCall);
			return false;
		}
	}
	if (list_count (tokens) != 0) {
		token_previous = list_last (tokens)->data;
		if (token_previous->type == PatternTokenTypeNot) {
			error (PatternNotMissingOperand);
			return false;
		}
	}
	if (parentheses_open != 0) {
		return false;
	}
	return true;
}

static bool tokenize_parentheses_open (size_t *i, PatternToken **token)
{
	if (!(*token = memory_create (sizeof (PatternToken)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeParenthesesOpen;
	(*i)++;
	return true;
}

static bool tokenize_parentheses_close (size_t *i, PatternToken **token)
{
	if (!(*token = memory_create (sizeof (PatternToken)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeParenthesesClose;
	(*i)++;
	return true;
}

static bool tokenize_not (size_t *i, PatternToken **token)
{
	if (!(*token = memory_create (sizeof (PatternToken)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeNot;
	(*i)++;
	return true;
}

static bool tokenize_or (const char *pattern, size_t length, size_t *i, PatternToken **token)
{
	if (!(*token)) {
		error (PatternOrMissingLeftOperand);
		return false;
	}
	if ((*token)->type != PatternTokenTypeParenthesesClose &&
	    (*token)->type != PatternTokenTypeRepeat &&
	    (*token)->type != PatternTokenTypeRange &&
	    (*token)->type != PatternTokenTypeSet &&
	    (*token)->type != PatternTokenTypeValue) {
		error (PatternOrInvalidLeftOperand);
		return false;
	}
	if (*i + 1 >= length) {
		error (PatternOrMissingRightOperand);
		return false;
	}
	if (pattern[*i + 1] == ')' ||
	    pattern[*i + 1] == '|' ||
	    pattern[*i + 1] == '{' ||
	    pattern[*i + 1] == '}' ||
	    pattern[*i + 1] == ']' ||
	    pattern[*i + 1] == '>') {
		error (PatternOrInvalidRightOperand);
		return false;
	}
	if (!(*token = memory_create (sizeof (PatternToken)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeOr;
	(*i)++;
	return true;
}

static bool tokenize_repeat (const char *pattern, size_t length, size_t *i, PatternToken **token)
{
	unsigned long long from;
	unsigned long long to;
        size_t digits;

	if (!(*token)) {
		error (PatternRepeatMissingOperand);
		return false;
	}
	if ((*token)->type != PatternTokenTypeParenthesesClose &&
	    (*token)->type != PatternTokenTypeRepeat &&
	    (*token)->type != PatternTokenTypeRange &&
	    (*token)->type != PatternTokenTypeSet &&
	    (*token)->type != PatternTokenTypeValue) {
		error (PatternRepeatInvalidOperand);
		return false;
	}
	if (++(*i) >= length) {
		error (PatternRepeatMissingFrom);
		return false;
	}
	if (pattern[*i] == '-') {
		from = 0;
	}
	else {
		if (!convert_string_to_unsigned_long_long (pattern + *i, &from, &digits)) {
			error (PatternRepeatInvalidFrom);
			return false;
		}
                *i += digits;
		if (*i >= length) {
			error (PatternRepeatMissingCloseBracket);
			return false;
		}
		if (pattern[*i] == '}') {
			if (!(*token = memory_create (sizeof (PatternTokenRepeat)))) {
				error (FunctionCall);
				return false;
			}
			(*token)->type = PatternTokenTypeRepeat;
			((PatternTokenRepeat *)(*token))->from = from;
			((PatternTokenRepeat *)(*token))->to = from;
			(*i)++;
			return true;
		}
		if (pattern[*i] != '-') {
			error (PatternRepeatMissingHyphen);
			return false;
		}
	}
	if (++(*i) >= length) {
		error (PatternRepeatMissingTo);
		return false;
	}
	if (pattern[*i] == '}') {
		to = ULLONG_MAX;
	}
	else {
		if (!convert_string_to_unsigned_long_long (pattern + *i, &to, &digits)) {
			error (PatternRepeatInvalidTo);
			return false;
		}
                *i += digits;
		if (*i >= length) {
			error (PatternRepeatMissingCloseBracket);
			return false;
		}
		if (pattern[*i] != '}') {
			error (PatternRepeatMissingCloseBracket);
			return false;
		}
	}
	if (!(*token = memory_create (sizeof (PatternTokenRepeat)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeRepeat;
	((PatternTokenRepeat *)(*token))->from = from;
	((PatternTokenRepeat *)(*token))->to = to;
	(*i)++;
	return true;
}

static bool tokenize_range (const char *pattern, size_t length, size_t *i, PatternToken **token)
{
	unsigned char from;
	unsigned char to;

	if (++(*i) >= length) {
		error (PatternRangeMissingFrom);
		return false;
	}
	if (pattern[*i] == '-') {
		from = 0;
	}
	else {
		if (!pattern_token_string_to_value (pattern, length, i, &from)) {
			error (PatternRangeInvalidFrom);
			return false;
		}
		if (*i >= length) {
			error (PatternRangeMissingHyphen);
			return false;
		}
		if (pattern[*i] != '-') {
			error (PatternRangeMissingHyphen);
			return false;
		}
	}
	if (++(*i) >= length) {
		error (PatternRangeMissingTo);
		return false;
	}
	if (pattern[*i] == ']') {
		to = UCHAR_MAX;
	}
	else {
		if (!pattern_token_string_to_value (pattern, length, i, &to)) {
			error (PatternRangeInvalidTo);
			return false;
		}
		if (*i >= length) {
			error (PatternRangeMissingCloseBracket);
			return false;
		}
		if (pattern[*i] != ']') {
			error (PatternRangeMissingCloseBracket);
			return false;
		}
	}
	if (!(*token = memory_create (sizeof (PatternTokenRange)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeRange;
	((PatternTokenRange *)(*token))->from = from;
	((PatternTokenRange *)(*token))->to = to;
	(*i)++;
	return true;
}

static bool tokenize_set (const char *pattern, size_t length, size_t *i, PatternToken **token)
{
	size_t values_length;
	size_t seek;
	unsigned char value;
	
	if (++(*i) >= length) {
		error_code (PatternSetMissingValue, 1);
		return false;
	}
	for (seek = *i, values_length = 0; seek < length && pattern[seek] != '>'; values_length++) {
		if (!pattern_token_string_to_value (pattern, length, &seek, &value)) {
			error_code (PatternSetValueInvalid, 1);
			return false;
		}
	}
	if (values_length == 0) {
		error_code (PatternSetMissingValue, 2);
		return false;
	}
	if (seek >= length) {
		error (PatternSetMissingClosingBracket);
		return false;
	}
	if (!(*token = memory_create (sizeof (PatternTokenSet)))) {
		error_code (FunctionCall, 1);
		return false;
	}
	(*token)->type = PatternTokenTypeSet;
	if (!(((PatternTokenSet *)(*token))->values = memory_create (values_length))) {
		memory_destroy (*token);
		*token = NULL;
		error_code (FunctionCall, 2);
		return false;
	} 
	((PatternTokenSet *)(*token))->values_length = values_length;
	for (values_length = 0; pattern[*i] != '>'; values_length++) {
		(void)pattern_token_string_to_value (pattern, length, i, &value);
		((PatternTokenSet *)(*token))->values[values_length] = value;
	}
	(*i)++;
	return true;
}

static bool tokenize_value (const char *pattern, size_t length, size_t *i, PatternToken **token)
{
	unsigned char value;
	
	if (!pattern_token_string_to_value (pattern, length, i, &value)) {
		error (PatternValueInvalid);
		return false;
	}
	if (!(*token = memory_create (sizeof (PatternTokenValue)))) {
		error (FunctionCall);
		return false;
	}
	(*token)->type = PatternTokenTypeValue;
	((PatternTokenValue *)(*token))->value = value;
	return true;
}

bool pattern_token_string_to_value (const char *pattern, size_t length, size_t *i, unsigned char *value)
{
	if (*i >= length) {
		return false;
	}
	if (pattern[*i] == '\0') {
		return false;
	}
	if (pattern_token_is_syntax (pattern[*i])) {
		return false;
	}
	if (pattern[*i] == '\\') {
		if (!pattern_token_escape_to_value (pattern, length, i, value)) {
			return false;
		}
		return true;
	}
	*value = (unsigned char)pattern[*i];
	(*i)++;
	return true;
}

bool pattern_token_escape_to_value (const char *pattern, size_t length, size_t *i, unsigned char *value)
{
	char buffer[9];

	if (*i + 1 >= length) {
		return false;
	}
	(*i)++;
	if (pattern[*i] == 'B') {
		if (*i + 8 >= length) {
			return false;
		}
		if (!pattern_token_is_binary (pattern[*i + 1],      
		                              pattern[*i + 2], 
		                              pattern[*i + 3], 
		                              pattern[*i + 4], 
		                              pattern[*i + 5], 
		                              pattern[*i + 6], 
		                              pattern[*i + 7], 
		                              pattern[*i + 8])) {
			return false;
		}
		buffer[0] = pattern[*i + 1];
		buffer[1] = pattern[*i + 2];
		buffer[2] = pattern[*i + 3];
		buffer[3] = pattern[*i + 4];
		buffer[4] = pattern[*i + 5];
		buffer[5] = pattern[*i + 6];
		buffer[6] = pattern[*i + 7];
		buffer[7] = pattern[*i + 8];
		buffer[8] = '\0';
		errno = 0;
		*value = (unsigned char)strtol (buffer, NULL, 2);
		if (errno != 0) {
			error (SystemCall);
			return false;
		}
		*i += 8 + 1;
		return true;
	}
	if (pattern[*i] == 'o') {
		if (*i + 3 >= length) {
			return false;
		}
		if (!pattern_token_is_octal (pattern[*i + 1], pattern[*i + 2], pattern[*i + 3])) {
			return false;
		}
		buffer[0] = pattern[*i + 1];
		buffer[1] = pattern[*i + 2];
		buffer[2] = pattern[*i + 3];
		buffer[3] = '\0';
		errno = 0;
		*value = (unsigned char)strtol (buffer, NULL, 8);
		if (errno != 0) {
			return false;
		}
		*i += 3 + 1;
		return true;
	}
	if (pattern[*i] == 'x') {
		if (*i + 2 >= length) {
			return false;
		}
		if (!pattern_token_is_hexadecimal (pattern[*i + 1], pattern[*i + 2])) {
			return false;
		}
		buffer[0] = pattern[*i + 1];
		buffer[1] = pattern[*i + 2];
		buffer[2] = '\0';
		errno = 0;
		*value = (unsigned char)strtol (buffer, NULL, 16);
		if (errno != 0) {
			return false;
		}
		*i += 2 + 1;
		return true;
	}
	if (pattern_token_escapable_to_byte (pattern[*i], value)) {
		(*i)++;
		return true;
	}
	return false;
}

bool pattern_token_escapable_to_byte (char escapable, unsigned char *value)
{
	if (pattern_token_is_syntax (escapable)) {
		*value = (unsigned char)escapable;
		return true;
	}
	switch (escapable) {
	case ' ':
	case '\'':
	case '\"':
	case '\\':
		*value = (unsigned char)escapable;
		return true;
	case 'a':
		*value = '\a';
		return true;
	case 'b':
		*value = '\b';
		return true;
	case 'f':
		*value = '\f';
		return true;
	case 'n':
		*value = '\n';
		return true;
	case 'r':
		*value = '\r';
		return true;
	case 't':
		*value = '\t';
		return true;
	case 'v':
		*value = '\v';
		return true;
	default:
		return false;
	};
}

bool pattern_token_is_syntax (char c)
{
	if (c == '(' ||
	    c == ')' ||
	    c == '!' ||
	    c == '|' ||
	    c == '{' ||
	    c == '}' ||
	    c == '[' ||
	    c == ']' ||
	    c == '<' ||
	    c == '>') {
		return true;
	}
	return false;
}

bool pattern_token_is_octal (char c1, char c2, char c3)
{
	if (!ascii_is_digit_octal (c1) ||
	    !ascii_is_digit_octal (c2) ||
	    !ascii_is_digit_octal (c3)) {
		return false;
	}
	if (c1 != '0' &&
	    c1 != '1' &&
	    c1 != '2' &&
	    c1 != '3') {
		return false;
	}
	return true;
}

bool pattern_token_is_hexadecimal (char c1, char c2)
{
	if (!ascii_is_digit_hexadecimal (c1) ||
	    !ascii_is_digit_hexadecimal (c2)) {
		return false;
	}
	return true;
}

bool pattern_token_is_binary (char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8)
{
	if ((c1 != '0' && c1 != '1') ||
	    (c2 != '0' && c2 != '1') ||
	    (c3 != '0' && c3 != '1') ||
	    (c4 != '0' && c4 != '1') ||
	    (c5 != '0' && c5 != '1') ||
	    (c6 != '0' && c6 != '1') ||
	    (c7 != '0' && c7 != '1') ||
	    (c8 != '0' && c8 != '1')) {
		return false;
	}
	return true;
}
