#include <lib.net/server.h>
#include <lib.net/client-simulator.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>

#include "test-server.h"

static Client *on_open (Server *server, int socket);
static void on_close (Server *server, Client *client);
static void on_read (Server *server, Client *client, char *buffer, size_t length);
static void on_stop (Server *server);
static Client *on_open_2 (Server *server, int socket);

bool test_server_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (server_create (0, NULL, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (server_create (8888, NULL, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (server_create (8888, &on_open, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_server_create_invalid_argument_4 (Test *test)
{
        TITLE ();
        CATCH (server_create (8888, &on_open, &on_close, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_server_create_invalid_argument_5 (Test *test)
{
        TITLE ();
        CATCH (server_create (8888, &on_open, &on_close, &on_read, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 5);
        PASS ();
}

bool test_server_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (server_create (8888, &on_open, &on_close, &on_read, &on_stop));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (800);
        CATCH (server_create (8888, &on_open, &on_close, &on_read, &on_stop));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create (Test *test)
{
        Server *server;

        TITLE ();
        CATCH (!(server = server_create (8888, &on_open, &on_close, &on_read, &on_stop)));
        CATCH (error_count () != 0);
        server_destroy (server);
        PASS ();
}

bool test_server_stop_invalid_argument (Test *test)
{
        TITLE ();
        server_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static ThreadSignal on_open_signal = THREAD_SIGNAL_INITIALIZER;

bool test_server_on_open (Test *test)
{
        Server *server;

        TITLE ();
        CATCH (!(server = server_create (8888, &on_open_2, &on_close, &on_read, &on_stop)));
        CATCH (!client_simulator_connect ("127.0.0.1", 8888));
        thread_signal_wait (&on_open_signal);
        server_destroy (server);
        PASS ();
}

static Client *on_open (Server *server, int socket)
{
        (void)server;
        (void)socket;
        thread_signal (&on_open_signal);
        return NULL;
}

static void on_close (Server *server, Client *client)
{
        (void)server;
        (void)client;
}

static void on_read (Server *server, Client *client, char *buffer, size_t length)
{
        (void)server;
        (void)client;
        (void)buffer;
        (void)length;
}

static void on_stop (Server *server)
{
        (void)server;
}

static Client *on_open_2 (Server *server, int socket)
{
        (void)server;
        close (socket);
        thread_signal (&on_open_signal);
        return NULL;
}
