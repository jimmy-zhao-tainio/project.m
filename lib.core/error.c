#include <stdio.h>
#include <lib.core/error.h>

static ErrorItem error_items[ERROR_ITEMS_MAX];

static size_t      error_literals_count = 0;
static const char *error_literals[] = {
#undef ERROR
#undef ERROR_STRING
#define ERROR_STRING(e) #e,
#define ERROR(e) ERROR_STRING(Error##e)
#include <lib.core/error-enums.h>
#include <lib.core/error-enums-pattern.h>
	NULL
#undef ERROR_STRING
#undef ERROR
};

void error_add (const char *file, const char *function, int line, int code, Error error)
{
	size_t i = ERROR_ITEMS_MAX;
	int invalid_error;

	if (error_literals_count == 0) {
		while (error_literals[error_literals_count++]);
	}
	if (error <= 1 || 
	    error >= error_literals_count - 1 ||
	    !file ||
	    !function) {
		invalid_error = error;
		error = ErrorInvalidError;
	}
	else {
		invalid_error = 0;
	}
	while (i-- > 1) {
		error_items[i].file = error_items[i - 1].file;
		error_items[i].function = error_items[i - 1].function;
		error_items[i].line = error_items[i - 1].line;
		error_items[i].code = error_items[i - 1].code;
		error_items[i].error = error_items[i - 1].error;
		error_items[i].literal = error_items[i - 1].literal;
		error_items[i].invalid_error = error_items[i - 1].invalid_error;
	}
	error_items[0].file = file;
	error_items[0].function = function;
	error_items[0].line = line;
	error_items[0].code = code;
	error_items[0].error = error;
	error_items[0].literal = error_literals[(size_t)error];
	error_items[0].invalid_error = invalid_error;
}

void error_reset (void)
{
	size_t i = ERROR_ITEMS_MAX;

	while (i-- > 0) {
		error_items[i].file = NULL;
		error_items[i].function = NULL;
		error_items[i].line = 0;
		error_items[i].code = 0;
		error_items[i].error = ErrorReserved;
		error_items[i].literal = NULL;
		error_items[i].invalid_error = 0;
	}
}

size_t error_count (void)
{
	size_t i = 0;

	while (i < ERROR_ITEMS_MAX && error_items[i].error != ErrorReserved) {
		i++;
	}
	return i;
}

ErrorItem error_at (size_t index)
{
	ErrorItem item = { NULL, NULL, 0, 0, 0, NULL, 0 };

	if (index >= ERROR_ITEMS_MAX) {
		return item;
	}
	item.file = error_items[index].file;
	item.function = error_items[index].function;
	item.line = error_items[index].line;
	item.code = error_items[index].code;
	item.error = error_items[index].error;
	item.literal = error_items[index].literal;
	item.invalid_error = error_items[index].invalid_error;
	return item;
}

void error_print (void)
{
	size_t i = 0;

	while (i < ERROR_ITEMS_MAX && error_items[i].error != ErrorReserved) {
		printf ("\t%s: %i\n\t\t%s", 
			error_items[i].function,
			error_items[i].line,
			error_items[i].literal);
		if (error_items[i].code != 0) {
			printf (" #%i\n", (int)error_items[i].code);
		}
		else {
			printf ("\n");
		}
		i++;
	}
}
