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
#include <math.h>
#include "typedefs.h"
#include "misc.h"
#include "mouse.h"

int Mouse::speedX = 80;
int Mouse::speedY = 160;
double Mouse::acceleration = 1.12;
int Mouse::rangeX = 320;
int Mouse::rangeY = 200;

int Mouse::X = Mouse::rangeX >> 1;
int Mouse::Y = Mouse::rangeY >> 1;
int Mouse::dX, Mouse::dY;
int Mouse::dX2, Mouse::dY2;
int Mouse::dfX, Mouse::dfY;
byte Mouse::buttons;

void Mouse::SetRange(int x, int y)
{
    dfX |= X << 16;
    dfY |= Y << 16;

    int sx, sy;

    dfX = sx = kscale(dfX, x, rangeX);
    dfY = sy = kscale(dfY, y, rangeY);
    rangeX = x;
    rangeY = y;
    X = sx >> 16;
    dfX = sx & 0xffff;
    Y = sy >> 16;
    dfY = sy & 0xffff;
}

byte ReadButtons(void);
#pragma aux ReadButtons = \
"mov ax,3" \
"int 0x33" \
"xor eax,eax" \
"mov al,bl" \
modify exact [eax ebx ecx edx]

void ReadDeltas(int *dx, int *dy);
#pragma aux ReadDeltas = \
"mov ax,0xb" \
"int 0x33" \
"movsx ecx,cx" \
"movsx edx,dx" \
"mov [esi],ecx" \
"mov [edi],edx" \
parm [esi][edi] \
modify exact [eax ebx ecx edx]



void Mouse::Read(int a1)
{
    int dx, dy;
    buttons = ReadButtons();
    ReadDeltas(&dx, &dy);
    if (a1 > 0)
    {
        dx = ksgn(dx) * a1 * pow((double)klabs(dx) / a1, acceleration);
        dy = ksgn(dy) * a1 * pow((double)klabs(dy) / a1, acceleration);
    }
    dx *= speedX;
    dy *= speedY;
    dfX += dx * rangeX;
    dfY += dy * rangeY;
    dX2 = dfX >> 16;
    dY2 = dfY >> 16;
    dfX &= 0xffff;
    dfY &= 0xffff;
    dX = X;
    dY = Y;
    X = ClipRange(X + dX2, 0, rangeX - 1);
    Y = ClipRange(Y + dY2, 0, rangeY - 1);
    dX = X - dX;
    dY = Y - dY;
}
