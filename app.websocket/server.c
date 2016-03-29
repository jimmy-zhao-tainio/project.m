#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <unistd.h>
#include <stdio.h>

#include "server.h"

static NetWebsocket *websocket;

void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection);

void server_start (void)
{
        websocket = net_websocket_create (&on_request);
}

void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        size_t i;
        NetHttpReader reader = connection->reader;

        printf ("uri: ");
        for (i = 0; i < reader.request.uri_length; i++) {
                putchar (reader.buffer[reader.request.uri_begin + i]);
        }
        printf ("\n");
        printf ("version: ");
        for (i = 0; i < reader.request.version_length; i++) {
                putchar (reader.buffer[reader.request.version_begin + i]);
        }
        printf ("\n");
        printf ("headers: ");
        for (i = 0; i < reader.request.headers_length; i++) {
                putchar (reader.buffer[reader.request.headers_begin + i]);
        }
        printf ("\n");

        (void)websocket;
        (void)connection;
}
