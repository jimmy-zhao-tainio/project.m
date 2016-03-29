#ifndef lib_net_websocket_h
#define lib_net_websocket_h

#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.websocket/http.h>

typedef struct _NetWebsocket NetWebsocket;
typedef struct _NetWebsocketConnection NetWebsocketConnection;

typedef void (*NetWebsocketOnRequest) (NetWebsocket *websocket,
                                       NetWebsocketConnection *connection);

struct _NetWebsocketConnection {
        NetHttpReader reader;
        bool upgraded;
};

struct _NetWebsocket {
        NetServer *server;
        NetStream *stream;
        NetWebsocketOnRequest on_request;
};

NetWebsocket *net_websocket_create (NetWebsocketOnRequest on_request);
void net_websocket_destroy (NetWebsocket *socket);

#endif
