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
#include "typedefs.h"
#include "build.h"
#include "endgame.h"
#include "globals.h"
#include "heat.h"
#include "key.h"
#include "levels.h"
#include "network.h"
#include "player.h"
#include "view.h"

char unk[8];

struct heatstruct1 {
    char __f_0[4];
    short f_4;
    char __f_6[10];
    short f_10;
    char __f_14[2];
};

struct heatstruct3 {
    char f_0;
    char f_1[1];
};

heatstruct1 char_295460;
short short_295474;
short short_295476;
short short_295478;

int func_835B0(void)
{
    int v4 = 20;
    func_83A94((char*)&char_295460, &v4);
    if (v4 != 20)
    {
        return -2;
    }
    myconnectindex = char_295460.f_4-1;
    connecthead = 0;
    numplayers = char_295460.f_10;
    for (int i = 0; i < numplayers-1; i++)
    {
        connectpoint2[i] = i+1;
    }
    connectpoint2[numplayers-1] = -1;
    int_28E3D4 = 3;
    short_295474 = 1;
    return 1;
}

void func_8364C(short nDest, char *pBuffer, short nSize)
{
    int t[6];
    heatstruct2 *va;
    heatstruct3 *vd;
    va = (heatstruct2*)func_83928(0, 1);
    if (!va)
    {
        for (int i = 0; i < 6; i++)
        {
            t[i] = func_83900(i);
        }
        if (t[3] <= 0)
            return;
        while (!va)
        {
            func_838B0();
            va = (heatstruct2*)func_83928(0, 1);
        }
    }
    unsigned short s = nSize + 1;
    va->f_16 = s;
    vd = (heatstruct3*)va->f_c;
    if (!vd || s >= va->f_14)
        return;
    vd->f_0 = nDest;
    memcpy(vd->f_1, pBuffer, nSize);
    func_83928(1, 3);
    func_838D4();
    short_295476 = 0;
    func_838B0();
}

void func_8384C(void);

short func_83700(short *pSource, char *pBuffer)
{
    heatstruct2 *va = NULL;
    heatstruct3 *vd;
    short v4;
    char *buf;
    if (bOutOfSync && !short_295478)
    {
        char t[3];
        t[0] = 0xfe;
        t[1] = 0x48;
        t[2] = 0xf1;
        func_8364C(-1, t, 3);
        short_295478 = 1;
    }
    if (short_295476 < 3)
        va = (heatstruct2*)func_83928(4, 1);
    else
        short_295476 = 2;
    if (!va)
    {
        *pSource = -1;
        return 0;
    }
    if (va)
    {
        v4 = va->f_16 - 1;
        vd = (heatstruct3*)va->f_c;
        *pSource = vd->f_0;
        buf = vd->f_1;
        if (buf[0] == 254 && v4 > 2)
        {
            if (buf[1] == 0x48)
            {
                switch (buf[2])
                {
                    case 0xf0:
                        func_8384C();
                        levelEndLevel(0);
                        gEndGameMgr.Setup();
                        viewResizeView(gViewSize);
                        numplayers = 1;
                        gStartNewGame = 0;
                        char_148E29 = 1;
                        *pSource = -1;
                        return 0;
                }
            }
        }
        memcpy(pBuffer, buf, v4);
        func_83928(1, 0);
        short_295476++;
    }
    return v4;
}

void func_8384C(void)
{
    if (gGameOptions.nGameType != GAMETYPE_3)
        return;
    for (int i = 0; i < numplayers; i++)
    {
        gPlayer[i].at2c6 = int_21EFB0[gPlayer[i].at2ea];
    }
}

void func_83888(void)
{
    keyFlushStream();
    while (keyGet() == 0)
    {
        gEndGameMgr.Draw();
    }
}
