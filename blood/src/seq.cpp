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
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"
#include "build.h"
#include "callback.h"
#include "db.h"
#include "debug4g.h"
#include "error.h"
#include "eventq.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "resource.h"
#include "seq.h"
#include "sfx.h"
#include "tile.h"

SEQINST siWall[kMaxXWalls];
SEQINST siMasked[kMaxXWalls];
SEQINST siCeiling[kMaxXSectors];
SEQINST siFloor[kMaxXSectors];
SEQINST siSprite[kMaxXSprites];

#define kMaxClients 256
#define kMaxSequences 1024

static ACTIVE activeList[kMaxSequences];
static int activeCount = 0;
static void(*clientCallback[kMaxClients])(int, int);
static int nClients = 0;

int seqRegisterClient(void(*pClient)(int, int))
{
    dassert(nClients < kMaxClients, 44);
    int id = nClients++;
    clientCallback[id] = pClient;
    return id;
}

void Seq::Preload(void)
{
    if (memcmp(signature, "SEQ\x1a", 4) != 0)
        ThrowError(53)("Invalid sequence");
    if ((version&0xff00) != 0x300)
        ThrowError(56)("Obsolete sequence version");
    for (int i = 0; i < nFrames; i++)
        tilePreloadTile(frames[i].tile);
}

void Seq::Precache(void)
{
    if (memcmp(signature, "SEQ\x1a", 4) != 0)
        ThrowError(66)("Invalid sequence");
    if ((version&0xff00) != 0x300)
        ThrowError(69)("Obsolete sequence version");
    for (int i = 0; i < nFrames; i++)
        tilePrecacheTile(frames[i].tile);
}

void seqCache(int id)
{
    DICTNODE *hSeq = gSysRes.Lookup(id, "SEQ");
    if (!hSeq)
        return;
    Seq *pSeq = (Seq*)gSysRes.Lock(hSeq);
    pSeq->Precache();
    gSysRes.Unlock(hSeq);
}

static void UpdateSprite(int nXSprite, SEQFRAME *pFrame)
{
    dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 114);
    int nSprite = xsprite[nXSprite].reference;
    dassert(nSprite >= 0 && nSprite < kMaxSprites, 116);
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pSprite->extra == nXSprite, 118);
    if (pSprite->flags & kSpriteFlag1)
    {
        if (tilesizy[pSprite->picnum] != tilesizy[pFrame->tile] || picanm[pSprite->picnum].yoffset != picanm[pFrame->tile].yoffset
            || (pFrame->at3_0 && pFrame->at3_0 != pSprite->yrepeat))
            pSprite->flags |= kSpriteFlag2;
    }
    pSprite->picnum = pFrame->tile;
    if (pFrame->at5_0)
        pSprite->pal = pFrame->at5_0;
    pSprite->shade = pFrame->at4_0;
    if (pFrame->at2_0)
        pSprite->xrepeat = pFrame->at2_0;
    if (pFrame->at3_0)
        pSprite->yrepeat = pFrame->at3_0;
    if (pFrame->at1_4)
        pSprite->cstat |= 2;
    else
        pSprite->cstat &= ~2;
    if (pFrame->at1_5)
        pSprite->cstat |= 512;
    else
        pSprite->cstat &= ~512;
    if (pFrame->at1_6)
        pSprite->cstat |= 1;
    else
        pSprite->cstat &= ~1;
    if (pFrame->at1_7)
        pSprite->cstat |= 256;
    else
        pSprite->cstat &= ~256;
    if (pFrame->at6_2)
        pSprite->cstat |= 32768;
    else
        pSprite->cstat &= ~32768;
    if (pFrame->at6_0)
        pSprite->cstat |= 4096;
    else
        pSprite->cstat &= ~4096;
    if (pFrame->at5_6)
        pSprite->flags |= kSpriteFlag8;
    else
        pSprite->flags &= ~kSpriteFlag8;
    if (pFrame->at5_7)
        pSprite->flags |= kSpriteFlag3;
    else
        pSprite->flags &= ~kSpriteFlag3;
    if (pFrame->at6_3)
        pSprite->flags |= kSpriteFlag10;
    else
        pSprite->flags &= ~kSpriteFlag10;
    if (pFrame->at6_4)
        pSprite->flags |= kSpriteFlag11;
    else
        pSprite->flags &= ~kSpriteFlag11;
}

static void UpdateWall(int nXWall, SEQFRAME *pFrame)
{
    dassert(nXWall > 0 && nXWall < kMaxXWalls, 194);
    int nWall = xwall[nXWall].reference;
    dassert(nWall >= 0 && nWall < kMaxWalls, 196);
    WALL *pWall = &wall[nWall];
    dassert(pWall->extra == nXWall, 198);
    pWall->picnum = pFrame->tile;
    if (pFrame->at5_0)
        pWall->pal = pFrame->at5_0;
    if (pFrame->at1_4)
        pWall->cstat |= 128;
    else
        pWall->cstat &= ~128;
    if (pFrame->at1_5)
        pWall->cstat |= 512;
    else
        pWall->cstat &= ~512;
    if (pFrame->at1_6)
        pWall->cstat |= 1;
    else
        pWall->cstat &= ~1;
    if (pFrame->at1_7)
        pWall->cstat |= 64;
    else
        pWall->cstat &= ~64;
}

static void UpdateMasked(int nXWall, SEQFRAME *pFrame)
{
    dassert(nXWall > 0 && nXWall < kMaxXWalls, 229);
    int nWall = xwall[nXWall].reference;
    dassert(nWall >= 0 && nWall < kMaxWalls, 231);
    WALL *pWall = &wall[nWall];
    dassert(pWall->extra == nXWall, 233);
    dassert(pWall->nextwall >= 0, 234);
    WALL *pWallNext = &wall[pWall->nextwall];
    pWall->overpicnum = pWallNext->overpicnum = pFrame->tile;
    if (pFrame->at5_0)
        pWall->pal = pWallNext->pal = pFrame->at5_0;
    if (pFrame->at1_4)
    {
        pWall->cstat |= 128;
        pWallNext->cstat |= 128;
    }
    else
    {
        pWall->cstat &= ~128;
        pWallNext->cstat &= ~128;
    }
    if (pFrame->at1_5)
    {
        pWall->cstat |= 512;
        pWallNext->cstat |= 512;
    }
    else
    {
        pWall->cstat &= ~512;
        pWallNext->cstat &= ~512;
    }
    if (pFrame->at1_6)
    {
        pWall->cstat |= 1;
        pWallNext->cstat |= 1;
    }
    else
    {
        pWall->cstat &= ~1;
        pWallNext->cstat &= ~1;
    }
    if (pFrame->at1_7)
    {
        pWall->cstat |= 64;
        pWallNext->cstat |= 64;
    }
    else
    {
        pWall->cstat &= ~64;
        pWallNext->cstat &= ~64;
    }
}

static void UpdateFloor(int nXSector, SEQFRAME *pFrame)
{
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 290);
    int nSector = xsector[nXSector].reference;
    dassert(nSector >= 0 && nSector < kMaxSectors, 292);
    SECTOR *pSector = &sector[nSector];
    dassert(pSector->extra == nXSector, 294);
    pSector->floorpicnum = pFrame->tile;
    pSector->floorshade = pFrame->at4_0;
    if (pFrame->at5_0)
        pSector->floorpal = pFrame->at5_0;
}

static void UpdateCeiling(int nXSector, SEQFRAME *pFrame)
{
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 305);
    int nSector = xsector[nXSector].reference;
    dassert(nSector >= 0 && nSector < kMaxSectors, 307);
    SECTOR *pSector = &sector[nSector];
    dassert(pSector->extra == nXSector, 309);
    pSector->ceilingpicnum = pFrame->tile;
    pSector->ceilingshade = pFrame->at4_0;
    if (pFrame->at5_0)
        pSector->ceilingpal = pFrame->at5_0;
}

void SEQINST::Update(ACTIVE *pActive)
{
    dassert(frameIndex < pSequence->nFrames, 320);
    switch (pActive->type)
    {
    case 0:
        UpdateWall(pActive->xindex, &pSequence->frames[frameIndex]);
        break;
    case 1:
        UpdateCeiling(pActive->xindex, &pSequence->frames[frameIndex]);
        break;
    case 2:
        UpdateFloor(pActive->xindex, &pSequence->frames[frameIndex]);
        break;
    case 3:
        UpdateSprite(pActive->xindex, &pSequence->frames[frameIndex]);
        if (pSequence->frames[frameIndex].at6_1)
        {
            SPRITE* pSprite = &sprite[xsprite[pActive->xindex].reference];
            sfxPlay3DSound(pSprite, pSequence->ata);
        }
        break;
    case 4:
        UpdateMasked(pActive->xindex, &pSequence->frames[frameIndex]);
        break;
    }
    if (pSequence->frames[frameIndex].at5_5 && atc != -1)
        clientCallback[atc](pActive->type, pActive->xindex);
}

SEQINST * GetInstance(int a1, int a2)
{
    switch (a1)
    {
    case 0:
        if (a2 > 0 && a2 < kMaxXWalls) return &siWall[a2];
        break;
    case 1:
        if (a2 > 0 && a2 < kMaxXSectors) return &siCeiling[a2];
        break;
    case 2:
        if (a2 > 0 && a2 < kMaxXSectors) return &siFloor[a2];
        break;
    case 3:
        if (a2 > 0 && a2 < kMaxXSprites) return &siSprite[a2];
        break;
    case 4:
        if (a2 > 0 && a2 < kMaxXWalls) return &siMasked[a2];
        break;
    }
    return NULL;
}

void UnlockInstance(SEQINST *pInst)
{
    dassert(pInst != NULL, 411);
    dassert(pInst->hSeq != NULL, 412);
    dassert(pInst->pSequence != NULL, 413);
    gSysRes.Unlock(pInst->hSeq);
    pInst->hSeq = NULL;
    pInst->pSequence = NULL;
    pInst->at13 = 0;
}

void seqSpawn(int a1, int a2, int a3, int a4)
{
    SEQINST *pInst = GetInstance(a2,a3);
    if (!pInst)
        return;
    DICTNODE *hSeq = gSysRes.Lookup(a1, "SEQ");
    if (!hSeq)
        ThrowError(435)("Missing sequence #%d", a1);
    int i = activeCount;
    if (pInst->at13)
    {
        if (pInst->hSeq == hSeq)
            return;
        UnlockInstance(pInst);
        for (i = 0; i < activeCount; i++)
        {
            if (activeList[i].type == a2 && activeList[i].xindex == a3)
                break;
        }
        dassert(i < activeCount, 452);
    }
    Seq *pSeq = (Seq*)gSysRes.Lock(hSeq);
    if (memcmp(pSeq->signature, "SEQ\x1a", 4) != 0)
        ThrowError(458)("Invalid sequence %d", a1);
    if ((pSeq->version & 0xff00) != 0x300)
        ThrowError(461)("Sequence %d is obsolete version", a1);
    pInst->hSeq = hSeq;
    pInst->pSequence = pSeq;
    pInst->at8 = a1;
    pInst->atc = a4;
    pInst->at13 = 1;
    pInst->at10 = pSeq->at8;
    pInst->frameIndex = 0;
    if (i == activeCount)
    {
        dassert(activeCount < kMaxSequences, 473);
        activeList[activeCount].type = a2;
        activeList[activeCount].xindex = a3;
        activeCount++;
    }
    pInst->Update(&activeList[i]);
}

void seqKill(int a1, int a2)
{
    SEQINST *pInst = GetInstance(a1, a2);
    if (!pInst || !pInst->at13)
        return;
    int i;
    for (i = 0; i < activeCount; i++)
    {
        if (activeList[i].type == a1 && activeList[i].xindex == a2)
            break;
    }
    dassert(i < activeCount, 499);
    activeCount--;
    activeList[i] = activeList[activeCount];
    pInst->at13 = 0;
    UnlockInstance(pInst);
}

void seqKillAll(void)
{
    for (int i = 0; i < kMaxXWalls; i++)
    {
        if (siWall[i].at13)
            UnlockInstance(&siWall[i]);
        if (siMasked[i].at13)
            UnlockInstance(&siMasked[i]);
    }
    for (i = 0; i < kMaxXSectors; i++)
    {
        if (siCeiling[i].at13)
            UnlockInstance(&siCeiling[i]);
        if (siFloor[i].at13)
            UnlockInstance(&siFloor[i]);
    }
    for (i = 0; i < kMaxXSprites; i++)
    {
        if (siSprite[i].at13)
            UnlockInstance(&siSprite[i]);
    }
    activeCount = 0;
}

int seqGetStatus(int a1, int a2)
{
    SEQINST *pInst = GetInstance(a1, a2);
    if (pInst && pInst->at13)
        return pInst->frameIndex;
    return -1;
}

int seqGetID(int a1, int a2)
{
    SEQINST *pInst = GetInstance(a1, a2);
    if (pInst)
        return pInst->at8;
    return -1;
}

void seqProcess(int a1)
{
    for (int i = 0; i < activeCount; i++)
    {
        SEQINST *pInst = GetInstance(activeList[i].type, activeList[i].xindex);
        Seq *pSeq = pInst->pSequence;
        dassert(pInst->frameIndex < pSeq->nFrames, 594);
        pInst->at10 -= a1;
        while (pInst->at10 < 0)
        {
            pInst->at10 += pSeq->at8;
            pInst->frameIndex++;
            if (pInst->frameIndex == pSeq->nFrames)
            {
                if (pSeq->atc & kSeqFlag0)
                    pInst->frameIndex = 0;
                else
                {
                    UnlockInstance(pInst);
                    if (pSeq->atc & kSeqFlag1)
                    {
                        switch (activeList[i].type)
                        {
                        case 3:
                        {
                            int nSprite = xsprite[activeList[i].xindex].reference;
                            dassert(nSprite >= 0 && nSprite < kMaxSprites, 618);
                            evKill(nSprite, 3);
                            if ((sprite[nSprite].flags & kSpriteFlag4) && sprite[nSprite].inittype >= kDudeBase && sprite[nSprite].inittype < kDudeMax)
                                evPost(nSprite, 3, gGameOptions.nMonsterRespawnTime, CALLBACK_ID_9);
                            else
                                DeleteSprite(nSprite);
                            break;
                        }
                        case 4:
                        {
                            int nWall = xwall[activeList[i].xindex].reference;
                            dassert(nWall >= 0 && nWall < kMaxWalls, 649);
                            wall[nWall].cstat &= ~(8+16+32);
                            //int nNextWall = wall[nWall].nextwall;
                            if (wall[nWall].nextwall != -1)
                                wall[wall[nWall].nextwall].cstat &= ~(8+16+32);
                            break;
                        }
                        }
                    }
                    activeList[i--] = activeList[--activeCount];
                    break;
                }
            }
            pInst->Update(&activeList[i]);
        }
    }
}

class SeqLoadSave : public LoadSave {
    virtual void Load(void);
    virtual void Save(void);
};

void SeqLoadSave::Load(void)
{
    int i, j;
    Read(&siWall, sizeof(siWall));
    Read(&siMasked, sizeof(siMasked));
    Read(&siCeiling, sizeof(siCeiling));
    Read(&siFloor, sizeof(siFloor));
    Read(&siSprite, sizeof(siSprite));
    Read(&activeList, sizeof(activeList));
    Read(&activeCount, sizeof(activeCount));
    for (i = 0; i < kMaxXWalls; i++)
    {
        siWall[i].hSeq = NULL;
        siMasked[i].hSeq = NULL;
        siWall[i].pSequence = NULL;
        siMasked[i].pSequence = NULL;
    }
    for (i = 0; i < kMaxXSectors; i++)
    {
        siCeiling[i].hSeq = NULL;
        siFloor[i].hSeq = NULL;
        siCeiling[i].pSequence = NULL;
        siFloor[i].pSequence = NULL;
    }
    for (i = 0; i < kMaxXSprites; i++)
    {
        siSprite[i].hSeq = NULL;
        siSprite[i].pSequence = NULL;
    }
    for (j = 0; j < activeCount; j++)
    {
        int type = activeList[j].type;
        int xindex = activeList[j].xindex;
        SEQINST *pInst = GetInstance(type, xindex);
        if (pInst->at13)
        {
            int nSeq = pInst->at8;
            DICTNODE *hSeq = gSysRes.Lookup(nSeq, "SEQ");
            if (!hSeq)
                ThrowError(735)("Missing sequence #%d", nSeq);
            Seq *pSeq = (Seq*)gSysRes.Lock(hSeq);
            if (memcmp(pSeq->signature, "SEQ\x1a", 4) != 0)
                ThrowError(740)("Invalid sequence %d", nSeq);
            if ((pSeq->version & 0xff00) != 0x300)
                ThrowError(743)("Sequence %d is obsolete version", nSeq);
            pInst->hSeq = hSeq;
            pInst->pSequence = pSeq;
        }
    }
}

void SeqLoadSave::Save(void)
{
    Write(&siWall, sizeof(siWall));
    Write(&siMasked, sizeof(siMasked));
    Write(&siCeiling, sizeof(siCeiling));
    Write(&siFloor, sizeof(siFloor));
    Write(&siSprite, sizeof(siSprite));
    Write(&activeList, sizeof(activeList));
    Write(&activeCount, sizeof(activeCount));
}

static SeqLoadSave myLoadSave;
