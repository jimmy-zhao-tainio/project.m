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

NetWebSocket *net_web_socket_create (const char *ip, uint16_t port)
{
        NetWebSocket *web_socket;

        if (!(web_socket = memory_create (sizeof (NetWebSocket)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(web_socket->server = net_server_create (ip, port, 
                                                      &on_server_connect, 
                                                      &on_server_error))) {
                memory_destroy (web_socket);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(web_socket->stream = net_stream_create (&on_stream_read, 
                                                      &on_stream_close,
                                                      &on_stream_error))) {
                net_server_destroy (web_socket->server);
                memory_destroy (web_socket);
                error_code (FunctionCall, 3);
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
