#ifndef hash_h
#define hash_h

#include <lib.core/defines.h>

typedef struct _Hash Hash;

struct _Hash
{
	size_t count;
};

Hash *hash_create  (void);
void  hash_destroy (Hash *hash);

#endif
