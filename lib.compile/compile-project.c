#include <stdlib.h>
#include <stdio.h>

#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.compile/compile-project.h>
#include <lib.compile/compile-print.h>
#include <lib.compile/compile.h>

static bool recursively_flatten_libraries       (CompileProject *project, Compile *compile);
static bool recursively_flatten_libraries_inner (CompileProject *project, Compile *compile, Directory *library, Tree *append);
static bool sort_libraries                      (CompileProject *project, Compile *compile);
static bool included_libraries_are_up_to_date   (CompileProject *project, Compile *compile);

CompileProject *compile_project_create (const char *path)
{
	CompileProject *project = NULL;

	if (!file_path_is_valid (path)) {
		compile_print ("The path is not valid: %s\n", path);
                error_code (FunctionCall, 1);
		return NULL;
	}
	if (!(project = memory_create (sizeof (CompileProject)))) {
                error_code (FunctionCall, 2);
		return NULL;
	}
	if (!(project->topological = topological_create ())) {
		compile_project_destroy (project);
                error_code (FunctionCall, 3);
		return NULL;
	}
	if (!(project->nodes = list_create ())) {
		compile_project_destroy (project);
                error_code (FunctionCall, 4);
		return NULL;
	}
	if (!(project->directory_to_compile = tree_create ())) {
		compile_project_destroy (project);
                error_code (FunctionCall, 5);
		return NULL;
	}
	if (!(project->directory = directory_open (path))) {
		compile_print ("Failed to open the directory: %s\n", path);
		compile_project_destroy (project);
                error_code (FunctionCall, 6);
		return NULL;
	}
	if (!string_begins_with (project->directory->name, "project.")) {
		compile_print ("The directory name must begin with 'project.'.\n");
		compile_project_destroy (project);
                error_code (FunctionCall, 7);
		return NULL;
	}
	project->sorted = NULL;
	return project;
}

bool compile_project_prepare (CompileProject *project)
{
	ListNode *node;
	Directory *sub_directory;
	Compile *compile;
	Compile *sub_compile;
	TreeIterator *iterator;

	if (!project) {
                error (InvalidArgument);
		return false;
	}
	if (!directory_read (project->directory)) {
		compile_print ("Failed to read directory: %s\n", project->directory->name);
                error_code (FunctionCall, 1);
		return false;
	}
	for (node = list_first (project->directory->directories); node; node = list_next (node)) {
		if (!(sub_directory = node->data)) {
                        error_code (InvalidOperation, 1);
			return false;
		}
		if (!string_begins_with (sub_directory->name, "lib.") &&
		    !string_begins_with (sub_directory->name, "app.") &&
                    !string_begins_with (sub_directory->name, "plugin.")) {
			continue;
		}
		if (!(compile = compile_create (project->directory, sub_directory))) {
                        error_code (FunctionCall, 2);
			return false;
		}
		if (!list_append (project->nodes, compile)) {
			compile_destroy (compile);
                        error_code (FunctionCall, 3);
			return false;
		}
		if (!topological_add_vertex (project->topological, (Object *)compile)) {
                        error_code (FunctionCall, 4);
			return false;
		}
		if (!tree_insert (project->directory_to_compile, (Object *)sub_directory, compile)) {
                        error_code (FunctionCall, 5);
			return false;
		}
	}
	for (node = list_first (project->directory->directories); node; node = list_next (node)) {
		if (!(sub_directory = node->data)) {
                        error_code (InvalidOperation, 3);
			return false;
		}
                if (!string_begins_with (sub_directory->name, "lib.") &&
		    !string_begins_with (sub_directory->name, "app.") &&
                    !string_begins_with (sub_directory->name, "plugin.")) {
			continue;
		}
		if (!(compile = tree_search (project->directory_to_compile, (Object *)sub_directory))) {
                        error_code (InvalidOperation, 5);
			return false;
		}
		if (!compile_prepare (compile)) {
                        error_code (FunctionCall, 6);
			return false;
		}
		if (!(iterator = tree_iterator_create (compile->libraries))) {
                        error_code (FunctionCall, 7);
			return false;
		}
		while (tree_iterator_next (iterator)) {
			if (string_equals (sub_directory->name, ((Directory *)iterator->key)->name)) {
				continue;
			}
			if (!(sub_compile = tree_search (project->directory_to_compile, iterator->key))) {
				tree_iterator_destroy (iterator);
                                error_code (InvalidOperation, 6);
				return false;
			}
			if (!topological_set_edge (project->topological, (Object *)compile, (Object *)sub_compile)) {
				tree_iterator_destroy (iterator);
				error_code (FunctionCall, 8);
				return false;
			}
		}
		tree_iterator_destroy (iterator);
	}
	if (!(project->sorted = topological_sort (project->topological))) {
		compile_print ("Topological sort of project directories failed.\n");
                error_code (InvalidOperation, 7);        
		return false;
	}
	for (node = list_first (project->sorted); node; node = list_next (node)) {
		if (!recursively_flatten_libraries (project, node->data)) {
			return false;
		}
		if (!sort_libraries (project, node->data)) {
			return false;
		}
	}
	for (node = list_last (project->sorted); node; node = list_previous (node)) {
		if (!compile_actions (node->data, project->directory->path)) {
                        error_code (FunctionCall, 9);
			return false;
		}
	}
	return true;
}

void compile_project_destroy (CompileProject *project)
{
	ListNode *node;

	if (!project) {
		return;
	}
	if (project->directory) {
		directory_close (project->directory);
	}
	if (project->topological) {
		topological_destroy (project->topological);
	}
	if (project->directory_to_compile) {
		tree_destroy (project->directory_to_compile);
	}
	if (project->sorted) {
		list_destroy (project->sorted);
	}
	for (node = list_first (project->nodes); node; node = list_next (node)) {
		compile_destroy (node->data);
	}
	if (project->nodes) {
		list_destroy (project->nodes);
	}
	memory_destroy (project);
}

bool compile_project_execute (CompileProject *project, bool bootstrap)
{
	ListNode *node;

	if (!project) {
		compile_debug_invalid_arguments ();
		return false;
	}
	for (node = list_last (project->sorted); node; node = list_previous (node)) {
		if (!compile_execute (node->data, bootstrap)) {
			return false;
		}
	}
	return true;
}

bool compile_project_execute_with_directory_name (CompileProject *project, const char *directory_name, bool bootstrap)
{
	Compile *compile = NULL;
	ListNode *node;

	if (!project) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (!directory_name) {
		compile_debug_invalid_arguments ();
		return false;
	}
	for (node = list_last (project->sorted); !compile && node; node = list_previous (node)) {
		if (string_equals (((Compile *)node->data)->directory->name, directory_name)) {
			compile = node->data;
		}
	}
	if (!compile) {
		compile_debug_invalid_value ();
		return false;
	}
	if (!included_libraries_are_up_to_date (project, compile)) {
		return false;
	}
	if (!compile_execute (compile, bootstrap)) {
		return false;
	}
	return true;
}

static bool recursively_flatten_libraries (CompileProject *project, Compile *compile)
{
	TreeIterator *iterator;
	Tree *append;

	if (!(append = tree_create ())) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(iterator = tree_iterator_create (compile->libraries))) {
		compile_debug_allocate_memory ();
		return false;
	}
	while (tree_iterator_next (iterator)) {
		if (!recursively_flatten_libraries_inner (project, compile, (Directory *)iterator->key, append)) {
			tree_iterator_destroy (iterator);
			tree_destroy (append);
			return false;
		}
	}
	tree_iterator_destroy (iterator);
	if (!(iterator = tree_iterator_create (append))) {
		compile_debug_allocate_memory ();
		return false;
	}
	while (tree_iterator_next (iterator)) {
		if (!tree_insert (compile->libraries, iterator->key, iterator->key)) {
			tree_iterator_destroy (iterator);
			tree_destroy (append);
			compile_debug_operation_failed ();
			return false;
		}
	}
	tree_iterator_destroy (iterator);
	tree_destroy (append);
	return true;
}

static bool recursively_flatten_libraries_inner (CompileProject *project, Compile *compile, Directory *library, Tree *append)
{
	Compile *compile_library;
	TreeIterator *iterator;
	
	if (!(compile_library = tree_search (project->directory_to_compile, (Object *)library))) {
		compile_debug_operation_failed ();
		return false;
	}
	if (!(iterator = tree_iterator_create (compile_library->libraries))) {
		compile_debug_allocate_memory ();
		return false;
	}
	while (tree_iterator_next (iterator)) {
		if (!recursively_flatten_libraries_inner (project, compile, (Directory *)iterator->key, append)) {
			tree_iterator_destroy (iterator);
			return false;
		}
	}
	tree_iterator_destroy (iterator);
	if (!tree_search (compile->libraries, (Object *)library) &&
	    !tree_search (append, (Object *)library)) {
		if (!tree_insert (append, (Object *)library, library)) {
			compile_debug_operation_failed ();
			return false;
		}
	}
	return true;
}

static bool sort_libraries (CompileProject *project, Compile *compile)
{
	Compile *library;
	ListNode *node;

	for (node = list_last (project->sorted); node; node = list_previous (node)) {
		library = node->data;
		if (!tree_search (compile->libraries, (Object *)library->directory)) {
			continue;
		}
		if (!list_append (compile->libraries_sorted, library->directory)) {
			compile_debug_operation_failed ();
			return false;
		}
	}
	return true;
}

static bool included_libraries_are_up_to_date (CompileProject *project, Compile *compile)
{
	Compile *library;
	TreeIterator *iterator;
	
	if (!(iterator = tree_iterator_create (compile->libraries))) {
		compile_debug_allocate_memory ();
		return false;
	}
	while (tree_iterator_next (iterator)) {
		if (!(library = tree_search (project->directory_to_compile, iterator->key))) {
			continue;
		}
		if (!library->actions) {
			continue;
		}
		if (list_count (library->actions) != 0) {
			tree_iterator_destroy (iterator);
			compile_print ("The library '%s' is out of date.\n", library->directory->name);
			return false;
		}
	}
	tree_iterator_destroy (iterator);
	return true;
}
