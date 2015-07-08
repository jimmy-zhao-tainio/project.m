#include <lib.net/net.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-client.h"

static void on_connect (Net *client, NetConnection connection);
static void on_close (Net *client, NetConnection connection);
static void on_read (Net *client, NetConnection connection, NetPackage package);
static void on_stop (Net *client);

bool test_client_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (NULL, 
                                  NULL, 
                                  NULL, 
                                  NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_client_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (&on_connect, 
                                  NULL, 
                                  NULL, 
                                  NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_client_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (&on_connect, 
                                  &on_close, 
                                  NULL, 
                                  NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_client_create_invalid_argument_4 (Test *test)
{
        TITLE ();
        CATCH (net_client_create (&on_connect, 
                                  &on_close, 
                                  &on_read, 
                                  NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_client_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (net_client_create (&on_connect, 
                                  &on_close, 
                                  &on_read, 
                                  &on_stop));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_client_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (800);
        CATCH (net_client_create (&on_connect, 
                                  &on_close, 
                                  &on_read, 
                                  &on_stop));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_client_create (Test *test)
{
        NetClient *client;

        TITLE ();
        CATCH (!(client = net_client_create (&on_connect, 
                                             &on_close, 
                                             &on_read, 
                                             &on_stop)));
        CATCH (error_count () != 0);
        net_destroy ((Net *)client);
        PASS ();
}

bool test_client_stop_invalid_argument (Test *test)
{
        TITLE ();
        net_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_client_connect_invalid_argument_1 (Test *test)
{
        NetAddress address = { NULL, 0 };

        TITLE ();
        CATCH (net_connect (NULL, address));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_client_connect_invalid_argument_2 (Test *test)
{
        NetAddress address = { NULL, 0 };
        NetClient client;

        TITLE ();
        CATCH (net_connect ((Net *)&client, address));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_client_connect_system_call_1 (Test *test)
{
        NetAddress address = { "999.999.999.999", 0 };
        NetClient client;

        TITLE ();
        CATCH (net_connect ((Net *)&client, address));
        CATCH (error_at (0).error != ErrorSystemCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

static void on_connect (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
}

static void on_close (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
}

static void on_read (Net *client, NetConnection connection, NetPackage package)
{
        (void)client;
        (void)connection;
        (void)package;
}

static void on_stop (Net *client)
{
        (void)client;
}

static ThreadSignal server_on_connect_signal = THREAD_SIGNAL_INITIALIZER;
static void server_on_connect (Net *server, NetConnection connection);
static void server_on_close (Net *server, NetConnection connection);
static void server_on_read (Net *server, NetConnection connection, NetPackage package);
static void server_on_stop (Net *server);
static ThreadSignal client_on_connect_signal = THREAD_SIGNAL_INITIALIZER;
static void client_on_connect (Net *client, NetConnection connection);
static void client_on_close (Net *client, NetConnection connection);
static void client_on_read (Net *client, NetConnection connection, NetPackage package);
static void client_on_stop (Net *client);

bool test_client_on_connect (Test *test)
{
        NetServer *server;
        NetClient *client;
        NetAddress localhost = { "127.0.0.1", 8888 };

        TITLE ();
        CATCH (!(server = net_server_create (localhost,
                                         &server_on_connect, 
                                         &server_on_close, 
                                         &server_on_read, 
                                         &server_on_stop)));
        CATCH (!(client = net_client_create (&client_on_connect, 
                                             &client_on_close, 
                                             &client_on_read, 
                                             &client_on_stop)));
        CATCH (!net_connect ((Net *)client, localhost));
        thread_signal_wait (&server_on_connect_signal);
        thread_signal_wait (&client_on_connect_signal);
        net_destroy ((Net *)server);
        net_destroy ((Net *)client);
        PASS ();
}

static void server_on_connect (Net *server, NetConnection connection)
{
        (void)server;
        (void)connection;
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

static void server_on_stop (Net *server)
{
        (void)server;
}

static void client_on_connect (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
        thread_signal (&client_on_connect_signal);
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
