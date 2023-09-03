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
#include "aicult.h"
#include "build.h"
#include "debug4g.h"
#include "db.h"
#include "dude.h"
#include "eventq.h"
#include "levels.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "trig.h"

static void TommySeqCallback(int, int);
static void TeslaSeqCallback(int, int);
static void ShotSeqCallback(int, int);
static void ThrowSeqCallback(int, int);
static void func_68170(int, int);
static void func_68230(int, int);
static void thinkSearch(SPRITE *, XSPRITE *);
static void thinkGoto(SPRITE *, XSPRITE *);
static void thinkChase(SPRITE *, XSPRITE *);

static int nTommyClient = seqRegisterClient(TommySeqCallback);
static int nTeslaClient = seqRegisterClient(TeslaSeqCallback);
static int nShotClient = seqRegisterClient(ShotSeqCallback);
static int nThrowClient = seqRegisterClient(ThrowSeqCallback);
static int n68170Client = seqRegisterClient(func_68170);
static int n68230Client = seqRegisterClient(func_68230);

AISTATE cultistIdle = { 0, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE cultistProneIdle = { 17, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE fanaticProneIdle = { 17, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE cultistProneIdle3 = { 17, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE cultistChase = { 9, -1, 0, NULL, aiMoveForward, thinkChase, NULL };
AISTATE fanaticChase = { 0, -1, 0, NULL, aiMoveTurn, thinkChase, NULL };
AISTATE cultistDodge = { 9, -1, 90, NULL, aiMoveDodge, NULL, &cultistChase };
AISTATE cultistGoto = { 9, -1, 600, NULL, aiMoveForward, thinkGoto, &cultistIdle };
AISTATE cultistProneChase = { 14, -1, 0, NULL, aiMoveForward, thinkChase, NULL };
AISTATE cultistProneDodge = { 14, -1, 90, NULL, aiMoveDodge, NULL, &cultistProneChase };
AISTATE cultistTThrow = { 7, nThrowClient, 120, NULL, NULL, NULL, &cultistTFire };
AISTATE cultistSThrow = { 7, nThrowClient, 120, NULL, NULL, NULL, &cultistSFire };
AISTATE cultistTsThrow = { 7, nThrowClient, 120, NULL, NULL, NULL, &cultistTsFire };
AISTATE cultistDThrow = { 7, nThrowClient, 120, NULL, NULL, NULL, &cultistChase };
AISTATE cultist139A78 = { 7, n68170Client, 120, NULL, NULL, NULL, &cultistChase };
AISTATE cultist139A94 = { 7, n68230Client, 120, NULL, NULL, NULL, &cultistIdle };
AISTATE cultist139AB0 = { 7, n68230Client, 120, NULL, NULL, thinkSearch, &cultist139A94 };
AISTATE cultist139ACC = { 7, n68230Client, 120, NULL, NULL, thinkSearch, &cultist139AB0 };
AISTATE cultist139AE8 = { 7, n68230Client, 120, NULL, NULL, thinkSearch, &cultist139AE8 };
AISTATE cultistSearch = { 9, -1, 1800, NULL, aiMoveForward, thinkSearch, &cultistIdle };
AISTATE cultistSFire = { 6, nShotClient, 60, NULL, NULL, NULL, &cultistChase };
AISTATE cultistTFire = { 6, nTommyClient, 0, NULL, aiMoveTurn, thinkChase, &cultistTFire };
AISTATE cultistTsFire = { 6, nTeslaClient, 0, NULL, aiMoveTurn, thinkChase, &cultistChase };
AISTATE cultistSProneFire = { 8, nShotClient, 60, NULL, NULL, NULL, &cultistProneChase };
AISTATE cultistTProneFire = { 8, nTommyClient, 0, NULL, aiMoveTurn, thinkChase, &cultistTProneFire };
AISTATE cultistTsProneFire = { 8, nTeslaClient, 0, NULL, aiMoveTurn, NULL, &cultistTsProneFire };
AISTATE cultistRecoil = { 5, -1, 0, NULL, NULL, NULL, &cultistDodge };
AISTATE cultistProneRecoil = { 5, -1, 0, NULL, NULL, NULL, &cultistProneDodge };
AISTATE cultistTeslaRecoil = { 4, -1, 0, NULL, NULL, NULL, &cultistDodge };
AISTATE cultistSwimIdle = { 13, -1, 0, NULL, NULL, aiThinkTarget, NULL };
AISTATE cultistSwimChase = { 13, -1, 0, NULL, aiMoveForward, thinkChase, NULL };
AISTATE cultistSwimDodge = { 13, -1, 90, NULL, aiMoveDodge, NULL, &cultistSwimChase };
AISTATE cultistSwimGoto = { 13, -1, 600, NULL, aiMoveForward, thinkGoto, &cultistSwimIdle };
AISTATE cultistSwimSearch = { 13, -1, 1800, NULL, aiMoveForward, thinkSearch, &cultistSwimIdle };
AISTATE cultistSSwimFire = { 8, nShotClient, 60, NULL, NULL, NULL, &cultistSwimChase };
AISTATE cultistTSwimFire = { 8, nTommyClient, 0, NULL, aiMoveTurn, thinkChase, &cultistTSwimFire };
AISTATE cultistTsSwimFire = { 8, nTeslaClient, 0, NULL, aiMoveTurn, thinkChase, &cultistTsSwimFire };
AISTATE cultistSwimRecoil = { 5, -1, 0, NULL, NULL, NULL, &cultistSwimDodge };

static void TommySeqCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int dx = Cos(pSprite->ang) >> 16;
    int dy = Sin(pSprite->ang) >> 16;
    int dz = gDudeSlope[nXSprite];
    dx += Random3(5000-gGameOptions.nDifficulty*1000);
    dy += Random3(5000-gGameOptions.nDifficulty*1000);
    dz += Random3(2500-gGameOptions.nDifficulty*500);
    actFireVector(pSprite, 0, 0, dx, dy, dz, kVectorCultistTommy);
    sfxPlay3DSound(pSprite, 4001);
}

static void TeslaSeqCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    if (!Chance(int_138BB0[gGameOptions.nDifficulty]))
        return;
    int dx = Cos(pSprite->ang) >> 16;
    int dy = Sin(pSprite->ang) >> 16;
    int dz = gDudeSlope[nXSprite];
    dx += Random3(5000-gGameOptions.nDifficulty*1000);
    dy += Random3(5000-gGameOptions.nDifficulty*1000);
    dz += Random3(2500-gGameOptions.nDifficulty*500);
    actFireMissile(pSprite, 0, 0, dx, dy, dz, 306);
    sfxPlay3DSound(pSprite, 470);
}

static void ShotSeqCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int dx = Cos(pSprite->ang) >> 16;
    int dy = Sin(pSprite->ang) >> 16;
    int dz = gDudeSlope[nXSprite];
    dx += Random2(4500-gGameOptions.nDifficulty*1000);
    dy += Random2(4500-gGameOptions.nDifficulty*1000);
    dz += Random2(2500-gGameOptions.nDifficulty*500);
    for (int i = 0; i < 8; i++)
    {
        actFireVector(pSprite, 0, 0, dx+Random3(1000), dy+Random3(1000), dz+Random3(500), kVectorShot);
    }
    if (Chance(0x8000))
        sfxPlay3DSound(pSprite, 1001);
    else
        sfxPlay3DSound(pSprite, 1002);
}

static void ThrowSeqCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int nMissile = 418;
    if (gGameOptions.nDifficulty > DIFFICULTY_2)
        nMissile = 419;
    char v4 = Chance(0x6000);
    sfxPlay3DSound(pSprite, 455, -1, 0);
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 249);
    SPRITE *pTarget = &sprite[pXSprite->target];
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 253);
    int dx = pTarget->x - pSprite->x;
    int dy = pTarget->y - pSprite->y;
    int dz = pTarget->z - pSprite->z;
    int nDist = approxDist(dx, dy);
    int nDist2 = nDist / 540;
    if (nDist > 0x1e00)
        v4 = 0;
    SPRITE *pMissile = actFireThing(pSprite, 0, 0, dz/128-14500, nMissile, (nDist2<<23)/120);
    if (v4)
        xsprite[pMissile->extra].ate_0 = 1;
    else
        evPost(pMissile->index, 3, 120*(1+Random(2)), COMMAND_ID_1);
}

static void func_68170(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int nMissile = 418;
    if (gGameOptions.nDifficulty > DIFFICULTY_2)
        nMissile = 419;
    sfxPlay3DSound(pSprite, 455, -1, 0);
    SPRITE *pMissile = actFireThing(pSprite, 0, 0, gDudeSlope[nXSprite]-9460, nMissile, 0x133333);
    evPost(pMissile->index, 3, 240+120*Random(2), COMMAND_ID_1);
}

static void func_68230(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int nMissile = 418;
    if (gGameOptions.nDifficulty > DIFFICULTY_2)
        nMissile = 419;
    sfxPlay3DSound(pSprite, 455, -1, 0);
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 372);
    SPRITE *pTarget = &sprite[pXSprite->target];
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 376);
    int dx = pTarget->x - pSprite->x;
    int dy = pTarget->y - pSprite->y;
    int dz = pTarget->z - pSprite->z;
    int nDist = approxDist(dx, dy);
    int nDist2 = nDist / 540;
    SPRITE *pMissile = actFireThing(pSprite, 0, 0, dz/128-14500, nMissile, (nDist2<<17)/120);
    xsprite[pMissile->extra].ate_0 = 1;
}

static char TargetNearExplosion(SPRITE *pSprite)
{
    for (short nSprite = headspritesect[pSprite->sectnum]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        if (sprite[nSprite].type == 418 || sprite[nSprite].statnum == 2)
            return 1;
    }
    return 0;
}

static void thinkSearch(SPRITE *pSprite, XSPRITE *pXSprite)
{
    aiChooseDirection(pSprite, pXSprite, pXSprite->at16_0);
    func_5F15C(pSprite, pXSprite);
}

static void thinkGoto(SPRITE *pSprite, XSPRITE *pXSprite)
{
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 450);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    int dx = pXSprite->at20_0-pSprite->x;
    int dy = pXSprite->at24_0-pSprite->y;
    int nAngle = getangle(dx, dy);
    int nDist = approxDist(dx, dy);
    aiChooseDirection(pSprite, pXSprite, nAngle);
    if (nDist < 5120 && klabs(pSprite->ang - nAngle) < pDudeInfo->at1b)
    {
        switch (pXSprite->at17_6)
        {
        case 0:
            aiNewState(pSprite, pXSprite, &cultistSearch);
            break;
        case 1:
        case 2:
            aiNewState(pSprite, pXSprite, &cultistSwimSearch);
            break;
        }
    }
    aiThinkTarget(pSprite, pXSprite);
}

static void thinkChase(SPRITE *pSprite, XSPRITE *pXSprite)
{
    if (pXSprite->target == -1)
    {
        switch (pXSprite->at17_6)
        {
        case 0:
            aiNewState(pSprite, pXSprite, &cultistGoto);
            break;
        case 1:
        case 2:
            aiNewState(pSprite, pXSprite, &cultistSwimGoto);
            break;
        }
        return;
    }
    int dx, dy, nDist;
    dassert(pSprite->type >= kDudeBase && pSprite->type < kDudeMax, 517);
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type - kDudeBase];
    dassert(pXSprite->target >= 0 && pXSprite->target < kMaxSprites, 520);
    SPRITE *pTarget = &sprite[pXSprite->target];
    XSPRITE *pXTarget = &xsprite[pTarget->extra];
    dx = pTarget->x-pSprite->x;
    dy = pTarget->y-pSprite->y;
    aiChooseDirection(pSprite, pXSprite, getangle(dx, dy));
    if (pXTarget->health == 0)
    {
        switch (pXSprite->at17_6)
        {
        case 0:
            aiNewState(pSprite, pXSprite, &cultistSearch);
            if (pSprite->type == 201)
                aiPlay3DSound(pSprite, 4021+Random(4), AI_SFX_PRIORITY_1, -1);
            else
                aiPlay3DSound(pSprite, 1021+Random(4), AI_SFX_PRIORITY_1, -1);
            break;
        case 1:
        case 2:
            aiNewState(pSprite, pXSprite, &cultistSwimSearch);
            break;
        }
        return;
    }
    if (IsPlayerSprite(pTarget))
    {
        PLAYER *pPlayer = &gPlayer[pTarget->type-kDudePlayer1];
        if (powerupCheck(pPlayer, 13) > 0)
        {
            switch (pXSprite->at17_6)
            {
            case 0:
                aiNewState(pSprite, pXSprite, &cultistSearch);
                break;
            case 1:
            case 2:
                aiNewState(pSprite, pXSprite, &cultistSwimSearch);
                break;
            }
            return;
        }
    }
    nDist = approxDist(dx, dy);
    if (nDist <= pDudeInfo->at17)
    {
        int nAngle = getangle(dx,dy);
        int nDeltaAngle = ((nAngle+1024-pSprite->ang)&2047)-1024;
        int height = (pDudeInfo->atb*pSprite->yrepeat)<<2;
        if (cansee(pTarget->x, pTarget->y, pTarget->z, pTarget->sectnum, pSprite->x, pSprite->y, pSprite->z - height, pSprite->sectnum))
        {
            if (nDist < pDudeInfo->at17 && klabs(nDeltaAngle) <= pDudeInfo->at1b)
            {
                aiSetTarget(pXSprite, pXSprite->target);
                int nXSprite = sprite[pXSprite->reference].extra;
                gDudeSlope[nXSprite] = divscale(pTarget->z-pSprite->z, nDist, 10);
                switch (pSprite->type)
                {
                case 201:
                    if (nDist < 0x1e00 && nDist > 0xe00 && klabs(nDeltaAngle) < 85 && !TargetNearExplosion(pTarget)
                        && (pTarget->flags&kSpriteFlag1) && gGameOptions.nDifficulty > DIFFICULTY_2 && IsPlayerSprite(pTarget) && gPlayer[pTarget->type-kDudePlayer1].at2e
                        && Chance(0x8000))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistTThrow);
                            break;
                        case 0:
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistTThrow);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultistTThrow);
                            break;
                        }
                    }
                    else if (nDist < 0x4600 && klabs(nDeltaAngle) < 28)
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTProneFire);
                            else if (aiSeqPlaying(pSprite, 13) && (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2))
                                aiNewState(pSprite, pXSprite, &cultistTSwimFire);
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202)
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistTFire);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistTProneFire);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistTSwimFire);
                            }
                            else
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistDodge);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistProneDodge);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSwimDodge);
                            }
                            break;
                        default:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistTSwimFire);
                            break;
                        }
                    }
                    break;
                case 202:
                    if (nDist < 0x2c00 && nDist > 0x1400 && !TargetNearExplosion(pTarget)
                        && (pTarget->flags&kSpriteFlag1) && gGameOptions.nDifficulty >= DIFFICULTY_2 && IsPlayerSprite(pTarget) && !gPlayer[pTarget->type-kDudePlayer1].at2e
                        && Chance(0x8000))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        case 0:
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        }
                    }
                    else if (nDist < 0x3200 && klabs(nDeltaAngle) < 28)
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 201)
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistSFire);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistSProneFire);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            }
                            else
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistDodge);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistProneDodge);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSwimDodge);
                            }
                            break;
                        default:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            break;
                        }
                    }
                    break;
                case 247:
                    if (nDist < 0x1e00 && nDist > 0xe00 && !TargetNearExplosion(pTarget)
                        && (pTarget->flags&kSpriteFlag1) && gGameOptions.nDifficulty > DIFFICULTY_2 && IsPlayerSprite(pTarget) && gPlayer[pTarget->type-kDudePlayer1].at2e
                        && Chance(0x8000))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistTsThrow);
                            break;
                        case 0:
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistTsThrow);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultistTsThrow);
                            break;
                        }
                    }
                    else if (nDist < 0x3200 && klabs(nDeltaAngle) < 28)
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTsFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTsProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistTsSwimFire);
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 201)
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistTsFire);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistTsProneFire);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistTsSwimFire);
                            }
                            else
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistDodge);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistProneDodge);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSwimDodge);
                            }
                            break;
                        default:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTsFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistTsProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistTsSwimFire);
                            break;
                        }
                    }
                    break;
                case 248:
                    if (nDist < 0x2c00 && nDist > 0x1400 && klabs(nDeltaAngle) < 85
                        && (pTarget->flags&kSpriteFlag1) && IsPlayerSprite(pTarget))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistDThrow);
                            break;
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistDThrow);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultistDThrow);
                            break;
                        }
                    }
                    else if (nDist < 0x1400 && klabs(nDeltaAngle) < 85
                        && (pTarget->flags&kSpriteFlag1) && IsPlayerSprite(pTarget))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultist139A78);
                            break;
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultist139A78);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultist139A78);
                            break;
                        }
                    }
                    break;
                case 249:
                    if (nDist < 0x1e00 && nDist > 0xe00 && !TargetNearExplosion(pTarget)
                        && (pTarget->flags&kSpriteFlag1) && gGameOptions.nDifficulty > DIFFICULTY_2 && IsPlayerSprite(pTarget) && gPlayer[pTarget->type-kDudePlayer1].at2e
                        && Chance(0x8000))
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        case 0:
                        case 4:
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 202 && pXSprite->at17_6 != 1 && pXSprite->at17_6 != 2)
                                aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        default:
                            aiNewState(pSprite, pXSprite, &cultistSThrow);
                            break;
                        }
                    }
                    else if (nDist < 0x3200 && klabs(nDeltaAngle) < 28)
                    {
                        int hit = HitScan(pSprite, pSprite->z, dx, dy, 0, CLIPMASK1, 0);
                        switch (hit)
                        {
                        case -1:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            break;
                        case 3:
                            if (sprite[gHitInfo.hitsprite].type != pSprite->type && sprite[gHitInfo.hitsprite].type != 201)
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistSFire);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistSProneFire);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            }
                            else
                            {
                                if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistDodge);
                                else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                    aiNewState(pSprite, pXSprite, &cultistProneDodge);
                                else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                    aiNewState(pSprite, pXSprite, &cultistSwimDodge);
                            }
                            break;
                        default:
                            if (!aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSFire);
                            else if (aiSeqPlaying(pSprite, 14) && pXSprite->at17_6 == 0)
                                aiNewState(pSprite, pXSprite, &cultistSProneFire);
                            else if (pXSprite->at17_6 == 1 || pXSprite->at17_6 == 2)
                                aiNewState(pSprite, pXSprite, &cultistSSwimFire);
                            break;
                        }
                    }
                    break;
                }
                return;
            }
        }
    }
    switch (pXSprite->at17_6)
    {
    case 0:
        aiNewState(pSprite, pXSprite, &cultistGoto);
        break;
    case 1:
    case 2:
        aiNewState(pSprite, pXSprite, &cultistSwimGoto);
        break;
    }
    pXSprite->target = -1;
}
