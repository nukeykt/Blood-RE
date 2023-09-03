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
#include "aicaleb.h"
#include "build.h"
#include "debug4g.h"
#include "db.h"
#include "dude.h"
#include "gameutil.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "trig.h"

static void SeqAttackCallback(int, int);
static void thinkSearch(SPRITE *, XSPRITE *);
static void thinkGoto(SPRITE *, XSPRITE *);
static void thinkChase(SPRITE *, XSPRITE *);
static void thinkSwimGoto(SPRITE *, XSPRITE *);
static void thinkSwimChase(SPRITE *, XSPRITE *);
static void func_65D04(SPRITE *, XSPRITE *);
static void func_65F44(SPRITE *, XSPRITE *);
static void func_661E0(SPRITE *, XSPRITE *);

static int nAttackClient = seqRegisterClient(SeqAttackCallback);

AISTATE tinycalebIdle = { 0, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE tinycalebChase = { 6, -1, 0, NULL, aiMoveForward, thinkChase, NULL };
AISTATE tinycalebDodge = { 6, -1, 90, NULL, aiMoveDodge, NULL, &tinycalebChase };
AISTATE tinycalebGoto = { 6, -1, 600, NULL, aiMoveForward, thinkGoto, &tinycalebIdle };
AISTATE tinycalebAttack = { 0, nAttackClient, 120, NULL, NULL, NULL, &tinycalebChase };
AISTATE tinycalebSearch = { 6, -1, 120, NULL, aiMoveForward, thinkSearch, &tinycalebIdle };
AISTATE tinycalebRecoil = { 5, -1, 0, NULL, NULL, NULL, &tinycalebDodge };
AISTATE tinycalebTeslaRecoil = { 4, -1, 0, NULL, NULL, NULL, &tinycalebDodge };
AISTATE tinycalebSwimIdle = { 10, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE tinycalebSwimChase = { 8, -1, 0, NULL, func_65D04, thinkSwimChase, NULL };
AISTATE tinycalebSwimDodge = { 8, -1, 90, NULL, aiMoveDodge, NULL, &tinycalebSwimChase };
AISTATE tinycalebSwimGoto = { 8, -1, 600, NULL, aiMoveForward, thinkSwimGoto, &tinycalebSwimIdle };
AISTATE tinycalebSwimSearch = { 8, -1, 120, NULL, aiMoveForward, thinkSearch, &tinycalebSwimIdle };
AISTATE tinycalebSwimAttack = { 10, nAttackClient, 0, NULL, NULL, thinkSwimChase, &tinycalebSwimChase };
AISTATE tinycalebSwimRecoil = { 5, -1, 0, NULL, NULL, NULL, &tinycalebSwimDodge };
AISTATE tinycaleb139660 = { 8, -1, 120, NULL, func_65F44, thinkSwimChase, &tinycalebSwimChase };
AISTATE tinycaleb13967C = { 8, -1, 0, NULL, func_661E0, thinkSwimChase, &tinycalebSwimChase };
AISTATE tinycaleb139698 = { 8, -1, 120, NULL, aiMoveTurn, NULL, &tinycalebSwimChase };

static void SeqAttackCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int dx = Cos(pSprite->ang)>>16;
    int dy = Sin(pSprite->ang)>>16;
    int dz = gDudeSlope[nXSprite];
    dx += Random2(1500);
    dy += Random2(1500);
    dz += Random2(1500);
    for (int i = 0; i < 2; i++)
    {
        actFireVector(pSprite, 0, 0, dx+Random3(1000), dy+Random3(1000), dz+Random3(500), kVectorShot);
    }
    if (Chance(0x8000))
        sfxPlay3DSound(pSprite, 10000+Random(5));
    if (Chance(0x8000))
        sfxPlay3DSound(pSprite, 1001);
    else
        sfxPlay3DSound(pSprite, 1002);
}

static void thinkSearch(SPRITE *pSprite, XSPRITE *pXSprite)
{
    aiChooseDirection(pSprite, pXSprite, pXSprite->at16_0);
    aiThinkTarget(pSprite, pXSprite);
}

static void thinkGoto(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 180);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    SECTOR *pSector = &sector[pSprite->sectnum];
    XSECTOR *pXSector = pSector->extra > 0 ? &xsector[pSector->extra] : NULL;
    int dx = pXSprite->at20_0-pSprite->x;
    int dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    int nDist = approxDist(dx, dy);
    aiChooseDirection(pSprite, pXSprite, nAngle);
    if (nDist < 512 && klabs(pSprite->ang - nAngle) < pDudeInfo->at1b)
    {
        if (pXSector && pXSector->at13_4)
            aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
        else
            aiNewState(pSprite, pXSprite, &tinycalebSearch);
    }
    aiThinkTarget(pSprite, pXSprite);
}

static void thinkChase(SPRITE *pSprite, XSPRITE *pXSprite)
{
    if (pXSprite->target == -1)
    {
        SECTOR *pSector = &sector[pSprite->sectnum];
        XSECTOR *pXSector = pSector->extra > 0 ? &xsector[pSector->extra] : NULL;
        if (pXSector && pXSector->at13_4)
            aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
        else
            aiNewState(pSprite, pXSprite, &tinycalebSearch);
        return;
    }
    int dx, dy, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 266);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 269);
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    dx = pTarget->x-pSprite->x;
    dy = pTarget->y-pSprite->y;
    aiChooseDirection(pSprite, pXSprite, getangle(dx, dy));
    if (pXTarget->health == 0)
    {
        SECTOR *pSector = &sector[pSprite->sectnum];
        XSECTOR *pXSector = pSector->extra > 0 ? &xsector[pSector->extra] : NULL;
        if (pXSector && pXSector->at13_4)
            aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
        else
        {
            aiPlay3DSound(pSprite, 11000+Random(4), AI_SFX_PRIORITY_1, -1);
            aiNewState(pSprite, pXSprite, &tinycalebSearch);
        }
        return;
    }
    if (IsPlayerSprite(pTarget))
    {
        PLAYER *pPlayer = &gPlayer[pTarget->type-kDudePlayer1];
        if (powerupCheck(pPlayer, 13) > 0)
        {
            SECTOR *pSector = &sector[pSprite->sectnum];
            XSECTOR *pXSector = pSector->extra > 0 ? &xsector[pSector->extra] : NULL;
            if (pXSector && pXSector->at13_4)
                aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
            else
                aiNewState(pSprite, pXSprite, &tinycalebSearch);
            return;
        }
    }
    nDist = approxDist(dx, dy);
    if (nDist <= pDudeInfo->at17)
    {
        int nAngle = getangle(dx, dy);
        int nDeltaAngle = ((nAngle+1024-pSprite->ang)&2047)-1024;
        int height = (pDudeInfo->atb*pSprite->yrepeat)<<2;
        if (cansee(pTarget->x, pTarget->y, pTarget->z, pTarget->sectnum, pSprite->x, pSprite->y, pSprite->z - height, pSprite->sectnum))
        {
            if (nDist < pDudeInfo->at17 && klabs(nDeltaAngle) <= pDudeInfo->at1b)
            {
                aiSetTarget(pXSprite, pXSprite->target);
                int nXSprite = sprite[pXSprite->reference].extra;
                gDudeSlope[nXSprite] = divscale(pTarget->z-pSprite->z, nDist, 10);
                if (nDist < 0x599 && klabs(nDeltaAngle) < 28)
                {
                    SECTOR *pSector = &sector[pSprite->sectnum];
                    XSECTOR *pXSector = pSector->extra > 0 ? &xsector[pSector->extra] : NULL;
                    int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                    switch (hit)
                    {
                    case -1:
                        if (pXSector && pXSector->at13_4)
                            aiNewState(pSprite, pXSprite, &tinycalebSwimAttack);
                        else
                            aiNewState(pSprite, pXSprite, &tinycalebAttack);
                        break;
                    case 3:
                        if (sprite[gHitInfo.hitsprite].type != pSprite->type)
                        {
                            if (pXSector && pXSector->at13_4)
                                aiNewState(pSprite, pXSprite, &tinycalebSwimAttack);
                            else
                                aiNewState(pSprite, pXSprite, &tinycalebAttack);
                        }
                        else
                        {
                            if (pXSector && pXSector->at13_4)
                                aiNewState(pSprite, pXSprite, &tinycalebSwimDodge);
                            else
                                aiNewState(pSprite, pXSprite, &tinycalebDodge);
                        }
                        break;
                    default:
                        if (pXSector && pXSector->at13_4)
                            aiNewState(pSprite, pXSprite, &tinycalebSwimAttack);
                        else
                            aiNewState(pSprite, pXSprite, &tinycalebAttack);
                        break;
                    }
                }
            }
            return;
        }
    }
    
    int nXSector = sector[pSprite->sectnum].extra;
    XSECTOR *pXSector = nXSector > 0 ? &xsector[nXSector] : NULL;
    if (pXSector && pXSector->at13_4)
        aiNewState(pSprite, pXSprite, &tinycalebSwimGoto);
    else
        aiNewState(pSprite, pXSprite, &tinycalebGoto);
    if (Chance(0x2000))
        sfxPlay3DSound(pSprite, 10000 + Random(5), -1, 0);
    pXSprite->target = -1;
}

static void thinkSwimGoto(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 473);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    int dx = pXSprite->at20_0-pSprite->x;
    int dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    int nDist = approxDist(dx, dy);
    aiChooseDirection(pSprite, pXSprite, nAngle);
    if (nDist < 512 && klabs(pSprite->ang - nAngle) < pDudeInfo->at1b)
        aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
    aiThinkTarget(pSprite, pXSprite);
}

static void thinkSwimChase(SPRITE *pSprite, XSPRITE *pXSprite)
{
    if (pXSprite->target == -1)
    {
        aiNewState(pSprite, pXSprite, &tinycalebSwimGoto);
        return;
    }
    int dx, dy, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 511);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 514);
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    dx = pTarget->x-pSprite->x;
    dy = pTarget->y-pSprite->y;
    aiChooseDirection(pSprite, pXSprite, getangle(dx, dy));
    if (pXTarget->health == 0)
    {
        aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
        return;
    }
    if (IsPlayerSprite(pTarget))
    {
        PLAYER *pPlayer = &gPlayer[pTarget->type-kDudePlayer1];
        if (powerupCheck(pPlayer, 13) > 0)
        {
            aiNewState(pSprite, pXSprite, &tinycalebSwimSearch);
            return;
        }
    }
    nDist = approxDist(dx, dy);
    if (nDist <= pDudeInfo->at17)
    {
        int nAngle = getangle(dx,dy);
        int nDeltaAngle = ((nAngle+1024-pSprite->ang)&2047)-1024;
        int height = pDudeInfo->atb+pSprite->z;
        int top, bottom;
        GetSpriteExtents(pSprite, &top, &bottom);
        if (cansee(pTarget->x, pTarget->y, pTarget->z, pTarget->sectnum, pSprite->x, pSprite->y, pSprite->z - height, pSprite->sectnum))
        {
            if (nDist < pDudeInfo->at17 && klabs(nDeltaAngle) <= pDudeInfo->at1b)
            {
                aiSetTarget(pXSprite, pXSprite->target);
                int x = pSprite->x;
                int y = pSprite->y;
                int floorZ = getflorzofslope(pSprite->sectnum, x, y);
                if (nDist < 0x400 && klabs(nDeltaAngle) < 85)
                    aiNewState(pSprite, pXSprite, &tinycalebSwimAttack);
                else
                    aiNewState(pSprite, pXSprite, &tinycaleb13967C);
            }
        }
        else
            aiNewState(pSprite, pXSprite, &tinycaleb13967C);
        return;
    }
    aiNewState(pSprite, pXSprite, &tinycalebSwimGoto);
    pXSprite->target = -1;
}

static void func_65D04(SPRITE *pSprite, XSPRITE *pXSprite)
{
    int nSprite = pSprite->index;
    int dx, dy, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 670);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    int nAng = ((pXSprite->at16_0+1024-pSprite->ang)&2047)-1024;
    int nTurnRange = (pDudeInfo->at44<<2)>>4;
    pSprite->ang = (pSprite->ang+ClipRange(nAng, -nTurnRange, nTurnRange))&2047;
    int nAccel = pDudeInfo->at38<<2;
    if (klabs(nAng) > 341)
        return;
    if (pXSprite->target == -1)
        pSprite->ang = (pSprite->ang+256)&2047;
    dx = pXSprite->at20_0-pSprite->x;
    dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    nDist = approxDist(dx, dy);
    if (Random(64) < 32 && nDist <= 0x400)
        return;
    int nSin = Sin(pSprite->ang);
    int nCos = Cos(pSprite->ang);
    int t1 = dmulscale30(xvel[nSprite], nCos, yvel[nSprite], nSin);
    int t2 = dmulscale30(xvel[nSprite], nSin, -yvel[nSprite], nCos);
    if (pXSprite->target == -1)
        t1 += nAccel;
    else
        t1 += nAccel>>2;
    xvel[nSprite] = dmulscale30(t1, nCos, t2, nSin);
    yvel[nSprite] = dmulscale30(t1, nSin, -t2, nCos);
}

static void func_65F44(SPRITE *pSprite, XSPRITE *pXSprite)
{
    int nSprite = pSprite->index;
    int dx, dy, dz, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 733);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    SPRITE *pTarget = &sprite[pXSprite->target];
    int z = pSprite->z + dudeInfo[pSprite->type - kDudeBase].atb;
    int z2 = pTarget->z + dudeInfo[pTarget->type - kDudeBase].atb;
    int nAng = ((pXSprite->at16_0+1024-pSprite->ang)&2047)-1024;
    int nTurnRange = (pDudeInfo->at44<<2)>>4;
    pSprite->ang = (pSprite->ang+ClipRange(nAng, -nTurnRange, nTurnRange))&2047;
    int nAccel = pDudeInfo->at38<<2;
    if (klabs(nAng) > 341)
    {
        pXSprite->at16_0 = (short)((pSprite->ang+512)&2047);
        return;
    }
    dx = pXSprite->at20_0-pSprite->x;
    dy = pXSprite->at24_0-pSprite->y;
    dz = z2 - z;
    int nAngle = getangle(dx, dy);
    nDist = approxDist(dx, dy);
    if (Chance(0x600) && nDist <= 0x400)
        return;
    int nSin = Sin(pSprite->ang);
    int nCos = Cos(pSprite->ang);
    int t1 = dmulscale30(xvel[nSprite], nCos, yvel[nSprite], nSin);
    int t2 = dmulscale30(xvel[nSprite], nSin, -yvel[nSprite], nCos);
    t1 += nAccel;
    xvel[nSprite] = dmulscale30(t1, nCos, t2, nSin);
    yvel[nSprite] = dmulscale30(t1, nSin, -t2, nCos);
    zvel[nSprite] = -dz;
}

static void func_661E0(SPRITE *pSprite, XSPRITE *pXSprite)
{
    int nSprite = pSprite->index;
    int dx, dy, dz, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 796);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    SPRITE *pTarget = &sprite[pXSprite->target];
    int z = pSprite->z + dudeInfo[pSprite->type - kDudeBase].atb;
    int z2 = pTarget->z + dudeInfo[pTarget->type - kDudeBase].atb;
    int nAng = ((pXSprite->at16_0+1024-pSprite->ang)&2047)-1024;
    int nTurnRange = (pDudeInfo->at44<<2)>>4;
    pSprite->ang = (pSprite->ang+ClipRange(nAng, -nTurnRange, nTurnRange))&2047;
    int nAccel = pDudeInfo->at38<<2;
    if (klabs(nAng) > 341)
    {
        pSprite->ang = (pSprite->ang+512)&2047;
        return;
    }
    dx = pXSprite->at20_0-pSprite->x;
    dy = pXSprite->at24_0-pSprite->y;
    dz = (z2 - z)<<3;
    int nAngle = getangle(dx, dy);
    nDist = approxDist(dx, dy);
    if (Chance(0x4000) && nDist <= 0x400)
        return;
    int nSin = Sin(pSprite->ang);
    int nCos = Cos(pSprite->ang);
    int t1 = dmulscale30(xvel[nSprite], nCos, yvel[nSprite], nSin);
    int t2 = dmulscale30(xvel[nSprite], nSin, -yvel[nSprite], nCos);
    t1 += nAccel>>1;
    xvel[nSprite] = dmulscale30(t1, nCos, t2, nSin);
    yvel[nSprite] = dmulscale30(t1, nSin, -t2, nCos);
    zvel[nSprite] = dz;
}
