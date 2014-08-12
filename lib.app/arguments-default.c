#include <lib.app/arguments-default.h>
#include <lib.app/arguments-usage.h>
#include <lib.core/memory.h>
#include <lib.core/print.h>
#include <lib.core/error.h>
#include <stdlib.h>

AppArgumentDefault app_argument_default_object;

bool app_arguments_default (int argc, char **argv, AppArgument *arguments)
{
        if (app_argument_default_object.help) {
                app_arguments_usage (argc, argv, arguments);
                exit (EXIT_SUCCESS);
        }
        if (app_argument_default_object.memory_commit_limit != UINT64_MAX) {
                memory_commit_limit (app_argument_default_object.memory_commit_limit);
        }
        if (app_argument_default_object.print_silent) {
                print_silent (true);
        }
        if (app_argument_default_object.print_log) {
                if (!print_log_begin (app_argument_default_object.print_log)) {
                        error (FunctionCall);
                        return false;
                }
        }
        if (app_argument_default_object.print_errors) {
                error_silent (false);
        }
        return true;
}
