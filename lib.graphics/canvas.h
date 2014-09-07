#ifndef lib_graphics_canvas_h
#define lib_graphics_canvas_h

#include <lib.graphics/structures.h>

Canvas *canvas_create (Size size);
void canvas_destroy (Canvas *canvas);
void canvas_draw_color (Canvas *canvas, Position position, Color color);
void canvas_draw_image (Canvas *canvas, Position position, Image image);
void canvas_fill_with_color (Canvas *canvas, Color color);
void canvas_fill_with_image (Canvas *canvas, Image image);
void canvas_fill_rectangle_with_color (Canvas *canvas, Rectangle rectangle, Color color);
void canvas_fill_rectangle_with_image (Canvas *canvas, Rectangle rectangle, Image image);

#endif
