#include "Monster.h"

Monster::Monster() :
    x(0), y(0),
    imgCurrentFrame(NULL),
    imgStillLeft(NULL), imgStillRight(NULL),
    facingRight(true),
    walking(false),
    attacking(false),
    deleteAnimations(true)
{
}

Monster::~Monster()
{
    if (imgStillLeft)
        SDL_FreeSurface(imgStillLeft);

    if (deleteAnimations)
    {
        for (map<ANIMATION, Animation*>::iterator iter = animations.begin(); iter != animations.end(); iter++)
        {
            delete iter->second;
        }
    }
}

void Monster::SetStillImage(SDL_Surface* imgStill)
{
    imgStillRight = imgStill;
    imgStillLeft = rotozoomSurfaceXY(imgStillRight, 0, -1, 1, SMOOTHING_OFF);
}

void Monster::AddAnimationImages(ANIMATION animationType, int imageCycleDelay, int imageCount, ...)
{
    va_list vl;
    va_start(vl, imageCount);

    Animation *animation = new Animation(imageCycleDelay);

    for (int i = 0; i < imageCount; i++)
    {
        animation->AddImage(imageCount, i, va_arg(vl, SDL_Surface*));
    }

    animations[animationType] = animation;

    va_end(vl);
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
        imgCurrentFrame = animations[ANIMATION::Attack]->CurrentFrame(facingRight);
        if (animations[ANIMATION::Attack]->Increment())
        {
            attacking = false;
        }
    }
    else
    {
        if (walking)
        {
            imgCurrentFrame = animations[ANIMATION::Walk]->CurrentFrame(facingRight);
            animations[ANIMATION::Walk]->Increment();
        }
        else
        {
            imgCurrentFrame = facingRight ? imgStillRight : imgStillLeft;
            animations[ANIMATION::Walk]->ResetAnimation();
        }
    }
    walking = false;
}

SDL_Surface* Monster::GetCurrentFrame()
{
    return imgCurrentFrame;
}
