#include <lib.app/arguments.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

#include "test-arguments-validate.h"

bool test_app_arguments_validate_boolean_value_pointer_is_null (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--argument", false, true, NULL, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentBooleanValuePointerIsNull);
        PASS ();
}

bool test_app_arguments_validate_integer_value_pointer_is_null (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        AppArgument arguments[] = {
                ARGUMENT_NAMED_INTEGER (NULL, "--argument", 123, true, NULL, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentIntegerValuePointerIsNull);
        PASS ();
}

bool test_app_arguments_validate_uint64_value_pointer_is_null (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        AppArgument arguments[] = {
                ARGUMENT_NAMED_UINT64 (NULL, "--argument", 123, true, NULL, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentUInt64ValuePointerIsNull);
        PASS ();
}

bool test_app_arguments_validate_string_value_pointer_is_null (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        AppArgument arguments[] = {
                ARGUMENT_NAMED_STRING (NULL, "--argument", "abc", true, NULL, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentStringValuePointerIsNull);
        PASS ();
}

bool test_app_arguments_validate_ordinal_required_discontinuity (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        int value_1;
        int value_2;

        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (1, false, &value_1, "test"),
                ARGUMENT_ORDINAL_INTEGER (1, true, &value_2, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentOrdinalRequiredDiscontinuity);
        PASS ();
}

bool test_app_arguments_validate_missing_short_or_long_form (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentMissingShortOrLongForm);
        PASS ();
}

bool test_app_arguments_validate_invalid_short_form_length (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-", NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidShortFormLength);
        PASS ();
}

bool test_app_arguments_validate_short_form_missing_single_dash (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("+a", NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentShortFormMissingSingleDash);
        PASS ();
}

bool test_app_arguments_validate_invalid_short_form_name_1 (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("--", NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidShortFormName);
        CATCH (error_at (1).code != 1);
        PASS ();
}

bool test_app_arguments_validate_invalid_short_form_name_2 (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-_", NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidShortFormName);
        CATCH (error_at (1).code != 2);
        PASS ();
}

bool test_app_arguments_validate_long_form_missing_double_dash (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentLongFormMissingDoubleDash);
        PASS ();
}

bool test_app_arguments_validate_invalid_long_form_length (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidLongFormLength);
        PASS ();
}

bool test_app_arguments_validate_invalid_long_form_name_1 (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "---", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidLongFormName);
        CATCH (error_at (1).code != 1);
        PASS ();
}

bool test_app_arguments_validate_invalid_long_form_name_2 (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--_", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidLongFormName);
        CATCH (error_at (1).code != 2);
        PASS ();
}

bool test_app_arguments_validate_short_form_duplicate (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-a", NULL, false, true, &value, "test"),
                ARGUMENT_NAMED_BOOLEAN ("-a", NULL, false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentShortFormDuplicate);
        PASS ();
}

bool test_app_arguments_validate_long_form_duplicate (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--argument", false, true, &value, "test"),
                ARGUMENT_NAMED_BOOLEAN (NULL, "--argument", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentLongFormDuplicate);
        PASS ();
}
