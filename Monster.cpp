#include "Monster.h"
#include "SDLGame.h"

Monster::Monster() :
    x(0), y(0),
    width(0), height(0),
    facingRight(true),
    AI(false),
    imgCurrentFrame(NULL),
    walking(false),
    attacking(false),
    pWorld(NULL),
    lastMoveToX(0), lastMoveToY(0),
    alignment(Monster::ALIGNMENT::BAD),
    tickCounter(0)
{
}

Monster::Monster(const Monster& monster)
{
    *this = monster;
}

Animation* Monster::AddAnimationImages(ANIMATION animationType, int imageCycleDelay, int imageCount, ...)
{
    va_list vl;
    va_start(vl, imageCount);

    Animation animation;

    for (int i = 0; i < imageCount; i++)
    {
        SDL_Surface* img = va_arg(vl, SDL_Surface*);
        animation.AddImage(img, SDLGame::CreateHorizontallyFlippedImage(img), imageCycleDelay);
        if (width == 0 || height == 0)
        {
            width = animation.CurrentImage()->w;
            height = animation.CurrentImage()->h;
        }
    }

    animations[animationType] = animation;

    va_end(vl);

    return &animations[animationType];
}

void Monster::Move(DIRECTION direction)
{
    if (!attacking)
    {
        switch (direction)
        {
        case DIRECTION::Left:
            facingRight = false;
            x-=1;
            break;
        case DIRECTION::Right:
            facingRight = true;
            x+=1;
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
}

void Monster::Update()
{
    // If we're a BAD monster look for GOOD players to move towards
    if ((tickCounter % 10) == 0 && alignment == Monster::ALIGNMENT::BAD && pWorld)
    {
        Circle ourRadius(GetMidPoint().x, GetMidPoint().y, 100);
        auto us = *find_if(pWorld->monsters.begin(), pWorld->monsters.end(), [this](const shared_ptr<Monster>& x) { return x.get() == this; });
        for (auto &monster : pWorld->monsters)
        {
            if (monster.get() != this)
            {
                if (monster->alignment == Monster::ALIGNMENT::GOOD)
                {
                    if (ourRadius.ContainsPoint(monster->GetMidPoint().x, monster->GetMidPoint().y))
                    {
                        // We've found Ginny nearby, now let's go chase her if we have line of sight
                        bool lineBroken = false;
                        Helper::CalcLine(GetMidPoint().x, GetMidPoint().y, monster->GetMidPoint().x, monster->GetMidPoint().y, [this, &lineBroken](int x, int y, int sx, int sy)
                        {
                            if (pWorld->SafeGetTile(x / TILE_SIZE, y / TILE_SIZE, 0)->tileType == TILE_TYPE::WALL_ALWAYS_ON_TOP)
                                lineBroken = true;
                        });
                        if (!lineBroken)
                        {
                            pWorld->MonsterMoveTo(us, monster->GetMidPoint().x, monster->GetMidPoint(true).y);
                        }
                    }
                }
            }
        }
    }

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
    Rect rect(x, y, width, height);

    // Shrink the monster to just skinny "feet"
    if (justFeet)
    {
        rect.y = (rect.y + rect.height) - 10;
        rect.height = 10;
        rect.x += 3;
        rect.width -= 6;
    }

    return rect;
}

Point Monster::GetMidPoint(bool fromFeet)
{
    return Point(x + (width / 2), fromFeet ? ((y + height) - 10) : (y + (height / 2)));
}

