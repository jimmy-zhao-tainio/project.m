#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.core/string.h>
#include <lib.encode/encode.h>
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
static void frame_on_read (NetStream *stream, 
                           NetStreamConnection *stream_connection, 
                           unsigned char *buffer, 
                           size_t length);

static bool upgrade_request (NetWebsocket *websocket, 
                             NetWebsocketConnection *connection,
                             NetStreamConnection *stream_connection);

NetWebsocket *net_websocket_create (NetWebsocketOnAdd on_add,
                                    NetWebsocketOnClose on_close,
                                    NetWebsocketOnRequest on_request)
{
        NetWebsocket *websocket;

        if (!on_add) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (!on_close) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_request) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!(websocket = memory_create (sizeof (NetWebsocket)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        websocket->on_add = on_add;
        websocket->on_close = on_close;
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

        if (websocket->test.ConnectError ||
            !net_stream_add (websocket->stream, socket)) {
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
        NetWebsocket *websocket = stream->tag;
        
        if (websocket->test.AddError1 ||
            !(connection = memory_create (sizeof (NetWebsocketConnection)))) {
                net_stream_close (stream, stream_connection);
                error_code (FunctionCall, 1);
                return;
        }
        connection->upgraded = false;
        if (websocket->test.AddError2 ||
            !net_http_reader_create (&connection->reader)) {
                memory_destroy (connection);
                net_stream_close (stream, stream_connection);
                error_code (FunctionCall, 2);
                return;
        }
        stream_connection->tag = connection;
        websocket->on_add (websocket, connection);
}

static void stream_on_close (NetStream *stream, 
                             NetStreamConnection *stream_connection)
{
        NetWebsocket *websocket = stream->tag;
        NetWebsocketConnection *connection = stream_connection->tag;

        net_stream_remove (stream, stream_connection);
        if (connection) {
                // See stream_on_add above.
                websocket->on_close (websocket, connection);
                net_http_reader_destroy (&connection->reader);
                memory_destroy (connection);
        }
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
        else {
                frame_on_read (stream, stream_connection, buffer, length);
        }
}

static void http_on_read (NetStream *stream, 
                          NetStreamConnection *stream_connection, 
                          unsigned char *buffer, 
                          size_t length)
{
        NetWebsocketConnection *connection = stream_connection->tag;
        NetWebsocket *websocket = stream->tag;

        if (websocket->test.HttpOnReadError ||
            !net_http_reader_append (&connection->reader, 
                                     (char *)buffer, 
                                     length)) {
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
                        error_code (FunctionCall, 2);
                        continue;
                }
                if (!upgrade_request (websocket, connection, stream_connection)) {
                        net_stream_close (stream, stream_connection);
                        net_http_request_end (&connection->reader);
                        error_code (FunctionCall, 3);
                        continue;
                }
                net_http_request_end (&connection->reader);
        } 
}

static bool upgrade_request (NetWebsocket *websocket, 
                             NetWebsocketConnection *connection,
                             NetStreamConnection *stream_connection)
{
        unsigned char hash[SHA_DIGEST_LENGTH];
        char *key;
        char *base64;
        char *response;

        // Get Sec-WebSocket-Key header
        if (websocket->test.UpgradeError1 ||
            !(key = net_http_get_header (&connection->reader, 
                                         "Sec-WebSocket-Key"))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (websocket->test.UpgradeError2 ||
            !string_append (&key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11")) {
                string_destroy (key);
                error_code (FunctionCall, 2);
                return false;
        }
        SHA1 ((unsigned char *)key, string_length (key), hash);
        if (websocket->test.UpgradeError3 ||
            !(base64 = encode_base64 ((char *)hash, SHA_DIGEST_LENGTH))) {
                string_destroy (key);
                error_code (FunctionCall, 3);
                return false;
        }
        if (websocket->test.UpgradeError4 ||
            !(response = string_create ("HTTP/1.1 101 Switching Protocols\r\n"
                                        "Upgrade: websocket\r\n"
                                        "Connection: Upgrade\r\n"
                                        "Sec-WebSocket-Accept: ")) ||
            !string_append (&response, base64) ||
            !string_append (&response, "\r\n\r\n")) {
                string_destroy (key);
                string_destroy (base64); 
                error_code (FunctionCall, 4);
                return false;
        }
        connection->upgraded = true;
        if (websocket->test.UpgradeError5 ||
            !net_stream_write_flags (websocket->stream,
                                     stream_connection,
                                     (unsigned char *)response,
                                     string_length (response),
                                     FREE_BUFFER)) {
                string_destroy (key);
                string_destroy (base64);
                string_destroy (response);
                error_code (FunctionCall, 5);
                return false;
        }
        string_destroy (key);
        string_destroy (base64);
        return true;
}

static void frame_on_read (NetStream *stream, 
                           NetStreamConnection *stream_connection, 
                           unsigned char *buffer, 
                           size_t length)
{
        (void)stream;
        (void)stream_connection;
        (void)buffer;
        (void)length;
}
