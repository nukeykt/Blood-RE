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
#include "typedefs.h"
#include "ai.h"
#include "aipod.h"
#include "build.h"
#include "debug4g.h"
#include "db.h"
#include "dude.h"
#include "fx.h"
#include "gameutil.h"
#include "levels.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "trig.h"

static void func_6FF08(int, int);
static void func_6FF54(int, int);
static void func_6FFA0(int, int);
static void func_70284(int, int);
static void func_7034C(SPRITE *, XSPRITE *);
static void func_70380(SPRITE *, XSPRITE *);
static void func_704D8(SPRITE *, XSPRITE *);

static int int_279B34 = seqRegisterClient(func_6FFA0);
static int int_279B38 = seqRegisterClient(func_70284);
static int int_279B3C = seqRegisterClient(func_6FF08);
static int int_279B40 = seqRegisterClient(func_6FF54);

AISTATE podIdle = { 0, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE pod13A600 = { 7, -1, 3600, NULL, aiMoveTurn, func_70380, &podSearch };
AISTATE podSearch = { 0, -1, 3600, NULL, aiMoveTurn, func_7034C, &podSearch };
AISTATE pod13A638 = { 8, int_279B34, 600, NULL, NULL, NULL, &podChase };
AISTATE podRecoil = { 5, -1, 0, NULL, NULL, NULL, &podChase };
AISTATE podChase = { 6, -1, 0, NULL, aiMoveTurn, func_704D8, NULL };
AISTATE tentacleIdle = { 0, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE tentacle13A6A8 = { 7, int_279B3C, 0, NULL, NULL, NULL, &tentacle13A6C4 };
AISTATE tentacle13A6C4 = { -1, -1, 0, NULL, NULL, NULL, &tentacleChase };
AISTATE tentacle13A6E0 = { 8, int_279B40, 0, NULL, NULL, NULL, &tentacle13A6FC };
AISTATE tentacle13A6FC = { -1, -1, 0, NULL, NULL, NULL, &tentacleIdle };
AISTATE tentacle13A718 = { 8, -1, 3600, NULL, aiMoveTurn, func_70380, &tentacleSearch };
AISTATE tentacleSearch = { 0, -1, 3600, NULL, aiMoveTurn, func_7034C, NULL };
AISTATE tentacle13A750 = { 6, int_279B38, 120, NULL, NULL, NULL, &tentacleChase };
AISTATE tentacleRecoil = { 5, -1, 0, NULL, NULL, NULL, &tentacleChase };
AISTATE tentacleChase = { 6, -1, 0, NULL, aiMoveTurn, func_704D8, NULL };

static void func_6FF08(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    sfxPlay3DSound(&sprite[nSprite], 2503, -1, 0);
}

static void func_6FF54(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    sfxPlay3DSound(&sprite[nSprite], 2500, -1, 0);
}

static void func_6FFA0(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 130);
    SPRITE *pTarget = &sprite[pXSprite->target];
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 134);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
    int x = pTarget->x-pSprite->x;
    int y = pTarget->y-pSprite->y;
    int dz = pTarget->z-pSprite->z;
    x += Random2(1000);
    y += Random2(1000);
    int nDist = approxDist(x, y);
    int nDist2 = nDist / 540;
    SPRITE *pMissile = NULL;
    switch (pSprite->type)
    {
    case 221:
        dz += 8000;
        if (pDudeInfo->at17*0.1 < nDist)
        {
            if (Chance(0x8000))
                sfxPlay3DSound(pSprite, 2474, -1, 0);
            else
                sfxPlay3DSound(pSprite, 2475, -1, 0);
            pMissile = actFireThing(pSprite, 0, -8000, dz/128-14500, 430, (nDist2<<23)/120);
        }
        if (pMissile)
            seqSpawn(68, 3, pMissile->extra, -1);
        break;
    case 223:
        dz += 8000;
        if (pDudeInfo->at17*0.1 < nDist)
        {
            sfxPlay3DSound(pSprite, 2454, -1, 0);
            pMissile = actFireThing(pSprite, 0, -8000, dz/128-14500, 429, (nDist2<<23)/120);
        }
        if (pMissile)
            seqSpawn(22, 3, pMissile->extra, -1);
        break;
    }
    for (int i = 0; i < 4; i++)
        func_746D4(pSprite, 240);
}

static void func_70284(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    sfxPlay3DSound(pSprite, 2502, -1, 0);
    int nDist, nBurn;
    DAMAGE_TYPE dmgType;
    switch (pSprite->type)
    {
    case 222:
    default:
        nBurn = 0;
        dmgType = kDamageBullet;
        nDist = 50;
        break;
    case 224:
        nBurn = (gGameOptions.nDifficulty*120)>>2;
        dmgType = kDamageExplode;
        nDist = 75;
        break;
    }
    func_2A620(nSprite, pSprite->x, pSprite->y, pSprite->z, pSprite->sectnum, nDist, 1, 5*(1+gGameOptions.nDifficulty), dmgType, 2, nBurn, 0, 0);
}

static void func_7034C(SPRITE *pSprite, XSPRITE *pXSprite)
{
    aiChooseDirection(pSprite, pXSprite, pXSprite->at16_0);
    aiThinkTarget(pSprite, pXSprite);
}

static void func_70380(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 379);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    int dx = pXSprite->at20_0-pSprite->x;
    int dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    int nDist = approxDist(dx, dy);
    aiChooseDirection(pSprite, pXSprite, nAngle);
    if (nDist < 512 && klabs(pSprite->ang - nAngle) < pDudeInfo->at1b)
    {
        switch (pSprite->type)
        {
        case 221:
        case 223:
            aiNewState(pSprite, pXSprite, &podSearch);
            break;
        case 222:
        case 224:
            aiNewState(pSprite, pXSprite, &tentacleSearch);
            break;
        }
    }
    aiThinkTarget(pSprite, pXSprite);
}

static void func_704D8(SPRITE *pSprite, XSPRITE *pXSprite)
{
    if (pXSprite->target == -1)
    {
        switch (pSprite->type)
        {
        case 221:
        case 223:
            aiNewState(pSprite, pXSprite, &pod13A600);
            break;
        case 222:
        case 224:
            aiNewState(pSprite, pXSprite, &tentacle13A718);
            break;
        }
        return;
    }
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 450);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 453);
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    int dx = pTarget->x-pSprite->x;
    int dy = pTarget->y-pSprite->y;
    aiChooseDirection(pSprite, pXSprite, getangle(dx, dy));
    if (pXTarget->health == 0)
    {
        switch (pSprite->type)
        {
        case 221:
        case 223:
            aiNewState(pSprite, pXSprite, &podSearch);
            break;
        case 222:
        case 224:
            aiNewState(pSprite, pXSprite, &tentacleSearch);
            break;
        }
        return;
    }
    int nDist = approxDist(dx, dy);
    if (nDist <= pDudeInfo->at17)
    {
        int nDeltaAngle = ((getangle(dx,dy)+1024-pSprite->ang)&2047)-1024;
        int height = (pDudeInfo->atb*pSprite->yrepeat)<<2;
        if (cansee(pTarget->x, pTarget->y, pTarget->z, pTarget->sectnum, pSprite->x, pSprite->y, pSprite->z - height, pSprite->sectnum))
        {
            if (nDist < pDudeInfo->at17 && klabs(nDeltaAngle) <= pDudeInfo->at1b)
            {
                aiSetTarget(pXSprite, pXSprite->target);
                if (klabs(nDeltaAngle) < 85 && pTarget->type != 221 && pTarget->type != 223)
                {
                    switch (pSprite->type)
                    {
                    case 221:
                    case 223:
                        aiNewState(pSprite, pXSprite, &pod13A638);
                        break;
                    case 222:
                    case 224:
                        aiNewState(pSprite, pXSprite, &tentacle13A750);
                        break;
                    }
                }
                return;
            }
        }
    }
    
    switch (pSprite->type)
    {
    case 221:
    case 223:
        aiNewState(pSprite, pXSprite, &pod13A600);
        break;
    case 222:
    case 224:
        aiNewState(pSprite, pXSprite, &tentacle13A718);
        break;
    }
    pXSprite->target = -1;
}
