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
#include "actor.h"
#include "ai.h"
#include "build.h"
#include "callback.h"
#include "db.h"
#include "debug4g.h"
#include "error.h"
#include "eventq.h"
#include "fx.h"
#include "globals.h"
#include "levels.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "sound.h"
#include "trig.h"
#include "triggers.h"
#include "view.h"

static void func_74C20(int nSprite) // 7
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX = gFX.fxSpawn(FX_15, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pFX)
    {
        xvel[pFX->index] = xvel[nSprite] + Random2(0x10000);
        yvel[pFX->index] = yvel[nSprite] + Random2(0x10000);
        zvel[pFX->index] = zvel[nSprite] - Random(0x1aaaa);
    }
    evPost(nSprite, 3, 3, CALLBACK_ID_7);
}

static void func_74D04(int nSprite) // 15
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX = gFX.fxSpawn(FX_49, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pFX)
    {
        xvel[pFX->index] = xvel[nSprite] + Random2(0x1aaaa);
        yvel[pFX->index] = yvel[nSprite] + Random2(0x1aaaa);
        zvel[pFX->index] = zvel[nSprite] - Random(0x1aaaa);
    }
    evPost(nSprite, 3, 3, CALLBACK_ID_15);
}

static void FinishHim(int nSprite) // 13
{
    SPRITE *pSprite = &sprite[nSprite];
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    if (playerSeqPlaying(&gPlayer[pSprite->type-kDudePlayer1], 16) && pXSprite->target == gMe->at5b)
        sndStartSample(3313, -1, 1);
}

static void FlameLick(int nSprite) // 0
{
    SPRITE *pSprite = &sprite[nSprite];
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    for (int i = 0; i < 3; i++)
    {
        int nDist = ((tilesizx[pSprite->picnum]/2)*pSprite->xrepeat)>>2;
        nDist >>= 1;
        int nAngle = Random(2048);
        int dx = mulscale30(nDist, Cos(nAngle));
        int dy = mulscale30(nDist, Sin(nAngle));
        int x = pSprite->x + dx;
        int y = pSprite->y + dy;
        int z = bottom-Random(bottom-top);
        SPRITE *pFX = gFX.fxSpawn(FX_32, pSprite->sectnum, x, y, z);
        if (pFX)
        {
            xvel[pFX->index] = xvel[nSprite] + Random2(-dx);
            yvel[pFX->index] = yvel[nSprite] + Random2(-dy);
            zvel[pFX->index] = zvel[nSprite] - Random(0x1aaaa);
        }
    }
    if (actGetBurnTime(pXSprite) > 0)
        evPost(nSprite, 3, 5, CALLBACK_ID_0);
}

static void Remove(int nSprite) // 1
{
    SPRITE *pSprite = &sprite[nSprite];
    evKill(nSprite, 3);
    if (pSprite->extra > 0)
        seqKill(3, pSprite->extra);
    sfxKill3DSound(pSprite, 0, -1);
    DeleteSprite(nSprite);
}

static void FlareBurst(int nSprite) // 2
{
    dassert(nSprite >= 0 && nSprite < kMaxSprites, 139);
    SPRITE *pSprite = &sprite[nSprite];
    int nRadius = 0x55555;
    int nAngle = getangle(xvel[nSprite], yvel[nSprite]);
    for (int i = 0; i < 8; i++)
    {
        SPRITE *pSpawn = actSpawnSprite(pSprite, 5);
        pSpawn->owner = pSprite->owner;
        pSpawn->picnum = 2424;
        pSpawn->shade = -128;
        pSpawn->xrepeat = pSpawn->yrepeat = 32;
        pSpawn->type = 303;
        pSpawn->clipdist = 2;
        long dx = 0;
        long dy = mulscale30r(nRadius, Sin((i<<11)/8));
        long dz = mulscale30r(nRadius, -Cos((i<<11)/8));
        if (i&1)
        {
            dy >>= 1;
            dz >>= 1;
        }
        RotateVector(&dx, &dy, nAngle);
        xvel[pSpawn->index] += dx;
        yvel[pSpawn->index] += dy;
        zvel[pSpawn->index] += dz;
        evPost(pSpawn->index, 3, 960, CALLBACK_ID_1);
    }
    evPost(nSprite, 3, 0, CALLBACK_ID_1);
}

static void FlareSpark(int nSprite) // 3
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX = gFX.fxSpawn(FX_28, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pFX)
    {
        xvel[pFX->index] = xvel[nSprite] + Random2(0x1aaaa);
        yvel[pFX->index] = yvel[nSprite] + Random2(0x1aaaa);
        zvel[pFX->index] = zvel[nSprite] - Random(0x1aaaa);
    }
    evPost(nSprite, 3, 4, CALLBACK_ID_3);
}

static void FlareSparkLite(int nSprite) // 4
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX = gFX.fxSpawn(FX_28, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pFX)
    {
        xvel[pFX->index] = xvel[nSprite] + Random2(0x1aaaa);
        yvel[pFX->index] = yvel[nSprite] + Random2(0x1aaaa);
        zvel[pFX->index] = zvel[nSprite] - Random(0x1aaaa);
    }
    evPost(nSprite, 3, 12, CALLBACK_ID_4);
}

static void ZombieSpurt(int nSprite) // 5
{
    dassert(nSprite >= 0 && nSprite < kMaxSprites, 212);
    SPRITE *pSprite = &sprite[nSprite];
    int nXSprite = pSprite->extra;
    dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 215);
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    SPRITE *pFX = gFX.fxSpawn(FX_27, pSprite->sectnum, pSprite->x, pSprite->y, top);
    if (pFX)
    {
        xvel[pFX->index] = xvel[nSprite] + Random2(0x11111);
        yvel[pFX->index] = yvel[nSprite] + Random2(0x11111);
        zvel[pFX->index] = zvel[nSprite] - 0x6aaaa;
    }
    if (pXSprite->at10_0 > 0)
    {
        evPost(nSprite, 3, 4, CALLBACK_ID_5);
        pXSprite->at10_0 -= 4;
    }
    else if (pXSprite->at12_0 > 0)
    {
        evPost(nSprite, 3, 60, CALLBACK_ID_5);
        pXSprite->at10_0 = 40;
        pXSprite->at12_0--;
    }
}

static void BloodSpurt(int nSprite) // 6
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX = gFX.fxSpawn(FX_27, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pFX)
    {
        pFX->ang = 0;
        xvel[pFX->index] = xvel[nSprite]>>8;
        yvel[pFX->index] = yvel[nSprite]>>8;
        zvel[pFX->index] = zvel[nSprite]>>8;
    }
    evPost(nSprite, 3, 6, CALLBACK_ID_6);
}

static void DynPuff(int nSprite) // 8
{
    SPRITE *pSprite = &sprite[nSprite];
    if (zvel[nSprite])
    {
        int nDist = ((tilesizx[pSprite->picnum]/2)*pSprite->xrepeat)>>2;
        int x = pSprite->x + mulscale30(nDist, Cos(pSprite->ang-512));
        int y = pSprite->y + mulscale30(nDist, Sin(pSprite->ang-512));
        SPRITE *pFX = gFX.fxSpawn(FX_7, pSprite->sectnum, x, y, pSprite->z);
        if (pFX)
        {
            xvel[pFX->index] = xvel[nSprite];
            yvel[pFX->index] = yvel[nSprite];
            zvel[pFX->index] = zvel[nSprite];
        }
    }
    evPost(nSprite, 3, 12, CALLBACK_ID_8);
}

static void Respawn(int nSprite) // 9
{
    SPRITE *pSprite = &sprite[nSprite];
    int nXSprite = pSprite->extra;
    dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 302);
    XSPRITE *pXSprite = &xsprite[pSprite->extra];
    if (pSprite->statnum != 8 && pSprite->statnum != 4)
        ThrowError(306)("Sprite %d is not on Respawn or Thing list\n", nSprite);
    if (!(pSprite->flags&kSpriteFlag4))
        ThrowError(309)("Sprite %d does not have the respawn attribute\n", nSprite);
    switch (pXSprite->atb_4)
    {
    case 1:
    {
        int nTime = mulscale16(actGetRespawnTime(pSprite), 0x4000);
        pXSprite->atb_4 = 2;
        evPost(nSprite, 3, nTime, CALLBACK_ID_9);
        break;
    }
    case 2:
    {
        int nTime = mulscale16(actGetRespawnTime(pSprite), 0x2000);
        pXSprite->atb_4 = 3;
        evPost(nSprite, 3, nTime, CALLBACK_ID_9);
        break;
    }
    case 3:
    {
        dassert(pSprite->owner != kStatRespawn, 336);
        dassert(pSprite->owner >= 0 && pSprite->owner < kMaxStatus, 338);
        ChangeSpriteStat(nSprite, pSprite->owner);
        pSprite->type = pSprite->inittype;
        pSprite->owner = -1;
        pSprite->flags &= ~16;
        xvel[nSprite] = yvel[nSprite] = zvel[nSprite] = 0;
        pXSprite->atb_4 = 0;
        pXSprite->at2c_0 = 0;
        pXSprite->atd_2 = 0;
        if (IsDudeSprite(pSprite))
        {
            int nType = pSprite->type-kDudeBase;
            pSprite->x = baseSprite[nSprite].x;
            pSprite->y = baseSprite[nSprite].y;
            pSprite->z = baseSprite[nSprite].z;
            pSprite->cstat |= 0x1101;
            pSprite->clipdist = dudeInfo[nType].ata;
            pXSprite->health = dudeInfo[nType].at2<<4;
            if (gSysRes.Lookup(dudeInfo[nType].seqStartID, "SEQ"))
                seqSpawn(dudeInfo[nType].seqStartID, 3, pSprite->extra);
            aiInitSprite(pSprite);
            pXSprite->atd_3 = 0;
        }
        if (pSprite->type == 400)
        {
            pSprite->cstat |= 257;
            pSprite->cstat &= (unsigned short)~32768;
        }
        gFX.fxSpawn(FX_29, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
        sfxPlay3DSound(pSprite, 350);
        break;
    }
    default:
        ThrowError(384)("Unexpected respawnPending value = %d", pXSprite->atb_4);
        break;
    }
}

static void PlayerBubble(int nSprite) // 10
{
    SPRITE *pSprite = &sprite[nSprite];
    PLAYER *pPlayer = NULL;
    if (!IsPlayerSprite(pSprite))
        return;
    pPlayer = &gPlayer[pSprite->type-kDudePlayer1];
    dassert(pPlayer != NULL, 398);
    if (pPlayer->at302)
    {
        int top, bottom;
        GetSpriteExtents(pSprite, &top, &bottom);
        for (int i = 0; i < (pPlayer->at302>>6); i++)
        {
            int nDist = ((tilesizx[pSprite->picnum]/2)*pSprite->xrepeat)>>2;
            int nAngle = Random(2048);
            int x = pSprite->x + mulscale30(nDist, Cos(nAngle));
            int y = pSprite->y + mulscale30(nDist, Sin(nAngle));
            int z = bottom-Random(bottom-top);
            SPRITE *pFX = gFX.fxSpawn((FX_ID)(FX_23+Random(3)), pSprite->sectnum, x, y, z, 0);
            if (pFX)
            {
                xvel[pFX->index] = xvel[nSprite] + Random2(0x1aaaa);
                yvel[pFX->index] = yvel[nSprite] + Random2(0x1aaaa);
                zvel[pFX->index] = zvel[nSprite] + Random2(0x1aaaa);
            }
        }
        evPost(nSprite, 3, 4, CALLBACK_ID_10);
    }
}

static void EnemyBubble(int nSprite) // 11
{
    SPRITE *pSprite = &sprite[nSprite];
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    for (int i = 0; i < (klabs(zvel[nSprite])>>18); i++)
    {
        int nDist = ((tilesizx[pSprite->picnum]/2)*pSprite->xrepeat)>>2;
        int nAngle = Random(2048);
        int x = pSprite->x + mulscale30(nDist, Cos(nAngle));
        int y = pSprite->y + mulscale30(nDist, Sin(nAngle));
        int z = bottom-Random(bottom-top);
        SPRITE *pFX = gFX.fxSpawn((FX_ID)(FX_23+Random(3)), pSprite->sectnum, x, y, z, 0);
        if (pFX)
        {
            xvel[pFX->index] = xvel[nSprite] + Random2(0x1aaaa);
            yvel[pFX->index] = yvel[nSprite] + Random2(0x1aaaa);
            zvel[pFX->index] = zvel[nSprite] + Random2(0x1aaaa);
        }
    }
    evPost(nSprite, 3, 4, CALLBACK_ID_11);
}

static void CounterCheck(int nSector) // 12
{
    dassert(nSector >= 0 && nSector < kMaxSectors, 454);
    SECTOR *pSector = &sector[nSector];
    if (pSector->type != 619)
        return;
    int nXSprite = pSector->extra;
    if (nXSprite > 0)
    {
        XSECTOR *pXSector = &xsector[nXSprite];
        int nType = pXSector->at4_0;
        int nReq = pXSector->atc_0;
        if (nType && nReq)
        {
            int nCount = 0;
            for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
            {
                if (sprite[nSprite].type == nType)
                    nCount++;
            }
            if (nCount >= nReq)
            {
                pXSector->atc_0 = 0;
                trTriggerSector(nSector, pXSector, 1);
            }
            else
                evPost(nSector, 6, 5, CALLBACK_ID_12);
        }
    }
}

void func_76140(int nSprite) // 14
{
    SPRITE *pSprite = &sprite[nSprite];
    long ceilZ, ceilHit, floorZ, floorHit;
    GetZRange(pSprite, &ceilZ, &ceilHit, &floorZ, &floorHit, pSprite->clipdist, CLIPMASK0);
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    pSprite->z += floorZ-bottom;
    int nAngle = Random(2048);
    int nDist = Random(16)<<4;
    int x = pSprite->x+mulscale28(nDist, Cos(nAngle));
    int y = pSprite->y+mulscale28(nDist, Sin(nAngle));
    gFX.fxSpawn(FX_48, pSprite->sectnum, x, y, pSprite->z, 0);
    if (pSprite->ang == 1024)
    {
        int nChannel = 28+(pSprite->index&2);
        dassert(nChannel < 32, 512);
        sfxPlay3DSound(pSprite, 385, nChannel, 1);
    }
    if (Chance(0x5000))
    {
        SPRITE *pFX = gFX.fxSpawn(FX_36, pSprite->sectnum, x, y, floorZ-64, 0);
        if (pFX)
            pFX->ang = nAngle;
    }
    gFX.func_73FFC(nSprite);
}

void func_7632C(SPRITE *pSprite)
{
    int nSprite = pSprite->index;
    xvel[nSprite] = yvel[nSprite] = zvel[nSprite] = 0;
    if (pSprite->extra > 0)
        seqKill(3, pSprite->extra);
    sfxKill3DSound(pSprite);
    switch (pSprite->type)
    {
    case 37:
    case 38:
    case 39:
        pSprite->picnum = 2465;
        break;
    case 40:
    case 41:
    case 42:
        pSprite->picnum = 2464;
        break;
    }
    pSprite->type = 51;
    pSprite->xrepeat = pSprite->yrepeat = 10;
}

int dword_13B32C[] = { 608, 609, 611 };
int dword_13B338[] = { 610, 612 };

static void func_763BC(int nSprite) // 16
{
    SPRITE *pSprite = &sprite[nSprite];
    long ceilZ, ceilHit, floorZ, floorHit;
    GetZRange(pSprite, &ceilZ, &ceilHit, &floorZ, &floorHit, pSprite->clipdist, CLIPMASK0);
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    pSprite->z += floorZ-bottom;
    long zv = zvel[nSprite]-velFloor[pSprite->sectnum];
    if (zv > 0)
    {
        actFloorBounceVector(&xvel[nSprite], &yvel[nSprite], &zv, pSprite->sectnum, 0x9000);
        zvel[nSprite] = zv;
        if (velFloor[pSprite->sectnum] == 0 && klabs(zvel[nSprite]) < 0x20000)
        {
            func_7632C(pSprite);
            return;
        }
        int nChannel = 28+(pSprite->index&2);
        dassert(nChannel < 32, 596);
        if (pSprite->type >= 37 && pSprite->type <= 39)
        {
            Random(3);
            sfxPlay3DSound(pSprite, 608+Random(2), nChannel, 1);
        }
        else
            sfxPlay3DSound(pSprite, dword_13B338[Random(2)], nChannel, 1);
    }
    else if (zvel[nSprite] == 0)
        func_7632C(pSprite);
}

static void func_765B8(int nSprite) // 17
{
    SPRITE *pSprite = &sprite[nSprite];
    if (pSprite->owner >= 0 && pSprite->owner < kMaxSprites)
    {
        SPRITE *pOwner = &sprite[pSprite->owner];
        XSPRITE *pXOwner = &xsprite[pOwner->extra];
        switch (pSprite->type)
        {
        case 147:
            trTriggerSprite(pOwner->index, pXOwner, 1);
            sndStartSample(8003, 255, 2);
            viewSetMessage("Blue Flag returned to base.");
            break;
        case 148:
            trTriggerSprite(pOwner->index, pXOwner, 1);
            sndStartSample(8002, 255, 2);
            viewSetMessage("Red Flag returned to base.");
            break;
        }
    }
    evPost(nSprite, 3, 0, CALLBACK_ID_1);
}

static void func_766B8(int nSprite) // 19
{
    SPRITE *pSprite = &sprite[nSprite];
    long ceilZ, ceilHit, floorZ, floorHit;
    GetZRange(pSprite, &ceilZ, &ceilHit, &floorZ, &floorHit, pSprite->clipdist, CLIPMASK0);
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    pSprite->z += floorZ-bottom;
    int nAngle = Random(2048);
    int nDist = Random(16)<<4;
    int x = pSprite->x+mulscale28(nDist, Cos(nAngle));
    int y = pSprite->y+mulscale28(nDist, Sin(nAngle));
    if (pSprite->ang == 1024 && pSprite->type == 53)
    {
        int nChannel = 28+(pSprite->index&2);
        dassert(nChannel < 32, 683);
        sfxPlay3DSound(pSprite, 385, nChannel, 1);
    }
    SPRITE *pFX = NULL;
    if (pSprite->type == 53 || pSprite->type == 430)
    {
        if (Chance(0x500) || pSprite->type == 430)
            pFX = gFX.fxSpawn(FX_55, pSprite->sectnum, x, y, floorZ-64, 0);
        if (pFX)
            pFX->ang = nAngle;
    }
    else
    {
        pFX = gFX.fxSpawn(FX_32, pSprite->sectnum, x, y, floorZ-64, 0);
        if (pFX)
            pFX->ang = nAngle;
    }
    gFX.func_73FFC(nSprite);
}

static void func_768E8(int nSprite) // 18
{
    SPRITE *pSprite = &sprite[nSprite];
    SPRITE *pFX;
    if (pSprite->type == 53)
        pFX = gFX.fxSpawn(FX_53, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z, 0);
    else
        pFX = gFX.fxSpawn(FX_54, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z, 0);
    if (pFX)
    {
        pFX->ang = 0;
        xvel[pFX->index] = xvel[nSprite]>>8;
        yvel[pFX->index] = yvel[nSprite]>>8;
        zvel[pFX->index] = zvel[nSprite]>>8;
    }
    evPost(nSprite, 3, 6, CALLBACK_ID_18);
}

static void func_769B4(int nSprite) // 19
{
    SPRITE *pSprite = &sprite[nSprite];
    if (pSprite->statnum == 4 && pSprite->type == 431 && !(pSprite->flags&kSpriteFlag5))
        xsprite[pSprite->extra].at32_0 = 0;
}

void func_76A08(SPRITE *pSprite, SPRITE *pSprite2, PLAYER *pPlayer)
{
    int nSprite = pSprite->index;
    int top, bottom;
    GetSpriteExtents(pSprite, &top, &bottom);
    pSprite->x = pSprite2->x;
    pSprite->y = pSprite2->y;
    pSprite->z = sector[pSprite2->sectnum].floorz-(bottom-pSprite->z);
    pSprite->ang = pSprite2->ang;
    ChangeSpriteSect(nSprite, pSprite2->sectnum);
    sfxPlay3DSound(pSprite2, 201);
    xvel[nSprite] = yvel[nSprite] = zvel[nSprite] = 0;
    ClearBitString(gInterpolateSprite, nSprite);
    viewBackupSpriteLoc(nSprite, pSprite);
    if (pPlayer != NULL)
    {
        playerResetInertia(pPlayer);
        pPlayer->at6b = pPlayer->at73 = 0;
    }
}

static void func_76B78(int nSprite)
{
    SPRITE *pSprite = &sprite[nSprite];
    int nOwner = actSpriteOwnerToSpriteId(pSprite);
    if (nOwner < 0 || nOwner >= kMaxSprites)
    {
        evPost(nSprite, 3, 0, CALLBACK_ID_1);
        return;
    }
    SPRITE *pOwner = &sprite[nOwner];
    PLAYER *pPlayer = IsPlayerSprite(pOwner) ? &gPlayer[pOwner->type - kDudePlayer1] : NULL;
    if (!pPlayer)
    {
        evPost(nSprite, 3, 0, CALLBACK_ID_1);
        return;
    }
    int dx = pOwner->x - pSprite->x;
    int dy = pOwner->y - pSprite->y;
    pSprite->ang = getangle(dx, dy);
    int nXSprite = pSprite->extra;
    if (nXSprite > 0)
    {
        XSPRITE *pXSprite = &xsprite[nXSprite];
        if (pXSprite->at10_0 == 0)
        {
            evPost(nSprite, 3, 0, CALLBACK_ID_1);
            return;
        }
        int nSprite2, nNextSprite;
        for (nSprite2 = headspritestat[6]; nSprite2 >= 0; nSprite2 = nNextSprite)
        {
            nNextSprite = nextspritestat[nSprite2];
            if (nOwner == nSprite2)
                continue;
            SPRITE *pSprite2 = &sprite[nSprite2];
            int nXSprite2 = pSprite2->extra;
            if (nXSprite2 < 0 || nXSprite2 >= kMaxXSprites)
                continue;
            if (pSprite2->flags & kSpriteFlag5)
                continue;
            XSPRITE *pXSprite2 = &xsprite[nXSprite2];
            PLAYER *pPlayer2 = IsPlayerSprite(pSprite2) ? &gPlayer[pSprite2->type-kDudePlayer1] : NULL;
            if (pXSprite2->health > 0 && (pPlayer2 || pXSprite2->atd_3 == 0))
            {
                if (pPlayer2)
                {
                    if (gGameOptions.nGameType == GAMETYPE_1)
                        continue;
                    if (gGameOptions.nGameType == GAMETYPE_3 && pPlayer->at2ea == pPlayer2->at2ea)
                        continue;
                    int x = gNetPlayers - 1;
                    if (x < 1)
                        x = 1;
                    int t = 0x8000/x;
                    if (!powerupCheck(pPlayer2, 14))
                        t += ((3200-pPlayer2->at33e[2])<<15)/3200;
                    if (Chance(t) || nNextSprite < 0)
                    {
                        int nDmg = actDamageSprite(nOwner, pSprite2, kDamageSpirit, pXSprite->at10_0<<4);
                        pXSprite->at10_0 = ClipLow(pXSprite->at10_0-nDmg, 0);
                        func_76A08(pSprite2, pSprite, pPlayer2);
                        evPost(nSprite, 3, 0, CALLBACK_ID_1);
                        return;
                    }
                }
                else
                {
                    int vd = 0x2666;
                    switch (pSprite2->type)
                    {
                    case 218:
                    case 219:
                    case 220:
                    case 250:
                    case 251:
                        vd = 0x147;
                        break;
                    case 227:
                    case 228:
                        vd = 0;
                        break;
                    case 252:
                    case 253:
                        vd = 0;
                        break;
                    case 239:
                    case 240:
                    case 241:
                    case 242:
                    case 243:
                    case 244:
                    case 245:
                        vd = 0;
                        break;
                    case 205:
                    case 221:
                    case 222:
                    case 223:
                    case 224:
                    case 225:
                    case 226:
                    case 229:
                        vd = 0;
                        break;
                    case 200:
                    case 201:
                    case 202:
                    case 203:
                    case 204:
                    case 217:
                        break;
                    }
                    if (vd && (Chance(vd) || nNextSprite < 0))
                    {
                        func_76A08(pSprite2, pSprite, NULL);
                        evPost(nSprite, 3, 0, CALLBACK_ID_1);
                        return;
                    }
                }
            }
        }
        pXSprite->at10_0 = ClipLow(pXSprite->at10_0-1, 0);
        evPost(pSprite->index, 3, 10, CALLBACK_ID_21);
    }
}



void (*gCallback[kCallbackMax])(int) = {
    FlameLick,
    Remove,
    FlareBurst,
    FlareSpark,
    FlareSparkLite,
    ZombieSpurt,
    BloodSpurt,
    func_74C20,
    DynPuff,
    Respawn,
    PlayerBubble,
    EnemyBubble,
    CounterCheck,
    FinishHim,
    func_76140,
    func_74D04,
    func_763BC,
    func_765B8,
    func_768E8,
    func_766B8,
    func_769B4,
    func_76B78
};

