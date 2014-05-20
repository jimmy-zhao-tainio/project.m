#include <stdio.h>
#include <stdarg.h>

#include <lib.compile/compile-print.h>

static int is_enabled = 1;

void compile_print (const char *format, ...)
{
	va_list args;
	va_start (args, format);

	if (is_enabled == 0) {
		return;
	}
	vprintf (format, args);
	va_end (args);
}

void compile_print_set_enabled (int enabled)
{
	is_enabled = enabled;
}

int compile_print_get_enabled (void)
{
	return is_enabled;
}
