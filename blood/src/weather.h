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
#ifndef _WEATHER_H_
#define _WEATHER_H_

#include "typedefs.h"
#include "misc.h"

#define kMaxVectors 4096

enum WEATHERTYPE {
    WEATHERTYPE_0,
    WEATHERTYPE_1,
    WEATHERTYPE_2,
    WEATHERTYPE_3,
};

class CWeather {
public:
    CWeather();
    ~CWeather();
    RandomizeVectors(void);
    SetDefaultBuffer(char *a1, int a2, int a3, int *a4);
    SetParticles(short nCount, short a2, short nTile);
    SetTranslucency(int);
    SetColor(char a1);
    SetColorShift(char);
    Initialize(char *, int, int, int, int, int *, short, short, short);
    Draw(char *pBuffer, int x, int y, int a4, int a5, int *pYLookup, int a7, int a8, int a9, int a10, int a11, int nCount, int nTile);
    Draw(int a1, int a2, int a3, int a4, int a5, int a6);
    SetWeatherType(WEATHERTYPE);

    short GetCount(void) {
        return f_12d8;
    }

    void SetCount(short t) {
        f_12d8 = ClipRange(t, 0, 4095);
    }

    BOOL Status(void) {
        return f_0.f_0 ? 1 : 0;
    }

    union {
        byte b;
        struct {
            unsigned int f_0 : 1;
            unsigned int f_1 : 2;
        };
    } f_0;
    char *f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int YLookup[1200]; // at18
    short f_12d8;
    short f_12da;
    short f_12dc;
    short f_12de[kMaxVectors][3];
    char f_72de;
    char f_72df;
};

extern CWeather gWeather;


#endif // !_WEATHER_H_
