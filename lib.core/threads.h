#ifndef threads_h
#define threads_h

#include <pthread.h>
#include <lib.core/defines.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>

typedef struct _Thread Thread;

struct _Thread
{
        pthread_t handle;
        void *argument;
        bool exit;
        ThreadSignal signal;
        ThreadLock lock; 
};

Thread *thread_create    (void (*function) (Thread *), void *argument);
void    thread_destroy   (Thread *thread);
bool    thread_wait      (Thread *thread);
bool    thread_set_exit  (Thread *thread);
bool    thread_get_exit  (Thread *thread);

#endif
