#include <stdio.h>

#include <lib.core/defines.h>
#include <lib.core/defines-private.h>
#include <lib.core/error.h>
#include <lib.core/object-private.h>
#include <lib.core/memory.h>
#include <lib.core/random.h>
#include <lib.core/tree-private.h>
#include <lib.core/file-private.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.core/print.h>
#include <lib.app/arguments.h>

#include "test.h"

static void print_catch (Test *test);
static void print_leak (Test *test);
static void print_pass (Test *test);
static void print_memory_size (void);
static bool test_memory_leak (Test *test);
static void reset (void);

bool test_array (int argc, char **argv, bool (*tests[]) (Test *test))
{
	Test test;
	Test test_memory;
	size_t test_count = 0;
	size_t test_count_ok = 0;
	size_t test_ok = 0;
	size_t test_memory_ok = 0;
        size_t i;
        AppArgument arguments[] = {
                ARGUMENT_DEFAULT,
                ARGUMENT_END
        };
        FILE *sh;
        char *path;
        char *path_pre_sh;
        char *path_post_sh;
        
        if (!(path = directory_current_path ())) {
                return EXIT_FAILURE;
        }
        if (!(path_pre_sh = string_create (path))) {
                string_destroy (path);
                return EXIT_FAILURE;
        }
        if (!string_append (&path_pre_sh, "/pre.sh")) {
                string_destroy (path);
                string_destroy (path_pre_sh);
                return EXIT_FAILURE;
        }
        if (!(path_post_sh = string_create (path))) {
                string_destroy (path);
                string_destroy (path_pre_sh);
                return EXIT_FAILURE;
        }
        if (!string_append (&path_post_sh, "/post.sh")) {
                string_destroy (path);
                string_destroy (path_pre_sh);
                string_destroy (path_post_sh);
                return EXIT_FAILURE;
        }
        if (file_exists (path_pre_sh) && (sh = popen ("./pre.sh", "r"))) {
                pclose (sh);
        }
        if (!app_arguments (argc, argv, arguments)) {
                app_arguments_usage (argc, argv, arguments);
                string_destroy (path);
                string_destroy (path_pre_sh);
                string_destroy (path_post_sh);
                return EXIT_FAILURE;
        }
	while (tests[test_count]) {
		reset ();
		test.title = NULL;
		test.line_catch = 0;
		test_ok = tests[test_count] (&test);
		test_memory_ok = test_memory_leak (&test_memory);
        	if (test_ok && test_memory_ok) {
                        tests[test_count] = NULL;
		        test_count_ok++;
		        print_pass (&test);
        	}
		test_count++;
	}
        if (test_count_ok != test_count) {
                /* Rerun failed tests with errors printed. */
                printf ("\n");
                error_silent (false);
                for (i = 0; i < test_count; i++) {
                        if (!tests[i]) {
                                continue;
                        }
                        reset ();
                        print_silent (false);
                        test_ok = tests[i] (&test);
                        test_memory_ok = test_memory_leak (&test_memory);
                        if (test_ok) {
                                if (test_memory_ok) {
                                        print_pass (&test);
                                }
                                else {
                                        print_leak (&test);
                                        print_memory_size ();
                                }
                        }
                        else {
                                print_catch (&test);
                                print_memory_size ();
                        }
                }
        }
	if (test_count != 0) {
		if (test_count_ok == test_count) {
			printf ("Okay, good!\n");
		}
	}
	else {
		printf ("No tests!\n");
	}
        if (file_exists (path_post_sh) && (sh = popen ("./post.sh", "r"))) {
                pclose (sh);
        }
        string_destroy (path);
        string_destroy (path_pre_sh);
        string_destroy (path_post_sh);
        if (test_count == 0) {
                return false;
        }
	if (test_count == test_count_ok) {
                return true;
        }
        return false;
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
	printf ("(memory_commit_size: %llu)\n", memory_commit_size ());
	printf ("(memory_total_create_size: %llu)\n\n", memory_total_create_size ());
}

static void reset (void)
{
        size_t_private_max (SIZE_MAX);
        unsigned_long_long_private_max (ULLONG_MAX);
        memory_commit_limit (app_argument_default_object.memory_commit_limit);
        memory_total_create_limit (ULLONG_MAX);
        memory_total_reset ();
        if (random_is_open ()) {
                random_close ();
        }
        error_reset ();
        object_id_set_counter (0);
        tree_iterator_path_size (TREE_ITERATOR_PATH_SIZE);
        file_path_size (FILE_PATH_SIZE);
        print_silent (true);
}
