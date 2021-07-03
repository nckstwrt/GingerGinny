#ifndef _SDLCOLOR_H
#define _SDLCOLOR_H

#include <SDL/SDL.h>

class SDLColor
{
public:
    SDLColor();
    SDLColor(int r, int g, int b);

    operator SDL_Color()
    {
        return color;
    }

    operator Uint32()
    {
        return *(Uint32*)&color;
    }

    SDL_Color color;
};

#endif