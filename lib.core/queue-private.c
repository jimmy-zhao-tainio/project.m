#include <lib.core/queue-private.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <string.h>

bool queue_grow (Queue *queue)
{
        size_t new_capacity;
        void *grown;

        if (queue->item_capacity == 1) {
                new_capacity = 2;
        }
        else {
                new_capacity = (size_t)((double)queue->item_capacity * 1.5);
                if (new_capacity <= queue->item_capacity) {
                        error (InvalidOperation);
                        return false;
                }
        }
        if (!(grown = memory_grow (queue->items, new_capacity * queue->item_size))) {
                error (FunctionCall);
                return false;
        }
        queue->items = grown;
        queue->item_capacity = new_capacity;
        return true;
}
