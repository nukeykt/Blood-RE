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
#include <stdio.h>
#include "typedefs.h"
#include "types.h"
#include "build.h"
#include "debug4g.h"
#include "resource.h"
#include "globals.h"

Resource gSysRes;
int gOldDisplayMode;
long gFrameClock;
int gFrameTicks;
int gFrame;
long volatile gGameClock;
int gCacheMiss;
int gFrameRate;
int32 gGamma;
char *int_148E0C;
char *int_148E10;
int int_148E14;
INPUT_MODE gInputMode;
BOOL gQuitGame;
BOOL gQuitRequest;
BOOL gPaused;
int gNetPlayers;
BOOL gSaveGameActive;
BOOL gSavingGame;
int gSaveGameNum;
BOOL gTenQuit;
BOOL char_148E29;
BOOL gInWindows;
BOOL gUse8250;
char *gVersionString;
char gVersionStringBuf[16];

char gBuildDate[] = __DATE__;
char gBuildTime[] = __TIME__;

const BLOODVERSION gGameVersion = { 21, 1 };

BOOL gAdultContent = 1;

void ClockStrobe(void)
{
    ++gGameClock;
    totalclock = gGameClock;
}

void CLOCK_STROBE_END(void) {}

void LockClockStrobe(void) {}

void UnlockClockStrobe(void) {}

char *GetVersionString(void)
{
    if (!gVersionString)
    {
        gVersionString = gVersionStringBuf;
        if (!gVersionString)
            return NULL;
        sprintf(gVersionString, "%d.%02d", gGameVersion.b.major, gGameVersion.b.minor);
    }
    return gVersionString;
}