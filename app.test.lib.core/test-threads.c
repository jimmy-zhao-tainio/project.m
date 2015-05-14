#include <lib.core/threads.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-threads.h"

static void worker (Thread *thread);

bool test_thread_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_create (NULL, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (thread_create (&worker, NULL));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_thread_create_1000 (Test *test)
{
        Thread *threads[1000];
        int i;

        TITLE ();
        for (i = 0; i < 1000; i++) {
                CATCH (!(threads[i] = thread_create (&worker, NULL)));
        }
        for (i = 0; i < 1000; i++) {
                thread_destroy (threads[i]);
                CATCH (error_count () != 0);
        }
        PASS ();
}

bool test_thread_destroy_invalid_argument (Test *test)
{
        TITLE ();
        thread_destroy (NULL);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_exit_invalid_argument (Test *test)
{
        TITLE ();
        thread_exit (NULL);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_wait_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_wait (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_set_exit_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_set_exit (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_get_exit_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_get_exit (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_signal_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_signal_create (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_signal_destroy_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_signal_destroy (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_signal_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_signal (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_signal_wait_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_signal_wait (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_lock_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_lock_create (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_lock_destroy_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_lock_destroy (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_lock_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_lock (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_thread_unlock_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (thread_unlock (NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

static void worker (Thread *thread)
{
        (void)thread;
}
