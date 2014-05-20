#include <lib.core/memory.h>
#include <lib.core/permute.h>
#include <lib.core/permute-private.h>
#include <lib.core/error.h>
#include <lib.core/string.h>

#include "test-permute.h"

bool test_permute_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (permute_create (0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "permute_create"));
	PASS ();
}

bool test_permute_create_overflow_1 (Test *test)
{
	TITLE ();
	CATCH (permute_create (SIZE_MAX));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "permute_create"));
	PASS ();
}

bool test_permute_create_overflow_2 (Test *test)
{
	TITLE ();
	CATCH (permute_create (SIZE_MAX / sizeof (size_t)));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "permute_create"));
	PASS ();
}

bool test_permute_create_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (Permute) - 1);
	CATCH (permute_create (1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (!string_equals (error_at (0).function, "permute_create"));
	PASS ();
}

bool test_permute_create_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (Permute) + (sizeof (size_t) * (1 + 1)) - 1);
	CATCH (permute_create (1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (!string_equals (error_at (0).function, "permute_create"));
	PASS ();
}

bool test_permute_create (Test *test)
{
	Permute *permute;

	TITLE ();
	CATCH (!(permute = permute_create (1)));
	CATCH (error_count () != 0);
	permute_destroy (permute);
	PASS ();
}

bool test_permute_destroy_invalid_argument (Test *test)
{
	TITLE ();
	permute_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "permute_destroy"));
	PASS ();
}

bool test_permute_destroy (Test *test)
{
	Permute *permute;

	TITLE ();
	CATCH (!(permute = permute_create (1)));
	permute_destroy (permute);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_permute_next_invalid_argument (Test *test)
{
	TITLE ();
	permute_next (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "permute_next"));
	PASS ();
}

bool test_permute_next_1 (Test *test)
{
	Permute *permute;
	const size_t *next;

	TITLE ();
	CATCH (!(permute = permute_create (1)));
	CATCH (!(next = permute_next (permute)));
	CATCH (next[0] != 1);
	CATCH (permute_next (permute));
	CATCH (error_count () != 0);
	permute_destroy (permute);
	PASS ();
}

bool test_permute_next_2 (Test *test)
{
	Permute *permute;
	const size_t *next;
	size_t i;
	size_t length = 2;
	size_t expected[2 * 2] = {
		1, 2,
		2, 1
	};
	size_t e = 0;

	TITLE ();
	CATCH (!(permute = permute_create (length)));
	while ((next = permute_next (permute))) {
		for (i = 0; i < length; i++) {
			CATCH (next[i] != expected[e++]);
		}
	}
	CATCH (e != 2 * 2);
	permute_destroy (permute);
	PASS ();
}

bool test_permute_next_3 (Test *test)
{
	Permute *permute;
	const size_t *next;
	size_t i;
	size_t length = 3;
	size_t expected[3 * 6] = {
		1, 2, 3,
		1, 3, 2,
		2, 1, 3,
		2, 3, 1,
		3, 1, 2,
		3, 2, 1
	};
	size_t e = 0;

	TITLE ();
	CATCH (!(permute = permute_create (length)));
	while ((next = permute_next (permute))) {
		for (i = 0; i < length; i++) {
			CATCH (next[i] != expected[e++]);
		}
	}
	CATCH (e != 3 * 6);
	permute_destroy (permute);
	PASS ();
}

bool test_permute_next_4 (Test *test)
{
	Permute *permute;
	const size_t *next;
	size_t i;
	size_t length = 4;
	size_t expected[4 * 24] = {
		1, 2, 3, 4,
		1, 2, 4, 3,
		1, 3, 2, 4,
		1, 3, 4, 2,
		1, 4, 2, 3,
		1, 4, 3, 2,
		2, 1, 3, 4,
		2, 1, 4, 3,
		2, 3, 1, 4,
		2, 3, 4, 1,
		2, 4, 1, 3,
		2, 4, 3, 1,
		3, 1, 2, 4,
		3, 1, 4, 2,
		3, 2, 1, 4,
		3, 2, 4, 1,
		3, 4, 1, 2,
		3, 4, 2, 1,
		4, 1, 2, 3,
		4, 1, 3, 2,
		4, 2, 1, 3,
		4, 2, 3, 1,
		4, 3, 1, 2,
		4, 3, 2, 1
	};
	size_t e = 0;

	TITLE ();
	CATCH (!(permute = permute_create (length)));
	while ((next = permute_next (permute))) {
		for (i = 0; i < length; i++) {
			CATCH (next[i] != expected[e++]);
		}
	}
	CATCH (e != 4 * 24);
	permute_destroy (permute);
	PASS ();
}

