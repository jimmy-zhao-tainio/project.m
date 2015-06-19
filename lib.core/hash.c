#include <lib.core/hash.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

Hash *hash_create (void)
{
	Hash *hash;

	if (!(hash = memory_create (sizeof (Hash)))) {
		error (FunctionCall);
		return NULL;
	}
        hash->count = 0;
	return hash;
}

void hash_destroy (Hash *hash)
{
	if (!hash) {
		error (InvalidArgument);
		return;
	}
	memory_destroy (hash);
}
