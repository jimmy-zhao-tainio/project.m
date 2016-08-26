#ifndef queue_h
#define queue_h

#include <lib.core/defines.h>
#include <lib.core/queue-structures.h>

bool queue_create (Queue *queue, size_t items, size_t item_size, QueueSize queue_size);
void queue_destroy (Queue *queue);
bool queue_push (Queue *queue, void *item);
bool queue_pop (Queue *queue, void *item);

#endif
