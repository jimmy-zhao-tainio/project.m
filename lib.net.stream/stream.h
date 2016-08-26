#ifndef lib_net_stream_h
#define lib_net_stream_h

#include <lib.net.poll/poll.h>
#include <lib.core/threads-lock.h>
#include <lib.net.stream/structures.h>

NetStream *net_stream_create (NetStreamOnAdd on_add, 
                              NetStreamOnClose on_close, 
                              NetStreamOnRead on_read,
                              void *tag);
void net_stream_destroy (NetStream *stream);
bool net_stream_add (NetStream *stream, int socket);
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
void net_stream_remove (NetStream *stream, NetStreamConnection *connection);

#endif
