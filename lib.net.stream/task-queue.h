#ifndef net_stream_task_queue_h
#define net_stream_task_queue_h

#include <lib.core/defines.h>
#include <lib.net.poll/poll.h>

typedef struct _NetStreamTaskQueue      NetStreamTaskQueue;
typedef struct _NetStreamTask           NetStreamTask;
typedef struct _NetStreamTaskWrite      NetStreamTaskWrite;
typedef struct _NetStreamTaskWriteFlags NetStreamTaskWriteFlags;
typedef struct _NetStreamTaskClose      NetStreamTaskClose;

typedef enum {
        NET_STREAM_TASK_WRITE,
        NET_STREAM_TASK_WRITE_FLAGS,
        NET_STREAM_TASK_CLOSE
} NetStreamTaskType;

struct _NetStreamTaskQueue
{
        size_t length;
        size_t front;
        size_t count;
        NetStreamTask *tasks;
};

struct _NetStreamTaskWrite
{
        unsigned char *buffer;
        size_t length;
};

struct _NetStreamTaskWriteFlags
{
        unsigned char *buffer;
        size_t length;
        NetPollFlag flags;
};

struct _NetStreamTaskClose
{
        bool not_used;
};

struct _NetStreamTask
{
        NetStreamTaskType type;
        union {
                NetStreamTaskWrite write;
                NetStreamTaskWriteFlags write_flags;
                NetStreamTaskClose close;
        } value;
};

NetStreamTaskQueue *net_stream_task_queue_create (void);
void net_stream_task_queue_destroy (NetStreamTaskQueue *task_queue);
bool net_stream_task_queue_push (NetStreamTaskQueue *task_queue,
                                 NetStreamTask task);
NetStreamTask net_stream_task_queue_pop (NetStreamTaskQueue *task_queue);

#endif
