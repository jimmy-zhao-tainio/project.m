#ifndef net_poll_queue_h
#define net_poll_queue_h

#include <lib.core/defines.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>
#include <lib.net.poll/event.h>
#include <lib.net.poll/connection.h>

typedef struct _NetPollQueue NetPollQueue;

struct _NetPollQueue
{
        ThreadLock lock;
        pthread_cond_t cond;
        size_t length;
        size_t front;
        size_t count;
        NetPollEvent *events;
};

bool net_poll_queue_create (NetPollQueue *queue,
                            size_t length);
void net_poll_queue_destroy (NetPollQueue *queue);
bool net_poll_queue_push_monitor (NetPollQueue *queue, NetPollConnection *connection);
bool net_poll_queue_push_close (NetPollQueue *queue, NetPollConnection *connection);
bool net_poll_queue_push_write (NetPollQueue *queue, NetPollConnection *connection);
bool net_poll_queue_push (NetPollQueue *queue, NetPollEvent event);
NetPollEvent net_poll_queue_pop (NetPollQueue *queue);

#endif
