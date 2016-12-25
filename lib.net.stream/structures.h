#ifndef lib_net_stream_structures_h
#define lib_net_stream_structures_h

#include <lib.core/defines.h>
#include <lib.core/threads.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>
#include <lib.net.poll/poll.h>
#include <lib.core/queue.h>

typedef struct _NetStream                NetStream;
typedef struct _NetStreamWorker          NetStreamWorker;
typedef struct _NetStreamConnection      NetStreamConnection;
typedef struct _NetStreamTaskQueue       NetStreamTaskQueue;
typedef struct _NetStreamTask            NetStreamTask;
typedef struct _NetStreamTaskAdd         NetStreamTaskAdd;
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
        NET_STREAM_TASK_ADD,
        NET_STREAM_TASK_WRITE,
        NET_STREAM_TASK_WRITE_FLAGS,
        NET_STREAM_TASK_CLOSE
} NetStreamTaskType;

struct _NetStream
{
        // I/O events.
        NetPoll *poll;
        // Application events.
        NetStreamOnAdd on_add;
        NetStreamOnClose on_close;
        NetStreamOnRead on_read;
        // Worker thread
        NetStreamWorker *worker;
        // For public use.
        void *tag;
};

struct _NetStreamWorker
{
        // Worker thread
        Thread *thread;
        // Lock and cond to wait/signal tasks
        ThreadLock lock;
        pthread_cond_t cond;
        // I/O for connections
        NetPoll *poll;
        // Connections with tasks to be processed by the worker
        Queue connections;
        // Flag to signal exiting
        bool exit;
};

struct _NetStreamConnection
{
        // Poll structure
        NetPollConnection poll;
        // Queue of NetStreamTasks
        Queue tasks;
        // Locking and flags to synchronize.
        ThreadLock lock;
        ThreadSignal write_signal;
        ThreadSignal close_signal;
        bool closed;
        bool write_success;
        // For public use.
        void *tag;
};

// Add a socket to poll monitor
struct _NetStreamTaskAdd
{
        bool not_used;
};

// Write buffer
struct _NetStreamTaskWrite
{
        unsigned char *buffer;
        size_t length;
};

// Write buffer, FREE_BUFFER and/or CLOSE_ON_WRITE
struct _NetStreamTaskWriteFlags
{
        unsigned char *buffer;
        size_t length;
        NetPollFlag flags;
};

// Close the connection.
// The "not_used" field is only there for the structure to compile.
struct _NetStreamTaskClose
{
        bool not_used;
};

struct _NetStreamTask
{
        NetStreamTaskType type;
        union {
                NetStreamTaskAdd add;
                NetStreamTaskWrite write;
                NetStreamTaskWriteFlags write_flags;
                NetStreamTaskClose close;
        } value;
};

#endif
