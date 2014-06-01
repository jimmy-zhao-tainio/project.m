#include <lib.core/convert.h>
#include <lib.core/ascii.h>
#include <lib.core/error.h>
#include <errno.h>
#include <stdlib.h>

bool convert_string_to_size_t (const char *string, size_t *result, size_t *digits)
{
        unsigned long long value;

        if (!convert_string_to_unsigned_long_long (string, &value, digits)) {
                error (FunctionCall);
                return false;
        }
        if (value > (size_t)-1) {
                error (Overflow);
                return false;
        }
        *result = value;
        return true;
}

bool convert_string_to_unsigned_long_long (const char *string, unsigned long long *result, size_t *digits)
{
	if (!string) {
		error (InvalidArgument);
		return false;
	}
	if (!result) {
		error (InvalidArgument);
		return false;
	}
        if (!digits) {
                error (InvalidArgument);
                return false;
        }
	for (*digits = 0; ascii_is_digit (string[*digits]); *digits += 1) {
        }
	if (*digits == 0) {
		error (InvalidOperation);
		return false;
	}
        errno = 0;
        *result = strtoull (string, NULL, 10);
        if (errno != 0) {
                error (InvalidOperation);
                return false;
        }
	return true;
}
