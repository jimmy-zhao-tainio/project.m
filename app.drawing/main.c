#include <lib.core/error.h>
#include <lib.core/random.h>
#include <lib.app/arguments.h>
#include <lib.graphics/graphics.h>
#include <lib.display.plugin/display-plugin.h>

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

static bool try (uint64_t width, uint64_t height, Canvas **canvas, DisplayPlugin **display_plugin)
{
        uint64_t x, y;
        unsigned char rgb[3];
        uint64_t count;

        if (!random_open ()) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(*canvas = canvas_create (size_value (width, height)))) {
                error_code (FunctionCall, 2);
                return false;
        }
        if (!(*display_plugin = display_plugin_create ("/home/user/project.m/plugin.display.x11/plugin.display.x11.so"))) {
                error_code (FunctionCall, 3);
                return false;
        }
        if (!(*display_plugin)->display_canvas (*canvas)) {
                error_code (FunctionCall, 4);
                return false;
        }
        for (count = 0; count < 10000; count++) {
                if (!random_value (0, width - 1, (unsigned long long *)&x) ||
                    !random_value (0, height - 1, (unsigned long long *)&y) ||
                    !random_bytes (rgb, 3)) {
                        error_code (FunctionCall, 5);
                        return false;
                }
                canvas_draw_color (*canvas, 
                                   position_value (x, y), 
                                   color_value ((uint8_t)rgb[0], (uint8_t)rgb[1], (uint8_t)rgb[2]));
        }
	return true;
}
