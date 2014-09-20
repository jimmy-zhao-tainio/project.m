#ifndef compile_project_h
#define compile_project_h

#include <lib.compile/compile-structures.h>

CompileProject *compile_project_create (const char *path);
bool compile_project_prepare (CompileProject *project);
bool compile_project_execute (CompileProject *project, bool bootstrap);
bool compile_project_execute_with_directory_name (CompileProject *project, const char *directory_name, bool bootstrap);
void compile_project_destroy (CompileProject *project);

#endif
