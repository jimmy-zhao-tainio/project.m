#ifndef random_h
#define random_h

#include <lib.core/defines.h>

bool random_open (void);
bool random_bytes (unsigned char *bytes, size_t bytes_length);
bool random_value (unsigned long long from, unsigned long long to, unsigned long long *value);
void random_close (void);

#endif
