#include "SDLGame.h"

SDLGame::SDLGame() : fpsManager{ 0 }, hw_surface(NULL), lastKeyPressed(0)
{
    for (int i = 0; i < SDLK_LAST; i++)
        keys[i] = false;
}

SDLGame::~SDLGame()
{
    if (hw_surface != NULL)
    {
        SDL_FreeSurface(hw_surface);
        SDL_Quit();
    }
}

bool SDLGame::SetupScreen(int width, int height, bool fullScreen)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    hw_surface = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | (fullScreen ? SDL_FULLSCREEN : 0));
    SDL_ShowCursor(0);
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 50);
    return (hw_surface != NULL);
}

void SDLGame::ClearScreen()
{
    SDL_FillRect(hw_surface, NULL, 0x000000);
}

void SDLGame::FlipScreen()
{
    SDL_Flip(hw_surface);
}

SDL_Event SDLGame::PollEvents()
{
    SDL_Event event = { 0 };
    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            lastKeyPressed = event.key.keysym.sym;
            keys[event.key.keysym.sym] = (event.type == SDL_KEYDOWN);
        }
    }
    return event;
}

SDL_Surface* SDLGame::LoadImage(const char* szImageFile)
{
    SDL_Surface* ret = NULL;
    SDL_Surface* img = IMG_Load(szImageFile);
    if (img != NULL)
    {
        ret = SDL_DisplayFormatAlpha(img);
        SDL_FreeSurface(img);
    }
    else
        printf("SDLGame::LoadImage - failed to load: %s\n", szImageFile);

    imageMap[szImageFile] = img;

    return ret;
}

void SDLGame::FreeImage(const char* szImageFile)
{
    map<string,SDL_Surface*>::iterator iter = imageMap.find(szImageFile);
    if (iter != imageMap.end())
    {
        SDL_FreeSurface(iter->second);
        imageMap.erase(iter);
    }
}

void SDLGame::BlitImage(SDL_Surface* img, int x, int y)
{
    SDL_Rect targetRect;
    targetRect.x = x;
    targetRect.y = y;
    targetRect.w = img->w;
    targetRect.h = img->h;
    SDL_BlitSurface(img, NULL, hw_surface, &targetRect);
}

void SDLGame::BlitImage(SDL_Surface* img, int srcX, int srcY, int w, int h, int x, int y)
{
    SDL_Rect sourceRect = { .x = (Sint16)srcX, .y = (Sint16)srcY, .w = (Uint16)w, .h = (Uint16)h };
    SDL_Rect targetRect = { .x = (Sint16)x, .y = (Sint16)y, .w = (Uint16)w, .h = (Uint16)h };
    SDL_BlitSurface(img, &sourceRect, hw_surface, &targetRect);
}

void SDLGame::DrawRect(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 color = SDL_MapRGB(hw_surface->format, r, g, b);
    SDL_Rect draw_rect = { .x = (Sint16)x, .y = (Sint16)y, .w = (Uint16)width, .h = (Uint16)height };
    SDL_FillRect(hw_surface, &draw_rect, color);
}

void SDLGame::FrameRateDelay()
{
    SDL_framerateDelay(&fpsManager);
}

SDL_Surface* SDLGame::GetSurface()
{
    return hw_surface;
}

TTF_Font* SDLGame::LoadFont(const char* szFont, int fontSize)
{
    return TTF_OpenFont(szFont, fontSize);
}

SDL_Surface* SDLGame::CreateTextSurface(TTF_Font* font, const char* szText, SDL_Color col)
{
    TTF_SetFontOutline(font, 1);
    return TTF_RenderText_Blended(font, szText, col);
}
