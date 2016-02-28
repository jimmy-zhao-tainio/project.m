#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <unistd.h>
#include <stdio.h>

#include "server.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length);

static NetServer *server;
static NetStream *stream;

void server_start (void)
{
        stream = net_stream_create (&server_stream_on_add, 
                                    &server_stream_on_close, 
                                    &server_stream_on_read,
                                    NULL);
        server = net_server_create ("127.0.0.1", 8888,
                                    &server_on_connect,
                                    &server_on_error,
                                    NULL);
}

static void server_on_connect (NetServer *server, int socket)
{
        (void)server;
        if (!net_stream_add (stream, socket)) {
                printf ("server_on_connect\n");
                exit (-1);
        }
}

static void server_on_error (NetServer *server)
{
        (void)server;
        printf ("server_on_error\n");
        exit (-1);
}

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        printf ("server_stream_on_close\n");
        exit (-1);
}

static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length)
{
        size_t i;

        (void)stream;
        (void)connection;
        (void)buffer;
        for (i = 0; i < length; i++) {
                putchar (buffer[i]);
        }
        fflush (stdout);
}
