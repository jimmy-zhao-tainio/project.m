#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <lib.net.stream/stream.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <lib.core/threads.h>
#include <unistd.h>
#include <stdio.h>

#include "test-stream-1.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

static void server_stream_on_read (NetStream *stream, 
                                   NetStreamConnection *connection,
                                   NetStreamPackage package);
static void server_stream_on_close (NetStream *stream, 
                                    NetStreamConnection *connection);
static void server_stream_on_error (NetStream *stream);

static void client_stream_on_read (NetStream *stream, 
                                   NetStreamConnection *connection,
                                   NetStreamPackage package);
static void client_stream_on_close (NetStream *stream, 
                                    NetStreamConnection *connection);
static void client_stream_on_error (NetStream *stream);

static ThreadSignal server_ready = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_ready = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal ready_server = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal ready_client = THREAD_SIGNAL_INITIALIZER;

static NetStream *server_stream = NULL;
static NetStream *client_stream = NULL;

static NetStreamConnection server_connection;
static NetStreamConnection client_connection;

static size_t server_read;
static size_t client_read;

bool test_stream_1 (Test *test)
{
        NetClientConnection connection;
        NetServer *server;
        NetClient *client;
        int i;

        TITLE ();
        for (i = 0; i < 1; i++) {
                CATCH (!(server_stream = net_stream_create (&server_stream_on_read,
                                                            &server_stream_on_close,
                                                            &server_stream_on_error,
                                                            2, 1)));
                CATCH (!(client_stream = net_stream_create (&client_stream_on_read,
                                                            &client_stream_on_close,
                                                            &client_stream_on_error,
                                                            2, 1)));
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
                net_stream_destroy (server_stream);
                net_stream_destroy (client_stream);
                CATCH (error_count () != 0);
        }
        PASS ();
}

static void server_on_connect (NetServer *server, int socket)
{
        NetStreamPackage package;
        char buffer[512];
        int i;
        char c;

        server_read = 0;
        server_connection.socket = socket;
        thread_signal_create (&server_connection.write_signal);
        if (!net_stream_add (server_stream, &server_connection)) {
                printf ("server_on_connect\n"); fflush (stdout);
        }
        for (i = 0; i < 8; i++) {
                for (c = 0; c < 64; c++) {
                        buffer[(i * 64) + c] = c;
                }
        }
        package.buffer = buffer;
        package.length = 512;
        for (i = 0; i < 1000; i++) {
                net_stream_write (server_stream,
                                  &server_connection,
                                  package);
        }
        (void)server;
}

static void server_on_error (NetServer *server)
{
        printf ("server_on_error\n"); fflush (stdout);
        (void)server;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        NetStreamPackage package;
        char buffer[512];
        int i;
        char c;

        client_read = 0;
        client_connection.socket = connection->socket;
        thread_signal_create (&client_connection.write_signal);
        if (!net_stream_add (client_stream, &client_connection)) {
                printf ("client_on_connect\n"); fflush (stdout);
        }
        for (i = 0; i < 8; i++) {
                for (c = 0; c < 64; c++) {
                        buffer[(i * 64) + c] = c;
                }
        }
        package.buffer = buffer;
        package.length = 512;
        for (i = 0; i < 1000; i++) {
                (void)package;
                net_stream_write (client_stream,
                                  &client_connection,
                                  package);
        }
        (void)client;
}

static void client_on_connect_error (NetClient *client, NetClientConnection *connection)
{
        printf ("client_on_connect_error\n"); fflush (stdout);
        (void)connection;
        (void)client;
}

static void client_on_error (NetClient *client)
{
        printf ("client_on_error\n"); fflush (stdout);
        (void)client;
}

static void server_stream_on_read (NetStream *stream, 
                                   NetStreamConnection *connection,
                                   NetStreamPackage package)
{
        (void)stream;
        (void)connection;
        (void)package;
        server_read += package.length;
        if (server_read >= 512 * 1000) {
                thread_signal (&server_ready);
        }
}

static void server_stream_on_close (NetStream *stream, 
                                    NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        printf ("server_stream_on_close\n"); fflush (stdout);
}


static void server_stream_on_error (NetStream *stream)
{
        (void)stream;
        printf ("server_stream_on_error\n"); fflush (stdout);
}

static void client_stream_on_read (NetStream *stream, 
                                   NetStreamConnection *connection,
                                   NetStreamPackage package)
{
        (void)stream;
        (void)connection;
        (void)package;
        client_read += package.length;
        if (client_read >= 512 * 1000) {
                thread_signal (&client_ready);
        }
}

static void client_stream_on_close (NetStream *stream, 
                                    NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        printf ("client_stream_on_close\n"); fflush (stdout);
}

static void client_stream_on_error (NetStream *stream)
{
        (void)stream;
        printf ("client_stream_on_error\n"); fflush (stdout);
}
