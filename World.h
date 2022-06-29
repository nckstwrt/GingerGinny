#ifndef _WORLD_H
#define _WORLD_H

#include "SDLGame.h"
#include "Monster.h"
#include "Tile.h"
#include "Helper.h"
#include "AStar.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>
using namespace std;

#define TILE_SIZE 16
#define MAX_TILE_PER_SQUARE 3
#define GINNY_COLOR SDLColor(198, 56, 0)

class Monster;

class World
{
public:
    World(SDLGame* pGame);
    ~World();

    void FreeTileMap();
    void LoadMap(const char* szMapFile);
    Tile* SafeGetTile(int x, int y, int i);
    void AddMonsterTemplate(const Monster& monsterToCopy, ALIGNMENT alignment = ALIGNMENT::BAD);
    shared_ptr<Monster> AddMonster(const Monster &monsterToCopy, int x, int y, bool facingRight, ALIGNMENT alignment);
    void Update();
    void DrawTiles(bool bottomTiles);
    void Draw();
    int TileXToDisplayPixelX(int tileX);
    int TileYToDisplayPixelY(int tileY);
    int PixelXToDisplayPixelX(int pixelX);
    int PixelYToDisplayPixelY(int pixelY);
    bool MonsterMove(shared_ptr<Monster> pMonster, DIRECTION direction, bool facingChange = true, bool alwaysMoveBack = false);
    vector<Point> MonsterMoveTo(shared_ptr<Monster> pMonster, int x, int y);
    void MonsterAttack(shared_ptr<Monster> pMonster);
    int DrawTextBox(TTF_Font* font, const char* szText, SDLColor textColor = SDLColor(40, 40, 40));

    AStar::Generator pathFinder;
    int offsetX;
    int offsetY;
    SDLGame* pGame;
    vector<shared_ptr<Monster>> monsters;
    vector<shared_ptr<Monster>> monsterTemplates;
    Tile ***tileMap;
    int tileMapWidth;
    int tileMapHeight;
    shared_ptr<Monster> pCameraFollow;

    vector<Point> debugPoints;
    vector<Rect> debugRects;
};

#endif