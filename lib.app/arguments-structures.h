#ifndef arguments_structures_h
#define arguments_structures_h

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

#endif
