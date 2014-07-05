#include <stdio.h>
#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/convert.h>
#include <lib.core/string.h>

static bool try (char **argv);
static void usage (const char *name);

int main (int argc, char **argv)
{
        if (argc != 4) {
                usage (argv[0]);
		return EXIT_FAILURE;
	}
        if (!random_open ()) {
                error_code (FunctionCall, 1);
                error_print ();
                return EXIT_FAILURE;
        }
        if (!try (argv)) {
                error_code (FunctionCall, 2);
                error_print ();
                random_close ();
                return EXIT_FAILURE;
        }
        random_close ();
	return EXIT_SUCCESS;
}

static bool try (char **argv)
{
        unsigned long long from, to, count, value;
        size_t digits;

        if (!convert_string_to_unsigned_long_long (argv[1], &from, &digits) ||
            string_length (argv[1]) != digits) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!convert_string_to_unsigned_long_long (argv[2], &to, &digits) ||
            string_length (argv[2]) != digits) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (!convert_string_to_unsigned_long_long (argv[3], &count, &digits) ||
            string_length (argv[3]) != digits) {
                error_code (InvalidArgument, 3);
                return false;
        }
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

static void usage (const char *name)
{
	printf ("%s from to count\n", name);
}
