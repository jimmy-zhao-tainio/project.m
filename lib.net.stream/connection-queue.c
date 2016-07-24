#include <lib.net.stream/connection-queue.h>
#include <lib.net.stream/stream.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>

static void worker (Thread *thread);

static void queue_push (NetStreamConnectionQueue *connection_queue, 
                        NetStreamConnection *connection);
static NetStreamConnection *queue_pop (NetStreamConnectionQueue *connection_queue);

static void task_write       (NetStream *stream, 
                              NetStreamConnection *connection,
                              NetStreamTaskWrite write);
static void task_write_flags (NetStream *stream, 
                              NetStreamConnection *connection,
                              NetStreamTaskWriteFlags write_flags);
static void task_close       (NetStream *stream, 
                              NetStreamConnection *connection,
                              NetStreamTaskClose close);

NetStreamConnectionQueue *net_stream_connection_queue_create (void *stream)
{
        NetStreamConnectionQueue *connection_queue;

        if (!(connection_queue = memory_create (sizeof (NetStreamConnectionQueue)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        connection_queue->length = 1024;
        connection_queue->stream = stream;
        if (!(connection_queue->connections = memory_create (connection_queue->length * 
                                                       sizeof (NetStreamConnection *)))) {
                memory_destroy (connection_queue);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!thread_lock_create (&connection_queue->lock)) {
                memory_destroy (connection_queue);
                memory_destroy (connection_queue->connections);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (pthread_cond_init (&connection_queue->cond, NULL) != 0) {
                memory_destroy (connection_queue);
                memory_destroy (connection_queue->connections);
                thread_lock_destroy (&connection_queue->lock);
                error_code (FunctionCall, 4);
                return NULL;
        }
        connection_queue->front = 0;
        connection_queue->count = 0;
        connection_queue->exit = false;
        if (!(connection_queue->worker = thread_create (&worker, connection_queue))) {
                memory_destroy (connection_queue);
                memory_destroy (connection_queue->connections);
                thread_lock_destroy (&connection_queue->lock);
                (void)pthread_cond_destroy (&connection_queue->cond);
                error_code (FunctionCall, 5);
                return NULL;
        }
        return connection_queue;
}

void net_stream_connection_queue_destroy (NetStreamConnectionQueue *connection_queue)
{
        // Lock
        connection_queue->exit = true;
        // Signal
        // Unlock

        if (!thread_wait (connection_queue->worker)) {
                error_code (FunctionCall, 1);
                return;
        }
        thread_destroy (connection_queue->worker);
        (void)thread_lock_destroy (&connection_queue->lock);
        (void)pthread_cond_destroy (&connection_queue->cond);
        memory_destroy (connection_queue->connections);
}

bool net_stream_connection_queue_push (NetStreamConnectionQueue *connection_queue, 
                                       NetStreamConnection *connection,
                                       NetStreamTask task)
{
        (void)task;
        if (!thread_lock (&connection_queue->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }

        // Check what/if to push.
        // ...

        queue_push (connection_queue, connection);

        if (pthread_cond_signal (&connection_queue->cond) != 0) {
                error (SystemCall);
                return connection;
        }
        if (!thread_unlock (&connection_queue->lock)) {
                error_code (FunctionCall, 3);
                return false;
        }
        return true;
}

static void worker (Thread *thread)
{
        NetStreamConnectionQueue *connection_queue = thread->argument;
        NetStreamConnection *connection;
        NetStreamTask task;

        while (true) {
                if (!thread_lock (&connection_queue->lock)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                while (connection_queue->count == 0) {
                        pthread_cond_wait (&connection_queue->cond, 
                                           &connection_queue->lock.mutex);
                        if (connection_queue->exit == true) {
                                if (!thread_unlock (&connection_queue->lock)) {
                                        error_code (FunctionCall, 2);
                                        return;
                                }
                                return;
                        }
                }
                connection = queue_pop (connection_queue);

                // Handle task.
                task = net_stream_task_queue_pop (connection->queue);
                if (task.type == NET_STREAM_TASK_WRITE) {
                        task_write ((NetStream *)connection_queue->stream,
                                    connection,
                                    task.value.write);
                }
                else if (task.type == NET_STREAM_TASK_WRITE_FLAGS) {
                        task_write_flags ((NetStream *)connection_queue->stream,
                                          connection,
                                          task.value.write_flags);
                }
                else if (task.type == NET_STREAM_TASK_CLOSE) {
                        task_close ((NetStream *)connection_queue->stream,
                                    connection,
                                    task.value.close);
                }
                if (!thread_unlock (&connection_queue->lock)) {
                        error_code (FunctionCall, 3);
                        return;
                }
        }
}

static void queue_push (NetStreamConnectionQueue *connection_queue, 
                        NetStreamConnection *connection)
{
        size_t index;

        if (connection_queue->front + connection_queue->count >= connection_queue->length) {
                index = connection_queue->front + connection_queue->count - 
                        connection_queue->length;
        }
        else {
                index = connection_queue->front + connection_queue->count;
        }
        connection_queue->connections[index] = connection;
        connection_queue->count++;
}

static NetStreamConnection *queue_pop (NetStreamConnectionQueue *connection_queue)
{
        NetStreamConnection *connection;

        connection = connection_queue->connections[connection_queue->front];
        if (connection_queue->front == connection_queue->length - 1) {
                connection_queue->front = 0;
        }
        else {
                connection_queue->front++;
        }
        connection_queue->count--;
        return connection;
}

static void task_write (NetStream *stream, 
                        NetStreamConnection *connection,
                        NetStreamTaskWrite write)
{
        if (!net_poll_write (stream->poll, 
                             &connection->poll, 
                             write.buffer, 
                             write.length)) {
                return;
        }
        thread_signal_wait (&connection->write_signal);
}

static void task_write_flags (NetStream *stream, 
                              NetStreamConnection *connection,
                              NetStreamTaskWriteFlags write_flags)
{
        if (!net_poll_write_flags (stream->poll, 
                                   &connection->poll, 
                                   write_flags.buffer,
                                   write_flags.length,
                                   write_flags.flags)) {
                return;
        }
        thread_signal_wait (&connection->write_signal);
}

static void task_close (NetStream *stream, 
                        NetStreamConnection *connection,
                        NetStreamTaskClose close)
{
        (void)close;
        if (!net_poll_close (stream->poll, &connection->poll)) {
                return;
        }
}
