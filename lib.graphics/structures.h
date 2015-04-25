#ifndef lib_graphics_structures_h
#define lib_graphics_structures_h

#include <lib.core/defines.h>
#include <lib.core/threads-lock.h>

typedef struct s_Position  Position;
typedef struct s_Size      Size;
typedef struct s_Rectangle Rectangle;
typedef struct s_Color     Color;
typedef struct s_Image     Image;
typedef struct s_Canvas    Canvas;

struct s_Position
{
        size_t x;
        size_t y;
};

struct s_Size
{
        size_t width;
        size_t height;
};

struct s_Rectangle
{
        size_t x;
        size_t y;
        size_t width;
        size_t height;
};

struct s_Color
{
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
};

struct s_Image
{
        size_t width;
        size_t height;
        Color *map;
};

struct s_Canvas
{
        Image image;
        bool changed;
        Position changed_from;
        Position changed_to;
        ThreadLock lock;
};

Position  position_value  (size_t x, size_t y);
Size      size_value      (size_t width, size_t height);
Rectangle rectangle_value (size_t x, size_t y, size_t width, size_t height);
Color     color_value     (uint8_t red, uint8_t green, uint8_t blue);

#endif
