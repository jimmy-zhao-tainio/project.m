#ifndef arguments_usage_h
#define arguments_usage_h

#include <lib.app/arguments-structures.h>

void app_arguments_usage (int argc, char **argv, AppArgument *arguments);
void app_arguments_print_named_form (AppArgument argument);

#endif
