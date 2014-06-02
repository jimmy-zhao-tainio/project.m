#include <lib.core/defines.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

#include "test-convert.h"

bool test_convert_string_to_size_t_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (convert_string_to_size_t (NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_convert_string_to_size_t_function_call (Test *test)
{
        size_t result;

        TITLE ();
        CATCH (convert_string_to_size_t (NULL, &result, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        PASS ();
}

bool test_convert_string_to_size_t_overflow (Test *test)
{
        size_t result;
        size_t digits;

        TITLE ();
        if (sizeof (size_t) == 2) {
		CATCH (convert_string_to_size_t ("75535", &result, &digits));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_to_size_t ("5294967295", &result, &digits));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_to_size_t ("28446744073709551615", &result, &digits));
	}
	else {
		CATCH (1);
	}
        PASS ();
}

bool test_convert_string_to_size_t (Test *test)
{
	size_t result;
        size_t digits;

	TITLE ();
	result = 1;
	CATCH (!convert_string_to_size_t ("0", &result, &digits));
	CATCH (result != 0);
        CATCH (digits != 1);
	CATCH (!convert_string_to_size_t ("1", &result, &digits));
	CATCH (result != 1);
        CATCH (digits != 1);
	CATCH (!convert_string_to_size_t ("10", &result, &digits));
	CATCH (result != 10);
        CATCH (digits != 2);
	CATCH (!convert_string_to_size_t ("11", &result, &digits));
	CATCH (result != 11);
        CATCH (digits != 2);
	CATCH (!convert_string_to_size_t ("12345", &result, &digits));
	CATCH (result != 12345);
        CATCH (digits != 5);
	CATCH (!convert_string_to_size_t ("54321", &result, &digits));
	CATCH (result != 54321);
        CATCH (digits != 5);
        if (sizeof (size_t) == 2) {
		CATCH (!convert_string_to_size_t ("65535", &result, &digits));
                CATCH (result != (size_t)-1);
                CATCH (digits != 5);
	}
	else if (sizeof (size_t) == 4) {
		CATCH (!convert_string_to_size_t ("4294967295", &result, &digits));
                CATCH (result != (size_t)-1);
                CATCH (digits != 10);
	}
	else if (sizeof (size_t) == 8) {
		CATCH (!convert_string_to_size_t ("18446744073709551615", &result, &digits));
                CATCH (result != (size_t)-1);
                CATCH (digits != 20);
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () != 0);
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_invalid_argument_1 (Test *test)
{
	unsigned long long result;
        size_t digits;

	TITLE ();
	CATCH (convert_string_to_unsigned_long_long (NULL, &result, &digits));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_invalid_argument_2 (Test *test)
{
        size_t digits;

	TITLE ();
	CATCH (convert_string_to_unsigned_long_long ("1", NULL, &digits));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_invalid_argument_3 (Test *test)
{
        unsigned long long result;

	TITLE ();
	CATCH (convert_string_to_unsigned_long_long ("1", &result, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 3);
	PASS ();
}

/*
bool test_convert_string_to_unsigned_long_long_invalid_operation_1 (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (convert_string_to_size_t ("", &result));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (!string_equals (error_at (0).function, "convert_string_to_size_t"));
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_invalid_operation_2 (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (convert_string_to_size_t ("01", &result));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (!string_equals (error_at (0).function, "convert_string_to_size_t"));
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_overflow_1 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_to_size_t ("75535", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_to_size_t ("5294967295", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_to_size_t ("28446744073709551615", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_to_size_t"));
	CATCH (error_at (0).line != 35);
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_overflow_2 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_to_size_t ("65536", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_to_size_t ("4294967296", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_to_size_t ("18446744073709551616", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_to_size_t"));
	CATCH (error_at (0).line != 39);
	PASS ();
}

bool test_convert_string_to_unsigned_long_long_overflow_3 (Test *test)
{
	size_t result;

	TITLE ();
	if (sizeof (size_t) == 2) {
		CATCH (convert_string_to_size_t ("165535", &result));
	}
	else if (sizeof (size_t) == 4) {
		CATCH (convert_string_to_size_t ("14294967295", &result));
	}
	else if (sizeof (size_t) == 8) {
		CATCH (convert_string_to_size_t ("118446744073709551615", &result));
	}
	else {
		CATCH (1);
	}
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "convert_string_to_size_t"));
	CATCH (error_at (0).line != 44);
	PASS ();
}

*/
