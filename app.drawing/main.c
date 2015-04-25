#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.core/threads.h>
#include <lib.app/arguments.h>
#include <lib.app/events.h>
#include <lib.graphics/graphics.h>
#include <lib.display.plugin/display-plugin.h>
#include <unistd.h>
#include <stdio.h>

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
        unsigned int x, y;

        Canvas *canvas = thread->argument;
        
        canvas_fill_with_color (canvas, White);
        x = 0;
        y = 0;
        while (!thread_get_exit (thread)) {
                canvas_draw_color (canvas, position_value (x, y), 
                                   color_value ((uint8_t)((x) * (y + y)), 
                                                (uint8_t)((y) * (x - y)), 
                                                (uint8_t)((x) * (x + y))
                                                ));
                x++;
                if (x == canvas->image.width) {
                        x = 0;
                        y++;
                }
                if (y == canvas->image.height) {
                        x = 0;
                        y = 0;
                        break;
                }
        }
        thread_exit (thread);
}
