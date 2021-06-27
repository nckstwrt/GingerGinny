#include <SDL/SDL.h>

class SDLColor
{
public:
    SDLColor();
    SDLColor(int r, int g, int b);

    operator SDL_Color()
    {
        return color;
    }

    SDL_Color color;
};