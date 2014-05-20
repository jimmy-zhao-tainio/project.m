#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/defines-private.h>

#include "test-string.h"

bool test_string_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (string_create (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "string_create"));
	PASS ();	
}

bool test_string_create_overflow (Test *test)
{
	TITLE ();
	size_t_private_max (1);
	CATCH (string_create ("1"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "string_create"));
	PASS ();	
}

bool test_string_create_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + 1);
	CATCH (string_create ("1"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (!string_equals (error_at (0).function, "string_create"));
	PASS ();	
}

bool test_string_create_1 (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("")));
	CATCH (memory_size (string) != 1);
	CATCH (string[0] != '\0');
	CATCH (string_length (string) != 0);
	string_destroy (string);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_string_create_2 (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("2")));
	CATCH (memory_size (string) != 2);
	CATCH (string[0] != '2');
	CATCH (string[1] != '\0');
	CATCH (string_length (string) != 1);
	string_destroy (string);
	CATCH (error_count () != 0);
	PASS ();	
}

bool test_string_create_3 (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("123")));
	CATCH (memory_size (string) != 4);
	CATCH (string[3] != '\0');
	CATCH (string_length (string) != 3);
	string_destroy (string);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_string_create_with_size_invalid_operation (Test *test)
{
	TITLE ();
	CATCH (string_create_with_size (0));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (!string_equals (error_at (0).function, "string_create_with_size"));
	PASS ();	
}

bool test_string_create_with_size_function_call (Test *test)
{
	TITLE ();
	CATCH (string_create_with_size (SIZE_MAX - sizeof (size_t)));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (!string_equals (error_at (0).function, "string_create_with_size"));
	PASS ();	
}

bool test_string_create_with_size (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create_with_size (1)));
	CATCH (memory_size (string) != 1);
	CATCH (string[0] != '\0');
	CATCH (string_length (string) != 0);
	string_destroy (string);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_string_destroy_invalid_argument (Test *test)
{
	TITLE ();
	string_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_destroy (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("test 1 2 3 test 1 2 3 test 1 2 3");
	string_destroy (string);
	PASS ();
}

bool test_string_append_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (string_append (NULL, "c"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_append_invalid_argument_2 (Test *test)
{
	char *string = NULL;

	TITLE ();
	CATCH (string_append (&string, "c"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_append_invalid_argument_3 (Test *test)
{
	char *string = "string";

	TITLE ();
	CATCH (string_append (&string, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_append_overflow_1 (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("")));
	size_t_private_max (1);
	CATCH (string_append (&string, "12"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	string_destroy (string);
	PASS ();
}

bool test_string_append_overflow_2 (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("1234")));
	size_t_private_max (5);
	CATCH (string_append (&string, "5"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	string_destroy (string);
	PASS ();
}

bool test_string_append_function_call (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("1234")));
	memory_commit_limit (sizeof (size_t) + 5);
	CATCH (string_append (&string, "5"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	string_destroy (string);
	PASS ();
}

bool test_string_append_1 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("");
	CATCH (!string_append (&string, ""));
	CATCH (memory_size (string) != 1);
	CATCH (string[0] != '\0');
	CATCH (string_length (string) != 0);
	string_destroy (string);
	PASS ();
}

bool test_string_append_2 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("");
	CATCH (!string_append (&string, "t"));
	CATCH (memory_size (string) != 2);
	CATCH (string[1] != '\0');
	CATCH (string_length (string) != 1);
	CATCH (!string_equals (string, "t"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_3 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("");
	CATCH (!string_append (&string, "test"));
	CATCH (memory_size (string) != 5);
	CATCH (string_length (string) != 4);
	CATCH (!string_equals (string, "test"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_4 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("t");
	CATCH (!string_append (&string, ""));
	CATCH (memory_size (string) != 2);
	CATCH (string[1] != '\0');
	CATCH (string_length (string) != 1);
	string_destroy (string);
	PASS ();
}

bool test_string_append_5 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("t");
	CATCH (!string_append (&string, "e"));
	CATCH (memory_size (string) != 3);
	CATCH (string[2] != '\0');
	CATCH (string_length (string) != 2);
	CATCH (!string_equals (string, "te"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_6 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("");
	CATCH (!string_append (&string, "t"));
	CATCH (!string_append (&string, "e"));
	CATCH (!string_append (&string, "s"));
	CATCH (!string_append (&string, "t"));
	CATCH (memory_size (string) != 5);
	CATCH (string[4] != '\0');
	CATCH (string_length (string) != 4);
	CATCH (!string_equals (string, "test"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_7 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("t");
	CATCH (!string_append (&string, "est"));
	CATCH (memory_size (string) != 5);
	CATCH (string[4] != '\0');
	CATCH (string_length (string) != 4);
	CATCH (!string_equals (string, "test"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_8 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("tes");
	CATCH (!string_append (&string, "t"));
	CATCH (memory_size (string) != 5);
	CATCH (string[4] != '\0');
	CATCH (string_length (string) != 4);
	CATCH (!string_equals (string, "test"));
	string_destroy (string);
	PASS ();
}

bool test_string_append_9 (Test *test)
{
	char *string;

	TITLE ();
	string = string_create ("te");
	CATCH (!string_append (&string, "st"));
	CATCH (memory_size (string) != 5);
	CATCH (string[4] != '\0');
	CATCH (string_length (string) != 4);
	CATCH (!string_equals (string, "test"));
	string_destroy (string);
	PASS ();
}

bool test_string_equals_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (string_equals ("", NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_equals_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (string_equals (NULL, ""));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_equals_1 (Test *test)
{
	TITLE ();
	CATCH (!string_equals ("", ""));
	PASS ();
}

bool test_string_equals_2 (Test *test)
{
	TITLE ();
	CATCH (string_equals ("", "test"));
	CATCH (string_equals ("test", ""));
	CATCH (string_equals ("t", "test"));
	CATCH (string_equals ("test", "t"));
	PASS ();
}

bool test_string_equals_3 (Test *test)
{
	TITLE ();
	CATCH (!string_equals ("test", "test"));
	CATCH (!string_equals ("t", "t"));
	PASS ();
}

bool test_string_begins_with_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (string_begins_with (NULL, ""));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_begins_with_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (string_begins_with ("", NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_begins_with (Test *test)
{
	TITLE ();
	CATCH (!string_begins_with ("", ""));
	CATCH (!string_begins_with ("test", "te"));
	CATCH (!string_begins_with ("test", "test"));
	CATCH (!string_begins_with ("t", "t"));
	CATCH (string_begins_with ("test", ""));
	CATCH (string_begins_with ("", "test"));
	CATCH (string_begins_with ("test", "test1"));
	PASS ();
}

bool test_string_ends_with_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (string_ends_with (NULL, ""));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_ends_with_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (string_ends_with ("", NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_ends_with (Test *test)
{
	TITLE ();
	CATCH (!string_ends_with ("", ""));
	CATCH (!string_ends_with ("test", "st"));
	CATCH (!string_ends_with ("test", "test"));
	CATCH (!string_ends_with ("t", "t"));
	CATCH (string_ends_with ("test", ""));
	CATCH (string_ends_with ("", "test"));
	CATCH (string_ends_with ("test", "test1"));
	PASS ();
}

bool test_string_index_invalid_argument_1 (Test *test)
{
	size_t index;

	TITLE ();
	CATCH (string_index (NULL, 'x', &index));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_index_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (string_index ("x", 'x', NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_index_overflow (Test *test)
{
	size_t index;

	TITLE ();
	size_t_private_max (1);
	CATCH (string_index ("x", 'x', &index));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	PASS ();
}

bool test_string_index (Test *test)
{
	size_t index;

	TITLE ();
	CATCH (string_index ("", '\0', &index));
	CATCH (string_index ("", 't', &index));
	CATCH (!string_index ("test1", 't', &index));
	CATCH (index != 0);
	CATCH (!string_index ("test1", '1', &index));
	CATCH (index != 4);
	PASS ();
}

static void destroy_array (char **array);

bool test_string_split_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (string_split (NULL, '/'));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_string_split_overflow (Test *test)
{
	TITLE ();
	size_t_private_max ((3 * sizeof (char *)) - 1);
	CATCH (string_split ("/", '/'));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (!string_equals (error_at (0).function, "string_split"));
	PASS ();
}

bool test_string_split_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit 
	(
		(3 * sizeof (char *)) + 
		sizeof (size_t) - 
		1
	);
	CATCH (string_split ("/", '/'));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 217);
	CATCH (!string_equals (error_at (0).function, "string_split"));
	PASS ();
}

bool test_string_split_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit 
	(
		(2 * sizeof (char *)) + 
		sizeof (size_t) + 
		sizeof (size_t)
	);
	CATCH (string_split ("", '/'));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 229);
	CATCH (!string_equals (error_at (0).function, "string_split"));
	PASS ();
}

bool test_string_split_1 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("", '/')));
	CATCH (!array[0] || array[1]); 
	CATCH (!string_equals (array[0], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_2 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a", '/')));
	CATCH (!array[0] || array[1]);
	CATCH (!string_equals (array[0], "a"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_3 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("aa", '/')));
	CATCH (!array[0] || array[1]);
	CATCH (!string_equals (array[0], "aa"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_4 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("aaa", '/')));
	CATCH (!array[0] || array[1]);
	CATCH (!string_equals (array[0], "aaa"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_5 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("/", '/')));
	CATCH (!array[0] || !array[1] || array[2]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_6 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("//", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], ""));
	CATCH (!string_equals (array[2], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_7 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("///", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || !array[3] || array[4]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], ""));
	CATCH (!string_equals (array[2], ""));
	CATCH (!string_equals (array[3], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_8 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("/a", '/')));
	CATCH (!array[0] || !array[1] || array[2]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], "a"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_9 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a/", '/')));
	CATCH (!array[0] || !array[1] || array[2]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_10 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("/a/", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], "a"));
	CATCH (!string_equals (array[2], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_11 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a/b", '/')));
	CATCH (!array[0] || !array[1] || array[2]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], "b"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_12 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("/a/b", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], "a"));
	CATCH (!string_equals (array[2], "b"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_13 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a/b/", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], "b"));
	CATCH (!string_equals (array[2], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_14 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("/a/b/c", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || !array[3] || array[4]);
	CATCH (!string_equals (array[0], ""));
	CATCH (!string_equals (array[1], "a"));
	CATCH (!string_equals (array[2], "b"));
	CATCH (!string_equals (array[3], "c"));	
	destroy_array (array);
	PASS ();
}

bool test_string_split_15 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a/b/c/", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || !array[3] || array[4]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], "b"));
	CATCH (!string_equals (array[2], "c"));
	CATCH (!string_equals (array[3], ""));
	destroy_array (array);
	PASS ();
}

bool test_string_split_16 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a/b/c", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], "b"));
	CATCH (!string_equals (array[2], "c"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_17 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("a//c", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || array[3]);
	CATCH (!string_equals (array[0], "a"));
	CATCH (!string_equals (array[1], ""));
	CATCH (!string_equals (array[2], "c"));
	destroy_array (array);
	PASS ();
}

bool test_string_split_18 (Test *test)
{
	char **array;

	TITLE ();
	CATCH (!(array = string_split ("aa/bb//cc", '/')));
	CATCH (!array[0] || !array[1] || !array[2] || !array[3] || array[4]);
	CATCH (!string_equals (array[0], "aa"));
	CATCH (!string_equals (array[1], "bb"));
	CATCH (!string_equals (array[2], ""));
	CATCH (!string_equals (array[3], "cc"));
	destroy_array (array);
	PASS ();
}

bool test_string_escape_c_null (Test *test)
{
	/*char *string = NULL;*/

	TITLE ();
	/*CATCH (string_escape_c (NULL));
	CATCH (string_escape_c (&string));*/
	CATCH (1);
}

bool test_string_unescape_c_null (Test *test)
{
	/*char *string = NULL;*/

	TITLE ();
	/*CATCH (string_unescape_c (NULL));
	CATCH (string_unescape_c (&string));*/
	CATCH (1);
}

bool test_string_escape_c (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("\"\n\r\n\n\r\\")));
	/*CATCH (!string_escape_c (&string));
	CATCH (!string_equals (string, "\\\"\\\n\\\r\n\\\n\r\\"));*/
	string_destroy (string);
	CATCH (1);
}

bool test_string_unescape_c (Test *test)
{
	char *string;

	TITLE ();
	CATCH (!(string = string_create ("\\\"\\\n\\\r\n\\\n\r\\")));
	/*CATCH (!string_unescape_c (&string));
	CATCH (!string_equals (string, "\"\n\r\n\n\r\\"));*/
	string_destroy (string);
	CATCH (1);
}

static void destroy_array (char **array)
{
	size_t i;

	if (!array) {
		return;
	}
	for (i = 0; array[i]; i++) {
		memory_destroy (array[i]);
	}
	memory_destroy (array);
}
