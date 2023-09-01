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
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"
#include "debug4g.h"
#include "helix.h"
#include "gfx.h"
#include "misc.h"


int clipX0;
int clipY0;
int clipX1 = 320;
int clipY1 = 200;

Rect clipRect(0, 0, 320, 200);

void gfxDrawBitmap(QBITMAP *qbm, int a2, int a3)
{
    dassert(qbm != NULL, 26);
    Rect rect(a2, a3, a2 + qbm->cols, a3 + qbm->rows);
    rect &= clipRect;
    if (!rect)
        return;

    Rect rect2(rect);
    rect2.offset(-a2, -a3);
    switch (qbm->bitModel)
    {
        case 0:
            Video.BlitM2V(&qbm->data[rect2.f_4 * qbm->stride + rect2.f_0], qbm->stride, rect.width(), rect.height(), 0, rect.f_0, rect.f_4);
            break;
        case 1:
            Video.BlitMT2V(&qbm->data[rect2.f_4 * qbm->stride + rect2.f_0], qbm->tcolor, qbm->stride, rect.width(), rect.height(), 0, rect.f_0, rect.f_4);
            break;
    }
}

void gfxPixel(int x, int y)
{
    if (clipRect.testXY(x, y))
        Video.SetPixel(0, x, y);
}

void gfxHLine(int a1, int a2, int a3)
{
    if (a1 < clipRect.f_4 || a1 >= clipRect.f_c)
        return;
    a2 = ClipLow(a2, clipRect.f_0);
    a3 = ClipHigh(a3, clipRect.f_8 - 1);
    if (a2 <= a3)
        Video.HLine(0, a1, a2, a3);
}

void gfxVLine(int a1, int a2, int a3)
{
    if (a1 < clipRect.f_0 || a1 >= clipRect.f_8)
        return;
    a2 = ClipLow(a2, clipRect.f_4);
    a3 = ClipHigh(a3, clipRect.f_c - 1);
    if (a2 <= a3)
        Video.VLine(0, a1, a2, a3);
}

void gfxHLineROP(int a1, int a2, int a3)
{
    if (a1 < clipRect.f_4 || a1 >= clipRect.f_c)
        return;
    a2 = ClipLow(a2, clipRect.f_0);
    a3 = ClipHigh(a3, clipRect.f_8 - 1);
    if (a2 <= a3)
        Video.HLineROP(0, a1, a2, a3);
}

void gfxVLineROP(int a1, int a2, int a3)
{
    if (a1 < clipRect.f_0 || a1 >= clipRect.f_8)
        return;
    a2 = ClipLow(a2, clipRect.f_4);
    a3 = ClipHigh(a3, clipRect.f_c - 1);
    if (a2 <= a3)
        Video.VLineROP(0, a1, a2, a3);
}

void gfxFillBox(int a1, int a2, int a3, int a4)
{
    Rect rect(a1, a2, a3, a4);

    rect &= clipRect;
    if (rect.isValid())
        Video.FillBox(0, rect.f_0, rect.f_4, rect.f_8, rect.f_c);
}

void gfxSetClip(int a1, int a2, int a3, int a4)
{
    clipRect.f_0 = a1;
    clipRect.f_4 = a2;
    clipRect.f_8 = a3;
    clipRect.f_c = a4;
    clipX0 = a1 << 8;
    clipY0 = a2 << 8;
    clipX1 = (a3 << 8) - 1;
    clipY1 = (a4 << 8) - 1;
}

void gfxDrawLine(int a1, int a2, int a3, int a4, int a5)
{
RETRY:
    int vd = 0;
    int vb = 0;
    if (a2 < clipY0)
        vb |= 8;
    else if (a2 > clipY1)
        vb |= 4;
    if (a1 < clipX0)
        vb |= 2;
    else if (a1 > clipX1)
        vb |= 1;
    if (a4 < clipY0)
        vd |= 8;
    else if (a4 > clipY1)
        vd |= 4;
    if (a3 < clipX0)
        vd |= 2;
    else if (a3 > clipX1)
        vd |= 1;
    if (vb & vd)
        return;
    if (vb != vd)
    {
        // int dy = a4 - a2;
        // int dx = a3 - a1;
        if (vb & 8)
        {
            a1 += kscale(a3 - a1, clipY0 - a2, a4 - a2);
            a2 = clipY0;
            goto RETRY;
        }
        if (vb & 4)
        {
            a1 += kscale(a3 - a1, clipY1 - a2, a4 - a2);
            a2 = clipY1;
            goto RETRY;
        }
        if (vb & 2)
        {
            a2 += kscale(a4 - a2, clipX0 - a1, a3 - a1);
            a1 = clipX0;
            goto RETRY;
        }
        if (vb & 1)
        {
            a2 += kscale(a4 - a2, clipX1 - a1, a3 - a1);
            a1 = clipX1;
            goto RETRY;
        }
        if (vd & 8)
        {
            a3 += kscale(a3 - a1, clipY0 - a4, a4 - a2);
            a4 = clipY0;
            goto RETRY;
        }
        if (vd & 4)
        {
            a3 += kscale(a3 - a1, clipY1 - a4, a4 - a2);
            a4 = clipY1;
            goto RETRY;
        }
        if (vd & 2)
        {
            a4 += kscale(a4 - a2, clipX0 - a3, a3 - a1);
            a3 = clipX0;
            goto RETRY;
        }
        if (vd & 1)
        {
            a4 += kscale(a4 - a2, clipX1 - a3, a3 - a1);
            a3 = clipX1;
            goto RETRY;
        }
    }
    if (a2 == a4)
    {
        Video.SetColor(a5);
        if (a1 < a3)
            Video.HLine(0, a2 >> 8, a1 >> 8, a3 >> 8);
        else
            Video.HLine(0, a2 >> 8, a3 >> 8, a1 >> 8);
    }
    else if (a1 == a3)
    {
        Video.SetColor(a5);
        if (a2 < a4)
            Video.VLine(0, a1 >> 8, a2 >> 8, a4 >> 8);
        else
            Video.VLine(0, a1 >> 8, a4 >> 8, a2 >> 8);
    }
    else
    {
        Video.SetColor(a5);
        Video.Line(0, a1 >> 8, a2 >> 8, a3 >> 8, a4 >> 8);
    }
}

void gfxDrawPixel(int a1, int a2, int a3)
{
    if (clipRect.testXY(a1, a2))
    {
        Video.SetColor(a3);
        Video.SetPixel(0, a1, a2);
    }
}

byte *fontTable = (byte*)0xffa6e;

void printChar(int x, int y, byte c)
{
    for (int i = 0; i < 8; i++)
    {
        byte mask = 0x80;
        for (int j = 0; j < 8; j++, mask >>= 1)
        {
            if (fontTable[c * 8 + i] & mask)
                Video.SetPixel(0, x+j, y+i);
        }
    }
}

int gfxGetTextLen(char *a1, QFONT *a2)
{
    if (a2 == NULL)
        return strlen(a1) * 8;

    int l = -a2->at11;
    for (char *s = a1; *s; s++)
        l += a2->at20[*s].ox + a2->at11;

    return l;
}

int gfxGetTextNLen(char *a1, QFONT *a2, int a3)
{
    if (a2 == NULL)
        return strlen(a1) * 8;

    int l = -a2->at11;
    for (char *s = a1; *s && a3 > 0; s++, a3--)
        l += a2->at20[*s].ox + a2->at11;

    return l;
}

int gfxGetLabelLen(char *a1, QFONT *a2)
{
    int l = 0;
    if (a2)
        l = -a2->at11;
    for (char* s = a1; *s; s++)
    {
        if (*s == '&')
            continue;
        if (!a2)
            l += 8;
        else
            l += a2->at20[*s].ox + a2->at11;
    }
    return l;
}

int gfxFindTextPos(char *a1, QFONT *a2, int a3)
{
    if (!a2)
        return a3 / 8;
    int l = -a2->at11;
    int pos = 0;
    for (char* s = a1; *s; s++, pos++)
    {
        l += a2->at20[*s].ox + a2->at11;
        if (l > a3)
            break;
    }
    return pos;
}

void gfxDrawText(int x, int y, int color, char* pzText, QFONT* pFont)
{
    if (pFont)
        y += pFont->atf;

    Video.SetColor(color);

    for (char* s = pzText; *s; s++)
    {
        if (pFont == NULL)
        {
            Rect rect1(x, y, x+8, y+8);
            if (clipRect.testRect(rect1))
                printChar(x, y, *s);
            x += 8;
        }
        else
        {
            QFONTCHAR* pChar = &pFont->at20[*s];
            Rect rect1(x, y+pChar->oy, x+pChar->w, y+pChar->h+pChar->oy);

            rect1 &= clipRect;

            if (rect1.isValid())
            {
                Rect rect2(rect1);

                rect2.offset(-x, -(y+pChar->oy));

                switch (pFont->at6)
                {
                case 0:
                    Video.BlitMono(&pFont->at820[pChar->offset+(rect2.f_4/8)*pChar->w+rect2.f_0], 1<<(rect2.f_4&7), pChar->w,
                        rect1.width(), rect1.height(), 0, rect1.f_0, rect1.f_4);
                    break;
                case 1:
                    Video.BlitMT2V(&pFont->at820[pChar->offset+rect2.f_4*pChar->w+rect2.f_0], pFont->at10, pChar->w,
                        rect1.width(), rect1.height(), 0, rect1.f_0, rect1.f_4);
                    break;
                }
            }
            x += pFont->at11 + pChar->ox;
        }
    }
}

void gfxDrawLabel(int x, int y, int color, char* pzLabel, QFONT* pFont)
{
    if (pFont)
        y += pFont->atf;

    BOOL v4 = FALSE;
    Video.SetColor(color);

    for (char* s = pzLabel; *s; s++)
    {
        if (*s == '&')
        {
            v4 = TRUE;
            continue;
        }
        if (pFont == NULL)
        {
            Rect rect1(x, y, x+8, y+8);
            if (clipRect.testRect(rect1))
            {
                printChar(x, y, *s);
                if (v4)
                    gfxHLine(y+8, x, x+6);
                x += 8;
            }
        }
        else
        {
            QFONTCHAR* pChar = &pFont->at20[*s];
            Rect rect1(x, y+pChar->oy, x+pChar->w, y+pChar->h+pChar->oy);

            rect1 &= clipRect;

            if (rect1.isValid())
            {
                Rect rect2(rect1);

                rect2.offset(-x, -(y+pChar->oy));

                switch (pFont->at6)
                {
                case 0:
                    Video.BlitMono(&pFont->at820[pChar->offset+(rect2.f_4/8)*pChar->w+rect2.f_0], 1<<(rect2.f_4&7), pChar->w,
                        rect1.width(), rect1.height(), 0, rect1.f_0, rect1.f_4);
                    if (v4)
                        gfxHLine(y+2, x, x+pChar->w-1);
                    break;
                case 1:
                    Video.BlitMT2V(&pFont->at820[pChar->offset+rect2.f_4*pChar->w+rect2.f_0], pFont->at10, pChar->w,
                        rect1.width(), rect1.height(), 0, rect1.f_0, rect1.f_4);
                    if (v4)
                        gfxHLine(y+2, x, x+pChar->w-1);
                    break;
                }
            }
            x += pFont->at11 + pChar->ox;
        }
        v4 = FALSE;
    }
}

