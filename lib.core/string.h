#ifndef string_h
#define string_h

#include <lib.core/defines.h>

char *string_create           (const char *c);
char *string_create_with_size (size_t size);
void  string_destroy          (char *string);

size_t string_length      (const char *string);
bool   string_append      (char **string, const char *c);
bool   string_begins_with (const char *string, const char *begin);
bool   string_ends_with   (const char *string, const char *end);
bool   string_equals      (const char *string, const char *match);
bool   string_index       (const char *string, char match, size_t *index);
char **string_split	  (const char *string, char separator);

#endif
