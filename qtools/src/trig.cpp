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
#include <string.h>
#include "typedefs.h"
#include "error.h"
#include "misc.h"
#include "resource.h"
#include "trig.h"

long costable[2048];

int OctantTable[8] = { 5, 6, 2, 1, 4, 7, 3, 0 };

int GetOctant(int x, int y)
{
    int vc = klabs(x)-klabs(y);
    int t = 7 + isneg(x) + isneg(y) * 2 + isneg(vc) * 4;
    return OctantTable[t];
}

void RotateVector(long *dx, long *dy, int nAngle)
{
    int ox = *dx;
    int oy = *dy;
    int sn = Sin(nAngle);
    int cs = Cos(nAngle);
    *dx = dmulscale30r(ox, cs, -oy, sn);
    *dy = dmulscale30r(ox, sn, oy, cs);
}

void RotatePoint(long *x, long *y, int nAngle, int ox, int oy)
{
    int dx = *x-ox;
    int dy = *y-oy;
    int sn = Sin(nAngle);
    int cs = Cos(nAngle);
    *x = ox+dmulscale30r(dx, cs, -dy, sn);
    *y = oy+dmulscale30r(dx, sn, dy, cs);
}

void trigInit(Resource &Res)
{
    DICTNODE *pTable = Res.Lookup("cosine","dat");
    if (!pTable)
        ThrowError(65)("Cosine table not found");
    if (Resource::Size(pTable) != 2048)
        ThrowError(67)("Cosine table incorrect size");
    memcpy(costable, Res.Load(pTable), pTable->size);
    costable[512] = 0;
    for (int i = 513; i <= 1024; i++)
    {
        costable[i] = -costable[1024-i];
    }
    for (i = 1025; i < 2048; i++)
    {
        costable[i] = costable[2048 - i];
    }
}
