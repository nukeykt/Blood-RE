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
#include <stdarg.h>
#include <stdio.h>
#include "typedefs.h"
#include "misc.h"
#include "textio.h"

int tioScreenRows;
int tioScreenCols;
int tioStride;

static struct {
    int f_0;
    int f_4;
    char f_8;
    char f_9;
} cursor;

static struct {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
} window;

byte nAttribute;

static char* Frames[] = {
    "\xDA\xC4\xBF\xB3\x20\xB3\xC0\xC4\xD9",
    "\xC9\xCD\xBB\xBA\x20\xBA\xC8\xCD\xBC",
    "\xDB\xDF\xDB\xDB\x20\xDB\xDB\xDC\xDB"
};

byte ShadeTable[] = {
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x88, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0, 0, 0, 0, 0, 0, 0, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x18, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x48, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x58, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x68, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x78, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77
};

static inline byte * tioVideoAddress(int x,int y)
{
    return (byte*)0xb8000 + (x * tioScreenCols + y) * 2;
}

void WriteString(int a1, int a2, char *s, byte a4)
{
    byte *va = tioVideoAddress(a1, a2);
    for (; *s; s++)
    {
        *va++ = *s;
        *va++ = a4;
    }
}

void tioInit(int a1)
{
    tioScreenCols = *(short*)0x44a;
    tioScreenRows = *(byte*)0x484 + 1;
    tioStride = tioScreenCols * 2;
    cursor.f_8 = *(byte*)0x460;
    cursor.f_9 = *(byte*)0x461;
    cursor.f_4 = *(byte*)0x450;
    cursor.f_0 = *(byte*)0x451;
    tioCursorOff();
    tioSetAttribute(7);
    tioWindow(0, 0, tioScreenRows, tioScreenCols);
    if (a1 & 1)
        tioClearWindow();
}

void tioCursorSet(int ch, int cl);
#pragma aux tioCursorSet = \
"mov ah,1" \
"int 0x10" \
parm nomemory [ch] [cl]

void tioCursorOff(void)
{
    tioCursorSet(0x20, 0);
}

void tioTerm(void)
{
    tioCursorSet(cursor.f_8, cursor.f_9);
}

void tioFill(int a1, int a2, int a3, int a4, byte a5, byte a6)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = tioVideoAddress(window.f_0 + a1, window.f_4 + a2);
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va++ = a5;
            *va++ = a6;
        }
        va += tioStride - a4 * 2;
    }
}

void tioFillChar(int a1, int a2, int a3, int a4, byte a5)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = tioVideoAddress(window.f_0 + a1, window.f_4 + a2);
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va = a5;
            va += 2;
        }
        va += tioStride - a4 * 2;
    }
}

void tioFillAttr(int a1, int a2, int a3, int a4, byte a5)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = tioVideoAddress(window.f_0 + a1, window.f_4 + a2) + 1;
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va = a5;
            va += 2;
        }
        va += tioStride - a4 * 2;
    }
}

void tioFillShadow(int a1, int a2, int a3, int a4)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = tioVideoAddress(window.f_0 + a1, window.f_4 + a2) + 1;
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va = ShadeTable[*va];
            va += 2;
        }
        va += tioStride - a4 * 2;
    }
}

void tioFrame(int a1, int a2, int a3, int a4, byte a5, byte a6)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = tioVideoAddress(window.f_0 + a1, window.f_4 + a2);

    *va++ = Frames[a5][0];
    *va++ = a6;

    for (int i = 0; i < a4 - 2; i++)
    {
        *va++ = Frames[a5][1];
        *va++ = a6;
    }

    *va++ = Frames[a5][2];
    *va++ = a6;

    va += tioStride - a4 * 2;

    for (int j = 0; j < a3 - 2; j++)
    {
        *va++ = Frames[a5][3];
        *va++ = a6;
        va += (a4 - 2) * 2;
        *va++ = Frames[a5][5];
        *va++ = a6;
        va += tioStride - a4 * 2;
    }

    *va++ = Frames[a5][6];
    *va++ = a6;

    for (i = 0; i < a4 - 2; i++)
    {
        *va++ = Frames[a5][7];
        *va++ = a6;
    }

    *va++ = Frames[a5][8];
    *va++ = a6;
}

void tioLeftString(int a1, int a2, char *a3, byte a4)
{
    WriteString(window.f_0 + a1, window.f_4 + a2, a3, a4);
}

void tioCenterString(int a1, int a2, char *a3, byte a4)
{
    a2 -= (strlen(a3) >> 1);
    WriteString(window.f_0 + a1, window.f_4 + a2, a3, a4);
}

void tioRightString(int a1, int a2, char *a3, byte a4)
{
    a2 -= (strlen(a3) - 1);
    WriteString(window.f_0 + a1, window.f_4 + a2, a3, a4);
}

void tioLeftString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    int l = strlen(a4);
    memset(buf, 32, tioScreenCols);
    buf[a3 - a2 + 1] = 0;
    if (l > a3 - a2 + 1)
        memcpy(buf, a4, a3 - a2 + 1);
    else
        memcpy(buf, a4, l);
    WriteString(window.f_0 + a1, window.f_4 + a2, buf, a5);
}

void tioCenterString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    memset(buf, 32, tioScreenCols);
    buf[a3 - a2 + 1] = 0;
    int vd = (a3 - a2 + 1 - strlen(a4)) >> 1;
    if (vd < 0)
        memcpy(buf, a4 - vd, a3 - a2 + 1);
    else
        memcpy(buf + vd, a4, strlen(a4));
    WriteString(window.f_0 + a1, window.f_4 + a2, buf, a5);
}

void tioRightString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    memset(buf, 32, tioScreenCols);
    buf[a3 - a2 + 1] = 0;
    int vd = a3 - a2 + 1 - strlen(a4);
    if (vd < 0)
        memcpy(buf, a4 - vd, a3 - a2 + 1);
    else
        memcpy(buf + vd, a4, strlen(a4));
    WriteString(window.f_0 + a1, window.f_4 + a2, buf, a5);
}

void tioWindow(int a1, int a2, int a3, int a4)
{
    window.f_0 = a1;
    window.f_4 = a2;
    window.f_8 = a3;
    window.f_c = a4;
}

void tioCursorPos(int dh, int dl);
#pragma aux tioCursorPos = \
"mov ah,2" \
"mov bh,0" \
"int 0x10" \
parm nomemory [dh] [dl]

void tioSetPos(int a1, int a2)
{
    cursor.f_0 = a1;
    cursor.f_4 = a2;
    tioCursorPos(a1 + window.f_0, a2 + window.f_4);
}

void tioScroll(int a1, byte a2, int a3, int a4, int a5, int a6);
#pragma aux tioScroll = \
"mov ah,6" \
"int 0x10" \
parm nomemory [al] [bh] [ch] [cl] [dh] [dl]

void tioClearWindow(void)
{
    tioScroll(0, nAttribute, window.f_0, window.f_4, window.f_0 + window.f_8 - 1, window.f_4 + window.f_c - 1);
    tioSetPos(0, 0);
}

byte tioSetAttribute(byte a1)
{
    byte old = nAttribute;
    nAttribute = a1;
    return old;
}

void tioPrint(char *s, ...)
{
    char buf[256];
    va_list arg;
    va_start(arg, s);
    vsprintf(buf, s, arg);
    va_end(arg);
    WriteString(window.f_0 + cursor.f_0, window.f_4 + cursor.f_4, buf, nAttribute);
    cursor.f_0++;
    cursor.f_4 = 0;
    if (cursor.f_0 >= window.f_8)
    {
        tioScroll(1, nAttribute, window.f_0, window.f_4, window.f_0 + window.f_8 - 1, window.f_4 + window.f_c - 1);
        cursor.f_0--;
    }
    tioSetPos(cursor.f_0, cursor.f_4);
}

int tioGauge(int a1, int a2)
{
    char buf[128];
    static char gauge[33];
    int vc = a1 * 32 / a2;
    int vbp = a1 * 100 / a2;
    int v4 = (a1 * 64 / a2) & 1;
    memset(gauge, 0xf9, 32);
    memset(gauge, 0xdb, vc);
    gauge[vc] = "\xF9\xDD"[v4];
    gauge[32] = 0;
    sprintf(buf, "%s  %3d%%", gauge, vbp);
    WriteString(window.f_0 + cursor.f_0, window.f_4 + cursor.f_4, buf, nAttribute);
    if (a1 < a2)
        return 1;

    tioPrint("");
    return 0;
}
