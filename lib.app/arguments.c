#include <lib.app/arguments.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <lib.core/convert.h>
#include <stdio.h>

static bool argument_named (int argc, char **argv, int *argi, AppArgument *arguments);
static bool argument_ordinal (int argc, char **argv, int *argi, AppArgument *arguments, int *order);
static bool check_required (AppArgument *arguments);
static void set_ordinal (AppArgument *arguments);
static void set_default (AppArgument *arguments);
static bool validate (AppArgument *arguments);
static void print_value_type (int type);

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
        if (!validate (arguments)) {
                error_code (FunctionCall, 1);
                return false;
        }
        set_ordinal (arguments);
        set_default (arguments);
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
        if (!check_required (arguments)) {
                error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

void app_arguments_usage (int argc, char **argv, AppArgument *arguments)
{
        size_t i;
        size_t required_arguments = 0;
        size_t optional_arguments = 0;
        size_t required_named_arguments = 0;
        size_t optional_named_arguments = 0;
        int indent_usage;

        if (argc < 0) {
                error_code (InvalidArgument, 1);
                error_print ();
                return;
        }
        if (!argv) {
                error_code (InvalidArgument, 2);
                error_print ();
                return;
        }
        if (!arguments) {
                error_code (InvalidArgument, 3);
                error_print ();
                return;
        }
        if (!validate (arguments)) {
                error_code (FunctionCall, 1);
                error_print ();
                return;
        }
        printf ("Usage: %s", argv[0]);
        indent_usage = (int)(string_length ("Usage: ") + string_length (argv[0]) + 1);
        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (arguments[i].type == AppArgumentTypeOrdinal) {
                        if (arguments[i].required) {
                                required_arguments++;
                        }
                        else {
                                optional_arguments++;
                        }
                }
                else {
                        if (arguments[i].required) {
                                required_named_arguments++;
                        }
                        else {
                                optional_named_arguments++;
                        }
                }
        }
        if (required_arguments != 0) {
                printf (" [%zu required argument%s]\n", 
                        required_arguments, required_arguments == 1 ? "" : "s");
                printf ("%*s", indent_usage, "");
        }
        if (optional_arguments != 0) {
                printf (" (%zu optional argument%s)\n", 
                        optional_arguments, optional_arguments == 1 ? "" : "s");
                printf ("%*s", indent_usage, "");
        }
        if (required_named_arguments != 0) {
                printf (" [%zu required named argument%s]\n", 
                        required_named_arguments, required_named_arguments == 1 ? "" : "s");
                printf ("%*s", indent_usage, "");
        }
        if (optional_named_arguments != 0) {
                printf (" (%zu optional named argument%s)\n", 
                        optional_named_arguments, optional_named_arguments == 1 ? "" : "s");
        }
        if (required_arguments != 0) {
                printf ("\nRequired arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type != AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (!arguments[i].required) {
                                continue;
                        }
                        printf ("\t");
                        print_value_type (arguments[i].value_type);
                        printf ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (optional_arguments != 0) {
                printf ("\nOptional arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type != AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (arguments[i].required) {
                                continue;
                        }
                        printf ("\t");
                        print_value_type (arguments[i].value_type);
                        printf ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (required_named_arguments != 0) {
                printf ("\nRequired named arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type == AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (!arguments[i].required) {
                                continue;
                        }
                        if (arguments[i].object.named.short_form) {
                                printf ("\t%s", arguments[i].object.named.short_form);
                        }
                        if (arguments[i].object.named.long_form) {
                                if (arguments[i].object.named.short_form) {
                                        printf (" | %s", arguments[i].object.named.short_form);
                                }
                                else {
                                        printf ("\t%s", arguments[i].object.named.short_form);
                                }
                        }
                        printf (" ");
                        print_value_type (arguments[i].value_type);
                        printf ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (optional_named_arguments != 0) {
                printf ("\nOptional named arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type == AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (arguments[i].required) {
                                continue;
                        }
                        if (arguments[i].object.named.short_form) {
                                printf ("\t%s", arguments[i].object.named.short_form);
                        }
                        if (arguments[i].object.named.long_form) {
                                if (arguments[i].object.named.short_form) {
                                        printf (" | %s", arguments[i].object.named.short_form);
                                }
                                else {
                                        printf ("\t%s", arguments[i].object.named.short_form);
                                }
                        }
                        printf (" ");
                        print_value_type (arguments[i].value_type);
                        printf ("\n\t    %s\n\n", arguments[i].description);
                }
        }
}

static bool argument_named (int argc, char **argv, int *argi, AppArgument *arguments)
{
        size_t i, digits;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                if (arguments[i].type != AppArgumentTypeNamed) {
                        continue;
                }
                if (!string_equals (argv[*argi], arguments[i].object.named.short_form) &&
                    !string_equals (argv[*argi], arguments[i].object.named.long_form)) {
                        continue;
                }
                if (arguments[i].have_value) {
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
                                error (AppArgumentMissingIntegerValue);
                                return false;
                        }
                        if (!convert_string_to_int (argv[*argi + 1], arguments[i].value.integer, &digits)) {
                                error (FunctionCall);
                                return false;
                        }
                        if (digits != string_length (argv[*argi + 1])) {
                                error (AppArgumentInvalidIntegerValue);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                case AppArgumentUInt64:
                        if (*argi + 1 == argc) {
                                error (AppArgumentMissingUInt64Value);
                                return false;
                        }
                        if (!convert_string_to_unsigned_long_long (argv[*argi + 1], 
                                                                   (unsigned long long *)arguments[i].value.uint64, 
                                                                   &digits)) {
                                error (FunctionCall);
                                return false;
                        }
                        if (digits != string_length (argv[*argi + 1])) {
                                error (AppArgumentInvalidUInt64Value);
                                return false;
                        }
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                case AppArgumentString:
                        if (*argi + 1 == argc) {
                                error (AppArgumentMissingStringValue);
                                return false;
                        }
                        *arguments[i].value.string = argv[*argi + 1];
                        arguments[i].have_value = true;
                        *argi += 2;
                        return true;
                }
        }
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
                                error (FunctionCall);
                                return false;
                        }
                        if (digits != string_length (argv[*argi])) {
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
                                error (FunctionCall);
                                return false;
                        }
                        if (digits != string_length (argv[*argi])) {
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
                        error (AppArgumentMissingRequiredArgument);
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

static bool validate (AppArgument *arguments)
{
        size_t i, k;
        size_t length;
        bool have_not_required = false;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                switch (arguments[i].value_type) {
                case AppArgumentBoolean:
                        if (!arguments[i].value.boolean) {
                                error (AppArgumentBooleanValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentInteger:
                        if (!arguments[i].value.integer) {
                                error (AppArgumentIntegerValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentUInt64:
                        if (!arguments[i].value.uint64) {
                                error (AppArgumentUInt64ValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentString:
                        if (!arguments[i].value.string) {
                                error (AppArgumentStringValuePointerIsNull);
                                return false;
                        }
                        break;
                }
                if (arguments[i].type == AppArgumentTypeOrdinal) {
                        if (arguments[i].required) {
                                if (have_not_required) {
                                        error (AppArgumentOrdinalRequiredDiscontinuity);
                                        return false;
                                }
                        }
                        else {
                                have_not_required = true;
                        }
                        continue;
                }
                if (!arguments[i].object.named.short_form &&
                    !arguments[i].object.named.long_form) {
                        error (AppArgumentMissingShortOrLongForm);
                        return false;
                }
                if (arguments[i].object.named.short_form) {
                        if (string_length (arguments[i].object.named.short_form) != 2) {
                                error (AppArgumentInvalidShortFormLength);
                                return false;
                        }
                        if (arguments[i].object.named.short_form[0] != '-') {
                                error (AppArgumentShortFormMissingSingleDash);
                                return false;
                        }
                        if (arguments[i].object.named.short_form[1] == '-') {
                                error_code (AppArgumentInvalidShortFormName, 1);
                                return false;
                        }
                        if (!((arguments[i].object.named.short_form[1] >= '0' &&
                               arguments[i].object.named.short_form[1] <= '9') ||
                              (arguments[i].object.named.short_form[1] >= 'a' &&
                               arguments[i].object.named.short_form[1] <= 'z') ||
                              (arguments[i].object.named.short_form[1] >= 'A' &&
                               arguments[i].object.named.short_form[1] <= 'Z'))) {
                                error_code (AppArgumentInvalidShortFormName, 2);
                                return false;
                        }
                }
                if (arguments[i].object.named.long_form) {
                        if (!string_begins_with (arguments[i].object.named.long_form, "--")) {
                                error (AppArgumentLongFormMissingDoubleDash);
                                return false;
                        }
                        if ((length = string_length (arguments[i].object.named.long_form)) < 3) {
                                error (AppArgumentInvalidLongFormLength);
                                return false;
                        }
                        if (arguments[i].object.named.long_form[2] == '-') {
                                error_code (AppArgumentInvalidLongFormName, 1);
                                return false;
                        }
                        for (k = 2; k < length; k++) {
                                if (!((arguments[i].object.named.long_form[k] >= '0' &&
                                       arguments[i].object.named.long_form[k] <= '9') ||
                                      (arguments[i].object.named.long_form[k] == '-') ||
                                      (arguments[i].object.named.long_form[k] >= 'a' &&
                                       arguments[i].object.named.long_form[k] <= 'z') ||
                                      (arguments[i].object.named.long_form[k] >= 'A' &&
                                       arguments[i].object.named.long_form[k] <= 'Z'))) {
                                        error_code (AppArgumentInvalidLongFormName, 2);
                                        return false;
                                }
                        }
                }
                for (k = 0; arguments[k].type != AppArgumentTypeEnd; k++) {
                        if (arguments[k].type != AppArgumentTypeNamed) {
                                continue;
                        }
                        if (k == i) {
                                continue;
                        }
                        if (string_equals (arguments[k].object.named.short_form,
                                           arguments[i].object.named.short_form)) {
                                error (AppArgumentShortFormDuplicate);
                                return false;
                        }
                        if (string_equals (arguments[k].object.named.long_form,
                                           arguments[i].object.named.long_form)) {
                                error (AppArgumentLongFormDuplicate);
                                return false;
                        }
                }
        }
        return true;
}

static void print_value_type (int type)
{
        switch (type) {
        case AppArgumentBoolean:
                break;
        case AppArgumentInteger:
                printf ("<Integer>");
                break;
        case AppArgumentUInt64:
                printf ("<UInt64>");
                break;
        case AppArgumentString:
                printf ("<String>");
                break;
        }
}
