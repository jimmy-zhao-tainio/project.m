#include <stdlib.h>
#include <stdio.h>

#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.compile/compile.h>
#include <lib.compile/compile-link.h>
#include <lib.compile/compile-action.h>
#include <lib.compile/compile-print.h>

static bool compile_set_vertices (Compile *compile);
static bool compile_prepare_o (Compile *compile, File *file);
static bool compile_prepare_ch (Compile *compile, File *file);
static bool read_included_headers (File *file, List *headers_local, List *headers_library);
static bool link_local_header (Compile *compile, File *file, char *header);
static bool link_library_header (Compile *compile, File *file, char *header);
static bool compile_o_file (Compile *compile, File *file, const char *project_path);
static bool compile_o_file_try (Compile *compile, File *file, char **c_path, char **c_name, const char *project_path);
static int o_file_is_latest (Compile *compile, File *file);

Compile *compile_create (Directory *project, Directory *directory)
{
	Compile *compile;

	if (!project ||
	    !directory) {
		compile_debug_invalid_arguments ();
		return NULL;
	}
	if (project == directory) {
		compile_debug_invalid_arguments ();
		return NULL;
	}
	if (!string_begins_with (directory->name, "app.") &&
	    !string_begins_with (directory->name, "lib.") &&
            !string_begins_with (directory->name, "plugin.")) {
		compile_debug_invalid_arguments ();
		return NULL;
	}
	if (!(compile = memory_create (sizeof (Compile)))) {
		compile_debug_allocate_memory ();
		return NULL;
	}
	compile->object.id = object_id ();
	if (!(compile->libraries = tree_create ())) {
		compile_debug_allocate_memory ();
		compile_destroy (compile);
		return NULL;
	}
	if (!(compile->libraries_sorted = list_create ())) {
		compile_debug_allocate_memory ();
		compile_destroy (compile);
		return NULL;
	}
	if (!(compile->files = topological_create ())) {
		compile_debug_allocate_memory ();
		compile_destroy (compile);
		return NULL;
	}
	if (!(compile->o_files = list_create ())) {
		compile_debug_allocate_memory ();
		compile_destroy (compile);
		return NULL;
	}
	if (!(compile->actions = list_create ())) {
		compile_debug_allocate_memory ();
		compile_destroy (compile);
		return NULL;
	}
	compile->project = project;
	compile->directory = directory;
	if (!compile_set_vertices (compile)) {
		compile_debug_operation_failed ();
		compile_destroy (compile);
		return NULL;
	}
	return compile;
}

bool compile_prepare (Compile *compile)
{
	ListNode *node;
	File *file;
	
	if (!compile) {
		compile_debug_invalid_arguments ();
		return false;
	}
	for (node = list_first (compile->directory->files); node; node = node->next) {
		file = node->data;
		if (string_begins_with (file->name, ".")) {
			continue;
		}
		if (string_ends_with (file->name, ".o")) {
			if (!compile_prepare_o (compile, file)) {
				return false;
			}
		}
		else if (string_ends_with (file->name, ".c") ||
		         string_ends_with (file->name, ".h")) {
			if (!compile_prepare_ch (compile, file)) {
				return false;
			}
		}
	}
	if (!(compile->sorted = topological_sort (compile->files))) {
		compile_debug_operation_failed ();
		return false;
	}
	return true;
}

bool compile_actions (Compile *compile, const char *project_path)
{
	ListNode *node;
	File *file;

	if (!compile ||
	    !project_path) {
		compile_debug_invalid_arguments ();
		return false;
	}
	for (node = list_last (compile->sorted); node; node = node->previous) {
		file = node->data;
		if (!string_ends_with (file->name, ".o")) {
			continue;
		}
		if (!list_append (compile->o_files, file)) {
			compile_debug_operation_failed ();
			return false;
		}
		switch (o_file_is_latest (compile, file)) {
		case -1:
			return false;
		case 0:
			break;
		case 1:
			continue;
		}
		if (!compile_o_file (compile, file, project_path)) {
			return false;
		}
	}
	if (list_count (compile->o_files) == 0) {
		return true;
	}
	if (string_begins_with (compile->directory->name, "lib.")) {
		return compile_link_library (compile);
	}
	else if (string_begins_with (compile->directory->name, "app.")) {
		return compile_link_application (compile);
	}
        else {
                return compile_link_plugin (compile);
        }
}

bool compile_execute (Compile *compile, bool bootstrap)
{
	CompileAction *action;
	ListNode *node;

	if (!compile) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (!compile->actions) {
		compile_debug_invalid_arguments ();
		return false;
	}
        if (!bootstrap) {
	        compile_print (compile->directory->name);
	        compile_print (":\n");
        }
	if (!bootstrap && list_count (compile->actions) == 0) {
		compile_print ("\t[DONE]\n");
	}
	for (node = list_first (compile->actions); node; node = node->next) {
		action = node->data;
                if (!bootstrap) {
                        compile_print ("\t");
                        compile_print (action->print);
                        if (system (action->command)) {
                                return false;
                        }
                }
                else {
                        compile_print (action->command);
                        compile_print ("\n");
                }
	}
	return true;
}

void compile_destroy (Compile *compile)
{
	ListNode *node;

	if (!compile) {
		compile_debug_invalid_arguments ();
		return;
	}
	if (compile->libraries) {
		tree_destroy (compile->libraries);
	}
	if (compile->libraries_sorted) {
		list_destroy (compile->libraries_sorted);
	}
	if (compile->files) {
		topological_destroy (compile->files);
	}
	if (compile->sorted) {
		list_destroy (compile->sorted);
	}
	if (compile->o_files) {
		list_destroy (compile->o_files);
	}
	if (compile->actions) {
		for (node = list_first (compile->actions); node; node = node->next) {
			compile_action_destroy (node->data);
		}
		list_destroy (compile->actions);
	}
	memory_destroy (compile);
}

static bool compile_set_vertices (Compile *compile)
{
	ListNode *node;
	File *file;
	File *co_file;
	char *co_name;

	if (!compile) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (!directory_read (compile->directory)) {
		compile_print ("Failed to read directory: %s\n", compile->directory->name);
		return false;
	}
	/* Adds topological vertices (not edges) for local .h, .c and .o files.
	 * For .c files, it checks if a .o file exists, if not, a fake file 
	 * with modified = 0 will be added to the list of files in the 
	 * directory structure.
	 * For .o files, it checks if a .c file exists, if not, this functions 
	 * stops and returns an error.
	 * OBS, vertices for included library .h files are added later when each
	 * file is read in compile_prepare.
	 */ 
	for (node = list_first (compile->directory->files); node; node = node->next) {
		file = node->data;
		if (string_begins_with (file->name, ".")) {
			continue;
		}
		if (!topological_add_vertex (compile->files, (Object *)file)) {
			compile_debug_operation_failed ();
			return false;
		}
		if (string_ends_with (file->name, ".h")) {
			continue;
		}
		if (string_ends_with (file->name, ".c")) {
			if (!(co_name = string_create (file->name))) {
				compile_debug_allocate_memory ();
				return false;
			}
			co_name[string_length (co_name) - 1] = 'o';
			if (!directory_find_file (compile->directory, co_name)) {
				if (!(co_file = directory_add_fake_file (compile->directory, co_name))) {
					string_destroy (co_name);
					compile_debug_operation_failed ();
					return false;
				}
				co_file->modified = 0;
			}
			string_destroy (co_name);
		}
		if (string_ends_with (file->name, ".o")) {
			if (!(co_name = string_create (file->name))) {
				compile_debug_allocate_memory ();
				return false;
			}
			co_name[string_length (co_name) - 1] = 'c';
			if (!directory_find_file (compile->directory, co_name)) {
				compile_print ("%s depends on ", file->path);
				compile_print ("%s but it could not be found in this directory.\n", co_name);
				string_destroy (co_name);
				return false;
			}
			string_destroy (co_name);
		}
	}
	return true;
}

static bool compile_prepare_o (Compile *compile, File *file)
{
	File *co_file;
	char *co_name;

	if (!(co_name = string_create (file->name))) {
		compile_debug_allocate_memory ();
		return false;
	}
	co_name[string_length (co_name) - 1] = 'c';
	if (!(co_file = directory_find_file (compile->directory, co_name))) {
		string_destroy (co_name);
		compile_print ("File not found: %s\n", co_name);
		return false;
	}
	string_destroy (co_name);
	if (!topological_set_edge (compile->files, (Object *)file, (Object *)co_file)) {
		compile_debug_operation_failed ();
		return false;
	}
	return true;
}

static bool compile_prepare_ch (Compile *compile, File *file)
{
	List *headers_local;
	List *headers_library;
	ListNode *node;
	int result;

	if (!file_open (file)) {
		compile_print ("Failed to open file: %s\n", file->path);
		return false;
	}
	if (!(headers_local = list_create ())) {
		file_close (file);
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(headers_library = list_create ())) {
		list_destroy (headers_local);
		file_close (file);
		compile_debug_allocate_memory ();
		return false;
	}
	if ((result = read_included_headers (file, headers_local, headers_library))) {
		for (node = list_first (headers_local); result && node; node = node->next) {
			result = link_local_header (compile, file, node->data);
		}
		for (node = list_first (headers_library); result && node; node = node->next) {
			result = link_library_header (compile, file, node->data);
		}
	}
	for (node = list_first (headers_local); node; node = node->next) {
		string_destroy (node->data);
	}
	for (node = list_first (headers_library); node; node = node->next) {
		string_destroy (node->data);
	}
	list_destroy (headers_local);
	list_destroy (headers_library);
	file_close (file);
	return result;
}

static bool link_local_header (Compile *compile, File *file, char *header)
{
	File *h_file;
	size_t index;

	if (!header) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (string_begins_with (header, ".")) {
		return true;
	}
	if (string_index (header, '/', &index)) {
		return true;
	}
	if (!(h_file = directory_find_file (compile->directory, header))) {
		compile_print ("File not found: %s\n", header);
		return false;
	}
	if (!topological_set_edge (compile->files, (Object *)file, (Object *)h_file)) {
		compile_debug_operation_failed ();
		return false;
	}
	return true;
}

static size_t array_length (char **array)
{
	size_t length;

	if (!array) {
		return 0;
	}
	for (length = 0; array[length]; length++) {
	}
	return length;
}

static void array_destroy (char **array)
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

static bool link_library_header (Compile *compile, File *file, char *header)
{
	Directory *directory_library;
	File *h_file;
	char **split;

	if (!header) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (!string_begins_with (header, "lib.")) {
		return true;
	}
	if (!(split = string_split (header, '/'))) {
		return true;
	}
	if (array_length (split) != 2) {
		array_destroy (split);	
		return true;
	}
	if (string_begins_with (split[1], ".")) {
		return true;
	}
	if (string_equals (compile->directory->name, split[0])) {
		if (!(h_file = directory_find_file (compile->directory, split[1]))) {
			compile_print ("File not found: %s\n", split[1]);
			array_destroy (split);
			return false;
		}
		if (!topological_set_edge (compile->files, (Object *)file, (Object *)h_file)) {
			compile_debug_operation_failed ();
			array_destroy (split);
			return false;
		}
		array_destroy (split);
		return true;
	}
	if (!(directory_library = directory_find_directory (compile->project, split[0]))) {
		compile_print ("File not found: %s\n", split[0]);
		array_destroy (split);
		return false;
	}
	if (!directory_read (directory_library)) {
		compile_print ("Failed to read directory: %s\n", directory_library->path);
		array_destroy (split);
		return false;
	}
	if (!(h_file = directory_find_file (directory_library, split[1]))) {
		compile_print ("File not found: %s\n", split[1]);
		array_destroy (split);
		return false;
	}
	array_destroy (split);
	if (!topological_contains_vertex (compile->files, (Object *)h_file)) {
		if (!topological_add_vertex (compile->files, (Object *)h_file)) {
			compile_debug_operation_failed ();
			return false;
		}
	}
	if (!topological_set_edge (compile->files, (Object *)file, (Object *)h_file)) {
		compile_debug_operation_failed ();
		return false;
	}
	if (!tree_search (compile->libraries, (Object *)directory_library)) {
		if (!tree_insert (compile->libraries, (Object *)directory_library, directory_library)) {
			compile_debug_operation_failed ();
			return false;
		}
	}
	return true;
}

static bool read_included_headers (File *file, List *headers_local, List *headers_library)
{
	char *line;
	char *header;
	size_t bytes_read;
	size_t begin;
	size_t end;
	size_t i;
	char token;

	if (!(line = string_create_with_size (128))) {
		compile_debug_allocate_memory ();
		return false;
	}
	while (1) {
		if (!file_readline (file, line, &bytes_read)) {
			string_destroy (line);
			return false;
		}
		if (bytes_read == 0) {
			string_destroy (line);
			return true;
		}
		if (!string_begins_with (line, "#include ")) {
			continue;
		}
		begin = string_length ("#include ");
		token = line[begin];
		if (token != '<' && token != '"') {
			continue;
		}
		if (token == '<') {
			token = '>';
		}
		begin = begin + 1;
		for (end = begin; line[end] != '\0' && line[end] != token; end++) {
		}
		if (line[end] != token) {
			continue;
		}
		if (!(header = string_create_with_size (end - begin + 1))) {
			string_destroy (line);
			compile_debug_allocate_memory ();
			return false;
		}
		for (i = 0; begin + i < end; i++) {
			header[i] = line[begin + i];
		}
		header[i] = '\0';
		if (token == '>') {
			if (list_append (headers_library, header)) {
				continue;
			}
		}
		else if (token == '"') {
			if (list_append (headers_local, header)) {
				continue;
			}
		}
		string_destroy (header);
		string_destroy (line);
		compile_print ("Failed to read file: %s\n", file->path);
		return false;
	}
}

static bool compile_o_file (Compile *compile, File *file, const char *project_path)
{
	char *c_path = NULL;
	char *c_name = NULL;
	int result;

	if (!file ||
	    !project_path) {
		compile_debug_invalid_arguments ();
		return false;
	}
	result = compile_o_file_try (compile, file, &c_path, &c_name, project_path);
	if (c_path) {
		string_destroy (c_path);
	}
	if (c_name) {
		string_destroy (c_name);
	}
	return result;
}

static bool compile_o_file_try (Compile *compile, File *file, char **c_path, char **c_name, const char *project_path)
{
	CompileAction *action;

	if (!file ||
	    !file->path ||
	    !project_path ||
	    !c_path ||
	    !c_name ||
	    *c_name ||
	    *c_path) {
		compile_debug_invalid_arguments ();
		return false;
	}
	if (!(*c_path = string_create (file->path)) ||
	    !(*c_name = string_create (file->name))) {
		compile_debug_allocate_memory ();
		return false;
	}
	(*c_path)[string_length (*c_path) - 1] = 'c';
	(*c_name)[string_length (*c_name) - 1] = 'c';
	if (!directory_find_file (compile->directory, *c_name)) {
		compile_print ("File not found: %s\n", *c_name);
		return false;
	}
	if (!(action = compile_action_create (COMPILE_ACTION_CC))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, action)) {
		compile_action_destroy (action);
		compile_debug_operation_failed ();
		return false;
	}
	if (!(action->command = string_create (""))) {
		compile_debug_allocate_memory ();
		return false;
	}
	(*c_path)[string_length (*c_path) - 1] = 'c';
	(*c_name)[string_length (*c_name) - 1] = 'c';
	if (!string_append (&action->command, "gcc ") ||
	    !string_append (&action->command, "-std=gnu99 ") ||
            !string_append (&action->command, "-O0 ") ||
            !string_append (&action->command, "-pedantic ") ||
            !string_append (&action->command, "-g ") ||
            !string_append (&action->command, "-Wall ") ||
            !string_append (&action->command, "-Wextra ") ||
            !string_append (&action->command, "-Wconversion ") ||
            !string_append (&action->command, "-Wformat-security ") ||
            !string_append (&action->command, "-Werror ") ||
            !string_append (&action->command, "-fPIC ") ||
            !string_append (&action->command, "-I") ||
	    !string_append (&action->command, project_path) ||
            !string_append (&action->command, " ") ||
	    !string_append (&action->command, "-c ") ||
	    !string_append (&action->command, *c_path)) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!string_append (&action->command, " -o ") ||
	    !string_append (&action->command, file->path)) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!compile_print_get_enabled ()) {
		if (!string_append (&action->command, " 1>/dev/null 2>/dev/null")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
	if (!(action->print = string_create ("[CC] ")) ||
	    !string_append (&action->print, *c_name) ||
	    !string_append (&action->print, "\n")) {
		compile_debug_allocate_memory ();
		return false;
	}
	file->modified = 0;
	return true;
}

static int o_file_is_latest (Compile *compile, File *file)
{
	List *edges;
	List *stack;
	ListNode *node;
	time_t modified = file->modified;
	
	/*
	 * Object file compile can happen if:
	 *
	 * Rules:
	 * 	#1: o-file does not exist (modified is 0).
	 * 	#2: any edge (or edge to an edge recursively) is newer than object file
	 */
	if (modified == 0) {
		return 0;
	}
	if (!(stack = list_create ())) {
		compile_debug_allocate_memory ();
		return -1;
	}
	if (!list_insert (stack, file)) {
		list_destroy (stack);
		compile_debug_operation_failed ();
		return -1;
	}
	while (list_count (stack) != 0) {
		node = list_first (stack);
		file = node->data;
		list_remove (stack, node);
		if (file->modified > modified) {
			list_destroy (stack);
			return 0;
		}
		if (!(edges = topological_get_edges (compile->files, (Object *)file))) {
			list_destroy (stack);
			compile_debug_operation_failed ();
			return -1;
		}
		for (node = list_first (edges); node; node = node->next) {
			if (!list_append (stack, node->data)) {
				list_destroy (stack);
				list_destroy (edges);
				compile_debug_operation_failed ();
				return -1;
			}
		}
		list_destroy (edges);
	}
	list_destroy (stack);
	return 1;
}

