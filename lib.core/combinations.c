#include <lib.core/combinations.h>
#include <lib.core/error.h>

bool combinations_a_b (unsigned int *a, unsigned int *b, unsigned int a_max, unsigned int b_max)
{
	if (!a || !b) {
		error (InvalidArgument);
		return false;
	}
	if (*a > a_max || *b > b_max) {
		error (InvalidArgument);
		return false;
	}
	if (*a == a_max && *b == b_max) {
		error (InvalidArgument);
		return false;
	}
	if (*b == b_max) {
		*b = 0;
		*a += 1;
	}
	else {
		*b += 1;
	}
	return true;
}
