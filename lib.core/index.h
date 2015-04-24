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
        size_t change_power;
        size_t change_position;
        unsigned char *map;
};

Index *index_create (size_t bits);
void index_destroy (Index *index);
void index_set (Index *index, size_t position);
void index_set_range (Index *index, size_t from, size_t to);
bool index_get (Index *index, size_t position);
void index_clear (Index *index);

#endif
