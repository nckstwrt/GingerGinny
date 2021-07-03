#ifndef _WORLD_H
#define _WORLD_H

#include "SDLGame.h"
#include "Monster.h"
#include "Tile.h"
#include <vector>
using namespace std;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define TILE_SIZE 16
#define MAX_TILE_PER_SQUARE 3

class World
{
public:
    World(SDLGame* pGame);
    ~World();

    void FreeTileMap();
    void LoadMap(const char* szMapFile);
    Tile* SafeGetTile(int x, int y, int i);
    Monster* AddMonster(Monster newMonster, int x, int y, bool facingRight);
    void Update();
    void Draw();
    int TileXToDisplayPixelX(int tileX);
    int TileYToDisplayPixelY(int tileY);
    int PixelXToDisplayPixelX(int pixelX);
    int PixelYToDisplayPixelY(int pixelY);
    void MonsterMove(Monster* pMonster, DIRECTION direction);
    void MonsterAttack(Monster* pMonster);
    int DrawTextBox(TTF_Font* font, const char* szText);

    int offsetX;
    int offsetY;
    SDLGame* pGame;
    vector<Monster> monsters;
    Tile ***tileMap;
    int tileMapWidth;
    int tileMapHeight;
    Monster* pCameraFollow;
};

#endif