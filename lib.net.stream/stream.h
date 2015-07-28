#ifndef lib_net_stream_h
#define lib_net_stream_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.core/threads.h>
#include <lib.net.stream/stream-epoll.h>

typedef struct _NetStream           NetStream;
typedef struct _NetStreamPackage    NetStreamPackage;
typedef struct _NetStreamConnection NetStreamConnection;

typedef void (*NetStreamOnRead)  (NetStream *stream, 
                                  NetStreamConnection *connection, 
                                  NetStreamPackage package);
typedef void (*NetStreamOnClose) (NetStream *stream, 
                                  NetStreamConnection *connection);
typedef void (*NetStreamOnError) (NetStream *stream);

struct _NetStream
{
        NetStreamOnClose on_close;
        NetStreamOnRead  on_read;
        NetStreamOnError on_error;
        Thread **threads_read;
        Thread **threads_write;
        size_t threads_read_size;
        size_t threads_write_size;
        NetStreamEpoll *epoll_read;
        NetStreamEpoll *epoll_write;
        ThreadSignal stop_read_signal;
        ThreadSignal stop_write_signal;
};

struct _NetStreamPackage
{
        char *buffer;
        size_t length;
};

struct _NetStreamConnection
{
        int socket;
        ThreadSignal write_signal;
};

NetStream *net_stream_create (NetStreamOnRead on_read, 
                              NetStreamOnClose on_close,
                              NetStreamOnError on_error, 
                              size_t threads_read_count,
                              size_t threads_write_count);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, 
                     NetStreamConnection *connection);
void net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       NetStreamPackage package);

#endif
