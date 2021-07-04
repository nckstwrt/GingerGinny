#include "Monster.h"

Monster::Monster() :
    x(0), y(0),
    width(0), height(0),
    facingRight(true),
    AI(false),
    imgCurrentFrame(NULL),
    walking(false),
    attacking(false)
{
}

Monster::Monster(const Monster& monster)
{
    *this = monster;
    for (map<ANIMATION, Animation>::iterator iter = this->animations.begin(); iter != this->animations.end(); iter++)
    {
        iter->second.MakeMaster(false);
    }
}

Animation* Monster::AddAnimationImages(ANIMATION animationType, int imageCycleDelay, int imageCount, ...)
{
    va_list vl;
    va_start(vl, imageCount);

    Animation animation;

    for (int i = 0; i < imageCount; i++)
    {
        animation.AddImage(imageCount, i, va_arg(vl, SDL_Surface*), imageCycleDelay);
        if (width == 0 || height == 0)
        {
            width = animation.CurrentImage()->w;
            height = animation.CurrentImage()->h;
        }
    }

    animations[animationType] = animation;
    animations[animationType].MakeMaster();

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

void Monster::MoveTo(int moveToX, int moveToY)
{
    directions = queue<DIRECTION>();

    // Move from feet to target
    int x0 = x + (width/2);
    int y0 = (y + height)-10;

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
    SDL_Rect m1 = { .x = (Sint16)x, .y = (Sint16)y, .w = (Uint16)width, .h = (Uint16)height };
    SDL_Rect m2 = { .x = (Sint16)p2->x, .y = (Sint16)p2->y, .w = (Uint16)p2->width, .h = (Uint16)p2->height };

    // Make it so only their feet (10 pixels worth) actually cause overlap
    m1.y = (m1.y + m1.h) - 10;
    m1.h = 10;
    m2.y = (m2.y + m2.h) - 10;
    m2.h = 10;
  
    // Shrink the x boundaries to give a little overlap
    m1.x += 3;
    m1.w -= 6;
    m2.x += 3;
    m2.w -= 6;


    return (m1.x < (m2.x + m2.w) && (m1.x + m1.w) > m2.x && m1.y < (m2.y + m2.h) && (m1.y + m1.h) > m2.y);
}

