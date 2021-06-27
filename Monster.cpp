#include "Monster.h"

Monster::Monster() :
    x(0), y(0),
    imgCurrentFrame(NULL),
    facingRight(true),
    walking(false),
    attacking(false),
    deleteAnimations(true)
{
}

Monster::~Monster()
{
    if (deleteAnimations)
    {
        for (map<ANIMATION, Animation*>::iterator iter = animations.begin(); iter != animations.end(); iter++)
        {
            delete iter->second;
        }
    }
}

Animation* Monster::AddAnimationImages(ANIMATION animationType, int imageCycleDelay, int imageCount, ...)
{
    va_list vl;
    va_start(vl, imageCount);

    Animation *animation = new Animation();

    for (int i = 0; i < imageCount; i++)
    {
        animation->AddImage(imageCount, i, va_arg(vl, SDL_Surface*), imageCycleDelay);
    }

    animations[animationType] = animation;

    va_end(vl);

    return animation;
}

void Monster::Move(DIRECTION direction)
{
    if (!attacking)
    {
        switch (direction)
        {
        case DIRECTION::Left:
            facingRight = false;
            x--;
            break;
        case DIRECTION::Right:
            facingRight = true;
            x++;
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
    if (attacking)
    {
        imgCurrentFrame = animations[ANIMATION::Attack]->CurrentImage(facingRight);
        if (animations[ANIMATION::Attack]->Increment())
        {
            attacking = false;
        }
    }
    else
    {
        if (walking)
        {
            imgCurrentFrame = animations[ANIMATION::Walk]->CurrentImage(facingRight);
            animations[ANIMATION::Walk]->Increment();
        }
        else
        {
            imgCurrentFrame = animations[ANIMATION::Idle]->CurrentImage(facingRight);
            animations[ANIMATION::Idle]->Increment();
            animations[ANIMATION::Walk]->ResetAnimation();
        }
    }
    walking = false;
}

SDL_Surface* Monster::GetCurrentFrame()
{
    return imgCurrentFrame;
}
