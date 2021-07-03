#ifndef _SDLFONT_H
#define _SDLFONT_H

#include <SDL/SDL_ttf.h>
#include "SDLColor.h"
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

class SDLFont
{
public:
    SDLFont();
    ~SDLFont();

    TTF_Font* LoadFont(const char* szFont, int fontSize);
    vector<SDL_Surface*> OutputText(TTF_Font* font, string text, SDLColor color, int maxLength, int* outHeight);
    void FreeSurfaces(vector<SDL_Surface*> surfaces);

private:
    vector<string> SplitString(const string& s, char delim = ' ');
    vector<TTF_Font*> loadedFonts;
};


#endif

