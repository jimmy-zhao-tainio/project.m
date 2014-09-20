#ifndef compile_h
#define compile_h

#include <lib.compile/compile-structures.h>

Compile *compile_create (Directory *project, Directory *directory);
bool compile_prepare (Compile *compile);
bool compile_actions (Compile *compile, const char *project_path);
bool compile_execute (Compile *compile, bool bootstrap);
void compile_destroy (Compile *compile);

#endif
