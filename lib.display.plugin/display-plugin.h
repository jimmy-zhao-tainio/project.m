#ifndef display_plugin_h
#define display_plugin_h

#include <lib.plugin/plugin.h>
#include <lib.graphics/graphics.h>

typedef struct s_DisplayPlugin DisplayPlugin;

struct s_DisplayPlugin
{
        Plugin *plugin;
        bool (*display_canvas)(Canvas *canvas);
};

DisplayPlugin *display_plugin_create (const char *path);
void display_plugin_destroy (DisplayPlugin *display_plugin);

#endif
