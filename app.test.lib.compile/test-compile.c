#include <stdlib.h>
#include <lib.core/string.h>
#include <lib.compile/compile.h>
#include <lib.compile/compile-print.h>

#include "test-compile.h"

bool test_compile_create_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_create (NULL, NULL));
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_create_not_app_or_lib (Test *test)
{
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "test_compile_create_bad_directory");
        /*
                d stage/test_compile_create_bad_directory
         */
	CATCH (compile_create (project, directory));
	directory_close (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_create_app_empty (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "app.test_compile_create");
        /*
                d stage/app.test_compile_create
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 0);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_create_lib_empty (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create");
        /*
                d stage/lib.test_compile_create
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 0);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_create_no_och_files (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create_no_och_files");
        /*
                d stage/lib.test_compile_create_no_och_files
                f file.3
                f file.1
                f file.2
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 3);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_create_h_files (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create_h_files");
        /*
                d stage/lib.test_compile_create_h_files
                f file2.h
                f file1.h
                f file3.h
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 3);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_create_c_files (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create_c_files");
        /*
                d stage/lib.test_compile_create_c_files
                f file2.c
                f file1.c
                f file3.c
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 6);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_create_co_files (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create_co_files");
        /*
                d stage/lib.test_compile_create_co_files
                f file2.c
                f file1.c
                f file3.c
         */
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 6);
	compile_destroy (compile);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_create_o_files (Test *test)
{
	Compile *compile;
	Directory *project;
	Directory *directory;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage");
	project = directory_open (path);
	directory_read (project);
	directory = directory_find_directory (project, "lib.test_compile_create_o_files");
        /*
                d stage/lib.test_compile_create_o_files
         */
	compile_print_set_enabled (false);
	CATCH (!(compile = compile_create (project, directory)));
	CATCH (!compile->libraries);
	CATCH (tree_count (compile->libraries) != 0);
	CATCH (!compile->files);
	CATCH (topological_count_vertices (compile->files) != 0);
	compile_destroy (compile);
	compile_print_set_enabled (true);
	directory_close (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_destroy_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	compile_destroy (NULL);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_prepare_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_prepare (NULL));
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_execute_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_execute (NULL, false));
	compile_print_set_enabled (true);
	PASS ();
}
