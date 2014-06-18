#ifndef defines_h
#define defines_h

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <limits.h>

bool size_t_add (size_t a, size_t b, size_t *result);
bool size_t_mul (size_t a, size_t b, size_t *result);
bool unsigned_long_long_add (unsigned long long a, unsigned long long b, unsigned long long *result);
bool unsigned_long_long_mul (unsigned long long a, unsigned long long b, unsigned long long *result);
bool unsigned_long_long_bit_get (unsigned long long a, unsigned int position);
unsigned long long unsigned_long_long_bit_set (unsigned long long a, unsigned int position, bool value);
/* Beware, the position returned for a = 0 is 0, same as when a = 1. */
unsigned int unsigned_long_long_bit_most_significant (unsigned long long a);

#endif
