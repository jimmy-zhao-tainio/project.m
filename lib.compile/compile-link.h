#ifndef compile_link_h
#define compile_link_h

#include <lib.compile/compile-structures.h>

bool compile_link_library (Compile *compile);
bool compile_link_application (Compile *compile);
bool compile_link_plugin (Compile *compile);

#endif
