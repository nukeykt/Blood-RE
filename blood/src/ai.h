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
#ifndef _AI_H_
#define _AI_H_

#include "build.h"
#include "actor.h"
#include "db.h"

struct AISTATE {
    int at0; // seq
    int at4; // seq callback
    int at8;
    void(*atc)(SPRITE *, XSPRITE *);
    void(*at10)(SPRITE *, XSPRITE *);
    void(*at14)(SPRITE *, XSPRITE *);
    AISTATE *at18; // next state ?
};

enum AI_SFX_PRIORITY {
    AI_SFX_PRIORITY_0 = 0,
    AI_SFX_PRIORITY_1,
    AI_SFX_PRIORITY_2,
};

struct DUDEEXTRA_SPIDER
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_GHOST
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_CULTIST
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_BAT
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_EEL
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_GILLBEAST
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_ZOMBIEAXE
{
    int at0;
    char at4;
};

struct DUDEEXTRA_ZOMBIEFAT
{
    int at0;
    char at4;
};

struct DUDEEXTRA_GARGOYLE
{
    int at0;
    int at4;
    char at8;
};

struct DUDEEXTRA_TINYCALEB
{
    int at0;
    char at4;
};

struct DUDEEXTRA_BEAST
{
    int at0;
    char at4;
};

struct DUDEEXTRA_CERBERUS
{
    int at0;
    char at4;
};

struct DUDEEXTRA_TCHERNOBOG
{
    int at0;
    char at4;
};

struct DUDEEXTRA {
    int at0;
    BOOL at4;
    AI_SFX_PRIORITY at5;
    union {
        DUDEEXTRA_GHOST ghost;
        DUDEEXTRA_CULTIST cultist;
        DUDEEXTRA_BAT bat;
        DUDEEXTRA_EEL eel;
        DUDEEXTRA_GILLBEAST gillBeast;
        DUDEEXTRA_ZOMBIEAXE zombieAxe;
        DUDEEXTRA_ZOMBIEFAT zombieFat;
        DUDEEXTRA_GARGOYLE gargoyle;
        DUDEEXTRA_SPIDER spider;
        DUDEEXTRA_TINYCALEB tinyCaleb;
        DUDEEXTRA_BEAST beast;
        DUDEEXTRA_CERBERUS cerberus;
        DUDEEXTRA_TCHERNOBOG tchernobog;
    } at6;
};

struct TARGETTRACK {
    int at0;
    int at4;
    int at8; // view angle
    int atc;
    int at10; // Move predict
};

extern DUDEEXTRA gDudeExtra[];
extern int gDudeSlope[kMaxXSprites];

void aiInit(void);

void aiNewState(SPRITE *,XSPRITE *,AISTATE *);

int aiDamageSprite(SPRITE *pSprite, XSPRITE *pXSprite, int nSource, DAMAGE_TYPE nDmgType, int nDamage);

void aiPlay3DSound(SPRITE *pSprite, int a2, AI_SFX_PRIORITY a3, int a4);

void aiSetTarget(XSPRITE *, int);
void aiSetTarget(XSPRITE *, int, int, int);
void aiActivateDude(SPRITE *, XSPRITE *);
void aiProcessDudes(void);
void aiInitSprite(SPRITE *);
BOOL aiSeqPlaying(SPRITE*, int);
void aiChooseDirection(SPRITE *, XSPRITE *, int);
void aiThinkTarget(SPRITE *, XSPRITE *);
void func_5F15C(SPRITE *, XSPRITE *);
void aiMoveForward(SPRITE *, XSPRITE *);
void aiMoveTurn(SPRITE *, XSPRITE *);
void aiMoveDodge(SPRITE *, XSPRITE *);

extern int int_138BB0[5];

#endif
