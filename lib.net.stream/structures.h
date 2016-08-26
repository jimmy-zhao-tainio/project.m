#ifndef lib_net_stream_structures_h
#define lib_net_stream_structures_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>
#include <lib.net.poll/poll.h>

typedef struct _NetStream                NetStream;
typedef struct _NetStreamConnection      NetStreamConnection;
typedef struct _NetStreamTaskQueue       NetStreamTaskQueue;
typedef struct _NetStreamTask            NetStreamTask;
typedef struct _NetStreamTaskWrite       NetStreamTaskWrite;
typedef struct _NetStreamTaskWriteFlags  NetStreamTaskWriteFlags;
typedef struct _NetStreamTaskClose       NetStreamTaskClose;
typedef struct _NetStreamConnectionQueue NetStreamConnectionQueue;

typedef void (*NetStreamOnAdd)   (NetStream *stream, 
                                  NetStreamConnection *connection);
typedef void (*NetStreamOnClose) (NetStream *stream, 
                                  NetStreamConnection *connection);
typedef void (*NetStreamOnRead)  (NetStream *stream,
                                  NetStreamConnection *connection, 
                                  unsigned char *buffer,
                                  size_t length);

typedef enum {
        NET_STREAM_TASK_WRITE,
        NET_STREAM_TASK_WRITE_FLAGS,
        NET_STREAM_TASK_CLOSE
} NetStreamTaskType;

struct _NetStream
{
        NetPoll *poll;
        NetStreamOnAdd on_add;
        NetStreamOnClose on_close;
        NetStreamOnRead on_read;
        NetStreamConnectionQueue *connection_queue;
        void *tag;
};

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

struct _NetStreamConnectionQueue
{
        Thread *worker;
        ThreadLock lock;
        pthread_cond_t cond;
        size_t length;
        size_t front;
        size_t count;
        NetStream *stream;
        NetStreamConnection **connections;
        bool exit;
};

#endif
