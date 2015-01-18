#include <plugin.display.x11/plugin.h>
#include <lib.core/error.h>
#include <lib.core/threads.h>
#include <X11/Xlib.h>

typedef struct
{
        Display *display;
        Window window;
        XEvent event;
        int screen;
        Thread *thread;
        ThreadSignal signal;
        bool initialized;
} Global;

static Global global;

static void worker (Thread *thread);

bool load (void)
{
        global.thread = NULL;
        
        return true;
}

bool unload (void)
{
        if (global.thread) {
                thread_destroy (global.thread);
        }
        return true;
}

bool display_canvas (Canvas *canvas)
{
        if (!thread_signal_create (&global.signal)) {
                error (FunctionCall);
                return false;
        }
        if (!(global.thread = thread_create (&worker, canvas))) {
                if (!thread_signal_destroy (&global.signal)) {
                        error (FunctionCall);
                }
                error (FunctionCall);
                return false;
        }
        if (!thread_signal_wait (&global.signal)) {
                if (!thread_signal_destroy (&global.signal)) {
                        error (FunctionCall);
                }
                error (FunctionCall);
                return false;
        }
        if (!thread_signal_destroy (&global.signal)) {
                error (FunctionCall);
                return false;
        }
        return global.initialized;
}

static void worker (Thread *thread)
{
        Canvas *canvas = thread->argument;
        bool running = true;
        Atom deleteMessage;

        if (!(global.display = XOpenDisplay (NULL))) {
                global.initialized = false;
                if (!thread_signal (&global.signal)) {
                        error (FunctionCall);
                }
                error (SystemCall);
                return;
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
        deleteMessage = XInternAtom (global.display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols (global.display, global.window, &deleteMessage, 1);
        global.initialized = true;
        if (!thread_signal (&global.signal)) {
                global.initialized = false;
                error (FunctionCall);
                return;
        }
        while (running) {
                XNextEvent (global.display, &global.event);
                switch (global.event.type) {
                case Expose:
                        XFillRectangle (global.display,
                                        global.window,
                                        DefaultGC (global.display,
                                                   global.screen),
                                        0,
                                        0,
                                        (unsigned int)canvas->image.width,
                                        (unsigned int)canvas->image.height);
                        break;
                case KeyPress:
                        running = false;
                        break;
                case ClientMessage:
                        if (global.event.xclient.data.l[0] == (unsigned int)deleteMessage) {
                                running = false;
                        }
                        break;
                }
        }
        XCloseDisplay (global.display);
        return;
}
