#include "SDLColor.h"

SDLColor::SDLColor()
{
    color = { .r = 0, .g = 0, .b = 0, .unused = 255 };
}

SDLColor::SDLColor(int r, int g, int b)
{
    color = { .r = (Uint8)r, .g = (Uint8)g, .b = (Uint8)b, .unused = 255 };
}
