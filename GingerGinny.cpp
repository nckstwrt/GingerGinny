#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include "SDLGame.h"
#include "SDLColor.h"
#include "Monster.h"
#include "World.h"
#include <SDL/SDL_mixer.h>

#define PLAY_MUSIC
#define TMNT_MUSIC_FILE "tmnt.wav"
#define HP_MUSIC_FILE "hp.wav"

FILE* fout;

void log(const char* fmt, ...)
{
    char printString[1000];
    va_list argp;
    va_start(argp, fmt);
    vsprintf(printString, fmt, argp);
    va_end(argp);
    fprintf(fout, printString);
    fflush(fout);
}

int main(int argc, char* argv[])
{
    fout = fopen("logfile.txt", "wt");
    fprintf(fout, "Hello World!\n");
    fflush(fout);

    int screen_width = 240;
    int screen_height = 240;
    bool fullScreen = false;
    bool useHWSurface = true;
    bool upsideDown = false;

    srand((unsigned int)time(NULL));

    SDLGame Game;
    World World(&Game);
    SDLFont fonts;

    // Harry Potter Music :)
    int mix_init_ret = Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
    log("mix_init_ret: %d\n", mix_init_ret);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 4096) == -1)
        return -1;
    Mix_Volume(-1, MIX_MAX_VOLUME);
    Mix_Music* musicTMNT = NULL;
#ifdef PLAY_MUSIC
    musicTMNT = Mix_LoadMUS(TMNT_MUSIC_FILE);
#endif
    if (musicTMNT != NULL)
    {
        if (Mix_PlayMusic(musicTMNT, 0) == -1)
        {
            log("Mix_PlayMusic: %s\n", Mix_GetError());
        }
    }
    else
    {
        log("Could not find or load tmnt.ogg!\n");
        log("Mix_LoadMUS: %s\n", Mix_GetError());
    }

    if (argc == 3)
    {
        screen_width = atoi(argv[1]);
        screen_height = atoi(argv[2]);
        fullScreen = false;
        useHWSurface = false;
        upsideDown = true;
    }

    Game.SetupScreen(screen_width, screen_height, fullScreen, useHWSurface, upsideDown);

    SDL_Surface* hw_surface = Game.GetSurface();

    auto harryPotterSprites = Game.LoadImage("images/pixel_harry_potter_sprites_by_mudkat101-da3f0nk_small2.png");

    Monster ginny(&World);
    Animation *idleAnim = ginny.AddAnimationImages(ANIMATION::Idle, 12, 2, "idle_000_vsmall.png", Game.LoadImage("images/idle_000_vsmall.png"), Game.LoadImage("images/idle_001_vsmall.png"));
    idleAnim->SetAnimationDelay(0, 200);
    ginny.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/walk_000_vsmall.png"), Game.LoadImage("images/walk_001_vsmall.png"), Game.LoadImage("images/walk_002_vsmall.png"), Game.LoadImage("images/walk_003_vsmall.png"));
    ginny.AddAnimationImages(ANIMATION::Attack, 4, 5, "", Game.LoadImage("images/attack_000_vsmall.png"), Game.LoadImage("images/attack_001_vsmall.png"), Game.LoadImage("images/attack_002_vsmall.png"), Game.LoadImage("images/attack_003_vsmall.png"), Game.LoadImage("images/attack_004_vsmall.png"));
    ginny.AddAnimationImages(ANIMATION::Hurt, 8, 3, "", Game.LoadImage("images/hurt_000_vsmall.png"), Game.LoadImage("images/hurt_001_vsmall.png"), Game.LoadImage("images/hurt_002_vsmall.png"));
    auto deathAnim = ginny.AddAnimationImages(ANIMATION::Death, 8, 4, "", Game.LoadImage("images/hurt_003_vsmall.png"), Game.LoadImage("images/hurt_004_vsmall.png"), Game.LoadImage("images/hurt_005_vsmall.png"), Game.LoadImage("images/hurt_006_vsmall.png"));
    deathAnim->SetAnimationDelay(3, 200);
    ginny.characterWidth = 16;
    World.AddMonsterTemplate(ginny, ALIGNMENT::GOOD);

    Monster ogre(&World);
    ogre.AddAnimationImages(ANIMATION::Idle, 6, 4, "DungeonTilesetII_v1.4/ogre_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_idle_anim_f3.png"));
    ogre.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/ogre_run_anim_f3.png"));
    ogre.characterWidth = 16;
    ogre.health = 2;
    World.AddMonsterTemplate(ogre);

    Monster orc_warrior(&World);
    orc_warrior.AddAnimationImages(ANIMATION::Idle, 6, 4, "DungeonTilesetII_v1.4/orc_warrior_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_idle_anim_f3.png"));
    orc_warrior.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/orc_warrior_run_anim_f3.png"));
    orc_warrior.characterWidth = 16;
    orc_warrior.health = 3;
    World.AddMonsterTemplate(orc_warrior);

    Monster demon(&World);
    demon.AddAnimationImages(ANIMATION::Idle, 6, 4, "DungeonTilesetII_v1.4/big_demon_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_idle_anim_f3.png"));
    demon.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_demon_run_anim_f3.png"));
    demon.characterWidth = 16;
    demon.health = 3;
    World.AddMonsterTemplate(demon);

    Monster zombie(&World);
    zombie.AddAnimationImages(ANIMATION::Idle, 6, 4, "DungeonTilesetII_v1.4/big_zombie_idle_anim_f0.png", Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_idle_anim_f3.png"));
    zombie.AddAnimationImages(ANIMATION::Walk, 6, 4, "", Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f0.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f1.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f2.png"), Game.LoadImage("images/DungeonTilesetII_v1.4/big_zombie_run_anim_f3.png"));
    zombie.characterWidth = 16;
    zombie.health = 2;
    World.AddMonsterTemplate(zombie);

    /*
    Monster hagrid(&World);
    SDL_Surface* hagridImg = Game.GetImageFromSheet(harryPotterSprites, 150, 234, 190 - 150, 280 - 234);
    hagrid.AddAnimationImages(ANIMATION::Idle, 1, 1, "", hagridImg);
*/    

    World.LoadMap("map2_version6.txt");

    // Load Hearts
    auto fullHeart = Game.LoadImage("images/DungeonTilesetII_v1.4/ui_heart_full.png");
    auto halfHeart = Game.LoadImage("images/DungeonTilesetII_v1.4/ui_heart_half.png");
    auto emptyHeart = Game.LoadImage("images/DungeonTilesetII_v1.4/ui_heart_empty.png");
    /*
    // Ginny is monster 0
    shared_ptr<Monster> pGinny = World.AddMonster(ginny, 6, 8, true, ALIGNMENT::GOOD);
    pGinny->characterWidth = 22;
    World.pCameraFollow = pGinny;
    
    shared_ptr<Monster> pOgre = World.AddMonster(ogre, 10, 18, false, ALIGNMENT::BAD);
    pOgre->characterWidth = 16;

    shared_ptr<Monster> pHagrid = World.AddMonster(hagrid, 12, 6, true, ALIGNMENT::NEUTRAL);
    */
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

        Game.BlitImage(pTitleScreen, (Game.GetWidth() / 2) - (pTitleScreen->w / 2), (Game.GetHeight() / 5));

        if (((counter++) % 4) == 0)
            color = SDLColor(rand() % 255, rand() % 255, rand() % 255);
        SDL_Surface* pressStart = Game.CreateTextSurface(ParryHotter, "Press Start", color);
        Game.BlitImage(pressStart, Game.GetWidth() / 2 - pressStart->w / 2, (Game.GetHeight() / 3) * 2);

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
    musicHP = Mix_LoadMUS(HP_MUSIC_FILE);
    if (musicHP == NULL)
        printf("HP Music Error: %s\n", Mix_GetError());
#endif
    if (musicHP != NULL)
      Mix_PlayMusic(musicHP, -1);

    // Run Game
    bool showText = true;
    while (runGame)
    {
        Game.ClearScreen();

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q] || Game.keys[SDLK_ESCAPE])
            break;

        if (World.pCameraFollow->health > 0)
        {
            if (Game.keys[SDLK_LEFT] || Game.keys[SDLK_l])
            {
                World.MonsterMove(World.pCameraFollow, DIRECTION::Left);
            }
            if (Game.keys[SDLK_RIGHT] || Game.keys[SDLK_r])
            {
                World.MonsterMove(World.pCameraFollow, DIRECTION::Right);
            }
            if (Game.keys[SDLK_UP] || Game.keys[SDLK_u])
            {
                World.MonsterMove(World.pCameraFollow, DIRECTION::Up);
            }
            if (Game.keys[SDLK_DOWN] || Game.keys[SDLK_d])
            {
                World.MonsterMove(World.pCameraFollow, DIRECTION::Down);
            }

            // https://steward-fu.github.io/website/handheld/miyoo-mini/sdlkey.htm
            if (Game.keys[SDLK_a] || Game.keys[SDLK_SPACE] || Game.keys[SDLK_LCTRL])
            {
                World.MonsterAttack(World.pCameraFollow);
            }

            /*
            for (int k = 0; k < 323; k++)
            {
                if (Game.keys[k] != 0)
                {
                    fprintf(fout, "Key Down: %d", k);
                    fflush(fout);
                }
            }*/
        }
        if (Game.keys[SDLK_p])
        {
            World.pCameraFollow->health--;
            Game.keys[SDLK_p] = false;
        }
       
        World.Update();
        World.Draw();

        // Draw Health Hearts
        int maxHealth = 6;
        for (int hearts = 0; hearts < (maxHealth/2); hearts++)
        {
            int heartX = 5 + ((((maxHealth / 2)-1) - hearts) * (fullHeart->w + 4));
            int watershed = (maxHealth - 1) - (hearts * 2);
            auto drawHeart = fullHeart;

            if (World.pCameraFollow->health == watershed)
                drawHeart = halfHeart;
            if (World.pCameraFollow->health < watershed)
                drawHeart = emptyHeart;

            Game.BlitImage(drawHeart, heartX, (hw_surface->h - 5) - drawHeart->h);
        }

        if (showText)
        {
            if (World.DrawTextBox(chary, "Teenage Mutant Ginger Ginny is here. And she is.... Evil!", GINNY_COLOR) == -1)
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