#ifndef index_h
#define index_h

#include <lib.core/defines.h>

typedef struct s_Index Index;

struct s_Index
{
        size_t bits;
        size_t power;
        size_t *power_bits;
        size_t *power_offset;
        unsigned char *map;
};

Index *index_create (size_t bits);
void index_destroy (Index *index);
void index_set (Index *index, size_t position, bool to_value);
bool index_get (Index *index, size_t position);

#endif
