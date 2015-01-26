#include <lib.core/threads.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

Thread *thread_create (void (*function) (Thread *), void *argument)
{
        Thread *thread;

        if (!function) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(thread = memory_create (sizeof (Thread)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        thread->argument = argument;
        thread->exit = false;
        if (!thread_signal_create (&thread->signal)) {
                memory_destroy (thread);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!thread_lock_create (&thread->lock)) {
                (void)thread_signal_destroy (&thread->signal);
                memory_destroy (thread);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (pthread_create (&thread->handle, NULL, (void *(*)(void *))function, thread)) {
                (void)thread_signal_destroy (&thread->signal);
                (void)thread_lock_destroy (&thread->lock);
                memory_destroy (thread);
                error_code (SystemCall, 3);
                return NULL;
        }
        return thread;
}

void thread_destroy (Thread *thread)
{
        if (!thread) {
                error (InvalidArgument);
                return;
        }
        (void)thread_signal_destroy (&thread->signal);
        thread_lock_destroy (&thread->lock);
        memory_destroy (thread);
}

void thread_exit (Thread *thread)
{
        (void)thread;
        pthread_exit (NULL);
}

bool thread_wait (Thread *thread)
{
        if (!thread) {
                error (InvalidArgument);
                return false;
        }
        if (pthread_join (thread->handle, NULL) != 0) {
                error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

bool thread_set_exit (Thread *thread)
{
        if (!thread) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock (&thread->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        thread->exit = true;
        if (!thread_signal (&thread->signal)) {
                error_code (FunctionCall, 2);
                return false;
        }
        if (!thread_unlock (&thread->lock)) {
                error_code (FunctionCall, 3);
                return false;
        }
        return true;
}

bool thread_get_exit (Thread *thread)
{
        bool exit;

        if (!thread) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock (&thread->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        exit = thread->exit;
        if (!thread_unlock (&thread->lock)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return exit;
}
