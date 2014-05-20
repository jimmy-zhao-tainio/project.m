#ifndef memory_h
#define memory_h

#include <lib.core/defines.h>

void   *memory_create  (size_t size);
size_t  memory_size    (void *memory);
void   *memory_grow    (void *memory, size_t size);
void    memory_destroy (void *memory);

unsigned long long memory_total_create_size   (void);
unsigned long long memory_total_destroy_size  (void);
unsigned long long memory_commit_size         (void);

void memory_commit_limit (unsigned long long limit);
void memory_total_create_limit (unsigned long long limit);
unsigned long long memory_commit_limit_get (void);
unsigned long long memory_total_create_limit_get (void);
void memory_total_reset (void);

#endif
