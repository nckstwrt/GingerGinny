#ifndef _WORLD_H
#define _WORLD_H

#include "SDLGame.h"
#include "Monster.h"
#include "Tile.h"
#include <vector>
using namespace std;

class World
{
public:
    World(SDLGame* pGame);
    void LoadMap(const char* szMapFile);
    void AddMonster(Monster newMonster, int x, int y, bool facingRight);
    void Update();
    void Draw();
    int TileXToPixelX(int tileX);
    int TileYToPixelY(int tileY);

    int offsetX;
    int offsetY;
    SDLGame* pGame;
    vector<Monster> monsters;
    vector<Tile> tiles;
};

#endif