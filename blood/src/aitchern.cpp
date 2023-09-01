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
#include "aitchern.h"
#include "build.h"
#include "debug4g.h"
#include "db.h"
#include "dude.h"
#include "eventq.h"
#include "gameutil.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "trig.h"

static void func_71A90(int, int);
static void func_71BD4(int, int);
static void func_720AC(int, int);
static void func_72580(SPRITE *, XSPRITE *);
static void func_725A4(SPRITE *, XSPRITE *);
static void func_72850(SPRITE *, XSPRITE *);
static void func_72934(SPRITE *, XSPRITE *);

static int int_279B54 = seqRegisterClient(func_71BD4);
static int int_279B58 = seqRegisterClient(func_720AC);
static int int_279B5C = seqRegisterClient(func_71A90);

AISTATE tchernobogIdle = { 0, -1, 0, NULL, NULL, func_725A4, NULL };
AISTATE tchernobogSearch = { 8, -1, 1800, NULL, aiMoveForward, func_72580, &tchernobogIdle };
AISTATE tchernobogChase = { 8, -1, 0, NULL, aiMoveForward, func_72934, NULL };
AISTATE tchernobogRecoil = { 5, -1, 0, NULL, NULL, NULL, &tchernobogSearch };
AISTATE tcherno13A9B8 = { 8, -1, 600, NULL, aiMoveForward, func_72850, &tchernobogIdle };
AISTATE tcherno13A9D4 = { 6, int_279B54, 60, NULL, NULL, NULL, &tchernobogChase };
AISTATE tcherno13A9F0 = { 6, int_279B58, 60, NULL, NULL, NULL, &tchernobogChase };
AISTATE tcherno13AA0C = { 7, int_279B5C, 60, NULL, NULL, NULL, &tchernobogChase };
AISTATE tcherno13AA28 = { 8, -1, 60, NULL, aiMoveTurn, NULL, &tchernobogChase };

static void func_71A90(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    int nTarget = pTarget->index;
    int nOwner = actSpriteIdToOwnerId(nSprite);
    if (pXTarget->at2c_0 == 0)
        evPost(nTarget, 3, 0, CALLBACK_ID_0);
    actBurnSprite(nOwner, pXTarget, 40);
    if (Chance(0x6000))
        aiNewState(pSprite, pXSprite, &tcherno13A9D4);
}

static void func_71BD4(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 130);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
    int height = pSprite->yrepeat*pDudeInfo->atb;
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 137);
    int x = pSprite->x;
    int y = pSprite->y;
    int z = height;
    TARGETTRACK tt = { 0x10000, 0x10000, 0x100, 0x55, 0x100000 };
    VECTOR3D aim;
    aim.dx = Cos(pSprite->ang)>>16;
    aim.dy = Sin(pSprite->ang)>>16;
    aim.dz = gDudeSlope[nXSprite];
    int nClosest = 0x7fffffff;
    for (short nSprite2 = headspritestat[6]; nSprite2 >= 0; nSprite2 = nextspritestat[nSprite2])
    {
        SPRITE *pSprite2 = &sprite[nSprite2];
        if (pSprite == pSprite2 || !(pSprite2->flags&kSpriteFlag3))
            continue;
        int x2 = pSprite2->x;
        int y2 = pSprite2->y;
        int z2 = pSprite2->z;
        int nDist = approxDist(x2-x, y2-y);
        if (nDist == 0 || nDist > 0x2800)
            continue;
        if (tt.at10)
        {
            int t = divscale(nDist, tt.at10, 12);
            x2 += (xvel[nSprite2]*t)>>12;
            y2 += (yvel[nSprite2]*t)>>12;
            z2 += (zvel[nSprite2]*t)>>8;
        }
        int tx = x+mulscale30(Cos(pSprite->ang), nDist);
        int ty = y+mulscale30(Sin(pSprite->ang), nDist);
        int tz = z+mulscale(gDudeSlope[nXSprite], nDist, 10);
        int tsr = mulscale(9460, nDist, 10);
        int top, bottom;
        GetSpriteExtents(pSprite2, &top, &bottom);
        if (tz-tsr > bottom || tz+tsr < top)
            continue;
        int dx = (tx-x2)>>4;
        int dy = (ty-y2)>>4;
        int dz = (tz-z2)>>8;
        int nDist2 = ksqrt(dx*dx+dy*dy+dz*dz);
        if (nDist2 < nClosest)
        {
            int nAngle = getangle(x2-x, y2-y);
            int nDeltaAngle = ((nAngle-pSprite->ang+1024)&2047)-1024;
            if (klabs(nDeltaAngle) <= tt.at8)
            {
                int tz = pSprite2->z-pSprite->z;
                if (cansee(x, y, z, pSprite->sectnum, x2, y2, z2, pSprite2->sectnum))
                {
                    nClosest = nDist2;
                    aim.dx = Cos(nAngle)>>16;
                    aim.dy = Sin(nAngle)>>16;
                    aim.dz = divscale(tz, nDist, 10);
                }
                else
                    aim.dz = tz;
            }
        }
    }
    actFireMissile(pSprite, -350, 0, aim.dx, aim.dy, aim.dz, 314);
    actFireMissile(pSprite, 350, 0, aim.dx, aim.dy, aim.dz, 314);
}

static void func_720AC(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 245);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
    int height = pSprite->yrepeat*pDudeInfo->atb;
    int ax, ay, az;
    ax = Cos(pSprite->ang)>>16;
    ay = Sin(pSprite->ang)>>16;
    int x = pSprite->x;
    int y = pSprite->y;
    int z = height;
    TARGETTRACK tt = { 0x10000, 0x10000, 0x100, 0x55, 0x100000 };
    VECTOR3D aim;
    aim.dx = ax;
    aim.dy = ay;
    aim.dz = gDudeSlope[nXSprite];
    int nClosest = 0x7fffffff;
    az = 0;
    for (short nSprite2 = headspritestat[6]; nSprite2 >= 0; nSprite2 = nextspritestat[nSprite2])
    {
        SPRITE *pSprite2 = &sprite[nSprite2];
        if (pSprite == pSprite2 || !(pSprite2->flags&kSpriteFlag3))
            continue;
        int x2 = pSprite2->x;
        int y2 = pSprite2->y;
        int z2 = pSprite2->z;
        int nDist = approxDist(x2-x, y2-y);
        if (nDist == 0 || nDist > 0x2800)
            continue;
        if (tt.at10)
        {
            int t = divscale(nDist, tt.at10, 12);
            x2 += (xvel[nSprite2]*t)>>12;
            y2 += (yvel[nSprite2]*t)>>12;
            z2 += (zvel[nSprite2]*t)>>8;
        }
        int tx = x+mulscale30(Cos(pSprite->ang), nDist);
        int ty = y+mulscale30(Sin(pSprite->ang), nDist);
        int tz = z+mulscale(gDudeSlope[nXSprite], nDist, 10);
        int tsr = mulscale(9460, nDist, 10);
        int top, bottom;
        GetSpriteExtents(pSprite2, &top, &bottom);
        if (tz-tsr > bottom || tz+tsr < top)
            continue;
        int dx = (tx-x2)>>4;
        int dy = (ty-y2)>>4;
        int dz = (tz-z2)>>8;
        int nDist2 = ksqrt(dx*dx+dy*dy+dz*dz);
        if (nDist2 < nClosest)
        {
            int nAngle = getangle(x2-x, y2-y);
            int nDeltaAngle = ((nAngle-pSprite->ang+1024)&2047)-1024;
            if (klabs(nDeltaAngle) <= tt.at8)
            {
                int tz = pSprite2->z-pSprite->z;
                if (cansee(x, y, z, pSprite->sectnum, x2, y2, z2, pSprite2->sectnum))
                {
                    nClosest = nDist2;
                    aim.dx = Cos(nAngle)>>16;
                    aim.dy = Sin(nAngle)>>16;
                    aim.dz = divscale(tz, nDist, 10);
                }
                else
                    aim.dz = tz;
            }
        }
    }
    actFireMissile(pSprite, 350, 0, aim.dx, aim.dy, -aim.dz, 314);
    actFireMissile(pSprite, -350, 0, ax, ay, az, 314);
}

static void func_72580(SPRITE *pSprite, XSPRITE *pXSprite)
{
    aiChooseDirection(pSprite, pXSprite, pXSprite->at16_0);
    aiThinkTarget(pSprite, pXSprite);
}

static void func_725A4(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 362);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
    DUDEEXTRA_TCHERNOBOG *pDudeExtraE = &gDudeExtra[pSprite->extra].at6.tchernobog;
    if (pDudeExtraE->at4 && pDudeExtraE->at0 < 10)
        pDudeExtraE->at0++;
    else if (pDudeExtraE->at0 >= 10 && pDudeExtraE->at4)
    {
        pXSprite->at16_0 += 256;
        POINT3D *pTarget = &baseSprite[pSprite->index];
        aiSetTarget(pXSprite, pTarget->x, pTarget->y, pTarget->z);
        aiNewState(pSprite, pXSprite, &tcherno13AA28);
        return;
    }
    if (Chance(pDudeInfo->at33))
    {
        for (int p = connecthead; p >= 0; p = connectpoint2[p])
        {
            PLAYER *pPlayer = &gPlayer[p];
            if (pPlayer->pXSprite->health == 0 || powerupCheck(pPlayer, 13) > 0)
                continue;
            int x = pPlayer->pSprite->x;
            int y = pPlayer->pSprite->y;
            int z = pPlayer->pSprite->z;
            int nSector = pPlayer->pSprite->sectnum;
            int dx = x-pSprite->x;
            int dy = y-pSprite->y;
            int nDist = approxDist(dx, dy);
            if (nDist > pDudeInfo->at17 && nDist > pDudeInfo->at13)
                continue;
            if (!cansee(x, y, z, nSector, pSprite->x, pSprite->y, pSprite->z-((pDudeInfo->atb*pSprite->yrepeat)<<2), pSprite->sectnum))
                continue;
            int nDeltaAngle = ((getangle(dx,dy)+1024-pSprite->ang)&2047)-1024;
            if (nDist < pDudeInfo->at17 && klabs(nDeltaAngle) <= pDudeInfo->at1b)
            {
                pDudeExtraE->at0 = 0;
                aiSetTarget(pXSprite, pPlayer->at5b);
                aiActivateDude(pSprite, pXSprite);
            }
            else if (nDist < pDudeInfo->at13)
            {
                pDudeExtraE->at0 = 0;
                aiSetTarget(pXSprite, x, y, z);
                aiActivateDude(pSprite, pXSprite);
            }
            else
                continue;
            break;
        }
    }
}

static void func_72850(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 445);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    int dx = pXSprite->at20_0-pSprite->x;
    int dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    int nDist = approxDist(dx, dy);
    aiChooseDirection(pSprite, pXSprite, nAngle);
    if (nDist < 512 && klabs(pSprite->ang - nAngle) < pDudeInfo->at1b)
        aiNewState(pSprite, pXSprite, &tchernobogSearch);
    aiThinkTarget(pSprite, pXSprite);
}

static void func_72934(SPRITE *pSprite, XSPRITE *pXSprite)
{
    if (pXSprite->target == -1)
    {
        aiNewState(pSprite, pXSprite, &tcherno13A9B8);
        return;
    }
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 484);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 487);
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    int dx = pTarget->x-pSprite->x;
    int dy = pTarget->y-pSprite->y;
    aiChooseDirection(pSprite, pXSprite, getangle(dx, dy));
    if (pXTarget->health == 0)
    {
        aiNewState(pSprite, pXSprite, &tchernobogSearch);
        return;
    }
    if (IsPlayerSprite(pTarget) && powerupCheck(&gPlayer[pTarget->type-kDudePlayer1], 13) > 0)
    {
        aiNewState(pSprite, pXSprite, &tchernobogSearch);
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
                if (nDist < 0x1f00 && nDist > 0xd00 && klabs(nDeltaAngle) < 85)
                    aiNewState(pSprite, pXSprite, &tcherno13AA0C);
                else if (nDist < 0xd00 && nDist > 0xb00 && klabs(nDeltaAngle) < 85)
                    aiNewState(pSprite, pXSprite, &tcherno13A9D4);
                else if (nDist < 0xb00 && nDist > 0x500 && klabs(nDeltaAngle) < 85)
                    aiNewState(pSprite, pXSprite, &tcherno13A9F0);
                return;
            }
        }
    }

    aiNewState(pSprite, pXSprite, &tcherno13A9B8);
    pXSprite->target = -1;
}
