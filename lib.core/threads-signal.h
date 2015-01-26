#ifndef threads_signal_h
#define threads_signal_h

#include <pthread.h>
#include <lib.core/threads-lock.h>
#include <lib.core/defines.h>

typedef struct _ThreadSignal ThreadSignal;

struct _ThreadSignal
{
        pthread_cond_t cond;
        ThreadLock lock;
};

bool thread_signal_create  (ThreadSignal *signal);
bool thread_signal_destroy (ThreadSignal *signal);
bool thread_signal         (ThreadSignal *signal);
bool thread_signal_wait    (ThreadSignal *signal);

#define THREAD_SIGNAL_INITIALIZER \
        { \
                        .cond = PTHREAD_COND_INITIALIZER, \
                        .lock = THREAD_LOCK_INITIALIZER \
        }

#endif
