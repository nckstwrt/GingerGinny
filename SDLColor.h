#ifndef _SDLCOLOR_H
#define _SDLCOLOR_H

#include <SDL/SDL.h>

class SDLColor
{
public:
    SDLColor();
    SDLColor(int r, int g, int b, int alpha = 255);

    operator SDL_Color()
    {
        return color;
    }

    operator Uint32()
    {
        return *(Uint32*)&color;
    }

    Uint32 ReversedUInt()
    {
        return ((color.r << 24) | (color.g << 16) | (color.b << 8) | color.unused);
    }

    SDL_Color color;
};

#endif