#include <lib.net.client/client.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-client.h"

static void on_connect       (NetClient *client, NetClientConnection *connection);
static void on_connect_error (NetClient *client, NetClientConnection *connection);
static void on_error         (NetClient *client);

bool test_client_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_client_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (&on_connect, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_client_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (&on_connect, &on_connect_error, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_client_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetClient) - 1);
        CATCH (net_client_create (&on_connect, &on_connect_error, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_client_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetClient) +
                                   sizeof (size_t) + sizeof (NetClientEpoll) - 1);
        CATCH (net_client_create (&on_connect, &on_connect_error, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_client_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetClient) +
                                   sizeof (size_t) + sizeof (NetClientEpoll) + 
                                   sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_client_create (&on_connect, &on_connect_error, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_client_create (Test *test)
{
        NetClient *client;

        TITLE ();
        CATCH (!(client = net_client_create (&on_connect, &on_connect_error, &on_error)));
        CATCH (error_count () != 0);
        net_client_destroy (client);
        PASS ();
}

bool test_client_destroy_invalid_argument (Test *test)
{
        TITLE ();
        net_client_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static void on_connect (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        (void)connection;
}

static void on_connect_error (NetClient *client, NetClientConnection *connection)
{
        (void)client;
        (void)connection;
}

static void on_error (NetClient *client)
{
        (void)client;
}
