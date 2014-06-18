#include <lib.pattern/pattern-whitespace.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/defines-private.h>
#include <lib.core/error.h>

#include "test-pattern-whitespace.h"

bool test_pattern_whitespace_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (pattern_whitespace (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_pattern_whitespace_overflow (Test *test)
{
	TITLE ();
	size_t_private_max (1);
	CATCH (pattern_whitespace ("a"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	PASS ();
}

bool test_pattern_whitespace_1 (Test *test)
{	
	char *stripped;

	TITLE ();
	CATCH (!(stripped = pattern_whitespace (" ")));
	CATCH (!string_equals (stripped, ""));
	CATCH (memory_size (stripped) != 1);
	string_destroy (stripped);
	PASS ();
}

bool test_pattern_whitespace_2 (Test *test)
{
	char *stripped;

	TITLE ();
	CATCH (!(stripped = pattern_whitespace ("\\ ")));
	CATCH (!string_equals (stripped, "\\ "));
	CATCH (memory_size (stripped) != 3);
	string_destroy (stripped);
	PASS ();
}

bool test_pattern_whitespace_3 (Test *test)
{
	char *stripped;

	TITLE ();
	CATCH (!(stripped = pattern_whitespace ("a")));
	CATCH (!string_equals (stripped, "a"));
	CATCH (memory_size (stripped) != 2);
	string_destroy (stripped);
	PASS ();
}

bool test_pattern_whitespace_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + 1);
	CATCH (pattern_whitespace ("a"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	PASS ();
}
