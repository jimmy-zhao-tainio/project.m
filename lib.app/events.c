#include <lib.app/events.h>
#include <lib.core/threads.h>
#include <lib.core/error.h>

typedef struct
{
        bool exit;
        ThreadLock lock;
        ThreadSignal signal;
} Global;

static Global global = { 
        .exit = false,
        .lock = THREAD_LOCK_INITIALIZER,
        .signal = THREAD_SIGNAL_INITIALIZER
};

static bool lock_exit_get (bool *exit);
static bool lock_exit_set (bool exit);

bool app_events (void)
{
        bool exit;

        if (!lock_exit_get (&exit)) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (exit == true) {
                return true;
        }
        do {
                if (!thread_signal_wait (&global.signal)) {
                        error (FunctionCall);
                        return false;
                }
                if (!lock_exit_get (&exit)) {
                        error_code (FunctionCall, 1);
                        return false;
                }
                if (exit == true) {
                        return true;
                }
        } while (true);
}

void app_event_exit (void)
{
        if (!lock_exit_set (true)) {
                error (FunctionCall);
                return;
        }
        if (!thread_signal (&global.signal)) {
                error (FunctionCall);
        }
}

static bool lock_exit_get (bool *exit)
{
        if (!thread_lock (&global.lock)) {
                return false;
        }
        *exit = global.exit;
        if (!thread_unlock (&global.lock)) {
                return false;
        }
        return true;
}

static bool lock_exit_set (bool exit)
{
        if (!thread_lock (&global.lock)) {
                return false;
        }
        global.exit = exit;
        if (!thread_unlock (&global.lock)) {
                return false;
        }
        return true;
}
