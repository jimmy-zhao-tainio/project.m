#include <stdlib.h>
#include <stdio.h>

#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/file.h>
#include <lib.core/file-private.h>
#include <lib.core/error.h>

#include "test-file.h"

bool test_file_path_size_1 (Test *test)
{
	char *path;

	TITLE ();
	file_path_size (0);
	CATCH (!(path = directory_current_path ()));
	memory_destroy (path);
	PASS ();
}

bool test_file_path_size_2 (Test *test)
{
	TITLE ();
	file_path_size (1);
	CATCH (directory_current_path ());
	PASS ();
}

bool test_file_directory_current_path_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (1);
	CATCH (directory_current_path ());
	CATCH (error_count () != 3);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (2).error != ErrorMemoryCommitLimit);
	PASS ();
}

bool test_file_directory_current_path_system_call (Test *test)
{
	TITLE ();
	file_path_size (1);
	CATCH (directory_current_path ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorSystemCall);
	PASS ();
}

bool test_file_directory_current_path (Test *test)
{
	char *path;

	TITLE ();
	CATCH (!string_ends_with ((path = directory_current_path ()), "/project.m/app.test.lib.core"));
	string_destroy (path);
	CATCH (!file_path_is_valid ((path = directory_current_path ())));
	string_destroy (path);
	PASS ();
}

bool test_file_path_is_valid (Test *test)
{
	size_t i;
	char *path;

	TITLE ();
	CATCH (file_path_is_valid (NULL));
	CATCH (file_path_is_valid (""));
	CATCH (file_path_is_valid (" "));
	CATCH (file_path_is_valid (" /"));
	CATCH (file_path_is_valid ("//"));
	CATCH (file_path_is_valid ("/a/"));
	CATCH (file_path_is_valid ("a"));
	CATCH (file_path_is_valid ("a/"));
	CATCH (file_path_is_valid ("/a//b"));
	CATCH (!file_path_is_valid ("/"));
	CATCH (!file_path_is_valid ("/a"));
	CATCH (!file_path_is_valid ("/a/b"));
	CATCH (!file_path_is_valid ("/a/b/c"));
	CATCH (!file_path_is_valid ("/aa"));
	CATCH (!file_path_is_valid ("/aa/bbb"));
	CATCH (!file_path_is_valid ("/aa/bbb/cccc"));
	CATCH (!(path = string_create_with_size (FILE_PATH_SIZE)));
	path[0] = '/';
	for (i = 1; i < FILE_PATH_SIZE - 1; i++) {
		path[i] = 'a';
	}
	path[FILE_PATH_SIZE - 1] = '\0';
	CATCH (!file_path_is_valid (path));
	string_destroy (path);
	CATCH (!(path = string_create_with_size (FILE_PATH_SIZE + 1)));
	path[0] = '/';
	for (i = 1; i < FILE_PATH_SIZE; i++) {
		path[i] = 'a';
	}
	path[FILE_PATH_SIZE] = '\0';
	CATCH (file_path_is_valid (path));
	string_destroy (path);
	PASS ();
}

bool test_file_name_from_path_invalid_operation (Test *test)
{
	TITLE ();
	CATCH (file_name_from_path (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_file_name_from_path (Test *test)
{
	char *name;

	TITLE ();
	CATCH (!(name = file_name_from_path ("/")));
	CATCH (!string_equals (name, "/"));
	string_destroy (name);
	CATCH (!(name = file_name_from_path ("/a")));
	CATCH (!string_equals (name, "a"));
	string_destroy (name);
	CATCH (!(name = file_name_from_path ("/a/bb")));
	CATCH (!string_equals (name, "bb"));
	string_destroy (name);
	CATCH (!(name = file_name_from_path ("/a/bb/ccc.dddd")));
	CATCH (!string_equals (name, "ccc.dddd"));
	string_destroy (name);
	PASS ();
}

bool test_file_directory_sub_path_invalid_operation (Test *test)
{
	TITLE ();
	CATCH (directory_sub_path (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_file_directory_sub_path_function_call (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + 1);
	CATCH (directory_sub_path ("/"));
	CATCH (error_count () != 3);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (2).error != ErrorMemoryCommitLimit);
	PASS ();
}

bool test_file_directory_sub_path (Test *test)
{
	char *path;

	TITLE ();
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/")), "/"), string_destroy (path)); 
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/a")), "/"), string_destroy (path)); 
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/a/b")), "/a"), string_destroy (path)); 
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/a/b/c")), "/a/b"), string_destroy (path));
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/aa")), "/"), string_destroy (path)); 
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/aa/bbb")), "/aa"), string_destroy (path)); 
	string_destroy (path);
	CATCH_CLEAN (!string_equals ((path = directory_sub_path ("/aa/bbb/cccc")), "/aa/bbb"), string_destroy (path)); 
	string_destroy (path);
	PASS ();
}

bool test_file_directory_open_invalid_operation (Test *test)
{
	TITLE ();
	CATCH (directory_open (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();	
}

bool test_file_directory_open_system_call (Test *test)
{
	TITLE ();
	CATCH (directory_open ("/dev/null/fail"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorSystemCall);
	PASS ();	
}

bool test_file_directory_open_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Directory) - 1);
	CATCH (directory_open ("/"));
	CATCH (error_count () != 2);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 148);
	CATCH (error_at (1).error != ErrorMemoryCommitLimit);
	PASS ();	
}

bool test_file_directory_open_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Directory) +
				 sizeof (size_t) + 1);
	CATCH (directory_open ("/"));
	CATCH (error_count () != 3);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 154);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (2).error != ErrorMemoryCommitLimit);
	PASS ();	
}

bool test_file_directory_open_function_call_3 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Directory) +
				 sizeof (size_t) + 2 +
				 sizeof (size_t) + 1);
	CATCH (directory_open ("/"));
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 160);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (3).error != ErrorMemoryCommitLimit);
	PASS ();
}

bool test_file_directory_open (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!(directory = directory_open (path)));
	CATCH (!string_equals (directory->path, path));
	CATCH (!string_equals (directory->name, "app.test.lib.core"));
	CATCH (directory->directories);
	CATCH (directory->files);
	CATCH (directory->modified == 0);
	directory_close (directory);
	string_destroy (path);
	PASS ();
}

bool test_file_directory_open_current_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (0);
	CATCH (directory_open_current ());
	CATCH (error_count () != 4);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (3).error != ErrorMemoryCommitLimit);
	PASS ();	
}

bool test_file_directory_open_current_2 (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	path = directory_current_path ();
	CATCH (!(directory = directory_open_current ()));
	CATCH (!string_equals (directory->path, path));
	CATCH (!string_equals (directory->name, "app.test.lib.core"));
	CATCH (directory->directories);
	CATCH (directory->files);
	CATCH (directory->modified == 0);
	directory_close (directory);
	string_destroy (path);
	PASS ();
}

bool test_file_directory_close_invalid_argument (Test *test)
{
	TITLE ();
	directory_close (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_file_directory_close (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!(directory = directory_open_current ()));
	directory_close (directory);
	string_destroy (path);
	PASS ();
}

bool test_file_directory_read_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (directory_read (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_file_directory_read_1 (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/empty"));
        /* d stage/empty */
	CATCH (!(directory = directory_open (path)));
	CATCH (!directory_read (directory));
	CATCH (!directory->directories);
	CATCH (!directory->files);
	directory_close (directory);
	string_destroy (path);
	PASS ();
}

bool test_file_directory_read_2 (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/1d_1f"));
        /*
                d stage/1d_1f
                f f1
                d stage/1d_1f/d1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!directory->directories);
	CATCH (!directory->files);
	CATCH (directory->directories->count != 1);
	CATCH (directory->files->count != 1);
	directory_close (directory);
	PASS ();
}

bool test_file_directory_read_3 (Test *test)
{
	char *path;
	Directory *directory;
	List *directories;
	List *files;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/1d_1f"));
        /*
                d stage/1d_1f
                f f1
                d stage/1d_1f/d1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!directory->directories);
	CATCH (!directory->files);
	CATCH (directory->directories->count != 1);
	CATCH (directory->files->count != 1);
	directories = directory->directories;
	files = directory->files;
	CATCH (!directory_read (directory));
	CATCH (directories != directory->directories);
	CATCH (files != directory->files);
	directory_close (directory);
	PASS ();
}

bool test_file_directory_read_4 (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/9d_9f"));
        /*
                d stage/9d_9f/d8
                d stage/9d_9f/d7
                d stage/9d_9f/d1
                d stage/9d_9f/d4
                d stage/9d_9f/d9
                d stage/9d_9f/d2
                d stage/9d_9f/d5
                d stage/9d_9f/d3
                d stage/9d_9f/d6
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!directory->directories);
	CATCH (!directory->files);
	CATCH (directory->directories->count != 9);
	CATCH (directory->files->count != 9);
	directory_close (directory);
	PASS ();
}

bool test_file_directory_read_5 (Test *test)
{
	char *path;
	Directory *d3_1f;
	Directory *d1;
	Directory *d2;
	Directory *d3;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/d3_1f"));
        /*
                d stage/d3_1f
                d stage/d3_1f/d1
                f f1
                d stage/d3_1f/d1/d2
                d stage/d3_1f/d1/d2/d3
                f f1
         */
	CATCH (!(d3_1f = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (d3_1f));
	CATCH (!d3_1f->directories);
	CATCH (d3_1f->directories->count != 1);
	CATCH (!(d1 = d3_1f->directories->first->data));
	CATCH (!directory_read (d1));
	CATCH (d1->files->count != 1);
	CATCH (d1->directories->count != 1);
	CATCH (!(d2 = d1->directories->first->data));
	CATCH (!directory_read (d2));
	CATCH (d2->directories->count != 1);
	CATCH (!(d3 = d2->directories->first->data));
	CATCH (!directory_read (d3));
	CATCH (d3->files->count != 1);
	directory_close (d3_1f);
	PASS ();
}

bool test_file_directory_read_6 (Test *test)
{
	char *path;
	Directory *d3_1f;
	Directory *d1;
	Directory *d2;
	Directory *d3;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/d3_1f"));
        /*
                d stage/d3_1f
                d stage/d3_1f/d1
                f f1
                d stage/d3_1f/d1/d2
                d stage/d3_1f/d1/d2/d3
                f f1
         */
	CATCH (!(d3_1f = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (d3_1f));
	CATCH (!d3_1f->directories);
	CATCH (d3_1f->directories->count != 1);
	CATCH (!(d1 = d3_1f->directories->first->data));
	CATCH (!directory_read (d1));
	CATCH (d1->files->count != 1);
	CATCH (d1->directories->count != 1);
	CATCH (!(d2 = d1->directories->first->data));
	CATCH (!directory_read (d2));
	CATCH (d2->directories->count != 1);
	CATCH (!(d3 = d2->directories->first->data));
	CATCH (!directory_read (d3));
	CATCH (d3->files->count != 1);
	directory_close (d3_1f);
	PASS ();
}

bool test_file_directory_find_invalid_argument (Test *test)
{
	char *path;
	Directory *directory;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/find"));
        /*
                d stage/find
                f f1
                d stage/find/d1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (directory_find_file (NULL, "file"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	CATCH (directory_find_directory (NULL, "directory"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	CATCH (directory_find_file (directory, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	CATCH (directory_find_directory (directory, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	error_reset ();
	directory_close (directory);
	PASS ();
}

bool test_file_directory_find (Test *test)
{
	char *path;
	Directory *directory;
	Directory *found_directory;
	File *found_file;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/find"));
        /*
                d stage/find
                f f1
                d stage/find/d1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!directory->directories);
	CATCH (!directory->files);
	CATCH (directory->directories->count != 1);
	CATCH (directory->files->count != 1);
	CATCH (!(found_directory = directory_find_directory (directory, "d1")));
	CATCH (!string_equals (found_directory->name, "d1"));
	CATCH (directory_find_directory (directory, "d2"));
	CATCH (!(found_file = directory_find_file (directory, "f1")));
	CATCH (!string_equals (found_file->name, "f1"));
	CATCH (directory_find_file (directory, "f2"));
	directory_close (directory);
	PASS ();
}

bool test_file_open_fail (Test *test)
{
	Directory *directory;
	File *file;
	char *path;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/open_fail"));
        /* stage does not exist */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f1")));
	CATCH (file_open (file));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorSystemCall);
	error_reset ();
	CATCH (file_open (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	directory_close (directory);
	PASS ();
}

bool test_file_open_fail_multiple (Test *test)
{
	Directory *directory;
	File *file;
	char *path;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/open"));
        /*
                d stage/open
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f1")));
	CATCH (!file_open (file));
	CATCH (file_open (file));
	directory_close (directory);
	PASS ();
}

bool test_file_open (Test *test)
{
	Directory *directory;
	File *file;
	char *path;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/open"));
        /*
                d stage/open
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f1")));
	CATCH (!file_open (file));
	directory_close (directory);
	PASS ();
}

bool test_file_close (Test *test)
{
	Directory *directory;
	File *file;
	char *path;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/open"));
        /*
                d stage/open
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f1")));
	file_close (file);
	directory_close (directory);
	PASS ();
}

bool test_file_readline_f1 (Test *test)
{
	Directory *directory;
	File *file;
	char *path;
	char *line;
	size_t bytes_read;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/readline"));
        /*
                d stage/readline
                f f2
                f f3 \
                         \
                        0 \
                        AB \
                        012 \
                        ABCD \
                        01234 \
                        ABCD \
                        012 \
                        AB \
                        0
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f1")));
	CATCH (file_readline (NULL, NULL, NULL));
	CATCH (file_readline (file, NULL, NULL));
	CATCH (!(line = string_create_with_size (1)));
	CATCH (file_readline (file, line, NULL));
	CATCH (file_readline (file, line, &bytes_read));
	directory_close (directory);
	string_destroy (line);
	PASS ();
}

bool test_file_readline_f2 (Test *test)
{
	Directory *directory;
	File *file;
	char *path;
	char *line;
	size_t bytes_read;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/readline"));
        /*
                d stage/readline
                f f2
                f f3 \
                         \
                        0 \
                        AB \
                        012 \
                        ABCD \
                        01234 \
                        ABCD \
                        012 \
                        AB \
                        0
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	CATCH (!directory_read (directory));
	CATCH (!(file = directory_find_file (directory, "f2")));
	CATCH (!file_open (file));
	CATCH (!(line = string_create_with_size (1)));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 0);
	CATCH (memory_size (line) != 1);
	directory_close (directory);
	string_destroy (line);
	PASS ();
}

bool test_file_readline_f3 (Test *test)
{
	Directory *directory;
	File *file;
	char *path;
	char *line;
	size_t bytes_read;
	char *check[] = { 
		"",
		"0",
		"AB",
		"012",
		"ABCD",
		"01234",
		"ABCD",
		"012",
		"AB",
		"0",
	};

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/readline"));
        /*
                d stage/readline
                f f2
                f f3 \
                         \
                        0 \
                        AB \
                        012 \
                        ABCD \
                        01234 \
                        ABCD \
                        012 \
                        AB \
                        0
                f f1
         */
	CATCH (!(directory = directory_open (path)));
	string_destroy (path);
	directory_read (directory);
	CATCH (!(file = directory_find_file (directory, "f3")));
	CATCH (!file_open (file));
	CATCH (!(line = string_create_with_size (5)));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 1);
	CATCH (!string_equals (line, check[0]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 2);
	CATCH (!string_equals (line, check[1]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 3);
	CATCH (!string_equals (line, check[2]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 4);
	CATCH (!string_equals (line, check[3]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 5);
	CATCH (!string_equals (line, check[4]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 5);
	CATCH (!string_equals (line, "0123"));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 5);
	CATCH (!string_equals (line, check[6]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 4);
	CATCH (!string_equals (line, check[7]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 3);
	CATCH (!string_equals (line, check[8]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 2);
	CATCH (!string_equals (line, check[9]));
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 0);
	CATCH (!file_readline (file, line, &bytes_read));
	CATCH (bytes_read != 0);
	directory_close (directory);
	string_destroy (line);
	PASS ();
}

bool test_file_remove (Test *test)
{
	char *path;
	FILE *file;

	TITLE ();
	CATCH (file_remove (NULL));
	CATCH (file_remove ("badpathformat"));
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/remove/file"));
        /* d stage/remove */
	if (!file_exists (path)) {
		CATCH (!(file = fopen (path, "ab+")));
		fclose (file);
	}
	CATCH (!file_remove (path));
	CATCH (file_exists (path));
	string_destroy (path);
	PASS ();
}

bool test_file_exists (Test *test)
{
	char *path;

	TITLE ();
	CATCH (file_exists (NULL));
	CATCH (file_exists ("badpathformat"));
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/exists/12"));
        /* 
                d stage/exists
                f 1234 
        */
	CATCH (file_exists (path));
	CATCH (!string_append (&path, "34"));
	CATCH (!file_exists (path));
	string_destroy (path);
	PASS ();
}
