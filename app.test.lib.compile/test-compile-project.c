#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.compile/compile-project.h>
#include <lib.compile/compile-print.h>
#include <lib.compile/compile.h>

#include "test-compile-project.h"

bool test_compile_project_create_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_project_create (NULL));
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_create_invalid_path (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_project_create ("invalid"));
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_create_not_project (Test *test)
{
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	CATCH (compile_project_create (path));
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_create (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_create");
        /*
                d stage/project.test_compile_project_create
         */
	CATCH (!(project = compile_project_create (path)));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	CATCH (compile_project_prepare (NULL));
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_prepare_empty (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_empty");
        /*
                d stage/project.test_compile_project_prepare_empty
         */
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_lib (Test *test)
{
	CompileProject *project;
	Compile *lib;
	ListNode *node;
	CompileAction *action;
	char *path;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_lib");
        /*
                d stage/project.test_compile_project_prepare_lib
                d stage/project.test_compile_project_prepare_lib/lib.test
                f test.h
                        #ifndef test_h \
                        #define test_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c
                        #include <lib.test/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
         */
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (topological_count_vertices (project->topological) != 1);
	CATCH (project->sorted->count != 1);
	lib = project->sorted->first->data;
	CATCH (!lib->actions);
	CATCH (list_count (lib->actions) != 3);
	CATCH (!(node = list_first (lib->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test.a\n"));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_lib_and_app (Test *test)
{
	CompileProject *project;
	Compile *app;
	Compile *lib;
	Directory *app_directory;
	Directory *lib_directory;
	char *path;
	List *edges;
	ListNode *node;
	CompileAction *action;

	TITLE ();
	CATCH (!(path = directory_current_path ()));
	CATCH (!string_append (&path, "/stage/project.test_compile_project_prepare_lib_and_app"));
        /*
                d stage/project.test_compile_project_prepare_lib_and_app
                d stage/project.test_compile_project_prepare_lib_and_app/app.test
                f main.c \
                        #include <stdio.h> \
                         \
                        #include <lib.test/test.h> \
                         \
                        int main (int argc, char **argv) \
                        { \
                                test_test (); \
                                return 0; \
                        }
                d stage/project.test_compile_project_prepare_lib_and_app/lib.test
                f test.h \
                        #ifndef test_h \
                        #define test_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
         */
	CATCH (!(project = compile_project_create (path)));
	CATCH (!compile_project_prepare (project));
	CATCH (!(app_directory = directory_find_directory (project->directory, "app.test")));
	CATCH (!(lib_directory = directory_find_directory (project->directory, "lib.test")));
	CATCH (!(app = tree_search (project->directory_to_compile, (Object *)app_directory)));
	CATCH (!(lib = tree_search (project->directory_to_compile, (Object *)lib_directory)));
	CATCH (topological_count_vertices (project->topological) != 2);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)app)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib)));
	CATCH (list_count (edges) != 0);
	list_destroy (edges);
	CATCH (project->sorted->count != 2);
	CATCH (project->sorted->first->data != app);
	CATCH (project->sorted->last->data != lib);
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 2);
	CATCH (!(node = list_first (app->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] main.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_BUILD);
	CATCH (!string_equals (action->print, "[BUILD] app.test\n"));
	CATCH (!lib->actions);
	CATCH (list_count (lib->actions) != 3);
	CATCH (!(node = list_first (lib->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test.a\n"));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_libs_and_app (Test *test)
{
	CompileProject *project;
	Compile *app;
	Compile *lib1;
	Compile *lib2;
	Compile *lib3;
	Directory *app_directory;
	Directory *lib1_directory;
	Directory *lib2_directory;
	Directory *lib3_directory;
	ListNode *node;
	CompileAction *action;
	char *path;
	List *edges;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_libs_and_app");
        /*
                d stage/project.test_compile_project_prepare_libs_and_app
                d stage/project.test_compile_project_prepare_libs_and_app/lib.test2
                f test.h \
                        #ifndef test2_h \
                        #define test2_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test2/test.h> \
                        #include <lib.test3/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app/lib.test1
                f test.h \
                        #ifndef test1_h \
                        #define test1_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test1/test.h> \
                        #include <lib.test2/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app/app.test
                f main.c \
                        #include <stdio.h> \
                         \
                        #include <lib.test1/test.h> \
                         \
                        int main (int argc, char **argv) \
                        { \
                                test_test (); \
                                return 0; \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app/lib.test3
                f test.h \
                        #ifndef test3_h \
                        #define test3_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test3/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
         */
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!(app_directory = directory_find_directory (project->directory, "app.test")));
	CATCH (!(lib1_directory = directory_find_directory (project->directory, "lib.test1")));
	CATCH (!(lib2_directory = directory_find_directory (project->directory, "lib.test2")));
	CATCH (!(lib3_directory = directory_find_directory (project->directory, "lib.test3")));
	CATCH (!(app = tree_search (project->directory_to_compile, (Object *)app_directory)));
	CATCH (!(lib1 = tree_search (project->directory_to_compile, (Object *)lib1_directory)));
	CATCH (!(lib2 = tree_search (project->directory_to_compile, (Object *)lib2_directory)));
	CATCH (!(lib3 = tree_search (project->directory_to_compile, (Object *)lib3_directory)));
	CATCH (topological_count_vertices (project->topological) != 4);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)app)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib1);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib1)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib2);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib2)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib3);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib3)));
	CATCH (list_count (edges) != 0);
	list_destroy (edges);
	CATCH (project->sorted->count != 4);
	CATCH (project->sorted->first->data != app);
	CATCH (project->sorted->first->next->data != lib1);
	CATCH (project->sorted->first->next->next->data != lib2);
	CATCH (project->sorted->first->next->next->next->data != lib3);
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 2);
	CATCH (!(node = list_first (app->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] main.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_BUILD);
	CATCH (!string_equals (action->print, "[BUILD] app.test\n"));
	CATCH (!lib1->actions);
	CATCH (list_count (lib1->actions) != 3);
	CATCH (!(node = list_first (lib1->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test1.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test1.a\n"));
	CATCH (!lib2->actions);
	CATCH (list_count (lib2->actions) != 3);
	CATCH (!(node = list_first (lib2->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test2.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test2.a\n"));
	CATCH (!lib3->actions);
	CATCH (list_count (lib3->actions) != 3);
	CATCH (!(node = list_first (lib3->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test3.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test3.a\n"));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_libs_and_app_circle (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_libs_and_app_circle");
        /*
                d stage/project.test_compile_project_prepare_libs_and_app_circle
                d stage/project.test_compile_project_prepare_libs_and_app_circle/lib.test2
                f test.h \
                        #ifndef test2_h \
                        #define test2_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test2/test.h> \
                        #include <lib.test3/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app_circle/lib.test1
                f test.h \
                        #ifndef test1_h \
                        #define test1_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test1/test.h> \
                        #include <lib.test2/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app_circle/app.test
                f main.c \
                        #include <stdio.h> \
                         \
                        #include <lib.test1/test.h> \
                         \
                        int main (int argc, char **argv) \
                        { \
                                test_test (); \
                                return 0; \
                        }
                d stage/project.test_compile_project_prepare_libs_and_app_circle/lib.test3
                f test.h \
                        #ifndef test3_h \
                        #define test3_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test3/test.h> \
                        #include <lib.test1/test.h> \
                         \
                        void test_test (void) \
                        { \
                        }

         */
	project = compile_project_create (path);
	CATCH (compile_project_prepare (project));
	compile_project_destroy (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_prepare_app_indirect_lib (Test *test)
{
	CompileProject *project;
	Compile *app;
	Compile *lib1;
	Compile *lib2;
	Directory *app_directory;
	Directory *lib1_directory;
	Directory *lib2_directory;
	ListNode *node;
	CompileAction *action;
	char *path;
	List *edges;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_app_indirect_lib");
        /*
                d stage/project.test_compile_project_prepare_app_indirect_lib
                d stage/project.test_compile_project_prepare_app_indirect_lib/lib.test2
                f test.h \
                        #ifndef test2_h \
                        #define test2_h \
                         \
                        void test_test2 (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test2/test.h> \
                         \
                        void test_test2 (void) \
                        { \
                        }
                d stage/project.test_compile_project_prepare_app_indirect_lib/lib.test1
                f test.h \
                        #ifndef test1_h \
                        #define test1_h \
                         \
                        void test_test (void); \
                         \
                        #endif
                f test.c \
                        #include <lib.test1/test.h> \
                        #include <lib.test2/test.h> \
                         \
                        void test_test (void) \
                        { \
                                test_test2 (); \
                        }
                d stage/project.test_compile_project_prepare_app_indirect_lib/app.test
                f main.c \
                        #include <stdio.h> \
                         \
                        #include <lib.test1/test.h> \
                         \
                        int main (int argc, char **argv) \
                        { \
                                test_test (); \
                                return 0; \
                        }

         */
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!(app_directory = directory_find_directory (project->directory, "app.test")));
	CATCH (!(lib1_directory = directory_find_directory (project->directory, "lib.test1")));
	CATCH (!(lib2_directory = directory_find_directory (project->directory, "lib.test2")));
	CATCH (!(app = tree_search (project->directory_to_compile, (Object *)app_directory)));
	CATCH (!(lib1 = tree_search (project->directory_to_compile, (Object *)lib1_directory)));
	CATCH (!(lib2 = tree_search (project->directory_to_compile, (Object *)lib2_directory)));
	CATCH (topological_count_vertices (project->topological) != 3);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)app)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib1);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib1)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != lib2);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (project->topological, (Object *)lib2)));
	CATCH (list_count (edges) != 0);
	list_destroy (edges);
	CATCH (project->sorted->count != 3);
	CATCH (project->sorted->first->data != app);
	CATCH (project->sorted->first->next->data != lib1);
	CATCH (project->sorted->first->next->next->data != lib2);
	CATCH (tree_count (lib2->libraries) != 0);
	CATCH (tree_count (lib1->libraries) != 1);
	CATCH (tree_search (lib1->libraries, (Object *)lib2_directory) != lib2_directory);
	CATCH (tree_count (app->libraries) != 2);
	CATCH (tree_search (app->libraries, (Object *)lib1_directory) != lib1_directory);
	CATCH (tree_search (app->libraries, (Object *)lib2_directory) != lib2_directory);
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 2);
	CATCH (!(node = list_first (app->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] main.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_BUILD);
	CATCH (!string_equals (action->print, "[BUILD] app.test\n"));
	CATCH (!lib1->actions);
	CATCH (list_count (lib1->actions) != 3);
	CATCH (!(node = list_first (lib1->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test1.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test1.a\n"));
	CATCH (!lib2->actions);
	CATCH (list_count (lib2->actions) != 3);
	CATCH (!(node = list_first (lib2->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test2.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test2.a\n"));
	compile_project_destroy (project);
	string_destroy (path);
	PASS ();
}

bool test_compile_project_prepare_app_invalid_lib_include (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_prepare_app_invalid_lib_include");
        /*
                d stage/project.test_compile_project_prepare_app_invalid_lib_include
                d stage/project.test_compile_project_prepare_app_invalid_lib_include/app.test.lib.core
                f test-pattern.h \
                        #include <lib.test/test.h>
                d stage/project.test_compile_project_prepare_app_invalid_lib_include/lib.core


         */
	project = compile_project_create (path);
	CATCH (compile_project_prepare (project));
	compile_project_destroy (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_destroy_null (Test *test)
{
	TITLE ();
	compile_project_destroy (NULL);
	PASS ();
}

bool test_compile_project_execute_null (Test *test)
{
	TITLE ();
	compile_print_set_enabled (false);
	compile_project_execute (NULL, false);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_execute_lib (Test *test)
{
	CompileProject *project;
	char *path;
	char *libtest1a;
	char *test1o;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_lib");
        /*
                d stage/project.test_compile_project_execute_lib
                d stage/project.test_compile_project_execute_lib/lib.test1
                f test1.c \
                        #include \"test1.h\" \
                         \
                        int test1 (void) \
                        { \
                                return 0; \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
         */
	libtest1a = string_create (path);
	test1o = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&test1o, "/lib.test1/test1.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (test1o)) {
		CATCH (!file_remove (test1o));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (libtest1a));
	CATCH (!file_exists (test1o));
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (test1o);
	PASS ();
}

bool test_compile_project_execute_app_twice (Test *test)
{
	CompileProject *project;
	Compile *app;
	ListNode *node;
	CompileAction *action;
	char *path;
	char *test1o;
	char *apptest;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_app_twice");
        /*
                d stage/project.test_compile_project_execute_app_twice
                d stage/project.test_compile_project_execute_app_twice/app.test
                f test1.c \
                        #include <stdio.h> \
                         \
                        int main (void) \
                        { \
                                return 0; \
                        }
         */
	test1o = string_create (path);
	apptest = string_create (path);
	string_append (&test1o, "/app.test/test1.o");
	string_append (&apptest, "/app.test/app.test");
	if (file_exists (test1o)) {
		CATCH (!file_remove (test1o));
	}
	if (file_exists (apptest)) {
		CATCH (!file_remove (apptest));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (list_count (project->nodes) != 1);
	app = project->nodes->first->data;
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 2);
	CATCH (!(node = list_first (app->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test1.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_BUILD);
	CATCH (!string_equals (action->print, "[BUILD] app.test\n"));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (test1o));
	CATCH (!file_exists (apptest));
	compile_project_destroy (project);
	CATCH (!(project = compile_project_create (path)));
	CATCH (!compile_project_prepare (project));
	CATCH (list_count (project->nodes) != 1);
	app = project->nodes->first->data;
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 0);
	CATCH (!compile_project_execute (project, false));
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (test1o);
	string_destroy (apptest);
	PASS ();
}

bool test_compile_project_execute_lib_twice (Test *test)
{
	CompileProject *project;
	Compile *lib;
	CompileAction *action;
	ListNode *node;
	char *path;
	char *test1o;
	char *libtest;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_lib_twice");
        /*
                d stage/project.test_compile_project_execute_lib_twice
                d stage/project.test_compile_project_execute_lib_twice/lib.test
                f test1.c \
                        #include <stdio.h> \
                         \
                        int test1 (void) \
                        { \
                                return 0; \
                        }
         */
	test1o = string_create (path);
	libtest = string_create (path);
	string_append (&test1o, "/lib.test/test1.o");
	string_append (&libtest, "/lib.test/lib.test.a");
	if (file_exists (test1o)) {
		CATCH (!file_remove (test1o));
	}
	if (file_exists (libtest)) {
		CATCH (!file_remove (libtest));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (list_count (project->nodes) != 1);
	lib = project->nodes->first->data;
	CATCH (!lib->actions);
	CATCH (list_count (lib->actions) != 3);
	CATCH (!(node = list_first (lib->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test1.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test.a\n"));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (test1o));
	CATCH (!file_exists (libtest));
	compile_project_destroy (project);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	lib = project->nodes->first->data;
	CATCH (list_count (lib->actions) != 0);
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (test1o);
	string_destroy (libtest);
	PASS ();
}

bool test_compile_project_execute_lib_and_app_twice (Test *test)
{
	CompileProject *project;
	Compile *lib;
	Compile *app;
	CompileAction *action;
	ListNode *node;
	char *path;
	char *libtest1a;
	char *libtest1_test1o;
	char *apptest1;
	char *maino;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_lib_and_app_twice");
        /*
                d stage/project.test_compile_project_execute_lib_and_app_twice
                d stage/project.test_compile_project_execute_lib_and_app_twice/lib.test1
                f test1.c \
                        #include <lib.test1/test1.h> \
                         \
                        int test1 (void) \
                        { \
                                return 0; \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_lib_and_app_twice/app.test1
                f main.c \
                        #include <lib.test1/test1.h> \
                         \
                        int main (void) \
                        { \
                                return test1(); \
                        }
         */
	libtest1a = string_create (path);
	libtest1_test1o = string_create (path);
	apptest1 = string_create (path);
	maino = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&libtest1_test1o, "/lib.test1/test1.o");
	string_append (&apptest1, "/app.test1/app.test1");
	string_append (&maino, "/app.test1/main.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (libtest1_test1o)) {
		CATCH (!file_remove (libtest1_test1o));
	}
	if (file_exists (apptest1)) {
		CATCH (!file_remove (apptest1));
	}
	if (file_exists (maino)) {
		CATCH (!file_remove (maino));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	if (string_equals (((Compile *)project->nodes->first->data)->directory->name, "lib.test1")) {
		lib = project->nodes->first->data;
		app = project->nodes->first->next->data;
	}
	else {
		lib = project->nodes->first->next->data;
		app = project->nodes->first->data;
	}
	CATCH (!app->actions);
	CATCH (list_count (app->actions) != 2);
	CATCH (!(node = list_first (app->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] main.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_BUILD);
	CATCH (!string_equals (action->print, "[BUILD] app.test1\n"));
	CATCH (!lib->actions);
	CATCH (list_count (lib->actions) != 3);
	CATCH (!(node = list_first (lib->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test1.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test1.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test1.a\n"));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (libtest1a));
	CATCH (!file_exists (libtest1_test1o));
	CATCH (!file_exists (apptest1));
	CATCH (!file_exists (maino));
	compile_project_destroy (project);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	if (string_equals (((Compile *)project->nodes->first->data)->directory->name, "lib.test1")) {
		lib = project->nodes->first->data;
		app = project->nodes->first->next->data;
	}
	else {
		lib = project->nodes->first->next->data;
		app = project->nodes->first->data;
	}
	CATCH (list_count (lib->actions) != 0);
	CATCH (list_count (app->actions) != 0);
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (libtest1_test1o);
	string_destroy (apptest1);
	string_destroy (maino);
	PASS ();
}

bool test_compile_project_execute_lib_and_lib_twice (Test *test)
{
	CompileProject *project;
	Compile *lib1;
	Compile *lib2;
	CompileAction *action;
	ListNode *node;
	char *path;
	char *libtest1a;
	char *libtest1_test1o;
	char *libtest2a;
	char *libtest2_test2o;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_lib_and_lib_twice");
        /*
                d stage/project.test_compile_project_execute_lib_and_lib_twice
                d stage/project.test_compile_project_execute_lib_and_lib_twice/lib.test2
                f test2.c \
                        #include <lib.test1/test1.h> \
                         \
                        int test2 (void) \
                        { \
                                return test1(); \
                        }
                d stage/project.test_compile_project_execute_lib_and_lib_twice/lib.test1
                f test1.c \
                        #include <lib.test1/test1.h> \
                         \
                        int test1 (void) \
                        { \
                                return 0; \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
         */
	libtest1a = string_create (path);
	libtest1_test1o = string_create (path);
	libtest2a = string_create (path);
	libtest2_test2o = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&libtest1_test1o, "/lib.test1/test1.o");
	string_append (&libtest2a, "/lib.test2/lib.test2.a");
	string_append (&libtest2_test2o, "/lib.test2/test2.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (libtest1_test1o)) {
		CATCH (!file_remove (libtest1_test1o));
	}
	if (file_exists (libtest2a)) {
		CATCH (!file_remove (libtest2a));
	}
	if (file_exists (libtest2_test2o)) {
		CATCH (!file_remove (libtest2_test2o));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	if (string_equals (((Compile *)project->nodes->first->data)->directory->name, "lib.test1")) {
		lib1 = project->nodes->first->data;
		lib2 = project->nodes->first->next->data;
	}
	else {
		lib1 = project->nodes->first->next->data;
		lib2 = project->nodes->first->data;
	}
	CATCH (list_count (lib1->actions) != 3);
	CATCH (!(node = list_first (lib1->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test1.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test1.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test1.a\n"));
	CATCH (list_count (lib2->actions) != 3);
	CATCH (!(node = list_first (lib2->actions)));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_CC);
	CATCH (!string_equals (action->print, "[CC] test2.c\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_ARCH);
	CATCH (!string_equals (action->print, "[ARCH] lib.test2.a\n"));
	CATCH (!(node = node->next));
	CATCH (!(action = node->data));
	CATCH (action->type != COMPILE_ACTION_INDEX);
	CATCH (!string_equals (action->print, "[INDEX] lib.test2.a\n"));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (libtest1a));
	CATCH (!file_exists (libtest1_test1o));
	CATCH (!file_exists (libtest2a));
	CATCH (!file_exists (libtest2_test2o));
	compile_project_destroy (project);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	if (string_equals (((Compile *)project->nodes->first->data)->directory->name, "lib.test1")) {
		lib1 = project->nodes->first->data;
		lib2 = project->nodes->first->next->data;
	}
	else {
		lib1 = project->nodes->first->next->data;
		lib2 = project->nodes->first->data;
	}
	CATCH (list_count (lib1->actions) != 0);
	CATCH (list_count (lib2->actions) != 0);
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (libtest1_test1o);
	string_destroy (libtest2a);
	string_destroy (libtest2_test2o);
	PASS ();
}

bool test_compile_project_execute_2libs_and_1app (Test *test)
{
	CompileProject *project;
	char *path;
	char *libtest1a;
	char *libtest2a;
	char *libtest1_test1o;
	char *libtest1_test2o;
	char *libtest2_test3o;
	char *libtest2_test4o;
	char *apptest1;
	char *maino;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_2libs_and_1app");
        /*
                d stage/project.test_compile_project_execute_2libs_and_1app
                d stage/project.test_compile_project_execute_2libs_and_1app/lib.test2
                f test3.c \
                        #include \"test3.h\" \
                         \
                        int test3 (void) \
                        { \
                                return 0; \
                        }
                f test4.h \
                        #ifndef lib_test4_h \
                        #define lib_test4_h \
                         \
                        int test4 (void); \
                         \
                        #endif
                f test4.c \
                        #include \"test4.h\" \
                         \
                        int test4 (void) \
                        { \
                                return 0; \
                        }
                f test3.h \
                        #ifndef lib_test3_h \
                        #define lib_test3_h \
                         \
                        int test3 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_2libs_and_1app/lib.test1
                f test2.h \
                        #ifndef lib_test2_h \
                        #define lib_test2_h \
                         \
                        int test2 (void); \
                         \
                        #endif
                f test1.c \
                        #include \"test1.h\" \
                         \
                        int test1 (void) \
                        { \
                                return 0; \
                        }
                f test2.c \
                        #include \"test2.h\" \
                         \
                        int test2 (void) \
                        { \
                                return 0; \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_2libs_and_1app/app.test1
                f main.c \
                        #include <lib.test1/test1.h> \
                        #include <lib.test1/test2.h> \
                        #include <lib.test2/test3.h> \
                        #include <lib.test2/test4.h> \
                         \
                        int main (void) \
                        { \
                                return test1 () && test2 () && test3 () && test4 (); \
                        }
         */
	libtest1a = string_create (path);
	libtest2a = string_create (path);
	libtest1_test1o = string_create (path);
	libtest1_test2o = string_create (path);
	libtest2_test3o = string_create (path);
	libtest2_test4o = string_create (path);
	apptest1 = string_create (path);
	maino = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&libtest2a, "/lib.test2/lib.test2.a");
	string_append (&libtest1_test1o, "/lib.test1/test1.o");
	string_append (&libtest1_test2o, "/lib.test1/test2.o");
	string_append (&libtest2_test3o, "/lib.test2/test3.o");
	string_append (&libtest2_test4o, "/lib.test2/test4.o");
	string_append (&apptest1, "/app.test1/app.test1");
	string_append (&maino, "/app.test1/main.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (libtest2a)) {
		CATCH (!file_remove (libtest2a));
	}
	if (file_exists (libtest1_test1o)) {
		CATCH (!file_remove (libtest1_test1o));
	}
	if (file_exists (libtest1_test2o)) {
		CATCH (!file_remove (libtest1_test2o));
	}
	if (file_exists (libtest2_test3o)) {
		CATCH (!file_remove (libtest2_test3o));
	}
	if (file_exists (libtest2_test4o)) {
		CATCH (!file_remove (libtest2_test4o));
	}
	if (file_exists (apptest1)) {
		CATCH (!file_remove (apptest1));
	}
	if (file_exists (maino)) {
		CATCH (!file_remove (maino));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (libtest1a));
	CATCH (!file_exists (libtest2a));
	CATCH (!file_exists (libtest1_test1o));
	CATCH (!file_exists (libtest1_test2o));
	CATCH (!file_exists (libtest2_test3o));
	CATCH (!file_exists (libtest2_test4o));
	CATCH (!file_exists (apptest1));
	CATCH (!file_exists (maino));
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (libtest2a);
	string_destroy (libtest1_test1o);
	string_destroy (libtest1_test2o);
	string_destroy (libtest2_test3o);
	string_destroy (libtest2_test4o);
	string_destroy (apptest1);
	string_destroy (maino);
	PASS ();
}

bool test_compile_project_execute_3libs_and_2app (Test *test)
{
	CompileProject *project;
	char *path;
	char *libtest1a;
	char *libtest2a;
	char *libshareda;
	char *libtest1_test1o;
	char *libtest1_test2o;
	char *libtest2_test3o;
	char *libtest2_test4o;
	char *libshared_sharedo;
	char *apptest1;
	char *apptest1_maino;
	char *apptest2;
	char *apptest2_maino;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_3libs_and_2app");
        /*
                d stage/project.test_compile_project_execute_3libs_and_2app
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.shared
                f shared.c \
                        #include <lib.shared/shared.h> \
                         \
                        int shared (void) \
                        { \
                                return 0; \
                        }
                f shared.h \
                        #ifndef lib_shared_h \
                        #define lib_shared_h \
                         \
                        int shared (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.test2
                f test3.c \
                        #include \"test3.h\" \
                         \
                        int test3 (void) \
                        { \
                                return 0; \
                        }
                f test4.h \
                        #ifndef lib_test4_h \
                        #define lib_test4_h \
                         \
                        int test4 (void); \
                         \
                        #endif
                f test4.c \
                        #include \"test4.h\" \
                         \
                        int test4 (void) \
                        { \
                                return 0; \
                        }
                f test3.h \
                        #ifndef lib_test3_h \
                        #define lib_test3_h \
                         \
                        int test3 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.test1
                f test2.h \
                        #ifndef lib_test2_h \
                        #define lib_test2_h \
                         \
                        int test2 (void); \
                         \
                        #endif
                f test1.c \
                        #include \"test1.h\" \
                        #include <lib.shared/shared.h> \
                         \
                        int test1 (void) \
                        { \
                                return 0 + shared (); \
                        }
                f test2.c \
                        #include \"test2.h\" \
                        #include <lib.shared/shared.h> \
                         \
                        int test2 (void) \
                        { \
                                return 0 + shared (); \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/app.test2
                f main.c \
                        #include <lib.shared/shared.h> \
                         \
                        int main (void) \
                        { \
                                return shared (); \
                        }
                d stage/project.test_compile_project_execute_3libs_and_2app/app.test1
                f main.c \
                        #include <lib.test1/test1.h> \
                        #include <lib.test1/test2.h> \
                        #include <lib.test2/test3.h> \
                        #include <lib.test2/test4.h> \
                         \
                        int main (void) \
                        { \
                                return test1 () && test2 () && test3 () && test4 (); \
                        }
         */
	libtest1a = string_create (path);
	libtest2a = string_create (path);
	libshareda = string_create (path);
	libtest1_test1o = string_create (path);
	libtest1_test2o = string_create (path);
	libtest2_test3o = string_create (path);
	libtest2_test4o = string_create (path);
	libshared_sharedo = string_create (path);
	apptest1 = string_create (path);
	apptest1_maino = string_create (path);
	apptest2 = string_create (path);
	apptest2_maino = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&libtest2a, "/lib.test2/lib.test2.a");
	string_append (&libshareda, "/lib.shared/lib.shared.a");
	string_append (&libtest1_test1o, "/lib.test1/test1.o");
	string_append (&libtest1_test2o, "/lib.test1/test2.o");
	string_append (&libtest2_test3o, "/lib.test2/test3.o");
	string_append (&libtest2_test4o, "/lib.test2/test4.o");
	string_append (&libshared_sharedo, "/lib.shared/shared.o");
	string_append (&apptest1, "/app.test1/app.test1");
	string_append (&apptest1_maino, "/app.test1/main.o");
	string_append (&apptest2, "/app.test2/app.test2");
	string_append (&apptest2_maino, "/app.test2/main.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (libtest2a)) {
		CATCH (!file_remove (libtest2a));
	}
	if (file_exists (libshareda)) {
		CATCH (!file_remove (libshareda));
	}
	if (file_exists (libtest1_test1o)) {
		CATCH (!file_remove (libtest1_test1o));
	}
	if (file_exists (libtest1_test2o)) {
		CATCH (!file_remove (libtest1_test2o));
	}
	if (file_exists (libtest2_test3o)) {
		CATCH (!file_remove (libtest2_test3o));
	}
	if (file_exists (libtest2_test4o)) {
		CATCH (!file_remove (libtest2_test4o));
	}
	if (file_exists (libshared_sharedo)) {
		CATCH (!file_remove (libshared_sharedo));
	}
	if (file_exists (apptest1)) {
		CATCH (!file_remove (apptest1));
	}
	if (file_exists (apptest1_maino)) {
		CATCH (!file_remove (apptest1_maino));
	}
	if (file_exists (apptest2)) {
		CATCH (!file_remove (apptest2));
	}
	if (file_exists (apptest2_maino)) {
		CATCH (!file_remove (apptest2_maino));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute (project, false));
	CATCH (!file_exists (libtest1a));
	CATCH (!file_exists (libtest2a));
	CATCH (!file_exists (libshareda));
	CATCH (!file_exists (libtest1_test1o));
	CATCH (!file_exists (libtest1_test2o));
	CATCH (!file_exists (libtest2_test3o));
	CATCH (!file_exists (libtest2_test4o));
	CATCH (!file_exists (libshared_sharedo));
	CATCH (!file_exists (apptest1));
	CATCH (!file_exists (apptest1_maino));
	CATCH (!file_exists (apptest2));
	CATCH (!file_exists (apptest2_maino));
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (libtest2a);
	string_destroy (libshareda);
	string_destroy (libtest1_test1o);
	string_destroy (libtest1_test2o);
	string_destroy (libtest2_test3o);
	string_destroy (libtest2_test4o);
	string_destroy (libshared_sharedo);
	string_destroy (apptest1);
	string_destroy (apptest1_maino);
	string_destroy (apptest2);
	string_destroy (apptest2_maino);
	PASS ();
}

bool test_compile_project_execute_with_directory_name_null (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_create");
        /*
                empty project.directory
         */
	CATCH (!(project = compile_project_create (path)));
	CATCH (compile_project_execute_with_directory_name (project, NULL, false));
	compile_project_destroy (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_execute_with_directory_name_not_found (Test *test)
{
	CompileProject *project;
	char *path;

	TITLE ();
	compile_print_set_enabled (false);
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_3libs_and_2app");
	CATCH (!(project = compile_project_create (path)));
	CATCH (compile_project_execute_with_directory_name (project, "not_found", false));
	compile_project_destroy (project);
	string_destroy (path);
	compile_print_set_enabled (true);
	PASS ();
}

bool test_compile_project_execute_with_directory_name_lib (Test *test)
{
	CompileProject *project;
	char *path;
	char *libtest1a;
	char *libtest2a;
	char *libshareda;
	char *libtest1_test1o;
	char *libtest1_test2o;
	char *libtest2_test3o;
	char *libtest2_test4o;
	char *libshared_sharedo;
	char *apptest1;
	char *apptest1_maino;
	char *apptest2;
	char *apptest2_maino;

	TITLE ();
	path = directory_current_path ();
	string_append (&path, "/stage/project.test_compile_project_execute_3libs_and_2app");
        /*
                d stage/project.test_compile_project_execute_3libs_and_2app
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.shared
                f shared.c \
                        #include <lib.shared/shared.h> \
                         \
                        int shared (void) \
                        { \
                                return 0; \
                        }
                f shared.h \
                        #ifndef lib_shared_h \
                        #define lib_shared_h \
                         \
                        int shared (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.test2
                f test3.c \
                        #include \"test3.h\" \
                         \
                        int test3 (void) \
                        { \
                                return 0; \
                        }
                f test4.h \
                        #ifndef lib_test4_h \
                        #define lib_test4_h \
                         \
                        int test4 (void); \
                         \
                        #endif
                f test4.c \
                        #include \"test4.h\" \
                         \
                        int test4 (void) \
                        { \
                                return 0; \
                        }
                f test3.h \
                        #ifndef lib_test3_h \
                        #define lib_test3_h \
                         \
                        int test3 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/lib.test1
                f test2.h \
                        #ifndef lib_test2_h \
                        #define lib_test2_h \
                         \
                        int test2 (void); \
                         \
                        #endif
                f test1.c \
                        #include \"test1.h\" \
                        #include <lib.shared/shared.h> \
                         \
                        int test1 (void) \
                        { \
                                return 0 + shared (); \
                        }
                f test2.c \
                        #include \"test2.h\" \
                        #include <lib.shared/shared.h> \
                         \
                        int test2 (void) \
                        { \
                                return 0 + shared (); \
                        }
                f test1.h \
                        #ifndef lib_test1_h \
                        #define lib_test1_h \
                         \
                        int test1 (void); \
                         \
                        #endif
                d stage/project.test_compile_project_execute_3libs_and_2app/app.test2
                f main.c \
                        #include <lib.shared/shared.h> \
                         \
                        int main (void) \
                        { \
                                return shared (); \
                        }
                d stage/project.test_compile_project_execute_3libs_and_2app/app.test1
                f main.c \
                        #include <lib.test1/test1.h> \
                        #include <lib.test1/test2.h> \
                        #include <lib.test2/test3.h> \
                        #include <lib.test2/test4.h> \
                         \
                        int main (void) \
                        { \
                                return test1 () && test2 () && test3 () && test4 (); \
                        }
         */
	libtest1a = string_create (path);
	libtest2a = string_create (path);
	libshareda = string_create (path);
	libtest1_test1o = string_create (path);
	libtest1_test2o = string_create (path);
	libtest2_test3o = string_create (path);
	libtest2_test4o = string_create (path);
	libshared_sharedo = string_create (path);
	apptest1 = string_create (path);
	apptest1_maino = string_create (path);
	apptest2 = string_create (path);
	apptest2_maino = string_create (path);
	string_append (&libtest1a, "/lib.test1/lib.test1.a");
	string_append (&libtest2a, "/lib.test2/lib.test2.a");
	string_append (&libshareda, "/lib.shared/lib.shared.a");
	string_append (&libtest1_test1o, "/lib.test1/test1.o");
	string_append (&libtest1_test2o, "/lib.test1/test2.o");
	string_append (&libtest2_test3o, "/lib.test2/test3.o");
	string_append (&libtest2_test4o, "/lib.test2/test4.o");
	string_append (&libshared_sharedo, "/lib.shared/shared.o");
	string_append (&apptest1, "/app.test1/app.test1");
	string_append (&apptest1_maino, "/app.test1/main.o");
	string_append (&apptest2, "/app.test2/app.test2");
	string_append (&apptest2_maino, "/app.test2/main.o");
	if (file_exists (libtest1a)) {
		CATCH (!file_remove (libtest1a));
	}
	if (file_exists (libtest2a)) {
		CATCH (!file_remove (libtest2a));
	}
	if (file_exists (libshareda)) {
		CATCH (!file_remove (libshareda));
	}
	if (file_exists (libtest1_test1o)) {
		CATCH (!file_remove (libtest1_test1o));
	}
	if (file_exists (libtest1_test2o)) {
		CATCH (!file_remove (libtest1_test2o));
	}
	if (file_exists (libtest2_test3o)) {
		CATCH (!file_remove (libtest2_test3o));
	}
	if (file_exists (libtest2_test4o)) {
		CATCH (!file_remove (libtest2_test4o));
	}
	if (file_exists (libshared_sharedo)) {
		CATCH (!file_remove (libshared_sharedo));
	}
	if (file_exists (apptest1)) {
		CATCH (!file_remove (apptest1));
	}
	if (file_exists (apptest1_maino)) {
		CATCH (!file_remove (apptest1_maino));
	}
	if (file_exists (apptest2)) {
		CATCH (!file_remove (apptest2));
	}
	if (file_exists (apptest2_maino)) {
		CATCH (!file_remove (apptest2_maino));
	}
	compile_print_set_enabled (false);
	project = compile_project_create (path);
	CATCH (!compile_project_prepare (project));
	CATCH (!compile_project_execute_with_directory_name (project, "lib.shared", false));
	CATCH (file_exists (libtest1a));
	CATCH (file_exists (libtest2a));
	CATCH (!file_exists (libshareda));
	CATCH (file_exists (libtest1_test1o));
	CATCH (file_exists (libtest1_test2o));
	CATCH (file_exists (libtest2_test3o));
	CATCH (file_exists (libtest2_test4o));
	CATCH (!file_exists (libshared_sharedo));
	CATCH (file_exists (apptest1));
	CATCH (file_exists (apptest1_maino));
	CATCH (file_exists (apptest2));
	CATCH (file_exists (apptest2_maino));
	compile_project_destroy (project);
	compile_print_set_enabled (true);
	string_destroy (path);
	string_destroy (libtest1a);
	string_destroy (libtest2a);
	string_destroy (libshareda);
	string_destroy (libtest1_test1o);
	string_destroy (libtest1_test2o);
	string_destroy (libtest2_test3o);
	string_destroy (libtest2_test4o);
	string_destroy (libshared_sharedo);
	string_destroy (apptest1);
	string_destroy (apptest1_maino);
	string_destroy (apptest2);
	string_destroy (apptest2_maino);
	PASS ();
}

