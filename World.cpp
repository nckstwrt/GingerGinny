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
    int x, y;
    char szImage[100] = { 0 };
    char spriteOrTile, rightOrleft;
    tileMapWidth = tileMapHeight = 0;
    FILE* f = fopen(szMapFile, "rt");
    while (true)
    {
        if (fscanf(f, "%d %d %s %c %c", &x, &y, szImage, &spriteOrTile, &rightOrleft) != 5)
            break;
        if (x > tileMapWidth)
            tileMapWidth = x;
        if (y > tileMapHeight)
            tileMapHeight = y;

        if (spriteOrTile == 'T')
        {
            Tile tile(x, y);
            tile.tileType = (strstr(szImage, "wall") == NULL) ? TILE_TYPE::FLOOR : TILE_TYPE::WALL;

            // If it's an animation load all 3 frames
            if (strstr(szImage, "_f0"))
            {
                string animImages = szImage;
                int imageSpeed = 4;
                int imageCount = 3;

                if (strstr(szImage, "spikes"))
                {
                    imageSpeed = 2;
                    imageCount = 4;
                }

                Helper::string_replace(animImages, "_f0", "_f%d");
                for (int i = 0; i < imageCount; i++)
                {
                    string imageFile = Helper::string_format(animImages, i);
                    tile.animation.AddImage(imageFile, pGame->GetLoadedImage(imageFile.c_str()), NULL, imageSpeed);
                }

                if (strstr(szImage, "spikes"))
                {
                    tile.animation.SetAnimationDelay(0, 100);
                    tile.animation.SetAnimationDelay(3, 20);
                    for (int i = 3; i != 0; i--)
                    {
                        string imageFile = Helper::string_format(animImages, i);
                        tile.animation.AddImage(imageFile, pGame->GetLoadedImage(imageFile.c_str()), NULL, imageSpeed);
                    }
                }
            }
            else
                tile.animation.AddImage(szImage, pGame->GetLoadedImage(szImage), NULL, 100);
            tiles.push_back(tile);
        }
        if (spriteOrTile == 'S')
        {
            auto iter = find_if(monsterTemplates.begin(), monsterTemplates.end(), [szImage](const shared_ptr<Monster>& monster) { return monster->HasImage(szImage); });
            if (iter != monsterTemplates.end())
            {
                AddMonster(*(*iter).get(), x, y, rightOrleft == 'R', (*iter)->alignment);
            }
            else
            {
                Monster spriteMonster(this);
                spriteMonster.AddAnimationImages(ANIMATION::Idle, 1000, 1, szImage, pGame->GetLoadedImage(szImage));
                AddMonster(spriteMonster, x, y, rightOrleft == 'R', ALIGNMENT::NEUTRAL);
            }
        }
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

    // Now force some walls to be on top and set up the pathfinder
    for (int y = 0; y < tileMapHeight; y++)
    {
        for (int x = 0; x < tileMapWidth; x++)
        {
            for (int i = 0; i < MAX_TILE_PER_SQUARE; i++)
            {
                if (SafeGetTile(x, y, i)->tileType == TILE_TYPE::WALL && SafeGetTile(x, y, 0)->tileType == TILE_TYPE::FLOOR)
                    SafeGetTile(x, y, i)->tileType = TILE_TYPE::WALL_ALWAYS_ON_TOP;

                // Set up pathfinder
                if (i == 0)
                {
                    if (SafeGetTile(x, y, 0)->tileType != TILE_TYPE::FLOOR)
                    {
                        pathFinder.addWall({ x, y });
                    }
                }
            }
        }
    }

    // Now set Ginny up
    auto ginny = find_if(monsters.begin(), monsters.end(), [szImage](const shared_ptr<Monster>& monster) { return monster->HasImage("idle_000_vsmall.png"); });
    if (ginny != monsters.end())
    {
        pCameraFollow = *ginny;
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

void World::AddMonsterTemplate(const Monster& monsterToCopy, ALIGNMENT alignment)
{
    shared_ptr<Monster> newMonster = make_shared<Monster>(monsterToCopy);
    newMonster->pWorld = this;
    newMonster->alignment = alignment;
    monsterTemplates.push_back(newMonster);
}

shared_ptr<Monster> World::AddMonster(const Monster &monsterToCopy, int tileX, int tileY, bool facingRight, ALIGNMENT alignment)
{
    shared_ptr<Monster> newMonster = make_shared<Monster>(monsterToCopy);
    newMonster->x = ((tileX * TILE_SIZE) + (TILE_SIZE / 2)) - (newMonster->width / 2);
    newMonster->y = ((tileY * TILE_SIZE) + (TILE_SIZE)) - (newMonster->height);  // Put their feet roughly centre square
    newMonster->facingRight = facingRight;
    newMonster->alignment = alignment;
    newMonster->pWorld = this;

    monsters.push_back(newMonster);
    if (monsters.size() == 1)
        pCameraFollow = monsters[0];
    return monsters.back();
}

void World::Update()
{
    // Update all Tiles
    for (int y = 0; y < tileMapHeight; y++)
        for (int x = 0; x < tileMapWidth; x++)
            SafeGetTile(x, y, 0)->Update();

    // Update all Monsters
    vector<shared_ptr<Monster>> deadMonsters;
    for (auto &monster : monsters)
    {
        monster->Update();
    }

    // If dying animation over and dead, remove
    monsters.erase(remove_if(monsters.begin(), monsters.end(), [this](auto& monster) 
    { 
        if (monster->health <= 0 && monster->hurtCounter == 0)
        {
            // This might be a chased monster, if so, no longer chase
            for (auto& checkMonster : monsters)
            {
                if (checkMonster->chasingMonster == monster)
                    checkMonster->chasingMonster = NULL;
            }
            return true;
        }
        else
            return false;
    }), monsters.end());

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

void World::DrawTiles(bool bottomTiles)
{
    int startTileX = offsetX / TILE_SIZE;
    int startTileY = offsetY / TILE_SIZE;

    for (int y = 0; y < (SCREEN_HEIGHT / TILE_SIZE) + 1; y++)
    {
        for (int x = 0; x < (SCREEN_WIDTH / TILE_SIZE) + 1; x++)
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

                            if (bottomTiles && (pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) && pTile->tileType != TILE_TYPE::WALL_ALWAYS_ON_TOP)
                               pGame->BlitImage(pTile->GetCurrentImage(), pixelX, pixelY);

                            if (!bottomTiles && (!(pTile->tileType == TILE_TYPE::FLOOR || (startTileY + y + 2 < tileMapHeight && tileMap[startTileY + y + 2][startTileX + x] != NULL)) || pTile->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP))
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

void World::Draw()
{
    // Draw Map
    DrawTiles(true);

    // Draw monsters
    for (auto &monster : monsters)
    {
        pGame->BlitImage(monster->GetCurrentFrame(), PixelXToDisplayPixelX(monster->x), PixelYToDisplayPixelY(monster->y));
    }

    // Draw parts of the floor + walls that should be over Ginny and the monsters
    DrawTiles(false);

    // Draw any debug points or rectangles
    for (auto& point : debugPoints)
        pGame->DrawPoint(PixelXToDisplayPixelX(point.x), PixelYToDisplayPixelY(point.y), SDLColor(255, 0, 0));
    for (auto& rect : debugRects)
        pGame->DrawRect(TileXToDisplayPixelX(rect.x), TileYToDisplayPixelY(rect.y), rect.width, rect.height, SDLColor(255, 0, 0, 60));
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

bool World::MonsterMove(shared_ptr<Monster> pMonster, DIRECTION direction, bool facingChange, bool alwaysMoveBack)
{
    int savedX = pMonster->x;
    int savedY = pMonster->y;
    pMonster->Move(direction, facingChange);

    auto monsterRect = pMonster->GetRect(true);
    int monsterX1 = monsterRect.x + 1;
    int monsterX2 = (monsterRect.x + monsterRect.width) - 1;
    int monsterY = monsterRect.y;
    
    bool moveBack = false;
    int monsterXTile1 = monsterX1 / TILE_SIZE;
    int monsterXTile2 = monsterX2 / TILE_SIZE;
    int monsterYTile = monsterY / TILE_SIZE;
    if (monsterXTile1 >= 0 && monsterXTile2 >= 0 && monsterYTile >= 0 &&
        monsterXTile1 < tileMapWidth && monsterXTile2 < tileMapWidth && monsterYTile < tileMapHeight)
    {
        if (SafeGetTile(monsterXTile1, monsterYTile, 0)->tileType != TILE_TYPE::FLOOR)
            moveBack = true;
        if (SafeGetTile(monsterXTile2, monsterYTile, 0)->tileType != TILE_TYPE::FLOOR)
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
                if (monster->blocking && pMonster->CheckOverlap(monster))
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

    // Add all other monsters in
    /* Don't add monsters else they'll block each other (which will mean they will find another route)
    for (auto& monster : monsters)
    {
        if (monster->blocking)
        {
            Rect monsterFeet = monster->GetRect(true);
            if (pMonster != monster)
            {
                Rect monsterRect = monster->GetRect(true);
                if (!monsterRect.ContainsPoint(x, y))
                {
                    monsterRect.DivideBy(TILE_SIZE);
                    pathFinder.addMonster(monsterRect);
                }
            }
        }
    }
    */
    // Run the A* Search
    auto path = pathFinder.findPath({ pMonster->GetMidPoint(true).x / TILE_SIZE, pMonster->GetMidPoint(true).y / TILE_SIZE }, { x / TILE_SIZE, y / TILE_SIZE });
    
    // Move the returned path into the centre of the tiles
    for (auto& p : path)
    {
        ret.push_back(Point((p.x * TILE_SIZE) + TILE_SIZE/2, (p.y * TILE_SIZE) + TILE_SIZE / 2));
    }
    reverse(ret.begin(), ret.end());
    
    // Skipping the first one seems to help a lot here
    if (ret.size() > 1)
    {
        for (unsigned int i = 1; i < ret.size(); i++)
            pMonster->MoveTo(ret[i].x, ret[i].y, i != 1);
    }

    return ret;
}

void World::MonsterAttack(shared_ptr<Monster> pMonster)
{
    pMonster->Attack();
}

int World::DrawTextBox(TTF_Font* font, const char* szText, SDLColor textColor)
{
    int ret = 0;
    int textBoxHeightStart = SCREEN_HEIGHT - 70;
    int outerIndent = 1;
    int innerIndent = 4;
    SDLColor backGroundColor = SDLColor(200, 200, 200);
    SDLColor triangeColor = SDLColor(30, 30, 30);
    pGame->DrawRect(outerIndent, textBoxHeightStart, SCREEN_WIDTH - (2 * outerIndent), SCREEN_HEIGHT - (textBoxHeightStart + outerIndent), backGroundColor, RECTANGLE_TYPE::FILLED_ROUNDED, 5);
    pGame->DrawRect(outerIndent + innerIndent, textBoxHeightStart + innerIndent, SCREEN_WIDTH - (2 * (outerIndent + innerIndent)), SCREEN_HEIGHT - (textBoxHeightStart + innerIndent + outerIndent + innerIndent), SDLColor(127, 127, 127), RECTANGLE_TYPE::BOX_ROUNDED, 5);
    pGame->OutputText(font, szText, textColor, (outerIndent + (innerIndent * 2) + 2), textBoxHeightStart + outerIndent + innerIndent, SCREEN_WIDTH - ((outerIndent + (innerIndent * 2) + 2)));
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
