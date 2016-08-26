#include <lib.net.stream/task-queue.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>

NetStreamTaskQueue *net_stream_task_queue_create (void)
{
        NetStreamTaskQueue *task_queue;

        if (!(task_queue = memory_create (sizeof (NetStreamTaskQueue)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        task_queue->length = 8;
        if (!(task_queue->tasks = memory_create (task_queue->length * 
                                                 sizeof (NetStreamTask)))) {
                memory_destroy (task_queue);
                error_code (FunctionCall, 2);
                return NULL;
        }
        task_queue->front = 0;
        task_queue->count = 0;
        return task_queue;
}

void net_stream_task_queue_destroy (NetStreamTaskQueue *task_queue)
{
        memory_destroy (task_queue->tasks);
        memory_destroy (task_queue);
}

bool net_stream_task_queue_push (NetStreamTaskQueue *task_queue,
                                 NetStreamTask task)
{
        void *grown;
        size_t index;

        if (task_queue->count == task_queue->length) {
                // Resize task_queue
                if (!(grown = memory_grow (task_queue->tasks,
                                           (task_queue->length * 2) * 
                                           sizeof (NetStreamTask)))) {
                        error (FunctionCall);
                        return false;
                }
                // This doesn't work without a memmove!
                // Isolate code to core.
                task_queue->tasks = grown;
                task_queue->length = task_queue->length * 2;
        }
        if (task_queue->front + task_queue->count >= task_queue->length) {
                index = task_queue->front + task_queue->count - task_queue->length;
        }
        else {
                index = task_queue->front + task_queue->count;
        }
        task_queue->tasks[index] = task;
        task_queue->count++;
        return true;
}

NetStreamTask net_stream_task_queue_pop (NetStreamTaskQueue *task_queue)
{
        NetStreamTask task;

        task = task_queue->tasks[task_queue->front];
        if (task_queue->front == task_queue->length - 1) {
                task_queue->front = 0;
        }
        else {
                task_queue->front++;
        }
        task_queue->count--;
        return task;
}
