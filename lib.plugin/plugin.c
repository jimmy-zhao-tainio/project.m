#include <lib.plugin/plugin.h>
#include <lib.core/file.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <dlfcn.h>

Plugin *plugin_create (const char *path)
{
        Plugin *plugin;

        if (!path) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(plugin = memory_create (sizeof (Plugin)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(plugin->name = file_name_from_path (path))) {
                memory_destroy (plugin);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(plugin->handle = dlopen (path, RTLD_NOW))) {
                printf ("%s\n", dlerror ());
                memory_destroy (plugin->name);
                memory_destroy (plugin);
                error_code (SystemCall, 1);
                return NULL;
        }
        if (!plugin_set_function (plugin, (void **)&plugin->load, "load")) {
                if (dlclose (plugin->handle) != 0) {
                        error_code (SystemCall, 2);
                }
                memory_destroy (plugin->name);
                memory_destroy (plugin);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!plugin_set_function (plugin, (void **)&plugin->unload, "unload")) {
                if (dlclose (plugin->handle) != 0) {
                        error_code (SystemCall, 3);
                }
                memory_destroy (plugin->name);
                memory_destroy (plugin);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return plugin;
}

bool plugin_set_function (Plugin *plugin, void **function, char *function_name)
{
        if (!plugin) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!function) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (!function_name) {
                error_code (InvalidArgument, 3);
                return false;
        }
        if (!plugin->handle) {
                error (InvalidOperation);
                return false;
        }
        if (!(*function = dlsym (plugin->handle, function_name))) {
                error (SystemCall);
                return false;
        }
        return true;
}

void plugin_destroy (Plugin *plugin)
{
        if (!plugin) {
                error (InvalidArgument);
                return;
        }
        if (dlclose (plugin->handle) != 0) {
                error (SystemCall);
        }
        memory_destroy (plugin->name);
        memory_destroy (plugin);
}
