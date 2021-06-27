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
    Idle,
    Walk,
    Run,
    Jump,
    Attack
};

class Animation
{
public:
    Animation(bool masterAnimation = false);
    ~Animation();

    void AddImage(int imageCount, int imageIndex, SDL_Surface* img, int imageDelay = 6);
    bool Increment();
    void ResetAnimation();
    SDL_Surface* CurrentImage(bool facingRight = true);
    void SetAnimationDelay(int imageIndex, int delay);
    void MakeMaster(bool isMaster = true);

private:
    bool masterAnimation;
    int currentImage;
    int currentFrame;
    int imageCount;
    SDL_Surface** imagesRight;
    SDL_Surface** imagesLeft;
    int* animationDelays;
};

#endif