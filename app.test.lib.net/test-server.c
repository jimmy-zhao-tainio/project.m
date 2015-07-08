#include <lib.net/net.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-server.h"

static void on_connect (Net *net, NetConnection connection);
static void on_close (Net *net, NetConnection connection);
static void on_read (Net *net, NetConnection connection, NetPackage package);
static void on_error (Net *net);
static NetAddress localhost = { "127.0.0.1", 8888 };

bool test_server_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (localhost, NULL, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (localhost, NULL, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (localhost, &on_connect, NULL, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_server_create_invalid_argument_4 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (localhost, &on_connect, &on_close, NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_server_create_invalid_argument_5 (Test *test)
{
        TITLE ();
        CATCH (net_server_create (localhost, &on_connect, &on_close, &on_read, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 5);
        PASS ();
}

bool test_server_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (net_server_create (localhost, &on_connect, &on_close, &on_read, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_server_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (800);
        CATCH (net_server_create (localhost, 
                              &on_connect, 
                              &on_close, 
                              &on_read, 
                              &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_server_create (Test *test)
{
        NetServer *server;

        TITLE ();
        CATCH (!(server = net_server_create (localhost, 
                                             &on_connect, 
                                             &on_close, 
                                             &on_read, 
                                             &on_error)));
        CATCH (error_count () != 0);
        net_destroy ((Net *)server);
        PASS ();
}

bool test_server_stop_invalid_argument (Test *test)
{
        TITLE ();
        net_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static void on_connect (Net *net, NetConnection connection)
{
        (void)net;
        (void)connection;
}

static void on_close (Net *net, NetConnection connection)
{
        (void)net;
        (void)connection;
}

static void on_read (Net *net, NetConnection connection, NetPackage package)
{
        (void)net;
        (void)connection;
        (void)package;
}

static void on_error (Net *net)
{
        (void)net;
}

static ThreadSignal server_on_connect_signal = THREAD_SIGNAL_INITIALIZER;
static void server_on_connect (Net *server, NetConnection connection);
static void server_on_close (Net *server, NetConnection connection);
static void server_on_read (Net *server, NetConnection connection, NetPackage package);
static void server_on_error (Net *server);
static void client_on_connect (Net *client, NetConnection connection);
static void client_on_close (Net *client, NetConnection connection);
static void client_on_read (Net *client, NetConnection connection, NetPackage package);
static void client_on_stop (Net *client);

bool test_server_on_connect (Test *test)
{
        NetServer *server;
        NetClient *client;

        TITLE ();
        CATCH (!(server = net_server_create (localhost, 
                                             &server_on_connect, 
                                             &server_on_close, 
                                             &server_on_read, 
                                             &server_on_error)));
        CATCH (!(client = net_client_create (&client_on_connect, 
                                             &client_on_close, 
                                             &client_on_read, 
                                             &client_on_stop)));
        CATCH (!net_connect ((Net *)client, localhost));
        thread_signal_wait (&server_on_connect_signal);
        net_destroy ((Net *)server);
        net_destroy ((Net *)client);
        PASS ();
}

static void server_on_connect (Net *server, NetConnection connection)
{
        (void)server;
        (void)connection;
        net_close ((Net *)server, connection);
        thread_signal (&server_on_connect_signal);
}

static void server_on_close (Net *server, NetConnection connection)
{
        (void)server;
        (void)connection;
}

static void server_on_read (Net *server, NetConnection connection, NetPackage package)
{
        (void)server;
        (void)connection;
        (void)package;
}

static void server_on_error (Net *server)
{
        (void)server;
}

static void client_on_connect (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
}

static void client_on_close (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
}

static void client_on_read (Net *client, NetConnection connection, NetPackage package)
{
        (void)client;
        (void)connection;
        (void)package;
}

static void client_on_stop (Net *client)
{
        (void)client;
}
