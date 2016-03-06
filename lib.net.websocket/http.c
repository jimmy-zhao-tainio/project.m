#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <string.h>

#include "http.h"

bool net_http_reader_create (NetHttpReader *reader)
{
        if (reader->initialized) {
                return true;
        }
        if (!(reader->buffer = memory_create (1024))) {
                error (FunctionCall);
                return false;
        }
        reader->size = 1024;
        reader->length = 0;
        reader->initialized = true;
        reader->request_end = 0;
        return true;
}

bool net_http_reader_append (NetHttpReader *reader, char *buffer, size_t length)
{
        void *grown;

        while (reader->length + length > reader->size) {
                if (reader->size == 1024 * 2 * 2 * 2 * 2) {
                        error (FunctionCall);
                        return false;
                }
                if (!(grown = memory_grow (reader->buffer, reader->size * 2))) {
                        error (FunctionCall);
                        return false;
                }
                reader->buffer = grown;
                reader->size = reader->size * 2;
        }
        (void)memcpy (reader->buffer + reader->length, buffer, length);
        reader->length = reader->length + length;
        return true;
}

void net_http_reader_destroy (NetHttpReader *reader)
{
        if (reader->initialized == false) {
                return;
        }
        if (reader->buffer) {
                memory_destroy (reader->buffer);
        }
        reader->buffer = NULL;
        reader->size = 0;
        reader->length = 0;
        reader->initialized = false;
}

bool net_http_request_begin (NetHttpReader *reader)
{
        while (reader->request_end + 3 < reader->length) {
                if (reader->buffer[reader->request_end + 0] == '\r' && 
                    reader->buffer[reader->request_end + 1] == '\n' &&
                    reader->buffer[reader->request_end + 2] == '\r' &&
                    reader->buffer[reader->request_end + 3] == '\n') {
                        reader->request_end += 4;
                        return true;
                }
                reader->request_end += 1;
        }
        return false;
}

void net_http_request_end (NetHttpReader *reader)
{
        (void)reader;
}
