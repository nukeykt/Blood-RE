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
#ifndef _MAP2D_H_
#define _MAP2D_H_

#include "build.h"
#include "typedefs.h"

class CViewMap {
public:
    BOOL bActive;
    int x, y, nZoom;
    short angle;
    BOOL bFollowMode;
    int forward, turn, strafe;
    CViewMap();
    void func_25C38(int, int, int, short, BOOL);
    void func_25C74(void);
    void func_25DB0(SPRITE *pSprite);
    void func_25E84(int*, int*);
    void FollowMode(BOOL);


    void SetPos(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void SetAngle(short a)
    {
        angle = a;
    }

    void SetInput(int f, int t, int s)
    {
        forward = f;
        turn = t;
        strafe = s;
    }

    void SetZoom(int z)
    {
        nZoom = z;
    }

    BOOL Mode(void)
    {
        return bFollowMode;
    }
};

extern CViewMap gViewMap;

#endif
