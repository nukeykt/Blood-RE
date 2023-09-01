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
#ifndef _DUDE_H_
#define _DUDE_H_

#include "typedefs.h"

struct DUDEINFO {
    short seqStartID; // seq
    short at2; // health
    ushort at4; // mass
    int at6; // unused?
    byte ata; // clipdist
    int atb;
    int atf;
    int at13; // target see range?
    int at17; // target see range?
    int at1b; // target see angle range
    int at1f; // unused?
    int at23; // burn health
    int at27; // recoil damage
    int at2b;
    int at2f;
    int at33;
    BOOL at37;
    int at38; // acceleration
    int at3c; // dodge
    int at40; // unused?
    int at44; // turn speed
    int at48[3];
    int at54[7]; // damage
    int at70[7]; // real damage
    int at8c;
    int at90;
};

extern DUDEINFO dudeInfo[];
extern DUDEINFO gPlayerTemplate[];

#endif
