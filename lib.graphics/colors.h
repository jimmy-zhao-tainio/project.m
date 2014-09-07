#ifndef lib_graphics_colors_h
#define lib_graphics_colors_h

#include <lib.graphics/structures.h>

extern Color Black;
extern Color White;
extern Color Red;
extern Color Green;
extern Color Blue;

#define color_equals(c1,c2) ((c1).red == (c2).red && (c1).green == (c2).green && (c1).blue == (c2).blue) 

#endif
