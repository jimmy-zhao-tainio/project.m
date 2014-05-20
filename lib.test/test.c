#include <stdio.h>

#include <lib.core/defines.h>
#include <lib.core/defines-private.h>
#include <lib.core/error.h>
#include <lib.core/object-private.h>
#include <lib.core/memory.h>
#include <lib.core/random.h>
#include <lib.core/tree-private.h>
#include <lib.core/file-private.h>

#include "test.h"

static void print_catch (Test *test);
static void print_leak (Test *test);
static void print_pass (Test *test);
static void print_memory_size (void);

static bool test_memory_leak (Test *test);

bool test_array (bool (*tests[]) (Test *test))
{
	Test test;
	Test test_memory;
	size_t test_count = 0;
	size_t test_count_ok = 0;
	size_t test_ok = 0;
	size_t test_memory_ok = 0;

	while (tests[test_count]) {
		size_t_private_max (SIZE_MAX);
		unsigned_long_long_private_max (ULLONG_MAX);
		memory_commit_limit (ULLONG_MAX);
		memory_total_create_limit (ULLONG_MAX);
		memory_total_reset ();
		random_close ();
		error_reset ();
		object_id_set_counter (0);
		tree_iterator_path_size (TREE_ITERATOR_PATH_SIZE);
		file_path_size (FILE_PATH_SIZE);
		test.title = NULL;
		test.line_catch = 0;
		test_ok = tests[test_count] (&test);
		test_memory_ok = test_memory_leak (&test_memory);
        	if (test_ok) {
			if (test_memory_ok) {
				test_count_ok++;
				print_pass (&test);
			}
			else {
				print_leak (&test);
				print_memory_size ();
			}
        	}
        	else {
			print_catch (&test);
			error_print ();
			print_memory_size ();
        	}
		test_count++;
	}
	if (test_count != 0) {
		if (test_count_ok == test_count) {
			printf ("Okay, good!\n");
		}
        	else {
			printf ("Good, try again!\n");
        	}
	}
	else {
		printf ("No tests!\n");
	}
	return test_count == 0 || test_count - test_count_ok != 0 ? 1 : 0;
}

static void print_catch (Test *test)
{
	printf ("\033[1;35m[CATCH]\033[0m %s: %lu\n", test->title, test->line_catch);
	fflush (stdout);
}

static void print_leak (Test *test)
{
	printf ("\033[1;36m[LEAK]\033[0m %s\n", test->title);
	fflush (stdout);
}

static void print_pass (Test *test)
{
	printf ("\033[1;32m[PASS]\033[0m %s\n", test->title);
	fflush (stdout);
}

bool test_memory_leak (Test *test)
{
	CATCH (memory_total_create_size () != memory_total_destroy_size ());
	CATCH (memory_commit_size () != 0);
	PASS ();
}

static void print_memory_size (void)
{
	if (memory_commit_limit_get () != (unsigned long long)-1) {
		printf ("\tmemory_commit_size: %llu\n", memory_commit_size ());
	}
	if (memory_total_create_limit_get () != (unsigned long long)-1) {
		printf ("\tmemory_total_create_size: %llu\n", memory_total_create_size ());
	}
}
