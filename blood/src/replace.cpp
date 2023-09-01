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
#include <stdlib.h>
#include "typedefs.h"
#include "build.h"
#include "crc32.h"
#include "globals.h"
#include "misc.h"
#include "resource.h"
#include "screen.h"
#include "tile.h"


void uninitcache()
{
}

extern "C" void agecache()
{
}

extern "C" void initcache()
{
}

extern "C" void allocache()
{
}

extern "C" void *kmalloc(long size)
{
    return Resource::Alloc(size);
}

extern "C" void kfree(void *pMem)
{
    Resource::Free(pMem);
}

extern "C" int loadpics()
{
    return tileInit(0, NULL) ? 0 : - 1;
}

extern "C" void loadtile(short nTile)
{
    tileLoadTile(nTile);
}

extern "C" void allocatepermanenttile(short a1, int a2, int a3)
{
    tileAllocTile(a1, a2, a3);
}

void overwritesprite (long thex, long they, short tilenum,
    signed char shade, char stat, char dapalnum)
{
   rotatesprite(thex<<16,they<<16,65536L,(stat&8)<<7,tilenum,shade,dapalnum,
      ((stat&1^1)<<4)+(stat&2)+((stat&4)>>2)+((stat&16)>>2)^((stat&8)>>1),
      windowx1,windowy1,windowx2,windowy2);
}

enum {
    kFakevarFlat = 0x8000,
    kFakevarMask = 0xc000,
};

extern "C" int animateoffs(short a1, ushort a2)
{
    int offset = 0;
    int frames;
    int vd;
    if (a1 < 0 || a1 >= kMaxTiles)
        return offset;
    frames = picanm[a1].animframes;
    if (frames > 0)
    {
        if ((a2&0xc000) == 0x8000)
            vd = (CRC32(&a2, 2)+gFrameClock)>>picanm[a1].animspeed;
        else
            vd = gFrameClock>>picanm[a1].animspeed;
        switch (picanm[a1].animtype)
        {
        case 1:
            offset = vd % (2*frames);
            if (offset >= frames)
                offset = 2*frames-offset;
            break;
        case 2:
            offset = vd % (frames+1);
            break;
        case 3:
            offset = -(vd % (frames+1));
            break;
        }
    }
    return offset;
}

extern "C" void uninitengine()
{
    tileTerm();
}

extern "C" void loadpalette()
{
    scrLoadPalette();
}

extern "C" int getpalookup(int a1, int a2)
{
    if (gFogMode)
        return ClipHigh(a1>>8, 15)*16+ClipRange(a2>>2, 0, 15);
    else
        return ClipRange((a1>>8)+a2, 0, 63);
}
