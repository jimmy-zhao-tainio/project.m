#include <lib.core/ascii.h>

bool ascii_is_white_space (char value)
{
	if (value == ' ' ||
	    value == '\f' ||
	    value == '\n' ||
	    value == '\r' ||
	    value == '\t' ||
	    value == '\v') {
		return true;
	}
	return false;
}

bool ascii_is_digit (char value)
{
	if (value == '0' ||
	    value == '1' ||
	    value == '2' ||
	    value == '3' ||
	    value == '4' ||
	    value == '5' ||
	    value == '6' ||
	    value == '7' ||
	    value == '8' ||
	    value == '9') {
		return true;
	}
	return false;
}

bool ascii_is_digit_octal (char value)
{
	if (value == '0' ||
	    value == '1' ||
	    value == '2' ||
	    value == '3' ||
	    value == '4' ||
	    value == '5' ||
	    value == '6' ||
	    value == '7') {
		return true;
	}
	return false;
}

bool ascii_is_digit_hexadecimal (char value)
{
	if (ascii_is_digit (value) ||
	    value == 'a' ||
	    value == 'b' ||
	    value == 'c' ||
	    value == 'd' ||
	    value == 'e' ||
	    value == 'f' ||
	    value == 'A' ||
	    value == 'B' ||
	    value == 'C' ||
	    value == 'D' ||
	    value == 'E' ||
	    value == 'F') {
		return true;
	}
	return false;
}

