#include <lib.net.poll/queue.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>

static void         inner_push (NetPollQueue *queue, NetPollEvent event);
static NetPollEvent inner_pop  (NetPollQueue *queue);

bool net_poll_queue_create (NetPollQueue *queue,
                            size_t length)
{
        if (!(queue->events = memory_create (length * sizeof (NetPollEvent)))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!thread_lock_create (&queue->lock)) {
                memory_destroy (queue->events);
                error_code (FunctionCall, 2);
                return false;
        }
        if (pthread_cond_init (&queue->cond, NULL) != 0) {
                memory_destroy (queue->events);
                thread_lock_destroy (&queue->lock);
                error_code (FunctionCall, 3);
                return false;
        }
        queue->length = length;
        queue->front = 0;
        queue->count = 0;
        return true;
}

void net_poll_queue_destroy (NetPollQueue *queue)
{
        (void)thread_lock_destroy (&queue->lock);
        (void)pthread_cond_destroy (&queue->cond);
        memory_destroy (queue->events);
}

bool net_poll_queue_push_monitor (NetPollQueue *queue, NetPollConnection *connection)
{
        NetPollEvent event = {
                .connection = connection,
                .type = NET_POLL_INTERNAL_MONITOR
        };
        return net_poll_queue_push (queue, event);
}

bool net_poll_queue_push_close (NetPollQueue *queue, NetPollConnection *connection)
{
        NetPollEvent event = {
                .connection = connection,
                .type = NET_POLL_INTERNAL_CLOSE
        };
        return net_poll_queue_push (queue, event);
}

bool net_poll_queue_push_write (NetPollQueue *queue, NetPollConnection *connection)
{
        NetPollEvent event = {
                .connection = connection,
                .type = NET_POLL_INTERNAL_WRITE
        };
        return net_poll_queue_push (queue, event);
}

bool net_poll_queue_push (NetPollQueue *queue, NetPollEvent event)
{
        if (!thread_lock (&queue->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        while (queue->count == queue->length) {
                pthread_cond_wait (&queue->cond, &queue->lock.mutex);
        }
        if (queue->count >= queue->length) {
                error (InvalidOperation);
                return false;
        }
        inner_push (queue, event);
        if (!thread_unlock (&queue->lock)) {
                error_code (FunctionCall, 3);
                return false;
        }
        return true;
}

NetPollEvent net_poll_queue_pop (NetPollQueue *queue)
{
        NetPollEvent event = { .type = NET_POLL_EMPTY };

        if (!thread_lock (&queue->lock)) {
                error_code (FunctionCall, 1);
                return event;
        }
        if (queue->count == 0) {
                if (!thread_unlock (&queue->lock)) {
                        error_code (FunctionCall, 2);
                }
                return event;
        }
        event = inner_pop (queue);
        if (queue->count == queue->length - 1) {
                if (pthread_cond_signal (&queue->cond) != 0) {
                        error (SystemCall);
                        return event;
                }
        }
        if (!thread_unlock (&queue->lock)) {
                error_code (FunctionCall, 4);
        }
        return event;
}

static void inner_push (NetPollQueue *queue, NetPollEvent event)
{
        size_t index;

        if (queue->front + queue->count >= queue->length) {
                index = queue->front + queue->count - queue->length;
        }
        else {
                index = queue->front + queue->count;
        }
        queue->events[index] = event;
        queue->count++;
}

static NetPollEvent inner_pop (NetPollQueue *queue)
{
        NetPollEvent event;
        
        event = queue->events[queue->front];
        if (queue->front == queue->length - 1) {
                queue->front = 0;
        }
        else {
                queue->front++;
        }
        queue->count--;
        return event;
}
