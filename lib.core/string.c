#include <string.h>

#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

char *string_create (const char *c)
{
	char *string;
	size_t length;

	if (!c) {
		error (InvalidArgument);
		return NULL;
	}
	length = strlen (c);
	if (!size_t_add (length, 1, &length)) {
		error (Overflow);
		return NULL;
	}
	if (!(string = memory_create (length))) {
		error (FunctionCall);
		return NULL;
	}
	strcpy (string, c);
	return string;
}

char *string_create_with_size (size_t size)
{
	char *string;
	
	if (size < 1) {
		error (InvalidOperation);
		return NULL;
	}
	if (!(string = memory_create (size))) {
		error (FunctionCall);
		return NULL;
	}
	string[0] = '\0';
	return string;
}

void string_destroy (char *string)
{
	if (!string) {
		error (InvalidArgument);
		return;
	}
	memory_destroy (string);
}

bool string_append (char **string, const char *c)
{
	char *pointer;
	size_t c_length;
	size_t string_length;
	size_t length;
	
	if (!string ||
	    !*string ||
	    !c) {
		error (InvalidArgument);	
		return false;
	}
	c_length = strlen (c);
	string_length = strlen (*string);
	if (c_length == 0) {
		return true;
	}
	if (!size_t_add (string_length, c_length, &length)) {
		error (Overflow);
		return false;
	}
	if (!size_t_add (length, 1, &length)) {
		error (Overflow);
		return false;
	}
	if (length > memory_size (*string)) {
		if (!(pointer = memory_grow (*string, length))) {
			error (FunctionCall);
			return false;
		}
		*string = pointer;
	}
	strncpy (&(*string)[string_length], c, c_length);
	(*string)[length - 1] = '\0';
	return true;
}

bool string_equals (const char *string, const char *match)
{
	if (!string || !match) {
		error (InvalidArgument);
		return false;
	}
	if (strcmp (string, match) != 0) {
		return false;
	}
	return true;
}

bool string_begins_with (const char *string, const char *begin)
{
	size_t string_length;
	size_t begin_length;

	if (!string || !begin) {
		error (InvalidArgument);
		return false;
	}
	string_length = strlen (string);
	begin_length = strlen (begin);
	if (string_length == 0) {
		if (begin_length == 0) {
			return true;
		}
		return false;
	}
	if (begin_length == 0) {
		return false;
	}
	if (string_length < begin_length) {
		return false;
	}
	if (strncmp (string, begin, begin_length) != 0) {
		return false;
	}
	return true;
}

bool string_ends_with (const char *string, const char *end)
{
	size_t string_length;
	size_t end_length;

	if (!string || !end) {
		error (InvalidArgument);
		return false;
	}
	string_length = strlen (string);
	end_length = strlen (end);
	if (string_length == 0) {
		if (end_length == 0) {
			return true;
		}
		return false;
	}
	if (end_length == 0) {
		return false;
	}
	if (string_length < end_length) {
		return false;
	}
	if (strncmp (&string[string_length - end_length], end, end_length) != 0) {
		return false;
	}
	return true;
}

bool string_index (const char *string, char match, size_t *index)
{
	size_t i;
	size_t length;

	if (!string || !index) {
		error (InvalidArgument);
		return false;
	}
	length = strlen (string);
	if (length == 0 ||
	    match == '\0') {
		return false;
	}
	if (!size_t_add (length, 1, NULL)) {
		error (Overflow);
		return false;
	}
	i = 0;
	do {
		if (string[i] == match) {
			*index = i;
			return true;
		}
	} while (i++ < length);
	return false;
}

static size_t split_next (const char *string, size_t position, char separator);
static char *split_copy (const char *string, size_t from, size_t to);
static void split_destroy (char **array);

char **string_split (const char *string, char separator)
{
	char **array;
	size_t array_size = 1;
	size_t array_position = 0;
	size_t i;
	size_t next;

	if (!string) {
		error (InvalidArgument);
		return NULL;
	}
	for (i = 0; string[i] != '\0'; i++) {
		if (string[i] == separator) {
			array_size++;
		}
	}
	array_size++;
	if (!size_t_mul (sizeof (char *), array_size, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!(array = memory_create (sizeof (char *) * array_size))) {
		error (FunctionCall);
		return NULL;
	}
	i = array_size;
	while (i-- > 0) {
		array[i] = NULL;
	}
	i = 0;
	while (array_position < array_size) {
		next = split_next (string, i, separator);
		if (!(array[array_position] = split_copy (string, i, next))) {
			split_destroy (array);
			error (FunctionCall);
			return NULL;
		}
		if (string[next] == '\0') {
			return array;
		}
		i = next + 1;
		array_position++;
	}
	return NULL; /* Unreachable but produces a compile warning, rewrite needed! */
}

static size_t split_next (const char *string, size_t position, char separator)
{
	size_t next = position;

	while (string[next] != '\0' &&
	       string[next] != separator) {
		next++;
	}
	return next;
}

static char *split_copy (const char *string, size_t from, size_t to)
{
	char *copy;
	size_t copy_size;
	size_t i;

	copy_size = (to - from) + 1;
	if (!(copy = memory_create (copy_size))) {
		return NULL;
	}
	for (i = 0; i < copy_size - 1; i++) {
		copy[i] = string[from + i];
	}
	copy[copy_size - 1] = '\0';
	return copy;
}

static void split_destroy (char **array)
{
	size_t i;

	for (i = 0; array[i]; i++) {
		memory_destroy (array[i]);
	}
	memory_destroy (array);
}

size_t string_length (const char *string)
{
	if (!string) {
		return 0;
	}
	return strlen (string);
}
