#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <lib.net.stream/stream.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <lib.core/threads.h>
#include <unistd.h>
#include <stdio.h>

#include "test-stream-10x512x10.h"

static void worker_server (Thread *thread);
static void worker_client (Thread *thread);

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
static ThreadSignal server_worker_start = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_worker_start = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal server_worker_ready = THREAD_SIGNAL_INITIALIZER;
static ThreadSignal client_worker_ready = THREAD_SIGNAL_INITIALIZER;

static NetStream *server_stream = NULL;
static NetStream *client_stream = NULL;

#define CONNECTIONS 10
#define BUFFER_SIZE 512
#define BUFFER_REPEAT 10

static NetStreamConnection server_connection[CONNECTIONS];
static NetStreamConnection client_connection[CONNECTIONS];

static size_t server_read = 0;
static size_t client_read = 0;

static size_t server_count = 0;
static size_t client_count = 0;

static ThreadLock server_lock = THREAD_LOCK_INITIALIZER;
static ThreadLock client_lock = THREAD_LOCK_INITIALIZER;

bool test_stream_10x512x10 (Test *test)
{
        NetClientConnection connection[CONNECTIONS];
        NetServer *server;
        NetClient *client;
        Thread *thread_server;
        Thread *thread_client;
        size_t i;

        TITLE ();
        CATCH (!(thread_server = thread_create (&worker_server, NULL)));
        CATCH (!(thread_client = thread_create (&worker_client, NULL)));
        CATCH (!(server_stream = net_stream_create (&server_stream_on_read,
                                                    &server_stream_on_close,
                                                    &server_stream_on_error)));
        CATCH (!(client_stream = net_stream_create (&client_stream_on_read,
                                                    &client_stream_on_close,
                                                    &client_stream_on_error)));
        CATCH (!(server = net_server_create ("127.0.0.1", 8888, 
                                             &server_on_connect, 
                                             &server_on_error)));
        CATCH (!(client = net_client_create (&client_on_connect,
                                             &client_on_connect_error,
                                             &client_on_error)));
        for (i = 0; i < CONNECTIONS; i++) {
                connection[i].ip = "127.0.0.1";
                connection[i].port = 8888;
                net_client_connect (client, &connection[i]);
        }
        CATCH (!thread_signal_wait (&server_worker_ready));
        CATCH (!thread_signal_wait (&client_worker_ready));
        net_server_destroy (server);
        net_client_destroy (client);
        net_stream_destroy (server_stream);
        net_stream_destroy (client_stream);
        thread_destroy (thread_server);
        thread_destroy (thread_client);
        CATCH (error_count () != 0);
        PASS ();
}

static void worker_server (Thread *thread)
{
        NetStreamPackage package;
        char buffer[BUFFER_SIZE] = { 0 };
        int i, sc;

        (void)thread;
        thread_signal_wait (&server_worker_start);
        package.buffer = buffer;
        package.length = BUFFER_SIZE;
        for (i = 0; i < BUFFER_REPEAT; i++) {
                for (sc = 0; sc < CONNECTIONS; sc++) {
                        net_stream_write (server_stream,
                                          &server_connection[sc],
                                          package);
                }
        }
        thread_signal_wait (&server_ready);
        thread_signal (&server_worker_ready);
}

static void worker_client (Thread *thread)
{
        NetStreamPackage package;
        char buffer[BUFFER_SIZE] = { 0 };
        int i, sc;

        (void)thread;
        thread_signal_wait (&client_worker_start);
        package.buffer = buffer;
        package.length = BUFFER_SIZE;
        for (i = 0; i < BUFFER_REPEAT; i++) {
                for (sc = 0; sc < CONNECTIONS; sc++) {
                        net_stream_write (client_stream,
                                          &client_connection[sc],
                                          package);
                }
        }
        thread_signal_wait (&client_ready);
        thread_signal (&client_worker_ready);
}

static void server_on_connect (NetServer *server, int socket)
{
        bool is_ready = false;

        thread_lock (&server_lock);
        server_connection[server_count].socket = socket;
        thread_signal_create (&server_connection[server_count].write_signal);
        if (!net_stream_add (server_stream, &server_connection[server_count])) {
                printf ("server_on_connect\n"); fflush (stdout);
        }
        server_count++;
        if (server_count == CONNECTIONS) {
                is_ready = true;
        }
        thread_unlock (&server_lock);
        if (is_ready) {
                thread_signal (&server_worker_start);
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
        bool is_ready = false;

        thread_lock (&client_lock);
        client_connection[client_count].socket = connection->socket;
        thread_signal_create (&client_connection[client_count].write_signal);
        if (!net_stream_add (client_stream, &client_connection[client_count])) {
                printf ("client_on_connect\n"); fflush (stdout);
        }
        client_count++;
        if (client_count == CONNECTIONS) {
                is_ready = true;
        }
        thread_unlock (&client_lock);
        if (is_ready) {
                thread_signal (&client_worker_start);
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
        size_t count;

        (void)stream;
        (void)connection;
        (void)package;
        thread_lock (&server_lock);
        server_read += package.length;
        count = server_read;
        thread_unlock (&server_lock);
        if (count >= CONNECTIONS * BUFFER_SIZE * BUFFER_REPEAT) {
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
        size_t count;

        (void)stream;
        (void)connection;
        (void)package;
        thread_lock (&client_lock);
        client_read += package.length;
        count = client_read;
        thread_unlock (&client_lock);
        if (count >= CONNECTIONS * BUFFER_SIZE * BUFFER_REPEAT) {
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
