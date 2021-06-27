#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "SDLGame.h"
#include "SDLColor.h"
#include "Monster.h"
#include <vector>
using namespace std;

class World
{
public:
    void AddMonster(Monster newMonster, int x, int y, bool facingRight)
    {
        newMonster.x = x;
        newMonster.y = y;
        newMonster.facingRight = facingRight;
        newMonster.AI = true;
        monsters.push_back(newMonster);
    }

    void Update()
    {
        for (vector<Monster>::iterator iter = monsters.begin(); iter != monsters.end(); iter++)
        {
            iter->Update();
        }
    }

    void Draw(SDLGame *game)
    {
        for (vector<Monster>::iterator iter = monsters.begin(); iter != monsters.end(); iter++)
        {
            game->BlitImage(iter->GetCurrentFrame(), iter->x, iter->y);
        }
    }

    vector<Monster> monsters;
};

int main(int argc, char* argv[])
{
    srand(time(NULL));

    SDLGame Game;
    World World;

    Game.SetupScreen(240, 240, false);

    SDL_Surface* hw_surface = Game.GetSurface();

    Monster ginny;
    Animation *idleAnim = ginny.AddAnimationImages(ANIMATION::Idle, 12, 2, Game.LoadImage("images/idle_000_vsmall.png"), Game.LoadImage("images/idle_001_vsmall.png"));
    idleAnim->SetAnimationDelay(0, 200);
    ginny.AddAnimationImages(ANIMATION::Walk, 6, 4, Game.LoadImage("images/walk_000_vsmall.png"), Game.LoadImage("images/walk_001_vsmall.png"), Game.LoadImage("images/walk_002_vsmall.png"), Game.LoadImage("images/walk_003_vsmall.png"));
    ginny.AddAnimationImages(ANIMATION::Attack, 4, 5, Game.LoadImage("images/attack_000_vsmall.png"), Game.LoadImage("images/attack_001_vsmall.png"), Game.LoadImage("images/attack_002_vsmall.png"), Game.LoadImage("images/attack_003_vsmall.png"), Game.LoadImage("images/attack_004_vsmall.png"));
    ginny.x = 0;
    ginny.y = 50;

    Monster ogre;
    ogre.AddAnimationImages(ANIMATION::Idle, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f3.png"));
    ogre.AddAnimationImages(ANIMATION::Walk, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f3.png"));

    Monster demon;
    demon.AddAnimationImages(ANIMATION::Idle, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f3.png"));
    demon.AddAnimationImages(ANIMATION::Walk, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f3.png"));

    Monster zombie;
    zombie.AddAnimationImages(ANIMATION::Idle, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f3.png"));
    zombie.AddAnimationImages(ANIMATION::Walk, 10, 4, Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f3.png"));

    World.AddMonster(ogre, 50, 90, true);
    World.AddMonster(ogre, 180, 110, false);
    World.AddMonster(demon, 30, 150, true);
    World.AddMonster(demon, 110, 190, false);
    World.AddMonster(zombie, 140, 20, true);
    World.AddMonster(zombie, 200, 10, false);

    SDL_Surface* floor1 = Game.LoadImage("images/DungeonTilesetII_v1.4/floor_1.png");

    TTF_Font* ParryHotter = Game.LoadFont("fonts/ParryHotter.ttf", 48);

    SDL_Surface* textSurface = Game.CreateTextSurface(ParryHotter, "Hello World!", SDLColor(0, 255, 0));

    while (true)
    {
        Game.ClearScreen();

        /*
        // Draw a green square in the middle of the screen
        SDL_Rect draw_rect = { .x = (Sint16)x, .y = 70, .w = 100, .h = 100 };
        Uint32 color = SDL_MapRGB(hw_surface->format, 0, 255, 0);
        SDL_FillRect(hw_surface, &draw_rect, color);
        */

        for (int y = 0; y < 15; y++)
        {
            for (int x = 0; x < 15; x++)
            {
                Game.BlitImage(floor1, 0, 0, 16, 16, x*16, y*16);
            }
        }

        Game.BlitImage(textSurface, 0, 0);

        World.Update();
        World.Draw(&Game);

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q] || Game.keys[SDLK_ESCAPE])
            break;

        if (Game.keys[SDLK_LEFT] || Game.keys[SDLK_l])
        {
            ginny.Move(DIRECTION::Left);
        }
        if (Game.keys[SDLK_RIGHT] || Game.keys[SDLK_r])
        {
            ginny.Move(DIRECTION::Right);
        }
        if (Game.keys[SDLK_UP] || Game.keys[SDLK_u])
        {
            ginny.Move(DIRECTION::Up);
        }
        if (Game.keys[SDLK_DOWN] || Game.keys[SDLK_d])
        {
            ginny.Move(DIRECTION::Down);
        }
        if (Game.keys[SDLK_a])
        {
            ginny.Attack();
        }

        ginny.Update();
        Game.BlitImage(ginny.GetCurrentFrame(), ginny.x, ginny.y);

        // Switch buffers to show the square we just drew
        Game.FlipScreen();
        Game.FrameRateDelay();
    }

    return 0;
}