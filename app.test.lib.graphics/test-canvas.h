#ifndef test_canvas_h
#define test_canvas_h

#include <lib.test/test.h>

bool test_canvas_create_invalid_argument_1 (Test *test);
bool test_canvas_create_invalid_argument_2 (Test *test);
bool test_canvas_create_overflow_1 (Test *test);
bool test_canvas_create_overflow_2 (Test *test);
bool test_canvas_create_function_call_1 (Test *test);
bool test_canvas_create_function_call_2 (Test *test);
bool test_canvas_create (Test *test);
bool test_canvas_destroy_invalid_argument (Test *test);
bool test_canvas_draw_color_invalid_argument (Test *test);
bool test_canvas_draw_color_out_of_range_1 (Test *test);
bool test_canvas_draw_color_out_of_range_2 (Test *test);
bool test_canvas_draw_color (Test *test);
bool test_canvas_draw_image_invalid_argument (Test *test);
bool test_canvas_draw_image_overflow_1 (Test *test);
bool test_canvas_draw_image_overflow_2 (Test *test);
bool test_canvas_draw_image_out_of_range_1 (Test *test);
bool test_canvas_draw_image_out_of_range_2 (Test *test);
bool test_canvas_draw_image_1 (Test *test);
bool test_canvas_draw_image_2 (Test *test);
bool test_canvas_fill_with_color_invalid_argument (Test *test);
bool test_canvas_fill_with_color_1 (Test *test);
bool test_canvas_fill_with_color_2 (Test *test);
bool test_canvas_fill_with_color_3 (Test *test);
bool test_canvas_fill_with_color_4 (Test *test);
bool test_canvas_fill_with_color_5 (Test *test);
bool test_canvas_fill_with_color_6 (Test *test);
bool test_canvas_fill_with_image_invalid_argument (Test *test);
bool test_canvas_fill_with_image_handled_overflow_1 (Test *test);
bool test_canvas_fill_with_image_handled_overflow_2 (Test *test);
bool test_canvas_fill_with_image_1 (Test *test);
bool test_canvas_fill_with_image_2 (Test *test);
bool test_canvas_fill_with_image_3 (Test *test);
bool test_canvas_fill_with_image_4 (Test *test);
bool test_canvas_fill_with_image_5 (Test *test);
bool test_canvas_fill_rectangle_with_color_invalid_argument (Test *test);
bool test_canvas_fill_rectangle_with_color_overflow_1 (Test *test);
bool test_canvas_fill_rectangle_with_color_overflow_2 (Test *test);
bool test_canvas_fill_rectangle_with_color_out_of_range_1 (Test *test);
bool test_canvas_fill_rectangle_with_color_out_of_range_2 (Test *test);
bool test_canvas_fill_rectangle_with_color_handled_overflow (Test *test);
bool test_canvas_fill_rectangle_with_color_1 (Test *test);
bool test_canvas_fill_rectangle_with_color_2 (Test *test);
bool test_canvas_fill_rectangle_with_color_3 (Test *test);
bool test_canvas_fill_rectangle_with_color_4 (Test *test);
bool test_canvas_fill_rectangle_with_color_5 (Test *test);
bool test_canvas_fill_rectangle_with_image_invalid_argument (Test *test);
bool test_canvas_fill_rectangle_with_image_overflow_1 (Test *test);
bool test_canvas_fill_rectangle_with_image_overflow_2 (Test *test);
bool test_canvas_fill_rectangle_with_image_out_of_range_1 (Test *test);
bool test_canvas_fill_rectangle_with_image_out_of_range_2 (Test *test);
bool test_canvas_fill_rectangle_with_image_1 (Test *test);
bool test_canvas_fill_rectangle_with_image_2 (Test *test);
bool test_canvas_fill_rectangle_with_image_3 (Test *test);

#endif
