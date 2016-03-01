#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "websocket.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void stream_on_add (NetStream *stream, 
                           NetStreamConnection *connection);
static void stream_on_close (NetStream *stream, 
                             NetStreamConnection *connection);
static void stream_on_read (NetStream *stream, 
                            NetStreamConnection *connection, 
                            unsigned char *buffer, 
                            size_t length);

NetWebsocket *net_websocket_create (void)
{
        NetWebsocket *websocket;

        if (!(websocket = memory_create (sizeof (NetWebsocket)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(websocket->stream = net_stream_create (&stream_on_add, 
                                                     &stream_on_close, 
                                                     &stream_on_read,
                                                     websocket))) {
                memory_destroy (websocket);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(websocket->server = net_server_create ("127.0.0.1", 8888,
                                                     &server_on_connect,
                                                     &server_on_error,
                                                     websocket))) {
                net_stream_destroy (websocket->stream);
                memory_destroy (websocket);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return websocket;
}

void net_websocket_destroy (NetWebsocket *websocket)
{
        net_server_destroy (websocket->server);
        net_stream_destroy (websocket->stream);
        memory_destroy (websocket);
}

static void server_on_connect (NetServer *server, int socket)
{
        NetWebsocket *websocket = server->tag;

        

        if (!net_stream_add (websocket->stream, socket)) {
                close (socket);
                error (FunctionCall);
        }
}

static void server_on_error (NetServer *server)
{
        (void)server;
}

static void stream_on_add (NetStream *stream, 
                           NetStreamConnection *connection)
{
        NetWebsocketConnection *websocket;
        
        if (!(websocket = memory_create (sizeof (NetStreamConnection)))) {
                net_stream_close (stream, connection);
                error_code (FunctionCall, 1);
                return;
        }
        if (!net_http_reader_create (&websocket->reader)) {
                memory_destroy (websocket);
                net_stream_close (stream, connection);
                error_code (FunctionCall, 2);
                return;
        }
        connection->tag = websocket;
}

static void stream_on_close (NetStream *stream, 
                             NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void stream_on_read (NetStream *stream, 
                            NetStreamConnection *connection, 
                            unsigned char *buffer, 
                            size_t length)
{
        NetWebsocketConnection *websocket = connection->tag;
        NetHttpRequest request;

        if (!net_http_reader_append (&websocket->reader, (char *)buffer, length)) {
                net_stream_close (stream, connection);
                error_code (FunctionCall, 1);
                return;
        }
        while (net_http_request_begin (&websocket->reader, &request)) {
                net_http_request_end (&websocket->reader, &request);
        }
}
