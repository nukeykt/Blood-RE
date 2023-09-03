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
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "typedefs.h"
#include "actor.h"
#include "build.h"
#include "controls.h"
#include "db.h"
#include "dude.h"
#include "misc.h"

#define kMaxPowerUps 49

struct PACKINFO {
    BOOL at0;
    int at1;
};

struct POSTURE {
    int at0;
    int at4;
    int at8;
    int atc[2];
    int at14;
    int at18;
    int at1c;
    int at20;
    int at24;
    int at28;
    int at2c;
    int at30;
};

struct PROFILE {
    BOOL at0;
    int skill;
    char name[12];
};

struct AMMOINFO {
    int at0;
    schar at4;
};

struct PLAYER {
    SPRITE *pSprite;
    XSPRITE *pXSprite;
    DUDEINFO *pDudeInfo;
    INPUT atc;
    int at22;
    int at26; // weapon qav
    int at2a; // qav callback
    BOOL at2e; // run
    int at2f; // state
    int at33; // unused?
    int at37;
    int at3b;
    int at3f; // bob height
    int at43; // bob width
    int at47;
    int at4b;
    int at4f; // bob sway y
    int at53; // bob sway x
    int at57; // Connect id
    int at5b; // spritenum
    int at5f; // life mode
    int at63;
    int at67; // view z
    int at6b;
    int at6f; // weapon z
    int at73;
    int at77;
    int at7b; // horiz
    int at7f; // horizoff
    int at83;
    BOOL at87; // underwater
    BOOL at88[8]; // keys
    byte at90; // flag capture
    short at91[8];
    int ata1[7];
    byte atbd; // weapon
    byte atbe; // pending weapon
    int atbf,atc3,atc7;
    byte atcb[14]; // hasweapon
    int atd9[14];
    int at111[2][14];
    //int at149[14];
    int at181[12]; // ammo
    char at1b1;
    int at1b2;
    int at1b6;
    int at1ba;
    VECTOR3D at1be; // world
    //int at1c6;
    VECTOR3D at1ca; // relative
    //int at1ca;
    //int at1ce;
    //int at1d2;
    int at1d6; // aim target sprite
    int at1da;
    short at1de[16];
    int at1fe;
    int at202[kMaxPowerUps];
    int at2c6; // frags
    int at2ca[8];
    int at2ea;
    int at2ee; // killer
    int at2f2;
    int at2f6;
    int at2fa;
    int at2fe;
    int at302;
    int at306;
    int at30a;
    int at30e;
    int at312;
    int at316;
    BOOL at31a; // God mode
    BOOL at31b; // Fall scream
    BOOL at31c;
    int at31d; // pack timer
    int at321; // pack id
    PACKINFO packInfo[5]; // at325
    int at33e[3]; // armor
    //int at342;
    //int at346;
    int playerVoodooTarget; // at34a
    int at34e;
    int at352;
    int at356;
    int at35a; // quake
    int at35e;
    int at362; // light
    int at366;
    int at36a; // blind
    int at36e; // choke
    int at372;
    BOOL at376; // hand
    int at377;
    int at37b; // weapon flash
    int at37f; // quake2
};

enum {
    kModeHuman = 0,
    kModeBeast,
    kModeMax
};

extern PROFILE gProfile[];

extern PLAYER *gMe, *gView;

extern PLAYER gPlayer[];

extern POSTURE gPosture[][3];

extern int int_21EFB0[8];
extern int int_21EFD0[8];
extern AMMOINFO gAmmoInfo[];

BOOL packItemActive(PLAYER *, int);
int powerupCheck(PLAYER *, int);

BOOL playerSeqPlaying(PLAYER *, int);
int playerDamageSprite(int, PLAYER *, DAMAGE_TYPE, int);

void playerResetInertia(PLAYER*);
void playerLandingSound(PLAYER*);

void packUseItem(PLAYER*, int);
int packCheckItem(PLAYER*, int);

void playerVoodooTarget(PLAYER*);

int UseAmmo(PLAYER *, int, int);

SPRITE *playerFireMissile(PLAYER *, int, long, long, long, int);
SPRITE *playerFireThing(PLAYER *, int, int, int, int);

BOOL powerupActivate(PLAYER *, int);
void powerupDeactivate(PLAYER*, int);
void playerSetGodMode(PLAYER*, BOOL);
BOOL packAddItem(PLAYER*, int);

void playerInit(int, unsigned int);

void playerStart(int nPlayer);

void playerProcess(PLAYER *);

void powerupInit(void);

#endif // !_PLAYER_H_
