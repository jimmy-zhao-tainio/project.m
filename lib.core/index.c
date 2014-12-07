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
                        index_destroy (index);
                        error_code (Overflow, 3);
                        return NULL;
                }
                index->power_bits[power - 1] = power_bits;
                if (power == 1) {
                        index->power_offset[power - 1] = 0;
                }
                else {
                        index->power_offset[power - 1] =
                                index->power_offset[power - 2] +
                                (index->power_bits[power - 2] / 8);
                }
                if (power_bits > bits) {
                        power_bits = bits;
                }
                if (!size_t_add (bits_total, power_bits, &bits_total)) {
                        index_destroy (index);
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

static void get_map_index (Index *index, 
                           size_t power, 
                           size_t position, 
                           size_t *byte_index, 
                           unsigned int *bit_index);

void index_set (Index *index, size_t position, bool to_value)
{
        size_t power;
        size_t byte;
        unsigned int bit;
        unsigned char max = (unsigned char)-1;

        if (!index) {
                error (InvalidArgument);
                return;
        }
        if (position >= index->bits) {
                error (InvalidOperation);
                return;
        }
        if (to_value == true) {
                for (power = 1; power <= index->power; power++) {
                        get_map_index (index, power, position, &byte, &bit);
                        if (unsigned_char_bit_get (index->map[byte], bit)) {
                                return;
                        }
                        if (power == index->power) {
                                index->map[byte] = unsigned_char_bit_set (index->map[byte], bit, true);
                                if (index->map[byte] != max) {
                                        return;
                                }
                                continue;
                        }
                }
                while (power --> 1) {
                        get_map_index (index, power, position, &byte, &bit);
                        index->map[byte] = unsigned_char_bit_set (index->map[byte], bit, true);
                        if (index->map[byte] != max) {
                                return;
                        }
                }
        }
        else {
                for (power = 1; power <= index->power; power++) {
                        get_map_index (index, power, position, &byte, &bit);
                        index->map[byte] = unsigned_char_bit_set (index->map[byte], bit, false);
                }
        }
}

static void get_map_index (Index *index, 
                           size_t power, 
                           size_t position, 
                           size_t *byte_index, 
                           unsigned int *bit_index)
{
        size_t bits_in_power;
        size_t bits_in_largest_power;
        size_t position_in_power;

        bits_in_power = index->power_bits[power - 1];
        bits_in_largest_power = index->power_bits[index->power - 1];
        position_in_power = (position * bits_in_power) / bits_in_largest_power;
        *byte_index = index->power_offset[power - 1] + (position_in_power / 8);
        *bit_index = position_in_power % 8;
}

bool index_get (Index *index, size_t position)
{
        size_t power;
        size_t byte;
        unsigned int bit;

        if (!index) {
                error (InvalidArgument);
                return false;
        }
        if (position >= index->bits) {
                error (InvalidOperation);
                return false;
        }
        for (power = 1; power <= index->power; power++) {
                get_map_index (index, power, position, &byte, &bit);
                if (unsigned_char_bit_get (index->map[byte], bit)) {
                        return true;
                }
        }
        return false;
}
