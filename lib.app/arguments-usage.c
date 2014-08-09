#include <lib.app/arguments-usage.h>
#include <lib.app/arguments-validate.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <lib.core/print.h>

static void print_value_type (int type);

void arguments_usage (int argc, char **argv, AppArgument *arguments)
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
        if (!arguments_validate (arguments)) {
                error_code (FunctionCall, 1);
                error_print ();
                return;
        }
        print ("Usage: %s", argv[0]);
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
                print (" [%zu required argument%s]\n", 
                        required_arguments, required_arguments == 1 ? "" : "s");
                print ("%*s", indent_usage, "");
        }
        if (optional_arguments != 0) {
                print (" (%zu optional argument%s)\n", 
                        optional_arguments, optional_arguments == 1 ? "" : "s");
                print ("%*s", indent_usage, "");
        }
        if (required_named_arguments != 0) {
                print (" [%zu required named argument%s]\n", 
                        required_named_arguments, required_named_arguments == 1 ? "" : "s");
                print ("%*s", indent_usage, "");
        }
        if (optional_named_arguments != 0) {
                print (" (%zu optional named argument%s)\n", 
                        optional_named_arguments, optional_named_arguments == 1 ? "" : "s");
        }
        if (required_arguments != 0) {
                print ("\nRequired arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type != AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (!arguments[i].required) {
                                continue;
                        }
                        print ("\t");
                        print_value_type (arguments[i].value_type);
                        print ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (optional_arguments != 0) {
                print ("\nOptional arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type != AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (arguments[i].required) {
                                continue;
                        }
                        print ("\t");
                        print_value_type (arguments[i].value_type);
                        print ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (required_named_arguments != 0) {
                print ("\nRequired named arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type == AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (!arguments[i].required) {
                                continue;
                        }
                        if (arguments[i].object.named.short_form) {
                                print ("\t%s", arguments[i].object.named.short_form);
                        }
                        if (arguments[i].object.named.long_form) {
                                if (arguments[i].object.named.short_form) {
                                        print (" | %s", arguments[i].object.named.short_form);
                                }
                                else {
                                        print ("\t%s", arguments[i].object.named.short_form);
                                }
                        }
                        print (" ");
                        print_value_type (arguments[i].value_type);
                        print ("\n\t    %s\n\n", arguments[i].description);
                }
        }
        if (optional_named_arguments != 0) {
                print ("\nOptional named arguments:\n\n");
                for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                        if (arguments[i].type == AppArgumentTypeOrdinal) {
                                continue;
                        }
                        if (arguments[i].required) {
                                continue;
                        }
                        if (arguments[i].object.named.short_form) {
                                print ("\t%s", arguments[i].object.named.short_form);
                        }
                        if (arguments[i].object.named.long_form) {
                                if (arguments[i].object.named.short_form) {
                                        print (" | %s", arguments[i].object.named.short_form);
                                }
                                else {
                                        print ("\t%s", arguments[i].object.named.short_form);
                                }
                        }
                        print (" ");
                        print_value_type (arguments[i].value_type);
                        print ("\n\t    %s\n\n", arguments[i].description);
                }
        }
}

static void print_value_type (int type)
{
        switch (type) {
        case AppArgumentBoolean:
                break;
        case AppArgumentInteger:
                print ("<Integer>");
                break;
        case AppArgumentUInt64:
                print ("<UInt64>");
                break;
        case AppArgumentString:
                print ("<String>");
                break;
        }
}
