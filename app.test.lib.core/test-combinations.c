#include <lib.core/combinations.h>
#include <lib.core/error.h>
#include <lib.core/string.h>

#include "test-combinations.h"

bool test_combinations_a_b_invalid_argument_1 (Test *test)
{
	unsigned int b;

	TITLE ();
	b = 0;
	CATCH (combinations_a_b (NULL, &b, 0, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "combinations_a_b"));
	PASS ();
}

bool test_combinations_a_b_invalid_argument_2 (Test *test)
{
	unsigned int a;

	TITLE ();
	a = 0;
	CATCH (combinations_a_b (&a, NULL, 0, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "combinations_a_b"));
	PASS ();
}

bool test_combinations_a_b_invalid_argument_3 (Test *test)
{
	unsigned int a;
	unsigned int b;

	TITLE ();
	a = 1;
	b = 0;
	CATCH (combinations_a_b (&a, &b, 0, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "combinations_a_b"));
	PASS ();
}

bool test_combinations_a_b_invalid_argument_4 (Test *test)
{
	unsigned int a;
	unsigned int b;

	TITLE ();
	a = 0;
	b = 1;
	CATCH (combinations_a_b (&a, &b, 0, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "combinations_a_b"));
	PASS ();
}

bool test_combinations_a_b_invalid_argument_5 (Test *test)
{
	unsigned int a;
	unsigned int b;

	TITLE ();
	a = 0;
	b = 0;
	CATCH (combinations_a_b (&a, &b, 0, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "combinations_a_b"));
	PASS ();
}

bool test_combinations_a_b (Test *test)
{
	unsigned int a;
	unsigned int b;

	TITLE ();
	a = 0;
	b = 0;
	CATCH (!combinations_a_b (&a, &b, 1, 0));
	CATCH (a != 1 || b != 0);
	a = 0;
	b = 0;
	CATCH (!combinations_a_b (&a, &b, 0, 1));
	CATCH (a != 0 || b != 1);
	a = 0;
	b = 0;
	CATCH (!combinations_a_b (&a, &b, 1, 1));
	CATCH (a != 0 || b != 1);
	CATCH (!combinations_a_b (&a, &b, 1, 1));
	CATCH (a != 1 || b != 0);
	CATCH (!combinations_a_b (&a, &b, 1, 1));
	CATCH (a != 1 || b != 1);
	a = 0;
	b = 0;
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 0 || b != 1);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 0 || b != 2);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 1 || b != 0);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 1 || b != 1);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 1 || b != 2);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 2 || b != 0);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 2 || b != 1);
	CATCH (!combinations_a_b (&a, &b, 2, 2));
	CATCH (a != 2 || b != 2);
	PASS ();
}
