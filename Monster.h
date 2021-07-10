#ifndef _MONSTER_H
#define _MONSTER_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include "Animation.h"
#include "World.h"
#include "Helper.h"
#include <map>
#include <memory>
#include <queue>
#include <algorithm>
using namespace std;

class World;

class Monster
{
public:
    Monster();
    Monster(const Monster &monster);

    Animation* AddAnimationImages(ANIMATION animation, int imageCycleDelay, int imageCount, ...);

    void Move(DIRECTION direction);
    void MoveTo(int moveToX, int moveToY, bool carryOnFromLastDirection = false);
    void Attack();
    void Update();
    SDL_Surface* GetCurrentFrame();
    bool CheckOverlap(shared_ptr<Monster> p2);
    Rect GetRect(bool justFeet = false);
    Point GetMidPoint(bool fromFeet = false);

    static void ChaseAI(Monster* pMonster);

    int x;
    int y;
    int width;
    int characterWidth;
    int height;
    bool facingRight;
    function<void(Monster*)> AI;
    bool walking;
    bool attacking;
    queue<DIRECTION> directions;
    World* pWorld;
    int lastMoveToX;
    int lastMoveToY;
    shared_ptr<Monster> chasingMonster;
    ALIGNMENT alignment;

private:
    SDL_Surface* imgCurrentFrame;
    map<ANIMATION, Animation> animations;
    unsigned int tickCounter;
};


#endif
