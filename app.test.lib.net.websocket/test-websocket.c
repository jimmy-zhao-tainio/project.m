#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <lib.core/string.h>
#include <unistd.h>
#include <stdio.h>

#include "test-websocket.h"

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection);
static void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection);

bool test_websocket_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_websocket_create (NULL, NULL, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_websocket_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_websocket_create (&on_add, NULL, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_websocket_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_websocket_create (&on_add, &on_close, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_websocket_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetWebsocket) - 1);
        CATCH (net_websocket_create (&on_add, &on_close, &on_request));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_websocket_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetWebsocket) +
                             sizeof (size_t) + sizeof (NetStream) +
                             sizeof (size_t) + sizeof (NetPoll) +
                             sizeof (size_t) + sizeof (NetPollEvents) +
                             sizeof (size_t) + (1024 * sizeof (NetPollEvent)) +
                             sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_websocket_create (&on_add, &on_close, &on_request));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_websocket_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetWebsocket) +
                             sizeof (size_t) + sizeof (NetStream) +
                             sizeof (size_t) + sizeof (NetPoll) +
                             sizeof (size_t) + sizeof (NetPollEvents) +
                             sizeof (size_t) + (1024 * sizeof (NetPollEvent)) +
                             sizeof (size_t) + sizeof (Thread) +
                             sizeof (size_t) + sizeof (NetServer) +
                             sizeof (size_t) + sizeof (NetServerEpoll) +
                             sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_websocket_create (&on_add, &on_close, &on_request));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_websocket_create (Test *test)
{
        NetWebsocket *websocket;

        TITLE ();
        CATCH (!(websocket = net_websocket_create (&on_add, 
                                                   &on_close, 
                                                   &on_request)));
        net_websocket_destroy (websocket);
        PASS ();
}

static void on_add (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}

static void on_close (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}

static void on_request (NetWebsocket *websocket, NetWebsocketConnection *connection)
{
        (void)websocket;
        (void)connection;
}
