#include "SDLGame.h"
#include "SDLColor.h"

int main(int argc, char* argv[])
{
    SDLGame Game;

    Game.SetupScreen(240, 240, false);

    SDL_Surface* hw_surface = Game.GetSurface();

    SDL_Surface* img = Game.LoadImage("images/idle_000_small.png");
    SDL_Surface* tileset = Game.LoadImage("images/16x16Tileset.png");

    SDL_Surface* img2 = rotozoomSurfaceXY(img, 0, -1, 1, SMOOTHING_OFF);

    TTF_Font* ParryHotter = Game.LoadFont("fonts/ParryHotter.ttf", 48);

    SDL_Surface* textSurface = Game.CreateTextSurface(ParryHotter, "Hello World!", SDLColor(0, 255, 0));

    int x = 0;
    int y = 50;
    bool facingRight = true;
    while (true)
    {
        if (x > 240 - 100)
            x = 0;

        Game.ClearScreen();

        /*
        // Draw a green square in the middle of the screen
        SDL_Rect draw_rect = { .x = (Sint16)x, .y = 70, .w = 100, .h = 100 };
        Uint32 color = SDL_MapRGB(hw_surface->format, 0, 255, 0);
        SDL_FillRect(hw_surface, &draw_rect, color);
        */

        for (int y = 0; y < 9; y++)
        {
            for (int x = 0; x < 9; x++)
            {
                Game.BlitImage(tileset, 534, 278, 36, 35, x*36, y*35);
            }
        }

        Game.BlitImage(textSurface, 0, 0);

        Game.BlitImage(facingRight ? img : img2, x, y);

        if (Game.PollEvents().type == SDL_QUIT || Game.keys[SDLK_q])
            break;

        if (Game.keys[SDLK_LEFT] || Game.keys[SDLK_l])
        {
            facingRight = false;
            x -= 1;
        }
        if (Game.keys[SDLK_RIGHT] || Game.keys[SDLK_r])
        {
            facingRight = true;
            x += 1;
        }
        if (Game.keys[SDLK_UP] || Game.keys[SDLK_u])
        {
            y -= 1;
        }
        if (Game.keys[SDLK_DOWN] || Game.keys[SDLK_d])
        {
            y += 1;
        }

        // Switch buffers to show the square we just drew
        Game.FlipScreen();
        Game.FrameRateDelay();
    }

    return 0;
}