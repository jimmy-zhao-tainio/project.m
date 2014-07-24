#ifndef convert_h
#define convert_h

#include <lib.core/defines.h>

bool convert_string_to_int (const char *string, int *result, size_t *digits);
bool convert_string_to_size_t (const char *string, size_t *result, size_t *digits);
bool convert_string_to_unsigned_long_long (const char *string, unsigned long long *result, size_t *digits);

#endif
