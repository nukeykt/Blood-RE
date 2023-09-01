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
#include "choke.h"
#include "error.h"
#include "globals.h"
#include "levels.h"
#include "misc.h"
#include "player.h"
#include "resource.h"

CChoke::CChoke(int _x, int _y, char *a1, void (*a2)(CChoke*, PLAYER*))
{
    f_14 = _x;
    f_18 = _y;
    f_0 = a1;
    f_1c = a2;
    if (!f_4 && f_0)
    {
        f_4 = gSysRes.Lookup(f_0, "QAV");
        if (!f_4)
            ThrowError(43)("Could not load QAV %s\n", f_0);
        f_8 = (QAV*)gSysRes.Lock(f_4);
        f_8->x = f_14;
        f_8->y = f_18;
        f_8->Preload();
        f_c = f_8->at10;
        f_10 = totalclock;
    }
}

void CChoke::func_83ff0(int a1, void(*a2)(CChoke*, PLAYER*))
{
    f_0 = NULL;
    f_1c = a2;
    if (!f_4 && a1 != -1)
    {
        f_4 = gSysRes.Lookup(a1, "QAV");
        if (!f_4)
            ThrowError(65)("Could not load QAV %d\n", a1);
        f_8 = (QAV*)gSysRes.Lock(f_4);
        f_8->x = f_14;
        f_8->y = f_18;
        f_8->Preload();
        f_c = f_8->at10;
        f_10 = totalclock;
    }
}

void CChoke::func_84080(char *a1, void(*a2)(CChoke*, PLAYER*))
{
    f_0 = a1;
    f_1c = a2;
    if (!f_4 && f_0)
    {
        f_4 = gSysRes.Lookup(f_0, "QAV");
        if (!f_4)
            ThrowError(65)("Could not load QAV %s\n", f_0);
        f_8 = (QAV*)gSysRes.Lock(f_4);
        f_8->x = f_14;
        f_8->y = f_18;
        f_8->Preload();
        f_c = f_8->at10;
        f_10 = totalclock;
    }
}

void CChoke::func_84110(int x, int y)
{
    if (!f_4)
        return;
    int v4 = gFrameClock;
    gFrameClock = gGameClock;
    f_8->x = x;
    f_8->y = y;
    int vd = totalclock-f_10;
    f_10 = totalclock;
    f_c -= vd;
    if (f_c <= 0 || f_c > f_8->at10)
        f_c = f_8->at10;
    int vdi = f_8->at10-f_c;
    f_8->Play(vdi-vd, vdi, -1, NULL);
    int vb = windowx1;
    int v10 = windowy1;
    int vc = windowx2;
    int v8 = windowy2;
    windowx1 = windowy1 = 0;
    windowx2 = xdim-1;
    windowy2 = ydim-1;
    f_8->Draw(vdi, 10, 0, 0);
    windowx1 = vb;
    windowy1 = v10;
    windowx2 = vc;
    windowy2 = v8;
    gFrameClock = v4;
}

void CChoke::func_84218()
{
    f_c = f_8->at10;
    f_10 = totalclock;
}

void func_84230(CChoke *, PLAYER *pPlayer)
{
    int t = gGameOptions.nDifficulty+2;
    if (pPlayer->at372 < 64)
        pPlayer->at372 = ClipHigh(pPlayer->at372+t, 64);
    if (pPlayer->at372 > (35-gGameOptions.nDifficulty*5))
        pPlayer->at36a = ClipHigh(pPlayer->at36a+t*4, 128);
}

CChoke gChoke;
