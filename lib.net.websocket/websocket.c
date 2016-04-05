#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.core/string.h>
#include <openssl/sha.h>
#include <unistd.h>
#include <stdio.h>

#include "websocket.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void stream_on_add (NetStream *stream, 
                           NetStreamConnection *stream_connection);
static void stream_on_close (NetStream *stream, 
                             NetStreamConnection *stream_connection);
static void stream_on_read (NetStream *stream, 
                            NetStreamConnection *stream_connection, 
                            unsigned char *buffer, 
                            size_t length);
static void http_on_read (NetStream *stream, 
                          NetStreamConnection *stream_connection, 
                          unsigned char *buffer, 
                          size_t length);

static void upgrade_request (NetWebsocket *websocket, NetWebsocketConnection *connection);

NetWebsocket *net_websocket_create (NetWebsocketOnRequest on_request)
{
        NetWebsocket *websocket;

        if (!(websocket = memory_create (sizeof (NetWebsocket)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        websocket->on_request = on_request;
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
                           NetStreamConnection *stream_connection)
{
        NetWebsocketConnection *connection;
        
        if (!(connection = memory_create (sizeof (NetWebsocketConnection)))) {
                net_stream_close (stream, stream_connection);
                error_code (FunctionCall, 1);
                return;
        }
        connection->upgraded = false;
        if (!net_http_reader_create (&connection->reader)) {
                memory_destroy (connection);
                net_stream_close (stream, stream_connection);
                error_code (FunctionCall, 2);
                return;
        }
        stream_connection->tag = connection;
}

static void stream_on_close (NetStream *stream, 
                             NetStreamConnection *stream_connection)
{
        (void)stream;
        (void)stream_connection;
}

static void stream_on_read (NetStream *stream, 
                            NetStreamConnection *stream_connection, 
                            unsigned char *buffer, 
                            size_t length)
{
        NetWebsocketConnection *connection = stream_connection->tag;

        if (!connection->upgraded) {
                http_on_read (stream, stream_connection, buffer, length);
        }
}

static void http_on_read (NetStream *stream, 
                          NetStreamConnection *stream_connection, 
                          unsigned char *buffer, 
                          size_t length)
{
        NetWebsocketConnection *connection = stream_connection->tag;
        NetWebsocket *websocket = stream->tag;
        size_t i;

        for (i = 0; i < length; i++)
                putchar (buffer[i]);

        if (!net_http_reader_append (&connection->reader, (char *)buffer, length)) {
                net_stream_close (stream, stream_connection);
                error_code (FunctionCall, 1);
                return;
        }
        while (net_http_request_begin (&connection->reader)) {
                if (!net_http_set_method (&connection->reader) ||
                    !net_http_set_uri (&connection->reader) ||
                    !net_http_set_version (&connection->reader) ||
                    !net_http_set_headers (&connection->reader)) {
                        net_stream_close (stream, stream_connection);
                        net_http_request_end (&connection->reader);
                        continue;
                }
                upgrade_request (websocket, connection);
                //websocket->on_request (websocket, connection);
                net_http_request_end (&connection->reader);
        } 
}

static void upgrade_request (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        unsigned char hash[SHA_DIGEST_LENGTH];
        char *key;
        size_t i;

        // Get Sec-WebSocket-Key header
        if (!(key = net_http_get_header (&connection->reader, "Sec-WebSocket-Key"))) {
                printf ("Failed to get Sec-WebSocket-Key\n");
                return;
        }
        if (!string_append (&key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")) {
                return;
        }
        SHA1 ((unsigned char *)key, string_length (key), hash);
        printf ("Key: '%s', hash: '", key);
        for (i = 0; i < SHA_DIGEST_LENGTH; i++)
                putchar ((char)hash[i]);
        printf ("'\n");
        string_destroy (key);
        (void)websocket;
        (void)connection;
}
