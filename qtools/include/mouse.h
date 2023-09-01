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
#ifndef _MOUSE_H_
#define _MOUSE_H_

#include "typedefs.h"

class Mouse
{
public:
    static int speedX, speedY;
    static int rangeX, rangeY;
    static int X, Y;
    static int dX, dY;
    static int dX2, dY2;
    static int dfX, dfY;
    static byte buttons;
    static double acceleration;

    static void SetRange(int, int);
    static void Read(int);
};

#endif
