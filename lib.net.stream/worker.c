#include <lib.net.stream/worker.h>
#include <lib.net.stream/connection.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <unistd.h>

static void worker_thread (Thread *thread);

static void handle_task (NetPoll *poll,
                         NetStreamConnection *connection,
                         NetStreamTask task);
static void task_add         (NetPoll *poll, 
                              NetStreamConnection *connection,
                              NetStreamTaskAdd add);
static void task_write       (NetPoll *poll, 
                              NetStreamConnection *connection,
                              NetStreamTaskWrite write);
static void task_write_flags (NetPoll *poll, 
                              NetStreamConnection *connection,
                              NetStreamTaskWriteFlags write_flags);
static void task_close       (NetPoll *poll, 
                              NetStreamConnection *connection,
                              NetStreamTaskClose close);

static bool signal_exit (NetStreamWorker *stream);

NetStreamWorker *net_stream_worker_create (NetPoll *poll)
{
        NetStreamWorker *worker;

        if (!(worker = memory_create (sizeof (NetStreamWorker)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        worker->poll = poll;
        if (!queue_create (&worker->connections,
                           1024, 
                           sizeof (NetStreamConnection *), 
                           QUEUE_SIZE_DYNAMIC)) {
                memory_destroy (worker);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!thread_lock_create (&worker->lock)) {
                queue_destroy (&worker->connections);
                memory_destroy (worker);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (pthread_cond_init (&worker->cond, NULL) != 0) {
                thread_lock_destroy (&worker->lock);
                queue_destroy (&worker->connections);
                memory_destroy (worker);
                error_code (FunctionCall, 4);
                return NULL;
        }
        worker->exit = false;
        if (!(worker->thread = thread_create (&worker_thread, worker))) {
                (void)pthread_cond_destroy (&worker->cond);
                thread_lock_destroy (&worker->lock);
                queue_destroy (&worker->connections);
                memory_destroy (worker);
                error_code (FunctionCall, 5);
                return NULL;
        }
        return worker;
}

void net_stream_worker_destroy (NetStreamWorker *worker)
{
        if (!signal_exit (worker)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (!thread_wait (worker->thread)) {
                error_code (FunctionCall, 2);
                return;
        }
        thread_destroy (worker->thread);
        (void)pthread_cond_destroy (&worker->cond);
        thread_lock_destroy (&worker->lock);
        queue_destroy (&worker->connections);
        memory_destroy (worker);
}

bool net_stream_worker_add (NetStreamWorker *worker, 
                            NetStreamConnection *connection,
                            NetStreamTask task)
{
        bool push_to_worker = false;

        if (!thread_lock (&worker->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (queue_is_empty (&connection->tasks)) {
                // This connection is not currently being processed by the worker.
                // We need add it to the queue of active connections later on.
                push_to_worker = true;
        }
        // Push task into connection->tasks
        if (!queue_push (&connection->tasks, &task)) {
                if (!thread_unlock (&worker->lock)) {
                        error_code (FunctionCall, 2);
                }
                error_code (FunctionCall, 3);
                return false;
        }
        if (push_to_worker) {
                // Push connection into worker->connections
                if (!queue_push (&worker->connections, &connection)) {
                        if (!thread_unlock (&worker->lock)) {
                                error_code (FunctionCall, 4);
                        }
                        // connection->tasks was empty.
                        // Let's pop what we pushed to clean things up.
                        (void)queue_pop (&connection->tasks, &task);
                        error_code (FunctionCall, 5);
                        return false;
                }
        }
        if (pthread_cond_signal (&worker->cond) != 0) {
                error (SystemCall);
                return false;
        }
        if (!thread_unlock (&worker->lock)) {
                error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

static void worker_thread (Thread *thread)
{
        NetStreamWorker *worker= thread->argument;
        NetStreamConnection *connection;
        NetStreamTask task;

        while (true) {
                if (!thread_lock (&worker->lock)) {
                        error_code (FunctionCall, 1);
                        return;
                }
                if (worker->exit == true) {
                        if (!thread_unlock (&worker->lock)) {
                                error_code (FunctionCall, 2);
                                return;
                        }
                        return;
                }
                while (queue_is_empty (&worker->connections)) {
                        pthread_cond_wait (&worker->cond, 
                                           &worker->lock.mutex);
                        if (worker->exit == true) {
                                if (!thread_unlock (&worker->lock)) {
                                        error_code (FunctionCall, 2);
                                        return;
                                }
                                return;
                        }
                }
                if (!queue_pop (&worker->connections, &connection)) {
                        if (!thread_unlock (&worker->lock)) {
                                error_code (FunctionCall, 3);
                                return;
                        }
                        continue;
                }
                if (!queue_pop (&connection->tasks, &task)) {
                        if (!thread_unlock (&worker->lock)) {
                                error_code (FunctionCall, 4);
                                return;
                        }
                        continue;
                }
                // Handle task.
                handle_task (worker->poll, connection, task);
                // Check if the connection have more tasks to be processed.
                if (!queue_is_empty (&connection->tasks)) {
                        if (!queue_push (&worker->connections, &connection)) {
                                error_code (FunctionCall, 5);
                                return;
                        }
                }
                if (!thread_unlock (&worker->lock)) {
                        error_code (FunctionCall, 6);
                        return;
                }
        }
}

static void handle_task (NetPoll *poll,
                         NetStreamConnection *connection,
                         NetStreamTask task)
{
        if (task.type == NET_STREAM_TASK_WRITE) {
                task_write (poll, connection, task.value.write);
        }
        else if (task.type == NET_STREAM_TASK_WRITE_FLAGS) {
                task_write_flags (poll, connection, task.value.write_flags);
        }
        else if (task.type == NET_STREAM_TASK_CLOSE) {
                task_close (poll, connection, task.value.close);
        }
        else if (task.type == NET_STREAM_TASK_ADD) {
                task_add (poll, connection, task.value.add);
        }
}

static void task_add (NetPoll *poll, 
                      NetStreamConnection *connection,
                      NetStreamTaskAdd add)
{
        (void)add;
        if (!net_poll_monitor (poll, &connection->poll)) {
                // If adding fails, we fail silently.
                // No "add_failed" event is implemented at this time.
                close (connection->poll.socket);
                net_stream_connection_destroy (connection);
                error (FunctionCall);
        }
}

static void task_write (NetPoll *poll, 
                        NetStreamConnection *connection,
                        NetStreamTaskWrite write)
{
        if (!net_poll_write (poll, 
                             &connection->poll, 
                             write.buffer, 
                             write.length)) {
                return;
        }
        thread_signal_wait (&connection->write_signal);
}

static void task_write_flags (NetPoll *poll, 
                              NetStreamConnection *connection,
                              NetStreamTaskWriteFlags write_flags)
{
        if (!net_poll_write_flags (poll, 
                                   &connection->poll, 
                                   write_flags.buffer,
                                   write_flags.length,
                                   write_flags.flags)) {
                return;
        }
        thread_signal_wait (&connection->write_signal);
}

static void task_close (NetPoll *poll, 
                        NetStreamConnection *connection,
                        NetStreamTaskClose close)
{
        (void)close;
        if (!net_poll_close (poll, &connection->poll)) {
                return;
        }
}

static bool signal_exit (NetStreamWorker *worker)
{
        if (!thread_lock (&worker->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        worker->exit = true;
        if (pthread_cond_signal (&worker->cond) != 0) {
                error (SystemCall);
                return false;
        }
        if (!thread_unlock (&worker->lock)) {
                error_code (FunctionCall, 3);
                return false;
        }
        return true;  
}
