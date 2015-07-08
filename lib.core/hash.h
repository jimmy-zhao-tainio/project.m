#ifndef hash_h
#define hash_h

#include <lib.core/defines.h>
#include <lib.core/object.h>

typedef struct _Hash Hash;

struct _Hash
{
	size_t count;
};

Hash *hash_create   (void);
void  hash_destroy  (Hash *hash);

Object *hash_find   (Hash *hash, unsigned long long object_id);
bool    hash_insert (Hash *hash, Object *object);
bool    hash_remove (Hash *hash, Object *object);

#endif
