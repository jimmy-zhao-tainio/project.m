#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-client-on-connect.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

ThreadSignal server_ready = THREAD_SIGNAL_INITIALIZER;
ThreadSignal client_ready = THREAD_SIGNAL_INITIALIZER;
ThreadSignal ready_server = THREAD_SIGNAL_INITIALIZER;
ThreadSignal ready_client = THREAD_SIGNAL_INITIALIZER;

bool test_client_on_connect (Test *test)
{
        NetClientConnection connection;
        NetServer *server;
        NetClient *client;
        int i;

        TITLE ();
        for (i = 0; i < 100; i++) {
                CATCH (!(server = net_server_create ("127.0.0.1", 8888, 
                                                     &server_on_connect, 
                                                     &server_on_error)));
                CATCH (!(client = net_client_create (&client_on_connect,
                                                     &client_on_connect_error,
                                                     &client_on_error)));
                connection.ip = "127.0.0.1";
                connection.port = 8888;
                net_client_connect (client, &connection);
                CATCH (!thread_signal_wait (&server_ready));
                CATCH (!thread_signal_wait (&client_ready));
                CATCH (!thread_signal (&ready_server));
                CATCH (!thread_signal (&ready_client));
                net_server_destroy (server);
                net_client_destroy (client);
                CATCH (error_count () != 0);
        }
        PASS ();
}

static void server_on_connect (NetServer *server, int socket)
{
        thread_signal (&server_ready);
        thread_signal_wait (&ready_server);
        close (socket);
        (void)server;
}

static void server_on_error (NetServer *server)
{
        (void)server;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        thread_signal (&client_ready);
        thread_signal_wait (&ready_client);
        close (connection->socket);
        (void)client;
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
