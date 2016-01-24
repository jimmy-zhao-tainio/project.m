#include <lib.core/threads.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <unistd.h>
#include <stdio.h>

#include "test-threads.h"

bool test_thread_signal_wait_forever (Test *test)
{
        ThreadSignal wait;

        TITLE ();
        CATCH (!thread_signal_create (&wait));
        thread_signal_wait (&wait);
        CATCH (true);
}
