#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/threads.h>
#include <lib.app/arguments.h>
#include <lib.app/events.h>
#include <lib.graphics/graphics.h>
#include <lib.display.plugin/display-plugin.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static bool try (uint64_t width, uint64_t height, Canvas **canvas, DisplayPlugin **display_plugin);

int main (int argc, char **argv)
{
        Canvas *canvas = NULL;
        DisplayPlugin *display_plugin = NULL;
        uint64_t width, height;
        bool success;
        AppArgument arguments[] = {
                ARGUMENT_ORDINAL_UINT64 (0, true, &width, "Width."),
                ARGUMENT_ORDINAL_UINT64 (0, true, &height, "Height."),
                ARGUMENT_DEFAULT,
                ARGUMENT_END
        };
        
        if (!app_arguments (argc, argv, arguments)) {
                app_arguments_usage (argc, argv, arguments);
                error (FunctionCall);
                return EXIT_FAILURE;
        }
        success = try (width, height, &canvas, &display_plugin);
        if (display_plugin) {
                display_plugin_destroy (display_plugin);
        }
        if (canvas) {
                canvas_destroy (canvas);
        }
        if (random_is_open ()) {
                random_close ();
        }
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void worker (Thread *worker);

static bool try (uint64_t width, uint64_t height, Canvas **canvas, DisplayPlugin **display_plugin)
{
        Thread *thread;
        bool success;

        if (!random_open ()) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(*canvas = canvas_create (size_value (width, height)))) {
                error_code (FunctionCall, 2);
                return false;
        }
        if (!(*display_plugin = display_plugin_create ("/home/user/project.m/plugin.display.sdl/plugin.display.sdl.so"))) {
                error_code (FunctionCall, 3);
                return false;
        }
        if (!(*display_plugin)->display_canvas (*canvas)) {
                error_code (FunctionCall, 4);
                return false;
        }
        if (!(thread = thread_create (&worker, *canvas))) {
                error_code (FunctionCall, 5);
                return false;
        }
        success = app_events ();
        if (!thread_set_exit (thread)) {
                error_code (FunctionCall, 6);
                return false;
        }
        if (!thread_wait (thread)) {
                error_code (FunctionCall, 7);
                return false;
        }
        thread_destroy (thread);
        return success;
}

static void worker (Thread *thread)
{
        Canvas *canvas = thread->argument;
        Image image = { .width = ((uint8_t)-1) * 3 < canvas->image.width ? 
                                 ((uint8_t)-1) * 3 : canvas->image.width,
                        .height = 1,
                        .map = canvas->image.map };
        unsigned long long value;
        unsigned int x;
        uint8_t r, g, b, s;
        uint8_t ri, gi, bi;

        random_value (0, (uint8_t)-1, &value);
        ri = (uint8_t)value;
        random_value (0, (uint8_t)-1, &value);
        gi = (uint8_t)value;
        random_value (0, (uint8_t)-1, &value);
        bi = (uint8_t)value;
        while (!thread_get_exit (thread)) {
                canvas_lock (canvas);
                r = ri, g = gi, b = bi;
                ri = (uint8_t)(ri - 3);
                gi = (uint8_t)(gi + 1);
                bi = (uint8_t)(bi + 5);
                for (x = 0, s = 0; x < image.width; x++, s++) {
                        canvas_draw_pixel (canvas, position_value (x, 0), color_value (r, g, b));
                        switch (s) {
                                case 3: s = 0;
                                case 0: r++; break;
                                case 1: g++; break;
                                case 2: b++; break;
                        }
                }
                canvas_fill_with_image (canvas, image);
                canvas_unlock (canvas);
                usleep (1000000 / 40);
        }
        return;
}
