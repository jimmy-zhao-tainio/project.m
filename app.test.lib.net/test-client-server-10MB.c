#include <lib.net/net.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <lib.core/threads.h>
#include <unistd.h>
#include <stdio.h>

#include "test-client-server-10MB.h"

static ThreadSignal server_ready_signal = THREAD_SIGNAL_INITIALIZER;
static void server_on_connect (Net *server, NetConnection connection);
static void server_on_close (Net *server, NetConnection connection);
static void server_on_read (Net *server, NetConnection connection, NetPackage package);
static void server_on_stop (Net *server);
static ThreadSignal client_ready_signal = THREAD_SIGNAL_INITIALIZER;
static void client_on_connect (Net *client, NetConnection connection);
static void client_on_close (Net *client, NetConnection connection);
static void client_on_read (Net *client, NetConnection connection, NetPackage package);
static void client_on_stop (Net *client);
static void connection_worker (Thread *thread);
static Thread *connection_thread = NULL;

bool test_client_server_10MB (Test *test)
{
        NetAddress localhost = { "127.0.0.1", 8888 };
        NetServer *server;
        NetClient *client;

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
        thread_signal_wait (&server_ready_signal);
        thread_signal_wait (&client_ready_signal);
        net_destroy ((Net *)server);
        net_destroy ((Net *)client);
        if (connection_thread) {
                thread_destroy (connection_thread);
        }
        PASS ();
}

static void server_on_connect (Net *server, NetConnection connection)
{
        (void)server;
        (void)connection;
}

static void server_on_close (Net *server, NetConnection connection)
{
        (void)server;
        (void)connection;
}

size_t buffer_read = 0;
size_t buffer_batch = 0;
size_t buffer_size = 10000000;
char buffer_char = 0;

static void server_on_read (Net *server, NetConnection connection, NetPackage package)
{
        size_t i;

        (void)server;
        (void)connection;
        buffer_batch++;
        for (i = 0; i < package.length; i++) {
                if (package.buffer[i] != buffer_char) {
                        printf ("buffer[%i] == %i != %i\n", 
                                (int)i, 
                                (int)package.buffer[i], 
                                (int)buffer_char); 
                        fflush (stdout);
                }
                buffer_char++;
        }
        buffer_read += package.length;
        if (buffer_read == buffer_size) {
                thread_signal (&server_ready_signal);
        }
        else if (buffer_read > buffer_size) {
                printf ("buffer_read = %i\n",  (int)buffer_read); 
                fflush (stdout);
        }
}

static void server_on_stop (Net *server)
{
        (void)server;
}

static NetConnection connection_argument;

static void client_on_connect (Net *client, NetConnection connection)
{
        connection.pointer = client;
        connection_argument = connection;
        connection_thread = thread_create (&connection_worker, NULL);
}

static void client_on_close (Net *client, NetConnection connection)
{
        (void)client;
        (void)connection;
}

static void connection_worker (Thread *thread)
{
        NetConnection connection = connection_argument;
        Net *net = connection.pointer;
        NetPackage package;
        size_t i;

        (void)thread;
        package.buffer = memory_create (buffer_size);
        package.length = buffer_size;
        for (i = 0; i < buffer_size; i++) {
                package.buffer[i] = buffer_char;
                buffer_char++;
        }
        buffer_char = 0;
        net_write (net, connection, package);
        memory_destroy (package.buffer);
        thread_signal (&client_ready_signal);
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
