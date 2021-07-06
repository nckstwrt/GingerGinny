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
    lastMoveToX(0), lastMoveToY(0)
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

        // Move from feet to target
        x0 = x + (width / 2);
        y0 = (y + height) - 10;
    }

    int x1 = moveToX;
    int y1 = moveToY;

    int sx = 0;
    int sy = 0;

    int dx = abs(x1 - x0);
    sx = x0 < x1 ? 1 : -1;
    int dy = -1 * abs(y1 - y0);
    sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;) 
    {  
        if (x0 == x1 && y0 == y1) 
            break;
        e2 = 2 * err;
        if (e2 >= dy) 
        { 
            err += dy; 
            x0 += sx; 
            directions.push(sx > 0 ? DIRECTION::Right : DIRECTION::Left);
        } 
        if (e2 <= dx) 
        { 
            err += dx; 
            y0 += sy; 
            directions.push(sy > 0 ? DIRECTION::Down : DIRECTION::Up);
        } 
    }

    lastMoveToX = moveToX;
    lastMoveToY = moveToY;
}

void Monster::Attack()
{
    attacking = true;
}

void Monster::Update()
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
    walking = false;
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

