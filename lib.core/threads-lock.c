#include <lib.core/threads-lock.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

bool thread_lock_create (ThreadLock *lock)
{
        pthread_mutexattr_t attr;
        
        if (pthread_mutexattr_init(&attr) != 0) {
                error_code (SystemCall, 1);
                return false;
        }
        if (pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE_NP) != 0) {
                error_code (SystemCall, 2);
                return false;
        }
        if (pthread_mutex_init (&lock->mutex, &attr) != 0) {
                error_code (SystemCall, 3);
                return false;
        }
        return true;
}

bool thread_lock_destroy (ThreadLock *lock)
{
        if (!lock) {
                error (InvalidArgument);
                return false;
        }
        if (pthread_mutex_destroy (&lock->mutex) != 0) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool thread_lock (ThreadLock *lock)
{
        if (!lock) {
                error (InvalidArgument);
                return false;
        }
        if (pthread_mutex_lock (&lock->mutex) != 0) {
                error (SystemCall);
                return false;
        }
        return true;
}

bool thread_unlock (ThreadLock *lock)
{
        if (!lock) {
                error (InvalidArgument);
                return false;
        }
        if (pthread_mutex_unlock (&lock->mutex) != 0) {
                error (SystemCall);
                return false;
        }
        return true;
}
