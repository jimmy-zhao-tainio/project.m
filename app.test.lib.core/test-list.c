#include <stdlib.h>
#include <stdio.h>

#include <lib.core/error.h>
#include <lib.core/list.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/defines-private.h>

#include "test-list.h"

bool test_list_create (Test *test)
{
	List *list;

	TITLE ();
	CATCH (!(list = list_create ()));
	list_destroy (list);
	PASS ();
}

bool test_list_destroy_invalid_argument (Test *test)
{
	TITLE ();
	list_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_list_destroy (Test *test)
{
	List *list;

	TITLE ();
	CATCH (!(list = list_create ()));
	list_destroy (list);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_list_insert_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (list_insert (NULL, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_list_insert_overflow (Test *test)
{
	List *list;

	TITLE ()
	CATCH (!(list = list_create ()));
	size_t_private_max (0);
	CATCH (list_insert (list, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	list_destroy (list);
	PASS ();
}

bool test_list_insert_function_call (Test *test)
{
	List *list;

	TITLE ()
	CATCH (!(list = list_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (list_insert (list, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	list_destroy (list);
	PASS ();
}

bool test_list_insert_1 (Test *test)
{
	List *list;
	ListNode *node;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(node = list_insert (list, NULL)));
	CATCH (list_first (list) != node);
	CATCH (list_last (list) != node);
	CATCH (list->count != 1);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	list_destroy (list);
	PASS ();
}

bool test_list_insert_2 (Test *test)
{
	List *list;
	ListNode *node;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(node = list_insert (list, "test")));
	CATCH (list_first (list) != node);
	CATCH (list_last (list) != node);
	CATCH (list->count != 1);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	CATCH (!string_equals (list_first (list)->data, "test"));
	list_destroy (list);
	PASS ();
}

bool test_list_insert_3 (Test *test)
{
	List *list;
	ListNode *nodes[10];
	ListNode *node;
	char *strings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	size_t i;

	TITLE ();
	CATCH (!(list = list_create ()));
	for (i = 0; i < 10; i++) {
		CATCH (!(nodes[i] = list_insert (list, strings[i])));
	}
	CATCH (list_first (list) != nodes[9]);
	CATCH (list_last (list) != nodes[0]);
	CATCH (list->count != 10);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	for (node = list_last (list), i = 0; i < 10; node = list_previous (node), i++) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	for (node = list_first (list), i = 10; i-- > 0; node = list_next (node)) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	list_destroy (list);
	PASS ();
}

bool test_list_append_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (list_append (NULL, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_list_append_overflow (Test *test)
{
	List *list;

	TITLE ()
	CATCH (!(list = list_create ()));
	size_t_private_max (0);
	CATCH (list_append (list, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	list_destroy (list);
	PASS ();
}

bool test_list_append_function_call (Test *test)
{
	List *list;

	TITLE ()
	CATCH (!(list = list_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (list_append (list, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	list_destroy (list);
	PASS ();
}
bool test_list_append_1 (Test *test)
{
	List *list;
	ListNode *node;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(node = list_append (list, NULL)));
	CATCH (list_first (list) != node);
	CATCH (list_last (list) != node);
	CATCH (list->count != 1);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	list_destroy (list);
	PASS ();
}

bool test_list_append_2 (Test *test)
{
	List *list;
	ListNode *node;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(node = list_append (list, "test")));
	CATCH (list_first (list) != node);
	CATCH (list_last (list) != node);
	CATCH (list->count != 1);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	CATCH (!string_equals (list_first (list)->data, "test"));
	list_destroy (list);
	PASS ();
}

bool test_list_append_3 (Test *test)
{
	List *list;
	ListNode *nodes[10];
	ListNode *node;
	char *strings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	size_t i;

	TITLE ();
	CATCH (!(list = list_create ()));
	for (i = 0; i < 10; i++) {
		CATCH (!(nodes[i] = list_append (list, strings[i])));
	}
	CATCH (list_first (list) != nodes[0]);
	CATCH (list_last (list) != nodes[9]);
	CATCH (list->count != 10);
	CATCH (list_previous (list_first (list)));
	CATCH (list_next (list_last (list)));
	for (node = list_last (list), i = 10; i-- > 0; node = list_previous (node)) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	for (node = list_first (list), i = 0; i < 10; node = list_next (node), i++) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	list_destroy (list);
	PASS ();
}

bool test_list_position_invalid_argument_1 (Test *test)
{
	List *list;
	ListNode *a, *b;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(a = list_insert (list, "a")));
	CATCH (!(b = list_insert (list, "b")));
	CATCH (list_position (NULL, a, b));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	CATCH (list_position (list, NULL, b));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	CATCH (list_position (list, a, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	list_destroy (list);
	PASS ();
}

bool test_list_position_invalid_argument_2 (Test *test)
{
	List *list;
	ListNode *a;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(a = list_insert (list, "a")));
	CATCH (list_position (list, a, a));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	list_destroy (list);
	PASS ();
}

bool test_list_position_1 (Test *test)
{
	List *list;
	ListNode *one;
	ListNode *two;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(one = list_insert (list, "one")));
	CATCH (!(two = list_append (list, "two")));
	CATCH (list_first (list) != one);
	CATCH (list_last (list) != two);
	CATCH (!list_position (list, one, two));
	CATCH (list_first (list) != one);
	CATCH (list_last (list) != two);
	CATCH (!list_position (list, two, one));
	CATCH (list_first (list) != two);
	CATCH (list_last (list) != one);
	CATCH (!list_position (list, one, two));
	CATCH (list_first (list) != one);
	CATCH (list_last (list) != two);
	list_destroy (list);
	PASS ();
}

bool test_list_position_2 (Test *test)
{
	List *list;
	ListNode *nodes[10];
	ListNode *node;
	char *strings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	size_t i;

	TITLE ();
	CATCH (!(list = list_create ()));
	for (i = 0; i < 10; i++) {
		CATCH (!(nodes[i] = list_append (list, strings[i])));
	}
	/* 0123456789
	 * 1234567890
	 * 2345678901
	 * 3456789012
	 * 4567890123
	 * 5678901234
	 * 6789012345
	 * 7890123456
	 * 8901234567
	 * 9012345678
	 * 0123456789
	 */
	for (i = 0; i < 10; i++) {
		CATCH (!list_position (list, list_last (list), list_first (list)));
	}
	for (node = list_first (list), i = 0; i < 10; node = list_next (node), i++) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	/* 0123456789
	 * 0123456798
	 * 0123456987
	 * 0123459876
	 * 0123498765
	 * 0123987654
	 * 0129876543
	 * 0198765432
	 * 0987654321
	 * 9876543210
	 */
	for (i = 9; i-- > 0;) {
		CATCH (!list_position (list, list_last (list), nodes[i]));
	}
	for (node = list_last (list), i = 0; i < 10; node = list_previous (node), i++) {
		CATCH (node != nodes[i]);
		CATCH (node->data != strings[i]);
	}
	list_destroy (list);
	PASS ();
}

bool test_list_remove_invalid_argument (Test *test)
{
	List *list;
	ListNode *node;	

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (!(node = list_insert (list, "node")));
	list_remove (NULL, node);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	list_remove (list, NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	list_destroy (list);
	PASS ();
}

bool test_list_remove (Test *test)
{
	List *list;
	ListNode *nodes[5];
	char *strings[5] = { "0", "1", "2", "3", "4" };
	size_t i;

	TITLE ();
	list_remove (NULL, NULL);
	CATCH (!(list = list_create ()));
	list_remove (list, NULL);
	for (i = 0; i < 5; i++) {
		CATCH (!(nodes[i] = list_append (list, strings[i])));
	}
	/* Remove 0 */
	list_remove (list, nodes[0]);
	CATCH (list_first (list) != nodes[1]);
	CATCH (list_next (list_first (list)) != nodes[2]);
	CATCH (list_next (list_next (list_first (list))) != nodes[3]);
	CATCH (list_next (list_next (list_next (list_first (list)))) != nodes[4]);
	CATCH (list_last (list) != nodes[4]);
	CATCH (list_count (list) != 4);
	/* Remove 2 */
	list_remove (list, nodes[2]);
	CATCH (list_first (list) != nodes[1]);
	CATCH (list_next (list_first (list)) != nodes[3]);
	CATCH (list_next (list_next (list_first (list))) != nodes[4]);
	CATCH (list_last (list) != nodes[4]);
	CATCH (list_count (list) != 3);
	/* Remove 4 */
	list_remove (list, nodes[4]);
	CATCH (list_first (list) != nodes[1]);
	CATCH (list_next (list_first (list)) != nodes[3]);
	CATCH (list_last (list) != nodes[3]);
	CATCH (list_count (list) != 2);
	/* Remove 1,3 */
	list_remove (list, nodes[1]);
	list_remove (list, nodes[3]);
	CATCH (list_first (list) != NULL);
	CATCH (list_last (list) != NULL);
	CATCH (list_count (list) != 0);
	list_destroy (list);
	PASS ();	
}

bool test_list_first (Test *test)
{
	List *list;
	ListNode *nodes[2];

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (list_first (list));
	CATCH (!(nodes[0] = list_insert (list, "0")));
	CATCH (list_first (list) != nodes[0]);
	CATCH (!(nodes[1] = list_insert (list, "1")));
	CATCH (list_first (list) != nodes[1]);
	list_destroy (list);
	PASS ();
}

bool test_list_last (Test *test)
{
	List *list;
	ListNode *nodes[2];

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (list_last (list));
	CATCH (!(nodes[0] = list_insert (list, "0")));
	CATCH (list_last (list) != nodes[0]);
	CATCH (!(nodes[1] = list_append (list, "1")));
	CATCH (list_last (list) != nodes[1]);
	list_destroy (list);
	PASS ();
}

bool test_list_count_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (list_count (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_list_count (Test *test)
{
	List *list;
	
	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (list_count (list) != 0);
	CATCH (!(list_append (list, "1")));
	CATCH (list_count (list) != 1);
	CATCH (!(list_append (list, "2")));
	CATCH (list_count (list) != 2);
	list_destroy (list);
	PASS ();
}

bool test_list_find_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (list_find (NULL, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_list_find (Test *test)
{
	List *list;
	ListNode *nodes[10];
	ListNode *node;
	char *strings[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	size_t i;

	TITLE ();
	CATCH (!(list = list_create ()));
	CATCH (list_find (list, NULL));
	for (i = 0; i < 10; i++) {
		CATCH (!(nodes[i] = list_append (list, strings[i])));
	}
	CATCH (!(node = list_find (list, strings[0])));
	CATCH (node->data != strings[0]);
	CATCH (!(node = list_find (list, strings[9])));
	CATCH (node->data != strings[9]);
	CATCH (!(node = list_find (list, strings[4])));
	CATCH (node->data != strings[4]);
	list_destroy (list);
	PASS ();
}
