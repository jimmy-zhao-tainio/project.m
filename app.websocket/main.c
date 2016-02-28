#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.app/arguments.h>
#include <lib.app/events.h>

#include "server.h"

static bool try (void);

int main (int argc, char **argv)
{
        AppArgument arguments[] = {
                ARGUMENT_DEFAULT,
                ARGUMENT_END
        };

        if (!app_arguments (argc, argv, arguments)) {
                app_arguments_usage (argc, argv, arguments);
                return EXIT_FAILURE;
        }
        if (!random_open ()) {
                error_code (FunctionCall, 1);
                return EXIT_FAILURE;
        }
        if (!try ()) {
                error_code (FunctionCall, 2);
                random_close ();
                return EXIT_FAILURE;
        }
        random_close ();
	return EXIT_SUCCESS;
}

static bool try (void)
{
        server_start ();
	return app_events ();
}
