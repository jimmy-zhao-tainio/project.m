#ifndef app_arguments_h
#define app_arguments_h

#include <lib.core/defines.h>

typedef struct s_AppArguments AppArguments;
typedef struct s_AppArgument AppArgument;
typedef struct s_AppArgumentOrdinal AppArgumentOrdinal;
typedef struct s_AppArgumentNamed AppArgumentNamed;

struct s_AppArgumentOrdinal
{
        int order;
};

struct s_AppArgumentNamed
{
        char *short_form;
        char *long_form;
};

struct s_AppArgument
{
        enum {
                AppArgumentTypeOrdinal,
                AppArgumentTypeNamed,
                AppArgumentTypeEnd
        } type;
        union {
                AppArgumentOrdinal ordinal;
                AppArgumentNamed named;
        } object;
        enum {
                AppArgumentBoolean,
                AppArgumentInteger,
                AppArgumentUInt64,
                AppArgumentString
        } value_type;
        bool have_value;
        union {
                bool boolean;
                int integer;
                uint64_t uint64;
                char *string;
        } value_default;
        bool required;
        union {
                bool *boolean;
                int *integer;
                uint64_t *uint64;
                char **string;
        } value;
        char *description;
};

bool app_arguments (int argc, char **argv, AppArgument *arguments);
void app_arguments_usage (int argc, char **argv, AppArgument *arguments);

#define ARGUMENT_ORDINAL_INTEGER(m_value_default, \
                                 m_required, \
                                 m_value, \
                                 m_description) \
        { \
                .type = AppArgumentTypeOrdinal, \
                .object.ordinal = { 0 }, \
                .value_type = AppArgumentInteger, \
                .have_value = false, \
                .value_default.integer = m_value_default, \
                .required = m_required, \
                .value.integer = m_value, \
                .description = m_description \
        }
#define ARGUMENT_ORDINAL_UINT64(m_value_default, \
                                m_required, \
                                m_value, \
                                m_description) \
        { \
                .type = AppArgumentTypeOrdinal, \
                .object.ordinal = { 0 }, \
                .value_type = AppArgumentUInt64, \
                .have_value = false, \
                .value_default.uint64 = m_value_default, \
                .required = m_required, \
                .value.uint64 = m_value, \
                .description = m_description \
        }
#define ARGUMENT_ORDINAL_STRING(m_value_default, \
                                m_required, \
                                m_value, \
                                m_description) \
        { \
                .type = AppArgumentTypeOrdinal, \
                .object.ordinal = { 0 }, \
                .value_type = AppArgumentString, \
                .have_value = false, \
                .value_default.string = m_value_default, \
                .required = m_required, \
                .value.string = m_value, \
                .description = m_description \
        }
#define ARGUMENT_NAMED_BOOLEAN(m_short_form, \
                               m_long_form, \
                               m_value_default, \
                               m_required, \
                               m_value, \
                               m_description) \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { m_short_form, m_long_form }, \
                .value_type = AppArgumentBoolean, \
                .have_value = false, \
                .value_default.boolean = m_value_default, \
                .required = m_required, \
                .value.boolean = m_value, \
                .description = m_description \
        }
#define ARGUMENT_NAMED_INTEGER(m_short_form, \
                               m_long_form, \
                               m_value_default, \
                               m_required, \
                               m_value, \
                               m_description) \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { m_short_form, m_long_form }, \
                .value_type = AppArgumentInteger, \
                .have_value = false, \
                .value_default.integer = m_value_default, \
                .required = m_required, \
                .value.integer = m_value, \
                .description = m_description \
        }
#define ARGUMENT_NAMED_UINT64(m_short_form, \
                              m_long_form, \
                              m_value_default, \
                              m_required, \
                              m_value, \
                              m_description) \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { m_short_form, m_long_form }, \
                .value_type = AppArgumentUInt64, \
                .have_value = false, \
                .value_default.uint64 = m_value_default, \
                .required = m_required, \
                .value.uint64 = m_value, \
                .description = m_description \
        }
#define ARGUMENT_NAMED_STRING(m_short_form, \
                              m_long_form, \
                              m_value_default, \
                              m_required, \
                              m_value, \
                              m_description) \
        { \
                .type = AppArgumentTypeNamed, \
                .object.named = { m_short_form, m_long_form }, \
                .value_type = AppArgumentString, \
                .have_value = false, \
                .value_default.string = m_value_default, \
                .required = m_required, \
                .value.string = m_value, \
                .description = m_description \
        }
#define ARGUMENT_END \
        { \
                .type = AppArgumentTypeEnd, \
                .object.ordinal = { -1 }, \
                .value_type = AppArgumentBoolean, \
                .have_value = false, \
                .value_default.boolean = false, \
                .required = false, \
                .value.boolean = NULL, \
                .description = NULL \
        }

#endif
