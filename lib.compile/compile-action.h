#ifndef compile_action_h
#define compile_action_h

#include <lib.compile/compile-structures.h>

CompileAction *compile_action_create (CompileActionType type);
void compile_action_destroy (CompileAction *action);

#endif
