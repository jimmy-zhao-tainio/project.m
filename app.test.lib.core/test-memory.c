#include <stdlib.h>

#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/defines.h>
#include <lib.core/defines-private.h>
#include <lib.core/string.h>

#include "test-memory.h"

bool test_memory_create_invalid_argument (Test *test)
{
	void *memory;

	TITLE ();
	CATCH ((memory = memory_create (0)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_memory_create_overflow_1 (Test *test)
{
	TITLE ();
	CATCH (memory_create (SIZE_MAX - (sizeof (size_t) - 1)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 25);
	PASS ();
}

bool test_memory_create_overflow_2 (Test *test)
{
	char *memory;

	TITLE ();
	unsigned_long_long_private_max (sizeof (size_t) + 1);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_create (1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 29);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_create_overflow_3 (Test *test)
{
	char *memory;

	TITLE ();
	CATCH (!(memory = memory_create (1)));
	memory_destroy (memory);
	unsigned_long_long_private_max (sizeof (size_t) + 1 + 
					sizeof (size_t) + 1 - 
					1);
	CATCH (memory_create (1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 34);
	PASS ();
}

bool test_memory_create_commit_limit (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + 1 -
			     1);
	CATCH (memory_create (1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorMemoryCommitLimit);
	PASS ();
}

bool test_memory_create_total_create_limit (Test *test)
{
	TITLE ();
	memory_total_create_limit (sizeof (size_t) + 1 -
				   1);
	CATCH (memory_create (1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorMemoryTotalCreateLimit);
	PASS ();
}

bool test_memory_create_out_of_memory (Test *test)
{
	TITLE ();
	CATCH (memory_create (SIZE_MAX - sizeof (size_t)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOutOfMemory);
	PASS ();
}

bool test_memory_create_1 (Test *test)
{
	char *memory;

	TITLE ();
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_size (memory) != 1);
	memory[0] = 't';
	memory_destroy (memory);
	PASS ();
}

bool test_memory_create_2 (Test *test)
{
	char **memory;
	char *array[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	size_t i;
	size_t size = sizeof (char *) * 10;

	TITLE ();
	CATCH (!(memory = memory_create (size)));
	CATCH (error_count () != 0);
	for (i = 0; i < 10; i++) {
		memory[i] = array[i];
	}
	CATCH (memory_size (memory) != size);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_create_3 (Test *test)
{
	char *memory;
	size_t amount;
	size_t i;

	TITLE ();
	
	for (amount = 1; amount <= 1024; amount++) {
		CATCH (!(memory = memory_create (amount)));
		for (i = 0; i < amount; i++) {
			memory[i] = 'w';
		}
		memory_destroy (memory);
	}
	PASS ();
}

bool test_memory_grow_invalid_argument_1 (Test *test)
{
	void *memory;

	TITLE ();
	memory = memory_create (2);
	CATCH ((memory_grow (NULL, 3)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_invalid_argument_2 (Test *test)
{
	void *memory;

	TITLE ();
	memory = memory_create (2);
	CATCH ((memory_grow (memory, 0)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_invalid_operation (Test *test)
{
	void *memory;

	TITLE ();
	memory = memory_create (2);
	CATCH ((memory_grow (memory, 1)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_overflow_1 (Test *test)
{
	void *memory;

	TITLE ();
	memory = memory_create (1);
	CATCH ((memory_grow (memory, SIZE_MAX - 7)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 83);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_overflow_2 (Test *test)
{
	char *memory1;
	char *memory2;

	TITLE ();
	unsigned_long_long_private_max (sizeof (size_t) + 1 + 
					sizeof (size_t) + 1);
	CATCH (!(memory1 = memory_create (1)));
	CATCH (!(memory2 = memory_create (1)));
	CATCH (memory_grow (memory2, 2));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 87);
	memory_destroy (memory1);
	memory_destroy (memory2);
	PASS ();
}

bool test_memory_grow_overflow_3 (Test *test)
{
	char *memory;

	TITLE ();
	unsigned_long_long_private_max (sizeof (size_t) + 1 +
					sizeof (size_t) + 1);
	CATCH (!(memory = memory_create (1)));
	memory_destroy (memory);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_grow (memory, 2));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (error_at (0).line != 91);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_commit_limit (Test *test)
{
	char *memory;

	TITLE ();
	memory_commit_limit (sizeof (size_t) + 1);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_grow (memory, 2));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorMemoryCommitLimit);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_total_create_limit (Test *test)
{
	char *memory;

	TITLE ();
	memory_total_create_limit (sizeof (size_t) + 1);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_grow (memory, 2));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorMemoryTotalCreateLimit);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_out_of_memory (Test *test)
{
	void *memory;

	TITLE ();
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_grow (memory, SIZE_MAX - sizeof (size_t)));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOutOfMemory);
	CATCH (!string_equals (error_at (0).function, "memory_grow"));
	memory_destroy (memory);
	PASS ();
}

bool test_memory_grow_1 (Test *test)
{
	char *memory;
	size_t total_create_size;
	size_t total_destroy_size;
	size_t commit_size;

	TITLE ();
	total_create_size = memory_total_create_size ();
	total_destroy_size = memory_total_destroy_size ();
	commit_size = memory_commit_size ();
	memory = memory_create (1);
	CATCH (!(memory = memory_grow (memory, 2)));
	memory[1] = 't';
	CATCH (memory_size (memory) != 2);
	memory_destroy (memory);
	CATCH (total_create_size + sizeof (size_t) + 2 != memory_total_create_size ());
	CATCH (total_destroy_size + sizeof (size_t) + 2 != memory_total_destroy_size ());
	CATCH (commit_size != memory_commit_size ());
	PASS ();
}

bool test_memory_grow_2 (Test *test)
{
	char *memory;
	size_t total_create_size;
	size_t total_destroy_size;
	size_t commit_size;

	TITLE ();
	total_create_size = memory_total_create_size ();
	total_destroy_size = memory_total_destroy_size ();
	commit_size = memory_commit_size ();
	memory = memory_create (1000);
	CATCH (!(memory = memory_grow (memory, 2000)));
	memory[2000 - 1] = 't';
	CATCH (memory_size (memory) != 2000);
	memory_destroy (memory);
	CATCH (total_create_size + sizeof (size_t) + 2000 != memory_total_create_size ());
	CATCH (total_destroy_size + sizeof (size_t) + 2000 != memory_total_destroy_size ());
	CATCH (commit_size != memory_commit_size ());
	PASS ();
}

bool test_memory_destroy_invalid_argument (Test *test)
{
	TITLE ();
	memory_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_memory_destroy (Test *test)
{
	void *memory;
	size_t total_create_size;
	size_t total_destroy_size;
	size_t commit_size;

	TITLE ();
	total_create_size = memory_total_create_size ();
	total_destroy_size = memory_total_destroy_size ();
	commit_size = memory_commit_size ();
	memory = memory_create (1000);
	memory_destroy (memory);
	CATCH (total_create_size + sizeof (size_t) + 1000 != memory_total_create_size ());
	CATCH (total_destroy_size + sizeof (size_t) + 1000 != memory_total_destroy_size ());
	CATCH (commit_size != memory_commit_size ());
	PASS ();
}

bool test_memory_total_create_size (Test *test)
{
	char *memory;
	
	TITLE ();
	CATCH (memory_total_create_size () != 0);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_total_create_size () != sizeof (size_t) + 1);
	memory_destroy (memory);
	PASS ();
}

bool test_memory_total_destroy_size (Test *test)
{
	char *memory;
	
	TITLE ();
	CATCH (memory_total_destroy_size () != 0);
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_total_destroy_size () != 0);
	memory_destroy (memory);
	CATCH (memory_total_destroy_size () != sizeof (size_t) + 1);
	PASS ();
}

bool test_memory_size_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (memory_size (NULL) != 0);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (!string_equals (error_at (0).function, "memory_size"));
	PASS ();
}

bool test_memory_size (Test *test)
{
	char *memory;
	
	TITLE ();
	CATCH (!(memory = memory_create (1)));
	CATCH (memory_size (memory) != 1);
	memory_destroy (memory);
	CATCH (!(memory = memory_create (2)));
	CATCH (memory_size (memory) != 2);
	memory_destroy (memory);
	PASS ();
}
