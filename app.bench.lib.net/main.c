#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.app/arguments.h>
#include <lib.app/events.h>

#include "server.h"
#include "client.h"

static bool try (size_t connections, size_t package_size, size_t duration);

int main (int argc, char **argv)
{
        uint64_t connections;
        uint64_t package_size;
        uint64_t seconds;

        AppArgument arguments[] = {
                ARGUMENT_NAMED_UINT64 ("-c", 
                                       "--connections",
                                       1000,
                                       true,
                                       &connections,
                                       "Number of concurrent connections"),
                ARGUMENT_NAMED_UINT64 ("-p",
                                       "--package-size",
                                       1024,
                                       true,
                                       &package_size,
                                       "Package size"),
                ARGUMENT_NAMED_UINT64 ("-s",
                                       "--seconds",
                                       10,
                                       true,
                                       &seconds,
                                       "Duration of the test in seconds"),
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
        if (!try ((size_t)connections, 
                  (size_t)package_size,
                  (size_t)seconds)) {
                error_code (FunctionCall, 2);
                random_close ();
                return EXIT_FAILURE;
        }
        random_close ();
	return EXIT_SUCCESS;
}

static bool try (size_t connections, size_t package_size, size_t seconds)
{
        server_start (connections, package_size, seconds);
        client_start (connections, package_size, seconds);
	return app_events ();
}
