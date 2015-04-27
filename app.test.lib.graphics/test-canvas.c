#include <lib.graphics/graphics.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/defines-private.h>
#include <lib.core/print.h>

#include "test-canvas.h"

bool test_canvas_create_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (canvas_create (size_value (0, 1)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_create_invalid_argument_2 (Test *test)
{
        TITLE ();
        CATCH (canvas_create (size_value (1, 0)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_create_overflow_1 (Test *test)
{
        TITLE ();
        CATCH (canvas_create (size_value (2, (size_t)-1)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_create_overflow_2 (Test *test)
{
        TITLE ();
        CATCH (canvas_create (size_value (2, ((size_t)-1) / 2)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (Canvas) - 1);
        CATCH (canvas_create (size_value (1, 1)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (Canvas) + 
                             sizeof (size_t) + sizeof (Color) - 1);
        CATCH (canvas_create (size_value (1, 1)));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_canvas_create (Test *test)
{
        Canvas *canvas;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 1))));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_destroy_invalid_argument (Test *test)
{
        TITLE ();
        canvas_destroy (NULL);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_draw_pixel_invalid_argument (Test *test)
{
        TITLE ();
        canvas_draw_pixel (NULL, position_value (0, 0), Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_draw_pixel_out_of_range_1 (Test *test)
{
        Canvas *canvas;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_pixel (canvas, position_value (3, 2), Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 1);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_pixel_out_of_range_2 (Test *test)
{
        Canvas *canvas;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_pixel (canvas, position_value (2, 3), Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 2);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_pixel (Test *test)
{
        Canvas *canvas;
        Position position;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        for (i = 0; i < 3 * 3; i++) {
                CATCH (!color_equals (canvas->image.map[i], Black));
        }
        for (position.x = 0; position.x < 3; position.x++) {
                for (position.y = 0; position.y < 3; position.y++) {
                        canvas_draw_pixel (canvas, position, Red);
                }
        }
        for (i = 0; i < 3 * 3; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        CATCH (error_count () != 0);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_invalid_argument (Test *test)
{
        Image image;

        TITLE ();
        canvas_draw_image (NULL, position_value (0, 0), image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_draw_image_overflow_1 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_image (canvas, position_value ((size_t)-1, 0), image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_overflow_2 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_image (canvas, position_value (0, (size_t)-1), image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 2);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_out_of_range_1 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_image (canvas, position_value (1, 0), image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 1);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_out_of_range_2 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
                        { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_image (canvas, position_value (0, 1), image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 2);
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_1 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_draw_image (canvas, position_value (0, 0), image);
        CATCH (error_count () != 0);
        for (i = 0; i < 3 * 3; i++) {
                CATCH (canvas->image.map[i].red   != (3 * i) + 1);
                CATCH (canvas->image.map[i].green != (3 * i) + 2);
                CATCH (canvas->image.map[i].blue  != (3 * i) + 3);
                CATCH (canvas->image.map[i].alpha != 0);
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_draw_image_2 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 1920,
                .height = 1080,
                .map = NULL
        };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1920, 1080))));
        CATCH (!(image.map = memory_create (1920 * 1080 * sizeof (Color))));
        for (i = 0; i < 1920 * 1080; i++) {
                image.map[i].red = (uint8_t)(i + 0);
                image.map[i].green = (uint8_t)(i + 1);
                image.map[i].blue = (uint8_t)(i + 2);
                image.map[i].alpha = (uint8_t)(i + 3);
        }
        canvas_draw_image (canvas, position_value (0, 0), image);
        CATCH (error_count () != 0);
        for (i = 0; i < 1920 * 1080; i++) {
                CATCH (canvas->image.map[i].red != (uint8_t)(i + 0));
                CATCH (canvas->image.map[i].green != (uint8_t)(i + 1));
                CATCH (canvas->image.map[i].blue != (uint8_t)(i + 2));
                CATCH (canvas->image.map[i].alpha != (uint8_t)(i + 3));
        }
        canvas_destroy (canvas);
        memory_destroy (image.map);
        PASS ();
}

bool test_canvas_fill_with_color_invalid_argument (Test *test)
{
        TITLE ();
        canvas_fill_with_color (NULL, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_fill_with_color_1 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 1))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 1; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_color_2 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 33))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 33; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_color_3 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 32 + 32 - 1))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 32 + 32 - 1; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_color_4 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 64))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 64; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_color_5 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1, 997))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 997; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_color_6 (Test *test)
{
        Canvas *canvas;
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1920, 1080))));
        canvas_fill_with_color (canvas, Red);
        for (i = 0; i < 1920 * 1080; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_invalid_argument (Test *test)
{
        Image image;

        TITLE ();
        canvas_fill_with_image (NULL, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_fill_with_image_handled_overflow_1 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 2,
                .height = 2,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, 
                        {  7,  8,  9, 0 }, { 10, 11, 12, 0 },
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 2))));
        size_t_private_max (3);
        canvas_fill_with_image (canvas, image);
        CATCH (!color_equals (canvas->image.map[0], image.map[0]));
        CATCH (!color_equals (canvas->image.map[1], image.map[1]));
        CATCH (!color_equals (canvas->image.map[2], image.map[0]));
        CATCH (!color_equals (canvas->image.map[3], image.map[2]));
        CATCH (!color_equals (canvas->image.map[4], image.map[3]));
        CATCH (!color_equals (canvas->image.map[5], image.map[2]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_handled_overflow_2 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 2,
                .height = 2,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, 
                        {  7,  8,  9, 0 }, { 10, 11, 12, 0 },
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (2, 3))));
        size_t_private_max (3);
        canvas_fill_with_image (canvas, image);
        CATCH (!color_equals (canvas->image.map[0], image.map[0]));
        CATCH (!color_equals (canvas->image.map[1], image.map[1]));
        CATCH (!color_equals (canvas->image.map[2], image.map[2]));
        CATCH (!color_equals (canvas->image.map[3], image.map[3]));
        CATCH (!color_equals (canvas->image.map[4], image.map[0]));
        CATCH (!color_equals (canvas->image.map[5], image.map[1]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_1 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (2, 2))));
        canvas_fill_with_image (canvas, image);
        CATCH (!color_equals (canvas->image.map[0], image.map[0]));
        CATCH (!color_equals (canvas->image.map[1], image.map[1]));
        CATCH (!color_equals (canvas->image.map[2], image.map[3]));
        CATCH (!color_equals (canvas->image.map[3], image.map[4]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_2 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_fill_with_image (canvas, image);
        for (i = 0; i < 3 * 3; i++) {
                CATCH (!color_equals (canvas->image.map[i], image.map[i]));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_3 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (4, 4))));
        canvas_fill_with_image (canvas, image);
        CATCH (!color_equals (canvas->image.map[0], image.map[0]));
        CATCH (!color_equals (canvas->image.map[1], image.map[1]));
        CATCH (!color_equals (canvas->image.map[2], image.map[2]));
        CATCH (!color_equals (canvas->image.map[3], image.map[0]));
        CATCH (!color_equals (canvas->image.map[4], image.map[3]));
        CATCH (!color_equals (canvas->image.map[5], image.map[4]));
        CATCH (!color_equals (canvas->image.map[6], image.map[5]));
        CATCH (!color_equals (canvas->image.map[7], image.map[3]));
        CATCH (!color_equals (canvas->image.map[8], image.map[6]));
        CATCH (!color_equals (canvas->image.map[9], image.map[7]));
        CATCH (!color_equals (canvas->image.map[10], image.map[8]));
        CATCH (!color_equals (canvas->image.map[11], image.map[6]));
        CATCH (!color_equals (canvas->image.map[12], image.map[0]));
        CATCH (!color_equals (canvas->image.map[13], image.map[1]));
        CATCH (!color_equals (canvas->image.map[14], image.map[2]));
        CATCH (!color_equals (canvas->image.map[15], image.map[0]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_4 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t x, y;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (6, 6))));
        canvas_fill_with_image (canvas, image);
        for (x = 0; x < 6; x++) {
                for (y = 0; y < 6; y++) {
                        CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x],
                                              image.map[(y % 3) * 3 + x % 3]));
                }
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_with_image_5 (Test *test)
{
        Canvas *canvas;
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t x, y;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1920, 1080))));
        canvas_fill_with_image (canvas, image);
        for (x = 0; x < 1920; x++) {
                for (y = 0; y < 1080; y++) {
                        CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x],
                                              image.map[(y % 3) * 3 + x % 3]));
                }
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_invalid_argument (Test *test)
{
        Rectangle rectangle;

        TITLE ();
        canvas_fill_rectangle_with_color (NULL, rectangle, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_overflow_1 (Test *test)
{
        Canvas canvas;
        Rectangle rectangle = { .x = 2, .width = 2, .y = 1, .height = 1 };

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_color (&canvas, rectangle, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_overflow_2 (Test *test)
{
        Canvas canvas;
        Rectangle rectangle = { .x = 1, .width = 1, .y = 2, .height = 2 };

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_color (&canvas, rectangle, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_out_of_range_1 (Test *test)
{
        Canvas canvas = { .image = { .width = 2, .height = 2, .map = NULL } };
        Rectangle rectangle = { .x = 1, .width = 2, .y = 0, .height = 2 };

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_color (&canvas, rectangle, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_out_of_range_2 (Test *test)
{
        Canvas canvas = { .image = { .width = 2, .height = 2, .map = NULL } };
        Rectangle rectangle = { .x = 0, .width = 2, .y = 1, .height = 2 };

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_color (&canvas, rectangle, Red);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_handled_overflow (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 0, .width = 33, .y = 0, .height = 1 };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (34, 1))));
        size_t_private_max (34);
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (i = 0; i < 33; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_1 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 0, .width = 16, .y = 0, .height = 16 };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (16, 16))));
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (i = 0; i < 16 * 16; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_2 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 0, .width = 32, .y = 0, .height = 32 };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (32, 32))));
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (i = 0; i < 32 * 32; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_3 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 0, .width = 33, .y = 0, .height = 33 };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (33, 33))));
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (i = 0; i < 33 * 33; i++) {
                CATCH (!color_equals (canvas->image.map[i], Red));
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_4 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 1, .width = 1, .y = 1, .height = 1 };
        size_t i;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (3, 3))));
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (i = 0; i < 3 * 3; i++) {
                if (i == 4) {
                        CATCH (!color_equals (canvas->image.map[i], Red));
                }
                else {
                        CATCH (!color_equals (canvas->image.map[i], Black));
                }
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_color_5 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 1, .width = 1920 - 2, .y = 1, .height = 1080 - 2 };
        size_t x, y;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (1920, 1080))));
        canvas_fill_rectangle_with_color (canvas, rectangle, Red);
        for (x = 0; x < 1920; x++) {
                for (y = 0; y < 1080; y++) {
                        if (x == 0 || x == 1920 - 1 || y == 0 || y == 1080 - 1) {
                                CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x], Black));
                        }
                        else {
                                CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x], Red));
                        }
                }
        }
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_invalid_argument (Test *test)
{
        Rectangle rectangle;
        Image image;

        TITLE ();
        canvas_fill_rectangle_with_image (NULL, rectangle, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_overflow_1 (Test *test)
{
        Canvas canvas;
        Rectangle rectangle = { .x = 2, .width = 2, .y = 1, .height = 1 };
        Image image;

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_image (&canvas, rectangle, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_overflow_2 (Test *test)
{
        Canvas canvas;
        Rectangle rectangle = { .x = 1, .width = 1, .y = 2, .height = 2 };
        Image image;

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_image (&canvas, rectangle, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_out_of_range_1 (Test *test)
{
        Canvas canvas = { .image = { .width = 2, .height = 2, .map = NULL } };
        Rectangle rectangle = { .x = 1, .width = 2, .y = 0, .height = 2 };
        Image image;

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_image (&canvas, rectangle, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_out_of_range_2 (Test *test)
{
        Canvas canvas = { .image = { .width = 2, .height = 2, .map = NULL } };
        Rectangle rectangle = { .x = 0, .width = 2, .y = 1, .height = 2 };
        Image image;

        TITLE ();
        size_t_private_max (3);
        canvas_fill_rectangle_with_image (&canvas, rectangle, image);
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorGraphicsOutOfRange);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_1 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 1, .width = 4, .y = 1, .height = 4 };
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t x, y;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (6, 6))));
        canvas_fill_rectangle_with_image (canvas, rectangle, image);
        for (x = 0; x < 6; x++) {
                for (y = 0; y < 6; y++) {
                        if (x == 0 || x == 5 || y == 0 || y == 5) {
                                CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x], Black));
                        }
                }
        }
        CATCH (!color_equals (canvas->image.map[7], image.map[0]));
        CATCH (!color_equals (canvas->image.map[8], image.map[1]));
        CATCH (!color_equals (canvas->image.map[9], image.map[2]));
        CATCH (!color_equals (canvas->image.map[10], image.map[0]));
        CATCH (!color_equals (canvas->image.map[13], image.map[3]));
        CATCH (!color_equals (canvas->image.map[14], image.map[4]));
        CATCH (!color_equals (canvas->image.map[15], image.map[5]));
        CATCH (!color_equals (canvas->image.map[16], image.map[3]));
        CATCH (!color_equals (canvas->image.map[19], image.map[6]));
        CATCH (!color_equals (canvas->image.map[20], image.map[7]));
        CATCH (!color_equals (canvas->image.map[21], image.map[8]));
        CATCH (!color_equals (canvas->image.map[22], image.map[6]));
        CATCH (!color_equals (canvas->image.map[25], image.map[0]));
        CATCH (!color_equals (canvas->image.map[26], image.map[1]));
        CATCH (!color_equals (canvas->image.map[27], image.map[2]));
        CATCH (!color_equals (canvas->image.map[28], image.map[0]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_2 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 1, .width = 1, .y = 1, .height = 1 };
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, 
                        {  7,  8,  9, 0 }, { 10, 11, 12, 0 },
                }
        };

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (2, 2))));
        canvas_fill_rectangle_with_image (canvas, rectangle, image);
        CATCH (!color_equals (canvas->image.map[0], Black));
        CATCH (!color_equals (canvas->image.map[1], Black));
        CATCH (!color_equals (canvas->image.map[2], Black));
        CATCH (!color_equals (canvas->image.map[3], image.map[0]));
        canvas_destroy (canvas);
        PASS ();
}

bool test_canvas_fill_rectangle_with_image_3 (Test *test)
{
        Canvas *canvas;
        Rectangle rectangle = { .x = 0, .width = 4, .y = 0, .height = 4 };
        Image image = { 
                .width = 3,
                .height = 3,
                .map = (Color[]) {
                        {  1,  2,  3, 0 }, {  4,  5,  6, 0 }, {  7,  8,  9, 0 },
                        { 10, 11, 12, 0 }, { 13, 14, 15, 0 }, { 16, 17, 18, 0 },
                        { 19, 20, 21, 0 }, { 22, 23, 24, 0 }, { 25, 26, 27, 0 }
                }
        };
        size_t x, y;

        TITLE ();
        CATCH (!(canvas = canvas_create (size_value (4, 4))));
        size_t_private_max (4);
        canvas_fill_rectangle_with_image (canvas, rectangle, image);
        for (x = 0; x < 4; x++) {
                for (y = 0; y < 4; y++) {
                        CATCH (!color_equals (canvas->image.map[(y * canvas->image.width) + x],
                                              image.map[(y % 3) * 3 + x % 3]));
                }
        }
        canvas_destroy (canvas);
        PASS ();
}
