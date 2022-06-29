#!/bin/bash
CXX="$CROSS_COMPILE"g++
$CXX GingerGinny.cpp -I /opt/miyoomini-toolchain/arm-linux-gnueabihf/libc/usr/include -I SDL/include -LMiyooMini -Ofast -fdata-sections -fdata-sections -fno-common -fno-PIC -flto SDLColor.cpp SDLGame.cpp Monster.cpp Animation.cpp World.cpp SDLFont.cpp AStar.cpp -O3 -o GingerGinny/GingerGinny.elf -lSDL -lSDL_ttf -lSDL_image -lSDL_gfx -lSDL_mixer -lmikmod
