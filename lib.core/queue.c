#include <lib.core/queue.h>
#include <lib.core/queue-private.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <string.h>

// Queue
//
// |---###########--|
//     ^         ^
//     |         count
//     pop_position
//
// Push at pop_position + count
// Pop at pop_position
//
// It wraps
//
// |###---------###|
//    ^         ^
//    +count    pop_position

static bool is_wrapped (Queue *queue);
static size_t push_position (Queue *queue);
static void set_pop_position (Queue *queue);

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
        queue->pop_position = 0;
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
        size_t copy_to;
        
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
        copy_to = push_position (queue) * queue->item_size;
        (void)memmove ((char *)queue->items + copy_to, item, queue->item_size);
        queue->item_count++;
        return true;
}

bool queue_pop (Queue *queue, void *item)
{
        size_t copy_from;

        if (queue->item_count == 0) {
                error (InvalidOperation);
                return false;
        }
        copy_from = queue->pop_position * queue->item_size;
        (void)memmove (item, (char *)queue->items + copy_from, queue->item_size);
        memset ((char *)queue->items + copy_from, '\0', queue->item_size);
        set_pop_position (queue);
        queue->item_count--;
        return true;
}

static size_t push_position (Queue *queue)
{
        if (is_wrapped (queue)) {
                return queue->pop_position + queue->item_count - queue->item_capacity;
        }
        return queue->pop_position + queue->item_count;
}

static bool is_wrapped (Queue *queue)
{
        if (queue->pop_position + queue->item_count >= queue->item_capacity) {
                return true;
        }
        return false;
}

static void set_pop_position (Queue *queue)
{
        if (queue->pop_position == queue->item_capacity - 1) {
                queue->pop_position = 0;
        }
        else {
                queue->pop_position++;
        }
}
