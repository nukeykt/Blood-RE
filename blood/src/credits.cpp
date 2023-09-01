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
#include <ctype.h>
#include "typedefs.h"
#include "build.h"
#include "cdrom.h"
#include "credits.h"
#include "debug4g.h"
#include "globals.h"
#include "key.h"
#include "misc.h"
#include "screen.h"
#include "smkplay.h"
#include "sound.h"

BOOL exitCredits = FALSE;

BOOL Wait(int nTicks)
{
    gGameClock = 0;
    while (gGameClock < nTicks)
    {
        BYTE key = keyGet();
        if (key)
        {
            if (key == bsc_Esc) // sc_Escape
                exitCredits = TRUE;
            return FALSE;
        }
    }
    return TRUE;
}

BOOL DoFade(byte r, byte g, byte b, int nTicks)
{
    dassert(nTicks > 0, 59);
    scrSetupFade(r, g, b);
    gGameClock = gFrameClock = 0;
    do
    {
        while (gGameClock < gFrameClock) { }
        gFrameClock += 2;
        WaitVBL();
        scrFadeAmount(divscale16(ClipHigh(gGameClock, nTicks), nTicks));
        if (keyGet() != 0)
            return FALSE;
    } while (gGameClock <= nTicks);
    return TRUE;
}

BOOL DoUnFade(int nTicks)
{
    dassert(nTicks > 0, 79);
    scrSetupUnfade();
    gGameClock = gFrameClock = 0;
    do
    {
        while (gGameClock < gFrameClock) { }
        gFrameClock += 2;
        // VSync
        WaitVBL();
        scrFadeAmount(0x10000-divscale16(ClipHigh(gGameClock, nTicks), nTicks));
        if (keyGet() != 0)
            return FALSE;
    } while (gGameClock <= nTicks);
    return TRUE;
}

void credLogos(void)
{
    BOOL bShift = keystatus[bsc_LShift] | keystatus[bsc_RShift];
    setview(0, 0, xdim-1, ydim-1);
    DoUnFade(1);
    clearview(0);
    if (bShift)
        return;
    {
        CSMKPlayer smkPlayer;
        if (smkPlayer.PlaySMKWithWAV("LOGO.SMK", 300) == 1)
        {
            rotatesprite(160<<16, 100<<16, 65536, 0, 2050, 0, 0, 0x4a, 0, 0, xdim-1, ydim-1);
            sndStartSample("THUNDER2", 128, -1);
            scrNextPage();
            if (!Wait(360))
                return;
            if (!DoFade(0, 0, 0, 60))
                return;
        }
        if (smkPlayer.PlaySMKWithWAV("GTI.SMK", 301) == 1)
        {
            clearview(0);
            rotatesprite(160<<16, 100<<16, 65536, 0, 2052, 0, 0, 0x0a, 0, 0, xdim-1, ydim-1);
            scrNextPage();
            DoUnFade(1);
            sndStartSample("THUNDER2", 128, -1);
            if (!Wait(360))
                return;
        }
    }
    sndPlaySong("PESTIS", 1);
    sndStartSample("THUNDER2", 128, -1);
    if (!DoFade(0, 0, 0, 60))
        return;
    clearview(0);
    scrNextPage();
    if (!DoUnFade(1))
        return;
    clearview(0);
    rotatesprite(160<<16, 100<<16, 65536, 0, 2518, 0, 0, 0x4a, 0, 0, xdim-1, ydim-1);
    scrNextPage();
    Wait(360);
    sndFadeSong(4000);
}

void credReset(void)
{
    clearview(0);
    scrNextPage();
    DoFade(0,0,0,1);
    scrSetupUnfade();
    DoUnFade(1);
}

void credPlaySmk(char *pzSMK, int nWAV)
{
    CSMKPlayer smkPlayer;
    if (int_148E14 >= 0)
    {
        if ('A'+int_148E14 == toupper(*pzSMK))
        {
            if (Redbook.func_82258() == 0 || Redbook.func_82258() > 20)
                return;
        }
        Redbook.cd_stop_audio();
    }
    smkPlayer.PlaySMKWithWAV(pzSMK, nWAV);
}

void credPlaySmk(char *pzSMK, char *pzWAV)
{
    CSMKPlayer smkPlayer;
    if (int_148E14 >= 0)
    {
        if ('A'+int_148E14 == toupper(*pzSMK))
        {
            if (Redbook.func_82258() == 0 || Redbook.func_82258() > 20)
                return;
        }
        Redbook.cd_stop_audio();
    }
    smkPlayer.PlaySMKWithWAV(pzSMK, pzWAV);
}
