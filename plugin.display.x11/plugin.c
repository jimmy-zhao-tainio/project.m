#include <plugin.display.x11/plugin.h>
#include <lib.core/error.h>
#include <X11/Xlib.h>

typedef struct
{
        Display *display;
        Window window;
        XEvent event;
        int screen;
} Global;

static Global global;

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
        if (!(global.display = XOpenDisplay (NULL))) {
                error (SystemCall);
                return false;
        }
        global.screen = DefaultScreen (global.display);
        global.window = XCreateSimpleWindow (global.display,
                                             RootWindow (global.display,
                                                         global.screen),
                                             10,
                                             10,
                                             (unsigned int)canvas->image.width,
                                             (unsigned int)canvas->image.height,
                                             1,
                                             BlackPixel (global.display,
                                                         global.screen),
                                             WhitePixel (global.display,
                                                         global.screen));
        XSelectInput (global.display, 
                      global.window, 
                      ExposureMask | KeyPressMask);
        XMapWindow (global.display,
                    global.window);
        while (1) {
                XNextEvent (global.display, &global.event);
                if (global.event.type == Expose) {
                        XFillRectangle (global.display,
                                        global.window,
                                        DefaultGC (global.display,
                                                   global.screen),
                                        0,
                                        0,
                                        (unsigned int)canvas->image.width,
                                        (unsigned int)canvas->image.height);
                }
                if (global.event.type == KeyPress) {
                        break;
                }
        }
        XCloseDisplay (global.display);
        return true;
}
