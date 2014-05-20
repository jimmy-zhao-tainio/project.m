#include <lib.core/object.h>
#include <lib.core/object-private.h>
#include <lib.core/error.h>

static unsigned long long counter = 0;

unsigned long long object_id (void)
{
	unsigned long long id = counter;

	if (!unsigned_long_long_add (counter, 1, &counter)) {
		error (Overflow);
		counter = 0;
	}
	return id;
}

void object_id_set_counter (unsigned long long c)
{
	counter = c;
}
