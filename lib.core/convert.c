#include <lib.core/convert.h>
#include <lib.core/ascii.h>
#include <lib.core/error.h>

static unsigned char convert_ascii_digit_to_decimal (char digit);

bool convert_string_decimal_to_size_t (char *string, size_t *result)
{
	size_t power;
	size_t i;
	size_t add;

	if (!string) {
		error (InvalidArgument);
		return false;
	}
	if (!result) {
		error (InvalidArgument);
		return false;
	}
	for (i = 0; ascii_is_digit (string[i]); i++);
	if (i == 0 || string[i] != '\0') {
		error (InvalidOperation);
		return false;
	}
	if (i != 1 && string[0] == '0') {
		error (InvalidOperation);
		return false;
	}
	*result = 0;
	power = 1;
	while (i-- > 0) {
		add = (size_t)convert_ascii_digit_to_decimal (string[i]);
		if (!size_t_mul (add, power, &add)) {
			error (Overflow);
			return false;
		}
		if (!size_t_add (*result, add, result)) {
			error (Overflow);
			return false;
		}
		if (i != 0) {
			if (!size_t_mul (power, 10, &power)) {
				error (Overflow);
				return false;
			}
		}
	}
	return true;
}

static unsigned char convert_ascii_digit_to_decimal (char digit)
{
	if (digit == '0') {
		return (unsigned char)0;
	}
	if (digit == '1') {
		return (unsigned char)1;
	}
	if (digit == '2') {
		return (unsigned char)2;
	}
	if (digit == '3') {
		return (unsigned char)3;
	}
	if (digit == '4') {
		return (unsigned char)4;
	}
	if (digit == '5') {
		return (unsigned char)5;
	}
	if (digit == '6') {
		return (unsigned char)6;
	}
	if (digit == '7') {
		return (unsigned char)7;
	}
	if (digit == '8') {
		return (unsigned char)8;
	}
	return (unsigned char)9;
}
