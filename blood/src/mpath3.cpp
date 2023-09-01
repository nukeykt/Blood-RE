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
#include "globals.h"
#include "misc.h"
#include "mpath.h"
#include "network.h"

struct mpathstruct4 {
    char __f_0[4];
    short f_4;
    char __f_6[10];
    short f_10;
    char __f_14[2];
};

struct mpathstruct5 {
    int f_0;
    char f_4[1];
};

extern BOOL char_148EE9;

int func_83370(void)
{
    mpathstruct4 buffer;
    int v8;
    int v4 = 20;
    if (!func_830F4(&v8))
    {
        return -1;
    }
    if (v8 != 28)
    {
        return -2;
    }
    func_83230((char*)&buffer, &v4);
    if (v4 != 20)
    {
        return -3;
    }
    numplayers = buffer.f_10;
    connecthead = 1;
    myconnectindex = buffer.f_4;
    for (int i = 0; i < numplayers-1; i++)
    {
        connectpoint2[i+1] = i+2;
    }
    connectpoint2[i+1] = -1;
    int_28E3D4 = 1;
    gSyncRate = 4;
    char_148EE9 = 1;
    return 0;
}

void func_83444(int nDest, char *pBuffer, int nSize)
{
    mpathstruct2 *va;
    mpathstruct5 *vd;
    while((va = (mpathstruct2*)func_83084(0, 1)) == NULL) { }
    vd = (mpathstruct5*)va->f_c;
    vd->f_0 = nDest;
    memcpy(vd->f_4, pBuffer, nSize);
    va->f_16 = nSize + 4;
    func_83084(1, 3);
    func_83014();
    func_82FE0();
}

short func_8349C(short * pSource, char * pBuffer)
{
    mpathstruct2 *va;
    mpathstruct5 *vd;
    short size;
    va = (mpathstruct2*)func_83084(4, 1);
    if (!va)
        return 0;
    vd = (mpathstruct5*)va->f_c;
    *pSource = vd->f_0;
    size = va->f_16 - 4;
    memcpy(pBuffer, vd->f_4, size);
    func_83084(1, 0);
    return size;
}

int func_834F8(void)
{
    return func_832E4();
}

int func_83500(void)
{
    int vc = -99999;
    int vs = gGameClock + vc;
    for (int i = connecthead; i >= 0; i = connectpoint2[i])
    {
        int va = (gNetFifoHead[myconnectindex] - gNetFifoHead[i]) / 10;
        if (va > vc)
        {
            vc = va;
            if (vc > 8)
                vc = 8;
            vs = gGameClock + vc;
        }
        if (gNetFifoHead[i] < gNetFifoHead[myconnectindex]-200)
            return 0;
    }
    while (vs > gGameClock)
    {
    }
    if (vc)
        gGameClock -= vc;
    return 1;
}
