#include <lib.app/arguments.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

#include "test-arguments.h"

bool test_app_arguments_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (app_arguments (-1, NULL, NULL))
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_app_arguments_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (app_arguments (0, NULL, NULL))
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_app_arguments_invalid_argument_3 (Test *test)
{
        char *argv[] = { "./app" };

        TITLE ();
        CATCH (app_arguments (1, argv, NULL))
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_app_arguments_function_call_1 (Test *test)
{
        char *argv[] = { "./app", "test" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN ("-t", "-t", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (1, argv, arguments))
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_app_arguments_function_call_2 (Test *test)
{
        char *argv[] = { "./app", "--dup", "--dup" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--dup", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (3, argv, arguments))
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_app_arguments_function_call_3 (Test *test)
{
        char *argv[] = { "./app", "NAN" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (1, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_app_arguments_function_call_4 (Test *test)
{
        char *argv[] = { "./app", "--argument" };
        bool value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_BOOLEAN (NULL, "--argument", false, false, &value, "test"),
                ARGUMENT_NAMED_BOOLEAN (NULL, "--required", false, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 5);
        PASS ();
}

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

bool test_app_arguments_named_duplicate (Test *test)
{
        char *argv[] = { "./app", "-a", "1", "-a", "2" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_INTEGER ("-a", NULL, 123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (5, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentDuplicate);
        PASS ();
}

bool test_app_arguments_named_missing_integer_value (Test *test)
{
        char *argv[] = { "./app", "-a" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_INTEGER ("-a", NULL, 123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentMissingIntegerValue);
        PASS ();
}

bool test_app_arguments_named_missing_uint64_value (Test *test)
{
        char *argv[] = { "./app", "-a" };
        uint64_t value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_UINT64 ("-a", NULL, 123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentMissingUInt64Value);
        PASS ();
}

bool test_app_arguments_named_function_call (Test *test)
{
        char *argv[] = { "./app", "-a", "abc" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_INTEGER ("-a", NULL, 123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (3, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorFunctionCall);
        PASS ();
}

bool test_app_arguments_named_invalid_integer_value (Test *test)
{
        char *argv[] = { "./app", "-a", "123abc" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_INTEGER ("-a", NULL, 123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (3, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidIntegerValue);
        PASS ();
}

bool test_app_arguments_named_invalid_uint64_value (Test *test)
{
        char *argv[] = { "./app", "-a", "123abc" };
        uint64_t value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_UINT64 ("-a", NULL, (uint64_t)123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (3, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidUInt64Value);
        PASS ();
}

bool test_app_arguments_named_missing_string_value (Test *test)
{
        char *argv[] = { "./app", "-a" };
        char *value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_STRING ("-a", NULL, "abc", true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentMissingStringValue);
        PASS ();
}

bool test_app_arguments_named_unknown_argument (Test *test)
{
        char *argv[] = { "./app", "-b" };
        char *value;
        AppArgument arguments[] = {
                ARGUMENT_NAMED_STRING ("-a", NULL, "abc", true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentUnknownNamedArgument);
        PASS ();
}

bool test_app_arguments_ordinal_function_call (Test *test)
{
        char *argv[] = { "./app", "abc" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorFunctionCall);
        PASS (); 
}

bool test_app_arguments_ordinal_invalid_integer_value (Test *test)
{
        char *argv[] = { "./app", "123abc" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidIntegerValue);
        PASS ();
}

bool test_app_arguments_ordinal_invalid_uint64_value (Test *test)
{
        char *argv[] = { "./app", "123abc" };
        uint64_t value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_UINT64 (123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentInvalidUInt64Value);
        PASS ();
}

bool test_app_arguments_ordinal_unknown_argument (Test *test)
{
        char *argv[] = { "./app", "123", "unknown" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (123, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (3, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentUnknownOrdinalArgument);
        PASS ();
}

bool test_app_arguments_missing_required_argument (Test *test)
{
        char *argv[] = { "./app", "123" };
        int value;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (123, true, &value, "test"),
                ARGUMENT_ORDINAL_INTEGER (321, true, &value, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (app_arguments (2, argv, arguments))
        CATCH (error_count () < 2);
        CATCH (error_at (1).error != ErrorAppArgumentMissingRequiredArgument);
        PASS ();
}

bool test_app_arguments (Test *test)
{
        char *argv[] = { 
                "./app", 
                "321",
                "654",
                "cba",
                "-b",
                "--named-integer", "-2",
                "--named-uint64", "2",
                "-s", "test"
        };
        int ordinal_integer;
        uint64_t ordinal_uint64;
        char *ordinal_string;
        bool named_boolean;
        int named_integer;
        uint64_t named_uint64;
        char *named_string;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_INTEGER (123, true, &ordinal_integer, "test"),
                ARGUMENT_ORDINAL_UINT64 (123, true, &ordinal_uint64, "test"),
                ARGUMENT_ORDINAL_STRING ("abc", true, &ordinal_string, "test"),
                ARGUMENT_NAMED_BOOLEAN ("-b", "--named-boolean", false, true, &named_boolean, "test"),
                ARGUMENT_NAMED_INTEGER ("-i", "--named-integer", 1, true, &named_integer, "test"),
                ARGUMENT_NAMED_UINT64 ("-u", "--named-uint64", 1, true, &named_uint64, "test"),
                ARGUMENT_NAMED_STRING ("-s", "--named-string", "abc", true, &named_string, "test"),
                ARGUMENT_END
        };

        TITLE ();
        CATCH (!app_arguments (11, argv, arguments))
        CATCH (error_count () != 0);
        CATCH (ordinal_integer != 321);
        CATCH (ordinal_uint64 != 654);
        CATCH (!string_equals (ordinal_string, "cba"));
        CATCH (named_boolean != true);
        CATCH (named_integer != -2);
        CATCH (named_uint64 != 2);
        CATCH (!string_equals (named_string, "test"));
        PASS ();
}
