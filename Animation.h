#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

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
    Attack,
    Hurt
};

class Animation
{
public:
    Animation();

    void AddImage(string imageName, SDL_Surface* img, SDL_Surface* leftImage,int imageDelay = 6);
    bool Increment();
    void ResetAnimation();
    SDL_Surface* CurrentImage(bool facingRight = true);
    void SetAnimationDelay(int imageIndex, int delay);
    bool HasImage(string imageName);

private:
    int currentImage;
    int currentFrame;
    vector<string> imageNames;
    vector<SDL_Surface*> imagesRight;
    vector<SDL_Surface*> imagesLeft;
    vector<int> animationDelays;
};

#endif