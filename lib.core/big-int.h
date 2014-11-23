#ifndef big_int_h
#define big_int_h

#include <lib.core/defines.h>

typedef struct s_BigInt BigInt;

struct s_BigInt
{
        unsigned char *memory;
        size_t digits;
};

BigInt *big_int_create (uint64_t value);
bool big_int_set (BigInt *integer, uint64_t value);
bool big_int_add (BigInt *a, BigInt *b, BigInt *to);
bool big_int_mul (BigInt *a, BigInt *b, BigInt *to);
bool big_int_pow (uint64_t base, uint64_t exponent, BigInt *to);
bool big_int_copy (BigInt *a, BigInt *to);
bool big_int_equals_string (BigInt *integer, const char *string);
bool big_int_to_value (BigInt *a, uint64_t *value);
bool big_int_print (BigInt *integer);
void big_int_destroy (BigInt *integer);


#endif
