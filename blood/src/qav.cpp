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
#include "typedefs.h"
#include "build.h"
#include "debug4g.h"
#include "misc.h"
#include "qav.h"
#include "tile.h"


#define kMaxClients 64
static void (*clientCallback[kMaxClients])(int, void *);
static int nClients;


int qavRegisterClient(void(*pClient)(int, void *))
{
    dassert(nClients < kMaxClients, 31);
    int id = nClients++;
    clientCallback[id] = pClient;

    return id;
}


static void DrawFrame(int x, int y, TILE_FRAME *pTile, int stat, int shade, int palnum)
{
    int angle = pTile->angle;
    byte pal;
    stat |= pTile->stat;
    if (stat & 0x100)
    {
        angle = (angle+1024)&2047;
        stat &= ~0x100;
        stat ^= 0x4;
    }
    pal = palnum > 0 ? (char)palnum : (char)pTile->palnum;
    rotatesprite((x + pTile->x) << 16, (y + pTile->y) << 16, pTile->z, angle,
                 pTile->picnum, ClipRange(pTile->shade + shade, -128, 127), pal, stat,
                 windowx1, windowy1, windowx2, windowy2);
}

void QAV::Draw(long ticks, int stat, int shade, int palnum)
{
    dassert(ticksPerFrame > 0, 72);
    int nFrame = ticks / ticksPerFrame;
    dassert(nFrame >= 0 && nFrame < nFrames, 74);
    FRAMEINFO *pFrame = &frames[nFrame];
    for (int i = 0; i < 8; i++)
    {
        if (pFrame->tiles[i].picnum > 0)
            DrawFrame(x, y, &pFrame->tiles[i], stat, shade, palnum);
    }
}

void QAV::Play(long start, long end, int nCallback, void *pData)
{
    dassert(ticksPerFrame > 0, 87);
    int frame;
    int ticks;
    if (start < 0)
        frame = (start + 1) / ticksPerFrame;
    else
        frame = start / ticksPerFrame + 1;
    
    for (ticks = ticksPerFrame * frame; ticks <= end; frame++, ticks += ticksPerFrame)
    {
        if (frame >= 0 && frame < nFrames)
        {
            FRAMEINFO *pFrame = &frames[frame];
            SOUNDINFO *pSound = &pFrame->sound;
            if (pSound->sound > 0)
            {
                if (!pSprite)
                    PlaySound(pSound->sound);
                else
                    PlaySound3D(pSprite, pSound->sound, 16+pSound->at4, 6);
            }
            if (pFrame->nCallbackId > 0 && nCallback != -1)
            {
                clientCallback[nCallback](pFrame->nCallbackId, pData);
            }
        }
    }
}

void QAV::Preload(void)
{
    for (int i = 0; i < nFrames; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (frames[i].tiles[j].picnum >= 0)
                tilePreloadTile(frames[i].tiles[j].picnum);
        }
    }
}
