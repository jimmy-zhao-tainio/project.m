#ifndef ascii_h
#define ascii_h

#include <lib.core/defines.h>

bool ascii_is_white_space (char value);
bool ascii_is_digit (char value);
bool ascii_is_digit_octal (char value);
bool ascii_is_digit_hexadecimal (char value);

#endif
