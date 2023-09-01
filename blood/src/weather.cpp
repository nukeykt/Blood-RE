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
#include "build.h"
#include "debug4g.h"
#include "trig.h"
#include "misc.h"
#include "weather.h"

#define kNoTile -1

CWeather gWeather;

CWeather::CWeather()
{
    f_0.b = 0;
    f_4 = NULL;
    f_8 = 0;
    f_c = 0;
    f_10 = 0;
    f_14 = 0;
    memset(YLookup, 0, sizeof(YLookup));
    f_12d8 = 0;
    f_12da = 0;
    f_12dc = -1;
    memset(f_12de, 0, sizeof(f_12de));
    f_72de = 0;
    f_72df = 0;
}

CWeather::~CWeather()
{
    f_0.b = 0;
    f_4 = NULL;
    f_8 = 0;
    f_c = 0;
    f_10 = 0;
    f_14 = 0;
    memset(YLookup, 0, sizeof(YLookup));
    f_12d8 = 0;
    f_12da = 0;
    f_12dc = -1;
    memset(f_12de, 0, sizeof(f_12de));
    f_72de = 0;
    f_72df = 0;
}

CWeather::RandomizeVectors(void)
{
    for (int i = 0; i < 4096; i++)
    {
        f_12de[i][0] = krand()&0x3fff;
        f_12de[i][1] = krand()&0x3fff;
        f_12de[i][2] = krand()&0x3fff;
    }
}

CWeather::SetDefaultBuffer(char *a1, int a2, int a3, int *a4)
{
    f_4 = a1;
    f_8 = a2;
    f_c = a3;
    memcpy(YLookup, a4, sizeof(YLookup));
}

CWeather::SetParticles(short nCount, short a2, short nTile)
{
    dassert(nCount >= 0 && nCount < kMaxVectors, 81);
    dassert(nTile == kNoTile || (nTile >= 0 && nTile < kMaxTiles), 82);
    f_12d8 = nCount;
    f_12da = a2;
    f_12dc = nTile;
}

CWeather::SetTranslucency(int a1)
{
    f_0.f_1 = ClipRange(a1, 0, 2);
}

CWeather::SetColor(char a1)
{
    f_72de = ClipRange(a1, 0, 255);
}

CWeather::SetColorShift(char a1)
{
    f_72df = ClipRange(a1, 0, 2);
}

CWeather::Initialize(char *pBuffer, int a2, int a3, int a4, int a5, int *pYLookup, short nCount, short a8, short nTile)
{
    dassert(pBuffer != NULL, 107);
    dassert(pYLookup != NULL, 108);
    dassert(nCount >= 0 && nCount < kMaxVectors, 109);
    dassert(nTile == kNoTile || (nTile >= 0 && nTile < kMaxTiles), 110);
    f_0.f_0 = 1;
    RandomizeVectors();
    SetDefaultBuffer(pBuffer, a4, a5, pYLookup);
    f_10 = a2;
    f_14 = a3;
    SetParticles(nCount, a8, nTile);
}

CWeather::Draw(char *pBuffer, int x, int y, int a4, int a5, int *pYLookup, int a7, int a8, int a9, int a10, int a11, int nCount, int nTile)
{
    dassert(pBuffer != NULL, 122);
    dassert(pYLookup != NULL, 123);
    dassert(nCount > 0 && nCount < kMaxVectors, 124);
    dassert(nTile == kNoTile || (nTile >= 0 && nTile < kMaxTiles), 125);
    pBuffer += pYLookup[y]+x;
    int nCos = Cos(a10)>>16;
    int nSin = Sin(a10)>>16;
    for (int i = 0; i < nCount; i++)
    {
        int v1 = ((f_12de[i][1]-a7*2)&0x3fff)-0x2000;
        int v2 = ((f_12de[i][0]-a8*2)&0x3fff)-0x2000;
        int v3 = (v1*nCos+v2*nSin)>>14;
        if (v3 > 4)
        {
            int v4 = (v2*nCos-v1*nSin)>>14;
            if (v3*v3+v4*v4 < 0x4000000)
            {
                int v5 = (a4<<15)/v3;
                unsigned int v6 = (((v4*v5)>>16)+(a4>>1));
                if (v6 < (unsigned int) a4)
                {
                    int v7 = (((a9>>3)-f_12de[i][2])&0x3fff)-0x2000;
                    unsigned int v8 = a11+((v5*v7)>>16);
                    if (v8 < (unsigned int) a5 && nTile == kNoTile)
                    {
                        if (f_0.f_1 == 0)
                        {
                            v3 >>= f_72df+8;
                            v3 = f_72de-v3;
                            char *pDest = pBuffer + pYLookup[v8] + v6;
                            *pDest = v3;
                        }
                        else
                        {
                            v3 >>= f_72df+8;
                            char *pDest = pBuffer + pYLookup[v8] + v6;
                            byte p1 = f_72de-v3;
                            byte p2 = *pDest;
                            if (f_0.f_1 == 1)
                                *pDest = transluc[(p1<<8)+p2];
                            else if (f_0.f_1 == 2)
                                *pDest = transluc[(p2<<8)+p1];
                        }
                    }
                }
                f_12de[i][2] += f_12da;
            }
        }
    }
}

CWeather::Draw(int a1, int a2, int a3, int a4, int a5, int a6)
{
    if (a6 == -1)
        a6 = f_12d8;
    if (Status() && a6 > 0)
        Draw(f_4, f_10, f_14, f_8, f_c, YLookup, a1, a2, a3, a4, a5, a6, f_12dc);
}

CWeather::SetWeatherType(WEATHERTYPE a1)
{
    switch (a1)
    {
    case WEATHERTYPE_1:
        SetTranslucency(2);
        f_12da = 128;
        SetColor(128);
        break;
    case WEATHERTYPE_2:
        SetTranslucency(0);
        f_12da = 32;
        SetColor(32);
        break;
    case WEATHERTYPE_3:
        SetTranslucency(0);
        f_12da = 84;
        SetColor(152);
        SetColorShift(2);
        break;
    default:
        f_12d8 = 0;
        break;
    }
}
