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
#include <stdlib.h>
#include "typedefs.h"
#include "debug4g.h"
#include "misc.h"

enum {
    kDebugMode0,
    kDebugMode1,
    kDebugMode2,
} debugMode;

static struct {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
} window = { 0, 0, 25, 80 };

static struct {
    int f_0;
    int f_4;
} cursor;

static byte nAttribute = 7;

static const char* Frames[] = {
    "\xDA\xC4\xBF\xB3\x20\xB3\xC0\xC4\xD9",
    "\xC9\xCD\xBB\xBA\x20\xBA\xC8\xCD\xBC",
    "\xDB\xDF\xDB\xDB\x20\xDB\xDB\xDC\xDB"
};

static inline byte * DebugVideoAddress(int x,int y)
{
    return (byte*)0xb0000 + (x * 80 + y) * 2;
}

void monoFill(int a1, int a2, int a3, int a4, byte a5, byte a6)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a1, window.f_4 + a2);
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va++ = a5;
            *va++ = a6;
        }
        va += 160 - a4 * 2;
    }
}

void monoFillChar(int a1, int a2, int a3, int a4, byte a5)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a1, window.f_4 + a2);
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va = a5;
            va += 2;
        }
        va += 160 - a4 * 2;
    }
}

void monoFillAttr(int a1, int a2, int a3, int a4, byte a5)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a1, window.f_4 + a2) + 1;
    for (int i = 0; i < a3; i++)
    {
        for (int j = 0; j < a4; j++)
        {
            *va = a5;
            va += 2;
        }
        va += 160 - a4 * 2;
    }
}

void monoFrame(int a1, int a2, int a3, int a4, byte a5, byte a6)
{
    a3 = ClipHigh(a3, window.f_8);
    a4 = ClipHigh(a4, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a1, window.f_4 + a2);

    *va++ = Frames[a5][0];
    *va++ = a6;

    for (int i = 0; i < a4 - 2; i++)
    {
        *va++ = Frames[a5][1];
        *va++ = a6;
    }

    *va++ = Frames[a5][2];
    *va++ = a6;

    va += 160 - a4 * 2;

    for (int j = 0; j < a3 - 2; j++)
    {
        *va++ = Frames[a5][3];
        *va++ = a6;
        va += (a4 - 2) * 2;
        *va++ = Frames[a5][5];
        *va++ = a6;
        va += 160 - a4 * 2;
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

void monoSetPos(int a1, int a2)
{
    cursor.f_0 = a1;
    cursor.f_4 = a2;
}

void monoScrollUp(int a1)
{
    byte *va = DebugVideoAddress(window.f_0, window.f_4);
    for (int i = 0; i < window.f_8 - a1; i++)
    {
        memcpy(va, va + 160 * a1, window.f_c * 2);
        va += 160;
    }
}

void monoScrollDown(int a1)
{
    byte *va = DebugVideoAddress(window.f_0 + window.f_8 - 1, window.f_4);
    for (int i = 0; i < window.f_8 - a1; i++)
    {
        memcpy(va, va - 160 * a1, window.f_c * 2);
        va -= 160;
    }
}

void monoNewLine(void)
{
    cursor.f_0++;
    cursor.f_4 = 0;
    if (cursor.f_0 >= window.f_8)
    {
        monoScrollUp(1);
        cursor.f_0 = window.f_8 - 1;
        monoFill(cursor.f_0, 0, 1, window.f_c, ' ', nAttribute);
    }
}

void monoPrint(char *s, byte a2)
{
    for (; *s; s++)
    {
        if (*s == 10)
            monoNewLine();
        if (*s >= 32)
        {
            byte *va = DebugVideoAddress(window.f_0 + cursor.f_0, window.f_4 + cursor.f_4);
            *va++ = *s;
            *va++ = a2;
            cursor.f_4++;
            if (cursor.f_4 == window.f_c)
                monoNewLine();
        }
    }
}

void monoPrintf(char *s, ...)
{
    char buf[256];
    va_list arg;
    va_start(arg, s);
    vsprintf(buf, s, arg);
    va_end(arg);
    monoPrint(buf, nAttribute);
}

void monoLeftString(int a1, int a2, char *a3, byte a4)
{
    monoSetPos(a1, a2);
    monoPrint(a3, a4);
}

void monoCenterString(int a1, int a2, char *a3, byte a4)
{
    monoSetPos(a1, a2 - (strlen(a3)>>1));
    monoPrint(a3, a4);
}

void monoRightString(int a1, int a2, char *a3, byte a4)
{
    monoSetPos(a1, a2 - (strlen(a3) - 1));
    monoPrint(a3, a4);
}

void monoLeftString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    int l = strlen(a4);
    memset(buf, 32, 256);
    buf[a3 - a2 + 1] = 0;
    if (l > a3 - a2 + 1)
        memcpy(buf, a4, a3 - a2 + 1);
    else
        memcpy(buf, a4, l);
    monoSetPos(a1, a2);
    monoPrint(buf, a5);
}

void monoCenterString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    memset(buf, 32, 256);
    buf[a3 - a2 + 1] = 0;
    int vd = (a3 - a2 + 1 - strlen(a4)) >> 1;
    if (vd < 0)
        memcpy(buf, a4 - vd, a3 - a2 + 1);
    else
        memcpy(buf + vd, a4, strlen(a4));
    monoSetPos(a1, a2);
    monoPrint(buf, a5);
}

void monoRightString(int a1, int a2, int a3, char *a4, byte a5)
{
    char buf[256];
    memset(buf, 32, 256);
    buf[a3 - a2 + 1] = 0;
    int vd = a3 - a2 + 1 - strlen(a4);
    if (vd < 0)
        memcpy(buf, a4 - vd, a3 - a2 + 1);
    else
        memcpy(buf + vd, a4, strlen(a4));
    monoSetPos(a1, a2);
    monoPrint(buf, a5);
}

void monoWindow(int a1, int a2, int a3, int a4)
{
    window.f_0 = a1;
    window.f_4 = a2;
    window.f_8 = a3;
    window.f_c = a4;
}

void monoClearWindow(void)
{
    monoFill(window.f_0, window.f_4, window.f_8, window.f_c, 32, 7);
    monoSetPos(0, 0);
}

byte monoSetAttribute(byte a1)
{
    byte old = nAttribute;
    nAttribute = a1;
    return old;
}

void monoSaveWindow(byte *a1, int a2, int a3, int a4, int a5)
{
    a4 = ClipHigh(a4, window.f_8);
    a5 = ClipHigh(a5, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a2, window.f_4 + a3);
    for (int i = 0; i < a4; i++)
    {
        memcpy(a1, va, a5 * 2);
        a1 += a5 * 2;
        va += 160;
    }
}

void monoRestoreWindow(byte *a1, int a2, int a3, int a4, int a5)
{
    a4 = ClipHigh(a4, window.f_8);
    a5 = ClipHigh(a5, window.f_c);
    byte *va = DebugVideoAddress(window.f_0 + a2, window.f_4 + a3);
    for (int i = 0; i < a4; i++)
    {
        memcpy(va, a1, a5 * 2);
        a1 += a5 * 2;
        va += 160;
    }
}

int dprintf(char *, ...)
{
    return 0;
}

void __dassert(char *s, char *m, int l)
{
    switch (debugMode)
    {
        case kDebugMode0:
            setvmode(3);
            printf("ERROR (%i) %s%\n", l, m);
            fflush(NULL);
            break;
        case kDebugMode1:
            dprintf("Assertion failed: %s in file %s at line %i\n", s, m, l);
            break;
        case kDebugMode2:
            dprintf("Assertion failed: %s in file %s at line %i\n", s, m, l);
            setvmode(3);
            printf("ERROR #:%s%i\n", m, l);
            fflush(NULL);
            break;
    }
    int3();
    exit(1);
}