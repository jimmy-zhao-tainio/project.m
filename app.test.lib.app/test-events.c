#include <lib.app/events.h>

#include "test-events.h"

bool test_app_events_1 (Test *test)
{
        TITLE ();
        app_event_exit ();
        CATCH (!app_events ());
        PASS ();
}

bool test_app_events_2 (Test *test)
{
        TITLE ();
        app_event_exit ();
        app_event_exit ();
        CATCH (!app_events ());
        PASS ();
}
