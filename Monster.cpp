#include "Monster.h"

Monster::Monster() :
    x(0), y(0),
    imgCurrentFrame(NULL),
    imgStillLeft(NULL), imgStillRight(NULL),
    walkImageCount(0),
    imgWalkRight(NULL), imgWalkLeft(NULL),
    facingRight(true),
    walking(false),
    walkingFrame(0),
    walkingFrameDelay(6)
{
}

Monster::~Monster()
{
    if (imgStillLeft)
        SDL_FreeSurface(imgStillLeft);
    if (imgWalkRight)
        delete[] imgWalkRight;
    if (imgWalkLeft)
    {
        for (int i = 0; i < walkImageCount; i++)
            SDL_FreeSurface(imgWalkLeft[i]);
        delete[] imgWalkLeft;
    }
}

void Monster::SetStillImage(SDL_Surface* imgStill)
{
    imgStillRight = imgStill;
    imgStillLeft = rotozoomSurfaceXY(imgStillRight, 0, -1, 1, SMOOTHING_OFF);
}

void Monster::AddWalkImages(int walkImageCount, ...)
{
    va_list vl;
    va_start(vl, walkImageCount);

    imgWalkRight = new SDL_Surface * [walkImageCount];
    imgWalkLeft = new SDL_Surface * [walkImageCount];
    for (int i = 0; i < walkImageCount; i++)
    {
        imgWalkRight[i] = va_arg(vl, SDL_Surface*);
        imgWalkLeft[i] = rotozoomSurfaceXY(imgWalkRight[i], 0, -1, 1, SMOOTHING_OFF);
    }
    this->walkImageCount = walkImageCount;

    va_end(vl);
}

void Monster::Move(DIRECTION direction)
{
    switch (direction)
    {
    case Left:
        facingRight = false;
        x--;
        break;
    case Right:
        facingRight = true;
        x++;
        break;
    case Up:
        y--;
        break;
    case Down:
        y++;
        break;
    }
    walking = true;
}

void Monster::Update()
{
    if (walking)
    {
        if (facingRight)
            imgCurrentFrame = imgWalkRight[(walkingFrame / walkingFrameDelay)];
        else
            imgCurrentFrame = imgWalkLeft[(walkingFrame / walkingFrameDelay)];
        walkingFrame++;
        if ((walkingFrame / walkingFrameDelay) == walkImageCount)
            walkingFrame = 0;
    }
    else
    {
        imgCurrentFrame = facingRight ? imgStillRight : imgStillLeft;
        walkingFrame = 0;
    }
    walking = false;
}

SDL_Surface* Monster::GetCurrentFrame()
{
    return imgCurrentFrame;
}
