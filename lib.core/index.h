#ifndef index_h
#define index_h

#include <lib.core/defines.h>

typedef struct s_Index Index;

struct s_Index
{
        size_t bits;
        unsigned char **array;
        size_t          array_count;
};

Index *index_create (size_t bits);

#endif
