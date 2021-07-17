#ifndef _SDLGAME_H
#define _SDLGAME_H
#define _CRT_SECURE_NO_WARNINGS

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_framerate.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "SDLColor.h"
#include "SDLFont.h"
#include <map>
#include <string>
using namespace std;

enum class RECTANGLE_TYPE
{
    BOX,
    FILLED,
    BOX_ROUNDED,
    FILLED_ROUNDED
};

class SDLGame
{
public:
    SDLGame();
    ~SDLGame();

    bool SetupScreen(int width, int height, bool fullScreen);
    void ClearScreen();
    void ClearScreen(SDLColor c);
    void FlipScreen();
    SDL_Event PollEvents();
    SDL_Surface* LoadImage(const char* szImageFile);
    SDL_Surface* GetLoadedImage(const char* szImageShortName, const char* szLoadFromDirectory = "images");
    void FreeImage(const char* szImageFile);
    void BlitImage(SDL_Surface* img, int x, int y);
    void BlitImage(SDL_Surface* img, int srcX, int srcY, int w, int h, int x, int y);
    SDL_Surface* GetImageFromSheet(SDL_Surface* img, int srcX, int srcY, int w, int h);
    static SDL_Surface* CreateHorizontallyFlippedImage(SDL_Surface* img);
    void DrawRect(int x, int y, int width, int height, SDLColor color, RECTANGLE_TYPE rectangleType = RECTANGLE_TYPE::FILLED, int rad = 5);
    void DrawPoint(int x, int y, SDLColor color);
    void FrameRateDelay();
    SDL_Surface* GetSurface();
    TTF_Font* LoadFont(const char* szFont, int fontSize);
    void OutputText(TTF_Font* font, const char* szText, SDLColor col, int x, int y, int maxWidth);
    SDL_Surface* CreateTextSurface(TTF_Font* font, const char* szText, SDL_Color col);
    void ResetKeys();

    bool keys[SDLK_LAST];
    int lastKeyPressed;
private:
    SDL_Surface* hw_surface;
    SDLFont fonts;
    FPSmanager fpsManager;
    map<string, SDL_Surface*> imageMap;
    static vector<SDL_Surface*> createdImages;
};

/*
MAP FN       TO KEY     KEY_K
MAP START    TO KEY     KEY_S
MAP UP       TO KEY     KEY_U
MAP LEFT     TO KEY     KEY_L
MAP FN+LEFT  TO KEY     KEY_J
MAP DOWN     TO KEY     KEY_D
MAP FN+DOWN  TO KEY     KEY_H
MAP RIGHT    TO KEY     KEY_R
MAP FN+RIGHT TO KEY     KEY_I
MAP R        TO KEY     KEY_N
MAP L        TO KEY     KEY_M
MAP A        TO KEY     KEY_A
MAP B        TO KEY     KEY_B
MAP X        TO KEY     KEY_X
MAP Y        TO KEY     KEY_Y
MAP MENU     TO KEY     KEY_Q
*/

#endif