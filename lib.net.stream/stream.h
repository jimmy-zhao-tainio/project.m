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
        Thread *thread;
        NetStreamEpoll *epoll;
        ThreadSignal stop;
        void *tag;
};

struct _NetStreamPackage
{
        char *buffer;
        size_t length;
};

struct _NetStreamConnection
{
        int socket;
};

NetStream *net_stream_create (NetStreamOnRead on_read, 
                              NetStreamOnClose on_close,
                              NetStreamOnError on_error,
                              void *tag);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, 
                     NetStreamConnection *connection);
void net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       NetStreamPackage package);

#endif
