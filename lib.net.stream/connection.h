#ifndef net_stream_connection_h
#define net_stream_connection_h

#include <lib.net.stream/structures.h>

NetStreamConnection *net_stream_connection_create (int socket);
void net_stream_connection_destroy (NetStreamConnection *connection);

#endif
