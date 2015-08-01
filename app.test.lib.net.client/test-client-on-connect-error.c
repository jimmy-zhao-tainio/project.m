#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-client-on-connect-error.h"

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

ThreadSignal ready = THREAD_SIGNAL_INITIALIZER;

bool test_client_on_connect_error (Test *test)
{
        NetClientConnection connection;
        NetClient *client;
        int i;

        TITLE ();
        for (i = 0; i < 100; i++) {
                CATCH (!(client = net_client_create (&client_on_connect,
                                                     &client_on_connect_error,
                                                     &client_on_error,
                                                     NULL)));
                connection.ip = "127.0.0.1";
                connection.port = 8888;
                net_client_connect (client, &connection);
                CATCH (!thread_signal_wait (&ready));
                net_client_destroy (client);
                CATCH (error_count () != 0);
        }
        PASS ();
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        close (connection->socket);
        (void)client;
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        thread_signal (&ready);
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        (void)client;
}
