#include <lib.core/defines.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

#include "test-convert.h"

bool test_convert_string_decimal_to_size_t_invalid_argument_1 (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (convert_string_decimal_to_size_t (NULL, &result));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	PASS ();
}

bool test_convert_string_decimal_to_size_t_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (convert_string_decimal_to_size_t ("1", NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	PASS ();
}

bool test_convert_string_decimal_to_size_t_invalid_operation_1 (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (convert_string_decimal_to_size_t ("", &result));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	PASS ();
}

bool test_convert_string_decimal_to_size_t_invalid_operation_2 (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (convert_string_decimal_to_size_t ("01", &result));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	PASS ();
}

bool test_convert_string_decimal_to_size_t_overflow_1 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_decimal_to_size_t ("75535", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_decimal_to_size_t ("5294967295", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_decimal_to_size_t ("28446744073709551615", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	CATCH (error_at (0).line != 35);
	PASS ();
}

bool test_convert_string_decimal_to_size_t_overflow_2 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_decimal_to_size_t ("65536", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_decimal_to_size_t ("4294967296", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_decimal_to_size_t ("18446744073709551616", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	CATCH (error_at (0).line != 39);
	PASS ();
}

bool test_convert_string_decimal_to_size_t_overflow_3 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_decimal_to_size_t ("165535", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_decimal_to_size_t ("14294967295", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_decimal_to_size_t ("118446744073709551615", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_decimal_to_size_t"));
	CATCH (error_at (0).line != 44);
	PASS ();
}

bool test_convert_string_decimal_to_size_t (Test *test)
{
	size_t result;

	TITLE ();
	result = 1;
	CATCH (!convert_string_decimal_to_size_t ("0", &result));
	CATCH (result != 0);
	CATCH (!convert_string_decimal_to_size_t ("1", &result));
	CATCH (result != 1);
	CATCH (!convert_string_decimal_to_size_t ("10", &result));
	CATCH (result != 10);
	CATCH (!convert_string_decimal_to_size_t ("11", &result));
	CATCH (result != 11);
	CATCH (!convert_string_decimal_to_size_t ("12345", &result));
	CATCH (result != 12345);
	CATCH (!convert_string_decimal_to_size_t ("54321", &result));
	CATCH (result != 54321);
	CATCH (error_count () != 0);
	PASS ();
}
