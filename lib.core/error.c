#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/threads-lock.h>

static ErrorItem   items[ERROR_ITEMS_MAX];
static size_t      literals_count = 0;
static const char *literals[] = {
#undef ERROR
#undef ERROR_STRING
#define ERROR_STRING(e) #e,
#define ERROR(e) ERROR_STRING(Error##e)
#include <lib.core/error-enums.h>
#include <lib.core/error-enums-pattern.h>
#include <lib.core/error-enums-app.h>
#include <lib.core/error-enums-graphics.h>
	NULL
#undef ERROR_STRING
#undef ERROR
};

static uint64_t count = 0;
static bool silent = true;
static void print_item (ErrorItem item, uint64_t item_count);
static ThreadLock lock = THREAD_LOCK_INITIALIZER;

#define LOCK() \
        if (!thread_lock (&lock)) { \
                fprintf (stderr, "%s: Failure to lock!\n", __func__); \
                fflush (stderr); \
        }
#define UNLOCK() \
        if (!thread_unlock (&lock)) { \
                fprintf (stderr, "%s: Failure to unlock!\n", __func__); \
                fflush (stderr); \
        }

void error_add (const char *file, const char *function, int line, int code, Error error)
{
	size_t i = ERROR_ITEMS_MAX;
	int invalid_error;

        LOCK ();
	if (literals_count == 0) {
		while (literals[literals_count++]);
	}
	if (error <= 1 || 
	    error >= literals_count - 1 ||
	    !file ||
	    !function) {
		invalid_error = error;
		error = ErrorInvalidError;
	}
	else {
		invalid_error = 0;
	}
	while (i-- > 1) {
		items[i].file = items[i - 1].file;
		items[i].function = items[i - 1].function;
		items[i].line = items[i - 1].line;
		items[i].code = items[i - 1].code;
		items[i].error = items[i - 1].error;
		items[i].literal = items[i - 1].literal;
		items[i].invalid_error = items[i - 1].invalid_error;
	}
	items[0].file = file;
	items[0].function = function;
	items[0].line = line;
	items[0].code = code;
	items[0].error = error;
	items[0].literal = literals[(size_t)error];
	items[0].invalid_error = invalid_error;
        print_item (items[0], ++count);
        UNLOCK ();
}

void error_reset (void)
{
	size_t i = ERROR_ITEMS_MAX;

        LOCK ();
	while (i-- > 0) {
		items[i].file = NULL;
		items[i].function = NULL;
		items[i].line = 0;
		items[i].code = 0;
		items[i].error = ErrorReserved;
		items[i].literal = NULL;
		items[i].invalid_error = 0;
	}
        count = 0;
        UNLOCK ();
}

size_t error_count (void)
{
	size_t i = 0;

        LOCK ();
	while (i < ERROR_ITEMS_MAX && items[i].error != ErrorReserved) {
		i++;
	}
        UNLOCK ();
	return i;
}

ErrorItem error_at (size_t index)
{
	ErrorItem item = { NULL, NULL, 0, 0, 0, NULL, 0 };

	if (index >= ERROR_ITEMS_MAX) {
		return item;
	}
        LOCK ();
	item.file = items[index].file;
	item.function = items[index].function;
	item.line = items[index].line;
	item.code = items[index].code;
	item.error = items[index].error;
	item.literal = items[index].literal;
	item.invalid_error = items[index].invalid_error;
        UNLOCK ();
	return item;
}

void error_silent (bool state)
{
        LOCK ();
        silent = state;
        UNLOCK ();
}

void error_print_all (void)
{
        bool silent_copy = silent;
        size_t i, m;

        m = i = error_count ();
        while (i-- > 0) {
                print_item (error_at (i), m - i);
        }
        error_silent (false);
        error_silent (silent_copy);
}

static void print_item (ErrorItem item, uint64_t item_count)
{
        if (!silent) {
                fflush (stdout);
                fprintf (stderr, 
                         ">| Error %llu:\n"
                                ">|\t File:     %s\n"
                                ">|\t Function: %s\n"
                                ">|\t Line:     %i\n"
                                ">|\t Enum:     %s",
                        (unsigned long long)item_count,
                        item.file,
                        item.function,
                        item.line,
                        item.literal);
                if (item.code != 0) {
                        fprintf (stderr, " #%i\n", (int)item.code);
                }
                else {
                        fprintf (stderr, "\n");
                }
                fflush (stderr);
        }
}
