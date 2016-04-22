#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <lib.net.client/client.h>
#include <lib.core/string.h>
#include <unistd.h>
#include <stdio.h>

#include "test-websocket-add-error-2.h"

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection);

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

static ThreadSignal client_ready = THREAD_SIGNAL_INITIALIZER;

bool test_websocket_add_error_2 (Test *test)
{
        NetWebsocket *websocket;
        NetClientConnection connection;
        NetClient *client;

        TITLE ();
        CATCH (!(websocket = net_websocket_create (&on_add, 
                                                   &on_close, 
                                                   &on_request)));
        websocket->test.AddError2 = true;
        // Connect with client.
        CATCH (!(client = net_client_create (&client_on_connect,
                                             &client_on_connect_error,
                                             &client_on_error,
                                             NULL)));
        connection.ip = "127.0.0.1";
        connection.port = 8888;
        net_client_connect (client, &connection);
        CATCH (!thread_signal_wait (&client_ready));
        usleep (100);
        net_client_destroy (client);
        net_websocket_destroy (websocket);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        CATCH (!string_equals (error_at (0).function, "stream_on_add"));
        PASS ();
}

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}

static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
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
        thread_signal (&client_ready);
        (void)connection;
        (void)client;
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        thread_signal (&client_ready);
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        thread_signal (&client_ready);
        (void)client;
} 
