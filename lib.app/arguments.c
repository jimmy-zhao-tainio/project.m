#include <lib.app/arguments.h>
#include <lib.app/arguments-validate.h>
#include <lib.app/arguments-usage.h>
#include <lib.app/arguments-default.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <lib.core/convert.h>
#include <lib.core/print.h>
#include <stdio.h>

static bool argument_named (int argc, char **argv, int *argi, AppArgument *arguments);
static bool argument_ordinal (int argc, char **argv, int *argi, AppArgument *arguments, int *order);
static bool check_required (AppArgument *arguments);
static void set_ordinal (AppArgument *arguments);
static void set_default (AppArgument *arguments);

bool app_arguments (int argc, char **argv, AppArgument *arguments)
{
        int argi = 1;
        int order = 1;

        if (argc < 0) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!argv) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (!arguments) {
                error_code (InvalidArgument, 3);
                return false;
        }
        if (!arguments_validate (arguments)) {
                error_code (FunctionCall, 1);
                return false;
        }
        set_ordinal (arguments);
        set_default (arguments);
        app_arguments_default_reset ();
        while (argi < argc) {
                if (string_begins_with (argv[argi], "-")) {
                        if (!argument_named (argc, argv, &argi, arguments)) {
                                error_code (FunctionCall, 2);
                                return false;
                        }
                }
                else {
                        if (!argument_ordinal (argc, argv, &argi, arguments, &order)) {
                                error_code (FunctionCall, 3);
                                return false;
                        }
                }
        }
        if (!app_arguments_default (argc, argv, arguments)) {
                error_code (FunctionCall, 4);
                return false;
        }
        if (!check_required (arguments)) {
                error_code (FunctionCall, 5);
                return false;
        }
        return true;
}

static bool argument_named (int argc, char **argv, int *argi, AppArgument *arguments)
{
        size_t i, digits;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (arguments[i].type != AppArgumentTypeNamed) {
                        continue;
                }
                if (!((arguments[i].object.named.short_form && 
                       string_equals (argv[*argi], arguments[i].object.named.short_form)) ||
                      (arguments[i].object.named.long_form && 
                       string_equals (argv[*argi], arguments[i].object.named.long_form)))) {
                        continue;
                }
                if (arguments[i].have_value) {
                        print ("The argument ");
                        app_arguments_print_named_form (arguments[i]);
                        print (" is duplicated.\n");
                        error (AppArgumentDuplicate);
                        return false;
                }
                switch (arguments[i].value_type) {
                case AppArgumentBoolean:
                        *arguments[i].value.boolean = !arguments[i].value_default.boolean;
                        arguments[i].have_value = true;
                        *argi += 1;
                        return true;
                case AppArgumentInteger:
                        if (*argi + 1 == argc) {
                                print ("The argument ");
                                app_arguments_print_named_form (arguments[i]);
                                print (" is missing an integer value.\n");
                                error (AppArgumentMissingIntegerValue);
                                return false;
                        }
                        if (!convert_string_to_int (argv[*argi + 1], arguments[i].value.integer, &digits)) {
                                print ("The value '%s' for ", argv[*argi + 1]);
                                app_arguments_print_named_form (arguments[i]);
                                print (" could not be converted to an integer value.\n");
                                error_code (FunctionCall, 1);
                                return false;
                        }
                        if (digits != string_length (argv[*argi + 1])) {
                                print ("The value '%s' for ", argv[*argi + 1]);
                                app_arguments_print_named_form (arguments[i]);
                                print (" could not be converted to an integer value.\n");
                                error (AppArgumentInvalidIntegerValue);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                case AppArgumentUInt64:
                        if (*argi + 1 == argc) {
                                print ("The argument ");
                                app_arguments_print_named_form (arguments[i]);
                                print (" is missing an UInt64 value.\n");
                                error (AppArgumentMissingUInt64Value);
                                return false;
                        }
                        if (!convert_string_to_unsigned_long_long (argv[*argi + 1], 
                                                                   (unsigned long long *)arguments[i].value.uint64, 
                                                                   &digits)) {
                                print ("The value '%s' for ", argv[*argi + 1]);
                                app_arguments_print_named_form (arguments[i]);
                                print (" could not be converted to an UInt64 value.\n");
                                error_code (FunctionCall, 2);
                                return false;
                        }
                        if (digits != string_length (argv[*argi + 1])) {
                                print ("The value '%s' for ", argv[*argi + 1]);
                                app_arguments_print_named_form (arguments[i]);
                                print (" could not be converted to an UInt64 value.\n");
                                error (AppArgumentInvalidUInt64Value);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                case AppArgumentString:
                        if (*argi + 1 == argc) {
                                print ("The argument ");
                                app_arguments_print_named_form (arguments[i]);
                                print (" is missing a string value.\n");
                                error (AppArgumentMissingStringValue);
                                return false;
                        }
                        *arguments[i].value.string = argv[*argi + 1];
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                }
        }
        print ("Unrecognized argument '%s'.\n", argv[*argi]);
        error (AppArgumentUnknownNamedArgument);
        return false;
}

static bool argument_ordinal (int argc, char **argv, int *argi, AppArgument *arguments, int *order)
{
        size_t i, digits;

        (void)argc;
        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (arguments[i].type != AppArgumentTypeOrdinal) {
                        continue;
                }
                if (arguments[i].object.ordinal.order != *order) {
                        continue;
                }
                switch (arguments[i].value_type) {
                case AppArgumentBoolean:
                        error (InvalidOperation);
                        return false;
                case AppArgumentInteger:
                        if (!convert_string_to_int (argv[*argi], arguments[i].value.integer, &digits)) {
                                print ("The value '%s' could not be converted to an integer value.\n", argv[*argi]);
                                error_code (FunctionCall, 1);
                                return false;
                        }
                        if (digits != string_length (argv[*argi])) {
                                print ("The value '%s' could not be converted to an integer value.\n", argv[*argi]);
                                error (AppArgumentInvalidIntegerValue);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 1;
                        *order += 1;
                        return true;
                case AppArgumentUInt64:
                        if (!convert_string_to_unsigned_long_long (argv[*argi], 
                                                                   (unsigned long long *)arguments[i].value.uint64, 
                                                                   &digits)) {
                                print ("The value '%s' could not be converted to an UInt64 value.\n", argv[*argi]);
                                error_code (FunctionCall, 2);
                                return false;
                        }
                        if (digits != string_length (argv[*argi])) {
                                print ("The value '%s' could not be converted to an UInt64 value.\n", argv[*argi]);
                                error (AppArgumentInvalidUInt64Value);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 1;
                        *order += 1;
                        return true;
                case AppArgumentString:
                        *arguments[i].value.string = argv[*argi];
                        arguments[i].have_value = true;
                        *argi += 1;
                        *order += 1;
                        return true;
                }
        }
        print ("Unrecognized argument '%s'.\n", argv[*argi]);
        error (AppArgumentUnknownOrdinalArgument);
        return false;
}

static bool check_required (AppArgument *arguments)
{
        size_t i;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (!arguments[i].required) {
                        continue;
                }
                if (!arguments[i].have_value) {
                        if (arguments[i].type == AppArgumentTypeOrdinal) {
                                print ("A required argument is missing.\n");
                                error_code (AppArgumentMissingRequiredArgument, 1);
                        }
                        else {
                                print ("The required argument ");
                                app_arguments_print_named_form (arguments[i]);
                                print (" is missing.\n");
                                error_code (AppArgumentMissingRequiredArgument, 2);
                        }
                        return false;
                }
        }
        return true;
}

static void set_ordinal (AppArgument *arguments)
{
        int order = 1;
        size_t i;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (arguments[i].type != AppArgumentTypeOrdinal) {
                        continue;
                }
                arguments[i].object.ordinal.order = order;
                order++;
        }
}

static void set_default (AppArgument *arguments)
{
        size_t i;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                arguments[i].have_value = false;
                switch (arguments[i].value_type) {
                case AppArgumentBoolean:
                        *arguments[i].value.boolean = arguments[i].value_default.boolean;
                        break;
                case AppArgumentInteger:
                        *arguments[i].value.integer = arguments[i].value_default.integer;
                        break;
                case AppArgumentUInt64:
                        *arguments[i].value.uint64 = arguments[i].value_default.uint64;
                        break;
                case AppArgumentString:
                        *arguments[i].value.string = arguments[i].value_default.string;
                        break;
                }
        }
}
