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
#ifndef _CHOKE_H_
#define _CHOKE_H_

#include "typedefs.h"
#include "player.h"
#include "qav.h"
#include "resource.h"

class CChoke {
public:
    char *f_0;
    DICTNODE *f_4;
    QAV *f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    void (*f_1c)(CChoke*, PLAYER*);

    CChoke()
    {
        f_0 = NULL;
        f_4 = NULL;
        f_8 = NULL;
        f_1c = NULL;
        f_14 = 0;
        f_18 = 0;
    };
    
    CChoke(int _x, int _y, char *a1, void(*a2)(CChoke*, PLAYER*));
    void func_83ff0(int a1, void(*a2)(CChoke*, PLAYER*));
    void func_84080(char *a1, void(*a2)(CChoke*, PLAYER*));
    void func_84110(int x, int y);
    void func_84218();
};

void func_84230(CChoke*, PLAYER* pPlayer);

extern CChoke gChoke;

#endif // !_CHOKE_H_
