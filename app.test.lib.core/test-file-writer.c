#include <lib.core/file-writer.h>
#include <lib.core/file-reader.h>
#include <lib.core/file.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-file-writer.h"

bool test_file_writer_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (file_writer_create (NULL, FileWriterModeTruncate));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_writer_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (file_writer_create ("/stage/file_writer/file", 9));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_file_writer_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (file_writer_create ("stage/file_writer/file", FileWriterModeTruncate));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_writer_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (file_writer_create ("/stage/file_writer/file", FileWriterModeTruncate));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_file_writer_create_system_call (Test *test)
{
        TITLE ();
        CATCH (file_writer_create ("/stage/file_writer/does/not/exist/file", FileWriterModeTruncate));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorSystemCall);
        PASS ();
}

bool test_file_writer_create_append (Test *test)
{
        FileWriter *writer;
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/append"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!(writer = file_writer_create (path, FileWriterModeAppend)));
        CATCH (!file_writer_write (writer, "abc", 3));
        file_writer_destroy (writer);
        CATCH (!(writer = file_writer_create (path, FileWriterModeAppend)));
        CATCH (!file_writer_write (writer, "123", 3));
        file_writer_destroy (writer);
        CATCH (!(reader = file_reader_create (path)));
        CATCH (!string_equals ((const char *)reader->map, "abc123"));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_create_truncate (Test *test)
{
        FileWriter *writer;
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/truncate"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (!file_writer_write (writer, "abc", 3));
        file_writer_destroy (writer);
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (!file_writer_write (writer, "123", 3));
        file_writer_destroy (writer);
        CATCH (!(reader = file_reader_create (path)));
        CATCH (!string_equals ((const char *)reader->map, "123"));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_write_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (file_writer_write (NULL, "test", 4));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_writer_write_invalid_argument_2 (Test *test)
{
        FileWriter *writer;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (file_writer_write (writer, NULL, 0));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        file_writer_destroy (writer);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_write_invalid_argument_3 (Test *test)
{
        FileWriter *writer;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (file_writer_write (writer, "", 0));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        file_writer_destroy (writer);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_write_system_call (Test *test)
{
        FileWriter *writer;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (file_writer_write (writer, "", 0));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        file_writer_destroy (writer);
        string_destroy (path);
        PASS ();
}
bool test_file_writer_write_args_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (file_writer_write_args (NULL, "%s", "test"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_file_writer_write_args_invalid_argument_2 (Test *test)
{
        FileWriter *writer;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (file_writer_write_args (writer, NULL, "test"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        file_writer_destroy (writer);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_write_args_system_call (Test *test)
{
        FileWriter *writer;
        FILE *file_readonly, *file_swap;
        char *path;

        TITLE ();
        CATCH (!(file_readonly = fopen ("stage/file_writer/file", "r")));
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        file_swap = writer->file;
        writer->file = file_readonly;
        CATCH (file_writer_write_args (writer, "%s", "test"));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorSystemCall);
        writer->file = file_swap;
        fclose (file_readonly);
        file_writer_destroy (writer);
        string_destroy (path);
        PASS ();
}

bool test_file_writer_write_args (Test *test)
{
        FileWriter *writer;
        FileReader *reader;
        char *path;

        TITLE ();
        CATCH (!(path = directory_current_path ()));
        CATCH (!string_append (&path, "/stage/file_writer/args"));
        if (file_exists (path)) {
                CATCH (!file_remove (path));
        }
        CATCH (!(writer = file_writer_create (path, FileWriterModeTruncate)));
        CATCH (!file_writer_write_args (writer, "abc%i123%s", 0, "4"));
        file_writer_destroy (writer);
        CATCH (!(reader = file_reader_create (path)));
        CATCH (!string_equals ((const char *)reader->map, "abc01234"));
        file_reader_destroy (reader);
        string_destroy (path);
        PASS ();
}
