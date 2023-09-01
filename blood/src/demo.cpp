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
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dos.h>
#include "typedefs.h"
#include "build.h"
#include "debug4g.h"
#include "demo.h"
#include "error.h"
#include "fire.h"
#include "gamemenu.h"
#include "globals.h"
#include "key.h"
#include "menu.h"
#include "messages.h"
#include "misc.h"
#include "network.h"
#include "player.h"
#include "screen.h"
#include "view.h"
#if 0
#include "function.h"
#endif

int int_28F380 = 0;

CDemo gDemo;

CDemo::CDemo()
{
    int_28F380 = 4;
    at0 = 0;
    at1 = 0;
    at3 = 0;
    at7 = -1;
    atb = 0;
    at59ef = 0;
    at59eb = 0;
    at2 = 0;
    memset(&atf, 0, sizeof(atf));
}

CDemo::~CDemo()
{
    at0 = 0;
    at1 = 0;
    at3 = 0;
    atb = 0;
    memset(&atf, 0, sizeof(atf));
    if (at7 != -1)
    {
        close(at7);
        at7 = -1;
    }
}

BOOL CDemo::Create(char *pzFile)
{
    char buffer[13] = "";
    char vc = 0;
    if (RecordStatus() || PlaybackStatus())
        ThrowError(104)("CDemo::Create called during demo record/playback process.");
    if (!pzFile)
    {
        for (int i = 0; i < 8 && vc == 0; i++)
        {
            sprintf(buffer, "BLOOD0%02d.DEM", i);
            if (access(buffer, 4) == -1)
                vc = 1;
        }
        if (vc == 1)
        {
            at7 = open(buffer, 0x222, 0x180);
            if (at7 == -1)
                return 0;
        }
        else
            return 0;
    }
    else
    {
        at7 = open(pzFile, 0x222, 0x180);
        if (at7 == -1)
            return 0;
    }
    at0 = 1;
    atb = 0;
    return 1;
}

void CDemo::Write(INPUT *pPlayerInputs)
{
    dassert(pPlayerInputs != NULL, 155);
    if (!RecordStatus())
        return;
    if (atb == 0)
    {
        atf.signature = '\x1aMED';
        atf.nVersion = gGameVersion;
        atf.nBuild = int_28F380;
        atf.nInputCount = atb;
        atf.nNetPlayers = gNetPlayers;
        atf.nMyConnectIndex = myconnectindex;
        atf.nConnectHead = connecthead;
        memcpy(atf.connectPoints, connectpoint2, sizeof(atf.connectPoints));
        memcpy(&atf.gameOptions, &gGameOptions, sizeof(gGameOptions));
        write(at7, &atf, sizeof(DEMOHEADER));
    }
    for (int p = connecthead; p >= 0; p = connectpoint2[p])
    {
        memcpy(&at1aa[atb&1023], &pPlayerInputs[p], sizeof(INPUT));
        atb++;
        if((atb&1023)==0)
            write(at7,at1aa,sizeof(at1aa));
    }
}

void CDemo::Close(void)
{
    if (RecordStatus())
    {
        if (atb&1023)
            write(at7,at1aa,sizeof(INPUT)*(atb&1023));
        atf.nInputCount = atb;
        lseek(at7, 0, SEEK_SET);
        write(at7, &atf, sizeof(DEMOHEADER));
    }
    if (at7 != -1)
    {
        close(at7);
        at7 = -1;
    }
    at0 = 0;
    at1 = 0;
}

BOOL CDemo::SetupPlayback(char *pzFile)
{
    at0 = 0;
    at1 = 0;
    if (pzFile)
    {
        at7 = open(pzFile, O_BINARY|O_RDWR, 0400);
        if (at7 == -1)
            return 0;
    }
    else
    {
        at7 = open(at59aa[at59eb], O_BINARY|O_RDWR, 0400);
        if (at7 == -1)
            return 0;
    }
    read(at7, &atf, sizeof(atf));
    if (atf.signature != '\x1aMED')
        return 0;
    if (atf.nVersion.w != gGameVersion.w && gGameVersion.w == 0x10b && atf.nVersion.w != 0x10a)
        return 0;
    if (atf.nBuild != int_28F380)
        return 0;
    at0 = 0;
    at1 = 1;
    return 1;
}

void CDemo::ProcessKeys(void)
{
    switch (gInputMode)
    {
        case INPUT_MODE_1:
            gGameMenuMgr.Process();
            break;
        case INPUT_MODE_2:
            gPlayerMsg.ProcessKeys();
            break;
        case INPUT_MODE_0:
        {
#if 0
            if (keystatus[0x25])
            {
                keystatus[0x25] = 0;
                if (gGameOptions.nGameType == GAMETYPE_1)
                {
                    gViewIndex = connectpoint2[gViewIndex];
                    if (gViewIndex == -1)
                        gViewIndex = connecthead;
                    gView = &gPlayer[gViewIndex];
                }
                else if (gGameOptions.nGameType == GAMETYPE_3)
                {
                    int oldViewIndex = gViewIndex;
                    do
                    {
                        gViewIndex = connectpoint2[gViewIndex];
                        if (gViewIndex == -1)
                            gViewIndex = connecthead;
                        if (oldViewIndex == gViewIndex || gMe->at2ea == gPlayer[gViewIndex].at2ea)
                            break;
                    } while (oldViewIndex != gViewIndex);
                    gView = &gPlayer[gViewIndex];
                }
            }
#endif
            byte nKey;
            while ((nKey = keyGet()) != 0)
            {
	            BOOL alt = keystatus[bsc_LAlt] | keystatus[bsc_RAlt];
	            BOOL ctrl = keystatus[bsc_LCtrl] | keystatus[bsc_RCtrl];
                switch (nKey)
                {
                case bsc_F12:
                    gViewIndex = connectpoint2[gViewIndex];
                    if (gViewIndex == -1)
                        gViewIndex = connecthead;
                    gView = &gPlayer[gViewIndex];
                    break;
                case bsc_Esc:
                    if (!CGameMenuMgr::Active())
                    {
                        gGameMenuMgr.Push(&menuMain, -1);
                        at2 = 1;
                    }
                    break;
                }
            }
            break;
        }
    }
}

void ProcessFrame(void);
void StartLevel(GAMEOPTIONS *);

void CDemo::Playback(void)
{
    ready2send = 0;
    int v4 = 0;
    if (!CGameMenuMgr::Active())
    {
        gGameMenuMgr.Push(&menuMain, -1);
        at2 = 1;
    }
    gNetFifoClock = gGameClock;
    gViewMode = 3;
_DEMOPLAYBACK:
    while (at1 && !gQuitGame)
    {
        while (gGameClock >= gNetFifoClock && !gQuitGame)
        {
            if (!v4)
            {
                viewResizeView(gViewSize);
                viewSetMessage("");
                gNetPlayers = atf.nNetPlayers;
                atb = atf.nInputCount;
                myconnectindex = atf.nMyConnectIndex;
                connecthead = atf.nConnectHead;
                memcpy(connectpoint2, atf.connectPoints, sizeof(atf.connectPoints));
                memcpy(&gGameOptions, &atf.gameOptions, sizeof(GAMEOPTIONS));
                gSkill = gGameOptions.nDifficulty;
                for (int i = 0; i < 8; i++)
                    playerInit(i, 0);
                StartLevel(&gGameOptions);
            }
            ProcessKeys();
            for (int p = connecthead; p >= 0; p = connectpoint2[p])
            {
                if ((v4&1023) == 0)
                {
                    unsigned int nSize = sizeof(INPUT)*(atb-v4);
                    if (nSize > sizeof(at1aa))
                        nSize = sizeof(at1aa);
                    read(at7, at1aa, nSize);
                }
                memcpy(&gFifoInput[gNetFifoHead[p]&255][p], &at1aa[v4&1023], sizeof(INPUT));
                gNetFifoHead[p]++;
                v4++;
                if (v4 >= atf.nInputCount)
                {
                    ready2send = 0;
                    if (DemoCount() != 1)
                    {
                        v4 = 0;
                        Close();
                        NextDemo();
                        gNetFifoClock = gGameClock;
                        goto _DEMOPLAYBACK;
                    }
                    else
                    {
                        lseek(at7, sizeof(DEMOHEADER), SEEK_SET);
                        v4 = 0;
                    }
                }
            }
            gNetFifoClock += 4;
            if (!gQuitGame)
                ProcessFrame();
            ready2send = 0;
        }
        viewDrawScreen();
        if (gInputMode == INPUT_MODE_1 && CGameMenuMgr::Active())
            gGameMenuMgr.Draw();
        scrNextPage();
        if (TestBitString(gotpic, 2342))
        {
            FireProcess();
            ClearBitString(gotpic, 2342);
        }
    }
    Close();
}

void CDemo::StopPlayback(void)
{
    at1 = 0;
}

void CDemo::LoadDemoInfo(void)
{
    struct find_t find;
    at59ef = 0;
    int status = _dos_findfirst("BLOOD*.DEM", 0, &find);
    while (!status && at59ef < 5)
    {
        int hFile2 = open(find.name, O_BINARY);
        if (hFile2 == -1)
            ThrowError(510)("File error #%d loading demo file header.", errno);
        read(hFile2, &atf, sizeof(atf));
        close(hFile2);
        if (atf.signature == '\x1aMED' && (atf.nVersion.w == gGameVersion.w || gGameVersion.w != 0x10b || atf.nVersion.w == 0x10a) && atf.nBuild == int_28F380)
        {
            strcpy(at59aa[at59ef], find.name);
            at59ef++;
        }
        status = _dos_findnext(&find);
    }
}

void CDemo::NextDemo(void)
{
    at59eb++;
    if (at59eb >= at59ef)
        at59eb = 0;
    SetupPlayback(NULL);
}

