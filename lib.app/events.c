#include <lib.app/events.h>
#include <lib.core/threads.h>
#include <lib.core/error.h>

typedef struct
{
                bool exit;
                ThreadSignal signal;
} Global;

static Global global = { 
        .exit = false,
        .signal = THREAD_SIGNAL_INITIALIZER
};

bool app_events (void)
{
        do {
                if (!thread_signal_wait (&global.signal)) {
                        error (FunctionCall);
                        return false;
                }
                if (global.exit == true) {
                        return true;
                }
        } while (true);
}

void app_event_exit (void)
{
        global.exit = true;
        if (!thread_signal (&global.signal)) {
                error (FunctionCall);
        }
}
