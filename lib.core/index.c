#include <lib.core/index.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <string.h>
#include <stdio.h>

static void get_map_index (Index *index, 
                           size_t power, 
                           size_t position, 
                           size_t *byte_index, 
                           unsigned int *bit_index);
static void set_range (Index *index, 
                       size_t from_byte, 
                       unsigned int from_bit, 
                       size_t to_byte, 
                       unsigned int to_bit);
static bool set_range_partial_from (Index *index, 
                                    size_t *from_byte, 
                                    unsigned int *from_bit, 
                                    size_t to_byte, 
                                    unsigned int to_bit);
static bool set_range_partial_to (Index *index, 
                                  size_t from_byte, 
                                  unsigned int from_bit, 
                                  size_t *to_byte, 
                                  unsigned int *to_bit);
static void set_range_middle (Index *index, 
                              size_t from_byte, 
                              size_t to_byte);

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

void index_set (Index *index, size_t position)
{
        size_t power;
        size_t byte;
        unsigned int bit;

        if (!index) {
                error (InvalidArgument);
                return;
        }
        if (position >= index->bits) {
                error (InvalidOperation);
                return;
        }
        for (power = 1; power <= index->power; power++) {
                get_map_index (index, power, position, &byte, &bit);
                index->map[byte] = unsigned_char_bit_set (index->map[byte], bit, true);
        }
}

void index_set_range (Index *index, size_t from, size_t to)
{
        size_t from_byte;
        size_t to_byte;
        size_t power;
        unsigned int from_bit;
        unsigned int to_bit;

        if (!index) {
                error (InvalidArgument);
                return;
        }
        if (from >= index->bits) {
                error_code (InvalidOperation, 1);
                return;
        }
        if (to >= index->bits) {
                error_code (InvalidOperation, 2);
                return;
        }
        if (from > to) {
                error_code (InvalidOperation, 3);
                return;
        }
        for (power = 1; power <= index->power; power++) {
                get_map_index (index, power, from, &from_byte, &from_bit);
                get_map_index (index, power, to, &to_byte, &to_bit);
                set_range (index, from_byte, from_bit, to_byte, to_bit);
        }
}

bool index_get (Index *index, size_t position)
{
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
        get_map_index (index, index->power, position, &byte, &bit);
        return unsigned_char_bit_get (index->map[byte], bit);
}

void index_clear (Index *index)
{
        if (!index) {
                error (InvalidArgument);
                return;
        }
        bzero (index->map, memory_size (index->map));
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

static void set_range (Index *index, 
                       size_t from_byte, 
                       unsigned int from_bit, 
                       size_t to_byte, 
                       unsigned int to_bit)
{
        if (set_range_partial_from (index, &from_byte, &from_bit, to_byte, to_bit)) {
                return;
        }
        if (set_range_partial_to (index, from_byte, from_bit, &to_byte, &to_bit)) {
                return;
        }
        set_range_middle (index, from_byte, to_byte);
}

static bool set_range_partial_from (Index *index, 
                                    size_t *from_byte, 
                                    unsigned int *from_bit, 
                                    size_t to_byte, 
                                    unsigned int to_bit)
{
        unsigned int bit;

        if (*from_bit == 0) {
                return false;
        }
        if (*from_byte == to_byte) {
                for (bit = *from_bit; bit <= to_bit; bit++) {
                        index->map[*from_byte] = unsigned_char_bit_set (index->map[*from_byte], bit, true);
                }
                return true;
        }
        for (bit = *from_bit; bit <= 7; bit++) {
                index->map[*from_byte] = unsigned_char_bit_set (index->map[*from_byte], bit, true);
        }
        *from_byte += 1;
        *from_bit = 0;
        return false;
}

static bool set_range_partial_to (Index *index, 
                                  size_t from_byte, 
                                  unsigned int from_bit, 
                                  size_t *to_byte, 
                                  unsigned int *to_bit)
{
        unsigned int bit;

        if (*to_bit == 7) {
                return false;
        }
        if (*to_byte == from_byte) {
                for (bit = from_bit; bit <= *to_bit; bit++) {
                        index->map[*to_byte] = unsigned_char_bit_set (index->map[*to_byte], bit, true);
                }
                return true;
        }
        // Since to_byte isn't from_byte, there is a bit at 0 and to_byte - 1 must be greater or equal to from_byte.
        for (bit = 0; bit <= *to_bit; bit++) {
                index->map[*to_byte] = unsigned_char_bit_set (index->map[*to_byte], bit, true);
        }
        *to_byte -= 1;
        *to_bit = 7;
        return false;
}

static void set_range_middle (Index *index,
                              size_t from_byte,
                              size_t to_byte)
{
        memset (&index->map[from_byte], 0xFF, (to_byte - from_byte) + 1);
}
