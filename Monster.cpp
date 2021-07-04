#include "Monster.h"

Monster::Monster() :
    x(0), y(0),
    width(0), height(0),
    facingRight(true),
    AI(false),
    targetX(-1),
    targetY(-1),
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

void Monster::Attack()
{
    attacking = true;
}

void Monster::Update()
{
    if (AI)
    {
        if (targetX >= -1)
        {
            if (x == targetX && y == targetY || (targetX == -1 || targetY == -1))
            {
                if ((rand() % 3) == 0)
                {
                    targetX = -100;
                    walking = false;
                }
                else
                {
                    targetX = rand() % (240 - 32);
                    targetY = rand() % (240 - 32);
                }
            }

            if (targetX >= 0)
            {
                double xDirection = targetX - x;
                double yDirection = targetY - y;

                double magnitude = sqrt(xDirection * xDirection + yDirection * yDirection);

                double xUnit = xDirection / magnitude;
                double yUnit = yDirection / magnitude;

                x = (int)((((double)x) + xUnit) + 0.5);
                y = (int)((((double)y) + yUnit) + 0.5);
                facingRight = xDirection > 0;
                walking = true;
            }
        }
        else
            targetX++;
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

