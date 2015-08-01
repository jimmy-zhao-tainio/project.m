#include <lib.net.websocket/websocket.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-web-socket.h"

static void on_connect (NetWebSocket *web_socket,
                        NetWebSocketConnection *connection);
static void on_read (NetWebSocket *web_socket,
                     NetWebSocketConnection *connection,
                     NetWebSocketPackage package);
static void on_close (NetWebSocket *web_socket,
                      NetWebSocketConnection *connection);
static void on_error (NetWebSocket *web_socket);

bool test_web_socket_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      NULL, 
                                      NULL, 
                                      NULL, 
                                      NULL, 
                                      NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_web_socket_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      NULL, 
                                      NULL, 
                                      NULL, 
                                      NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_web_socket_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      NULL, 
                                      NULL, 
                                      NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_web_socket_create_invalid_argument_4 (Test *test)
{
        TITLE ();
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      &on_close, 
                                      NULL, 
                                      NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_web_socket_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetWebSocket) - 1);
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      &on_close, 
                                      &on_error, 
                                      NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_web_socket_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetWebSocket) +
                             sizeof (size_t) + (sizeof (NetWebSocketConnection) * 10) -
                             1);
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      &on_close, 
                                      &on_error, 
                                      NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_web_socket_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetWebSocket) +
                                   sizeof (size_t) + (sizeof (NetWebSocketConnection) * 10) +
                                   sizeof (size_t) + sizeof (NetServer) +
                                   sizeof (size_t) + sizeof (NetServerEpoll) +
                                   sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      &on_close, 
                                      &on_error, 
                                      NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_web_socket_create_function_call_4 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetWebSocket) +
                                   sizeof (size_t) + (sizeof (NetWebSocketConnection) * 10) +
                                   sizeof (size_t) + sizeof (NetServer) +
                                   sizeof (size_t) + sizeof (NetServerEpoll) +
                                   sizeof (size_t) + sizeof (Thread) +
                                   sizeof (size_t) + sizeof (NetStream) +
                                   sizeof (size_t) + sizeof (NetStreamEpoll) +
                                   sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_web_socket_create ("127.0.0.1", 8888, 10,
                                      &on_connect, 
                                      &on_read, 
                                      &on_close, 
                                      &on_error, 
                                      NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_web_socket_create (Test *test)
{
        NetWebSocket *web_socket;

        TITLE ();
        CATCH (!(web_socket = net_web_socket_create ("127.0.0.1", 8888, 10,
                                                     &on_connect, 
                                                     &on_read, 
                                                     &on_close, 
                                                     &on_error, 
                                                     NULL)));
        CATCH (error_count () != 0);
        net_web_socket_destroy (web_socket);
        PASS ();
}

bool test_web_socket_destroy_invalid_argument (Test *test)
{
        TITLE ();
        net_web_socket_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static void on_connect (NetWebSocket *web_socket,
                        NetWebSocketConnection *connection)
{
        (void)web_socket;
        (void)connection;
}

static void on_read (NetWebSocket *web_socket,
                     NetWebSocketConnection *connection,
                     NetWebSocketPackage package)
{
        (void)web_socket;
        (void)connection;
        (void)package;
}

static void on_close (NetWebSocket *web_socket,
                      NetWebSocketConnection *connection)
{
        (void)web_socket;
        (void)connection;
}

static void on_error (NetWebSocket *web_socket)
{
        (void)web_socket;
}
