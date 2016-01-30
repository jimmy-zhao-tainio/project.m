#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll-write-3.h"

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

static ThreadSignal server_read_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_read_signal = THREAD_SIGNAL_INITIALIZER;

static NetPollConnection poll_client = { .closed = false };
static NetPollConnection poll_server = { .closed = false };

static void poll_on_monitor (NetPoll *poll, NetPollConnection *connection, bool success);
static void poll_on_close   (NetPoll *poll, NetPollConnection *connection, bool success);
static void poll_on_read    (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length);
static void poll_on_write   (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length);

#define BufferSize 10000000 /* Large enough size to cause an EWOULDBLOCK */

static unsigned char *write_buffer;

static size_t client_read_count = 0;
static size_t server_read_count = 0;

bool test_poll_write_3 (Test *test)
{
        NetPoll *poll;
        NetServer *server;
        NetClient *client;
        NetClientConnection connect;
        size_t i;

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
                                         &poll_on_read,
                                         &poll_on_write)));

        // Allocate buffer
        CATCH (!(write_buffer = memory_create (BufferSize)));
        for (i = 0; i < BufferSize; i++) {
                write_buffer[i] = (unsigned char)i;
        }

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
        CATCH (!net_poll_write (poll, &poll_server, write_buffer, BufferSize));
        CATCH (!net_poll_write (poll, &poll_client, write_buffer, BufferSize));
        thread_signal_wait (&client_read_signal);
        thread_signal_wait (&server_read_signal);

        // We're done, let's close poll client.
        net_poll_close (poll, &poll_client);

        // Now wait for client and server poll to close.
        thread_signal_wait (&client_close_signal);
        thread_signal_wait (&server_close_signal);

        net_poll_destroy (poll);
        net_server_destroy (server);
        net_client_destroy (client);
        memory_destroy (write_buffer);
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

static void poll_on_read (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length)
{
        size_t i;

        (void)poll;
        (void)connection;
        (void)buffer;
        (void)length;

        if (connection == &poll_client) {
                for (i = 0; i < length; i++, client_read_count++) {
                        if (buffer[i] != (unsigned char)client_read_count) {
                                printf ("poll_on_read: read something strange\n");
                                fflush (stdout);
                                exit (-1);
                        }
                }
                if (client_read_count == BufferSize) {
                        thread_signal (&client_read_signal);
                }
        }
        else if (connection == &poll_server) {
                for (i = 0; i < length; i++, server_read_count++) {
                        if (buffer[i] != (unsigned char)server_read_count) {
                                printf ("poll_on_read: read something strange\n");
                                fflush (stdout);
                                exit (-1);
                        }
                }
                if (server_read_count == BufferSize) {
                        thread_signal (&server_read_signal);
                }
        }
        else {
                printf ("Connection is not known\n");
                fflush (stdout);
                exit (-1);
        }
}

static void poll_on_write (NetPoll *poll, NetPollConnection *connection, unsigned char *buffer, size_t length)
{
        (void)poll;
        (void)connection;
        (void)buffer;
        (void)length;
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
