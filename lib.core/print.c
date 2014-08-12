#include <lib.core/print.h>
#include <lib.core/error.h>
#include <lib.core/file-writer.h>
#include <stdio.h>

static bool silent = false;
static bool log = false;
static FileWriter *log_writer;
static uint64_t count = 0;

bool print (const char *format, ...)
{
        va_list args;

        va_start (args, format);
        count++;
        if (!silent) {
                if (vprintf (format, args) < 0) {
                        va_end (args);
                        error (SystemCall);
                        return false;
                }
                fflush (stdout);
        }
        if (log) {
                if (!file_writer_write_va_list (log_writer, format, args)) {
                        va_end (args);
                        error (FunctionCall);
                        return false;
                }
        }
        va_end (args);
        return true;
}

void print_silent (bool state)
{
        silent = state;
}

bool print_log_begin (const char *path)
{
        if (!path) {
                error (InvalidArgument);
                return false;
        }
        if (log) {
                error (InvalidOperation);
                return false;
        }
        if (!(log_writer = file_writer_create (path, FileWriterModeAppend))) {
                error (FunctionCall);
                return false;
        }
        log = true;
        return true;
}

bool print_log_end (void)
{
        if (!log) {
                error (InvalidOperation);
                return false;
        }
        file_writer_destroy (log_writer);
        log = false;
        return true;
}

uint64_t print_count (void)
{
        return count;
}
