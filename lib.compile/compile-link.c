#include <lib.compile/compile-link.h>
#include <lib.compile/compile-action.h>
#include <lib.compile/compile-print.h>
#include <lib.core/file-reader.h>
#include <lib.core/string.h>

static bool link_library_try (Compile *compile, char **lib_path, char **lib_filename);
static int library_is_newer (Directory *directory, time_t modified);
static char *link_flags (Directory *directory);

bool compile_link_library (Compile *compile)
{
	char *lib_path = NULL;
	char *lib_filename = NULL;

	int result = link_library_try (compile, &lib_path, &lib_filename);
	if (lib_path) {
		string_destroy (lib_path);
	}
	if (lib_filename) {
		string_destroy (lib_filename);
	}
	return result;
}

static bool link_library_try (Compile *compile, char **lib_path, char **lib_filename)
{
	CompileAction *rm;
	CompileAction *arch;
	CompileAction *index;
	TreeIterator *iterator;
	ListNode *node;
	File *lib_file;
	const char *linker = "ar rsc ";
	bool newer_library = false;
	Directory *directory;
	File *file;
	
	if (!(*lib_filename = string_create (compile->directory->name)) || 
	    !string_append (lib_filename, ".a")) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(*lib_path = string_create (compile->directory->path)) ||
	    !string_append (lib_path, "/") ||
	    !string_append (lib_path, *lib_filename)) {
		compile_debug_allocate_memory ();
		return false;
	}
	if ((lib_file = directory_find_file (compile->directory, *lib_filename))) {
		if (!(iterator = tree_iterator_create (compile->libraries))) {
			compile_debug_allocate_memory ();
			return false;
		}
		while (!newer_library && tree_iterator_next (iterator)) {
			switch (library_is_newer ((Directory *)iterator->key, 
                                                  lib_file->modified)) {
			case -1:
				tree_iterator_destroy (iterator);
				return false;
			case 0:
				continue;
			case 1:
				newer_library = true;
				break;
			}
		}
		tree_iterator_destroy (iterator);
	}
	if (newer_library == false && 
	    lib_file &&
	    list_count (compile->actions) == 0) {
		return true;
	}
        if (!(rm = compile_action_create (COMPILE_ACTION_SILENT))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, rm)) {
		compile_action_destroy (rm);
		compile_debug_operation_failed ();
		return false;
	}
	if (!(rm->command = string_create ("rm -f ")) || 
	    !string_append (&rm->command, *lib_path)) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(arch = compile_action_create (COMPILE_ACTION_ARCH))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, arch)) {
		compile_action_destroy (arch);
		compile_debug_operation_failed ();
		return false;
	}
	if (!(arch->command = string_create (linker)) || 
	    !string_append (&arch->command, *lib_path) || 
	    !string_append (&arch->command, " ")) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(index = compile_action_create (COMPILE_ACTION_INDEX))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, index)) {
		compile_action_destroy (index);
		compile_debug_operation_failed ();
		return false;
	}
	for (node = list_first (compile->o_files); node; node = list_next (node)) {
		file = node->data;
		if (!string_append (&arch->command, file->path) || 
	    	    !string_append (&arch->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
	for (node = list_last (compile->libraries_sorted); 
             node;
             node = list_previous (node)) {
		directory = node->data;
		if (!string_append (&arch->command, directory->path) || 
	    	    !string_append (&arch->command, "/") || 
	    	    !string_append (&arch->command, directory->name) ||
	    	    !string_append (&arch->command, ".a") ||
	    	    !string_append (&arch->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
	if (!(arch->print = string_create ("[ARCH] ")) ||
	    !string_append (&arch->print, *lib_filename) ||
	    !string_append (&arch->print, "\n")) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(index->command = string_create ("ranlib ")) || 
	    !string_append (&index->command, *lib_path)) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!(index->print = string_create ("[INDEX] ")) ||
	    !string_append (&index->print, *lib_filename) ||
	    !string_append (&index->print, "\n")) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (lib_file) {
		lib_file->modified = 0;
	}
	return true;
}

bool compile_link_application (Compile *compile)
{
	CompileAction *action;
	const char *linker = "gcc -pthread -lrt -rdynamic ";
	const char *flag = "-O0 -o ";
        char *dot_link;
	bool newer_library = 0;
	File *app_file;
	File *file;
	ListNode *node;
	TreeIterator *iterator;
	Directory *directory;

	if ((app_file = directory_find_file (compile->directory, 
                                             compile->directory->name))) {
		if (!(iterator = tree_iterator_create (compile->libraries))) {
			compile_debug_allocate_memory ();
			return false;
		}
		while (!newer_library && tree_iterator_next (iterator)) {
			switch (library_is_newer ((Directory *)iterator->key, 
                                                  app_file->modified)) {
			case -1:
				tree_iterator_destroy (iterator);
				return false;
			case 0:
				continue;
			case 1:
				newer_library = true;
				break;
			}
		}
		tree_iterator_destroy (iterator);
	}
	if (newer_library == false && 
	    app_file && 
	    list_count (compile->actions) == 0) {
		return true;
	}
	if (!(action = compile_action_create (COMPILE_ACTION_BUILD))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, action)) {
		compile_action_destroy (action);
		compile_debug_operation_failed ();
		return false;
	}
	if (!(action->command = string_create (linker))) {
		compile_debug_allocate_memory ();
		return false;
	}
	for (node = list_first (compile->o_files); node; node = list_next (node)) {
		file = node->data;
		if (!string_append (&action->command, file->path) || 
	    	    !string_append (&action->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
	for (node = list_last (compile->libraries_sorted); 
             node; 
             node = list_previous (node)) {
		directory = node->data;
		if (!string_append (&action->command, directory->path) || 
	    	    !string_append (&action->command, "/") || 
	    	    !string_append (&action->command, directory->name) ||
	    	    !string_append (&action->command, ".a") ||
	    	    !string_append (&action->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
        dot_link = link_flags (compile->directory);
	if (!string_append (&action->command, flag) || 
	    !string_append (&action->command, compile->directory->path) ||
	    !string_append (&action->command, "/") ||
	    !string_append (&action->command, compile->directory->name) ||
            !string_append (&action->command, " -lm -ldl ") ||
            (dot_link && !string_append (&action->command, dot_link)) ||
	    !string_append (&action->command, " 1>/dev/null")) {
		compile_debug_allocate_memory ();
		return false;
	}
        if (dot_link) {
                string_destroy (dot_link);
        }
	if (!(action->print = string_create ("[BUILD] ")) ||
	    !string_append (&action->print, compile->directory->name) ||
	    !string_append (&action->print, "\n")) {
		compile_debug_allocate_memory ();
		return false;
	}
	return true;	
}

bool compile_link_plugin (Compile *compile)
{
        CompileAction *action;
	const char *linker = "gcc -lrt -shared ";
	const char *flag = "-O0 -o ";
        char *dot_link;
	bool newer_library = 0;
	File *so_file;
        char *so_file_name;
	File *file;
	ListNode *node;
	TreeIterator *iterator;
	Directory *directory;

        if (!(so_file_name = string_create (compile->directory->name)) ||
            !string_append (&so_file_name, ".so")) {
                if (so_file_name) {
                        string_destroy (so_file_name);
                }
                return false;
        }
	if ((so_file = directory_find_file (compile->directory, so_file_name))) {
                string_destroy (so_file_name);
                so_file_name = NULL;
		if (!(iterator = tree_iterator_create (compile->libraries))) {
			compile_debug_allocate_memory ();
			return false;
		}
		while (!newer_library && tree_iterator_next (iterator)) {
			switch (library_is_newer ((Directory *)iterator->key, 
                                                  so_file->modified)) {
			case -1:
				tree_iterator_destroy (iterator);
				return false;
			case 0:
				continue;
			case 1:
				newer_library = true;
				break;
			}
		}
		tree_iterator_destroy (iterator);
	}
        if (so_file_name) {
                string_destroy (so_file_name);
        }
	if (newer_library == false && 
	    so_file && 
	    list_count (compile->actions) == 0) {
		return true;
	}
	if (!(action = compile_action_create (COMPILE_ACTION_BUILD))) {
		compile_debug_allocate_memory ();
		return false;
	}
	if (!list_append (compile->actions, action)) {
		compile_action_destroy (action);
		compile_debug_operation_failed ();
		return false;
	}
	if (!(action->command = string_create (linker))) {
		compile_debug_allocate_memory ();
		return false;
	}
	for (node = list_first (compile->o_files); node; node = list_next (node)) {
		file = node->data;
		if (!string_append (&action->command, file->path) || 
	    	    !string_append (&action->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
	for (node = list_last (compile->libraries_sorted); 
             node; 
             node = list_previous (node)) {
		directory = node->data;
		if (!string_append (&action->command, directory->path) || 
	    	    !string_append (&action->command, "/") || 
	    	    !string_append (&action->command, directory->name) ||
	    	    !string_append (&action->command, ".a") ||
	    	    !string_append (&action->command, " ")) {
			compile_debug_allocate_memory ();
			return false;
		}
	}
        dot_link = link_flags (compile->directory);
	if (!string_append (&action->command, flag) || 
	    !string_append (&action->command, compile->directory->path) ||
	    !string_append (&action->command, "/") ||
	    !string_append (&action->command, compile->directory->name) ||
	    !string_append (&action->command, ".so -lm -lSDL2 -lX11 ") ||
            (dot_link && !string_append (&action->command, dot_link)) ||
	    !string_append (&action->command, " 1>/dev/null")) {
		compile_debug_allocate_memory ();
		return false;
	}
        if (dot_link) {
                string_destroy (dot_link);
        }
	if (!(action->print = string_create ("[BUILD] ")) ||
	    !string_append (&action->print, compile->directory->name) ||
	    !string_append (&action->print, "\n")) {
		compile_debug_allocate_memory ();
		return false;
	}
	return true;
}

static int library_is_newer (Directory *directory, time_t modified)
{
	File *file;
	char *lib_filename;

	if (modified == 0) {
		return 0;
	}
	if (!(lib_filename = string_create (directory->name))) {
		compile_debug_allocate_memory ();
		return -1;
	}
	if (!string_append (&lib_filename, ".a")) {
		string_destroy (lib_filename);
		compile_debug_allocate_memory ();
		return -1;
	}
	if (!(file = directory_find_file (directory, lib_filename))) {
		string_destroy (lib_filename);
		return 1;
	}
	string_destroy (lib_filename);
	if (file->modified == 0) {
		return 1;
	}
	if (file->modified > modified) {
		return 1;
	}
	return 0;
}

static char *link_flags (Directory *directory)
{
        File *file;
        FileReader *reader;
        char *flags;
        size_t length;

        if (!(file = directory_find_file (directory, ".link"))) {
                return NULL;
        }
        if (!(reader = file_reader_create (file->path))) {
                return NULL;
        }
        if (!(flags = string_create ((const char *)reader->map))) {
		compile_debug_allocate_memory ();
                return NULL;
        }
        length = string_length (flags);
        if (length > 0 && flags[length - 1] == '\n') {
                flags[length - 1] = '\0';
        }
        file_reader_destroy (reader);
        return flags;
}
