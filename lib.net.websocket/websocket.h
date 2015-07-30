#ifndef lib_net_websocket_h
#define lib_net_websocket_h

#include <lib.net.server/server.h>
#include <lib.net.stream/stream.h>

typedef struct _NetWebSocket NetWebSocket;

struct _NetWebSocket
{
        NetServer *server;
        NetStream *stream;
};

NetWebSocket *net_web_socket_create (const char *ip, uint16_t port);
void net_web_socket_destroy (NetWebSocket *web_socket);

#endif
