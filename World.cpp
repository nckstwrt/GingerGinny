#include "World.h"

World::World(SDLGame* pGame) :
    pGame(pGame),
    offsetX(0),
    offsetY(1)
{
}

void World::LoadMap(const char* szMapFile)
{
    char szImage[100];
    int x, y;
    FILE* f = fopen(szMapFile, "rt");
    while (true)
    {
        if (fscanf(f, "%d %d %s", &x, &y, szImage) != 3)
            break;
        Tile tile(x, y);
        tile.image = pGame->GetLoadedImage(szImage);
        if (tile.image == NULL)
        {
            string tileDirectory = "images/DungeonTilesetII_v1.4/";
            tile.image = pGame->LoadImage((tileDirectory + szImage).c_str());
        }
        tiles.push_back(tile);
    }
    fclose(f);
}

void World::AddMonster(Monster newMonster, int x, int y, bool facingRight)
{
    newMonster.x = x;
    newMonster.y = y;
    newMonster.facingRight = facingRight;
    newMonster.AI = true;
    monsters.push_back(newMonster);
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
    for (vector<Tile>::iterator iter = tiles.begin(); iter != tiles.end(); iter++)
    {
        int pixelX = TileXToPixelX(iter->x);
        int pixelY = TileYToPixelY(iter->y);
        if (pixelX >= 0 && pixelX < 240 && pixelY >= 0 && pixelY < 240)
            pGame->BlitImage(iter->image, pixelX, pixelY);
    }

    // Draw monsters
    for (vector<Monster>::iterator iter = monsters.begin(); iter != monsters.end(); iter++)
    {
        pGame->BlitImage(iter->GetCurrentFrame(), iter->x, iter->y);
    }
}

int World::TileXToPixelX(int tileX)
{
    return (tileX - offsetX) * 16;
}

int World::TileYToPixelY(int tileY)
{
    return (tileY - offsetY) * 16;
}
