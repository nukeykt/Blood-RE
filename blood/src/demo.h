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
#ifndef _DEMO_H_
#define _DEMO_H_
#include "typedefs.h"
#include "controls.h"
#include "globals.h"
#include "levels.h"

struct DEMOHEADER
{
    ulong signature;
    BLOODVERSION nVersion;
    long nBuild;
    long nInputCount;
    int nNetPlayers;
    short nMyConnectIndex;
    short nConnectHead;
    short connectPoints[8];
    GAMEOPTIONS gameOptions;
};

class CDemo {
public:
    CDemo();
    ~CDemo();
    void Close(void);
    void StopPlayback(void);
    BOOL Create(char *);
    void Write(INPUT *);
    BOOL SetupPlayback(char *);
    void ProcessKeys(void);
    void Playback(void);
    void NextDemo(void);
    void LoadDemoInfo(void);
    BOOL at0; // record
    BOOL at1; // playback
    BOOL at2;
    int at3;
    int at7;
    int atb;
    DEMOHEADER atf;
    INPUT at1aa[1024];
    char at59aa[5][13];
    int at59eb;
    int at59ef;
    BOOL RecordStatus() { return at0; }
    BOOL PlaybackStatus() { return at1; }
    int DemoCount() { return at59ef; }
};

extern CDemo gDemo;

#endif
