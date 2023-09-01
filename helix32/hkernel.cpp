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
#include "helix.h"

VGT Video;
int gColor;
int gROP;
int gError;
int gPages;
PAGE_STRUCT gPageTable[4];
int gYLookup[1200];
int nTextMode;

char *ModelName[] = {
    "0:MONO",
    "1:PLANAR",
    "2:PACKEDPIXEL",
    "3:CHAIN256",
    "4:NONCHAIN256"
};

void InstallDriver(VGT **vgt)
{
    Video = **vgt;
    nTextMode = gGetMode();
    Video.Init();
}


void gRestoreMode(void)
{
    gSetMode(nTextMode);
}

int gFindMode(int a1, int a2, int a3, int a4)
{
    int v8 = 0;
    while (a3 >>= 1)
    {
        v8++;
    }

    for (VGT **v4 = &VGTBegin; v4 < &VGTEnd; v4++)
    {
        if ((*v4)->xRes == a1 && (*v4)->yRes == a2 && (*v4)->cRes == v8 && (*v4)->model == a4)
        {
            InstallDriver(v4);
            return TRUE;
        }
    }
    return FALSE;
}

void gEnumDrivers(void)
{
    int v8 = 1;
    for (VGT **v4 = &VGTBegin; v4 < &VGTEnd; v4++)
        printf("%2i: %-30s (%ix%ix%i %s) \n", v8++, (*v4)->name, (*v4)->xRes, (*v4)->yRes, (*v4)->cRes, ModelName[(*v4)->model]);
}
