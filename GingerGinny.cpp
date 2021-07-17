#include <time.h>
#include "SDLGame.h"
#include "SDLColor.h"
#include "Monster.h"
#include "World.h"
#include <SDL/SDL_mixer.h>

//#define PLAY_MUSIC

int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));

    SDLGame Game;
    World World(&Game);
    SDLFont fonts;

    // Harry Potter Music :)
    Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
        return -1;
    Mix_Music* musicTMNT = NULL;
#ifdef PLAY_MUSIC
    musicTMNT = Mix_LoadMUS("tmnt.ogg");
#endif
    if (musicTMNT != NULL)
        Mix_PlayMusic(musicTMNT, 0);

    Game.SetupScreen(SCREEN_WIDTH, SCREEN_HEIGHT, false);

    SDL_Surface* hw_surface = Game.GetSurface();

    auto harryPotterSprites = Game.LoadImage("images/pixel_harry_potter_sprites_by_mudkat101-da3f0nk_small2.png");

    Monster ginny;
    Animation *idleAnim = ginny.AddAnimationImages(ANIMATION::Idle, 12, 2, "images/idle_000_vsmall.png", Game.LoadImage("images/idle_000_vsmall.png"), Game.LoadImage("images/idle_001_vsmall.png"));
    idleAnim->SetAnimationDelay(0, 200);
    ginny.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/walk_000_vsmall.png"), Game.LoadImage("images/walk_001_vsmall.png"), Game.LoadImage("images/walk_002_vsmall.png"), Game.LoadImage("images/walk_003_vsmall.png"));
    ginny.AddAnimationImages(ANIMATION::Attack, 4, 5, "", Game.LoadImage("images/attack_000_vsmall.png"), Game.LoadImage("images/attack_001_vsmall.png"), Game.LoadImage("images/attack_002_vsmall.png"), Game.LoadImage("images/attack_003_vsmall.png"), Game.LoadImage("images/attack_004_vsmall.png"));

    Monster ogre;
    ogre.AddAnimationImages(ANIMATION::Idle, 6, 4, "ogre_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f3.png"));
    ogre.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f3.png"));

    Monster demon;
    demon.AddAnimationImages(ANIMATION::Idle, 6, 4, "big_demon_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f3.png"));
    demon.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f3.png"));

    Monster zombie;
    zombie.AddAnimationImages(ANIMATION::Idle, 6, 4, "big_zombie_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f3.png"));
    zombie.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f3.png"));

    Monster hagrid;
    SDL_Surface* hagridImg = Game.GetImageFromSheet(harryPotterSprites, 150, 234, 190 - 150, 280 - 234);
    hagrid.AddAnimationImages(ANIMATION::Idle, 1, 1, "", hagridImg);
    
    ogre.characterWidth = 16;
    World.AddMonsterTemplate(ogre);

    World.LoadMap("map2_version3.txt");

    // Ginny is monster 0
    shared_ptr<Monster> pGinny = World.AddMonster(ginny, 6, 8, true, ALIGNMENT::GOOD);
    pGinny->characterWidth = 22;
    World.pCameraFollow = pGinny;
    
    shared_ptr<Monster> pOgre = World.AddMonster(ogre, 10, 18, false, ALIGNMENT::BAD);
    pOgre->characterWidth = 16;

    shared_ptr<Monster> pHagrid = World.AddMonster(hagrid, 12, 6, true, ALIGNMENT::NEUTRAL);
    /*
    World.AddMonster(ogre, 180, 110, false);
    World.AddMonster(demon, 30, 150, true);
    World.AddMonster(demon, 110, 190, false);
    World.AddMonster(zombie, 140, 20, true);
    World.AddMonster(zombie, 200, 10, false);
    */

    auto magicFont = fonts.LoadFont("fonts/MagicSchoolOne.ttf", 16);
    auto yoster = fonts.LoadFont("fonts/MagicSchoolOne.ttf", 16);
    auto chary = fonts.LoadFont("fonts/chary.ttf", 18);

    // Start Screen
    bool runGame = true;
    auto pTitleScreen = Game.LoadImage("images/TeenageMutantGingerGinny.png");
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

    Game.ResetKeys();

    // Stop TMNT theme start hp theme
    while (!Mix_FadeOutMusic(300) && Mix_PlayingMusic())
        SDL_Delay(50);
    Mix_Music* musicHP = NULL;
#ifdef PLAY_MUSIC
    musicHP = Mix_LoadMUS("hp.ogg");
    if (musicHP == NULL)
        printf("HP Music Error: %s\n", Mix_GetError());
#endif
    if (musicHP != NULL)
      Mix_PlayMusic(musicHP, -1);

    // Run Game
    bool showText = true;
    vector<Point> points;
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
        if (Game.keys[SDLK_b])
        {
            //pOgre->MoveTo(pGinny->x+(pGinny->width/2), (pGinny->y+pGinny->height)-10);
            points = World.MonsterMoveTo(pOgre, pGinny->GetMidPoint(true).x, pGinny->GetMidPoint(true).y);
            Game.keys[SDLK_b] = false;
        }
        if (Game.keys[SDLK_c])
        {
            shared_ptr<Monster> temp = pGinny;
            pGinny = pOgre;
            pOgre = temp;
            World.pCameraFollow = pGinny;
            Game.keys[SDLK_c] = false;
        }
        if (Game.keys[SDLK_e])
        {
            int r = 50; // radius
            int ox = pOgre->x, oy = pOgre->y; // origin

            points.clear();
            for (int x = -r; x < r; x++)
            {
                int height = (int)sqrt(r * r - x * x);

                for (int y = -height; y < height; y++)
                {
                    points.push_back({ x + ox, y + oy });
                }
            }
            Game.keys[SDLK_e] = false;
        }

        World.Update();
        World.Draw();

        for (auto& point : points)
        {
            pixelRGBA(hw_surface, World.PixelXToDisplayPixelX(point.x), World.PixelYToDisplayPixelY(point.y), 255, 0, 0, 255);
        }

        //Game.BlitImage(harryPotterSprites, 3, 6, 32-3, 45-6, 30, 30);


        if (showText)
        {
            if (World.DrawTextBox(chary, "Teenage Mutant Ginger Ginny is here. And she is.... Evil!") == -1)
                break;
            if (World.DrawTextBox(chary, "....Or is she?") == -1)
                break;
            if (World.DrawTextBox(chary, "(yes she is!)") == -1)
                break;
            showText = false;
        }

        // Switch buffers and add frame rate control
        Game.FlipScreen();
        Game.FrameRateDelay();
    }

    Mix_FreeMusic(musicTMNT);
    Mix_FreeMusic(musicHP);
    return 0;
}