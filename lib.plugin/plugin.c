#include <lib.plugin/plugin.h>
#include <lib.core/file.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/threads.h>
#include <lib.core/string.h>
#include <stdio.h>
#include <dlfcn.h>

/* Workaround for memory leaks in glibc (_dlerror_run) */
static void dlopen_workaround (Thread *thread); 
static void dlclose_workaround (Thread *thread);
static void dlsym_workaround (Thread *thread);

Plugin *plugin_create (const char *path)
{
        Plugin *plugin;
        Thread *thread;

        if (!path) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(plugin = memory_create (sizeof (Plugin)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(plugin->path = string_create (path))) {
                plugin_destroy (plugin);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(plugin->name = file_name_from_path (path))) {
                plugin_destroy (plugin);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(thread = thread_create (dlopen_workaround, plugin))) {
                plugin_destroy (plugin);
                error_code (FunctionCall, 4);
                return NULL;
        }
        if (!thread_wait (thread)) {
                plugin_destroy (plugin);
                error_code (FunctionCall, 5);
                return NULL;
        }
        thread_destroy (thread);
        if (!plugin_set_function (plugin, (void **)&plugin->load, "load")) {
                error_code (FunctionCall, 1);
                thread_exit (thread);
        }
        if (!plugin_set_function (plugin, (void **)&plugin->unload, "unload")) {
                error_code (FunctionCall, 2);
                thread_exit (thread);
        }
        if (!plugin->handle || !plugin->load || !plugin->unload) {
                plugin_destroy (plugin);
                error_code (FunctionCall, 6);
                return NULL;
        }
        
        return plugin;
}

typedef struct {
        Plugin *plugin;
        void **function;
        char *function_name;
} DLSymWorkaround;

bool plugin_set_function (Plugin *plugin, void **function, char *function_name)
{
        DLSymWorkaround workaround;
        Thread *thread;

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
        workaround.plugin = plugin;
        workaround.function = function;
        workaround.function_name = function_name;
        if (!(thread = thread_create (dlsym_workaround, &workaround))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!thread_wait (thread)) {
                error_code (FunctionCall, 2);
                return false;
        }
        thread_destroy (thread);
        if (!*function) {
                error_code (FunctionCall, 3);
                return false;
        }
        return true;
}

void plugin_destroy (Plugin *plugin)
{
        Thread *thread;

        if (!plugin) {
                error (InvalidArgument);
                return;
        }
        if (!(thread = thread_create (dlclose_workaround, plugin))) {
                error_code (FunctionCall, 1);
                return;
        }
        if (!thread_wait (thread)) {
                error_code (FunctionCall, 2);
                return;
        }
        thread_destroy (thread);
        if (plugin->path) {
                memory_destroy (plugin->path);
        }
        if (plugin->name) {
                memory_destroy (plugin->name);
        }
        memory_destroy (plugin);
}

static void dlopen_workaround (Thread *thread)
{
        Plugin *plugin;

        plugin = thread->argument;
        if (!(plugin->handle = dlopen (plugin->path, RTLD_NOW))) {
                error (SystemCall);
        }
        thread_exit (thread);
}

static void dlclose_workaround (Thread *thread)
{
        Plugin *plugin;

        plugin = thread->argument;
        if (plugin->handle) {
                if (dlclose (plugin->handle) != 0) {
                        error (SystemCall);
                }
        }
        thread_exit (thread);
}

static void dlsym_workaround (Thread *thread)
{
        DLSymWorkaround *workaround;

        workaround = thread->argument;
        if (!(*workaround->function = dlsym (workaround->plugin->handle, workaround->function_name))) {
                error (SystemCall);
        }
        thread_exit (thread);
}
