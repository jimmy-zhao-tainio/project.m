#include <limits.h>
#include <lib.core/ascii.h>

#include "test-ascii.h"

bool test_ascii_is_white_space (Test *test)
{
	int i;

	TITLE ();
	for (i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
		if ((char)i == ' ' ||
		    (char)i == '\f' ||
		    (char)i == '\n' ||
		    (char)i == '\r' ||
		    (char)i == '\t' ||
		    (char)i == '\v') {
			CATCH (!ascii_is_white_space ((char)i));
		}
		else {
			CATCH (ascii_is_white_space ((char)i));
		}
	}
	PASS ();
}

bool test_ascii_is_digit (Test *test)
{
	int i;

	TITLE ();
	for (i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
		if ((char)i == '0' ||
		    (char)i == '1' ||
		    (char)i == '2' ||
		    (char)i == '3' ||
		    (char)i == '4' ||
		    (char)i == '5' ||
		    (char)i == '6' ||
		    (char)i == '7' ||
		    (char)i == '8' ||
		    (char)i == '9') {
			CATCH (!ascii_is_digit ((char)i));
		}
		else {
			CATCH (ascii_is_digit ((char)i));
		}
	}
	PASS ();
}

bool test_ascii_is_digit_octal (Test *test)
{
	int i;

	TITLE ();
	for (i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
		if ((char)i == '0' ||
		    (char)i == '1' ||
		    (char)i == '2' ||
		    (char)i == '3' ||
		    (char)i == '4' ||
		    (char)i == '5' ||
		    (char)i == '6' ||
		    (char)i == '7') {
			CATCH (!ascii_is_digit_octal ((char)i));
		}
		else {
			CATCH (ascii_is_digit_octal ((char)i));
		}
	}
	PASS ();
}

bool test_ascii_is_digit_hexadecimal (Test *test)
{
	int i;

	TITLE ();
	for (i = SCHAR_MIN; i <= SCHAR_MAX; i++) {
		if ((char)i == '0' ||
		    (char)i == '1' ||
		    (char)i == '2' ||
		    (char)i == '3' ||
		    (char)i == '4' ||
		    (char)i == '5' ||
		    (char)i == '6' ||
		    (char)i == '7' ||
		    (char)i == '8' ||
		    (char)i == '9' ||
		    (char)i == 'a' ||
		    (char)i == 'b' ||
		    (char)i == 'c' ||
		    (char)i == 'd' ||
		    (char)i == 'e' ||
		    (char)i == 'f' ||
		    (char)i == 'A' ||
		    (char)i == 'B' ||
		    (char)i == 'C' ||
		    (char)i == 'D' ||
		    (char)i == 'E' ||
		    (char)i == 'F') {
			CATCH (!ascii_is_digit_hexadecimal ((char)i));
		}
		else {
			CATCH (ascii_is_digit_hexadecimal ((char)i));
		}
	}
	PASS ();
}
