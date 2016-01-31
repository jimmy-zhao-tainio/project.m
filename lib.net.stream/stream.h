#ifndef lib_net_stream_h
#define lib_net_stream_h

#include <lib.net.poll/poll.h>

typedef struct _NetStream           NetStream;
typedef struct _NetStreamConnection NetStreamConnection;
 
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
};

struct _NetStreamConnection
{
        NetPollConnection poll;
        bool is_writing;
};

NetStream *net_stream_create (NetStreamOnAdd on_add, 
                              NetStreamOnClose on_close, 
                              NetStreamOnRead on_read);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, int socket);
bool net_stream_write (NetStream *stream, 
                       NetStreamConnection *connection, 
                       unsigned char *buffer, 
                       size_t length);

#endif
