#ifndef file_writer_h
#define file_writer_h

#include <lib.core/defines.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct s_FileWriter FileWriter;

typedef enum {
        FileWriterModeAppend,
        FileWriterModeTruncate
} FileWriterMode;

struct s_FileWriter
{
        FILE *file;
};

FileWriter *file_writer_create (const char *path, FileWriterMode mode);
bool file_writer_write (FileWriter *writer, const char *buffer, size_t length);
bool file_writer_write_args (FileWriter *writer, const char *format, ...);
bool file_writer_write_va_list (FileWriter *writer, const char *format, va_list args);
void file_writer_destroy (FileWriter *writer);

#endif
