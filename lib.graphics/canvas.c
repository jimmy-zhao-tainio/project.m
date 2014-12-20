#include <lib.graphics/canvas.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <string.h>

Canvas *canvas_create (Size size)
{
        Canvas *canvas;

        if (size.width == 0) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (size.height == 0) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!size_t_mul (size.width, size.height, NULL)) {
                error_code (Overflow, 1);
                return NULL;
        }
        if (!size_t_mul (size.width * size.height, sizeof (Color), NULL)) {
                error_code (Overflow, 2);
                return NULL;
        }
        if (!(canvas = memory_create (sizeof (Canvas)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(canvas->image.map = memory_create (size.width * size.height * sizeof (Color)))) {
                memory_destroy (canvas);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(canvas->changes = index_create (size.width * size.height))) {
                memory_destroy (canvas->image.map);
                memory_destroy (canvas);
                error_code (FunctionCall, 3);
                return NULL;
        }
        canvas->image.width = size.width;
        canvas->image.height = size.height;
        return canvas;
}

void canvas_destroy (Canvas *canvas)
{
        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        if (canvas->image.map) {
                memory_destroy (canvas->image.map);
        }
        if (canvas->changes) {
                index_destroy (canvas->changes);
        }
        memory_destroy (canvas);
}

void canvas_draw_color (Canvas *canvas, Position position, Color color)
{
        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        if (position.x >= canvas->image.width) {
                error_code (GraphicsOutOfRange, 1);
                return;
        }
        if (position.y >= canvas->image.height) {
                error_code (GraphicsOutOfRange, 2);
                return;
        }
        canvas->image.map[(position.y * canvas->image.width) + position.x] = color;
}

void canvas_draw_image (Canvas *canvas, Position position, Image image)
{
        size_t right;
        size_t bottom;
        size_t row;

        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        if (!size_t_add (position.x, image.width, &right)) {
                error_code (Overflow, 1);
                return;
        }
        if (!size_t_add (position.y, image.height, &bottom)) {
                error_code (Overflow, 2);
                return;
        }
        if (canvas->image.width < right) {
                error_code (GraphicsOutOfRange, 1);
                return;
        }
        if (canvas->image.height < bottom) {
                error_code (GraphicsOutOfRange, 2);
                return;
        }
        for (row = 0; row < image.height; row++) {
                memcpy (&canvas->image.map[(position.y * canvas->image.width) + position.x + 
                                           (row * canvas->image.width)],
                        &image.map[row * image.width],
                        image.width * sizeof (Color));
        }
}

void canvas_fill_with_color (Canvas *canvas, Color color)
{
        size_t size;
        size_t amount = 32;
        size_t i;
        size_t doubled;

        (void)color;
        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        size = canvas->image.width * canvas->image.height;
        /* Manually copy 32 pixels horizontally. */
        for (i = 0; i < size && i < amount; i++) {
                canvas->image.map[i] = color;
        }
        /* Copy previous pixels. */
        for (; i < size; i += amount) {
                if (size_t_add (i, i, &doubled)) {
                        amount = doubled < size ? i : size - i;
                }
                else {
                        amount = size - i;
                }
                memmove (&canvas->image.map[i], 
                         &canvas->image.map[0], 
                         amount * sizeof (Color));
        }
}

void canvas_fill_with_image (Canvas *canvas, Image image)
{
        size_t width, height;
        size_t x, y, i;
        size_t amount;
        size_t size;
        size_t doubled;

        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        width = canvas->image.width < image.width ? canvas->image.width : image.width;
        height = canvas->image.height < image.height ? canvas->image.height : image.height;
        // Copy image to canvas top corner.
        for (y = 0; y < height; y++) {
                memcpy (&canvas->image.map[y * canvas->image.width], 
                        &image.map[y * image.width], 
                        width * sizeof (Color));
        }
        // Tile horizontally.
        for (x = width; x < canvas->image.width; x += amount) {
                if (size_t_add (x, x, &doubled)) {
                        amount = doubled < canvas->image.width ? x : canvas->image.width - x;
                }
                else {
                        amount = canvas->image.width - x;
                }
                for (y = 0; y < height; y++) {
                        memmove (&canvas->image.map[(y * canvas->image.width) + x], 
                                 &canvas->image.map[(y * canvas->image.width)], 
                                 amount * sizeof (Color));
                }
        }
        // Tile vertically.
        size = canvas->image.width * canvas->image.height;
        for (i = canvas->image.width * height; 
             i < canvas->image.width * canvas->image.height; 
             i += amount) {
                if (size_t_add (i, i, &doubled)) {
                        amount = doubled < size ? i : size - i;
                }
                else {
                        amount = size - i;
                }
                memmove (&canvas->image.map[i],
                         &canvas->image.map[0],
                         amount * sizeof (Color));
        }
}

void canvas_fill_rectangle_with_color (Canvas *canvas, Rectangle rectangle, Color color)
{
        size_t amount = 32;
        size_t i;
        size_t doubled;
        size_t right;
        size_t bottom;

        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        if (!size_t_add (rectangle.x, rectangle.width, &right)) {
                error_code (Overflow, 1);
                return;
        }
        if (!size_t_add (rectangle.y, rectangle.height, &bottom)) {
                error_code (Overflow, 2);
                return;
        }
        if (canvas->image.width < right) {
                error_code (GraphicsOutOfRange, 1);
                return;
        }
        if (canvas->image.height < bottom) {
                error_code (GraphicsOutOfRange, 2);
                return;
        }
        /* Manually copy 32 pixels horizontally */
        for (i = 0; i < rectangle.width && i < amount; i++) {
                canvas->image.map[(rectangle.y * canvas->image.width) + rectangle.x + i] = color;
        }
        /* Copy pixels horizontally until full. */
        for (; i < rectangle.width; i += amount) {
                if (size_t_add (i, i, &doubled)) {
                        amount = i + i < rectangle.width ? i : rectangle.width - i;
                }
                else {
                        amount = rectangle.width - i;
                }
                memmove (&canvas->image.map[(rectangle.y * canvas->image.width) + rectangle.x + i], 
                         &canvas->image.map[(rectangle.y * canvas->image.width) + rectangle.x], 
                         amount * sizeof (Color));
        }
        /* Copy vertically. */
        for (i = 0; i < rectangle.height; i++) {
                memmove (&canvas->image.map[(rectangle.y * canvas->image.width) + rectangle.x + (i * canvas->image.width)],
                         &canvas->image.map[(rectangle.y * canvas->image.width) + rectangle.x],
                         rectangle.width * sizeof (Color));
        }
}

void canvas_fill_rectangle_with_image (Canvas *canvas, Rectangle rectangle, Image image)
{
        size_t width, height;
        size_t x, y;
        size_t amount;
        size_t doubled;
        size_t right;
        size_t bottom;

        if (!canvas) {
                error (InvalidArgument);
                return;
        }
        if (!size_t_add (rectangle.x, rectangle.width, &right)) {
                error_code (Overflow, 1);
                return;
        }
        if (!size_t_add (rectangle.y, rectangle.height, &bottom)) {
                error_code (Overflow, 2);
                return;
        }
        if (canvas->image.width < right) {
                error_code (GraphicsOutOfRange, 1);
                return;
        }
        if (canvas->image.height < bottom) {
                error_code (GraphicsOutOfRange, 2);
                return;
        }
        width = rectangle.width < image.width ? rectangle.width : image.width;
        height = rectangle.height < image.height ? rectangle.height : image.height;
        // Copy image to rectangle top corner.
        for (y = 0; y < height; y++) {
                memcpy (&canvas->image.map[((y + rectangle.y) * canvas->image.width) + rectangle.x],
                        &image.map[y * image.width],
                        width * sizeof (Color));
        }
        // Tile horizontally.
        for (x = width; x < rectangle.width; x += amount) {
                if (size_t_add (x, x, &doubled)) {
                        amount = doubled < rectangle.width ? x : rectangle.width - x;
                }
                else {
                        amount = rectangle.width - x;
                }
                for (y = 0; y < height; y++) {
                        memmove (&canvas->image.map[((y + rectangle.y) * canvas->image.width) + rectangle.x + x], 
                                 &canvas->image.map[((y + rectangle.y) * canvas->image.width) + rectangle.x], 
                                 amount * sizeof (Color));
                }
        }
        // Tile vertically.
        for (y = rectangle.y; y + height < rectangle.y + rectangle.height; y++) {
                memmove (&canvas->image.map[((y + height) * canvas->image.width) + rectangle.x],
                         &canvas->image.map[(y * canvas->image.width) + rectangle.x],
                         rectangle.width * sizeof (Color));
        }
}

/*
static void lock_write_enter (Canvas *canvas, Rectangle rectangle)
{
        // Lock the region by rectangle.
        (void)canvas;
        (void)rectangle;
}

static void lock_write_leave (Canvas *canvas)
{
        (void)canvas;
}*/
