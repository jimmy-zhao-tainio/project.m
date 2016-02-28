#include <lib.net.stream/stream.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <unistd.h>
#include <stdio.h>

#include "test-stream.h"

bool test_stream_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetStream) - 1);
        CATCH (net_stream_create (NULL, NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_stream_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetStream) +
                             sizeof (size_t) + sizeof (NetPoll) - 1);
        CATCH (net_stream_create (NULL, NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_stream_create (Test *test)
{
        NetStream *stream;

        TITLE ();
        CATCH (!(stream = net_stream_create (NULL, NULL, NULL, NULL)));
        net_stream_destroy (stream);
        CATCH (error_count () != 0);
        PASS ();
}
