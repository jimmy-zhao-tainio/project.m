#include <lib.net.stream/stream.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-stream.h"

static void on_read  (NetStream *stream, NetStreamConnection *connection, NetStreamPackage package);
static void on_close (NetStream *stream, NetStreamConnection *connection);
static void on_error (NetStream *stream);

bool test_stream_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (net_stream_create (NULL, &on_close, &on_error));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_stream_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (net_stream_create (&on_read, NULL, &on_error));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_stream_create_invalid_argument_3 (Test *test)
{
        TITLE ();
        CATCH (net_stream_create (&on_read, &on_close, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_stream_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetStream) - 
                             1);
        CATCH (net_stream_create (&on_read, &on_close, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_stream_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetStream) +
                                   sizeof (size_t) + sizeof (NetStreamEpoll) +
                                   sizeof (size_t) + sizeof (Thread) - 
                                   1);
        CATCH (net_stream_create (&on_read, &on_close, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_stream_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_total_create_limit (sizeof (size_t) + sizeof (NetStream) +
                                   sizeof (size_t) + sizeof (NetStreamEpoll) +
                                   sizeof (size_t) + sizeof (Thread) +
                                   sizeof (size_t) + sizeof (NetStreamEpoll) +
                                   sizeof (size_t) + sizeof (Thread) -
                                   1);
        CATCH (net_stream_create (&on_read, &on_close, &on_error));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_stream_create (Test *test)
{
        NetStream *stream;

        TITLE ();
        CATCH (!(stream = net_stream_create (&on_read, &on_close, &on_error)));
        CATCH (error_count () != 0);
        net_stream_destroy (stream);
        PASS ();
}

static void on_read (NetStream *stream, NetStreamConnection *connection, NetStreamPackage package)
{
        (void)stream;
        (void)connection;
        (void)package;
}

static void on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void on_error (NetStream *stream)
{
        (void)stream;
}
