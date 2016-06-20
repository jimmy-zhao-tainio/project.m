#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-close-2.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

static void on_monitor (NetPoll *poll, NetPollConnection *connection, bool success);
static void on_close (NetPoll *poll, NetPollConnection *connection, bool success);

static ThreadSignal server_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal monitor_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal close_signal = THREAD_SIGNAL_INITIALIZER;

static int server_socket;
static int client_socket;

static bool monitor_success;
static bool close_success;

static NetPollConnection poll_connection;

bool test_poll_close_2 (Test *test)
{
        NetPoll *poll;
        NetServer *server;
        NetClientConnection connection;
        NetClient *client;

        TITLE ();
        monitor_success = false;
        close_success = false;
        poll_connection.closed = false;
        poll_connection.socket = 0;
        poll_connection.pointer = NULL;
        CATCH (!(server = net_server_create ("127.0.0.1", 8888,
                                             &server_on_connect,
                                             &server_on_error,
                                             NULL)));
        CATCH (!(client = net_client_create (&client_on_connect,
                                             &client_on_connect_error,
                                             &client_on_error,
                                             NULL)));
        connection.ip = "127.0.0.1";
        connection.port = 8888;
        net_client_connect (client, &connection);
        thread_signal_wait (&server_signal);
        CATCH (!(poll = net_poll_create (&on_monitor, &on_close, NULL, NULL)));
        poll_connection.socket = server_socket;
        CATCH (!net_poll_monitor (poll, &poll_connection));
        thread_signal_wait (&monitor_signal);
        CATCH (!monitor_success);
        CATCH (!net_poll_close (poll, &poll_connection));
        thread_signal_wait (&close_signal);
        CATCH (!close_success);
        net_poll_destroy (poll);
        close (server_socket);
        close (client_socket);
        net_server_destroy (server);
        net_client_destroy (client);
        CATCH (error_count () != 0);
        PASS ();
}

static void on_monitor (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        (void)connection;
        (void)success;
        monitor_success = success;
        thread_signal (&monitor_signal);
}

static void on_close (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        (void)connection;
        (void)success;
        close_success = success;
        thread_signal (&close_signal);
}

static void server_on_connect (NetServer *server, int socket)
{
        (void)server;
        server_socket = socket;
        thread_signal (&server_signal);
}

static void server_on_error (NetServer *server)
{
        printf ("server_on_error\n");
        (void)server;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        (void)connection;
        client_socket = connection->socket;
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        printf ("client_on_connect_error\n");
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        printf ("client_on_error\n");
        (void)client;
}
