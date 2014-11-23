#include <lib.core/index.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>

Index *index_create (size_t bits)
{
        Index *index;
        size_t power = 1;
        size_t power_bits;
        size_t bits_total = 0;

        if (bits == 0) {
                error (InvalidArgument);
                return NULL;
        }
        while (true) {
                if (!size_t_pow (8, power, &power_bits)) {
                        error_code (Overflow, 1);
                        return NULL;
                }
                if (power_bits > bits) {
                        power_bits = bits;
                }
                if (!size_t_add (bits_total, power_bits, &bits_total)) {
                        error_code (Overflow, 2);
                        return NULL;
                }
                if (power_bits >= bits) {
                        break;
                }
                power++;
        }
        if (!(index = memory_create (sizeof (Index)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(index->power_bits = memory_create (power * sizeof (size_t)))) {
                memory_destroy (index);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(index->power_offset = memory_create (power * sizeof (size_t)))) {
                memory_destroy (index->power_bits);
                memory_destroy (index);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(index->map = memory_create ((bits_total / 8) + ((bits_total % 8) == 0 ? 0 : 1)))) {
                memory_destroy (index->power_bits);
                memory_destroy (index->power_offset);
                memory_destroy (index);
                error_code (FunctionCall, 4);
                return NULL;
        }
        index->bits = bits;
        index->power = power;
        power = 1;
        while (true) {
                if (!size_t_pow (8, power, &power_bits)) {
                        error_code (Overflow, 3);
                        return NULL;
                }
                index->power_bits[power - 1] = power_bits;
                index->power_offset[power - 1] = bits_total / 8;
                if (power_bits > bits) {
                        power_bits = bits;
                }
                if (!size_t_add (bits_total, power_bits, &bits_total)) {
                        error_code (Overflow, 4);
                        return NULL;
                }
                if (power_bits >= bits) {
                        break;
                }
                power++;
        }
        return index;
}

void index_destroy (Index *index)
{
        if (!index) {
                error (InvalidArgument);
                return;
        }
        memory_destroy (index->map);
        memory_destroy (index->power_offset);
        memory_destroy (index->power_bits);
        memory_destroy (index);
}

void index_set (Index *index, size_t position, bool to_value)
{
        //     |         |         |         |         |         |         |         |
        //     0         1         2         3         4         5         6         7
        // |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| |||| ||||
        // 0123 4567 8911 1111 1111 2222 2222 2233 3333 3333 4444 4444 4455 5555 5555 6666
        //             01 2345 6789 0123 4567 8901 2345 6789 0123 4567 8901 2345 6789 0123

        size_t power;
        size_t bit_index;
        size_t offset = 0;

        power = index->power;
        while (power --> 0) {
                bit_index = (position - offset) / index->power_bits[power - 1];
                if (unsigned_char_bit_get (index->map[index->power_offset[power - 1]], (unsigned int)bit_index)) {
                        continue;
                }
                index->map[index->power_offset[power - 1]] = 
                        unsigned_char_bit_set (index->map[index->power_offset[power - 1]], 
                                               (unsigned int)bit_index, 
                                               to_value);
        }
}

bool index_get (Index *index, size_t position)
{
        (void)index;
        (void)position;
        return false;
}
