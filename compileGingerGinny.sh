#!/bin/bash
$CXX GingerGinny.cpp SDLColor.cpp SDLGame.cpp Monster.cpp Animation.cpp World.cpp -O3 -o GingerGinny/GingerGinny.elf -lSDL -lSDL_ttf -lSDL_image -lSDL_gfx
rm GingerGinny.opk
mksquashfs ./GingerGinny GingerGinny.opk
