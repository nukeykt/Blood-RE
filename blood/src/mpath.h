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
#ifndef _MPATH_H_
#define _MPATH_H_

typedef struct {
    char __f_0[12];
    void *f_c;
    char __f_10[6];
    short f_16;
} mpathstruct2;

#ifdef __cplusplus
extern "C" {
#endif
void func_82FE0(void);
void func_83014(void);
int func_83084(int a1, int a2);
int func_830F4(int *a1);

void func_83230(char *a1, int *a2);
int func_832E4(void);
#ifdef __cplusplus
};
#endif

#endif
