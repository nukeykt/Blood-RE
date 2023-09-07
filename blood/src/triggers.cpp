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
#include <string.h>
#include "typedefs.h"
#include "actor.h"
#include "ai.h"
#include "build.h"
#include "db.h"
#include "debug4g.h"
#include "dude.h"
#include "endgame.h"
#include "error.h"
#include "eventq.h"
#include "fx.h"
#include "gameutil.h"
#include "gib.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "misc.h"
#include "player.h"
#include "seq.h"
#include "sfx.h"
#include "sound.h"
#include "trig.h"
#include "triggers.h"
#include "view.h"

int basePath[kMaxSectors];

int gBusyCount = 0;

enum BUSYID {
    BUSYID_0 = 0,
    BUSYID_1,
    BUSYID_2,
    BUSYID_3,
    BUSYID_4,
    BUSYID_5,
    BUSYID_6,
    BUSYID_7,
};

struct BUSY {
    int at0;
    int at4;
    int at8;
    BUSYID atc;
};

BUSY gBusy[128];

static void FireballTrapSeqCallback(int, int);
static void MGunFireSeqCallback(int, int);
static void MGunOpenSeqCallback(int, int);

static int nFireballTrapClient = seqRegisterClient(FireballTrapSeqCallback);
static int nMGunFireClient = seqRegisterClient(MGunFireSeqCallback);
static int nMGunOpenClient = seqRegisterClient(MGunOpenSeqCallback);

static unsigned int GetWaveValue(unsigned int nPhase, int nType)
{
    switch (nType)
    {
    case 0:
        return 0x8000-(Cos((nPhase<<10)>>16)>>15);
    case 1:
        return nPhase;
    case 2:
        return 0x10000-(Cos((nPhase<<9)>>16)>>14);
    case 3:
        return Sin((nPhase<<9)>>16)>>14;
    }
    return nPhase;
}

static BOOL SetSpriteState(int nSprite, XSPRITE *pXSprite, int nState)
{
    if ((pXSprite->at1_7&0xffff) == 0 && pXSprite->at1_6 == nState)
        return 0;
    pXSprite->at1_7 = nState<<16;
    pXSprite->at1_6 = nState;
    evKill(nSprite, 3);
    if ((sprite[nSprite].flags & kSpriteFlag4) != 0 && sprite[nSprite].inittype >= kDudeBase && sprite[nSprite].inittype < kDudeMax)
    {
        pXSprite->atb_4 = 3;
        evPost(nSprite, 3, gGameOptions.nMonsterRespawnTime, CALLBACK_ID_9);
        return 1;
    }
    if (nState != pXSprite->atb_0 && pXSprite->at9_4 > 0)
        evPost(nSprite, 3, (pXSprite->at9_4*120) / 10, pXSprite->atb_0 ? COMMAND_ID_1 : COMMAND_ID_0);
    if (pXSprite->at4_0)
    {
        if (pXSprite->at6_4 != 5 && pXSprite->at7_4 && pXSprite->at1_6)
            evSend(nSprite, 3, pXSprite->at4_0, (COMMAND_ID)pXSprite->at6_4);
        if (pXSprite->at6_4 != 5 && pXSprite->at7_5 && !pXSprite->at1_6)
            evSend(nSprite, 3, pXSprite->at4_0, (COMMAND_ID)pXSprite->at6_4);
    }
    return 1;
}

static BOOL SetWallState(int nWall, XWALL *pXWall, int nState)
{
    if ((pXWall->at1_7&0xffff) == 0 && pXWall->at1_6 == nState)
        return 0;
    pXWall->at1_7 = nState<<16;
    pXWall->at1_6 = nState;
    evKill(nWall, 0);
    if (nState != pXWall->atd_4 && pXWall->atc_0 > 0)
        evPost(nWall, 0, (pXWall->atc_0*120) / 10, pXWall->atd_4 ? COMMAND_ID_1 : COMMAND_ID_0);
    if (pXWall->at6_0)
    {
        if (pXWall->at9_2 != 5 && pXWall->ata_2 && pXWall->at1_6)
            evSend(nWall, 0, pXWall->at6_0, (COMMAND_ID)pXWall->at9_2);
        if (pXWall->at9_2 != 5 && pXWall->ata_3 && !pXWall->at1_6)
            evSend(nWall, 0, pXWall->at6_0, (COMMAND_ID)pXWall->at9_2);
    }
    return 1;
}

static BOOL SetSectorState(int nSector, XSECTOR *pXSector, int nState)
{
    if ((pXSector->at1_7&0xffff) == 0 && pXSector->at1_6 == nState)
        return 0;
    pXSector->at1_7 = nState<<16;
    pXSector->at1_6 = nState;
    evKill(nSector, 6);
    if (nState == 1)
    {
        if (pXSector->at9_2 != 5 && pXSector->ata_2 && pXSector->at6_0)
            evSend(nSector, 6, pXSector->at6_0, (COMMAND_ID)pXSector->at9_2);
        if (pXSector->at1b_2)
        {
            pXSector->at1b_2 = 0;
            pXSector->at1b_3 = 0;
        }
        else if (pXSector->atf_6)
            evPost(nSector, 6, (pXSector->atc_0 * 120) / 10, COMMAND_ID_0);
    }
    else
    {
        if (pXSector->at9_2 != 5 && pXSector->ata_3 && pXSector->at6_0)
            evSend(nSector, 6, pXSector->at6_0, (COMMAND_ID)pXSector->at9_2);
        if (pXSector->at1b_3)
        {
            pXSector->at1b_2 = 0;
            pXSector->at1b_3 = 0;
        }
        else if (pXSector->atf_7)
            evPost(nSector, 6, (pXSector->at19_6 * 120) / 10, COMMAND_ID_1);
    }
    return 1;
}

static void AddBusy(int a1, BUSYID a2, int nDelta)
{
    dassert(nDelta != 0, 311);
    int i;
    for (i = 0; i < gBusyCount; i++)
    {
        if (gBusy[i].at0 == a1 && gBusy[i].atc == a2)
            break;
    }
    if (i == gBusyCount)
    {
        if (gBusyCount == 128)
            return;
        gBusy[i].at0 = a1;
        gBusy[i].atc = a2;
        gBusy[i].at8 = nDelta > 0 ? 0 : 65536;
        gBusyCount++;
    }
    gBusy[i].at4 = nDelta;
}

void ReverseBusy(int a1, BUSYID a2)
{
    int i;
    for (i = 0; i < gBusyCount; i++)
    {
        if (gBusy[i].at0 == a1 && gBusy[i].atc == a2)
        {
            gBusy[i].at4 = -gBusy[i].at4;
            break;
        }
    }
}

unsigned int GetSourceBusy(EVENT a1)
{
    int nIndex = a1.at0_0;
    switch (a1.at1_5)
    {
    case 6:
    {
        int nXIndex = sector[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXSectors, 378);
        return xsector[nXIndex].at1_7;
    }
    case 0:
    {
        int nXIndex = wall[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXWalls, 383);
        return xwall[nXIndex].at1_7;
    }
    case 3:
    {
        int nXIndex = sprite[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXSprites, 388);
        return xsprite[nXIndex].at1_7;
    }
    }
    return 0;
}

void func_43CF8(SPRITE *pSprite, XSPRITE *pXSprite, EVENT a3)
{
    switch (a3.at2_0)
    {
    case 30:
    {
        int nPlayer = pXSprite->at18_2;
        if (nPlayer >= 0 && nPlayer < gNetPlayers)
        {
            PLAYER *pPlayer = &gPlayer[nPlayer];
            if (pPlayer->pXSprite->health > 0)
            {
                pPlayer->at181[8] = ClipHigh(pPlayer->at181[8]+pXSprite->at14_0, gAmmoInfo[8].at0);
                pPlayer->atcb[9] = 1;
                if (pPlayer->atbd != 9)
                {
                    pPlayer->atc3 = 0;
                    pPlayer->atbe = 9;
                }
                evKill(pSprite->index, 3);
            }
        }
        break;
    }
    case 35:
    {
        int nTarget = pXSprite->target;
        if (nTarget >= 0 && nTarget < kMaxSprites)
        {
            if (!pXSprite->at32_0)
            {
                SPRITE *pTarget = &sprite[nTarget];
                if (pTarget->statnum == 6 && !(pTarget->flags&kSpriteFlag5) && pTarget->extra > 0 && pTarget->extra < kMaxXSprites)
                {
                    int top, bottom;
                    GetSpriteExtents(pSprite, &top, &bottom);
                    int nType = pTarget->type-kDudeBase;
                    DUDEINFO *pDudeInfo = &dudeInfo[nType];
                    int z1 = (top-pSprite->z)-256;
                    int x = pTarget->x;
                    int y = pTarget->y;
                    int z = pTarget->z;
                    int nDist = approxDist(x - pSprite->x, y - pSprite->y);
                    if (nDist != 0 && cansee(pSprite->x, pSprite->y, top, pSprite->sectnum, x, y, z, pTarget->sectnum))
                    {
                        int t = divscale(nDist, 0x1aaaaa, 12);
                        x += (xvel[nTarget]*t)>>12;
                        y += (yvel[nTarget]*t)>>12;
                        int angBak = pSprite->ang;
                        pSprite->ang = getangle(x-pSprite->x, y-pSprite->y);
                        int dx = Cos(pSprite->ang)>>16;
                        int dy = Sin(pSprite->ang)>>16;
                        int tz = pTarget->z - (pTarget->yrepeat * pDudeInfo->atf) * 4;
                        int dz = divscale(tz - top - 256, nDist, 10);
                        int nMissileType = 316+(pXSprite->at14_0 ? 1 : 0);
                        double t2;
                        if (!pXSprite->at14_0)
                            t2 = 120 / 10.0;
                        else
                            t2 = (3*120) / 10.0;
                        SPRITE *pMissile = actFireMissile(pSprite, 0, z1, dx, dy, dz, nMissileType);
                        if (pMissile)
                        {
                            pMissile->owner = pSprite->owner;
                            pXSprite->at32_0 = 1;
                            evPost(pSprite->index, 3, t2, CALLBACK_ID_20);
                            pXSprite->at14_0 = ClipLow(pXSprite->at14_0-1, 0);
                        }
                        pSprite->ang = angBak;
                    }
                }
            }
        }
        return;
    }
    }
    actPostSprite(pSprite->index, kStatFree);
}

static void ActivateGenerator(int);

void OperateSprite(int nSprite, XSPRITE *pXSprite, EVENT a3)
{
    SPRITE *pSprite = &sprite[nSprite];
    switch (a3.at2_0)
    {
    case 6:
        pXSprite->at17_5 = 1;
        return;
    case 7:
        pXSprite->at17_5 = 0;
        return;
    case 8:
        pXSprite->at17_5 = pXSprite->at17_5 ^ 1;
        return;
    }
    if (pSprite->statnum == 6 && pSprite->type >= kDudeBase && pSprite->type < kDudeMax)
    {
        switch (a3.at2_0)
        {
        case 0:
            SetSpriteState(nSprite, pXSprite, 0);
            break;
        case 35:
            if (pXSprite->at1_6)
                break;
        case 1:
        case 30:
        case 33:
            if (!pXSprite->at1_6)
                SetSpriteState(nSprite, pXSprite, 1);
            aiActivateDude(pSprite, pXSprite);
            break;
        }
        return;
    }
    switch (pSprite->type)
    {
    case 413:
        if (pXSprite->health > 0)
        {
            if (a3.at2_0 == 1)
            {
                if (SetSpriteState(nSprite, pXSprite, 1))
                {
                    seqSpawn(38, 3, pSprite->extra, nMGunOpenClient);
                    if (pXSprite->at10_0 > 0)
                        pXSprite->at12_0 = pXSprite->at10_0;
                }
            }
            else if (a3.at2_0 == 0)
            {
                if (SetSpriteState(nSprite, pXSprite, 0))
                    seqSpawn(40, 3, pSprite->extra);
            }
        }
        break;
    case 414:
        if (SetSpriteState(nSprite, pXSprite, 1))
            pSprite->flags |= 7;
        break;
    case 408:
        if (SetSpriteState(nSprite, pXSprite, 0))
            actPostSprite(nSprite, kStatFree);
        break;
    case 405:
        if (SetSpriteState(nSprite, pXSprite, 0))
            actPostSprite(nSprite, kStatFree);
        break;
    case 456:
        switch (a3.at2_0)
        {
        case 0:
            pXSprite->at1_6 = 0;
            pSprite->cstat |= 32768;
            pSprite->cstat &= ~1;
            break;
        case 1:
            pXSprite->at1_6 = 1;
            pSprite->cstat &= ~32768;
            pSprite->cstat |= 1;
            break;
        case 3:
            pXSprite->at1_6 ^= 1;
            pSprite->cstat ^= 32768;
            pSprite->cstat ^= 1;
            break;
        }
        break;
    case 452:
        if (a3.at2_0 == 1)
        {
            if (SetSpriteState(nSprite, pXSprite, 1))
            {
                seqSpawn(38, 3, pSprite->extra);
                sfxPlay3DSound(pSprite, 441, 0);
            }
        }
        else if (a3.at2_0 == 0)
        {
            if (SetSpriteState(nSprite, pXSprite, 0))
            {
                seqSpawn(40, 3, pSprite->extra);
                sfxKill3DSound(pSprite);
            }
        }
        break;
    case 23:
        switch (a3.at2_0)
        {
        case 0:
            SetSpriteState(nSprite, pXSprite, 0);
            break;
        case 1:
            if (SetSpriteState(nSprite, pXSprite, 1))
                seqSpawn(37, 3, pSprite->extra);
            break;
        default:
            SetSpriteState(nSprite, pXSprite, pXSprite->at1_6 ^ 1);
            if (pXSprite->at1_6)
                seqSpawn(37, 3, pSprite->extra);
            break;
        }
        break;
    case 20:
        switch (a3.at2_0)
        {
        case 0:
            if (SetSpriteState(nSprite, pXSprite, 0))
                sfxPlay3DSound(pSprite, pXSprite->at12_0, 0);
            break;
        case 1:
            if (SetSpriteState(nSprite, pXSprite, 1))
                sfxPlay3DSound(pSprite, pXSprite->at10_0, 0);
            break;
        default:
            if (SetSpriteState(nSprite, pXSprite, pXSprite->at1_6 ^ 1))
                sfxPlay3DSound(pSprite, pXSprite->at1_6 ? pXSprite->at10_0 : pXSprite->at12_0, 0);
            break;
        }
        break;
    case 21:
        switch (a3.at2_0)
        {
        case 0:
            if (SetSpriteState(nSprite, pXSprite, 0))
                sfxPlay3DSound(pSprite, pXSprite->at12_0, 0);
            break;
        case 1:
            if (SetSpriteState(nSprite, pXSprite, 1))
                sfxPlay3DSound(pSprite, pXSprite->at10_0, 0);
            break;
        default:
            if (SetSpriteState(nSprite, pXSprite, pXSprite->atb_0 ^ 1))
                sfxPlay3DSound(pSprite, pXSprite->at1_6 ? pXSprite->at10_0 : pXSprite->at12_0, 0);
            break;
        }
        break;
    case 22:
        switch (a3.at2_0)
        {
        case 0:
            if (--pXSprite->at10_0 < 0)
                pXSprite->at10_0 += pXSprite->at14_0;
            break;
        default:
            if (++pXSprite->at10_0 >= pXSprite->at14_0)
                pXSprite->at10_0 -= pXSprite->at14_0;
            break;
        }
        if (pXSprite->at6_4 == 5 && pXSprite->at4_0)
            evSend(nSprite, 3, pXSprite->at4_0, COMMAND_ID_5);
        sfxPlay3DSound(pSprite, pXSprite->at18_2, -1, 0);
        if (pXSprite->at10_0 == pXSprite->at12_0)
            SetSpriteState(nSprite, pXSprite, 1);
        else
            SetSpriteState(nSprite, pXSprite, 0);
        break;
    case 18:
        if (gGameOptions.nMonsterSettings && pXSprite->at10_0 >= kDudeBase && pXSprite->at10_0 < kDudeMax)
        {
            SPRITE *pSpawn = func_36878(pSprite, pXSprite->at10_0, -1, 0);
            if (pSpawn)
            {
                XSPRITE *pXSpawn = &xsprite[pSpawn->extra];
                gKillMgr.func_263E0(1);
                switch (pXSprite->at10_0)
                {
                case 240:
                case 242:
                case 241:
                case 252:
                case 253:
                case 239:
                {
                    pXSpawn->health = dudeInfo[pXSprite->at10_0 - kDudeBase].at2 << 4;
                    pXSpawn->at2c_0 = 10;
                    pXSpawn->target = -1;
                    aiActivateDude(pSpawn, pXSpawn);
                    break;
                }
                }
            }
        }
        break;
    case 19:
    {
        pXSprite->at7_4 = 0;
        pXSprite->atd_2 = 1;
        SetSpriteState(nSprite, pXSprite, 1);
        for (int p = connecthead; p >= 0; p = connectpoint2[p])
        {
            SPRITE *pPlayerSprite = gPlayer[p].pSprite;
            int dx = (pSprite->x - pPlayerSprite->x)>>4;
            int dy = (pSprite->y - pPlayerSprite->y)>>4;
            int dz = (pSprite->z - pPlayerSprite->z)>>8;
            int nDist = dx*dx+dy*dy+dz*dz+0x40000;
            gPlayer[p].at37f = divscale16(pXSprite->at10_0, nDist);
        }
        break;
    }
    case 400:
        if (pSprite->flags&kSpriteFlag4)
            return;
    case 418:
    case 419:
    case 420:
        actExplodeSprite(pSprite);
        break;
    case 459:
        switch (a3.at2_0)
        {
        case 1:
            SetSpriteState(nSprite, pXSprite, 1);
            break;
        default:
            pSprite->cstat &= ~32768;
            actExplodeSprite(pSprite);
            break;
        }
        break;
    case 402:
        if (pSprite->statnum == 8)
            break;
        switch (a3.at2_0)
        {
            case 1:
                sfxPlay3DSound(pSprite, 454, 0);
                evPost(nSprite, 3, 18, COMMAND_ID_0);
                break;
            default:
                actExplodeSprite(pSprite);
                break;
        }
        break;
    case 401:
        if (pSprite->statnum == 8)
            break;
        switch (a3.at2_0)
        {
        case 35:
            if (!pXSprite->at1_6)
            {
                sfxPlay3DSound(pSprite, 452, 0);
                evPost(nSprite, 3, 30, COMMAND_ID_0);
                pXSprite->at1_6 = 1;
            }
            break;
        case 1:
            sfxPlay3DSound(pSprite, 451, 0);
            pXSprite->ate_4 = 1;
            break;
        default:
            actExplodeSprite(pSprite);
            break;
        }
        break;
    case 431:
        func_43CF8(pSprite, pXSprite, a3);
        break;
    case 700:
    case 701:
    case 702:
    case 703:
    case 704:
    case 705:
    case 708:
    case 706:
    case 707:
        switch (a3.at2_0)
        {
        case 21:
            if (pSprite->type != 700)
                ActivateGenerator(nSprite);
            if (pXSprite->at4_0)
                evSend(nSprite, 3, pXSprite->at4_0, (COMMAND_ID)pXSprite->at6_4);
            if (pXSprite->at8_0 > 0)
                evPost(nSprite, 3, 120*(pXSprite->at8_0+Random2(pXSprite->at10_0)) / 10, COMMAND_ID_21);
            break;
        case 0:
            SetSpriteState(nSprite, pXSprite, 0);
            break;
        default:
            if (!pXSprite->at1_6)
            {
                SetSpriteState(nSprite, pXSprite, 1);
                evPost(nSprite, 3, 0, COMMAND_ID_21);
            }
            break;
        }
        break;
    case 711:
        if (gGameOptions.nGameType == GAMETYPE_0)
        {
            if (gMe->pXSprite->health <= 0)
                break;
            gMe->at30a = 0;
        }
        sndStartSample(pXSprite->at10_0, -1, 1);
        break;
    case 416:
    case 417:
    case 425:
    case 426:
    case 427:
        switch (a3.at2_0)
        {
        case 0:
            if (SetSpriteState(nSprite, pXSprite, 0))
                actActivateGibObject(pSprite, pXSprite);
            break;
        case 1:
            if (SetSpriteState(nSprite, pXSprite, 1))
                actActivateGibObject(pSprite, pXSprite);
            break;
        default:
            if (SetSpriteState(nSprite, pXSprite, pXSprite->at1_6 ^ 1))
                actActivateGibObject(pSprite, pXSprite);
            break;
        }
        break;
    default:
        switch (a3.at2_0)
        {
        case 0:
            SetSpriteState(nSprite, pXSprite, 0);
            break;
        case 1:
            SetSpriteState(nSprite, pXSprite, 1);
            break;
        default:
            SetSpriteState(nSprite, pXSprite, pXSprite->at1_6 ^ 1);
            break;
        }
        break;
    }
}

void SetupGibWallState(WALL *pWall, XWALL *pXWall)
{
    WALL *pWall2 = NULL;
    if (pWall->nextwall >= 0)
        pWall2 = &wall[pWall->nextwall];
    if (pXWall->at1_6)
    {
        pWall->cstat &= ~65;
        if (pWall2)
        {
            pWall2->cstat &= ~65;
            pWall->cstat &= ~16;
            pWall2->cstat &= ~16;
        }
        return;
    }
    BOOL bVector = pXWall->at10_6 ? 1 : 0;
    pWall->cstat |= 1;
    if (bVector)
        pWall->cstat |= 64;
    if (pWall2)
    {
        pWall2->cstat &= ~1;
        if (bVector)
            pWall2->cstat |= 64;
        pWall->cstat |= 16;
        pWall2->cstat |= 16;
    }
}

void OperateWall(int nWall, XWALL *pXWall, EVENT a3)
{
    WALL *pWall = &wall[nWall];
    switch (a3.at2_0)
    {
    case 6:
        pXWall->at13_2 = 1;
        return;
    case 7:
        pXWall->at13_2 = 0;
        return;
    case 8:
        pXWall->at13_2 ^= 1;
        return;
    }
    switch (pWall->type)
    {
        case 511:
        {
            BOOL bStatus;
            switch (a3.at2_0)
            {
            case 51:
                bStatus = SetWallState(nWall, pXWall, 1);
                break;
            case 0:
                bStatus = SetWallState(nWall, pXWall, 0);
                break;
            case 1:
                bStatus = SetWallState(nWall, pXWall, 1);
                break;
            default:
                bStatus = SetWallState(nWall, pXWall, pXWall->at1_6 ^ 1);
                break;
            }
            if (bStatus)
            {
                SetupGibWallState(pWall, pXWall);
                if (pXWall->at1_6)
                {
                    CGibVelocity vel(100, 100, 250);
                    int nType = ClipRange(pXWall->at4_0, 0, kGibMax);
                    if (nType > 0)
                        GibWall(nWall, (GIBTYPE)nType, &vel);
                }
            }
            break;
        }
        default:
            switch (a3.at2_0)
            {
            case 0:
                SetWallState(nWall, pXWall, 0);
                break;
            case 1:
                SetWallState(nWall, pXWall, 1);
                break;
            default:
                SetWallState(nWall, pXWall, pXWall->at1_6 ^ 1);
                break;
            }
            break;
    }
}

static void SectorStartSound(int nSector, int nState)
{
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 0 && pSprite->type == 709)
        {
            int nXSprite = pSprite->extra;
            dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 1173);
            XSPRITE *pXSprite = &xsprite[nXSprite];
            if (nState)
            {
                if (pXSprite->at14_0)
                    sfxPlay3DSound(pSprite, pXSprite->at14_0, 0);
            }
            else
            {
                if (pXSprite->at10_0)
                    sfxPlay3DSound(pSprite, pXSprite->at10_0, 0);
            }
        }
    }
}

static void SectorEndSound(int nSector, int nState)
{
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 0 && pSprite->type == 709)
        {
            int nXSprite = pSprite->extra;
            dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 1197);
            XSPRITE *pXSprite = &xsprite[nXSprite];
            if (nState)
            {
                if (pXSprite->at12_0)
                    sfxPlay3DSound(pSprite, pXSprite->at12_0, 0);
            }
            else
            {
                if (pXSprite->at18_2)
                    sfxPlay3DSound(pSprite, pXSprite->at18_2, 0);
            }
        }
    }
}

static void PathSound(int nSector, int nSound)
{
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 0 && pSprite->type == 709)
            sfxPlay3DSound(pSprite, nSound, 0);
    }
}

void DragPoint(int nWall, long x, long y)
{
    viewInterpolateWall(nWall, &wall[nWall]);
    wall[nWall].x = x;
    wall[nWall].y = y;

    int vsi = numwalls;
    int vb = nWall;
    do
    {
        if (wall[vb].nextwall >= 0)
        {
            vb = wall[wall[vb].nextwall].point2;
            viewInterpolateWall(vb, &wall[vb]);
            wall[vb].x = x;
            wall[vb].y = y;
        }
        else
        {
            vb = nWall;
            do
            {
                if (wall[lastwall(vb)].nextwall >= 0)
                {
                    vb = wall[lastwall(vb)].nextwall;
                    viewInterpolateWall(vb, &wall[vb]);
                    wall[vb].x = x;
                    wall[vb].y = y;
                }
                else
                    break;
                vsi--;
            } while (vb != nWall && vsi > 0);
            break;
        }
        vsi--;
    } while (vb != nWall && vsi > 0);
}

static void TranslateSector(int nSector, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, char a12)
{
    int nXSector = sector[nSector].extra;
    XSECTOR *pXSector = &xsector[nXSector];
    int v20 = interpolate16(a6, a9, a2);
    int vc = interpolate16(a6, a9, a3);
    int v28 = vc - v20;
    int v24 = interpolate16(a7, a10, a2);
    int v8 = interpolate16(a7, a10, a3);
    int v2c = v8 - v24;
    int v44 = interpolate16(a8, a11, a2);
    int vbp = interpolate16(a8, a11, a3);
    int v14 = vbp - v44;
    long x, y;
    int nWall = sector[nSector].wallptr;
    if (a12)
    {
        for (int i = 0; i < sector[nSector].wallnum; nWall++, i++)
        {
            x = baseWall[nWall].x;
            y = baseWall[nWall].y;
            if (vbp)
                RotatePoint(&x, &y, vbp, a4, a5);
            DragPoint(nWall, x+(vc-a4), y+(v8-a5));
        }
    }
    else
    {
        for (int i = 0; i < sector[nSector].wallnum; nWall++, i++)
        {
            int v10 = wall[nWall].point2;
            x = baseWall[nWall].x;
            y = baseWall[nWall].y;
            if (wall[nWall].cstat&kWallStat14)
            {
                if (vbp)
                    RotatePoint(&x, &y, vbp, a4, a5);
                DragPoint(nWall, x+(vc-a4), y+(v8-a5));
                if (!(wall[v10].cstat&kWallStat14_15))
                {
                    x = baseWall[v10].x;
                    y = baseWall[v10].y;
                    if (vbp)
                        RotatePoint(&x, &y, vbp, a4, a5);
                    DragPoint(v10, x+(vc-a4), y+(v8-a5));
                }
            }
            else if (wall[nWall].cstat&kWallStat15)
            {
                if (vbp)
                    RotatePoint(&x, &y, -vbp, a4, a5);
                DragPoint(nWall, x-(vc-a4), y-(v8-a5));
                if (!(wall[v10].cstat&kWallStat14_15))
                {
                    x = baseWall[v10].x;
                    y = baseWall[v10].y;
                    if (vbp)
                        RotatePoint(&x, &y, -vbp, a4, a5);
                    DragPoint(v10, x-(vc-a4), y-(v8-a5));
                }
            }
        }
    }
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 10 || pSprite->statnum == 16)
            continue;
        x = baseSprite[nSprite].x;
        y = baseSprite[nSprite].y;
        if (sprite[nSprite].cstat&kSpriteStat13)
        {
            if (vbp)
                RotatePoint(&x, &y, vbp, a4, a5);
            viewBackupSpriteLoc(nSprite, pSprite);
            pSprite->ang = (pSprite->ang+v14)&2047;
            pSprite->x = x+(vc-a4);
            pSprite->y = y+(v8-a5);
        }
        else if (sprite[nSprite].cstat&kSpriteStat14)
        {
            if (vbp)
                RotatePoint(&x, &y, -vbp, a4, a4);
            viewBackupSpriteLoc(nSprite, pSprite);
            pSprite->ang = (pSprite->ang-v14)&2047;
            pSprite->x = x-(vc-a4);
            pSprite->y = y-(v8-a5);
        }
        else if (pXSector->at13_3)
        {
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            int floorZ = getflorzofslope(nSector, pSprite->x, pSprite->y);
            if (!(pSprite->cstat&kSpriteMask) && bottom >= floorZ)
            {
                if (v14)
                    RotatePoint(&pSprite->x, &pSprite->y, v14, v20, v24);
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->ang = (pSprite->ang+v14)&2047;
                pSprite->x += v28;
                pSprite->y += v2c;
            }
        }
    }
}

static void ZTranslateSector(int nSector, XSECTOR *pXSector, int a3, int a4)
{
    SECTOR *pSector = &sector[nSector];
    viewInterpolateSector(nSector, pSector);
    int dz = pXSector->at28_0-pXSector->at24_0;
    if (dz != 0)
    {
        int oldZ = pSector->floorz;
        pSector->floorz = pXSector->at24_0 + mulscale16(dz, GetWaveValue(a3, a4));
        baseFloor[nSector] = pSector->floorz;
        velFloor[nSector] += (pSector->floorz-oldZ)<<8;
        for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite->statnum == 10 || pSprite->statnum == 16)
                continue;
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            if (pSprite->cstat&kSpriteStat13)
            {
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->z += pSector->floorz-oldZ;
            }
            else if (pSprite->flags&kSpriteFlag1)
                pSprite->flags |= kSpriteFlag2;
            else if (bottom >= oldZ && (pSprite->cstat&kSpriteMask) == kSpriteFace)
            {
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->z += pSector->floorz-oldZ;
            }
        }
    }
    int dz2 = pXSector->at20_0-pXSector->at1c_0;
    if (dz2 != 0)
    {
        int oldZ = pSector->ceilingz;
        pSector->ceilingz = pXSector->at1c_0 + mulscale16(dz2, GetWaveValue(a3, a4));
        baseCeil[nSector] = pSector->ceilingz;
        velCeil[nSector] += pSector->ceilingz-oldZ;
        for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite->statnum == 10 || pSprite->statnum == 16)
                continue;
            if (pSprite->cstat&kSpriteStat14)
            {
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->z += pSector->ceilingz-oldZ;
            }
        }
    }
}

int GetHighestSprite(int nSector, int nStatus, int *a3)
{
    *a3 = sector[nSector].floorz;
    int v8 = -1;
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        if (sprite[nSprite].statnum == nStatus || nStatus == 1024)
        {
            SPRITE *pSprite = &sprite[nSprite];
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            if (pSprite->z-top < *a3)
            {
                *a3 = pSprite->z-top;
                v8 = nSprite;
            }
        }
    }
    return v8;
}

int GetCrushedSpriteExtents(unsigned int nSector, int *pzTop, int *pzBot)
{
    dassert(pzTop != NULL && pzBot != NULL, 1530);
    dassert(nSector < numsectors, 1531);
    SECTOR *pSector = &sector[nSector];
    int vc = -1;
    int vbp = pSector->ceilingz;
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 6 || pSprite->statnum == 4)
        {
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            if (vbp > top)
            {
                vbp = top;
                *pzTop = top;
                *pzBot = bottom;
                vc = nSprite;
            }
        }
    }
    return vc;
}

static int VCrushBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1576);
    int nXSector = sector[nSector].extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1578);
    XSECTOR *pXSector = &xsector[nXSector];
    int nWave = (a2 > pXSector->at1_7) ? pXSector->at7_2 : pXSector->at7_5;
    int vc;
    int v10;
    int dz1 = pXSector->at20_0 - pXSector->at1c_0;
    if (dz1 != 0)
        vc = pXSector->at1c_0 + mulscale16(dz1, GetWaveValue(a2, nWave));
    int dz2 = pXSector->at28_0 - pXSector->at24_0;
    if (dz2 != 0)
        v10 = pXSector->at24_0 + mulscale16(dz2, GetWaveValue(a2, nWave));
    int v18;
    int nSprite = GetHighestSprite(nSector, 6, &v18);
    if (nSprite >= 0 && vc >= v18)
        return 1;
    viewInterpolateSector(nSector, &sector[nSector]);
    if (dz1 != 0)
        sector[nSector].ceilingz = vc;
    if (dz2 != 0)
        sector[nSector].floorz = v10;
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

static int VSpriteBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1637);
    int nXSector = sector[nSector].extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1639);
    XSECTOR *pXSector = &xsector[nXSector];
    int nWave = (a2 > pXSector->at1_7) ? pXSector->at7_2 : pXSector->at7_5;
    int dz1 = pXSector->at28_0 - pXSector->at24_0;
    if (dz1 != 0)
    {
        for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite->cstat&kSpriteStat13)
            {
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->z = baseSprite[nSprite].z+mulscale16(dz1, GetWaveValue(a2, nWave));
            }
        }
    }
    int dz2 = pXSector->at20_0 - pXSector->at1c_0;
    if (dz2 != 0)
    {
        for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite->cstat&kSpriteStat14)
            {
                viewBackupSpriteLoc(nSprite, pSprite);
                pSprite->z = baseSprite[nSprite].z+mulscale16(dz2, GetWaveValue(a2, nWave));
            }
        }
    }
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

inline int GetBusyDelta(XSECTOR* pXSector, int a2)
{
    if (a2)
        return 65536 / ClipLow((120 * pXSector->ata_4) / 10, 1);
    else
        return -65536 / ClipLow((120 * pXSector->at18_2) / 10, 1);
}

static int VDoorBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1707);
    int nXSector = sector[nSector].extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1709);
    XSECTOR *pXSector = &xsector[nXSector];
    int vbp = GetBusyDelta(pXSector, pXSector->at1_6);
    int top, bottom;
    int nSprite = GetCrushedSpriteExtents(nSector,&top,&bottom);
    if (nSprite >= 0 && a2 > pXSector->at1_7)
    {
        SPRITE *pSprite = &sprite[nSprite];
        dassert(pSprite->extra > 0 && pSprite->extra < kMaxXSprites, 1721);
        XSPRITE *pXSprite = &xsprite[pSprite->extra];
        if (pXSector->at20_0 > pXSector->at1c_0 || pXSector->at28_0 < pXSector->at24_0)
        {
            if (pXSector->atd_5)
            {
                if (pXSector->at30_0)
                {
                    if (pXSprite->health <= 0)
                        return 2;
                    int nDamage;
                    if (pXSector->at4_0 == 0)
                        nDamage = 500;
                    else
                        nDamage = pXSector->at4_0;
                    actDamageSprite(nSprite, &sprite[nSprite], DAMAGE_TYPE_0, nDamage<<4);
                }
                a2 = ClipRange(a2-(vbp/2)*4, 0, 65536);
            }
            else if (pXSector->at30_0 && pXSprite->health > 0)
            {
                int nDamage;
                if (pXSector->at4_0 == 0)
                    nDamage = 500;
                else
                    nDamage = pXSector->at4_0;
                actDamageSprite(nSprite, &sprite[nSprite], DAMAGE_TYPE_0, nDamage<<4);
                a2 = ClipRange(a2-(vbp/2)*4, 0, 65536);
            }
        }
    }
    else if (nSprite >= 0 && a2 < pXSector->at1_7)
    {
        SPRITE *pSprite = &sprite[nSprite];
        dassert(pSprite->extra > 0 && pSprite->extra < kMaxXSprites, 1753);
        XSPRITE *pXSprite = &xsprite[pSprite->extra];
        if (pXSector->at1c_0 > pXSector->at20_0 || pXSector->at24_0 < pXSector->at28_0)
        {
            if (pXSector->atd_5)
            {
                if (pXSector->at30_0)
                {
                    if (pXSprite->health <= 0)
                        return 2;
                    int nDamage;
                    if (pXSector->at4_0 == 0)
                        nDamage = 500;
                    else
                        nDamage = pXSector->at4_0;
                    actDamageSprite(nSprite, &sprite[nSprite], DAMAGE_TYPE_0, nDamage<<4);
                }
                a2 = ClipRange(a2+(vbp/2)*4, 0, 65536);
            }
            else if (pXSector->at30_0 && pXSprite->health > 0)
            {
                int nDamage;
                if (pXSector->at4_0 == 0)
                    nDamage = 500;
                else
                    nDamage = pXSector->at4_0;
                actDamageSprite(nSprite, &sprite[nSprite], DAMAGE_TYPE_0, nDamage<<4);
                a2 = ClipRange(a2+(vbp/2)*4, 0, 65536);
            }
        }
    }
    int nWave = (a2 > pXSector->at1_7) ? pXSector->at7_2 : pXSector->at7_5;
    ZTranslateSector(nSector, pXSector, a2, nWave);
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

static int HDoorBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1815);
    SECTOR *pSector = &sector[nSector];
    int nXSector = pSector->extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1816);
    XSECTOR *pXSector = &xsector[nXSector];
    int nWave = (a2 > pXSector->at1_7) ? pXSector->at7_2 : pXSector->at7_5;
    SPRITE *pSprite1 = &sprite[pXSector->at2c_0];
    SPRITE *pSprite2 = &sprite[pXSector->at2e_0];
    TranslateSector(nSector, GetWaveValue(pXSector->at1_7, nWave), GetWaveValue(a2, nWave), pSprite1->x, pSprite1->y, pSprite1->x, pSprite1->y, pSprite1->ang, pSprite2->x, pSprite2->y, pSprite2->ang, pSector->type == 616);
    ZTranslateSector(nSector, pXSector, a2, nWave);
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

static int RDoorBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1863);
    SECTOR *pSector = &sector[nSector];
    int nXSector = pSector->extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1866);
    XSECTOR *pXSector = &xsector[nXSector];
    int nWave = (a2 > pXSector->at1_7) ? pXSector->at7_2 : pXSector->at7_5;
    SPRITE *pSprite = &sprite[pXSector->at2c_0];
    TranslateSector(nSector, GetWaveValue(pXSector->at1_7, nWave), GetWaveValue(a2, nWave), pSprite->x, pSprite->y, pSprite->x, pSprite->y, 0, pSprite->x, pSprite->y, pSprite->ang, pSector->type == 617);
    ZTranslateSector(nSector, pXSector, a2, nWave);
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

static int StepRotateBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1904);
    SECTOR *pSector = &sector[nSector];
    int nXSector = pSector->extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1907);
    XSECTOR *pXSector = &xsector[nXSector];
    SPRITE *pSprite = &sprite[pXSector->at2c_0];
    int vbp;
    if (pXSector->at1_7 < a2)
    {
        vbp = pXSector->at4_0+pSprite->ang;
        int nWave = pXSector->at7_2;
        TranslateSector(nSector, GetWaveValue(pXSector->at1_7, nWave), GetWaveValue(a2, nWave), pSprite->x, pSprite->y, pSprite->x, pSprite->y, pXSector->at4_0, pSprite->x, pSprite->y, vbp, 1);
    }
    else
    {
        vbp = pXSector->at4_0-pSprite->ang;
        int nWave = pXSector->at7_5;
        TranslateSector(nSector, GetWaveValue(pXSector->at1_7, nWave), GetWaveValue(a2, nWave), pSprite->x, pSprite->y, pSprite->x, pSprite->y, vbp, pSprite->x, pSprite->y, pXSector->at4_0, 1);
    }
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        pXSector->at4_0 = vbp&2047;
        return 3;
    }
    return 0;
}

static int GenSectorBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1951);
    SECTOR *pSector = &sector[nSector];
    int nXSector = pSector->extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1954);
    XSECTOR *pXSector = &xsector[nXSector];
    pXSector->at1_7 = a2;
    if (pXSector->at9_2 == 5 && pXSector->at6_0)
        evSend(nSector, 6, pXSector->at6_0, COMMAND_ID_5);
    if ((a2&0xffff) == 0)
    {
        SetSectorState(nSector, pXSector, a2>>16);
        SectorEndSound(nSector, a2>>16);
        return 3;
    }
    return 0;
}

static int PathBusy(unsigned int nSector, unsigned int a2)
{
    dassert(nSector < numsectors, 1981);
    SECTOR *pSector = &sector[nSector];
    int nXSector = pSector->extra;
    dassert(nXSector > 0 && nXSector < kMaxXSectors, 1984);
    XSECTOR *pXSector = &xsector[nXSector];
    SPRITE *pSprite = &sprite[basePath[nSector]];
    SPRITE *pSprite1 = &sprite[pXSector->at2c_0];
    XSPRITE *pXSprite1 = &xsprite[pSprite1->extra];
    SPRITE *pSprite2 = &sprite[pXSector->at2e_0];
    XSPRITE *pXSprite2 = &xsprite[pSprite2->extra];
    int nWave = pXSprite1->at7_6;
    TranslateSector(nSector, GetWaveValue(pXSector->at1_7, nWave), GetWaveValue(a2, nWave), pSprite->x, pSprite->y, pSprite1->x, pSprite1->y, pSprite1->ang, pSprite2->x, pSprite2->y, pSprite2->ang, 1);
    ZTranslateSector(nSector, pXSector, a2, nWave);
    pXSector->at1_7 = a2;
    if ((a2&0xffff) == 0)
    {
        evPost(nSector, 6, (120*pXSprite2->at9_4)/10, COMMAND_ID_1);
        pXSector->at1_6 = 0;
        pXSector->at1_7 = 0;
        if (pXSprite1->at18_2)
            PathSound(nSector, pXSprite1->at18_2);
        pXSector->at2c_0 = pXSector->at2e_0;
        pXSector->at4_0 = pXSprite2->at10_0;
        return 3;
    }
    return 0;
}

int(*gBusyProc[])(unsigned int, unsigned int) =
{
    VCrushBusy,
    VSpriteBusy,
    VDoorBusy,
    HDoorBusy,
    RDoorBusy,
    StepRotateBusy,
    GenSectorBusy,
    PathBusy
};

void OperateDoor(unsigned int nSector, XSECTOR *pXSector, EVENT a3, BUSYID a4) 
{
    switch (a3.at2_0)
    {
    case 0:
        if (pXSector->at1_7)
        {
            AddBusy(nSector, a4, GetBusyDelta(pXSector, 0));
            SectorStartSound(nSector, 1);
        }
        break;
    case 1:
        if (pXSector->at1_7 != 0x10000)
        {
            AddBusy(nSector, a4, GetBusyDelta(pXSector, 1));
            SectorStartSound(nSector, 0);
        }
        break;
    default:
        if (pXSector->at1_7&0xffff)
        {
            if (pXSector->atd_5)
            {
                ReverseBusy(nSector, a4);
                pXSector->at1_6 = !pXSector->at1_6;
            }
        }
        else
        {
            AddBusy(nSector, a4, GetBusyDelta(pXSector, !pXSector->at1_6));
            SectorStartSound(nSector, pXSector->at1_6);
        }
        break;
    }
}

BOOL SectorContainsDudes(int nSector)
{
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        if (sprite[nSprite].statnum == 6)
            return 1;
    }
    return 0;
}

void TeleFrag(int nKiller, int nSector)
{
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 6)
            actDamageSprite(nKiller, pSprite, DAMAGE_TYPE_3, 4000);
        else if (pSprite->statnum == 4)
            actDamageSprite(nKiller, pSprite, DAMAGE_TYPE_3, 4000);
    }
}

void OperateTeleport(unsigned int nSector, XSECTOR *pXSector)
{
    dassert(nSector < numsectors, 2132);
    int nDest = pXSector->at2c_0;
    dassert(nDest < kMaxSprites, 2134);
    SPRITE *pDest = &sprite[nDest];
    dassert(pDest->statnum == kStatMarker, 2137);
    dassert(pDest->type == kMarkerWarpDest, 2138);
    dassert(pDest->sectnum >= 0 && pDest->sectnum < kMaxSectors, 2139);
    for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->statnum == 6)
        {
            PLAYER *pPlayer = IsPlayerSprite(pSprite) ? &gPlayer[pSprite->type - kDudePlayer1] : NULL;
            if (pPlayer || !SectorContainsDudes(pDest->sectnum))
            {
                if (!(gGameOptions.uNetGameFlags&2))
                    TeleFrag(pXSector->at4_0, pDest->sectnum);
                pSprite->x = pDest->x;
                pSprite->y = pDest->y;
                pSprite->z += sector[pDest->sectnum].floorz-sector[nSector].floorz;
                pSprite->ang = pDest->ang;
                ChangeSpriteSect(nSprite, pDest->sectnum);
                sfxPlay3DSound(pDest, 201, -1, 0);
                xvel[nSprite] = yvel[nSprite] = zvel[nSprite] = 0;
                ClearBitString(gInterpolateSprite, nSprite);
                viewBackupSpriteLoc(nSprite, pSprite);
                if (pPlayer)
                {
                    playerResetInertia(pPlayer);
                    pPlayer->at6b = pPlayer->at73 = 0;
                }
            }
        }
    }
}

void OperatePath(unsigned int nSector, XSECTOR *pXSector, EVENT a3)
{
    dassert(nSector < numsectors, 2200);
    SPRITE *pSprite2 = &sprite[pXSector->at2c_0];
    XSPRITE *pXSprite2 = &xsprite[pSprite2->extra];
    SPRITE *pSprite;
    XSPRITE *pXSprite;
    int nId = pXSprite2->at12_0;
    for (int nSprite = headspritestat[16]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        pSprite = &sprite[nSprite];
        if (pSprite->type == 15)
        {
            pXSprite = &xsprite[pSprite->extra];
            if (pXSprite->at10_0 == nId)
                break;
        }
    }
    if (nSprite < 0)
        ThrowError(2228)("Unable to find path marker with id #%d", nId);
    pXSector->at2e_0 = nSprite;
    pXSector->at24_0 = pSprite2->z;
    pXSector->at28_0 = pSprite->z;
    switch (a3.at2_0)
    {
    case 1:
        pXSector->at1_6 = 0;
        pXSector->at1_7 = 0;
        AddBusy(nSector, BUSYID_7, 65536/ClipLow((120*pXSprite2->at8_0)/10,1));
        if (pXSprite2->at14_0)
            PathSound(nSector, pXSprite2->at14_0);
        break;
    }
}

void OperateSector(unsigned int nSector, XSECTOR *pXSector, EVENT a3)
{
    dassert(nSector < numsectors, 2264);
    SECTOR *pSector = &sector[nSector];
    switch (a3.at2_0)
    {
    case 6:
        pXSector->at35_0 = 1;
        break;
    case 7:
        pXSector->at35_0 = 0;
        break;
    case 8:
        pXSector->at35_0 ^= 1;
        break;
    case 9:
        pXSector->at1b_2 = 0;
        pXSector->at1b_3 = 1;
        break;
    case 10:
        pXSector->at1b_2 = 1;
        pXSector->at1b_3 = 0;
        break;
    case 11:
        pXSector->at1b_2 = 1;
        pXSector->at1b_3 = 1;
        break;
    default:
        switch (pSector->type)
        {
        case 602:
            OperateDoor(nSector, pXSector, a3, BUSYID_1);
            break;
        case 600:
            OperateDoor(nSector, pXSector, a3, BUSYID_2);
            break;
        case 614:
        case 616:
            OperateDoor(nSector, pXSector, a3, BUSYID_3);
            break;
        case 615:
        case 617:
            OperateDoor(nSector, pXSector, a3, BUSYID_4);
            break;
        case 613:
            switch (a3.at2_0)
            {
            case 1:
                pXSector->at1_6 = 0;
                pXSector->at1_7 = 0;
                AddBusy(nSector, BUSYID_5, GetBusyDelta(pXSector, 1));
                SectorStartSound(nSector, 0);
                break;
            case 0:
                pXSector->at1_6 = 1;
                pXSector->at1_7 = 65536;
                AddBusy(nSector, BUSYID_5, GetBusyDelta(pXSector, 0));
                SectorStartSound(nSector, 1);
                break;
            }
            break;
        case 604:
            OperateTeleport(nSector, pXSector);
            break;
        case 612:
            OperatePath(nSector, pXSector, a3);
            break;
        default:
            if (pXSector->ata_4 || pXSector->at18_2)
                OperateDoor(nSector, pXSector, a3, BUSYID_6);
            else
            {
                switch (a3.at2_0)
                {
                case 0:
                    SetSectorState(nSector, pXSector, 0);
                    break;
                case 1:
                    SetSectorState(nSector, pXSector, 1);
                    break;
                default:
                    SetSectorState(nSector, pXSector, pXSector->at1_6^1);
                    break;
                }
            }
            break;
        }
        break;
    }
}

void InitPath(unsigned int nSector, XSECTOR *pXSector)
{
    dassert(nSector < numsectors, 2383);
    SPRITE *pSprite;
    XSPRITE *pXSprite;
    int nId = pXSector->at4_0;
    for (int nSprite = headspritestat[16]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        pSprite = &sprite[nSprite];
        if (pSprite->type == 15)
        {
            pXSprite = &xsprite[pSprite->extra];
            if (pXSprite->at10_0 == nId)
                break;
        }
    }
    if (nSprite < 0)
        ThrowError(2409)("Unable to find path marker with id #%d", nId);
    pXSector->at2c_0 = nSprite;
    basePath[nSector] = nSprite;
    if (pXSector->at1_6)
        evPost(nSector, 6, 0, COMMAND_ID_1);
}

void LinkSector(int nSector, XSECTOR *pXSector, EVENT a3)
{
    SECTOR *pSector = &sector[nSector];
    unsigned int nBusy = GetSourceBusy(a3);
    switch (pSector->type)
    {
    case 602:
        VSpriteBusy(nSector, nBusy);
        break;
    case 600:
        VDoorBusy(nSector, nBusy);
        break;
    case 614:
    case 616:
        HDoorBusy(nSector, nBusy);
        break;
    case 615:
    case 617:
        RDoorBusy(nSector, nBusy);
        break;
    default:
        pXSector->at1_7 = nBusy;
        if ((nBusy&0xffff) == 0)
            SetSectorState(nSector, pXSector, nBusy>>16);
        break;
    }
}

void LinkSprite(int nSprite, XSPRITE *pXSprite, EVENT a3)
{
    SPRITE *pSprite = &sprite[nSprite];
    unsigned int nBusy = GetSourceBusy(a3);
    switch (pSprite->type)
    {
        case 22:
            if (a3.at1_5 == 3)
            {
                int nSprite2 = a3.at0_0;
                int nXSprite2 = sprite[nSprite2].extra;
                dassert(nXSprite2 > 0 && nXSprite2 < kMaxXSprites, 2490);
                pXSprite->at10_0 = xsprite[nXSprite2].at10_0;
                if (pXSprite->at10_0 == pXSprite->at12_0)
                    SetSpriteState(nSprite, pXSprite, 1);
                else
                    SetSpriteState(nSprite, pXSprite, 0);
            }
            break;
        default:
            pXSprite->at1_7 = nBusy;
            if ((nBusy&0xffff) == 0)
                SetSpriteState(nSprite, pXSprite, nBusy>>16);
            break;
    }
}

void LinkWall(int nWall, XWALL *pXWall, EVENT a3)
{
    unsigned int nBusy = GetSourceBusy(a3);
    pXWall->at1_7 = nBusy;
    if ((nBusy & 0xffff) == 0)
        SetWallState(nWall, pXWall, nBusy>>16);
}

void trTriggerSector(unsigned int nSector, XSECTOR *pXSector, int a3)
{
    dassert(nSector < numsectors, 2561);
    if (!pXSector->at35_0 && !pXSector->at16_6)
    {
        if (pXSector->at16_5)
            pXSector->at16_6 = 1;
        if (pXSector->at16_4)
        {
            if (pXSector->at6_0)
                evSend(nSector, 6, pXSector->at6_0, (COMMAND_ID)pXSector->at9_2);
        }
        else
        {
            EVENT evnt;
            evnt.at2_0 = a3;
            OperateSector(nSector, pXSector, evnt);
        }
    }
}

void trMessageSector(unsigned int nSector, EVENT a2)
{
    dassert(nSector < numsectors, 2603);
    dassert(sector[nSector].extra > 0 && sector[nSector].extra < kMaxXSectors, 2604);
    int nXSector = sector[nSector].extra;
    XSECTOR *pXSector = &xsector[nXSector];
    if (!pXSector->at35_0 || a2.at2_0 == 7 || a2.at2_0 == 8)
    {
        if (a2.at2_0 == 5)
            LinkSector(nSector, pXSector, a2);
        else
            OperateSector(nSector, pXSector, a2);
    }
}

void trTriggerWall(unsigned int nWall, XWALL *pXWall, int a3)
{
    dassert(nWall < numwalls, 2635);
    if (!pXWall->at13_2 && !pXWall->at10_1)
    {
        if (pXWall->at10_0)
            pXWall->at10_1 = 1;
        if (pXWall->atf_7)
        {
            if (pXWall->at6_0)
                evSend(nWall, 0, pXWall->at6_0, (COMMAND_ID)pXWall->at9_2);
        }
        else
        {
            EVENT evnt;
            evnt.at2_0 = a3;
            OperateWall(nWall, pXWall, evnt);
        }
    }
}

void trMessageWall(unsigned int nWall, EVENT a2)
{
    dassert(nWall < numwalls, 2677);
    dassert(wall[nWall].extra > 0 && wall[nWall].extra < kMaxXWalls, 2678);
    int nXWall = wall[nWall].extra;
    XWALL *pXWall = &xwall[nXWall];
    if (!pXWall->at13_2 || a2.at2_0 == 7 || a2.at2_0 == 8)
    {
        if (a2.at2_0 == 5)
            LinkWall(nWall, pXWall, a2);
        else
            OperateWall(nWall, pXWall, a2);
    }
}

void trTriggerSprite(unsigned int nSprite, XSPRITE *pXSprite, int a3)
{
    if (!pXSprite->at17_5 && !pXSprite->atd_2)
    {
        if (pXSprite->atd_1)
            pXSprite->atd_2 = 1;
        if (pXSprite->atd_0)
        {
            if (pXSprite->at4_0)
                evSend(nSprite, 3, pXSprite->at4_0, (COMMAND_ID)pXSprite->at6_4);
        }
        else
        {
            EVENT evnt;
            evnt.at2_0 = a3;
            OperateSprite(nSprite, pXSprite, evnt);
        }
    }
}

void trMessageSprite(unsigned int nSprite, EVENT a2)
{
    if (sprite[nSprite].statnum == kStatFree)
        return;
    SPRITE *pSprite = &sprite[nSprite];
    if (pSprite->extra <= 0 || pSprite->extra >= kMaxXSprites)
        return;
    int nXSprite = sprite[nSprite].extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    if (!pXSprite->at17_5 || a2.at2_0 == 7 || a2.at2_0 == 8)
    {
        if (a2.at2_0 == 5)
            LinkSprite(nSprite, pXSprite, a2);
        else
            OperateSprite(nSprite, pXSprite, a2);
    }
}

void ProcessMotion(void)
{
    SECTOR *pSector;
    int nSector;
    for (pSector = sector, nSector = 0; nSector < numsectors; nSector++, pSector++)
    {
        int nXSector = pSector->extra;
        if (nXSector <= 0)
            continue;
        XSECTOR *pXSector = &xsector[nXSector];
        if (pXSector->at3a_0 != 0)
        {
            if (pXSector->at3b_4)
                pXSector->at38_0 += pXSector->at3a_0;
            else if (pXSector->at1_7 == 0)
                continue;
            else
                pXSector->at38_0 += mulscale16(pXSector->at3a_0, pXSector->at1_7);
            int vdi = mulscale30(Sin(pXSector->at38_0), pXSector->at39_3<<8);
            for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
            {
                SPRITE *pSprite = &sprite[nSprite];
                if (pSprite->cstat&(kSpriteStat13|kSpriteStat14))
                {
                    viewBackupSpriteLoc(nSprite, pSprite);
                    pSprite->z += vdi;
                }
            }
            if (pXSector->at3b_5)
            {
                int floorZ = pSector->floorz;
                viewInterpolateSector(nSector, pSector);
                pSector->floorz = baseFloor[nSector]+vdi;
                for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
                {
                    SPRITE *pSprite = &sprite[nSprite];
                    if (pSprite->flags&kSpriteFlag1)
                        pSprite->flags |= kSpriteFlag2;
                    else
                    {
                        int top, bottom;
                        GetSpriteExtents(pSprite, &top, &bottom);
                        if (bottom >= floorZ && (pSprite->cstat&kSpriteMask) == kSpriteFace)
                        {
                            viewBackupSpriteLoc(nSprite, pSprite);
                            pSprite->z += vdi;
                        }
                    }
                }
            }
            if (pXSector->at3b_6)
            {
                int ceilZ = pSector->ceilingz;
                viewInterpolateSector(nSector, pSector);
                pSector->ceilingz = baseCeil[nSector]+vdi;
                for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
                {
                    SPRITE *pSprite = &sprite[nSprite];
                    int top, bottom;
                    GetSpriteExtents(pSprite, &top, &bottom);
                    if (top <= ceilZ && (pSprite->cstat&kSpriteMask) == kSpriteFace)
                    {
                        viewBackupSpriteLoc(nSprite, pSprite);
                        pSprite->z += vdi;
                    }
                }
            }
        }
    }
}

void AlignSlopes(void)
{
    SECTOR *pSector;
    int nSector;
    for (pSector = sector, nSector = 0; nSector < numsectors; nSector++, pSector++)
    {
        if (pSector->align)
        {
            WALL *pWall = &wall[pSector->wallptr+pSector->align];
            WALL *pWall2 = &wall[pWall->point2];
            int nNextSector = pWall->nextsector;
            if (nNextSector >= 0)
            {
                int x = (pWall->x+pWall2->x)/2;
                int y = (pWall->y+pWall2->y)/2;
                viewInterpolateSector(nSector, pSector);
                alignflorslope(nSector, x, y, getflorzofslope(nNextSector, x, y));
                alignceilslope(nSector, x, y, getceilzofslope(nNextSector, x, y));
            }
        }
    }
}

void trProcessBusy(void)
{
    memset(velFloor, 0, sizeof(velFloor));
    memset(velCeil, 0, sizeof(velCeil));
    for (int i = gBusyCount-1; i >= 0; i--)
    {
        int oldBusy = gBusy[i].at8;
        gBusy[i].at8 = ClipRange(gBusy[i].at8+gBusy[i].at4*4, 0, 65536);
        int nStatus = gBusyProc[gBusy[i].atc](gBusy[i].at0, gBusy[i].at8);
        switch (nStatus)
        {
        case 1:
            gBusy[i].at8 = oldBusy;
            break;
        case 2:
            gBusy[i].at8 = oldBusy;
            gBusy[i].at4 = -gBusy[i].at4;
            break;
        case 3:
            gBusyCount--;
            gBusy[i] = gBusy[gBusyCount];
            break;
        }
    }
    ProcessMotion();
    AlignSlopes();
}

static void InitGenerator(int);

void trInit(void)
{
    int j;
    int nSector, nWall, nSprite;
    gBusyCount = 0;
    for (nWall = 0; nWall < numwalls; nWall++)
    {
        baseWall[nWall].x = wall[nWall].x;
        baseWall[nWall].y = wall[nWall].y;
    }
    for (nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kStatFree)
        {
            sprite[nSprite].inittype = sprite[nSprite].type;
            baseSprite[nSprite].x = sprite[nSprite].x;
            baseSprite[nSprite].y = sprite[nSprite].y;
            baseSprite[nSprite].z = sprite[nSprite].z;
        }
        else
            sprite[nSprite].inittype = -1;
    }
    for (nWall = 0; nWall < numwalls; nWall++)
    {
        if (wall[nWall].extra > 0)
        {
            int nXWall = wall[nWall].extra;
            dassert(nXWall < kMaxXWalls, 3007);
            XWALL *pXWall = &xwall[nXWall];
            if (pXWall->at1_6)
                pXWall->at1_7 = 65536;
        }
    }
    dassert((numsectors >= 0) && (numsectors < kMaxSectors), 3023);
    for (nSector = 0; nSector < numsectors; nSector++)
    {
        SECTOR *pSector = &sector[nSector];
        baseFloor[nSector] = pSector->floorz;
        baseCeil[nSector] = pSector->ceilingz;
        int nXSector = pSector->extra;
        if (nXSector > 0)
        {
            dassert(nXSector < kMaxXSectors, 3034);
            XSECTOR *pXSector = &xsector[nXSector];
            if (pXSector->at1_6)
                pXSector->at1_7 = 65536;
            SPRITE *pSprite1 = NULL;
            SPRITE *pSprite2 = NULL;
            switch (pSector->type)
            {
            case 619:
                pXSector->at16_5 = 1;
                evPost(nSector, 6, 0, CALLBACK_ID_12);
                break;
            case 600:
            case 602:
                ZTranslateSector(nSector, pXSector, pXSector->at1_7, 1);
                break;
            case 614:
            case 616:
                pSprite1 = &sprite[pXSector->at2c_0];
                pSprite2 = &sprite[pXSector->at2e_0];
                TranslateSector(nSector, 0, -65536, pSprite1->x, pSprite1->y, pSprite1->x, pSprite1->y, pSprite1->ang, pSprite2->x, pSprite2->y, pSprite2->ang, pSector->type == 616);
                for (j = 0; j < pSector->wallnum; j++)
                {
                    int nWall = pSector->wallptr+j;
                    baseWall[nWall].x = wall[nWall].x;
                    baseWall[nWall].y = wall[nWall].y;
                }
                for (nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
                {
                    baseSprite[nSprite].x = sprite[nSprite].x;
                    baseSprite[nSprite].y = sprite[nSprite].y;
                    baseSprite[nSprite].z = sprite[nSprite].z;
                }
                TranslateSector(nSector, 0, pXSector->at1_7, pSprite1->x, pSprite1->y, pSprite1->x, pSprite1->y, pSprite1->ang, pSprite2->x, pSprite2->y, pSprite2->ang, pSector->type == 616);
                ZTranslateSector(nSector, pXSector, pXSector->at1_7, 1);
                break;
            case 615:
            case 617:
                pSprite1 = &sprite[pXSector->at2c_0];
                TranslateSector(nSector, 0, -65536, pSprite1->x, pSprite1->y, pSprite1->x, pSprite1->y, 0, pSprite1->x, pSprite1->y, pSprite1->ang, pSector->type == 617);
                for (j = 0; j < pSector->wallnum; j++)
                {
                    int nWall = pSector->wallptr+j;
                    baseWall[nWall].x = wall[nWall].x;
                    baseWall[nWall].y = wall[nWall].y;
                }
                for (nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
                {
                    baseSprite[nSprite].x = sprite[nSprite].x;
                    baseSprite[nSprite].y = sprite[nSprite].y;
                    baseSprite[nSprite].z = sprite[nSprite].z;
                }
                TranslateSector(nSector, 0, pXSector->at1_7, pSprite1->x, pSprite1->y, pSprite1->x, pSprite1->y, 0, pSprite1->x, pSprite1->y, pSprite1->ang, pSector->type == 617);
                ZTranslateSector(nSector, pXSector, pXSector->at1_7, 1);
                break;
            case 612:
                InitPath(nSector, pXSector);
                break;
            default:
                break;
            }
        }
    }
    for (nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        int nXSprite = sprite[nSprite].extra;
        if (sprite[nSprite].statnum < kStatFree && nXSprite > 0)
        {
            dassert(nXSprite < kMaxXSprites, 3146);
            XSPRITE *pXSprite = &xsprite[nXSprite];
            if (pXSprite->at1_6)
                pXSprite->at1_7 = 65536;
            switch (sprite[nSprite].type)
            {
            case 23:
                pXSprite->atd_1 = 1;
                break;
            case 700:
            case 701:
            case 702:
            case 703:
            case 704:
            case 705:
            case 708:
            case 706:
            case 707:
                InitGenerator(nSprite);
                break;
            case 401:
                pXSprite->ate_4 = 1;
                break;
            case 414:
                if (pXSprite->at1_6)
                    sprite[nSprite].flags |= 7;
                else
                    sprite[nSprite].flags &= ~7;
                break;
            }
            if (pXSprite->atd_7)
                sprite[nSprite].cstat |= 256;
            if (pXSprite->atd_6)
                sprite[nSprite].cstat |= 4096;
        }
    }
    evSend(0, 0, 7, COMMAND_ID_1);
    if (gGameOptions.nGameType == GAMETYPE_1)
        evSend(0, 0, 9, COMMAND_ID_1);
    else if (gGameOptions.nGameType == GAMETYPE_2)
        evSend(0, 0, 8, COMMAND_ID_1);
    else if (gGameOptions.nGameType == GAMETYPE_3)
    {
        evSend(0, 0, 8, COMMAND_ID_1);
        evSend(0, 0, 10, COMMAND_ID_1);
    }
}

void trTextOver(int nId)
{
    char *pzMessage = levelGetMessage(nId);
    if (pzMessage)
        viewSetMessage(pzMessage);
}

static void InitGenerator(int nSprite)
{
    dassert(nSprite < kMaxSprites, 3276);
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pSprite->statnum != kMaxStatus, 3278);
    int nXSprite = pSprite->extra;
    dassert(nXSprite > 0, 3281);
    XSPRITE *pXSprite = &xsprite[nXSprite];
    switch (sprite[nSprite].type)
    {
    case 700:
        pSprite->cstat &= ~(32768+1);
        pSprite->cstat |= 32768;
        break;
    }
    if (pXSprite->at1_6 != pXSprite->atb_0 && pXSprite->at8_0 > 0)
        evPost(nSprite, 3, (120*(pXSprite->at8_0+Random2(pXSprite->at10_0)))/10, COMMAND_ID_21);
}

static void ActivateGenerator(int nSprite)
{
    dassert(nSprite < kMaxSprites, 3314);
    SPRITE *pSprite = &sprite[nSprite];
    dassert(pSprite->statnum != kMaxStatus, 3316);
    int nXSprite = pSprite->extra;
    dassert(nXSprite > 0, 3319);
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int top, bottom;
    switch (pSprite->type)
    {
    case 701:
        GetSpriteExtents(pSprite, &top, &bottom);
        actSpawnThing(pSprite->sectnum, pSprite->x, pSprite->y, bottom, 423);
        break;
    case 702:
        GetSpriteExtents(pSprite, &top, &bottom);
        actSpawnThing(pSprite->sectnum, pSprite->x, pSprite->y, bottom, 424);
        break;
    case 708:
        sfxPlay3DSound(pSprite, pXSprite->at12_0);
        break;
    case 703:
        switch (pXSprite->at12_0)
        {
        case 0:
            FireballTrapSeqCallback(3, nXSprite);
            break;
        case 1:
            seqSpawn(35, 3, nXSprite, nFireballTrapClient);
            break;
        case 2:
            seqSpawn(36, 3, nXSprite, nFireballTrapClient);
            break;
        }
        break;
    case 706:
        GetSpriteExtents(pSprite, &top, &bottom);
        gFX.fxSpawn(FX_23, pSprite->sectnum, pSprite->x, pSprite->y, top, 0);
        break;
    case 707:
        GetSpriteExtents(pSprite, &top, &bottom);
        gFX.fxSpawn(FX_26, pSprite->sectnum, pSprite->x, pSprite->y, top, 0);
        break;
    }
}

static void FireballTrapSeqCallback(int, int nXSprite)
{
    XSPRITE *pXSprite = &xsprite[nXSprite];
    int nSprite = pXSprite->reference;
    SPRITE *pSprite = &sprite[nSprite];
    int dx, dy, dz;
    if (pSprite->cstat&kSpriteStat5)
    {
        dx = dy = 0;
        if (pSprite->cstat & kSpriteStat3)
            dz = 0x4000;
        else
            dz = -0x4000;
    }
    else
    {
        dx = Cos(pSprite->ang)>>16;
        dy = Sin(pSprite->ang)>>16;
        dz = 0;
    }
    actFireMissile(pSprite, 0, 0, dx, dy, dz, 305);
}
static void MGunFireSeqCallback(int, int nXSprite)
{
    int nSprite = xsprite[nXSprite].reference;
    SPRITE *pSprite = &sprite[nSprite];
    XSPRITE *pXSprite = &xsprite[nXSprite];
    if (pXSprite->at12_0 > 0 || pXSprite->at10_0 == 0)
    {
        if (pXSprite->at12_0 > 0)
        {
            pXSprite->at12_0--;
            if (pXSprite->at12_0 == 0)
                evPost(nSprite, 3, 1, COMMAND_ID_0);
        }
        int dx = (Cos(pSprite->ang)>>16)+Random2(1000);
        int dy = (Sin(pSprite->ang)>>16)+Random2(1000);
        int dz = Random2(1000);
        actFireVector(pSprite, 0, 0, dx, dy, dz, VECTOR_TYPE_2);
        sfxPlay3DSound(pSprite, 359);
    }
}

static void MGunOpenSeqCallback(int, int nXSprite)
{
    seqSpawn(39, 3, nXSprite, nMGunFireClient);
}

class TriggersLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

void TriggersLoadSave::Load()
{
    Read(&gBusyCount, sizeof(gBusyCount));
    Read(gBusy, sizeof(gBusy));
    Read(basePath, sizeof(basePath));
}

void TriggersLoadSave::Save()
{
    Write(&gBusyCount, sizeof(gBusyCount));
    Write(gBusy, sizeof(gBusy));
    Write(basePath, sizeof(basePath));
}

static TriggersLoadSave myLoadSave;
