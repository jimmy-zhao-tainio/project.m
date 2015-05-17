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

        do {
                if (!thread_signal_wait (&global.signal)) {
                        error_code (FunctionCall, 1);
                        return false;
                }
                if (!lock_exit_get (&exit)) {
                        error_code (FunctionCall, 2);
                        return false;
                }
                if (exit) {
                        return true;
                }
        } while (true);
}

void app_event_exit (void)
{
        if (!lock_exit_set (true)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (!thread_signal (&global.signal)) {
                error_code (FunctionCall, 2);
        }
}

void app_events_reset (void)
{
        global.exit = false;
        global.signal.set = false;
}

static bool lock_exit_get (bool *exit)
{
        if (!thread_lock (&global.lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        *exit = global.exit;
        if (!thread_unlock (&global.lock)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}

static bool lock_exit_set (bool exit)
{
        if (!thread_lock (&global.lock)) {
                error_code (FunctionCall, 1);
                return false;
        }
        global.exit = exit;
        if (!thread_unlock (&global.lock)) {
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}
