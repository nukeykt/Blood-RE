/*
 * Copyright (C) 2018, 2022 nukeykt
 *
 * This file is part of Blood-RE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _QAV_H_
#define _QAV_H_

#include "typedefs.h"
#include "build.h"

struct TILE_FRAME
{
    int picnum;
    int x;
    int y;
    int z;
    int stat;
    signed char shade;
    byte palnum;
    ushort angle;
};

struct SOUNDINFO
{
    int sound;
    unsigned char at4;
    char pad[3];
};

struct FRAMEINFO
{
    int nCallbackId; // 0
    SOUNDINFO sound; // 4
    TILE_FRAME tiles[8]; // 12
};

struct QAV
{
    char sign[4]; // 0
    byte pad1[4]; // 4
    int nFrames; // 8
    int ticksPerFrame; // C
    int at10; // 10
    int x; // 14
    int y; // 18
    SPRITE *pSprite; // 1c
    byte pad3[4]; // 20
    FRAMEINFO frames[1]; // 24
    void Draw(long ticks, int stat, int shade, int palnum);
    void Play(long, long, int, void *);
    void Preload(void);

    void PlaySound(int nSound);
    void PlaySound3D(SPRITE *pSprite, int nSound, int a3, int a4);
};

int qavRegisterClient(void(*pClient)(int, void *));


#endif
