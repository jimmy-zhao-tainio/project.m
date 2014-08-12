#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>
#include <lib.app/arguments.h>

static bool try (uint64_t from, uint64_t to, uint64_t count);

int main (int argc, char **argv)
{
        uint64_t from, to, count;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_UINT64 (0, true, &from, "Minimum value."),
                ARGUMENT_ORDINAL_UINT64 (0, true, &to, "Maximum value."),
                ARGUMENT_ORDINAL_UINT64 (0, true, &count, "Number of random numbers to generate."),
                ARGUMENT_SHARED,
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
        if (!try (from, to, count)) {
                error_code (FunctionCall, 2);
                random_close ();
                return EXIT_FAILURE;
        }
        random_close ();
	return EXIT_SUCCESS;
}

static bool try (uint64_t from, uint64_t to, uint64_t count)
{
        unsigned long long value;

        if (from > to) {
                error (InvalidOperation);
                return false;
        }
        while (count-- > 0) {
                if (!random_value (from, to, &value)) {
                        error_code (FunctionCall, 2);
                        return false;
                }
                if (printf ("%llu\n", value) <= 0) {
                        error (SystemCall);
                        return false;
                }
        }
	return true;
}
