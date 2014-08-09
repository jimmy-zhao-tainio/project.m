#include <stdio.h>
#include <stdarg.h>

#include <lib.compile/compile-print.h>

static bool enabled = true;

void compile_print (const char *format, ...)
{
	va_list args;

        if (!enabled) {
                return;
        }
	va_start (args, format);
	vprintf (format, args);
	va_end (args);
}

void compile_print_set_enabled (bool state)
{
	enabled = state;
}

bool compile_print_get_enabled (void)
{
	return enabled;
}
