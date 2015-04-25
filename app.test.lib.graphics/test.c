#include <stdio.h>

#include <lib.test/test.h>

#include "test-canvas.h"

bool (*tests[]) (Test *test) =
{
        &test_canvas_create_invalid_argument_1,
        &test_canvas_create_invalid_argument_2,
        &test_canvas_create_overflow_1,
        &test_canvas_create_overflow_2,
        &test_canvas_create_function_call_1,
        &test_canvas_create_function_call_3,
        &test_canvas_create,
        &test_canvas_destroy_invalid_argument,
        &test_canvas_draw_color_invalid_argument,
        &test_canvas_draw_color_out_of_range_1,
        &test_canvas_draw_color_out_of_range_2,
        &test_canvas_draw_color,
        &test_canvas_draw_image_invalid_argument,
        &test_canvas_draw_image_overflow_1,
        &test_canvas_draw_image_overflow_2,
        &test_canvas_draw_image_out_of_range_1,
        &test_canvas_draw_image_out_of_range_2,
        &test_canvas_draw_image_1,
        &test_canvas_draw_image_2,
        &test_canvas_fill_with_color_invalid_argument,
        &test_canvas_fill_with_color_1,
        &test_canvas_fill_with_color_2,
        &test_canvas_fill_with_color_3,
        &test_canvas_fill_with_color_4,
        &test_canvas_fill_with_color_5,
        &test_canvas_fill_with_color_6,
        &test_canvas_fill_with_image_invalid_argument,
        &test_canvas_fill_with_image_handled_overflow_1,
        &test_canvas_fill_with_image_handled_overflow_2,
        &test_canvas_fill_with_image_1,
        &test_canvas_fill_with_image_2,
        &test_canvas_fill_with_image_3,
        &test_canvas_fill_with_image_4,
        &test_canvas_fill_with_image_5,
        &test_canvas_fill_rectangle_with_color_invalid_argument,
        &test_canvas_fill_rectangle_with_color_overflow_1,
        &test_canvas_fill_rectangle_with_color_overflow_2,
        &test_canvas_fill_rectangle_with_color_out_of_range_1,
        &test_canvas_fill_rectangle_with_color_out_of_range_2,
        &test_canvas_fill_rectangle_with_color_handled_overflow,
        &test_canvas_fill_rectangle_with_color_1,
        &test_canvas_fill_rectangle_with_color_2,
        &test_canvas_fill_rectangle_with_color_3,
        &test_canvas_fill_rectangle_with_color_4,
        &test_canvas_fill_rectangle_with_color_5,
        &test_canvas_fill_rectangle_with_image_invalid_argument,
        &test_canvas_fill_rectangle_with_image_overflow_1,
        &test_canvas_fill_rectangle_with_image_overflow_2,
        &test_canvas_fill_rectangle_with_image_out_of_range_1,
        &test_canvas_fill_rectangle_with_image_out_of_range_2,
        &test_canvas_fill_rectangle_with_image_1,
        &test_canvas_fill_rectangle_with_image_2,
        &test_canvas_fill_rectangle_with_image_3,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
