#include "Animation.h"

Animation::Animation() :
    currentFrame(0),
    currentImage(0),
    imageCount(0),
    imagesRight(NULL),
    imagesLeft(NULL),
    animationDelays(NULL)
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
    if (animationDelays)
        delete[] animationDelays;
}

void Animation::AddImage(int imageCount, int imageIndex, SDL_Surface* img, int imageDelay)
{
    if (!imagesRight)
        imagesRight = new SDL_Surface * [imageCount];
    if (!imagesLeft)
        imagesLeft = new SDL_Surface * [imageCount];
    if (!animationDelays)
        animationDelays = new int[imageCount];

    imagesRight[imageIndex] = img;
    imagesLeft[imageIndex] = rotozoomSurfaceXY(imagesRight[imageIndex], 0, -1, 1, SMOOTHING_OFF);
    animationDelays[imageIndex] = imageDelay;

    this->imageCount = imageCount;
}

bool Animation::Increment()
{
    bool endOfCycle = false;
    currentFrame++;
    if (currentFrame == animationDelays[currentImage])
    {
        currentImage++;
        currentFrame = 0;
    }
    if (currentImage == imageCount)
    {
        currentImage = 0;
        endOfCycle = true;
    }
    return endOfCycle;
}

void Animation::ResetAnimation()
{
    currentFrame = 0;
}

SDL_Surface* Animation::CurrentImage(bool facingRight)
{
    return facingRight ? imagesRight[currentImage] : imagesLeft[currentImage];
}

void Animation::SetAnimationDelay(int imageIndex, int delay)
{
    animationDelays[imageIndex] = delay;
}