#include "SDLFont.h"

SDLFont::SDLFont()
{
}

SDLFont::~SDLFont()
{
    for (TTF_Font* font : loadedFonts)
        TTF_CloseFont(font);
}

TTF_Font* SDLFont::LoadFont(const char* szFont, int fontSize)
{
    auto font = TTF_OpenFont(szFont, fontSize);
    loadedFonts.push_back(font);
    return font;
}

vector<SDL_Surface*> SDLFont::OutputText(TTF_Font* font, string text, SDLColor color, int maxLength, int* outHeight)
{
    vector<SDL_Surface*> createdSurfaces;
    auto words = SplitString(text);

    string lineString = "";
    string oldString;
    int width, height;

    for (string word : words)
    {
        oldString = lineString;
        lineString += word;
        TTF_SizeText(font, lineString.c_str(), &width, &height);
        if (width > maxLength)
        {
            auto newSurface = TTF_RenderText_Blended(font, oldString.c_str(), color);
            if (outHeight)
                *outHeight = height;
            createdSurfaces.push_back(newSurface);
            lineString = word;
        }
        lineString += " ";
    }

    if (lineString != "")
    {
        auto newSurface = TTF_RenderText_Blended(font, lineString.c_str(), color);
        if (outHeight)
            *outHeight = (height * 6) / 8;
        createdSurfaces.push_back(newSurface);
    }

    return createdSurfaces;
}

void SDLFont::FreeSurfaces(vector<SDL_Surface*> surfaces)
{
    for (auto surface : surfaces)
    {
        SDL_FreeSurface(surface);
    }
}

vector<string> SDLFont::SplitString(const string& s, char delim)
{
    vector<string> result;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim))
        result.push_back(item);

    return result;
}
