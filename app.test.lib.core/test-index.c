#include <lib.core/index.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <stdio.h>

#include "test-index.h"

bool test_index_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (index_create (0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_index_create_overflow_1 (Test *test)
{
	TITLE ();
	CATCH (index_create (SIZE_MAX));
	CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_index_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_total_create_limit (368 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_index_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (384 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_index_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_total_create_limit (400 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_index_create_function_call_4 (Test *test)
{
        TITLE ();
        memory_total_create_limit (409 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_index_create_1 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (1)));
        CATCH (index->bits != 1);
        CATCH (index->power != 1);
        CATCH (memory_size (index->power_bits) != 1 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (memory_size (index->power_offset) != 1 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (memory_size (index->map) != 1);
        index_destroy (index);
        PASS ();
}

bool test_index_create_2 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        CATCH (index->bits != 8);
        CATCH (index->power != 1);
        CATCH (memory_size (index->power_bits) != 1 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (memory_size (index->power_offset) != 1 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (memory_size (index->map) != 1);
        index_destroy (index);
        PASS ();
}

bool test_index_create_3 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (9)));
        CATCH (index->bits != 9);
        CATCH (index->power != 2);
        CATCH (memory_size (index->power_bits) != 2 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (index->power_bits[1] != 64);
        CATCH (memory_size (index->power_offset) != 2 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (index->power_offset[1] != 1);
        CATCH (memory_size (index->map) != 1 + 1 + 1);
        index_destroy (index);
        PASS ();
}

bool test_index_create_4 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (16)));
        CATCH (index->bits != 16);
        CATCH (index->power != 2);
        CATCH (memory_size (index->power_bits) != 2 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (index->power_bits[1] != 64);
        CATCH (memory_size (index->power_offset) != 2 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (index->power_offset[1] != 1);
        CATCH (memory_size (index->map) != 1 + 2);
        index_destroy (index);
        PASS ();
}

bool test_index_create_5 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        CATCH (index->bits != 64);
        CATCH (index->power != 2);
        CATCH (memory_size (index->power_bits) != 2 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (index->power_bits[1] != 64);
        CATCH (memory_size (index->power_offset) != 2 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (index->power_offset[1] != 1);
        CATCH (memory_size (index->map) != 1 + 8);
        index_destroy (index);
        PASS ();
}

bool test_index_create_6 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (512)));
        CATCH (index->bits != 512);
        CATCH (index->power != 3);
        CATCH (memory_size (index->power_bits) != 3 * sizeof (size_t));
        CATCH (index->power_bits[0] != 8);
        CATCH (index->power_bits[1] != 64);
        CATCH (index->power_bits[2] != 512);
        CATCH (memory_size (index->power_offset) != 3 * sizeof (size_t));
        CATCH (index->power_offset[0] != 0);
        CATCH (index->power_offset[1] != 1);
        CATCH (index->power_offset[2] != 9);
        CATCH (memory_size (index->map) != 1 + 8 + 64);
        index_destroy (index);
        PASS ();
}
