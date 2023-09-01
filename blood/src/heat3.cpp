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
#include <stdio.h>
#include "heat.h"

int int_29547C;

struct heatstruct4 {
    short f_0;
    short f_2;
    short f_4;
    char f_6[1];
};

struct heatstruct5 {
    char __f_0[2];
    short f_2;
    short f_4;
    char __f_6[2];
    char f_8[1];
};

struct heatstruct6 {
    char __f_0[4];
    short f_4;
};

struct heatstruct7 {
    char __f_0[4];
    short f_4;
};

struct heatstruct8 {
    char __f_0[4];
    short f_4;
};

struct heatstruct9 {
    char __f_0[2];
    short f_2;
    short f_4;
    char __f_6[2];
    int f_8;
};

void func_83A50(heatstruct4 *a1)
{
    a1->f_0 = 1;
    a1->f_2 = 0;
    a1->f_4 = 2;
}

void *func_83A64(heatstruct2 *va)
{
    func_83928(6, 8);
    func_838D4();
    while ((va = (heatstruct2*)func_83954(7)) == NULL)
    {
        func_838B0();
    }
    return va->f_c;
}

void func_83A94(char *a1, int *a2)
{
    heatstruct2 *va;
    heatstruct4 *vd;
    heatstruct5 *vd2;

    va = (heatstruct2*)func_83954(6);
    if (!va)
    {
        *a2 = 0;
        return;
    }
    vd = (heatstruct4*)va->f_c;
    func_83A50(vd);
    vd->f_4 = 1;
    vd2 = (heatstruct5*)func_83A64(va);
    if (vd2->f_2 > *a2 || vd2->f_4 != 0)
    {
        *a2 = 0;
    }
    else
    {
        *a2 = vd2->f_2;
        memcpy(a1, vd2->f_8, vd2->f_2);
    }
    func_83928(7, 6);
}

int func_83B14(void)
{
    heatstruct2 *va;
    heatstruct4 *vd;
    heatstruct6 *vd2;
    va = (heatstruct2*)func_83954(6);
    if (!va)
        return 1;
    vd = (heatstruct4*)va->f_c;
    func_83A50(vd);
    vd->f_4 = 4;
    vd2 = (heatstruct6*)func_83A64(va);
    func_83928(7, 6);
    int_29547C = vd2->f_4 == 0;
    return vd2->f_4;
}

int func_83B74(void)
{
    heatstruct2 *va;
    heatstruct4 *vd;
    heatstruct7 *vd2;
    va = (heatstruct2*)func_83954(6);
    if (!va)
        return 1;
    vd = (heatstruct4*)va->f_c;
    func_83A50(vd);
    vd->f_4 = 6;
    vd2 = (heatstruct7*)func_83A64(va);
    func_83928(7, 6);
    int_29547C = 0;
    return vd2->f_4;
}

int func_83BC8(char *a1, char *a2, char *a3, int a4)
{
    heatstruct2 *va;
    heatstruct4 *vd;
    heatstruct8 *vd2;
    if (!int_29547C)
        return 1;
    va = (heatstruct2*)func_83954(6);
    if (!va)
        return 1;
    vd = (heatstruct4*)va->f_c;
    func_83A50(vd);
    vd->f_4 = 5;
    int *vc= (int*)vd->f_6;
    strcpy(vd->f_6 + 4, a1);
    strcpy(vd->f_6 + 24, a2);
    memcpy(vd->f_6 + 44, a3, a4);
    *vc = a4;
    vd2 = (heatstruct8*)func_83A64(va);
    func_83928(7, 6);
    return vd2->f_4;
}

int func_83CA0(char *a1, char *a2, int a3)
{
    char buf[64];
    sprintf(buf, "%d", a3);
    return func_83BC8(a1, a2, buf, strlen(buf) + 1);
}

int func_83CD8(char *a1, char *a2, char *a3)
{
    return func_83BC8(a1, a2, a3, strlen(a3) + 1);
}

int func_83CF8(void)
{
    heatstruct2 *va;
    heatstruct4 *vd;
    heatstruct9 *vd2;
    int vb;
    int s;

    va = (heatstruct2*)func_83954(6);
    vd = (heatstruct4*)va->f_c;
    func_83A50(vd);
    vd->f_4 = 3;
    func_83928(6, 8);
    func_838D4();
    while ((va = (heatstruct2*)func_83954(7)) == NULL)
    {
        func_838B0();
    }
    vd2 = (heatstruct9*)va->f_c;
    s = vd2->f_2;
    if (s != 4 || vd2->f_4 != 0)
        return -1;
    vb = vd2->f_8;
    func_83928(7, 6);
    return vb;
}
