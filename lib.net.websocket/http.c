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
        reader->request.begin = 0;
        reader->request.end = 0;
        reader->size = 1024;
        reader->index = 0;
        reader->initialized = true;
        return true;
}

bool net_http_reader_append (NetHttpReader *reader, char *buffer, size_t length)
{
        void *grown;

        while (reader->index + length > reader->size) {
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
        (void)memcpy (reader->buffer + reader->index, buffer, length);
        reader->index = reader->index + length;
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
        reader->index = 0;
        reader->initialized = false;
}

bool net_http_request_begin (NetHttpReader *reader, NetHttpRequest *request)
{
        (void)reader;
        (void)request;
        return true;
}

void net_http_request_end (NetHttpReader *reader, NetHttpRequest *request)
{
        (void)reader;
        (void)request;
}
