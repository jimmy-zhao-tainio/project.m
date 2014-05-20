#include <lib.core/error.h>
#include <lib.core/string.h>

#include "test-error.h"

bool test_error (Test *test)
{
	ErrorItem item;
	size_t i;

	TITLE ();
	for (i = 0; i < ERROR_ITEMS_MAX; i++) {
		item = error_at (i);
		CATCH (item.file);
		CATCH (item.function);
		CATCH (item.line);
		CATCH (item.code);
		CATCH (item.error);
		CATCH (item.literal);
		CATCH (item.invalid_error);
	}
	CATCH (error_count () != 0);
	PASS ();
}

bool test_error_add_invalid_error (Test *test)
{
	char *file = "file";
	char *function = "function";
	ErrorItem item;

	TITLE ();
	error (Reserved);
	item = error_at (0);
	CATCH (item.error != ErrorInvalidError);
	CATCH (!string_equals (item.literal, "ErrorInvalidError"));
	CATCH (item.invalid_error != 0);
	error_reset ();
	error (Null);
	item = error_at (0);
	CATCH (item.error != ErrorInvalidError);
	CATCH (!string_equals (item.literal, "ErrorInvalidError"));
	CATCH (item.invalid_error != ErrorNull);
	error_reset ();
	error_add (file, function, 1, 0, (Error)32767);
	item = error_at (0);
	CATCH (item.error != ErrorInvalidError);
	CATCH (item.file != file);
	CATCH (item.function != function);
	CATCH (item.line != 1);
	CATCH (!string_equals (item.literal, "ErrorInvalidError"));
	CATCH (item.invalid_error != (Error)32767);
	error_reset ();
	error_add (file, function, 1, 0, (Error)-32768);
	item = error_at (0);
	CATCH (item.error != ErrorInvalidError);
	CATCH (item.file != file);
	CATCH (item.function != function);
	CATCH (item.line != 1);
	CATCH (!string_equals (item.literal, "ErrorInvalidError"));
	/*CATCH (item.invalid_error != (Error)-32768);*/
	error_reset ();
	error (InvalidError);
	item = error_at (0);
	CATCH (item.error != ErrorInvalidError);
	CATCH (!string_equals (item.literal, "ErrorInvalidError"));
	CATCH (item.invalid_error != ErrorInvalidError);
	PASS ();
}

bool test_error_add (Test *test)
{
	size_t i;

	TITLE ();
	for (i = 0; i < ERROR_ITEMS_MAX * 2; i++) {
		error(InvalidArgument);
	}
	for (i = 0; i < ERROR_ITEMS_MAX; i++) {
		CATCH (error_at (i).error != ErrorInvalidArgument);
	}
	error (Overflow);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_ends_with (error_at (0).file, "test-error.c"));
	CATCH (!string_equals (error_at (0).function, "test_error_add"));
	CATCH (error_at (0).line == 0);
	CATCH (!string_equals (error_at (0).literal, "ErrorOverflow"));
	CATCH (error_at (0).invalid_error != 0);
	CATCH (error_count () != ERROR_ITEMS_MAX);
	PASS ();
}

bool test_error_reset (Test *test)
{
	ErrorItem item;
	size_t i;

	TITLE ();
	for (i = 0; i < ERROR_ITEMS_MAX * 2; i++) {
		error(InvalidArgument);
	}
	error_reset ();
	CATCH (error_count () != 0);
	for (i = 0; i < ERROR_ITEMS_MAX; i++) {
		item = error_at (i);
		CATCH (item.file);
		CATCH (item.function);
		CATCH (item.line);
		CATCH (item.code);
		CATCH (item.error);
		CATCH (item.literal);
		CATCH (item.invalid_error);
	}
	PASS ();
}

bool test_error_count (Test *test)
{
	size_t i;

	TITLE ();
	for (i = 0; i < ERROR_ITEMS_MAX * 2; i++) {
		error(InvalidArgument);
		if (i + 1 < ERROR_ITEMS_MAX) {
			CATCH (error_count () != i + 1);
		}
		else {
			CATCH (error_count () != ERROR_ITEMS_MAX);
		}
	}
	PASS ();
}

bool test_error_at (Test *test)
{
	size_t i;

	TITLE ();
	error (Overflow);
	for (i = 0; i < ERROR_ITEMS_MAX - 1; i++) {
		error(InvalidArgument);
	}
	CATCH (error_at (ERROR_ITEMS_MAX - 1).error != ErrorOverflow);
	CATCH (error_at (ERROR_ITEMS_MAX).error != ErrorReserved);
	CATCH (error_at (ERROR_ITEMS_MAX).error != 0);
	CATCH (error_at ((size_t)-1).error != 0);
	PASS ();
}
