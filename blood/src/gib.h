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
#ifndef _GIB_H_
#define _GIB_H_

#include "typedefs.h"
#include "build.h"

enum GIBTYPE {
    GIBTYPE_0 = 0,
    GIBTYPE_1,
    GIBTYPE_2,
    GIBTYPE_3,
    GIBTYPE_4,
    GIBTYPE_5,
    GIBTYPE_6,
    GIBTYPE_7,
    GIBTYPE_8,
    GIBTYPE_9,
    GIBTYPE_10,
    GIBTYPE_11,
    GIBTYPE_12,
    GIBTYPE_13,
    GIBTYPE_14,
    GIBTYPE_15,
    GIBTYPE_16,
    GIBTYPE_17,
    GIBTYPE_18,
    GIBTYPE_19,
    GIBTYPE_20,
    GIBTYPE_21,
    GIBTYPE_22,
    GIBTYPE_23,
    GIBTYPE_24,
    GIBTYPE_25,
    GIBTYPE_26,
    GIBTYPE_27,
    GIBTYPE_28,
    GIBTYPE_29,
    GIBTYPE_30,
    kGibMax
};

class CGibPosition {
public:
    int x, y, z;
    CGibPosition(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
};

class CGibVelocity {
public:
    int vx, vy, vz;
    CGibVelocity(int _vx, int _vy, int _vz) : vx(_vx), vy(_vy), vz(_vz) {}
};

void GibSprite(SPRITE *, GIBTYPE, CGibPosition *a3 = NULL, CGibVelocity *a4 = NULL);
void GibWall(int, GIBTYPE, CGibVelocity *a3 = NULL);

#endif
