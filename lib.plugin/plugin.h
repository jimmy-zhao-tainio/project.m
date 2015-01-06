#ifndef plugin_h
#define plugin_h

#include <lib.core/defines.h>

typedef struct s_Plugin Plugin;

struct s_Plugin
{
        char *path;
        char *name;
        void *handle;
        bool (*load)(void);
        void (*unload)(void);
};

Plugin *plugin_create (const char *path);
bool plugin_set_function (Plugin *plugin, void **function, char *function_name);
void plugin_destroy (Plugin *plugin);

#endif
