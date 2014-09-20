#include <lib.display.plugin/display-plugin.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

DisplayPlugin *display_plugin_create (const char *path)
{
        DisplayPlugin *display_plugin;

        if (!path) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(display_plugin = memory_create (sizeof (DisplayPlugin)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(display_plugin->plugin = plugin_create (path))) {
                memory_destroy (display_plugin);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(plugin_set_function (display_plugin->plugin, 
                                   (void **)&display_plugin->display_canvas, 
                                   "display_canvas"))) {
                plugin_destroy (display_plugin->plugin);
                memory_destroy (display_plugin);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!display_plugin->plugin->load ()) {
                plugin_destroy (display_plugin->plugin);
                memory_destroy (display_plugin);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return display_plugin;
}

void display_plugin_destroy (DisplayPlugin *display_plugin)
{
        if (!display_plugin) {
                error (InvalidArgument);
                return;
        }
        display_plugin->plugin->unload ();
        plugin_destroy (display_plugin->plugin);
        memory_destroy (display_plugin);
}
