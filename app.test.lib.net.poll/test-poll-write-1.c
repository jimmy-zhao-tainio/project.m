#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-write-1.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);
static int  server_socket;

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);
static int  client_socket;

static ThreadSignal server_connect_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_connect_signal = THREAD_SIGNAL_INITIALIZER;

static ThreadSignal server_monitor_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_monitor_signal = THREAD_SIGNAL_INITIALIZER;

static ThreadSignal server_close_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_close_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_read_signal = THREAD_SIGNAL_INITIALIZER;

static NetPollConnection poll_client = { .closed = false, .write = { .lock = THREAD_LOCK_INITIALIZER } };
static NetPollConnection poll_server = { .closed = false, .write = { .lock = THREAD_LOCK_INITIALIZER } };

static void poll_on_monitor (NetPoll *poll, NetPollConnection *connection, bool success);
static void poll_on_read    (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length);
static void poll_on_close   (NetPoll *poll, NetPollConnection *connection, bool success);

bool test_poll_write_1 (Test *test)
{
        NetPoll *poll;
        NetServer *server;
        NetClient *client;
        NetClientConnection connect;
        unsigned char *buffer = (unsigned char *)"X";

        TITLE ();
        CATCH (!(server = net_server_create ("127.0.0.1", 8888,
                                             &server_on_connect,
                                             &server_on_error,
                                             NULL)));
        CATCH (!(client = net_client_create (&client_on_connect,
                                             &client_on_connect_error,
                                             &client_on_error,
                                             NULL)));
        CATCH (!(poll = net_poll_create (&poll_on_monitor, 
                                         &poll_on_close, 
                                         &poll_on_read)));

        // Connect client.
        connect.ip = "127.0.0.1";
        connect.port = 8888;
        net_client_connect (client, &connect);

        // Wait for connections to be completed.
        thread_signal_wait (&client_connect_signal);
        thread_signal_wait (&server_connect_signal);

        // Poll client and server connection.
        poll_client.socket = client_socket;
        poll_server.socket = server_socket;
        CATCH (!net_poll_monitor (poll, &poll_client));
        CATCH (!net_poll_monitor (poll, &poll_server));

        // Wait for monitors to be completed.
        thread_signal_wait (&client_monitor_signal);
        thread_signal_wait (&server_monitor_signal);

        // Write and wait.
        CATCH (!net_poll_write (poll, &poll_server, buffer, 1));
        thread_signal_wait (&client_read_signal);

        // We're done, let's close poll client.
        net_poll_close (poll, &poll_client);

        // Now wait for client and server poll to close.
        thread_signal_wait (&client_close_signal);
        thread_signal_wait (&server_close_signal);

        net_poll_destroy (poll);
        net_server_destroy (server);
        net_client_destroy (client);
        CATCH (error_count () != 0);
        PASS ();
}

static void poll_on_monitor (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        if (success == false) {
                printf ("on_monitor: success == false\n"); fflush (stdout);
                exit (-1);
        }
        if (connection == &poll_server) {
                thread_signal (&server_monitor_signal);
        }
        if (connection == &poll_client) {
                thread_signal (&client_monitor_signal);
        }
}

static void poll_on_read (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length)
{
        (void)poll;
        (void)connection;
        (void)buffer;
        (void)length;
        if (length == 1 && buffer[0] == 'X') {
                thread_signal (&client_read_signal);
        }
        else {
                printf ("poll_on_read: read something strange\n");
                fflush (stdout);
                exit (-1);
        }
}

static void poll_on_close (NetPoll *poll, NetPollConnection *connection, bool success)
{
        (void)poll;
        if (success == false) {
                printf ("on_close: success == false\n"); fflush (stdout);
                exit (-1);
        }
        if (connection == &poll_server) {
                thread_signal (&server_close_signal);
        }
        if (connection == &poll_client) {
                thread_signal (&client_close_signal);
        }
}

static void server_on_connect (NetServer *server, int socket)
{
        (void)server;
        server_socket = socket;
        thread_signal (&server_connect_signal);
}

static void server_on_error (NetServer *server)
{
        printf ("server_on_error\n"); fflush (stdout);
        exit (-1);
        (void)server;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        client_socket = connection->socket;
        thread_signal (&client_connect_signal);
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        printf ("client_on_connect_error\n"); fflush (stdout);
        exit (-1);
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        printf ("client_on_error\n"); fflush (stdout);
        exit (-1);
        (void)client;
}
