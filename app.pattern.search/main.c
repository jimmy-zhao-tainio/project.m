#include <stdio.h>
#include <lib.core/defines.h>
#include <lib.core/error.h>
#include <lib.core/file-reader.h>
#include <lib.core/pattern-search.h>

static bool try (FileReader **reader, PatternSearch **search, const char *pattern, const char *file);
static bool print_range (const char *buffer, size_t from, size_t to);
static void usage (const char *name);

int main (int argc, char **argv)
{
	FileReader *reader = NULL;
	PatternSearch *search = NULL;
        bool success;

        if (argc != 3) {
                usage (argv[0]);
		return EXIT_FAILURE;
	}
        success = try (&reader, &search, argv[1], argv[2]);
        if (reader) {
                file_reader_destroy (reader);
        }
        if (search) {
                pattern_search_destroy (search);
        }
        if (!success || error_count () != 0) {
                error_print ();
                return EXIT_FAILURE;
        }
	return EXIT_SUCCESS;
}

static bool try (FileReader **reader, PatternSearch **search, const char *pattern, const char *file)
{
        size_t from, to;

        if (!(*reader = file_reader_create (file))) {
		error_code (FunctionCall, 1);
		return false;
	}
	if (!(*search = pattern_search_create ((*reader)->map, (size_t)(*reader)->size, pattern, false))) {
                error_code (FunctionCall, 2);
		return false;
	}
	while (pattern_search_next (*search, &from, &to)) {
                if (!print_range ((const char *)(*reader)->map, from, to)) {
                        error_code (FunctionCall, 3);
                        return false;
                }
                if (putc ((int)'\n', stdout) == EOF) {
                        error (SystemCall);
                        return false;
                }
	}
	return true;
}

static bool print_range (const char *buffer, size_t from, size_t to)
{
        if (to - from == 0) {
                return true;
        }
        if (fwrite (buffer + from, 1, to - from, stdout) != to - from) {
                error (SystemCall);
                return false;
        }
        return true;
}

static void usage (const char *name)
{
	printf ("%s pattern file\n", name);
}
