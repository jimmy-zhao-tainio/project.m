#ifndef lib_net_websocket_h
#define lib_net_websocket_h

#include <lib.net.server/server.h>
#include <lib.net.stream/stream.h>
#include <lib.core/object.h>

typedef struct _NetWebSocket           NetWebSocket;
typedef struct _NetWebSocketConnection NetWebSocketConnection;
typedef struct _NetWebSocketPackage    NetWebSocketPackage;

typedef void (*NetWebSocketOnConnect) (NetWebSocket *web_socket,
                                       NetWebSocketConnection *connection);
typedef void (*NetWebSocketOnRead) (NetWebSocket *web_socket,
                                    NetWebSocketConnection *connection,
                                    NetWebSocketPackage package);
typedef void (*NetWebSocketOnClose) (NetWebSocket *web_socket,
                                     NetWebSocketConnection *connection);
typedef void (*NetWebSocketOnError) (NetWebSocket *web_socket);

struct _NetWebSocket
{
        NetServer *server;
        NetStream *stream;
        NetWebSocketConnection *connections;
        size_t                  connections_size;
        size_t                  connections_count;
        NetWebSocketOnConnect on_connect;
        NetWebSocketOnRead on_read;
        NetWebSocketOnClose on_close;
        NetWebSocketOnError on_error;
        void *tag;
};

struct _NetWebSocketConnection
{
        Object id;
        int index;
        int socket;
};

struct _NetWebSocketPackage
{
        char *buffer;
        size_t length;
};

NetWebSocket *net_web_socket_create (const char *ip, uint16_t port, size_t connections,
                                     NetWebSocketOnConnect on_connect,
                                     NetWebSocketOnRead on_read,
                                     NetWebSocketOnClose on_close,
                                     NetWebSocketOnError on_error,
                                     void *tag);
void net_web_socket_destroy (NetWebSocket *web_socket);

#endif
