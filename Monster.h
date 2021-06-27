#ifndef _MONSTER_H
#define _MONSTER_H

#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>

enum DIRECTION
{
    Left,
    Right,
    Up,
    Down
};

class Monster
{
public:
    Monster();
    ~Monster();

    void SetStillImage(SDL_Surface* imgStill);
    void AddWalkImages(int walkImageCount, ...);

    void Move(DIRECTION direction);
    void Update();
    SDL_Surface* GetCurrentFrame();

    int x;
    int y;

private:
    SDL_Surface* imgCurrentFrame;

    SDL_Surface* imgStillRight;
    SDL_Surface* imgStillLeft;

    int walkImageCount;
    SDL_Surface** imgWalkRight;
    SDL_Surface** imgWalkLeft;

    bool facingRight;
    bool walking;
    int walkingFrame;
    int walkingFrameDelay;
};


#endif
