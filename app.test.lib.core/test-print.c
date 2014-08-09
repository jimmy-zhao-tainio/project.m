#include <lib.core/print.h>
#include <lib.core/error.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.core/file-reader.h>

#include "test-print.h"

bool test_print (Test *test)
{
        TITLE ();
        CATCH (!print (" \b"));
        CATCH (!print ("%i\b", 1));
        PASS ();
}

bool test_print_silent (Test *test)
{
        TITLE ();
        print_silent (true);
        CATCH (!print ("\nThis should not be visible!\n"));
        PASS ();
}

bool test_print_log_begin_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (print_log_begin (NULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_print_log_begin_invalid_operation (Test *test)
{
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/print_log/log"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!print_log_begin (path));
        CATCH (print_log_begin (path));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (!print_log_end ());
        string_destroy (path);
        PASS ();
}

bool test_print_log_begin_function_call (Test *test)
{
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/print_log/path/does/not/exist/log"));
        CATCH (file_exists (path));
        CATCH (print_log_begin (path));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        string_destroy (path);
        PASS ();
}

bool test_print_log_end_invalid_operation (Test *test)
{
        TITLE ();
        CATCH (print_log_end ());
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidOperation);
        PASS ();
}

bool test_print_log_1 (Test *test)
{
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/print_log/log"));
        print_silent (true);
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!print_log_begin (path));
        CATCH (!print ("test %i %i %i...", 1, 2, 3));
        CATCH (!print_log_end ());
        CATCH (!(reader = file_reader_create (path)));
        print_silent (false);
        CATCH (!string_equals ((const char *)reader->map, "test 1 2 3..."));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}

bool test_print_log_2 (Test *test)
{
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/print_log/log"));
        print_silent (false);
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!print_log_begin (path));
        CATCH (!print (" \b"));
        CATCH (!print_log_end ());
        CATCH (!(reader = file_reader_create (path)));
        print_silent (true);
        CATCH (!string_equals ((const char *)reader->map, " \b"));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}
