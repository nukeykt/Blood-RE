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
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "typedefs.h"
#include "debug4g.h"
#include "key.h"

byte keyBuffer[256];
int volatile bufHead;
int bufTail;

byte (*keyCallback)(byte, BOOL);
byte volatile extended;

void (interrupt *oldKeyHandler)(void);

BOOL filterMake[256];
BOOL filterBreak[256];

byte volatile keystatus[256];

struct SCANTEXT {
    byte scan;
    char *text;
};

SCANTEXT ScanToText[] = {
    bsc_Esc, "Esc",
    bsc_Backspace, "Backspace",
    bsc_Tab, "Tab",
    bsc_Enter, "Enter",
    bsc_LCtrl, "LCtrl",
    bsc_LShift, "LShift",
    bsc_RShift, "RShift",
    bsc_Pad_Star, "Pad*",
    bsc_LAlt, "LAlt",
    bsc_SpaceBar, "SpaceBar",
    bsc_CapsLock, "CapsLock",
    bsc_F1, "F1",
    bsc_F2, "F2",
    bsc_F3, "F3",
    bsc_F4, "F4",
    bsc_F5, "F5",
    bsc_F6, "F6",
    bsc_F7, "F7",
    bsc_F8, "F8",
    bsc_F9, "F9",
    bsc_F10, "F10",
    bsc_NumLock, "NumLock",
    bsc_ScrlLock, "ScrlLock",
    bsc_Pad_7, "Pad7",
    bsc_Pad_8, "Pad8",
    bsc_Pad_9, "Pad9",
    bsc_Pad_Minus, "Pad-",
    bsc_Pad_4, "Pad4",
    bsc_Pad_5, "Pad5",
    bsc_Pad_6, "Pad6",
    bsc_Pad_Plus, "Pad+",
    bsc_Pad_1, "Pad1",
    bsc_Pad_2, "Pad2",
    bsc_Pad_3, "Pad3",
    bsc_Pad_0, "Pad0",
    bsc_Pad_Period, "Pad.",
    bsc_SysReq, "SysReq",
    bsc_F11, "F11",
    bsc_F12, "F12",
    bsc_Pad_Enter, "PadEnter",
    bsc_RCtrl, "RCtrl",
    bsc_Pad_Slash, "Pad/",
    bsc_PrntScrn, "PrntScrn",
    bsc_RAlt, "RAlt",
    bsc_Pause, "Pause",
    bsc_Break, "Break",
    bsc_Home, "Home",
    bsc_Up, "Up",
    bsc_PgUp, "PgUp",
    bsc_Left, "Left",
    bsc_Right, "Right",
    bsc_End, "End",
    bsc_Down, "Down",
    bsc_PgDn, "PgDn",
    bsc_Ins, "Ins",
    bsc_Del, "Del"
};

byte ScanToAscii[256] = 
{
    0, '\x1B', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\x08', '\x09',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\x0D', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};

byte ScanToAsciiShifted[256] = 
{
    0, '\x1B', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\x08', '\x09',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\x0D', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.'
};


void keyPragma1(void);
#pragma aux keyPragma1 = \
"mov ecx,0x20000" \
"L1:" \
"in al,0x64" \
"test al,2" \
"loopne L1"

void interrupt newKeyHandler(void)
{
    keyPragma1();
    outp(0x64, 0xad);
    keyPragma1();
    byte k = inp(0x60);
    if ((k & 0xf0) == 0xe0)
    {
        extended = 0x80 | (k & 0x0f);
    }
    else if (extended > 0x80)
    {
        extended--;
    }
    else
    {
        byte va = (k & 0x7f) | extended;
        extended = 0;
        if (k < 0x80)
        {
            if (!filterMake[va])
            {
                if (keyCallback)
                    va = keyCallback(va, TRUE);
                keystatus[va] = TRUE;
                keyBuffer[bufHead] = va;
                bufHead = (bufHead + 1) & 255;
            }
        }
        else
        {
            if (!filterBreak[va])
            {
                if (keyCallback)
                    va = keyCallback(va, FALSE);
                keystatus[va] = FALSE;
            }
        }
    }
    outp(0x20, 0x20);
    keyPragma1();
    outp(0x64, 0xae);
}

void NEWKEYEND(void)
{
}

void keyRemove(void)
{
    if (oldKeyHandler)
    {
        _dos_setvect(9, oldKeyHandler);
        oldKeyHandler = NULL;
    }
}

void keyInstall(void)
{
    memset(filterMake, 0, sizeof(filterMake));
    memset(filterBreak, 0, sizeof(filterBreak));
    filterMake[0xaa] = TRUE;
    filterMake[0xb6] = TRUE;
    filterBreak[0xaa] = TRUE;
    filterBreak[0xb6] = TRUE;
    filterBreak[0xc5] = TRUE;
    filterBreak[0xc6] = TRUE;
    oldKeyHandler = _dos_getvect(9);
    _dos_setvect(9, newKeyHandler);
    atexit(keyRemove);
}


byte keyGet(void)
{
    if (bufHead == bufTail)
        return 0;
    byte key = keyBuffer[bufTail];
    bufTail = (bufTail + 1) & 255;
    return key;
}

BOOL keyCompareStream(byte *s, int c)
{
    int p = (bufHead - c) & 255;
    while (--c != -1)
    {
        if ((byte)keyBuffer[p] != *s)
            return FALSE;
        s++;
        p = (p + 1) & 255;
    }
    return TRUE;
}

BOOL keyCompareStream(byte *s, int c, int o)
{
    int p = (bufHead - c - o) & 255;
    while (--c != -1)
    {
        if ((byte)keyBuffer[p] != *s)
            return FALSE;
        s++;
        p = (p + 1) & 255;
    }
    return TRUE;
}

void keyReadStream(byte *s, int c)
{
    int p = (bufHead - c) & 255;
    while (--c != -1)
    {
        *s++ = (byte)keyBuffer[p];
        p = (p + 1) & 255;
    }
}

void keyPokeStream(byte k)
{
    _disable();
    keyBuffer[bufHead] = k;
    bufHead = (bufHead + 1) & 255;
    _enable();
}

void keyFlushStream(void)
{
    bufTail = bufHead;
}

char *keyName(byte scanCode)
{
    static char buffer[4];
    dassert(scanCode < 256, 336);
    for (uint i = 0; i < 56; i++)
    {
        if (scanCode == ScanToText[i].scan)
            return ScanToText[i].text;
    }
    byte c = ScanToAscii[scanCode];
    if (c != 0)
    {
        buffer[0] = toupper(c);
        buffer[1] = 0;
        return buffer;
    }
    sprintf(buffer, "0x%0x", scanCode);
    return buffer;
}
