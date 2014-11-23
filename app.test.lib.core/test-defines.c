#include <lib.core/defines.h>
#include <lib.core/defines-private.h>
#include <lib.core/error.h>
#include <lib.core/combinations.h>
#include <lib.core/memory.h>

#include "test-defines.h"

bool test_defines (Test *test)
{
	TITLE ();
	CATCH (SIZE_MAX > ULLONG_MAX);
	CATCH (SIZE_MAX - 1 != (SIZE_MAX / 2) * 2);
	CATCH (ULLONG_MAX - 1 != (ULLONG_MAX / 2) * 2);
	PASS ();
}

bool test_size_t_add (Test *test)
{
	size_t result;

	TITLE ();
	size_t_private_max (SIZE_MAX);
	CATCH (!size_t_add (0, 0, NULL));
	CATCH (error_count () != 0);
	CATCH (!size_t_add (1, 0, NULL));
	CATCH (!size_t_add (0, 1, NULL));
	CATCH (!size_t_add (SIZE_MAX / 2, 0, NULL));
	CATCH (!size_t_add (0, SIZE_MAX / 2, NULL));
	CATCH (!size_t_add ((SIZE_MAX / 2) + 1, (SIZE_MAX / 2), NULL));
	CATCH (!size_t_add ((SIZE_MAX / 2), (SIZE_MAX / 2) + 1, NULL));
	CATCH (!size_t_add (0, SIZE_MAX, NULL));
	CATCH (!size_t_add (SIZE_MAX, 0, NULL));
	CATCH (size_t_add (1, SIZE_MAX, NULL));
	CATCH (size_t_add (SIZE_MAX, 1, NULL));
	CATCH (size_t_add (SIZE_MAX / 2, SIZE_MAX, NULL));
	CATCH (size_t_add (SIZE_MAX, SIZE_MAX / 2, NULL));
	CATCH (size_t_add ((SIZE_MAX / 2) + 1, SIZE_MAX, NULL));
	CATCH (size_t_add (SIZE_MAX, (SIZE_MAX / 2) + 1, NULL));
	CATCH (size_t_add (SIZE_MAX - 1, SIZE_MAX, NULL));
	CATCH (size_t_add (SIZE_MAX, SIZE_MAX - 1, NULL));
	CATCH (size_t_add (SIZE_MAX, SIZE_MAX, NULL));
	result = 1;
	CATCH (!size_t_add (1, 2, &result));
	CATCH (result != 3);
	CATCH (size_t_add (1, SIZE_MAX, &result));
	CATCH (result != 3);
	PASS ();
}

bool test_size_t_mul (Test *test)
{
	size_t result;

	TITLE ();
	CATCH (!size_t_mul (0, 0, NULL));
	CATCH (error_count () != 0);
	CATCH (!size_t_mul (1, 0, NULL));
	CATCH (!size_t_mul (0, 1, NULL));
	CATCH (!size_t_mul (1, 1, NULL));
	CATCH (!size_t_mul (0, SIZE_MAX, NULL));
	CATCH (!size_t_mul (SIZE_MAX, 0, NULL));
	CATCH (!size_t_mul (1, SIZE_MAX, NULL));
	CATCH (!size_t_mul (SIZE_MAX, 1, NULL));
	CATCH (!size_t_mul (SIZE_MAX / 2, 2, NULL));
	CATCH (!size_t_mul (2, SIZE_MAX / 2, NULL));
	CATCH (size_t_mul ((SIZE_MAX / 2) + 1, 2, NULL));
	CATCH (size_t_mul (2, (SIZE_MAX / 2) + 1, NULL));
	CATCH (size_t_mul (SIZE_MAX / 2, SIZE_MAX / 2, NULL));
	CATCH (size_t_mul (SIZE_MAX / 2, SIZE_MAX / 2, NULL));
	CATCH (size_t_mul (SIZE_MAX, SIZE_MAX, NULL));
	CATCH (size_t_mul (SIZE_MAX, SIZE_MAX, NULL));
	result = 1;
	CATCH (!size_t_mul (2, 3, &result));
	CATCH (result != 6);
	CATCH (size_t_mul (2, SIZE_MAX, &result));
	CATCH (result != 6);
	PASS ();
}

bool test_size_t_pow_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (size_t_pow (0, 0, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_size_t_pow_function_call_1 (Test *test)
{
        size_t value;

        TITLE ();
        memory_commit_limit (0);
        CATCH (size_t_pow (1, 1, &value));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_size_t_pow_function_call_2 (Test *test)
{
        size_t value;

        TITLE ();
        memory_total_create_limit (150);
        CATCH (size_t_pow (1, 1, &value));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_size_t_pow (Test *test)
{
        size_t value;

        TITLE ();

        CATCH (!size_t_pow (0, 0, &value));
        CATCH (value != 1);
        CATCH (!size_t_pow (0, 1, &value));
        CATCH (value != 0);
        CATCH (!size_t_pow (1, SIZE_MAX, &value));
        CATCH (value != 1);
        CATCH (!size_t_pow (0, SIZE_MAX, &value));
        CATCH (value != 0);
        CATCH (size_t_pow (2, 64, &value));
        CATCH (!size_t_pow (2, 32, &value));
        CATCH (value != 4294967296);
        PASS ();
}

bool test_size_t_private_max (Test *test)
{
	unsigned int a, b, max;
	size_t computed;

	TITLE ();
	for (max = 1; max < 9; max++) {
		a = 0;
		b = 0;
		size_t_private_max (max - 1);
		do {
			if (a > max - 1 ||
			    b > max - 1 ||
			    a + b > max - 1) {
				CATCH (size_t_add (a, b, &computed));
			}
			else {
				CATCH (!size_t_add (a, b, &computed));
				CATCH (computed != a + b);
			}
			if (a > max - 1 ||
			    b > max - 1 ||
			    a * b > max - 1) {
				CATCH (size_t_mul (a, b, &computed));
			}
			else {
				CATCH (!size_t_mul (a, b, &computed));
				CATCH (computed != a * b);
			}
		} while (combinations_a_b (&a, &b, max, max));
	}
	PASS ();
}

bool test_unsigned_long_long_add (Test *test)
{
	unsigned long long result;

	TITLE ();
	unsigned_long_long_private_max (ULLONG_MAX);
	CATCH (!unsigned_long_long_add (0, 0, NULL));
	CATCH (error_count () != 0);
	CATCH (!unsigned_long_long_add (1, 0, NULL));
	CATCH (!unsigned_long_long_add (0, 1, NULL));
	CATCH (!unsigned_long_long_add (ULLONG_MAX / 2, 0, NULL));
	CATCH (!unsigned_long_long_add (0, ULLONG_MAX / 2, NULL));
	CATCH (!unsigned_long_long_add ((ULLONG_MAX / 2) + 1, (ULLONG_MAX / 2), NULL));
	CATCH (!unsigned_long_long_add ((ULLONG_MAX / 2), (ULLONG_MAX / 2) + 1, NULL));
	CATCH (!unsigned_long_long_add (0, ULLONG_MAX, NULL));
	CATCH (!unsigned_long_long_add (ULLONG_MAX, 0, NULL));
	CATCH (unsigned_long_long_add (1, ULLONG_MAX, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX, 1, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX / 2, ULLONG_MAX, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX, ULLONG_MAX / 2, NULL));
	CATCH (unsigned_long_long_add ((ULLONG_MAX / 2) + 1, ULLONG_MAX, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX, (ULLONG_MAX / 2) + 1, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX - 1, ULLONG_MAX, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX, ULLONG_MAX - 1, NULL));
	CATCH (unsigned_long_long_add (ULLONG_MAX, ULLONG_MAX, NULL));
	result = 1;
	CATCH (!unsigned_long_long_add (1, 2, &result));
	CATCH (result != 3);
	CATCH (unsigned_long_long_add (1, ULLONG_MAX, &result));
	CATCH (result != 3);
	PASS ();
}

bool test_unsigned_long_long_mul (Test *test)
{
	unsigned long long result;

	TITLE ();
	unsigned_long_long_private_max (ULLONG_MAX);
	CATCH (!unsigned_long_long_mul (0, 0, NULL));
	CATCH (error_count () != 0);
	CATCH (!unsigned_long_long_mul (1, 0, NULL));
	CATCH (!unsigned_long_long_mul (0, 1, NULL));
	CATCH (!unsigned_long_long_mul (1, 1, NULL));
	CATCH (!unsigned_long_long_mul (0, ULLONG_MAX, NULL));
	CATCH (!unsigned_long_long_mul (ULLONG_MAX, 0, NULL));
	CATCH (!unsigned_long_long_mul (1, ULLONG_MAX, NULL));
	CATCH (!unsigned_long_long_mul (ULLONG_MAX, 1, NULL));
	CATCH (!unsigned_long_long_mul (ULLONG_MAX / 2, 2, NULL));
	CATCH (!unsigned_long_long_mul (2, ULLONG_MAX / 2, NULL));
	CATCH (unsigned_long_long_mul ((ULLONG_MAX / 2) + 1, 2, NULL));
	CATCH (unsigned_long_long_mul (2, (ULLONG_MAX / 2) + 1, NULL));
	CATCH (unsigned_long_long_mul (ULLONG_MAX / 2, ULLONG_MAX / 2, NULL));
	CATCH (unsigned_long_long_mul (ULLONG_MAX / 2, ULLONG_MAX / 2, NULL));
	CATCH (unsigned_long_long_mul (ULLONG_MAX, ULLONG_MAX, NULL));
	CATCH (unsigned_long_long_mul (ULLONG_MAX, ULLONG_MAX, NULL));
	result = 1;
	CATCH (!unsigned_long_long_mul (2, 3, &result));
	CATCH (result != 6);
	CATCH (unsigned_long_long_mul (2, ULLONG_MAX, &result));
	CATCH (result != 6);
	PASS ();
}

bool test_unsigned_long_long_private_max (Test *test)
{
	unsigned int a, b, max;
	unsigned long long computed;

	TITLE ();
	for (max = 1; max < 9; max++) {
		a = 0;
		b = 0;
		unsigned_long_long_private_max (max - 1);
		do {
			if (a > max - 1 ||
			    b > max - 1 ||
			    a + b > max - 1) {
				CATCH (unsigned_long_long_add (a, b, &computed));
			}
			else {
				CATCH (!unsigned_long_long_add (a, b, &computed));
				CATCH (computed != a + b);
			}
			if (a > max - 1 ||
			    b > max - 1 ||
			    a * b > max - 1) {
				CATCH (unsigned_long_long_mul (a, b, &computed));
			}
			else {
				CATCH (!unsigned_long_long_mul (a, b, &computed));
				CATCH (computed != a * b);
			}
		} while (combinations_a_b (&a, &b, max, max));
	}
	PASS ();
}

bool test_unsigned_long_long_bit_get (Test *test)
{
	unsigned int i;

	TITLE ();
	CATCH (unsigned_long_long_bit_get (0, 0) != 0);
	CATCH (unsigned_long_long_bit_get (1, 0) != 1);
	CATCH (unsigned_long_long_bit_get (2, 0) != 0);
	CATCH (unsigned_long_long_bit_get (2, 1) != 1);
	for (i = 0; i < 64; i++) {
		CATCH (unsigned_long_long_bit_get ((unsigned long long)-1, i) != 1);
	}
	PASS ();
}

bool test_unsigned_long_long_bit_set (Test *test)
{
	unsigned long long a;
	unsigned int i;

	TITLE ();
	CATCH (unsigned_long_long_bit_set (0, 0, 1) != 1);
	CATCH (unsigned_long_long_bit_set (256, 8, 0) != 0);
	CATCH (unsigned_long_long_bit_set (0, 8, 1) != 256);
	CATCH (unsigned_long_long_bit_set (0, 63, 1) != 9223372036854775808ULL);
	for (a = 0, i = 0; i < 64; i++) {
		a = unsigned_long_long_bit_set (a, i, 1);
	}
	CATCH (a != (unsigned long long)-1);
	PASS ();
}

bool test_unsigned_long_long_bit_most_significant (Test *test)
{
	TITLE ();
	CATCH (unsigned_long_long_bit_most_significant (0) != 0);
	CATCH (unsigned_long_long_bit_most_significant (1) != 0);
	CATCH (unsigned_long_long_bit_most_significant (2) != 1);
	CATCH (unsigned_long_long_bit_most_significant (3) != 1);
	CATCH (unsigned_long_long_bit_most_significant (4) != 2);
	CATCH (unsigned_long_long_bit_most_significant ((unsigned char)-1) != 7);
	CATCH (unsigned_long_long_bit_most_significant ((unsigned long long)-1) != 63);
	PASS ();
}
