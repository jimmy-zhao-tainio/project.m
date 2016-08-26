#ifndef net_stream_connection_queue_h
#define net_stream_connection_queue_h

#include <lib.net.stream/structures.h>

NetStreamConnectionQueue *net_stream_connection_queue_create (NetStream *stream);
void net_stream_connection_queue_destroy (NetStreamConnectionQueue *connection_queue);
bool net_stream_connection_queue_push (NetStreamConnectionQueue *connection_queue, 
                                       NetStreamConnection *connection, 
                                       NetStreamTask task);

#endif
