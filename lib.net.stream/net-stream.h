#ifndef lib_net_stream_h
#define lib_net_stream_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.core/threads.h>
#include <lib.core/list.h>

typedef struct _NetStream           NetStream;
typedef struct _NetStreamConnection NetStreamConnection;
typedef struct _NetStreamPackage    NetStreamPackage;
typedef struct _NetStreamThread     NetStreamThread;

typedef void (*NetStreamOnClose) (NetStream *stream, NetStreamConnection connection);
typedef void (*NetStreamOnRead)  (NetStream *stream, NetStreamConnection connection, NetStreamPackage package);
typedef void (*NetStreamOnError) (NetStream *stream);

struct _NetStream
{
        NetStreamOnClose on_close;
        NetStreamOnRead on_read;
        NetStreamOnError on_error;
        NetStreamThread *threads;
        NetStreamConnection *connections;
        size_t threads_count;
};

struct _NetStreamPackage
{
        char *buffer;
        size_t length;
        size_t position;
};

struct _NetStreamConnection
{
        Object object;
        void *pointer;
        int socket;
        size_t index;
        NetStreamThread *thread;
        NetStreamPackage write;
};

struct _NetStreamThread
{
        Thread *thread;
        ThreadLock lock;
        NetStream *stream;
        NetStreamConnection *connections;
        size_t connections_size;
        size_t connections_count;
};

NetStream *net_stream_create (NetStreamOnClose on_close, 
                              NetStreamOnRead on_read, 
                              NetStreamOnError on_error, 
                              size_t thread_count,
                              size_t connections_per_thread);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, 
                     int socket,
                     void *pointer,
                     NetStreamConnection *connection);
void net_stream_remove (NetStream *stream,
                        NetStreamConnection connection);
void net_stream_write (NetStream *stream, 
                       NetStreamConnection connection, 
                       NetStreamPackage package);

#endif
