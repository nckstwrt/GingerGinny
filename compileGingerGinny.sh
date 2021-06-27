#!/bin/bash
$CXX GingerGinny.cpp SDLColor.cpp SDLGame.cpp Monster.cpp -o GingerGinny/GingerGinny.elf -lSDL -lSDL_ttf -lSDL_image -lSDL_gfx
mksquashfs ./GingerGinny GingerGinny.opk
