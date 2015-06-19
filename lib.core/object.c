#include <lib.core/object.h>
#include <lib.core/object-private.h>
#include <lib.core/error.h>
#include <lib.core/threads-lock.h>

static ThreadLock lock = THREAD_LOCK_INITIALIZER;
static unsigned long long counter = 0;

unsigned long long object_id (void)
{
	unsigned long long id;
        
        if (!thread_lock (&lock)) {
                error_code (FunctionCall, 1);
                return 0;
        }
        id = counter;
	if (!unsigned_long_long_add (counter, 1, &counter)) {
		error (Overflow);
		counter = 0;
	}
        if (!thread_unlock (&lock)) {
                error_code (FunctionCall, 2);
                return 0;
        }
	return id;
}

void object_id_set_counter (unsigned long long c)
{
	counter = c;
}
