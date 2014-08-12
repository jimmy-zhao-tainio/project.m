#ifndef app_arguments_default_h
#define app_arguments_default_h

#include <lib.app/arguments-structures.h>

typedef struct s_AppArgumentDefault AppArgumentDefault;

struct s_AppArgumentDefault
{
        bool help;
        uint64_t memory_commit_limit;
        bool print_silent;
        char *print_log;
        bool print_errors;
};

extern AppArgumentDefault app_argument_default_object;

#define ARGUMENT_DEFAULT_HELP                "--help"
#define ARGUMENT_DEFAULT_MEMORY_COMMIT_LIMIT "--memory-commit-limit"
#define ARGUMENT_DEFAULT_PRINT_SILENT        "--print-silent"
#define ARGUMENT_DEFAULT_PRINT_LOG           "--print-log"
#define ARGUMENT_DEFAULT_PRINT_ERRORS        "--print-errors"

#define ARGUMENT_DEFAULT \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { NULL, ARGUMENT_DEFAULT_HELP }, \
                .value_type = AppArgumentBoolean, \
                .have_value = false, \
                .value_default.boolean = false, \
                .required = false, \
                .value.boolean = &app_argument_default_object.help, \
                .description = "Print this help message and exit." \
        }, \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { NULL, ARGUMENT_DEFAULT_MEMORY_COMMIT_LIMIT }, \
                .value_type = AppArgumentUInt64, \
                .have_value = false, \
                .value_default.uint64 = UINT64_MAX, \
                .required = false, \
                .value.uint64 = &app_argument_default_object.memory_commit_limit, \
                .description = "Set memory commit limit in bytes." \
        }, \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { NULL, ARGUMENT_DEFAULT_PRINT_SILENT }, \
                .value_type = AppArgumentBoolean, \
                .have_value = false, \
                .value_default.boolean = false, \
                .required = false, \
                .value.boolean = &app_argument_default_object.print_silent, \
                .description = "Do not print to stdout." \
        }, \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { NULL, ARGUMENT_DEFAULT_PRINT_LOG }, \
                .value_type = AppArgumentString, \
                .have_value = false, \
                .value_default.string = NULL, \
                .required = false, \
                .value.string = &app_argument_default_object.print_log, \
                .description = "Print to log file. Specify a full path." \
        }, \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { NULL, ARGUMENT_DEFAULT_PRINT_ERRORS }, \
                .value_type = AppArgumentBoolean, \
                .have_value = false, \
                .value_default.boolean = false, \
                .required = false, \
                .value.boolean = &app_argument_default_object.print_errors, \
                .description = "Print errors." \
        }

bool app_arguments_default (int argc, char **argv, AppArgument *arguments);

#endif
