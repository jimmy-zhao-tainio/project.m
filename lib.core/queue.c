#include <lib.core/queue.h>
#include <lib.core/queue-private.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <string.h>

// Queue
//
// |---###########--|
//     ^         ^
//     position  +count
//
// Push at position + count
// Pop at position
//
// It wraps
//
// |###---------###|
//    ^         ^
//    +count    position

bool queue_create (Queue *queue, size_t items, size_t item_size, QueueSize queue_size)
{
        size_t size;

        if (!size_t_mul (items, item_size, &size)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (size == 0) {
                error (InvalidOperation);
                return false;
        }
        if (!(queue->items = memory_create (size))) {
                error_code (FunctionCall, 2);
                return false;
        }
        queue->item_capacity = items;
        queue->item_count = 0;
        queue->item_size = item_size;
        queue->position = 0;
        queue->queue_size = queue_size;
        return true;
}

void queue_destroy (Queue *queue)
{
        if (!queue->items) {
                error (InvalidOperation);
                return;
        }
        memory_destroy (queue->items);
        queue->items = NULL;
}

bool queue_push (Queue *queue, void *item)
{
        size_t index;
        
        if (queue->item_count == queue->item_capacity) {
                if (queue->queue_size == QUEUE_SIZE_FIXED) {
                        error_code (InvalidOperation, 1);
                        return false;
                }
                if (!queue_grow (queue)) {
                        error_code (InvalidOperation, 2);
                        return false;
                }
        }
        if (queue->position + queue->item_count >= queue->item_capacity) {
                // Wrapped
                index = queue->position + queue->item_count - queue->item_capacity;
        }
        else {
                index = queue->position + queue->item_count;
        }
        (void)memmove ((char *)queue->items + index, item, queue->item_size);
        queue->item_count++;
        return true;
}

bool queue_pop (Queue *queue, void *item)
{
        if (queue->item_count == 0) {
                error (InvalidOperation);
                return false;
        }
        (void)memmove (item, (char *)queue->items + queue->position, queue->item_size);
        if (queue->position == queue->item_capacity - 1) {
                queue->position = 0;
        }
        else {
                queue->position++;
        }
        queue->item_count--;
        return true;
}
