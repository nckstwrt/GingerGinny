#ifndef _MONSTER_H
#define _MONSTER_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include "Animation.h"
#include <map>
using namespace std;

class Monster
{
public:
    Monster();
    ~Monster();

    void SetStillImage(SDL_Surface* imgStill);
    void AddAnimationImages(ANIMATION animation, int imageCycleDelay, int imageCount, ...);

    void Move(DIRECTION direction);
    void Attack();
    void Update();
    SDL_Surface* GetCurrentFrame();

    int x;
    int y;

private:
    SDL_Surface* imgCurrentFrame;

    SDL_Surface* imgStillRight;
    SDL_Surface* imgStillLeft;

    map<ANIMATION, Animation*> animations;

    bool facingRight;
    bool walking;
    bool attacking;
    bool deleteAnimations;
};


#endif
