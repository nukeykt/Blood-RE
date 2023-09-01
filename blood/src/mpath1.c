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
#include <dos.h>

void func_82FE0(void)
{
    union REGS regs;
    regs.w.ax = 0x1680;
    int386(0x2f, &regs, &regs);
}

void func_83014(void)
{
    union REGS regs;
    regs.x.eax = 0x218AA;
    regs.x.ebx = 0;
    regs.x.ecx = 0;
    int386(0x48, &regs, &regs);
}

int func_8304C(int a1)
{
    union REGS regs;
    regs.x.eax = 0x618AA;
    regs.x.ebx = a1;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

int func_83084(int a1, int a2)
{
    union REGS regs;
    regs.x.eax = 0x718AA;
    regs.x.ebx = a1;
    regs.x.ecx = a2;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

int func_830BC(int a1)
{
    union REGS regs;
    regs.x.eax = 0x818AA;
    regs.x.ebx = a1;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

int func_830F4(int *a1)
{
    union REGS regs;
    regs.x.eax = 0xB18AA;
    int386(0x48, &regs, &regs);
    *a1 = regs.x.ecx;
    return regs.x.eax;
}

int func_83130(int a1, int a2)
{
    union REGS regs;
    regs.x.eax = 0x918AA;
    regs.x.ebx = a1;
    regs.x.ecx = a2;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

int func_83168(int a1, int a2)
{
    union REGS regs;
    regs.x.eax = 0xA18AA;
    regs.x.ebx = a1;
    regs.x.ecx = a2;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

int func_831A0(void)
{
    union REGS regs;
    regs.x.eax = 0xC18AA;
    int386(0x48, &regs, &regs);
    return regs.x.eax;
}

void func_831D4(void)
{
    union REGS regs;
    regs.x.eax = 0xD18AA;
    int386(0x48, &regs, &regs);
}
