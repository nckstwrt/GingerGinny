#include "SDLGame.h"
#include "SDLColor.h"
#include "Monster.h"

int main(int argc, char* argv[])
{
    SDLGame Game;

    Game.SetupScreen(240, 240, false);

    SDL_Surface* hw_surface = Game.GetSurface();

    Monster ginny;
    ginny.SetStillImage(Game.LoadImage("images/idle_000_small.png"));
    ginny.AddWalkImages(4, Game.LoadImage("images/walk_000_small.png"), Game.LoadImage("images/walk_001_small.png"), Game.LoadImage("images/walk_002_small.png"), Game.LoadImage("images/walk_003_small.png"));
    ginny.x = 0;
    ginny.y = 50;

    SDL_Surface* floor1 = Game.LoadImage("images/DungeonTilesetII_v1.4/floor_1.png");

    TTF_Font* ParryHotter = Game.LoadFont("fonts/ParryHotter.ttf", 48);

    SDL_Surface* textSurface = Game.CreateTextSurface(ParryHotter, "Hello World!", SDLColor(0, 255, 0));

    int x = 0;
    int y = 50;
    bool facingRight = true;
    int walkingFrame = 0;
    bool walking = false;
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

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q])
            break;

        walking = false;
        if (Game.keys[SDLK_LEFT] || Game.keys[SDLK_l])
        {
            ginny.Move(Left);
        }
        if (Game.keys[SDLK_RIGHT] || Game.keys[SDLK_r])
        {
            ginny.Move(Right);
        }
        if (Game.keys[SDLK_UP] || Game.keys[SDLK_u])
        {
            ginny.Move(Up);
        }
        if (Game.keys[SDLK_DOWN] || Game.keys[SDLK_d])
        {
            ginny.Move(Down);
        }

        ginny.Update();
        Game.BlitImage(ginny.GetCurrentFrame(), ginny.x, ginny.y);

        // Switch buffers to show the square we just drew
        Game.FlipScreen();
        Game.FrameRateDelay();
    }

    return 0;
}