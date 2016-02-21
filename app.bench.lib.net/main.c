#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.app/arguments.h>
#include <lib.app/events.h>

#include "server.h"
#include "client.h"

static bool try (size_t connections, size_t packages, size_t from, size_t to);

int main (int argc, char **argv)
{
        uint64_t connections;
        uint64_t packages;
        uint64_t size_from;
        uint64_t size_to;

        AppArgument arguments[] = {
                ARGUMENT_NAMED_UINT64 ("-c", 
                                       "--connections",
                                       1000,
                                       true,
                                       &connections,
                                       "Number of concurrent connections"),
                ARGUMENT_NAMED_UINT64 ("-p", 
                                       "--packages",
                                       1000,
                                       true,
                                       &packages,
                                       "Number of packages per connection"),
                ARGUMENT_ORDINAL_UINT64 (10,
                                         false,
                                         &size_from,
                                         "Smallest package size"),
                ARGUMENT_ORDINAL_UINT64 (10,
                                         false,
                                         &size_to,
                                         "Largest package size"),
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
                  (size_t)packages, 
                  (size_t)size_from, 
                  (size_t)size_to)) {
                error_code (FunctionCall, 2);
                random_close ();
                return EXIT_FAILURE;
        }
        random_close ();
	return EXIT_SUCCESS;
}

static bool try (size_t connections, size_t packages, size_t from, size_t to)
{
        server_start ();
        client_start (connections, packages, from, to);
	return app_events ();
}
