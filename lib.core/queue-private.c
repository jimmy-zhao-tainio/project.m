#include <lib.core/queue-private.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <string.h>

bool queue_grow (Queue *queue)
{
        size_t new_capacity;
        void *grown;
        size_t move_from;
        size_t move_to;
        size_t move_bytes;
        size_t zero_from;
        size_t zero_bytes;

        if (queue->item_count != queue->item_capacity) {
                error_code (InvalidOperation, 1);
                return false;
        }
        if (queue->item_capacity == 1) {
                new_capacity = 2;
        }
        else {
                new_capacity = (size_t)((double)queue->item_capacity * 1.5);
                if (new_capacity <= queue->item_capacity) {
                        error_code (InvalidOperation, 2);
                        return false;
                }
        }
        if (!(grown = memory_grow (queue->items, new_capacity * queue->item_size))) {
                error (FunctionCall);
                return false;
        }
        if (queue->pop_position != 0) {
                move_bytes = (queue->item_capacity - queue->pop_position) * queue->item_size;
                move_from = queue->pop_position * queue->item_size;
                move_to = (new_capacity * queue->item_size) - move_bytes;
                memmove ((char *)grown + move_to,
                         (char *)grown + move_from,
                         move_bytes);
                zero_bytes = move_to - move_from;
                memset ((char *)grown + move_from, '\0', zero_bytes);
                // Adjust pop_position
                queue->pop_position = new_capacity - (queue->item_capacity - queue->pop_position);
        }
        else {
                zero_from = queue->item_capacity * queue->item_size;
                zero_bytes = (new_capacity * queue->item_size) - zero_from;
                memset ((char *)grown + zero_from, '\0', zero_bytes);
        }
        queue->items = grown;
        queue->item_capacity = new_capacity;
        return true;
}
