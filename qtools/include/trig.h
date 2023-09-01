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
#ifndef _TRIG_H_
#define _TRIG_H_
#include "typedefs.h"
#include "resource.h"
extern long costable[2048];

int GetOctant(int x, int y);
void RotateVector(long *dx, long *dy, int nAngle);
void RotatePoint(long *x, long *y, int nAngle, int ox, int oy);
void trigInit(Resource &Res);

inline long Sin(int ang)
{
    return costable[(ang - 512) & 2047];
}

inline long Cos(int ang)
{
    return costable[ang & 2047];
}

#endif
