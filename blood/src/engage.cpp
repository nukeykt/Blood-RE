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
#include <i86.h>
#include <stdlib.h>
#include "typedefs.h"
#include "build.h"
#include "network.h"

struct engagecom {
    short f_0;
    short f_2;
    short f_4;
    short f_6;
    short f_8;
    short f_a;
    short f_c;
    short f_e;
    char f_10[1];
};

engagecom *int_285480;

static union REGS regs;

extern "C" int _argc;
extern "C" char **_argv;

int func_83D80(void)
{
    char *token;
    char delims[4] = "\\-/";
    for (int i = _argc-1; i > 0; i--)
    {
        if ((token = strtok(_argv[i], delims)) != 0)
            if (!stricmp("net", token))
                break;
    }
    if (i == 0)
        return 1;

    int_285480 = (engagecom*)atol(_argv[i+1]);
    numplayers = int_285480->f_a;
    myconnectindex = int_285480->f_8-1;
    connecthead = 1;
    for (i = 0; i < numplayers - 1; i++)
        connectpoint2[i+1] = i+2;
    connectpoint2[i+1] = -1;
    int_28E3D4 = 2;
    return 0;
}

short func_83E44(int nDest, char *pBuffer, int nSize)
{
    if (nSize > 2048)
        return 1;
    int_285480->f_2 = 1;
    int_285480->f_4 = nDest + 1;
    memcpy(int_285480->f_10, pBuffer, nSize);
    int_285480->f_6 = nSize;
    int386(int_285480->f_0, &regs, &regs);
    return 0;
}

short func_83EB0(short *pSource, char *pBuffer)
{
    int_285480->f_2 = 2;
    int386(int_285480->f_0, &regs, &regs);
    if (int_285480->f_4 < 0)
        return 0;
    *pSource = int_285480->f_4 - 1;
    int nSize = int_285480->f_6;
    memcpy(pBuffer, int_285480->f_10, nSize);
    return nSize;
}
