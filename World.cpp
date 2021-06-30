#include "World.h"

World::World(SDLGame* pGame) :
    pGame(pGame),
    offsetX(0),
    offsetY(16),
    tileMap(NULL),
    tileMapWidth(0),
    tileMapHeight(0)
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
            tileMap[iter->y][iter->x] = new Tile[3]();
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
}

Monster* World::AddMonster(Monster newMonster, int tileX, int tileY, bool facingRight)
{
    newMonster.x = tileX;
    newMonster.y = tileY;
    newMonster.facingRight = facingRight;
    //newMonster.AI = true;
    monsters.push_back(newMonster);
    return &monsters.back();
}

void World::Update()
{
    for (vector<Monster>::iterator iter = monsters.begin(); iter != monsters.end(); iter++)
    {
        iter->Update();
    }
}

void World::Draw()
{
    // Draw Map
    int startTileX = offsetX / TILE_SIZE;
    int startTileY = offsetY / TILE_SIZE;
    for (int y = 0; y < (SCREEN_HEIGHT / TILE_SIZE); y++)
    {
        for (int x = 0; x < (SCREEN_WIDTH / TILE_SIZE); x++)
        {
            if (startTileY + y < tileMapHeight && startTileX + x < tileMapWidth)
            {
                if (tileMap[startTileY + y][startTileX + x] != NULL)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        Tile* pTile = &tileMap[startTileY + y][startTileX + x][i];
                        if (pTile != NULL && pTile->image != NULL)
                        {
                            int pixelX = TileXToDisplayPixelX(pTile->x);
                            int pixelY = TileYToDisplayPixelY(pTile->y);
                            //if (pixelX >= 0 && pixelX < SCREEN_WIDTH && pixelY >= 0 && pixelY < SCREEN_HEIGHT)
                                pGame->BlitImage(pTile->image, pixelX, pixelY);
                        }
                        else
                            break;
                    }
                }
            }
        }
    }
    /*
    for (vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end(); iter++)
    {
        int pixelX = TileXToDisplayPixelX(iter->x);
        int pixelY = TileYToDisplayPixelY(iter->y);
        if (pixelX >= 0 && pixelX < SCREEN_WIDTH && pixelY >= 0 && pixelY < SCREEN_HEIGHT)
            pGame->BlitImage(iter->image, pixelX, pixelY);
    }*/

    // Draw monsters (skipping the first one, which is Ginny)
    for (vector<Monster>::iterator iter = monsters.begin()+1; iter != monsters.end(); iter++)
    {
        pGame->BlitImage(iter->GetCurrentFrame(), PixelXToDisplayPixelX(iter->x), PixelYToDisplayPixelY(iter->y));
    }

    // Draw Ginny last so she's always on top
    pGame->BlitImage(monsters[0].GetCurrentFrame(), PixelXToDisplayPixelX(monsters[0].x), PixelYToDisplayPixelY(monsters[0].y));
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
    
    bool moveBack = false;
    int monsterXTile = pMonster->x / TILE_SIZE;
    int monsterYTile = pMonster->y / TILE_SIZE;
    if (monsterXTile >= 0 && monsterYTile >= 0 && 
        monsterXTile < tileMapWidth && monsterYTile < tileMapHeight)
    {
        // Check Tile (Y and X are supposed to be the wrong way round)
        if (tileMap[monsterYTile][monsterXTile] != NULL && tileMap[monsterYTile][monsterXTile][0].tileType == TILE_TYPE::EMPTY)
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
