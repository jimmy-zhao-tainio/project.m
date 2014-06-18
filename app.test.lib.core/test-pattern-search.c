#include <stdio.h>
#include <lib.core/string.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/pattern-search.h>

#include "test-pattern-search.h"

bool test_pattern_search_create_function_call_1 (Test *test)
{
	const char *memory = "a";

	TITLE ();
	memory_commit_limit (0);
	CATCH (pattern_search_create ((const unsigned char *)memory, string_length (memory), "a", true));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_pattern_search_create_function_call_2 (Test *test)
{
	const char *memory = "a";

	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (PatternSearch));
	CATCH (pattern_search_create ((const unsigned char *)memory, string_length (memory), "a", true));
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_pattern_search_create_function_call_3 (Test *test)
{
	const char *memory = "a";

	TITLE ();
	memory_total_create_limit (sizeof (size_t) + sizeof (PatternSearch) +
				   sizeof (size_t) + 2);
	CATCH (pattern_search_create ((const unsigned char *)memory, string_length (memory), "a", true));
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 3);
	PASS ();
}

bool test_pattern_search_create_function_call_4 (Test *test)
{
	const char *memory = "a";

	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (PatternSearch));
	CATCH (pattern_search_create ((const unsigned char *)memory, string_length (memory), "a", false));
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 4);
	PASS ();
}

bool test_pattern_search_create_function_call_5 (Test *test)
{
	const char *memory = "a";

	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (PatternSearch) +
			     sizeof (size_t) + sizeof (List) +
			     sizeof (size_t) + sizeof (ListNode) +
			     sizeof (size_t) + sizeof (PatternTokenValue));
	CATCH (pattern_search_create ((const unsigned char *)memory, string_length (memory), "a", false));
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 5);
	PASS ();
}

bool test_pattern_search_create (Test *test)
{
	PatternSearch *search;
	const char *memory = "a";

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory) + 1, "a", true)));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_1 (Test *test)
{
	PatternSearch *search;
	const char *memory = "a";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory) + 1, "a", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 0 || to != 1);
	CATCH (pattern_search_next (search, &from, &to));
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_2 (Test *test)
{
	PatternSearch *search;
	const char *memory = "ab";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory) + 1, "a|b", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 0 || to != 1);
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 1 || to != 2);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_3 (Test *test)
{
	PatternSearch *search;
	const char *memory = "abc";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, 3, "a|b", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 0 || to != 1);
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 1 || to != 2);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_4 (Test *test)
{
	PatternSearch *search;
	const char *memory = "abc";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, 3, "(a|b){2}!<ab>", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 0 || to != 3);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_5 (Test *test)
{
	PatternSearch *search;
	const char *memory = "abcabc";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory), "(a|b){2}!<ab>", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 0 || to != 3);
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 3 || to != 6);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_6 (Test *test)
{
	PatternSearch *search;
	const char *memory = "aaaaaa";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory), "z", true)));
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_7 (Test *test)
{
	PatternSearch *search;
	const char *memory = "aaaaaz";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory) + 1, "z", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 5 || to != 6);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_8 (Test *test)
{
	PatternSearch *search;
	const char *memory = "aaa/*.**.***/aaa";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory), "/*(!*|**{-}!<*/>){-}*{-}/", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 3 || to != 13);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}

bool test_pattern_search_next_9 (Test *test)
{
	PatternSearch *search;
	const char *memory = "aaa<!-- this-is--a comment --->aaa";
	size_t from, to;

	TITLE ();
	CATCH (!(search = pattern_search_create ((const unsigned char *)memory, string_length (memory), "\\<\\!--(!-|(-{1-}!\\>)){-}!-{-}-{2-}\\>", true)));
	CATCH (!pattern_search_next (search, &from, &to));
	CATCH (from != 3 || to != 31);
	CATCH (pattern_search_next (search, &from, &to));
	pattern_search_destroy (search);
	PASS ();
}
