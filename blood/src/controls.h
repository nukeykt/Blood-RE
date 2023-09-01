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
#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include "typedefs.h"

union BUTTONFLAGS
{
    char            byte;
    struct
    {
        unsigned jump           : 1;    // player is jumping (once!)
        unsigned crouch         : 1;    // player is crouching
        unsigned shoot          : 1;    // normal attack
        unsigned shoot2         : 1;    // alternate attack
        unsigned lookUp         : 1;    // > glance or aim up/down
        unsigned lookDown       : 1;    // > if glancing then lookCenter is set
    };
};

union KEYFLAGS
{
    short           word;
    struct
    {
        unsigned action         : 1;    // open or activate
        unsigned jab            : 1;    // quick attack
        unsigned prevItem       : 1;    // next inventory item
        unsigned nextItem       : 1;    // prev inventory item
        unsigned useItem        : 1;    // use inventory item
        unsigned prevWeapon     : 1;    // prev useable weapon
        unsigned nextWeapon     : 1;    // next useable weapon
        unsigned holsterWeapon  : 1;    // holster current weapon

        unsigned lookCenter     : 1;    // used for lookUp/lookDown only
        unsigned lookLeft       : 1;    // > glance or aim up/down
        unsigned lookRight      : 1;    // > if glancing then lookCenter is set
        unsigned spin180        : 1;    // spin 180 degrees

        unsigned pause          : 1;    // pause the game
        unsigned quit           : 1;    // quit the game
        unsigned restart        : 1;    // restart the level
    };
};

union USEFLAGS
{
    char            byte;
    struct
    {
        unsigned useBeastVision     : 1;
        unsigned useCrystalBall     : 1;
        unsigned useJumpBoots       : 1;
        unsigned useMedKit          : 1;
    };
};

union SYNCFLAGS
{
    char            byte;
    struct
    {
        unsigned buttonChange   : 1;
        unsigned keyChange      : 1;
        unsigned useChange      : 1;
        unsigned weaponChange   : 1;
        unsigned mlookChange    : 1;
        unsigned run            : 1;    // player is running
    };
};

struct INPUT
{
    SYNCFLAGS       syncFlags;              // always sent: indicates optional fields
    schar           forward;                // always sent
    sshort          turn;                   // always sent
    schar           strafe;                 // always sent

    // optional fields
    BUTTONFLAGS     buttonFlags;
    KEYFLAGS        keyFlags;
    USEFLAGS        useFlags;
    uchar           newWeapon;      // sent as 0 every frame unless changed
    schar           mlook;
};

extern INPUT gInput;
extern BOOL bSilentAim;

void func_2906C(void);

void ctrlInit(void);

void ctrlGetInput(void);
void ctrlTerm(void);

#endif
