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
    Monster(const Monster &monster);

    Animation* AddAnimationImages(ANIMATION animation, int imageCycleDelay, int imageCount, ...);

    void Move(DIRECTION direction);
    void Attack();
    void Update();
    SDL_Surface* GetCurrentFrame();
    Monster Duplicate();

    int x;
    int y;
    bool facingRight;
    bool AI;
    int targetX;
    int targetY;

private:
    SDL_Surface* imgCurrentFrame;
    map<ANIMATION, Animation*> animations;
    bool walking;
    bool attacking;
    bool deleteAnimations;
};


#endif
