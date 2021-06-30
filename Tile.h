#ifndef _TILE_H
#define _TILE_H
#include <SDL/SDL.h>

enum class TILE_TYPE : char
{
    EMPTY,
    FLOOR,
    WALL
};

class Tile
{
public:
    Tile() : Tile(0, 0) 
    {
    }
    Tile(int x, int y) : x(x), y(y), image(NULL), tileType(TILE_TYPE::EMPTY)
    {
    }
    int x;
    int y;
    TILE_TYPE tileType;
    SDL_Surface* image;
};

#endif