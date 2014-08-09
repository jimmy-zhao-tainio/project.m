#ifndef print_h
#define print_h

#include <lib.core/defines.h>

bool print (const char *format, ...);

void print_silent (bool state);
bool print_log_begin (const char *path);
bool print_log_end (void);

#endif
