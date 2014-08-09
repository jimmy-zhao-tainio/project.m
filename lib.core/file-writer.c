#include <lib.core/file-writer.h>
#include <lib.core/file.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <stdarg.h>

FileWriter *file_writer_create (const char *path, FileWriterMode mode)
{
        FileWriter *writer;
        char *c_mode = NULL;

        if (!path) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (mode != FileWriterModeAppend &&
            mode != FileWriterModeTruncate) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!file_path_is_valid (path)) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(writer = memory_create (sizeof (FileWriter)))) {
                error_code (FunctionCall, 2);
                return NULL;
        }
        switch (mode) {
        case FileWriterModeAppend:
                c_mode = "a";
                break;
        case FileWriterModeTruncate:
                c_mode = "w";
                break;
        };
        if (!(writer->file = fopen (path, c_mode))) {
                memory_destroy (writer);
                error (SystemCall);
                return NULL;
        }
        return writer;
}

bool file_writer_write (FileWriter *writer, const char *buffer, size_t length)
{
        if (!writer) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!buffer) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (length == 0) {
                error_code (InvalidArgument, 3);
                return false;
        }
        if (fwrite (buffer, 1, length, writer->file) != length) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool file_writer_write_args (FileWriter *writer, const char *format, ...)
{
        va_list args;

        va_start (args, format);
        if (!file_writer_write_va_list (writer, format, args)) {
                va_end (args);
                return false;
        }
        va_end (args);
        return true;
}

bool file_writer_write_va_list (FileWriter *writer, const char *format, va_list args)
{
        if (!writer) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!format) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (vfprintf (writer->file, format, args) < 0) {
                error (SystemCall);
                return false;
        }
        return true;
}

void file_writer_destroy (FileWriter *writer)
{
        if (!writer) {
                error (InvalidArgument);
                return;
        }
        if (fclose (writer->file) != 0) {
                error (SystemCall);
        }
        memory_destroy (writer);
}
