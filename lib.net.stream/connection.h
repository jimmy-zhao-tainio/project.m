#ifndef net_stream_connection_h
#define net_stream_connection_h

#include <lib.net.poll/poll.h>
#include <lib.core/threads-lock.h>
#include <lib.net.stream/task-queue.h>

typedef struct _NetStreamConnection NetStreamConnection;

struct _NetStreamConnection
{
        NetPollConnection poll;
        NetStreamTaskQueue *queue;
        ThreadLock lock;
        ThreadSignal write_signal;
        ThreadSignal close_signal;
        bool closed;
        bool write_success;
        void *tag;
};

NetStreamConnection *net_stream_connection_create (int socket);
void net_stream_connection_destroy (NetStreamConnection *connection);

#endif
