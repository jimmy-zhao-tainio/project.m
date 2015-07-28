#include <lib.net.server/server.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-server.h"

static void on_connect (NetServer *server, int socket);
static void on_error   (NetServer *server);

bool test_server_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (NULL, 0, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_server_create ("127.0.0.1", 0, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_server_create ("127.0.0.1", 8888, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_server_create_invalid_argument_4 (Test *test)
{
        TITLE ();
        CATCH (net_server_create ("127.0.0.1", 8888, &on_connect, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_server_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetServer) - 1);
        CATCH (net_server_create ("127.0.0.1", 8888, &on_connect, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetServer) +
                                   sizeof (size_t) + sizeof (NetServerEpoll) - 1);
        CATCH (net_server_create ("127.0.0.1", 8888, &on_connect, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create_function_call_4 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetServer) +
                                   sizeof (size_t) + sizeof (NetServerEpoll) + 
                                   sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_server_create ("127.0.0.1", 8888, &on_connect, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_server_create (Test *test)
{
        NetServer *server;

        TITLE ();
        CATCH (!(server = net_server_create ("127.0.0.1", 8888, &on_connect, &on_error)));
        CATCH (error_count () != 0);
        net_server_destroy (server);
        PASS ();
}

bool test_server_destroy_invalid_argument (Test *test)
{
        TITLE ();
        net_server_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static void on_connect (NetServer *server, int socket)
{
        (void)server;
        (void)socket;
}

static void on_error (NetServer *server)
{
        (void)server;
}
