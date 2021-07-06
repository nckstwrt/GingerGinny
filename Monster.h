#ifndef _MONSTER_H
#define _MONSTER_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include "Animation.h"
#include "World.h"
#include <map>
#include <memory>
#include <queue>
using namespace std;

class World;

class Monster
{
public:
    Monster();
    Monster(const Monster &monster);

    Animation* AddAnimationImages(ANIMATION animation, int imageCycleDelay, int imageCount, ...);

    void Move(DIRECTION direction);
    void MoveTo(int moveToX, int moveToY);
    void Attack();
    void Update();
    SDL_Surface* GetCurrentFrame();
    bool CheckOverlap(shared_ptr<Monster> p2);

    int x;
    int y;
    int width;
    int height;
    bool facingRight;
    bool AI;
    bool walking;
    bool attacking;
    queue<DIRECTION> directions;
    World* pWorld;

private:
    SDL_Surface* imgCurrentFrame;
    map<ANIMATION, Animation> animations;
};


#endif
