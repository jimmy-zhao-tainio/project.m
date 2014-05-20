#include <lib.core/defines.h>
#include <lib.core/defines-private.h>

static size_t PRIVATE_SIZE_MAX = SIZE_MAX;
static unsigned long long PRIVATE_ULLONG_MAX = ULLONG_MAX;

void size_t_private_max (size_t max)
{
	PRIVATE_SIZE_MAX = max;
}

bool size_t_add (size_t a, size_t b, size_t *result)
{
	if (a > PRIVATE_SIZE_MAX ||
	    b > PRIVATE_SIZE_MAX) {
		return false;
	}
	if (b > (PRIVATE_SIZE_MAX - a)) {
		return false;
	}
	if (result) {
		*result = a + b;
	}
	return true;
}

bool size_t_mul (size_t a, size_t b, size_t *result)
{
	if (a > PRIVATE_SIZE_MAX ||
	    b > PRIVATE_SIZE_MAX) {
		return false;
	}
	if (a != 0 && b > (PRIVATE_SIZE_MAX / a)) {
		return false;
	}
	if (result) {
		*result = a * b;
	}
	return true;
}

void unsigned_long_long_private_max (unsigned long long max)
{
	PRIVATE_ULLONG_MAX = max;
}

bool unsigned_long_long_add (unsigned long long a, unsigned long long b, unsigned long long *result)
{
	if (a > PRIVATE_ULLONG_MAX ||
	    b > PRIVATE_ULLONG_MAX) {
		return false;
	}
	if (b > (PRIVATE_ULLONG_MAX - a)) {
		return false;
	}
	if (result) {
		*result = a + b;
	}
	return true;
}

bool unsigned_long_long_mul (unsigned long long a, unsigned long long b, unsigned long long *result)
{
	if (a > PRIVATE_ULLONG_MAX ||
	    b > PRIVATE_ULLONG_MAX) {
		return false;
	}
	if (a != 0 && b > (PRIVATE_ULLONG_MAX / a)) {
		return false;
	}
	if (result) {
		*result = a * b;
	}
	return true;
}

bool unsigned_long_long_bit_get (unsigned long long a, unsigned int position)
{
	return a & (1ULL << position);
}

unsigned long long unsigned_long_long_bit_set (unsigned long long a, unsigned int position, bool value)
{
	if (value) {
		return a | (1ULL << position);
	}
	else {
		return a & ~(1ULL << position);
	}
}

/* Beware, the position returned for a = 0 is 0. */
unsigned int unsigned_long_long_bit_most_significant (unsigned long long a)
{
	unsigned int position = 0;

	while (a >>= 1) {
		position++;
	}
	return position;
}
