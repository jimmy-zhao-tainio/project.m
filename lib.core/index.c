#include <lib.core/index.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

Index *index_create (size_t bits)
{
        Index *index;
        size_t array_count;
        size_t bit_count;

        for (array_count = 0, bit_count = 0; bit_count < bits; array_count++) {
                if (bit_count == 0) {
                        bit_count = 8;
                }
                else if (!size_t_mul (bit_count, 8, &bit_count)) {
                        bit_count = bits;
                }
        }
        if (array_count == 0) {
                error (InvalidOperation);
                return NULL;
        }
        if (!(index = memory_create (sizeof (Index)))) {
                error (FunctionCall);
                return NULL;
        }
        if (!(index->array = memory_create (sizeof (unsigned char *) * array_count))) {
                memory_destroy (index);
                error (Overflow);
                return NULL;
        }
        for (array_count = 0, bit_count = 0; bit_count < bits; array_count++) {
                if (bit_count == 0) {
                        bit_count = 8;
                }
                else if (!size_t_mul (bit_count, 8, &bit_count)) {
                        bit_count = bits;
                }
                if (!(index->array[array_count] = memory_create ((bit_count / 8) + ((bits % 8) == 0 ? 0 : 1)))) {
                        while (array_count --> 0) {
                                memory_destroy (index->array[array_count]);
                        }
                        memory_destroy (index->array);
                        memory_destroy (index);
                        error (FunctionCall);
                        return NULL;
                }
        }
        return index;
}
