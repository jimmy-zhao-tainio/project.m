#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <string.h>
#include <stdio.h>

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
        reader->request.end = 0;
        return true;
}

bool net_http_reader_append (NetHttpReader *reader, char *buffer, size_t length)
{
        void *grown;

        while (reader->length + length > reader->size) {
                if (reader->size == 1024 * 2 * 2 * 2 * 2) {
                        error (InvalidOperation);
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
        while (reader->request.end + 3 < reader->length) {
                if (reader->buffer[reader->request.end + 0] == '\r' && 
                    reader->buffer[reader->request.end + 1] == '\n' &&
                    reader->buffer[reader->request.end + 2] == '\r' &&
                    reader->buffer[reader->request.end + 3] == '\n') {
                        reader->request.end += 4;
                        return true;
                }
                reader->request.end += 1;
        }
        return false;
}

void net_http_request_end (NetHttpReader *reader)
{
        if (reader->length == reader->request.end) {
                reader->request.end = 0;
                reader->length = 0;
                return;
        }
        memmove (reader->buffer, 
                 reader->buffer + reader->request.end, 
                 reader->length - reader->request.end);
        reader->length = reader->length - reader->request.end;
        reader->request.end = 0;
}

bool net_http_set_method (NetHttpReader *reader)
{
        reader->request.method_get = false;
        reader->request.method_head = false;

        if (reader->length > 3 &&
            reader->buffer[0] == 'G' &&
            reader->buffer[1] == 'E' &&
            reader->buffer[2] == 'T' &&
            reader->buffer[3] == ' ') {
                reader->request.method_get = true;
                reader->request.method_length = 3;
                return true;
        }
        else if (reader->length > 4 &&
                 reader->buffer[0] == 'H' &&
                 reader->buffer[1] == 'E' &&
                 reader->buffer[2] == 'A' &&
                 reader->buffer[3] == 'D' &&
                 reader->buffer[4] == ' ') {
                reader->request.method_head = true;
                reader->request.method_length = 4;
                return true;
        }
        reader->request.method_length = 0;
        return false;
}

bool net_http_set_uri (NetHttpReader *reader)
{
        size_t uri_end;

        reader->request.uri_begin = reader->request.method_length + 1;
        uri_end = reader->request.uri_begin;
        while (uri_end < reader->length && reader->buffer[uri_end] != ' ') {
                uri_end++;
        }
        if (uri_end == reader->length) {
                return false;
        }
        if (uri_end == reader->request.uri_begin) {
                return false;
        }
        reader->request.uri_length = uri_end - reader->request.uri_begin;
        return true;
}

bool net_http_set_version (NetHttpReader *reader)
{
        reader->request.version_begin = reader->request.uri_begin + 
                                        reader->request.uri_length +
                                        1 /* ' ' */;
        reader->request.version_length = 8; /* "HTTP/X.X"; */
        if (reader->request.version_begin + 
            reader->request.version_length +
            2 /* "\r\n" */ <= reader->length) {
                return true;
        }
        return false;
}

static bool is_line_break (char *buffer, size_t length, size_t index);
static bool find_line_break (char *buffer, size_t length, size_t *index);

bool net_http_set_headers (NetHttpReader *reader)
{
        size_t headers_end;

        reader->request.headers_count = 0;
        reader->request.headers_begin = 
                reader->request.version_begin +
                reader->request.version_length + 
                2 /* "\r\n" */;
        headers_end = reader->request.headers_begin;
        while (true) {
                // Check for empty line.
                if (is_line_break (reader->buffer, 
                                   reader->request.end, 
                                   headers_end)) {
                        headers_end++; // Position '\n' in '\r\n'.
                        reader->request.headers_length = 
                                (headers_end - 
                                 reader->request.headers_begin) + 1;
                        return true;
                }
                // Find ":"
                while (headers_end < reader->request.end &&
                       reader->buffer[headers_end] != ':') {
                        headers_end++;
                }
                if (headers_end == reader->request.end) {
                        return false;
                }
                // Skip ":"
                headers_end += 1;
                // Find "\r\n"
                if (!find_line_break (reader->buffer,
                                      reader->request.end,
                                      &headers_end)) {
                        return false;
                }
                // Skip "\r\n"
                headers_end += 2;
                reader->request.headers_count++;
        }
}

char *net_http_get_header (NetHttpReader *reader, char *header)
{
        size_t index;
        size_t length;
        size_t begin;
        char *value;

        index = reader->request.headers_begin;
        length = strlen (header);
        while (index + length <
               reader->request.headers_begin + reader->request.headers_length) {
                if (strncmp (reader->buffer + index, header, length) != 0) {
                        // Skip line.
                        // Find ":"
                        while (index < reader->request.end &&
                               reader->buffer[index] != ':') {
                                index++;
                        }
                        if (index == reader->request.end) {
                                return NULL;
                        }
                        // Skip ":"
                        index += 1;
                        // Find "\r\n"
                        if (!find_line_break (reader->buffer,
                                              reader->request.end,
                                              &index)) {
                                return NULL;
                        }
                        // Skip "\r\n"
                        index += 2;
                        continue;
                }
                // Find ':'
                while (index < reader->request.end &&
                       reader->buffer[index] != ':') {
                        index++;
                }
                if (index == reader->request.end) {
                        return NULL;
                }
                index += 1;
                // Skip ' '
                while (index < reader->request.end &&
                       reader->buffer[index] == ' ') {
                        index++;
                }
                if (index == reader->request.end) {
                        return NULL;
                }
                begin = index;
                while (index < reader->request.end &&
                       reader->buffer[index] != '\r') {
                        index++;
                }
                if (index == reader->request.end) {
                        return NULL;
                }
                if (begin == index) {
                        return NULL;
                }
                if (!(value = string_create_with_size ((index - begin) + 1))) {
                        error (FunctionCall);
                        return NULL;
                }
                (void)memmove (value, reader->buffer + begin, index - begin);
                return value;
        }
        return NULL;
}

static bool find_line_break (char *buffer, size_t length, size_t *index)
{
        while (*index + 1 < length) {
                if (buffer[*index + 0] == '\r' &&
                    buffer[*index + 1] == '\n') {
                        return true;
                }
                *index = *index + 1;
        }
        return false;
}

static bool is_line_break (char *buffer, size_t length, size_t index)
{
        if (index + 1 < length &&
            buffer[index + 0] == '\r' &&
            buffer[index + 1] == '\n') {
                return true;
        }
        return false;
}
