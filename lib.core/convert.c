#include <lib.core/convert.h>
#include <lib.core/ascii.h>
#include <lib.core/error.h>
#include <errno.h>
#include <stdlib.h>

bool convert_string_to_size_t (const char *string, size_t *result, size_t *digits)
{
        unsigned long long value;

        if (!result) {
                error (InvalidArgument);
                return false;
        }
        if (!convert_string_to_unsigned_long_long (string, &value, digits)) {
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
        char *endptr;

	if (!string) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!result) {
		error_code (InvalidArgument, 2);
		return false;
	}
        if (!digits) {
                error_code (InvalidArgument, 3);
                return false;
        }
	for (*digits = 0; ascii_is_digit (string[*digits]); *digits += 1) {
        }
	if (*digits == 0) {
		error_code (InvalidOperation, 1);
		return false;
	}
        if (*digits > 1 && string[0] == '0') {
                error_code (InvalidOperation, 2);
                return false;
        }
        errno = 0;
        *result = strtoull (string, &endptr, 10);
        if (errno != 0) {
                if (errno == ERANGE) {
                        error (Overflow);
                        return false;
                }
                error_code (InvalidOperation, 3);
                return false;
        }
        if (endptr != string + *digits) {
                error_code (InvalidOperation, 4);
                return false;
        }
	return true;
}
