#include <lib.app/arguments.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <lib.core/convert.h>

static bool argument_named (int argc, char **argv, int *argi, AppArgument *arguments);
static bool argument_ordinal (int argc, char **argv, int *argi, AppArgument *arguments, int *order);
static bool check_required (AppArgument *arguments);
static void set_ordinal (AppArgument *arguments);
static void set_default (AppArgument *arguments);
static bool validate (AppArgument *arguments);

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
                        if (!((arguments[i].object.named.short_form[1] >= 'a' &&
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
                                if (!((arguments[i].object.named.long_form[k] == '-') ||
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
