#ifndef threads_lock_h
#define threads_lock_h

#include <pthread.h>
#include <lib.core/defines.h>

typedef struct _ThreadLock ThreadLock;

struct _ThreadLock
{
        pthread_mutex_t mutex;
};

bool thread_lock_create  (ThreadLock *lock);
bool thread_lock_destroy (ThreadLock *lock);
bool thread_lock         (ThreadLock *lock);
bool thread_unlock       (ThreadLock *lock);

#define THREAD_LOCK_INITIALIZER { .mutex = PTHREAD_MUTEX_INITIALIZER }

#endif
