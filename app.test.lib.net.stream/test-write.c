#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "test-write.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);
static int  server_socket;

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);
static int  client_socket;

static ThreadSignal server_connect_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_connect_signal = THREAD_SIGNAL_INITIALIZER;

static ThreadSignal server_add_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_add_signal = THREAD_SIGNAL_INITIALIZER;

static ThreadSignal server_close_signal = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_close_signal = THREAD_SIGNAL_INITIALIZER;

static ThreadSignal server_read_signal = THREAD_SIGNAL_INITIALIZER;

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, unsigned char *buffer, size_t length);

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection);

static NetStreamConnection *stream_connection_server;
static NetStreamConnection *stream_connection_client;

static size_t write_count = 2000;
static size_t read_count = 0;

bool test_write (Test *test)
{
        NetServer *server;
        NetClient *client;
        NetClientConnection connect;
        NetStream *stream_server;
        NetStream *stream_client;
        unsigned char *buffer = (unsigned char *)"x";
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

        // Connect client.
        connect.ip = "127.0.0.1";
        connect.port = 8888;
        net_client_connect (client, &connect);

        // Wait for connections to be completed.
        thread_signal_wait (&client_connect_signal);
        thread_signal_wait (&server_connect_signal);

        // Create streams.
        CATCH (!(stream_server = net_stream_create (&server_stream_on_add, 
                                                    &server_stream_on_close, 
                                                    &server_stream_on_read)));
        CATCH (!(stream_client = net_stream_create (&client_stream_on_add, 
                                                    &client_stream_on_close, 
                                                    NULL)));

        // Add sockets to streams.
        CATCH (!net_stream_add (stream_server, server_socket));
        CATCH (!net_stream_add (stream_client, client_socket));

        // Wait for add to be completed.
        thread_signal_wait (&client_add_signal);
        thread_signal_wait (&server_add_signal);

        // Write 2000 times.
        for (i = 0; i < write_count; i++) {
                CATCH (!net_stream_write (stream_client, 
                                          stream_connection_client,
                                          buffer,
                                          1));
        }

        thread_signal_wait (&server_read_signal);

        // Close client connection, should cause server to close as well.
        net_stream_close (stream_client, stream_connection_client);

        // Wait for close to be completed.
        thread_signal_wait (&client_close_signal);
        thread_signal_wait (&server_close_signal);

        // Remove closed streams.
        net_stream_remove (stream_server, stream_connection_server);
        net_stream_remove (stream_client, stream_connection_client);

        // We're done, let's destroy streams.
        net_stream_destroy (stream_server);
        net_stream_destroy (stream_client);

        net_server_destroy (server);
        net_client_destroy (client);
        CATCH (error_count () != 0);
        PASS ();
}

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        stream_connection_server = connection;
        thread_signal (&server_add_signal);
}

static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        thread_signal (&server_close_signal);
}

static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, unsigned char *buffer, size_t length)
{
        read_count += length;

        (void)stream;
        (void)connection;
        (void)buffer;
        (void)length;
        if (read_count == 2000) {
                thread_signal (&server_read_signal);
        }
}

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        stream_connection_client = connection;
        thread_signal (&client_add_signal);
}

static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        thread_signal (&client_close_signal);
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
