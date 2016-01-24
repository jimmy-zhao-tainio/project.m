#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <unistd.h>
#include <stdio.h>

#include "test-poll.h"

bool test_poll_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetPoll) - 1);
        CATCH (net_poll_create (NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_poll_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetPoll) +
                             sizeof (size_t) + sizeof (NetPollEvents) - 1);
        CATCH (net_poll_create (NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_poll_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (NetPoll) +
                             sizeof (size_t) + sizeof (NetPollEvents) +
                             sizeof (size_t) + (sizeof (NetPollEvent) * 1024) +
                             sizeof (size_t) + sizeof (Thread) - 1);
        CATCH (net_poll_create (NULL, NULL, NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_poll_create (Test *test)
{
        NetPoll *poll;

        TITLE ();
        CATCH (!(poll = net_poll_create (NULL, NULL, NULL)));
        net_poll_destroy (poll);
        CATCH (error_count () != 0);
        PASS ();
}
