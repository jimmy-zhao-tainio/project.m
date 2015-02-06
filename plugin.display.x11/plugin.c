#include <plugin.display.x11/plugin.h>
#include <lib.core/error.h>
#include <lib.core/threads.h>
#include <lib.app/events.h>
#include <X11/Xlib.h>
#include <unistd.h>

typedef struct
{
        Display *display;
        Window window;
        XEvent event;
        int screen;
        Thread *thread;
        ThreadSignal signal;
        bool initialized;
        Thread *timer;
} Global;

static Global global = {
        .display = NULL,
        .thread = NULL,
        .signal = THREAD_SIGNAL_INITIALIZER,
        .initialized = false,
        .timer = NULL
};

static void worker (Thread *thread);
static void timer (Thread *thread);

bool load (void)
{
        global.thread = NULL;
        global.timer = NULL;
        return true;
}

bool unload (void)
{
        if (global.thread) {
                thread_set_exit (global.thread);
                thread_wait (global.thread);
                thread_destroy (global.thread);
        }
        if (global.timer) {
                thread_set_exit (global.timer);
                thread_wait (global.timer);
                thread_destroy (global.timer);
        }
        return true;
}

bool display_canvas (Canvas *canvas)
{
        if (!(global.thread = thread_create (&worker, canvas))) {
                error (FunctionCall);
                return false;
        }
        if (!thread_signal_wait (&global.signal)) {
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
        if (!(global.timer = thread_create (&timer, canvas))) {
                XCloseDisplay (global.display);
                error (FunctionCall);
                return;
        }
        global.initialized = true;
        if (!thread_signal (&global.signal)) {
                XCloseDisplay (global.display);
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
        app_event_exit ();
}

static void timer (Thread *thread)
{
        Canvas *canvas = thread->argument;

        while (true) {
                if (thread_get_exit (thread)) {
                        thread_exit (thread);
                }
                usleep (16667);
                if (thread_get_exit (thread)) {
                        return;
                }
                if (!thread_lock (&canvas->lock)) {
                        error (FunctionCall);
                        return;
                }
                /* check canvas->index for changes */
                if (!thread_unlock (&canvas->lock)) {
                        error (FunctionCall);
                        return;
                }
        }
}
