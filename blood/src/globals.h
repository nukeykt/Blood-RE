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
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "typedefs.h"
#include "types.h"
#include "resource.h"

enum INPUT_MODE {
    INPUT_MODE_0,
    INPUT_MODE_1,
    INPUT_MODE_2,
    INPUT_MODE_3,
};

union BLOODVERSION {
    struct {
        byte minor;
        byte major;
    } b;
    ushort w;
};

extern const BLOODVERSION gGameVersion;

extern Resource gSysRes;

extern BOOL gUse8250;

extern long volatile gGameClock;

extern int32 gGamma;

extern char gBuildDate[];
extern char gBuildTime[];

extern int gCacheMiss;
extern long gFrameClock;
extern int gFrameTicks;

extern int int_148E14;

extern INPUT_MODE gInputMode;

extern int gFrameRate;

extern BOOL gQuitRequest;

extern BOOL gSaveGameActive;

extern BOOL gPaused;

extern int gNetPlayers;

extern int gFrame;

extern char *int_148E0C;
extern char *int_148E10;

extern BOOL gQuitGame;
extern BOOL gTenQuit;
extern BOOL char_148E29;
extern int gSaveGameNum;

extern BOOL gInWindows;

extern int gOldDisplayMode;

extern BOOL gAdultContent;

char *GetVersionString(void);
void ClockStrobe(void);
void LockClockStrobe(void);
void UnlockClockStrobe(void);

#endif // !_GLOBALS_H_
