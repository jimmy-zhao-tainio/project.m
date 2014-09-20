#include <lib.graphics/structures.h>

Position position_value (size_t x, size_t y)
{
        Position position = { x, y };
        return position;
}

Size size_value (size_t width, size_t height)
{
        Size size = { width, height };
        return size;
}

Rectangle rectangle_value (size_t x, size_t y, size_t width, size_t height)
{
        Rectangle rectangle = { x, y, width, height };
        return rectangle;
}

Color color_value (uint8_t red, uint8_t green, uint8_t blue)
{
        Color color = { red, green, blue };
        return color;
}
