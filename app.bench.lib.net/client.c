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

static NetClient *client;
static NetStream *stream;

void client_start (void)
{
        client = net_client_create (&client_on_connect,
                                    &client_on_connect_error,
                                    &client_on_error,
                                    NULL);
        stream = net_stream_create (&client_stream_on_add, 
                                    &client_stream_on_close, 
                                    &client_stream_on_read);
}

static void client_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
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
