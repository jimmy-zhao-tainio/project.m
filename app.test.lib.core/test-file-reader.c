#include <lib.core/file-reader.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-file-reader.h"

bool test_file_reader_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (file_reader_create ("stage/file_reader/file"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_reader_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (file_reader_create ("/stage/file_reader/file"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_file_reader_create_system_call_1 (Test *test)
{
        TITLE ();
        CATCH (file_reader_create ("/stage/error_stat"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorSystemCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_reader_create_system_call_2 (Test *test)
{
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/error_open"));
        CATCH (file_reader_create (path));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorSystemCall);
        CATCH (error_at (0).code != 2);
        string_destroy (path);
        PASS ();
}

bool test_file_reader_create_system_call_3 (Test *test)
{
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/error_mmap"));
        CATCH (file_reader_create (path));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorSystemCall);
        CATCH (error_at (0).code != 3);
        string_destroy (path);
        PASS ();
}

bool test_file_reader_create (Test *test)
{
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_reader/file"));
        CATCH (!(reader = file_reader_create (path)));
        CATCH (reader->size != 5);
        CATCH (!string_equals ((const char *)reader->map, "test\n"));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}

bool test_file_reader_destroy_invalid_argument (Test *test)
{
        TITLE ();
        file_reader_destroy (NULL);
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}
