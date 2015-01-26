#include <lib.core/threads-signal.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

bool thread_signal_create (ThreadSignal *signal)
{
        if (!signal) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock_create (&signal->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (pthread_cond_init (&signal->cond, NULL) != 0) {
                if (!thread_lock_destroy (&signal->lock)) {
                        error_code (FunctionCall, 2);
                }
                error (SystemCall);
                return false;
        }
        return true;
}

bool thread_signal_destroy (ThreadSignal *signal)
{
        if (!signal) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock_destroy (&signal->lock)) {
                error (FunctionCall);
        }
        if (pthread_cond_destroy (&signal->cond) != 0) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool thread_signal (ThreadSignal *signal)
{
        if (!signal) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock (&signal->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (pthread_cond_signal (&signal->cond) != 0) {
                error (SystemCall);
                return false;
        }
        if (!thread_unlock (&signal->lock)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}

bool thread_signal_wait (ThreadSignal *signal)
{
        if (!signal) {
                error (InvalidArgument);
                return false;
        }
        if (!thread_lock (&signal->lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (pthread_cond_wait (&signal->cond, &signal->lock.mutex) != 0) {
                error (SystemCall);
                return false;
        }
        if (!thread_unlock (&signal->lock)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}
