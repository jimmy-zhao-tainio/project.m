#ifndef queue_structures_h
#define queue_structures_h

#include <lib.core/defines.h>

typedef struct _Queue Queue;

typedef enum {
        QUEUE_SIZE_FIXED,
        QUEUE_SIZE_DYNAMIC
} QueueSize;

struct _Queue
{
        QueueSize queue_size;
        size_t item_capacity;
        size_t item_count;
        size_t item_size;
        size_t pop_position;
        size_t push_position;
        void *items;
};

#endif