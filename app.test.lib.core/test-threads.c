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

static void wait_twice (Thread *thread);
static ThreadSignal wait_1;
static ThreadSignal wait_2;

bool test_thread_signal_wait_twice (Test *test)
{
        Thread *thread;

        TITLE ();
        CATCH (!thread_signal_create (&wait_1));
        CATCH (!thread_signal_create (&wait_2));
        CATCH (!(thread = thread_create (&wait_twice, NULL)));
        CATCH (!thread_signal (&wait_2));
        CATCH (!thread_signal_wait (&wait_1));
        CATCH (!thread_signal (&wait_2));
        CATCH (!thread_signal_wait (&wait_1));
        thread_signal_destroy (&wait_1);
        thread_signal_destroy (&wait_2);
        thread_wait (thread);
        thread_destroy (thread);
        CATCH (error_count () != 0);
        PASS ();
}

static void wait_twice (Thread *thread)
{
        (void)thread;
        if (!thread_signal_wait (&wait_2)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (!thread_signal (&wait_1)) {
                error_code (FunctionCall, 2);
                return;
        }
        if (!thread_signal_wait (&wait_2)) {
                error_code (FunctionCall, 3);
                return;
        }
        if (!thread_signal (&wait_1)) {
                error_code (FunctionCall, 4);
                return;
        }
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

static bool recursive (ThreadLock *lock, int i);

bool test_thread_lock_recursive (Test *test)
{
        ThreadLock lock;

        TITLE ();
        CATCH (!thread_lock_create (&lock));
        CATCH (!recursive (&lock, 1000));
        CATCH (!thread_lock_destroy (&lock));
        PASS ();
}

static bool recursive (ThreadLock *lock, int i)
{
        if (i == 0) {
                return true;
        }
        if (!thread_lock (lock)) {
                return false;
        }
        if (!recursive (lock, i - 1)) {
                return false;
        }
        if (!thread_unlock (lock)) {
                return false;
        }
        return true;
}

static void worker (Thread *thread)
{
        (void)thread;
}
