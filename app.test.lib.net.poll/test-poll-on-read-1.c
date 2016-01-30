#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-on-read-1.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);
static ThreadSignal server_signal = THREAD_SIGNAL_INITIALIZER;
static int server_socket;

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);
static int  client_socket;

static void on_monitor (NetPoll *poll, NetPollConnection *connection, bool success);

static void on_read (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length);
static ThreadSignal read_signal = THREAD_SIGNAL_INITIALIZER;
static bool read_success;

static void on_close (NetPoll *poll, NetPollConnection *connection, bool success);
static ThreadSignal close_signal = THREAD_SIGNAL_INITIALIZER;
static bool close_success;

bool test_poll_on_read_1 (Test *test)
{
        NetPoll *poll;
        NetServer *server;
        NetClientConnection connection;
        NetPollConnection poll_connection = { 0 };
        NetClient *client;

        TITLE ();
        read_success = false;
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
        CATCH (!(poll = net_poll_create (&on_monitor, &on_close, &on_read, NULL)));
        poll_connection.socket = server_socket;
        CATCH (!net_poll_monitor (poll, &poll_connection));
        thread_signal_wait (&read_signal);
        CATCH (!read_success);
        close (client_socket);
        thread_signal_wait (&close_signal);
        CATCH (!close_success);
        net_poll_destroy (poll);
        close (server_socket);
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
}

static void on_read (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length)
{
        (void)poll;
        (void)connection;
        if (length == 1 && buffer[0] == 'X') {
                read_success = true;
        }
        else {
                read_success = false;
        }
        thread_signal (&read_signal);
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
        char *buffer = "X";
        (void)client;
        client_socket = connection->socket;
        write (client_socket, buffer, 1);
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
