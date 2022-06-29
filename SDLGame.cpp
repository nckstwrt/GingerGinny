#include "SDLGame.h"

SDLGame::SDLGame() : fpsManager{ 0 }, surface_buffer(NULL), lastKeyPressed(0)
{
    for (int i = 0; i < SDLK_LAST; i++)
        keys[i] = false;
}

SDLGame::~SDLGame()
{
    for (map<string, SDL_Surface*>::iterator iter = imageMap.begin(); iter != imageMap.end(); iter++)
    {
        SDL_FreeSurface(iter->second);
    }
    for (auto &img : createdImages)
    {
        SDL_FreeSurface(img);
    }

    if (surface_buffer != NULL && real_surface != surface_buffer)
    {
        SDL_FreeSurface(surface_buffer);
    }

    if (real_surface != NULL)
    {
        SDL_FreeSurface(real_surface);
        SDL_Quit();
    }
}

bool SDLGame::SetupScreen(int width, int height, bool fullScreen, bool useHWSurface, bool upsideDown)
{
    screen_width = width;
    screen_height = height;
    this->upsideDown = upsideDown;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    real_surface = SDL_SetVideoMode(width, height, 32, (useHWSurface ? (SDL_HWSURFACE | SDL_DOUBLEBUF) : 0) | (fullScreen ? SDL_FULLSCREEN : 0));

    if (useHWSurface)
        surface_buffer = real_surface;
    else
    {
        surface_buffer = SDL_CreateRGBSurface(real_surface->flags, real_surface->w, real_surface->h, 32, 0, 0, 0, 0);
    }

    SDL_ShowCursor(0);
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 50);
    return (surface_buffer != NULL);
}

void SDLGame::ClearScreen()
{
    SDL_FillRect(surface_buffer, NULL, 0x000000);
}

void SDLGame::ClearScreen(SDLColor c)
{
    SDL_FillRect(surface_buffer, NULL, c);
}

void SDLGame::FlipScreen()
{
    if (upsideDown)
    {
        SDL_Surface* flippedSurface = rotozoomSurfaceXY(surface_buffer, 0, -1, -1, SMOOTHING_OFF);
        SDL_FillRect(real_surface, NULL, 0x000000);
        SDL_BlitSurface(flippedSurface, NULL, real_surface, NULL);
        SDL_Flip(real_surface);
        SDL_FreeSurface(flippedSurface);
    }
    else
    { 
        SDL_FillRect(real_surface, NULL, 0x000000);
        SDL_BlitSurface(surface_buffer, NULL, real_surface, NULL);
        SDL_Flip(real_surface);
    }
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

    if (strrchr(szImageFile, '/') != NULL)
        imageMap[strrchr(szImageFile, '/') + 1] = ret;
    else
        imageMap[szImageFile] = ret;

    return ret;
}

SDL_Surface* SDLGame::GetLoadedImage(const char* szImageShortName, const char *szLoadFromDirectory)
{
    SDL_Surface* ret = NULL;
    if (imageMap.find(szImageShortName) != imageMap.end())
        ret = imageMap[szImageShortName];
    else
    {
        if (szLoadFromDirectory != NULL)
        {
            string dirFile = szLoadFromDirectory;
            dirFile += "/";
            dirFile += szImageShortName;
            ret = LoadImage(dirFile.c_str());
        }
    }
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
    if (img)
    {
        SDL_Rect targetRect;
        targetRect.x = x;
        targetRect.y = y;
        targetRect.w = img->w;
        targetRect.h = img->h;
        SDL_BlitSurface(img, NULL, surface_buffer, &targetRect);
    }
}

void SDLGame::BlitImage(SDL_Surface* img, int srcX, int srcY, int w, int h, int x, int y)
{
    if (img)
    {
        SDL_Rect sourceRect = { .x = (Sint16)srcX, .y = (Sint16)srcY, .w = (Uint16)w, .h = (Uint16)h };
        SDL_Rect targetRect = { .x = (Sint16)x, .y = (Sint16)y, .w = (Uint16)w, .h = (Uint16)h };
        SDL_BlitSurface(img, &sourceRect, surface_buffer, &targetRect);
    }
}

SDL_Surface* SDLGame::GetImageFromSheet(SDL_Surface* img, int srcX, int srcY, int w, int h)
{
    SDL_Rect sourceRect = { .x = (Sint16)srcX, .y = (Sint16)srcY, .w = (Uint16)w, .h = (Uint16)h };
    SDL_Rect targetRect = { .x = (Sint16)0, .y = (Sint16)0, .w = (Uint16)w, .h = (Uint16)h };
    SDL_Surface *newSurface = SDL_CreateRGBSurface(0, w, h, img->format->BytesPerPixel, img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask);
    SDL_Surface* newSurfaceConverted = SDL_DisplayFormatAlpha(newSurface);
    SDL_FreeSurface(newSurface);

    SDL_SetAlpha(img, 0, 0);
    SDL_BlitSurface(img, &sourceRect, newSurfaceConverted, &targetRect);

    createdImages.push_back(newSurfaceConverted);

    return newSurfaceConverted;
}

SDL_Surface* SDLGame::CreateHorizontallyFlippedImage(SDL_Surface* img)
{
    auto newImg = rotozoomSurfaceXY(img, 0, -1, 1, SMOOTHING_OFF);
    createdImages.push_back(newImg);
    return newImg;
}

SDL_Surface* SDLGame::CreateVerticallyFlippedImage(SDL_Surface* img)
{
    auto newImg = rotozoomSurfaceXY(img, 0, 1, -1, SMOOTHING_OFF);
    createdImages.push_back(newImg);
    return newImg;
}

void SDLGame::DrawRect(int x, int y, int width, int height, SDLColor color, RECTANGLE_TYPE rectangleType, int rad)
{
    switch (rectangleType)
    {
    case RECTANGLE_TYPE::BOX:
        rectangleColor(surface_buffer, x, y, x + width, y + height, color.ReversedUInt());
        break;
    case RECTANGLE_TYPE::FILLED:
        boxColor(surface_buffer, x, y, x + width, y + height, color.ReversedUInt());
        break;
    case RECTANGLE_TYPE::BOX_ROUNDED:
        roundedRectangleColor(surface_buffer, x, y, x + width, y + height, rad, color.ReversedUInt());
        break;
    case RECTANGLE_TYPE::FILLED_ROUNDED:
        roundedBoxColor(surface_buffer, x, y, x + width, y + height, rad, color.ReversedUInt());
        break;
    }
}

void SDLGame::DrawPoint(int x, int y, SDLColor color)
{
    pixelColor(surface_buffer, x, y, color.ReversedUInt());
}

void SDLGame::FrameRateDelay()
{
    SDL_framerateDelay(&fpsManager);
}

SDL_Surface* SDLGame::GetSurface()
{
    return surface_buffer;
}

TTF_Font* SDLGame::LoadFont(const char* szFont, int fontSize)
{
    return fonts.LoadFont(szFont, fontSize);
}

void SDLGame::OutputText(TTF_Font* font, const char* szText, SDLColor col, int x, int y, int maxWidth)
{
    int lineHeight = 0;
    auto lines = fonts.OutputText(font, szText, col, maxWidth, &lineHeight);
    for (auto &line : lines)
    {
        BlitImage(line, x, y);
        y += lineHeight;
    }
    fonts.FreeSurfaces(lines);
}

SDL_Surface* SDLGame::CreateTextSurface(TTF_Font* font, const char* szText, SDL_Color col)
{
    return TTF_RenderText_Blended(font, szText, col);
}

void SDLGame::ResetKeys()
{
    memset(&keys, 0, sizeof(keys));
}

int SDLGame::GetWidth()
{
    return screen_width;
}

int SDLGame::GetHeight()
{
    return screen_height;
}
