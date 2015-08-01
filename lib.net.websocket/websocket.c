#include <lib.net.websocket/websocket.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static void on_server_connect (NetServer *server, 
                               int socket);
static void on_server_error (NetServer *server);
static void on_stream_read (NetStream *stream,
                            NetStreamConnection *connection,
                            NetStreamPackage package);
static void on_stream_close (NetStream *stream,
                             NetStreamConnection *connection);
static void on_stream_error (NetStream *stream);

NetWebSocket *net_web_socket_create (const char *ip, uint16_t port, size_t connections,
                                     NetWebSocketOnConnect on_connect,
                                     NetWebSocketOnRead on_read,
                                     NetWebSocketOnClose on_close,
                                     NetWebSocketOnError on_error,
                                     void *tag)
{
        NetWebSocket *web_socket;

        if (!on_connect) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (!on_read) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_close) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!on_error) {
                error_code (InvalidArgument, 4);
                return NULL;
        }
        if (!(web_socket = memory_create (sizeof (NetWebSocket)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        web_socket->on_connect = on_connect;
        web_socket->on_read = on_read;
        web_socket->on_close = on_close;
        web_socket->on_error = on_error;
        web_socket->tag = tag;
        if (!(web_socket->connections = memory_create (sizeof (NetWebSocketConnection) *
                                                       connections))) {
                memory_destroy (web_socket);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(web_socket->server = net_server_create (ip, port,
                                                      &on_server_connect, 
                                                      &on_server_error,
                                                      web_socket))) {
                memory_destroy (web_socket->connections);
                memory_destroy (web_socket);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(web_socket->stream = net_stream_create (&on_stream_read, 
                                                      &on_stream_close,
                                                      &on_stream_error,
                                                      web_socket))) {
                net_server_destroy (web_socket->server);
                memory_destroy (web_socket->connections);
                memory_destroy (web_socket);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return web_socket;
}

void net_web_socket_destroy (NetWebSocket *web_socket)
{
        if (!web_socket) {
                error (InvalidArgument);
                return;
        }
        net_server_destroy (web_socket->server);
        net_stream_destroy (web_socket->stream);
        memory_destroy (web_socket->connections);
        memory_destroy (web_socket);
}

static void on_server_connect (NetServer *server, 
                               int socket)
{
        (void)server;
        (void)socket;
}

static void on_server_error (NetServer *server)
{
        (void)server;
}

static void on_stream_read (NetStream *stream,
                            NetStreamConnection *connection,
                            NetStreamPackage package)
{
        (void)stream;
        (void)connection;
        (void)package;
}

static void on_stream_close (NetStream *stream,
                             NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void on_stream_error (NetStream *stream)
{
        (void)stream;
}
