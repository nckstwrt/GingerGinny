#include "Animation.h"

Animation::Animation() :
    currentFrame(0),
    currentImage(0)
{
}

void Animation::AddImage(string imageName, SDL_Surface* img, SDL_Surface* leftImage, int imageDelay)
{
    if (img == NULL)
        printf("Animation::AddImage - img is NULL!!!!\n");

    imageNames.push_back(imageName);
    imagesRight.push_back(img);
    if (leftImage != NULL)
        imagesLeft.push_back(leftImage);
    animationDelays.push_back(imageDelay);
}

bool Animation::Increment()
{
    bool endOfCycle = false;
    if (currentFrame == animationDelays[currentImage])
    {
        currentImage++;
        currentFrame = 0;
    }
    else
        currentFrame++;
    if (currentImage == imagesRight.size())
    {
        currentImage = 0;
        currentFrame = 0;
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

bool Animation::HasImage(string imageName)
{
    return find(imageNames.begin(), imageNames.end(), imageName) != imageNames.end();
}