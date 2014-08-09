#ifndef compile_print_h
#define compile_print_h

#include <lib.core/defines.h>

void compile_print (const char *format, ...);
void compile_print_set_enabled (bool state);
bool compile_print_get_enabled (void);

#define compile_debug() compile_print ("%s(%i): ", __FILE__, __LINE__);
#define compile_debug_allocate_memory() \
	compile_debug (); \
	compile_print ("Failed to allocate memory.\n");
#define compile_debug_invalid_arguments() \
	compile_debug (); \
	compile_print ("Invoked with invalid arguments.\n");
#define compile_debug_invalid_value() \
	compile_debug (); \
	compile_print ("Invalid value.\n");
#define compile_debug_operation_failed() \
	compile_debug (); \
	compile_print ("Operation failed.\n");

#endif
