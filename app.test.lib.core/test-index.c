#include <lib.core/index.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <string.h>
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

bool test_index_destroy_invalid_argument (Test *test)
{
	TITLE ();
	index_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_index_set_invalid_argument (Test *test)
{
        TITLE ();
        index_set (NULL, 0, false);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_index_set_invalid_operation (Test *test)
{
        Index index;

        TITLE ();
        index.bits = 1;
        index_set (&index, 1, false);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        PASS ();
}

bool test_index_set_1 (Test *test)
{
        Index *index;
        unsigned int i;

        TITLE ();
        CATCH (!(index = index_create (8)));
        for (i = 0; i < 8; i++) {
                CATCH (unsigned_char_bit_get (index->map[0], i) != false);
                index_set (index, i, true);
                CATCH (unsigned_char_bit_get (index->map[0], i) != true);
                index_set (index, i, false);
                CATCH (unsigned_char_bit_get (index->map[0], i) != false);
                CATCH (index->map[0] != 0);
        }
        index_destroy (index);
        PASS ();
}

bool test_index_set_2 (Test *test)
{
        Index *index;
        unsigned int i;
        unsigned int b;

        TITLE ();
        CATCH (!(index = index_create (16)));

        for (b = 0; b < 2; b++) {
                for (i = 0; i < 8; i++) {
                        index_set (index, (b * 8) + i, true);
                        CATCH (unsigned_char_bit_get (index->map[b + 1], i) != true);
                }
                CATCH (unsigned_char_bit_get (index->map[0], b) != true);
        }
        index_destroy (index);
        PASS ();
}
