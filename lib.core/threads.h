#ifndef threads_h
#define threads_h

#include <pthread.h>
#include <lib.core/defines.h>
#include <lib.core/threads-lock.h>

typedef struct _Thread Thread;
typedef struct _ThreadSignal ThreadSignal;

struct _ThreadSignal
{
        ThreadLock lock;
        pthread_cond_t cond;
};

struct _Thread
{
        pthread_t handle;
        void *argument;
        bool exit;
        ThreadSignal signal;
        ThreadLock lock; 
};

Thread *thread_create  (void (*function) (Thread *), void *argument);
void    thread_destroy (Thread *thread);
void    thread_exit    (Thread *thread);
bool    thread_wait    (Thread *thread);
bool    thread_set_exit  (Thread *thread);
bool    thread_get_exit  (Thread *thread);

bool thread_signal_create  (ThreadSignal *signal);
bool thread_signal_destroy (ThreadSignal *signal);
bool thread_signal         (ThreadSignal *signal);
bool thread_signal_wait    (ThreadSignal *signal);

#endif
