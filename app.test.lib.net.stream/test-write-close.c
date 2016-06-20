#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-write-close.h"

// Client
static void on_connect       (NetClient *client, NetClientConnection *connection);
static void on_connect_error (NetClient *client, NetClientConnection *connection);
static void on_error         (NetClient *client);

static ThreadSignal connect_signal;
static ThreadSignal add_signal;
static ThreadSignal close_signal;

static void stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void stream_on_close (NetStream *stream, NetStreamConnection *connection);

static NetStreamConnection *stream_connection;

unsigned char *buffer = (unsigned char *)"y";

static int client_socket;

// Server
static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                            unsigned char *buffer, size_t length);

static NetServer *server;
static NetStream *server_stream;
static ThreadSignal server_close_signal;
static NetStreamConnection *server_connection;

bool test_write_close (Test *test)
{
        // Client

        NetClient *client;
        NetClientConnection connect;
        NetStream *stream;
        size_t i;
        
        TITLE ();

        // Client
        thread_signal_create (&connect_signal);
        thread_signal_create (&add_signal);
        thread_signal_create (&close_signal);
        // Create streams.
        if (!(stream = net_stream_create (&stream_on_add, 
                                          &stream_on_close, 
                                          NULL,
                                          NULL))) {
                exit (-1);
        }
        if (!(client = net_client_create (&on_connect,
                                          &on_connect_error,
                                          &on_error,
                                          NULL))) {
                exit (-1);
        }

        // Server
        thread_signal_create (&server_close_signal);
        if (!(server_stream = net_stream_create (&server_stream_on_add, 
                                          &server_stream_on_close, 
                                          &server_stream_on_read,
                                          NULL))) {
                exit (-1);
        }
        if (!(server = net_server_create ("127.0.0.1", 8888,
                                          &server_on_connect,
                                          &server_on_error,
                                          NULL))) {
                exit (-1);
        }


        // Client
        // Connect client.
        connect.ip = "127.0.0.1";
        connect.port = 8888;
        net_client_connect (client, &connect);

        thread_signal_wait (&connect_signal);

        // Add sockets to streams.
        if (!net_stream_add (stream, client_socket)) {
                exit (-1);
        }

        // Wait for add to be completed.
        thread_signal_wait (&add_signal);

        for (i = 0; i < 3; i++) {
                net_stream_write (stream, stream_connection, buffer, 1);
        }

        thread_signal_wait (&close_signal);
        net_stream_remove (stream, stream_connection);
        net_stream_destroy (stream);
        net_client_destroy (client);

        thread_signal_destroy (&connect_signal);
        thread_signal_destroy (&add_signal);
        thread_signal_destroy (&close_signal);

        // Server
        thread_signal_wait (&server_close_signal);
        net_stream_remove (server_stream, server_connection);
        net_stream_destroy (server_stream);
        net_server_destroy (server);
        thread_signal_destroy (&server_close_signal);

        PASS ();
}

// Client

static void stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        stream_connection = connection;
        thread_signal (&add_signal);
}

static void stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        //printf ("client: stream_on_close\n"); fflush (stdout);
        thread_signal (&close_signal);
}

static void on_connect (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        client_socket = connection->socket;
        thread_signal (&connect_signal);
}

static void on_connect_error (NetClient *client, NetClientConnection *connection)
{
        (void)connection;
        (void)client;
        printf ("client_on_connect_error\n"); fflush (stdout);
        exit (-1);
}

static void on_error (NetClient *client)
{
        (void)client;
        printf ("client_on_error\n"); fflush (stdout);
        exit (-1);
}

// Server
static void server_on_connect (NetServer *server, int socket)
{
        (void)server;
        if (!net_stream_add (server_stream, socket)) {
                exit (-1);
        }
}

static void server_on_error (NetServer *server)
{
        (void)server;
        printf ("server_on_error\n"); fflush (stdout);
        exit (-1);
}

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        server_connection = connection;
        (void)stream;
        (void)connection;
}

static void server_stream_on_read (NetStream *stream, 
                            NetStreamConnection *connection, 
                            unsigned char *buffer, 
                            size_t length)
{
        (void)stream;
        (void)connection;
        (void)buffer;
        (void)length;
        // Possible threading issue.
        net_stream_close (stream, connection);
}

static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        thread_signal (&server_close_signal);
}
