#ifndef _TILE_H
#define _TILE_H
#include <SDL/SDL.h>
#include "Animation.h"

enum class TILE_TYPE : char
{
    EMPTY,
    FLOOR,
    WALL,
    WALL_ALWAYS_ON_TOP
};

class Tile
{
public:
    Tile() : Tile(0, 0) 
    {
    }
    Tile(int x, int y) : x(x), y(y),  tileType(TILE_TYPE::EMPTY)
    {
    }

    void Update()
    {
        if (tileType != TILE_TYPE::EMPTY)
            animation.Increment();
    }

    SDL_Surface* GetCurrentImage()
    {
        if (tileType != TILE_TYPE::EMPTY)
        {
            return animation.CurrentImage();
        }
        else
            return NULL;
    }

    int x;
    int y;
    TILE_TYPE tileType;
    Animation animation;
};

#endif