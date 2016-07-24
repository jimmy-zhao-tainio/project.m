#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <lib.net.client/client.h>
#include <lib.core/string.h>
#include <unistd.h>
#include <stdio.h>

#include "test-websocket-upgrade-5.h"

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection);

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection);

static NetStream *stream_client;
static NetStreamConnection *stream_connection_client;

static ThreadSignal websocket_close_ready = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_close_ready = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal stream_add_ready = THREAD_SIGNAL_INITIALIZER;

bool test_websocket_upgrade_5 (Test *test)
{
        NetWebsocket *websocket;
        NetClientConnection connection;
        NetClient *client;
        char *buffer = "GET * HTTP/1.1\r\n"
                       "Upgrade: websocket\r\n"
                       "Connection: Upgrade\r\n"
                       "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                       "Sec-WebSocket-Version: 13\r\n\r\n";


        TITLE ();
        CATCH (!(websocket = net_websocket_create (&on_add, 
                                                   &on_close, 
                                                   &on_request)));
        websocket->test.UpgradeError5 = true;
        // Connect with client.
        CATCH (!(stream_client = net_stream_create (&client_stream_on_add,
                                                    &client_stream_on_close,
                                                    NULL,
                                                    NULL)));
        CATCH (!(client = net_client_create (&client_on_connect,
                                             &client_on_connect_error,
                                             &client_on_error,
                                             NULL)));
        connection.ip = "127.0.0.1";
        connection.port = 8888;
        net_client_connect (client, &connection);
        CATCH (!thread_signal_wait (&stream_add_ready));
        net_stream_write (stream_client,
                          stream_connection_client,
                          (unsigned char *)buffer,
                          string_length (buffer));
        CATCH (!thread_signal_wait (&websocket_close_ready));
        CATCH (!thread_signal_wait (&client_close_ready));
        net_stream_remove (stream_client, stream_connection_client);
        net_stream_destroy (stream_client);
        net_client_destroy (client);
        net_websocket_destroy (websocket);
        CATCH (error_at (1).error != ErrorFunctionCall);
        CATCH (error_at (1).code != 5);
        CATCH (!string_equals (error_at (1).function, "upgrade_request"));
        PASS ();
}

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}

static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        thread_signal (&websocket_close_ready);
        (void)websocket;
        (void)connection;
}

static void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        // Write
        (void)connection;
        (void)client;
        net_stream_add (stream_client, connection->socket);
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        (void)client;
} 

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        stream_connection_client = connection;
        thread_signal (&stream_add_ready);
}

static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        thread_signal (&client_close_ready);
}
