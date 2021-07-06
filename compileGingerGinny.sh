#!/bin/bash
$CXX GingerGinny.cpp SDLColor.cpp SDLGame.cpp Monster.cpp Animation.cpp World.cpp SDLFont.cpp AStar.cpp -O3 -o GingerGinny/GingerGinny.elf -lSDL -lSDL_ttf -lSDL_image -lSDL_gfx -lSDL_mixer -lmikmod
rm GingerGinny.opk
mksquashfs ./GingerGinny GingerGinny.opk
