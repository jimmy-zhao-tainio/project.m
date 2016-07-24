#ifndef lib_net_stream_h
#define lib_net_stream_h

#include <lib.net.poll/poll.h>
#include <lib.core/threads-lock.h>
#include <lib.net.stream/connection.h>
#include <lib.net.stream/connection-queue.h>

typedef struct _NetStream NetStream;
 
/* Runs in one single thread, blocks streams until returned from. */
typedef void (*NetStreamOnAdd)   (NetStream *stream, 
                                  NetStreamConnection *connection);
typedef void (*NetStreamOnClose) (NetStream *stream, 
                                  NetStreamConnection *connection);
typedef void (*NetStreamOnRead)  (NetStream *stream,
                                  NetStreamConnection *connection, 
                                  unsigned char *buffer,
                                  size_t length);

struct _NetStream
{
        NetPoll *poll;
        NetStreamOnAdd on_add;
        NetStreamOnClose on_close;
        NetStreamOnRead on_read;
        NetStreamConnectionQueue *connection_queue;
        void *tag;
};

/* Runs in current thread, blocking. */
NetStream *net_stream_create (NetStreamOnAdd on_add, 
                              NetStreamOnClose on_close, 
                              NetStreamOnRead on_read,
                              void *tag);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, int socket);

/* Runs in worker thread in queue, non blocking. */
bool net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       unsigned char *buffer, 
                       size_t length);
bool net_stream_write_flags (NetStream *stream, 
                             NetStreamConnection *connection, 
                             unsigned char *buffer, 
                             size_t length,
                             NetPollFlag flags);
bool net_stream_close (NetStream *stream, NetStreamConnection *connection);

/* Runs in calling thread. */
void net_stream_remove (NetStream *stream, NetStreamConnection *connection);

#endif
