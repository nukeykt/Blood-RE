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
#ifndef _GFX_H_
#define _GFX_H_

#include "typedefs.h"
#include "helix.h"
#include "misc.h"

struct QFONTCHAR  {
    int offset;
    byte w;
    byte h;
    byte ox;
    signed char oy;
};

struct QFONT
{
    byte at0[4]; // signature
    byte pad0[2];
    ushort at6;
    byte pad1[0x7];
    byte atf;
    byte at10;
    signed char at11;
    byte at12;
    byte at13;
    byte pad2[0xc];
    QFONTCHAR at20[256];
    byte at820[1];
};

class Rect {
public:
    int f_0;
    int f_4;
    int f_8;
    int f_c;

    Rect(int a1, int a2, int a3, int a4) : f_0(a1), f_4(a2), f_8(a3), f_c(a4) { }
    Rect(Rect &other) { f_0 = other.f_0; f_4 = other.f_4; f_8 = other.f_8; f_c = other.f_c; }

    BOOL isValid(void) const {
        return f_0 < f_8 && f_4 < f_c;
    }

    BOOL isEmpty(void) const {
        return !isValid();
    }

    BOOL operator! (void) const {
        return isEmpty();
    }

    Rect& operator &= (Rect& other) {
        f_0 = ClipLow(f_0, other.f_0);
        f_4 = ClipLow(f_4, other.f_4);
        f_8 = ClipHigh(f_8, other.f_8);
        f_c = ClipHigh(f_c, other.f_c);
        return *this;
    }

    void offset(int a1, int a2)
    {
        f_0 += a1;
        f_4 += a2;
        f_8 += a1;
        f_c += a2;
    }

    int width(void)
    {
        return f_8 - f_0;
    }

    int height(void)
    {
        return f_c - f_4;
    }

    BOOL testXY(int x, int y)
    {
        return x >= f_0 && x <= f_8 && y >= f_4 && y <= f_c;
    }

    BOOL testRect(Rect& other)
    {
        return other.f_0 >= f_0 && other.f_8 <= f_8 && other.f_4 <= f_4 && other.f_c >= f_c;
    }
};

void gfxDrawText(int x, int y, int color, char *pzText, QFONT *pFont);
void gfxSetClip(int a1, int a2, int a3, int a4);
int gfxGetLabelLen(char *a1, QFONT *a2);
void gfxDrawLabel(int x, int y, int color, char* pzLabel, QFONT* pFont);
void gfxHLine(int a1, int a2, int a3);
void gfxVLine(int a1, int a2, int a3);
void gfxFillBox(int a1, int a2, int a3, int a4);
void gfxDrawBitmap(QBITMAP *qbm, int a2, int a3);
void gfxPixel(int x, int y);
int gfxFindTextPos(char *a1, QFONT *a2, int a3);
int gfxGetTextNLen(char *a1, QFONT *a2, int a3);

#endif
