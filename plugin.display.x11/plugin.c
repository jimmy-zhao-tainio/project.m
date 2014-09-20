#include <plugin.display.x11/plugin.h>

bool load (void)
{
        return true;
}

bool unload (void)
{
        return true;
}

bool display_canvas (Canvas *canvas)
{
        (void)canvas;
        return true;
}
