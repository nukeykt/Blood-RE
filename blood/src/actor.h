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
#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "typedefs.h"
#include "db.h"
#include "gameutil.h"
#include "misc.h"

enum DAMAGE_TYPE {
    kDamageFall,
    kDamageBurn,
    kDamageBullet,
    kDamageExplode,
    kDamageDrown,
    kDamageSpirit,
    kDamageTesla,
    kDamageMax
};

enum VECTOR_TYPE {
    kVectorPitchfork = 0,
    kVectorShot,
    kVectorCultistTommy,
    kVectorTommySpread,
    kVectorShotgun,
    kVectorTommy,
    kVectorBatBite,
    kVectorEelBite,
    kVectorGillbeastBite,
    kVectorFireBeast, // no idea what precisely this is...
    kVectorZombieAxe,
    kVectorButcherHack,
    kVectorGhostSlash,
    kVectorSlash,
    kVectorCerberusBite,
    kVectorHoundBite,
    kVectorRatBite,
    kVectorSpiderBite,
    kVectorUnused_18,
    kVectorUnused_19,
    kVectorFireTrap,
    kVectorVoodoo,
    kVectorMax,
};

struct SPRITEHIT {
    int hit;
    int ceilhit;
    int florhit;
};

struct THINGINFO
{
    short at0; // health
    short at2; // mass
    char at4; // clipdist
    ushort at5; // flags
    int at7; // elasticity
    int atb; // damage resistance
    short atf; // cstat
    short at11; // picnum
    schar at13; // shade
    char at14; // pal
    char at15; // xrepeat
    char at16; // yrepeat
    int at17[7]; // damage
};

struct MissileType
{
    short picnum;
    int at2; // speed
    int at6; // angle
    char ata; // xrepeat
    char atb; // yrepeat
    char atc; // shade
    char atd; // clipdist
};

struct EXPLOSION
{
    char at0;
    char at1; // dmg
    char at2; // dmg rnd
    int at3; // radius
    int at7;
    int atb;
    int atf;
    int at13;
    int at17;
};

enum {
    kExplode0 = 0,
    kExplodeMax = 8,
};

enum {
    kThingFlag0 = 1,
    kThingFlag1 = 2,
};

struct AMMOITEMDATA
{
    short at0; // unused?
    short picnum; // at2
    schar shade; // at4
    char at5;
    char xrepeat; // at6
    char yrepeat; // at7
    short at8;
    char ata;
    char atb;
};

struct WEAPONITEMDATA
{
    short at0; // unused?
    short picnum; // at2
    schar shade; // at4
    char at5;
    char xrepeat; // at6
    char yrepeat; // at7
    short at8;
    short ata;
    short atc;
};

struct ITEMDATA
{
    short at0; // unused?
    short picnum; // at2
    schar shade; // at4
    char at5; // unused?
    char xrepeat; // at6
    char yrepeat; // at7
    short at8;
};

enum {
    kSpriteOwnerMask = 0xfff,
    kSpriteOwnerFlag = 0x1000
};

extern AMMOITEMDATA gAmmoItemData[];
extern WEAPONITEMDATA gWeaponItemData[];
extern ITEMDATA gItemData[];

extern SPRITEHIT gSpriteHit[kMaxXSprites];
extern int gDudeDrag;

extern short gAffectedSectors[kMaxSectors];
extern short gAffectedXWalls[kMaxXWalls];

inline BOOL IsPlayerSprite(SPRITE *pSprite)
{
    return (pSprite->type >= kDudePlayer1 && pSprite->type <= kDudePlayer8) ? 1 : 0;
}

inline BOOL IsDudeSprite(SPRITE *pSprite)
{
    return (pSprite->type >= kDudeBase && pSprite->type < kDudeMax) ? 1 : 0;
}

inline int actGetBurnTime(XSPRITE *pXSprite)
{
    return pXSprite->at2c_0;
}

inline void actBurnSprite(int nSource, XSPRITE *pXSprite, int nTime)
{
    int nRange = sprite[pXSprite->reference].statnum == 6 ? 2400 : 1200;
    pXSprite->at2c_0 = ClipHigh(pXSprite->at2c_0 + nTime, nRange);
    pXSprite->at2e_0 = nSource;
}

BOOL IsItemSprite(SPRITE *);
BOOL IsAmmoSprite(SPRITE *);
BOOL IsWeaponSprite(SPRITE *);

BOOL IsUnderwaterSector(int);

int actWallBounceVector(long *, long *, int, int);
int actFloorBounceVector(long *, long *, long *, int, int);
SPRITE *actDropObject(SPRITE *,int);

int actDamageSprite(int, SPRITE *, DAMAGE_TYPE, int);
void actPostSprite(int nSprite, int nStatus);
SPRITE * actFireThing(SPRITE *pSprite, int a2, int a3, int a4, int thingType, int a6);

void func_2A620(int nSprite, int x, int y, int z, int nSector, int nDist, int a7, int a8, DAMAGE_TYPE a9, int a10, int a11, int a12 = 0, int a13 = 0);

SPRITE * actSpawnSprite(int nSector, int x, int y, int z, int nStat, BOOL a6);

BOOL actCheckRespawn(SPRITE *pSprite);

void actExplodeSprite(SPRITE *pSprite);

void actFireVector(SPRITE *, int, int, int, int, int, VECTOR_TYPE);

SPRITE *actSpawnSprite(SPRITE *pSource, int nStat);

BOOL actHealDude(XSPRITE *,int,int);

int actOwnerIdToSpriteId(int);
int actSpriteIdToOwnerId(int);
int actSpriteOwnerToSpriteId(SPRITE *);

int actGetRespawnTime(SPRITE *);

SPRITE *func_36878(SPRITE *pSource, short nType, int a3, int a4);

SPRITE *actFireMissile(SPRITE *, int, int, int, int, int, int);

void actFireVector(SPRITE *, int, int, int, int, int, VECTOR_TYPE);

void actActivateGibObject(SPRITE *,XSPRITE *);

SPRITE *actSpawnThing(int, int, int, int, int);

void actHitcodeToData(int, HITINFO*, int *a3 = 0, SPRITE **a4 = 0, XSPRITE **a5 = 0,
    int *a6 = 0, WALL **a7 = 0, XWALL **a8 = 0, int *a9 = 0, SECTOR **a10 = 0, XSECTOR **a11 = 0);

void actAirDrag(SPRITE*, int);

void actInit(void);

void actProcessSprites(void);
void actPostProcess(void);


#endif // !_ACTOR_H_
