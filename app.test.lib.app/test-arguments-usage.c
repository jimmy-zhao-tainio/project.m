#include "test-arguments-usage.h"

#include <lib.app/arguments.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <lib.core/file-reader.h>
#include <lib.core/print.h>
#include <lib.core/file.h>

static bool log_begin (void);
static bool log_end (void);

bool test_app_arguments_usage_invalid_argument_1 (Test *test)
{
        char *argv[] = { "./app", "test" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-t", "-t", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        app_arguments_usage (-1, argv, arguments);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_app_arguments_usage_invalid_argument_2 (Test *test)
{
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-t", "-t", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        app_arguments_usage (2, NULL, arguments);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_app_arguments_usage_invalid_argument_3 (Test *test)
{
        char *argv[] = { "./app", "test" };

        TITLE ();
        app_arguments_usage (2, argv, NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_app_arguments_usage_function_call (Test *test)
{
        char *argv[] = { "./app", "test" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-t", "-t", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        app_arguments_usage (2, argv, arguments);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        PASS ();
}

bool test_app_arguments_usage (Test *test)
{
        char *argv[] = { 
                "./app", 
        };
        int ordinal_integer;
        uint64_t ordinal_uint64;
        char *ordinal_string;
        bool named_boolean;
        int named_integer;
        uint64_t named_uint64;
        char *named_string;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (                   123,   true,  &ordinal_integer, "1"),
                ARGUMENT_ORDINAL_UINT64  (                   123,   true,  &ordinal_uint64,  "2"),
                ARGUMENT_ORDINAL_STRING  (                   "abc", true,  &ordinal_string,  "3"),
                ARGUMENT_ORDINAL_INTEGER (                   123,   false, &ordinal_integer, "4"),
                ARGUMENT_ORDINAL_UINT64  (                   123,   false, &ordinal_uint64,  "5"),
                ARGUMENT_ORDINAL_STRING  (                   "abc", false, &ordinal_string,  "6"),
                ARGUMENT_NAMED_BOOLEAN   ("-b", "--boolean", false, true,  &named_boolean,   "7"),
                ARGUMENT_NAMED_INTEGER   ("-i", "--integer", 1,     true,  &named_integer,   "8"),
                ARGUMENT_NAMED_UINT64    ("-u", "--uint64",  1,     true,  &named_uint64,    "9"),
                ARGUMENT_NAMED_STRING    ("-s", "--string",  "abc", true,  &named_string,    "10"),
                ARGUMENT_NAMED_BOOLEAN   ("-B", "--Boolean", false, false, &named_boolean,   "7"),
                ARGUMENT_NAMED_INTEGER   ("-I", "--Integer", 1,     false, &named_integer,   "8"),
                ARGUMENT_NAMED_UINT64    ("-U", "--Uint64",  1,     false, &named_uint64,    "9"),
                ARGUMENT_NAMED_STRING    ("-S", "--String",  "abc", false, &named_string,    "10"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (!log_begin ());
        app_arguments_usage (1, argv, arguments);
        CATCH (error_count () != 0);
        CATCH (!log_end ());
        PASS ();
}

static bool log_begin (void)
{
        char *path;

        if (!(path = directory_current_path ())) {
                return false;
        }
        if (!string_append (&path, "/stage/log")) {
                string_destroy (path);
                return false;
        }
        if (file_exists (path)) {
                if (!file_remove (path)) {
                        string_destroy (path);
                        return false;
                }
        }
        if (!print_log_begin (path)) {
                string_destroy (path);
                return false;
        }
        string_destroy (path);
        return true;
}

static bool log_end ()
{
        FileReader *reader;
        FileReader *reference;
        char *path;

        (void)print_log_end ();
        if (!(path = directory_current_path ())) {
                return false;
        }
        if (!string_append (&path, "/stage/log")) {
                return false;
        }
        if (!(reader = file_reader_create (path))) {
                return false;
        }
        if (!string_append (&path, "_reference")) {
                return false;
        }
        if (!(reference = file_reader_create (path))) {
                return false;
        }
        if (!string_equals ((const char *)reader->map, (const char *)reference->map)) {
                return false;
        }
        string_destroy (path);
        file_reader_destroy (reader);
        file_reader_destroy (reference);
        return true;
}
