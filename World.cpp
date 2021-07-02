#include "World.h"

World::World(SDLGame* pGame) :
    pGame(pGame),
    offsetX(0),
    offsetY(0),
    tileMap(NULL),
    tileMapWidth(0),
    tileMapHeight(0),
    pCameraFollow(NULL)
{
}

World::~World()
{
    FreeTileMap();
}

void World::FreeTileMap()
{
    if (tileMap)
    {
        for (int y = 0; y < tileMapHeight; y++)
        {
            for (int x = 0; x < tileMapWidth; x++)
            {
                delete[] tileMap[y][x];
            }
            delete[] tileMap[y];
        }
        delete[] tileMap;
        tileMap = NULL;
    }
}

void World::LoadMap(const char* szMapFile)
{
    vector<Tile> tiles;
    char szImage[100];
    int x, y;
    tileMapWidth = tileMapHeight = 0;
    FILE* f = fopen(szMapFile, "rt");
    while (true)
    {
        if (fscanf(f, "%d %d %s", &x, &y, szImage) != 3)
            break;
        if (x > tileMapWidth)
            tileMapWidth = x;
        if (y > tileMapHeight)
            tileMapHeight = y;
        Tile tile(x, y);
        tile.image = pGame->GetLoadedImage(szImage);
        tile.tileType = (strstr(szImage, "wall") == NULL) ? TILE_TYPE::FLOOR : TILE_TYPE::WALL;

        if (tile.image == NULL)
        {
            string tileDirectory = "images/DungeonTilesetII_v1.4/";
            tile.image = pGame->LoadImage((tileDirectory + szImage).c_str());
        }
        tiles.push_back(tile);
    }
    fclose(f);

    // Now set up the tileMap for easy access
    tileMapWidth++;
    tileMapHeight++;
    tileMap = new Tile**[tileMapHeight]();
    for (int y = 0; y < tileMapHeight; y++)
        tileMap[y] = new Tile*[tileMapWidth]();
    for (vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end(); iter++)
    {
        if (tileMap[iter->y][iter->x] == NULL)
        {
            tileMap[iter->y][iter->x] = new Tile[MAX_TILE_PER_SQUARE]();
            tileMap[iter->y][iter->x][0] = *iter;
        }
        else
        {
            if (tileMap[iter->y][iter->x][1].tileType == TILE_TYPE::EMPTY)
                tileMap[iter->y][iter->x][1] = *iter;
            else
                tileMap[iter->y][iter->x][2] = *iter;
        }
    }

    // Now force some walls to be on top
    for (int y = 0; y < tileMapHeight; y++)
    {
        for (int x = 0; x < tileMapWidth; x++)
        {
            for (int i = 0; i < MAX_TILE_PER_SQUARE; i++)
            {
                if (SafeGetTile(x, y, i)->tileType == TILE_TYPE::WALL &&
                    (SafeGetTile(x + 1, y + 2, 0)->tileType == TILE_TYPE::EMPTY || SafeGetTile(x - 1, y + 2, 0)->tileType == TILE_TYPE::EMPTY) &&
                    SafeGetTile(x, y - 1, 0)->tileType == TILE_TYPE::FLOOR)
                    {
                        SafeGetTile(x, y, i)->tileType = TILE_TYPE::WALL_ALWAYS_ON_TOP;
                        //SafeGetTile(x, y - 1, 0)->tileType = TILE_TYPE::DEBUG;
                    }
            }
        }
    }
}

Tile* World::SafeGetTile(int x, int y, int i)
{
    static Tile staticTile;
    Tile* ret = &staticTile;

    if (x >= 0 && x < tileMapWidth && y >= 0 && y < tileMapHeight && i < MAX_TILE_PER_SQUARE)
    {
        if (tileMap[y][x] != NULL)
        {
            ret = &tileMap[y][x][i];
        }
    }

    return ret;
}

Monster* World::AddMonster(Monster newMonster, int tileX, int tileY, bool facingRight)
{
    newMonster.x = tileX;
    newMonster.y = tileY;
    newMonster.facingRight = facingRight;
    //newMonster.AI = true;
    monsters.push_back(newMonster);
    if (monsters.size() == 1)
        pCameraFollow = &monsters[0];
    return &monsters.back();
}

void World::Update()
{
    // Update all Monsters
    for (vector<Monster>::iterator iter = monsters.begin(); iter != monsters.end(); iter++)
    {
        iter->Update();
    }

    // Update the Viewpoint Offset based on who the camera is following (horizontal)
    if (PixelXToDisplayPixelX(pCameraFollow->x) < (SCREEN_WIDTH / 6))
        offsetX -= (SCREEN_WIDTH / 6) - PixelXToDisplayPixelX(pCameraFollow->x);
    if (PixelXToDisplayPixelX(pCameraFollow->x + pCameraFollow->width) > ((SCREEN_WIDTH / 6) * 5))
        offsetX += PixelXToDisplayPixelX(pCameraFollow->x + pCameraFollow->width) - ((SCREEN_WIDTH / 6) * 5);
    if (offsetX < 0)
        offsetX = 0;

    // Now update for the vertical
    if (PixelYToDisplayPixelY(pCameraFollow->y) < (SCREEN_HEIGHT / 6))
        offsetY -= (SCREEN_HEIGHT / 6) - PixelYToDisplayPixelY(pCameraFollow->y);
    if (PixelYToDisplayPixelY(pCameraFollow->y + pCameraFollow->height) > ((SCREEN_HEIGHT / 6) * 5))
        offsetY += PixelYToDisplayPixelY(pCameraFollow->y + pCameraFollow->height) - ((SCREEN_HEIGHT / 6) * 5);
    if (offsetY < 0)
        offsetY = 0;
}

void World::Draw()
{
    // Draw Map
    int startTileX = offsetX / TILE_SIZE;
    int startTileY = offsetY / TILE_SIZE;
    for (int y = 0; y < (SCREEN_HEIGHT / TILE_SIZE)+1; y++)
    {
        for (int x = 0; x < (SCREEN_WIDTH / TILE_SIZE)+1; x++)
        {
            if (startTileY + y < tileMapHeight && startTileX + x < tileMapWidth)
            {
                if (tileMap[startTileY + y][startTileX + x] != NULL)
                {
                    for (int i = 0; i < MAX_TILE_PER_SQUARE; i++)
                    {
                        Tile* pTile = &tileMap[startTileY + y][startTileX + x][i];
                        if (pTile != NULL && pTile->image != NULL)
                        {
                            int pixelX = TileXToDisplayPixelX(pTile->x);
                            int pixelY = TileYToDisplayPixelY(pTile->y);

                            if (pTile->tileType == TILE_TYPE::DEBUG)
                            {
                                pGame->DrawRect(pixelX, pixelY, 16, 16, 255, 0, 0);
                            }
                            else
                            if ((pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) && pTile->tileType != TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                pGame->BlitImage(pTile->image, pixelX, pixelY);
                        }
                        else
                            break;
                    }
                }
            }
        }
    }

    // Draw monsters (skipping the first one, which is Ginny)
    for (vector<Monster>::iterator iter = monsters.begin()+1; iter != monsters.end(); iter++)
    {
        pGame->BlitImage(iter->GetCurrentFrame(), PixelXToDisplayPixelX(iter->x), PixelYToDisplayPixelY(iter->y));
    }

    // Draw Ginny last so she's always on top of the other monsters
    pGame->BlitImage(monsters[0].GetCurrentFrame(), PixelXToDisplayPixelX(monsters[0].x), PixelYToDisplayPixelY(monsters[0].y));

    // Draw parts of the floor + walls that should be over Ginny
    for (int y = 0; y < (SCREEN_HEIGHT / TILE_SIZE)+1; y++)
    {
        for (int x = 0; x < (SCREEN_WIDTH / TILE_SIZE)+1; x++)
        {
            if (startTileY + y < tileMapHeight && startTileX + x < tileMapWidth)
            {
                if (tileMap[startTileY + y][startTileX + x] != NULL)
                {
                    for (int i = 0; i < MAX_TILE_PER_SQUARE; i++)
                    {
                        Tile* pTile = &tileMap[startTileY + y][startTileX + x][i];
                        if (pTile != NULL && pTile->image != NULL)
                        {
                            int pixelX = TileXToDisplayPixelX(pTile->x);
                            int pixelY = TileYToDisplayPixelY(pTile->y);

                            if (pTile->tileType == TILE_TYPE::DEBUG)
                            {
                                pGame->DrawRect(pixelX, pixelY, 16, 16, 255, 0, 0);
                            }
                            else
                            if (!(pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) || pTile->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                pGame->BlitImage(pTile->image, pixelX, pixelY);
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
}

int World::TileXToDisplayPixelX(int tileX)
{
    return (tileX * TILE_SIZE) - offsetX;
}

int World::TileYToDisplayPixelY(int tileY)
{
    return (tileY * TILE_SIZE) - offsetY;
}

int World::PixelXToDisplayPixelX(int pixelX)
{
    return (pixelX - offsetX);
}

int World::PixelYToDisplayPixelY(int pixelY)
{
    return (pixelY - offsetY);
}

void World::MonsterMove(Monster* pMonster, DIRECTION direction)
{
    int savedX = pMonster->x;
    int savedY = pMonster->y;
    pMonster->Move(direction);

    int monsterX1 = pMonster->x - 4;
    int monsterX2 = pMonster->x + pMonster->width + 6;
    int monsterY = pMonster->y + pMonster->height - 8;
    
    bool moveBack = false;
    int monsterXTile1 = monsterX1 / TILE_SIZE;
    int monsterXTile2 = monsterX2 / TILE_SIZE;
    int monsterYTile = monsterY / TILE_SIZE;
    if (monsterXTile1 >= 0 && monsterXTile2 >= 0 && monsterYTile >= 0 &&
        monsterXTile1 < tileMapWidth && monsterXTile2 < tileMapWidth && monsterYTile < tileMapHeight)
    {
        // Check Tile (Y and X are supposed to be the wrong way round)
        if (tileMap[monsterYTile][monsterXTile1] == NULL || (tileMap[monsterYTile][monsterXTile1] != NULL && tileMap[monsterYTile][monsterXTile1][0].tileType != TILE_TYPE::FLOOR))
            moveBack = true;
        if (tileMap[monsterYTile][monsterXTile2] == NULL || (tileMap[monsterYTile][monsterXTile2] != NULL && tileMap[monsterYTile][monsterXTile2][0].tileType != TILE_TYPE::FLOOR))
            moveBack = true;
    }
    else
        moveBack = true;

    if (moveBack)
    {
        pMonster->x = savedX;
        pMonster->y = savedY;
    }
}

void World::MonsterAttack(Monster* pMonster)
{
    pMonster->Attack();
}
