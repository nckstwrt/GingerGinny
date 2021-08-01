#include "Monster.h"
#include "SDLGame.h"

Monster::Monster(World *pWorld) :
    x(0), y(0),
    width(0), characterWidth(0), height(0),
    facingRight(true),
    AI(Monster::ChaseAI),
    imgCurrentFrame(NULL),
    walking(false),
    attacking(false),
    health(6),
    pWorld(pWorld),
    lastMoveToX(0), lastMoveToY(0),
    chasingMonster(NULL),
    alignment(ALIGNMENT::BAD),
    blocking(true),
    hurtCounter(0),
    tickCounter(0)
{
}

Monster::Monster(const Monster& monster)
{
    *this = monster;
}

Animation* Monster::AddAnimationImages(ANIMATION animationType, int imageCycleDelay, int imageCount, string firstImageName, ...)
{
    va_list vl;
    va_start(vl, firstImageName);

    Animation animation;

    for (int i = 0; i < imageCount; i++)
    {
        SDL_Surface* img = va_arg(vl, SDL_Surface*);
        animation.AddImage(firstImageName, img, pWorld->pGame->CreateHorizontallyFlippedImage(img), imageCycleDelay);
        if (width == 0 || height == 0)
        {
            width = animation.CurrentImage()->w;
            characterWidth = width;
            height = animation.CurrentImage()->h;
        }
    }

    animations[animationType] = animation;

    va_end(vl);

    return &animations[animationType];
}

void Monster::Move(DIRECTION direction, bool facingChange)
{
    if (!attacking)
    {
        switch (direction)
        {
        case DIRECTION::Left:
            if (facingChange)
                facingRight = false;
            x -= 1;
            break;
        case DIRECTION::Right:
            if (facingChange)
                facingRight = true;
            x += 1;
            break;
        case DIRECTION::Up:
            y--;
            break;
        case DIRECTION::Down:
            y++;
            break;
        }
        walking = true;
    }
}

void Monster::MoveTo(int moveToX, int moveToY, bool carryOnFromLastDirection)
{
    int x0, y0;

    if (carryOnFromLastDirection)
    {
        x0 = lastMoveToX;
        y0 = lastMoveToY;
    }
    else
    {
        directions = queue<DIRECTION>();

        x0 = GetMidPoint(true).x;
        y0 = GetMidPoint(true).y;
    }

    int x1 = moveToX;
    int y1 = moveToY;

    Helper::CalcLine(x0, y0, x1, y1, [this](int x, int y, int dx, int dy) -> void {
        if (dx != 0)
            directions.push(dx > 0 ? DIRECTION::Right : DIRECTION::Left);
        else
            directions.push(dy > 0 ? DIRECTION::Down : DIRECTION::Up);
    });

    lastMoveToX = moveToX;
    lastMoveToY = moveToY;
}

void Monster::Attack()
{
    attacking = true;

    for (auto& monster : pWorld->monsters)
    {
        if (GetUs() != monster)
        {
            auto distance = monster->GetMidPoint().DistanceTo(GetMidPoint());
            if (distance < 35 && ((facingRight && monster->x >= x) || (!facingRight && monster->x < x)) && monster->alignment != ALIGNMENT::NEUTRAL)
            {
                if (monster->hurtCounter == 0)
                {
                    monster->hurtCounter = 25;
                    monster->health--;
                }
            }
        }
    }
}

void Monster::Update()
{
    // If has directions to go to, move the monster
    if (directions.size() > 0)
    {
        pWorld->MonsterMove(GetUs(), directions.front());
        directions.pop();
        if (directions.size() == 0)
            walking = false;
    }

    if (AI && hurtCounter == 0)    // Only let a monster move normally if it's not being currently hurt
        AI(this);

    if (hurtCounter > 0)
    {
        if (animations.find(ANIMATION::Hurt) != animations.end())
        {
            imgCurrentFrame = animations[ANIMATION::Hurt].CurrentImage(facingRight);
            if (animations[ANIMATION::Hurt].Increment()) 
                hurtCounter = 0;
        }
        else
        {
            directions = queue<DIRECTION>();
            pWorld->MonsterMove(GetUs(), facingRight ? DIRECTION::Left : DIRECTION::Right, false);

            hurtCounter--;

            // If no hurt animation do walk animation
            imgCurrentFrame = animations[ANIMATION::Walk].CurrentImage(facingRight);
            animations[ANIMATION::Walk].Increment();
            if (animations.find(ANIMATION::Idle) != animations.end())
                animations[ANIMATION::Idle].ResetAnimation();
        }
    }
    else
    {
        if (attacking)
        {
            imgCurrentFrame = animations[ANIMATION::Attack].CurrentImage(facingRight);
            if (animations[ANIMATION::Attack].Increment())
            {
                attacking = false;
            }
        }
        else
        {
            if (walking)
            {
                imgCurrentFrame = animations[ANIMATION::Walk].CurrentImage(facingRight);
                animations[ANIMATION::Walk].Increment();
                if (animations.find(ANIMATION::Idle) != animations.end())
                    animations[ANIMATION::Idle].ResetAnimation();
            }
            else
            {
                imgCurrentFrame = animations[ANIMATION::Idle].CurrentImage(facingRight);
                animations[ANIMATION::Idle].Increment();
                if (animations.find(ANIMATION::Walk) != animations.end())
                    animations[ANIMATION::Walk].ResetAnimation();
            }
        }
    }
    walking = false;

    tickCounter++;
}

SDL_Surface* Monster::GetCurrentFrame()
{
    return imgCurrentFrame;
}

bool Monster::CheckOverlap(shared_ptr<Monster> p2)
{
    auto m1 = GetRect(true);
    auto m2 = p2->GetRect(true);

    return m1.IntersectsRect(m2);
}

Rect Monster::GetRect(bool justFeet)
{
    int charWidthDiff = (width - characterWidth)/2;
    Rect rect(x + charWidthDiff, y, characterWidth, height);

    // Shrink the monster to just skinny "feet"
    if (justFeet)
    {
        rect.y = (rect.y + rect.height) - 5;
        rect.height = 5;
        rect.x;
        rect.width;
    }

    return rect;
}

Point Monster::GetMidPoint(bool fromFeet)
{
    int charWidthDiff = (width - characterWidth)/2;
    return Point(x + charWidthDiff + (characterWidth / 2), fromFeet ? ((y + height) - 5) : (y + (height / 2)));
}

bool Monster::HasImage(const char* szImage)
{
    bool ret = false;
    for (auto& anim : animations)
    {
        if (anim.second.HasImage(szImage))
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool Monster::TakeDamage(int damage)
{
    if (hurtCounter == 0)
    {
        health -= damage;
        hurtCounter = 20;
    }
    return (health == 0);
}

shared_ptr<Monster> Monster::GetUs()
{
    if (us == nullptr)
        us = *find_if(pWorld->monsters.begin(), pWorld->monsters.end(), [this](const shared_ptr<Monster>& x) { return x.get() == this; });
    return us;
}

void Monster::ChaseAI(Monster *pMonster)
{
    // If we're a BAD monster look for GOOD players to move towards
    if ((pMonster->tickCounter % 20) == 0 && pMonster->alignment == ALIGNMENT::BAD && pMonster->pWorld)
    {
        // see if we can find a monster to chase
        if (pMonster->chasingMonster == NULL)
        {
            Circle ourRadius(pMonster->GetMidPoint().x, pMonster->GetMidPoint().y, 100);
            for (auto& monster : pMonster->pWorld->monsters)
            {
                if (monster.get() != pMonster)
                {
                    if (monster->alignment == ALIGNMENT::GOOD)
                    {
                        if (ourRadius.ContainsPoint(monster->GetMidPoint().x, monster->GetMidPoint().y))
                        {
                            // We've found Ginny nearby, now let's go chase her if we have line of sight
                            bool lineBroken = false;
                            Helper::CalcLine(pMonster->GetMidPoint().x, pMonster->GetMidPoint().y, monster->GetMidPoint().x, monster->GetMidPoint().y, [pMonster, &lineBroken](int x, int y, int sx, int sy)
                            {
                                if (pMonster->pWorld->SafeGetTile(x / TILE_SIZE, y / TILE_SIZE, 0)->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                    lineBroken = true;
                            });
                            if (!lineBroken)
                            {
                                pMonster->chasingMonster = monster;
                            }
                        }
                    }
                }
            }
        }
        
        // If we do have a chasing monster
        if (pMonster->chasingMonster)
        {
            // Let's try and go to the left or right of Ginny so we can attack her:
            // 1. Check nearest side for a safe tile
            Point monsterMidPoint(pMonster->chasingMonster->GetMidPoint(true).x, pMonster->chasingMonster->GetMidPoint(true).y);

            // Check if we're close enough to hit Ginny
            auto distanceToMonster = pMonster->GetMidPoint().DistanceTo(pMonster->chasingMonster->GetMidPoint());
            if (distanceToMonster < 22)
            {
                pMonster->chasingMonster->TakeDamage();
            }
            else
            {
                // Hack, monsters are shorter than Ginny so looks weird if they go after the midpoint
                monsterMidPoint.y -= 10;

                Point leftPoint(monsterMidPoint.x - TILE_SIZE, monsterMidPoint.y);
                Point rightPoint(monsterMidPoint.x + TILE_SIZE, monsterMidPoint.y);

                Point& nearestPoint = rightPoint;
                Point& furthestPoint = leftPoint;
                Point& targetPoint = monsterMidPoint;
                if (pMonster->GetMidPoint().DistanceTo(leftPoint) < pMonster->GetMidPoint().DistanceTo(rightPoint))
                {
                    nearestPoint = leftPoint;
                    furthestPoint = rightPoint;
                }

                // Check if the nearest point is a wall, then the furthest point, failing that all just go direct
                if (pMonster->pWorld->SafeGetTile(nearestPoint.x / TILE_SIZE, nearestPoint.y / TILE_SIZE, 0)->tileType == TILE_TYPE::FLOOR)
                    targetPoint = nearestPoint;
                else
                {
                    if (pMonster->pWorld->SafeGetTile(furthestPoint.x / TILE_SIZE, furthestPoint.y / TILE_SIZE, 0)->tileType == TILE_TYPE::FLOOR)
                        targetPoint = furthestPoint;
                    else
                        targetPoint = monsterMidPoint;
                }

                //pMonster->pWorld->debugRects.push_back(Rect(targetPoint.x / 16, targetPoint.y / 16, 16, 16));
                pMonster->pWorld->MonsterMoveTo(pMonster->GetUs(), targetPoint.x, targetPoint.y);
            }
        }
    }
}

