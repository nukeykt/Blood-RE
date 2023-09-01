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
#include <string.h>
#include "mpath.h"

typedef struct {
    short f_0;
    short f_2;
    short f_4;
} mpathstruct1;

typedef struct {
    char __f_0[2];
    short f_2;
    short f_4;
    char __f_6[2];
    char f_8[1];
} mpathstruct3;

void func_83210(mpathstruct1 *a1)
{
    a1->f_0 = 1;
    a1->f_2 = 0;
    a1->f_4 = 2;
}

void func_83230(char *a1, int *a2)
{
    mpathstruct2 *va;
    mpathstruct1 *vd;
    mpathstruct3 *vd2;

    va = (mpathstruct2*)func_830BC(6);
    if (!va)
    {
        *a2 = 0;
        return;
    }
    vd = va->f_c;
    func_83210(vd);
    vd->f_4 = 1;
    func_83084(6, 8);
    func_83014();
    while ((va = (mpathstruct2*)func_830BC(7)) == NULL)
    {
        func_82FE0();
    }
    vd2 = va->f_c;
    if (vd2->f_2 > *a2 || vd2->f_4)
    {
        *a2 = 0;
        return;
    }
    *a2 = vd2->f_2;
    memcpy(a1, vd2->f_8, vd2->f_2);
    func_83084(7, 6);
}

int func_832E4()
{
    mpathstruct1 *vd;
    mpathstruct3 *vd2;
    mpathstruct2 *va;
    int vb;
    int s;

    va = (mpathstruct2*)func_830BC(6);
    vd = va->f_c;
    func_83210(vd);
    vd->f_4 = 3;
    func_83084(6, 8);
    func_83014();
    while ((va = (mpathstruct2*)func_830BC(7)) == NULL)
    {
        func_82FE0();
    }
    vd2 = va->f_c;
    s = vd2->f_2;
    if (s != 4 || vd2->f_4)
        return -1;
    vb = *(int*)vd2->f_8;
    func_83084(7, 6);
    return vb;
}
