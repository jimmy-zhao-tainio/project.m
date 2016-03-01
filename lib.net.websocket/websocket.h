#ifndef lib_net_websocket_h
#define lib_net_websocket_h

#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.websocket/http.h>

typedef struct _NetWebsocket NetWebsocket;
typedef struct _NetWebsocketConnection NetWebsocketConnection;

struct _NetWebsocketConnection {
        NetHttpReader reader;
};

struct _NetWebsocket {
        NetServer *server;
        NetStream *stream;
};

NetWebsocket *net_websocket_create (void);
void net_websocket_destroy (NetWebsocket *socket);

#endif
