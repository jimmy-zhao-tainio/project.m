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
        index_set (NULL, 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_index_set_invalid_operation (Test *test)
{
        Index index;

        TITLE ();
        index.bits = 1;
        index_set (&index, 1);
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
                index_set (index, i);
                CATCH (unsigned_char_bit_get (index->map[0], i) != true);
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
                        index_set (index, (b * 8) + i);
                        CATCH (unsigned_char_bit_get (index->map[b + 1], i) != true);
                }
                CATCH (unsigned_char_bit_get (index->map[0], b) != true);
        }
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_invalid_argument (Test *test)
{
        TITLE ();
	index_set_range (NULL, 0, 0);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_index_set_range_invalid_operation_1 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 8, 8);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 1);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_invalid_operation_2 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 7, 8);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 2);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_invalid_operation_3 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 7, 6);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 3);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_1 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 0, 0);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_2 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 0, 1);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_3 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 1, 1);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_4 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 1, 6);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 6) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_5 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 1, 7);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 7) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_6 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        index_set_range (index, 0, 7);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 7) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_7 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set_range (index, 0, 0);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != false);
        CATCH (unsigned_char_bit_get (index->map[1], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 1) != false);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_8 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set_range (index, 1, 7);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 7) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_9 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set_range (index, 1, 8);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 7) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 0) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_10 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set_range (index, 1, 15);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 2) != false);
        CATCH (unsigned_char_bit_get (index->map[1], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 7) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 7) != true);
        CATCH (unsigned_char_bit_get (index->map[3], 0) != false);
        index_destroy (index);
        PASS ();
}

bool test_index_set_range_11 (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set_range (index, 1, 16);
        CATCH (unsigned_char_bit_get (index->map[0], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[0], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[1], 7) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 0) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 1) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 2) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 3) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 4) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 5) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 6) != true);
        CATCH (unsigned_char_bit_get (index->map[2], 7) != true);
        CATCH (unsigned_char_bit_get (index->map[3], 0) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_get_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (index_get (NULL, 0));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_index_get_invalid_operation (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (8)));
        CATCH (index_get (index, 8));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        index_destroy (index);
        PASS ();
}

bool test_index_get (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (64)));
        index_set (index, 0);
        index_set (index, 63);
        CATCH (index_get (index, 0) != true);
        CATCH (index_get (index, 1) != false);
        CATCH (index_get (index, 63) != true);
        index_destroy (index);
        PASS ();
}

bool test_index_clear_invalid_argument (Test *test)
{
        TITLE ();
        index_clear (NULL);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_index_clear (Test *test)
{
        Index *index;
        size_t position;

        TITLE ();
        CATCH (!(index = index_create (128)));
        for (position = 0; position < 128; position += 2) {
                index_set (index, position);
        }
        index_clear (index);
        for (position = 0; position < 128; position++) {
                CATCH (index_get (index, position) != 0);
        }
        index_destroy (index);
        PASS ();
}
