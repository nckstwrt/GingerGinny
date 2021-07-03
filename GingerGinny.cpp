#include <time.h>
#include "SDLGame.h"
#include "SDLColor.h"
#include "Monster.h"
#include "World.h"
#include <SDL/SDL_mixer.h>

int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    SDLGame Game;
    World World(&Game);

    // Harry Potter Music :)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        return -1;
    Mix_VolumeMusic(25);
    Mix_Music* musicTMNT = Mix_LoadMUS("tmnt.ogg");
    if (musicTMNT != NULL)
        Mix_PlayMusic(musicTMNT, 0);

    Game.SetupScreen(SCREEN_WIDTH, SCREEN_HEIGHT, false);
    World.LoadMap("map1.txt");

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

    // Ginny is monster 0
    Monster *pGinny = World.AddMonster(ginny, 50, 50, true);
    /*
    World.AddMonster(ogre, 50, 90, true);
    World.AddMonster(ogre, 180, 110, false);
    World.AddMonster(demon, 30, 150, true);
    World.AddMonster(demon, 110, 190, false);
    World.AddMonster(zombie, 140, 20, true);
    World.AddMonster(zombie, 200, 10, false);
    */

    // Start Screen
    bool runGame = true;
    auto pTitleScreen = Game.LoadImage("TeenageMutantGingerGinny.png");
    TTF_Font* ParryHotter = Game.LoadFont("fonts/ParryHotter.ttf", 36);
    int counter = 0;
    SDLColor color;
    while (true)
    {
        Game.ClearScreen(SDLColor(34,34,34));

        Game.BlitImage(pTitleScreen, 0, 25);

        if (((counter++) % 4) == 0)
            color = SDLColor(rand() % 255, rand() % 255, rand() % 255);
        SDL_Surface* pressStart = Game.CreateTextSurface(ParryHotter, "Press Start", color);
        Game.BlitImage(pressStart, SCREEN_WIDTH / 2 - pressStart->w / 2, 140);

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q] || Game.keys[SDLK_ESCAPE])
        {
            runGame = false;
            break;
        }

        if (Game.keys[SDLK_RETURN] || Game.keys[SDLK_s])
            break;

        Game.FlipScreen();
        Game.FrameRateDelay();
        SDL_FreeSurface(pressStart);
    }

    // Stop TMNT theme start hp theme
    Mix_FadeOutMusic(100);
    Mix_VolumeMusic(128);
    Mix_Music* musicHP = Mix_LoadMUS("hp.ogg");
    if (musicHP != NULL)
        Mix_PlayMusic(musicHP, -1);

    // Run Game
    while (runGame)
    {
        Game.ClearScreen();

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q] || Game.keys[SDLK_ESCAPE])
            break;

        if (Game.keys[SDLK_LEFT] || Game.keys[SDLK_l])
        {
            World.MonsterMove(pGinny, DIRECTION::Left);
        }
        if (Game.keys[SDLK_RIGHT] || Game.keys[SDLK_r])
        {
            World.MonsterMove(pGinny, DIRECTION::Right);
        }
        if (Game.keys[SDLK_UP] || Game.keys[SDLK_u])
        {
            World.MonsterMove(pGinny, DIRECTION::Up);
        }
        if (Game.keys[SDLK_DOWN] || Game.keys[SDLK_d])
        {
            World.MonsterMove(pGinny, DIRECTION::Down);
        }
        if (Game.keys[SDLK_a])
        {
            World.MonsterAttack(pGinny);
        }

        World.Update();
        World.Draw();

        // Switch buffers and add frame rate control
        Game.FlipScreen();
        Game.FrameRateDelay();
    }

    Mix_FreeMusic(musicTMNT);
    Mix_FreeMusic(musicHP);
    return 0;
}