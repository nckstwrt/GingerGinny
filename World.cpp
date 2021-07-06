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
    char szImage[100] = { 0 };
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
        tile.tileType = (strstr(szImage, "wall") == NULL) ? TILE_TYPE::FLOOR : TILE_TYPE::WALL;

        // If it's an animation load all 3 frames
        if (strstr(szImage, "_f0"))
        {
            string animImages = szImage;
            int imageSpeed = 15;
            int imageCount = 3;

            if (strstr(szImage, "spikes"))
            {
                imageSpeed = 10;
                imageCount = 4;
            }

            Helper::string_replace(animImages, "_f0", "_f%d");
            for (int i = 0; i < imageCount; i++)
            {
                string imageFile = Helper::string_format(animImages, i);
                tile.animation.AddImage(pGame->GetLoadedImage(imageFile.c_str(), "images/DungeonTilesetII_v1.4"), NULL, imageSpeed);
            }

            if (strstr(szImage, "spikes"))
            {
                tile.animation.SetAnimationDelay(0, 300);
                for (int i = 3; i != 0; i--)
                {
                    string imageFile = Helper::string_format(animImages, i);
                    tile.animation.AddImage(pGame->GetLoadedImage(imageFile.c_str(), "images/DungeonTilesetII_v1.4"), NULL, imageSpeed);
                }
            }
        }
        else
            tile.animation.AddImage(pGame->GetLoadedImage(szImage, "images/DungeonTilesetII_v1.4"), NULL, 100);

        tiles.push_back(tile);
    }
    fclose(f);

    // Now set up the tileMap for easy access
    tileMapWidth++;
    tileMapHeight++;
    tileMap = new Tile * *[tileMapHeight]();
    for (int y = 0; y < tileMapHeight; y++)
        tileMap[y] = new Tile * [tileMapWidth]();
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

    // Now make our pathfinder
    pathFinder.setDiagonalMovement(true);
    pathFinder.setHeuristic(AStar::Heuristic::euclidean);
    pathFinder.setWorldSize({ tileMapWidth, tileMapHeight });

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
                if (i == 0)
                {
                    if (/*SafeGetTile(x, y, 0)->tileType != TILE_TYPE::WALL &&*/ SafeGetTile(x, y, 0)->tileType != TILE_TYPE::FLOOR)
                    {
                        pathFinder.addWall({ x, y });
                    }
                }
                else
                    if (SafeGetTile(x, y, i)->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP)
                        pathFinder.addWall({ x, y });
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

shared_ptr<Monster> World::AddMonster(const Monster &monsterToCopy, int tileX, int tileY, bool facingRight)
{
    shared_ptr<Monster> newMonster = make_shared<Monster>(monsterToCopy);
    newMonster->x = tileX;
    newMonster->y = tileY;
    newMonster->facingRight = facingRight;
    newMonster->pWorld = this;
    //newMonster.AI = true;
    monsters.push_back(newMonster);
    if (monsters.size() == 1)
        pCameraFollow = monsters[0];
    return monsters.back();
}

void World::Update()
{
    // Update all Monsters
    for (auto &monster : monsters)
    {
        // If has directions to go to, move the monster
        if (monster->directions.size() > 0)
        {
            if (MonsterMove(monster, monster->directions.front()))
            {
                if (monster->directions.size() == 1)
                    monster->walking = false;
                monster->directions.pop();
            }
        }

        monster->Update();
    }

    // Sort the monsters by Y order
    sort(monsters.begin(), monsters.end(), [](const shared_ptr<Monster> a, const shared_ptr<Monster> b) -> bool { return a->y+a->height < b->y+b->height; });

    // Update the Viewpoint Offset based on who the camera is following (horizontal)
    if (PixelXToDisplayPixelX(pCameraFollow->x) < (SCREEN_WIDTH / 4))
        offsetX -= (SCREEN_WIDTH / 4) - PixelXToDisplayPixelX(pCameraFollow->x);
    if (PixelXToDisplayPixelX(pCameraFollow->x + pCameraFollow->width) > ((SCREEN_WIDTH / 4) * 3))
        offsetX += PixelXToDisplayPixelX(pCameraFollow->x + pCameraFollow->width) - ((SCREEN_WIDTH / 4) * 3);
    if (offsetX < 0)
        offsetX = 0;

    // Now update for the vertical
    if (PixelYToDisplayPixelY(pCameraFollow->y) < (SCREEN_HEIGHT / 4))
        offsetY -= (SCREEN_HEIGHT / 4) - PixelYToDisplayPixelY(pCameraFollow->y);
    if (PixelYToDisplayPixelY(pCameraFollow->y + pCameraFollow->height) > ((SCREEN_HEIGHT / 4) * 3))
        offsetY += PixelYToDisplayPixelY(pCameraFollow->y + pCameraFollow->height) - ((SCREEN_HEIGHT / 4) * 3);
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
                        if (pTile != NULL && pTile->GetCurrentImage() != NULL)
                        {
                            int pixelX = TileXToDisplayPixelX(pTile->x);
                            int pixelY = TileYToDisplayPixelY(pTile->y);

                            if (pTile->tileType == TILE_TYPE::DEBUG)
                            {
                                pGame->DrawRect(pixelX, pixelY, 16, 16, SDLColor(255, 0, 0));
                            }
                            else
                            if ((pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) && pTile->tileType != TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                pGame->BlitImage(pTile->GetCurrentImage(), pixelX, pixelY);
                        }
                        else
                            break;
                    }
                }
            }
        }
    }

    // Draw monsters
    for (auto &monster : monsters)
    {
        pGame->BlitImage(monster->GetCurrentFrame(), PixelXToDisplayPixelX(monster->x), PixelYToDisplayPixelY(monster->y));
    }

    // Draw parts of the floor + walls that should be over Ginny and the monsters
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
                        if (pTile != NULL && pTile->GetCurrentImage() != NULL)
                        {
                            int pixelX = TileXToDisplayPixelX(pTile->x);
                            int pixelY = TileYToDisplayPixelY(pTile->y);

                            if (pTile->tileType == TILE_TYPE::DEBUG)
                            {
                                pGame->DrawRect(pixelX, pixelY, 16, 16, SDLColor(255, 0, 0));
                            }
                            else
                            if (!(pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) || pTile->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                pGame->BlitImage(pTile->GetCurrentImage(), pixelX, pixelY);
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

bool World::MonsterMove(shared_ptr<Monster> pMonster, DIRECTION direction, bool alwaysMoveBack)
{
    int savedX = pMonster->x;
    int savedY = pMonster->y;
    pMonster->Move(direction);

    int monsterX1 = pMonster->x - 4;
    int monsterX2 = pMonster->x + (pMonster->width + 6);
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

    // Check for monsters
    if (!moveBack)
    {
        for (auto &monster : monsters)
        {
            if (pMonster != monster)
            {
                if (pMonster->CheckOverlap(monster))
                {
                    moveBack = true;
                    break;
                }
            }
        }
    }

    if (moveBack || alwaysMoveBack)
    {
        pMonster->x = savedX;
        pMonster->y = savedY;
    }

    return !moveBack;
}

vector<Point> World::MonsterMoveTo(shared_ptr<Monster> pMonster, int x, int y)
{
    vector<Point> ret;
   
    // Remove previously placed monsters (as they may have removed)
    pathFinder.clearMonsters();
    for (auto& monster : monsters)
    {
        Rect monsterFeet = monster->GetRect(true);
        pathFinder.addMonster({ monsterFeet.x / TILE_SIZE, monsterFeet.y / TILE_SIZE }, { (monsterFeet.x+ monsterFeet.width) / TILE_SIZE, (monsterFeet.y+ monsterFeet.height) / TILE_SIZE });
    }

    // If we are moving to a spot with a monster, remove that as a blocker
    pathFinder.removeMonster({ x / TILE_SIZE, y / TILE_SIZE });

    // Run the A* Search
    auto path = pathFinder.findPath({ (pMonster->x + (pMonster->width / 2)) / TILE_SIZE, ((pMonster->y + pMonster->height) - 5) / TILE_SIZE }, { x / TILE_SIZE, y / TILE_SIZE });
    
    // Move the returned path into the centre of the tiles
    for (auto& p : path)
    {
        ret.push_back(Point((p.x * TILE_SIZE) + TILE_SIZE/2, (p.y * TILE_SIZE) + TILE_SIZE / 2));
    }
    reverse(ret.begin(), ret.end());
    
    //if (ret.size() > 1)
    for (unsigned int i = 0; i < ret.size(); i++)
        pMonster->MoveTo(ret[i].x, ret[i].y, i != 0);

    return ret;
}

void World::MonsterAttack(shared_ptr<Monster> pMonster)
{
    pMonster->Attack();
}

int World::DrawTextBox(TTF_Font* font, const char* szText)
{
    int ret = 0;
    int textBoxHeightStart = SCREEN_HEIGHT - 70;
    int outerIndent = 1;
    int innerIndent = 4;
    SDLColor backGroundColor = SDLColor(200, 200, 200);
    SDLColor triangeColor = SDLColor(30, 30, 30);
    pGame->DrawRect(outerIndent, textBoxHeightStart, SCREEN_WIDTH - (2 * outerIndent), SCREEN_HEIGHT - (textBoxHeightStart + outerIndent), backGroundColor, RECTANGLE_TYPE::FILLED_ROUNDED, 5);
    pGame->DrawRect(outerIndent + innerIndent, textBoxHeightStart + innerIndent, SCREEN_WIDTH - (2 * (outerIndent + innerIndent)), SCREEN_HEIGHT - (textBoxHeightStart + innerIndent + outerIndent + innerIndent), SDLColor(127, 127, 127), RECTANGLE_TYPE::BOX_ROUNDED, 5);
    pGame->OutputText(font, szText, SDLColor(40, 40, 40), outerIndent + (innerIndent * 2), textBoxHeightStart + outerIndent + innerIndent, SCREEN_WIDTH - (outerIndent + (innerIndent * 2)));
    filledTrigonColor(pGame->GetSurface(), SCREEN_WIDTH - 20, SCREEN_HEIGHT - 18, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 18, SCREEN_WIDTH - 15, SCREEN_HEIGHT - 10, triangeColor.ReversedUInt());

    Uint32 counter = 0;
    while (true)
    {
        if (((counter++) % 50) < 8)
            triangeColor = backGroundColor;
        else
            triangeColor = SDLColor(30, 30, 30);
        filledTrigonColor(pGame->GetSurface(), SCREEN_WIDTH - 20, SCREEN_HEIGHT - 18, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 18, SCREEN_WIDTH - 15, SCREEN_HEIGHT - 10, triangeColor.ReversedUInt());
        
        if (pGame->PollEvents().type == SDL_QUIT || pGame->keys[SDLK_q] || pGame->keys[SDLK_ESCAPE])
        {
            ret = -1;
            break;
        }

        if (pGame->keys[SDLK_a] || pGame->keys[SDLK_b] || pGame->keys[SDLK_x] || pGame->keys[SDLK_y] || pGame->keys[SDLK_s])
            break;

        pGame->FlipScreen();
        pGame->FrameRateDelay();
    }

    // Reset the keys so they don't carry over to the game
    pGame->ResetKeys();
    return ret;
}
