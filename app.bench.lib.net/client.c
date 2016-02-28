#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <lib.net.client/client.h>
#include <unistd.h>
#include <stdio.h>

#include "client.h"

static void client_on_connect       (NetClient *client, NetClientConnection *connection);
static void client_on_connect_error (NetClient *client, NetClientConnection *connection);
static void client_on_error         (NetClient *client);

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void client_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length);
static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection);

static void worker (Thread *thread);

static NetClient *client;
static NetStream *stream;

static size_t connections_max;
static size_t connections_count = 0;
static ThreadSignal connections_ready = THREAD_SIGNAL_INITIALIZER;
static NetClientConnection *connections;
static NetStreamConnection **streams;

static size_t package_size;
static size_t seconds;

static unsigned char *buffer;

void client_start (size_t count, size_t _package_size, size_t _seconds)
{
        connections_max = count;
        package_size = _package_size;
        seconds = _seconds;
        
        buffer = memory_create (sizeof (char) * package_size);
        connections = memory_create (sizeof (NetClientConnection) * count);
        streams = memory_create (sizeof (NetStreamConnection *) * count);
        stream = net_stream_create (&client_stream_on_add, 
                                    &client_stream_on_close, 
                                    &client_stream_on_read,
                                    NULL);
        client = net_client_create (&client_on_connect,
                                    &client_on_connect_error,
                                    &client_on_error,
                                    NULL);
        (void)thread_create (&worker, NULL);
}

static void worker (Thread *thread)
{
        size_t i;

        (void)connections_count;
        (void)thread;
        for (i = 0; i < connections_max; i++) {
                connections[i].ip = "127.0.0.1";
                connections[i].port = 8888;
                net_client_connect (client, &connections[i]);
        }
        thread_signal_wait (&connections_ready);
        while (true) {
                for (i = 0; i < connections_max; i++) {
                        if (!net_stream_write (stream,
                                               streams[i],
                                               buffer,
                                               package_size)) {
                                printf ("worker: net_stream_write\n");
                                fflush (stdout);
                                exit (-1);
                        }
                }
        }
}

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        streams[connections_count] = connection;
        connections_count++;
        if (connections_count == connections_max) {
                thread_signal (&connections_ready);
        }
}

static void client_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void client_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length)
{
        (void)stream;
        (void)connection;
        (void)buffer;
        (void)length;
}

static void client_on_connect (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        (void)connection;

        if (!net_stream_add (stream, connection->socket)) {
                printf ("client_on_connect\n");
                exit (-1);
        }
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
