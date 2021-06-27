#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>

enum class DIRECTION
{
    Left,
    Right,
    Up,
    Down
};

enum class ANIMATION
{
    None,
    Walk,
    Run,
    Jump,
    Attack
};

class Animation
{
public:
    Animation(int imageCycleDelay = 6);
    ~Animation();

    void AddImages(int imageCount, ...);
    void AddImage(int imageCount, int imageIndex, SDL_Surface* img);

    bool Increment();
    void ResetAnimation();
    SDL_Surface* CurrentFrame(bool facingRight = true);

private:
    int currentFrame;
    int imageCount;
    int imageCycleDelay;
    SDL_Surface** imagesRight;
    SDL_Surface** imagesLeft;
};

#endif