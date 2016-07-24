#ifndef net_stream_connection_queue_h
#define net_stream_connection_queue_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/connection.h>

typedef struct _NetStreamConnectionQueue NetStreamConnectionQueue;

struct _NetStreamConnectionQueue
{
        Thread *worker;
        ThreadLock lock;
        pthread_cond_t cond;
        size_t length;
        size_t front;
        size_t count;
        void *stream; /* Circular dependency */
        NetStreamConnection **connections;
        bool exit;
};

NetStreamConnectionQueue *net_stream_connection_queue_create (void *stream);
void net_stream_connection_queue_destroy (NetStreamConnectionQueue *connection_queue);
bool net_stream_connection_queue_insert (NetStreamConnectionQueue *connection_queue, 
                                         NetStreamConnection *connection, 
                                         NetStreamTask task);

#endif
