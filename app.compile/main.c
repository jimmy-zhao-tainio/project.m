#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib.core/defines.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.compile/compile-project.h>
#include <lib.compile/compile-print.h>
#include <lib.compile/compile.h>

static bool try (int argc, char **argv, CompileProject **project, char **path, char **name, char **sub_path, char **sub_name);

int main (int argc, char **argv)
{
	CompileProject *project = NULL;
	char *path = NULL;
	char *name = NULL;
	char *sub_path = NULL;
	char *sub_name = NULL;
	bool result;
        
	result = try (argc, argv, &project, &path, &name, &sub_path, &sub_name);
	if (project) {
		compile_project_destroy (project);
	}
	if (path) {
		string_destroy (path);
	}
	if (name) {
		string_destroy (name);
	}
	if (sub_path) {
		string_destroy (sub_path);
	}
	if (sub_name) {
		string_destroy (sub_name);
	}
	return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

static bool try (int argc, char **argv, CompileProject **project, char **path, char **name, char **sub_path, char **sub_name)
{
        bool bootstrap;

        if (argc == 2 && string_equals (argv[1], "--bootstrap")) {
                bootstrap = true;
        }
        else {
                bootstrap = false;
        }
	if (!(*path = directory_current_path ())) {
		compile_print ("Failed to get current path.\n");
		return false;
	}
	if (!(*name = file_name_from_path (*path))) {
		compile_print ("Failed to get name from path.\n");
		return false;
	}
	if (string_begins_with (*name, "project.")) {
		if (!(*project = compile_project_create (*path)) ||
		    !compile_project_prepare (*project) ||
		    !compile_project_execute (*project, bootstrap)) {
			return false;
		}
		else {
			return true;
		}
	}
	else if (string_begins_with (*name, "lib.") ||
	         string_begins_with (*name, "app.")) {
		if (!(*sub_path = directory_sub_path (*path))) {
			return false;
		}
		if (!(*sub_name = file_name_from_path (*sub_path))) {
			return false;
		}
		if (!string_begins_with (*sub_name, "project.")) {
			return false;
		}
		if (!(*project = compile_project_create (*sub_path)) ||
		    !compile_project_prepare (*project) ||
		    !compile_project_execute_with_directory_name (*project, *name, bootstrap)) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		compile_print ("Directory name must begin with either 'project.', 'app.' or 'lib.'.\n");	
	}
	return false;
}
