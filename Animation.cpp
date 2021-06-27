#include "Animation.h"

Animation::Animation(int imageCycleDelay) :
    currentFrame(0),
    imageCount(0),
    imageCycleDelay(imageCycleDelay),
    imagesRight(NULL),
    imagesLeft(NULL)
{
}

Animation::~Animation()
{
    if (imagesRight)
        delete[] imagesRight;
    if (imagesLeft)
    {
        for (int i = 0; i < imageCount; i++)
            SDL_FreeSurface(imagesLeft[i]);
        delete[] imagesLeft;
    }
}

void Animation::AddImages(int imageCount, ...)
{
    va_list vl;
    va_start(vl, imageCount);

    imagesRight = new SDL_Surface * [imageCount];
    imagesLeft = new SDL_Surface * [imageCount];
    for (int i = 0; i < imageCount; i++)
    {
        imagesRight[i] = va_arg(vl, SDL_Surface*);
        imagesLeft[i] = rotozoomSurfaceXY(imagesRight[i], 0, -1, 1, SMOOTHING_OFF);
    }
    this->imageCount = imageCount;

    va_end(vl);
}

void Animation::AddImage(int imageCount, int imageIndex, SDL_Surface* img)
{
    if (!imagesRight)
        imagesRight = new SDL_Surface * [imageCount];
    if (!imagesLeft)
        imagesLeft = new SDL_Surface * [imageCount];

    imagesRight[imageIndex] = img;
    imagesLeft[imageIndex] = rotozoomSurfaceXY(imagesRight[imageIndex], 0, -1, 1, SMOOTHING_OFF);

    this->imageCount = imageCount;
}

bool Animation::Increment()
{
    bool endOfCycle = false;
    currentFrame++;
    if (currentFrame / imageCycleDelay == imageCount)
    {
        currentFrame = 0;
        endOfCycle = true;
    }
    return endOfCycle;
}

void Animation::ResetAnimation()
{
    currentFrame = 0;
}

SDL_Surface* Animation::CurrentFrame(bool facingRight)
{
    return facingRight ? imagesRight[currentFrame / imageCycleDelay] : imagesLeft[currentFrame / imageCycleDelay];
}