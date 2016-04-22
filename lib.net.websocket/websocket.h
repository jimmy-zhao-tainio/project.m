#ifndef lib_net_websocket_h
#define lib_net_websocket_h

#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.websocket/http.h>

typedef struct _NetWebsocket NetWebsocket;
typedef struct _NetWebsocketTest NetWebsocketTest;
typedef struct _NetWebsocketConnection NetWebsocketConnection;

typedef void (*NetWebsocketOnAdd) (NetWebsocket *websocket,
                                   NetWebsocketConnection *connection);
typedef void (*NetWebsocketOnClose) (NetWebsocket *websocket,
                                     NetWebsocketConnection *connection);
typedef void (*NetWebsocketOnRequest) (NetWebsocket *websocket,
                                       NetWebsocketConnection *connection);

struct _NetWebsocketConnection {
        NetHttpReader reader;
        bool upgraded;
};

struct _NetWebsocketTest {
        bool ConnectError;
        bool AddError1;
        bool AddError2;
        bool HttpOnReadError;
};

struct _NetWebsocket {
        NetServer *server;
        NetStream *stream;
        NetWebsocketOnAdd on_add;
        NetWebsocketOnClose on_close;
        NetWebsocketOnRequest on_request;
        NetWebsocketTest test;
};

NetWebsocket *net_websocket_create (NetWebsocketOnAdd on_add,
                                    NetWebsocketOnClose on_close,
                                    NetWebsocketOnRequest on_request);
void net_websocket_destroy (NetWebsocket *socket);

#endif
