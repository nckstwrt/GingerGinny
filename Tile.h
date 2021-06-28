#ifndef _TILE_H
#define _TILE_H
#include <SDL/SDL.h>

class Tile
{
public:
    Tile(int x, int y) : x(x), y(y), image(NULL)
    {
    }
    int x;
    int y;
    SDL_Surface* image;
};

#endif