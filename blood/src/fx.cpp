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
#include "typedefs.h"
#include "actor.h"
#include "build.h"
#include "callback.h"
#include "config.h"
#include "db.h"
#include "debug4g.h"
#include "eventq.h"
#include "fx.h"
#include "levels.h"
#include "misc.h"
#include "seq.h"
#include "trig.h"
#include "view.h"

CFX gFX;

struct FXDATA {
    CALLBACK_ID funcID; // callback
    char at1; // detail
    short at2; // seq
    ushort at4; // flags
    int at6; // gravity
    int ata; // air drag
    int ate;
    short at12; // picnum
    unsigned char at14; // xrepeat
    unsigned char at15; // yrepeat
    short at16; // cstat
    signed char at18; // shade
    char at19; // pal
};

enum {
    kFXFlag0 = 1,
    kFXFlag1 = 2,
};

static FXDATA gFXData[] = {
    { CALLBACK_ID_NONE, 0, 49, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 50, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 51, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 52, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 44, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 45, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 46, 1, -128, 8192, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 42, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 43, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 48, 3, -256, 8192, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 60, 3, -256, 8192, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_14, 2, 0, 1, 46603, 2048, 480, 2154, 40, 40, 0, -12, 0 },
    { CALLBACK_ID_NONE, 2, 0, 3, 46603, 5120, 480, 2269, 24, 24, 0, -128, 0 },
    { CALLBACK_ID_NONE, 2, 0, 3, 46603, 5120, 480, 1720, 24, 24, 0, -128, 0 },
    { CALLBACK_ID_NONE, 1, 0, 1, 58254, 3072, 480, 2280, 48, 48, 0, -128, 0 },
    { CALLBACK_ID_NONE, 1, 0, 1, 58254, 3072, 480, 3135, 48, 48, 0, -128, 0 },
    { CALLBACK_ID_NONE, 0, 0, 3, 58254, 1024, 480, 3261, 32, 32, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 58254, 1024, 480, 3265, 32, 32, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 58254, 1024, 480, 3269, 32, 32, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 58254, 1024, 480, 3273, 32, 32, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 58254, 1024, 480, 3277, 32, 32, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 0, 1, -27962, 8192, 600, 1128, 16, 16, 514, -16, 0 },
    { CALLBACK_ID_NONE, 2, 0, 1, -18641, 8192, 600, 1128, 12, 12, 514, -16, 0 },
    { CALLBACK_ID_NONE, 2, 0, 1, -9320, 8192, 600, 1128, 8, 8, 514, -16, 0 },
    { CALLBACK_ID_NONE, 2, 0, 1, -18641, 8192, 600, 1131, 32, 32, 514, -16, 0 },
    { CALLBACK_ID_14, 2, 0, 3, 27962, 4096, 480, 733, 32, 32, 0, -16, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 18641, 4096, 120, 2261, 12, 12, 0, -128, 0 },
    { CALLBACK_ID_NONE, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 58254, 3328, 480, 2185, 48, 48, 0, 0, 0 },
    { CALLBACK_ID_NONE, 0, 0, 3, 58254, 1024, 480, 2620, 48, 48, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 55, 1, -13981, 5120, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 56, 1, -13981, 5120, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 57, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 58, 1, 0, 2048, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 0, 0, 0, 0, 960, 956, 32, 32, 610, 0, 0 },
    { CALLBACK_ID_16, 2, 62, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_16, 2, 63, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_16, 2, 64, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_16, 2, 65, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_16, 2, 66, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_16, 2, 67, 0, 46603, 1024, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 0, 0, 0, 838, 16, 16, 80, -8, 0 },
    { CALLBACK_ID_NONE, 0, 0, 3, 34952, 8192, 0, 2078, 64, 64, 0, -8, 0 },
    { CALLBACK_ID_NONE, 0, 0, 3, 34952, 8192, 0, 1106, 64, 64, 0, -8, 0 },
    { CALLBACK_ID_NONE, 0, 0, 3, 58254, 3328, 480, 2406, 48, 48, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 3, 46603, 4096, 480, 3511, 64, 64, 0, -128, 0 },
    { CALLBACK_ID_NONE, 0, 8, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 11, 3, -256, 8192, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 2, 11, 3, 0, 8192, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { CALLBACK_ID_NONE, 1, 30, 3, 0, 0, 0, 0, 40, 40, 80, -8, 0 },
    { CALLBACK_ID_19, 2, 0, 3, 27962, 4096, 480, 4023, 32, 32, 0, -16, 0 },
    { CALLBACK_ID_19, 2, 0, 3, 27962, 4096, 480, 4028, 32, 32, 0, -16, 0 },
    { CALLBACK_ID_NONE, 2, 0, 0, 0, 0, 480, 926, 32, 32, 610, -12, 0 },
    { CALLBACK_ID_NONE, 1, 70, 1, -13981, 5120, 0, 0, 0, 0, 0, 0, 0 }
};

void CFX::func_73FB0(int nSprite)
{
    if (nSprite < 0 || nSprite >= kMaxSprites)
        return;
    evKill(nSprite, 3);
    if (sprite[nSprite].extra > 0)
        seqKill(3, sprite[nSprite].extra);
    DeleteSprite(nSprite);
}

void CFX::func_73FFC(int nSprite)
{
    if (nSprite < 0 || nSprite >= kMaxSprites)
        return;
    SPRITE *pSprite = &sprite[nSprite];
    if (pSprite->extra > 0)
        seqKill(3, pSprite->extra);
    if (pSprite->statnum != kStatFree)
        actPostSprite(nSprite, kStatFree);
}

SPRITE *CFX::fxSpawn(FX_ID nFx, int nSector, int x, int y, int z, unsigned int duration)
{
    if (nSector < 0 || nSector >= numsectors)
        return NULL;
    int nSector2 = nSector;
    if (!FindSector(x, y, z, &nSector2))
        return NULL;
    if (gbAdultContent && gGameOptions.nGameType <= GAMETYPE_0)
    {
        switch (nFx)
        {
        case FX_0:
        case FX_1:
        case FX_2:
        case FX_3:
        case FX_13:
        case FX_34:
        case FX_35:
        case FX_36:
            return NULL;
        default:
            break;
        }
    }
    if (nFx < 0 || nFx >= kFXMax)
        return NULL;
    FXDATA *pFX = &gFXData[nFx];
    if (gStatCount[1] == 512)
    {
        int nSprite = headspritestat[1];;
        while ((sprite[nSprite].flags & kSpriteFlag5) && nSprite != -1)
            nSprite = nextspritestat[nSprite];
        if (nSprite == -1)
            return NULL;
        func_73FB0(nSprite);
    }
    SPRITE *pSprite = actSpawnSprite(nSector, x, y, z, 1, 0);
    pSprite->type = nFx;
    pSprite->picnum = pFX->at12;
    pSprite->cstat |= pFX->at16;
    pSprite->shade = pFX->at18;
    pSprite->pal = pFX->at19;
    pSprite->detail = pFX->at1;
    if (pFX->at14 > 0)
        pSprite->xrepeat = pFX->at14;
    if (pFX->at15 > 0)
        pSprite->yrepeat = pFX->at15;
    if ((pFX->at4 & kFXFlag0) && Chance(0x8000))
        pSprite->cstat |= 4;
    if ((pFX->at4 & kFXFlag1) && Chance(0x8000))
        pSprite->cstat |= 8;
    if (pFX->at2 != 0)
    {
        int nXSprite = dbInsertXSprite(pSprite->index);
        seqSpawn(pFX->at2, 3, nXSprite);
    }
    if (duration == 0)
        duration = pFX->ate;
    if (duration)
        evPost((int)pSprite->index, 3, duration+Random2(duration/2), CALLBACK_ID_1);
    return pSprite;
}

void CFX::fxProcess(void)
{
    for (int nSprite = headspritestat[1]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        viewBackupSpriteLoc(nSprite, pSprite);
        short nSector = pSprite->sectnum;
        dassert(nSector >= 0 && nSector < kMaxSectors, 252);
        dassert(pSprite->type < kFXMax, 254);
        FXDATA *pFXData = &gFXData[pSprite->type];
        actAirDrag(pSprite, pFXData->ata);
        if (xvel[nSprite])
            pSprite->x += xvel[nSprite]>>12;
        if (yvel[nSprite])
            pSprite->y += yvel[nSprite]>>12;
        if (zvel[nSprite])
            pSprite->z += zvel[nSprite]>>8;
        // Weird...
        if (xvel[nSprite] || (yvel[nSprite] && pSprite->z >= sector[pSprite->sectnum].floorz))
        {
            updatesector(pSprite->x, pSprite->y, &nSector);
            if (nSector == -1)
            {
                func_73FFC(nSprite);
                continue;
            }
            if (pSprite->z >= getflorzofslope(pSprite->sectnum, pSprite->x, pSprite->y))
            {
                if (pFXData->funcID >= 0 && pFXData->funcID < kCallbackMax)
                {
                    dassert(gCallback[pFXData->funcID] != NULL, 290);
                    gCallback[pFXData->funcID](nSprite);
                }
                else
                    func_73FFC(nSprite);
                continue;
            }
            if (nSector != pSprite->sectnum)
            {
                dassert(nSector >= 0 && nSector < kMaxSectors, 300);
                ChangeSpriteSect(nSprite, nSector);
            }
        }
        if (xvel[nSprite] || yvel[nSprite] || zvel[nSprite])
        {
            int ceilZ, floorZ;
            getzsofslope(nSector, pSprite->x, pSprite->y, &ceilZ, &floorZ);
            if (pSprite->z < ceilZ && !(sector[nSector].ceilingstat&kSectorStat0))
            {
                func_73FFC(nSprite);
                continue;
            }
            if (pSprite->z > floorZ)
            {
                if (pFXData->funcID >= 0 && pFXData->funcID < kCallbackMax)
                {
                    dassert(gCallback[pFXData->funcID] != NULL, 319);
                    gCallback[pFXData->funcID](nSprite);
                }
                else
                    func_73FFC(nSprite);
                continue;
            }
        }
        zvel[nSprite] += pFXData->at6;
    }
}

void fxSpawnBlood(SPRITE *pSprite, int)
{
    if (pSprite->sectnum < 0 || pSprite->sectnum >= numsectors)
        return;
    int nSector = pSprite->sectnum;
    if (!FindSector(pSprite->x, pSprite->y, pSprite->z, &nSector))
        return;
    if (gbAdultContent && gGameOptions.nGameType <= GAMETYPE_0)
        return;
    SPRITE *pBlood = gFX.fxSpawn(FX_27, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pBlood)
    {
        pBlood->ang = 1024;
        xvel[pBlood->index] = Random2(0x6aaaa);
        yvel[pBlood->index] = Random2(0x6aaaa);
        zvel[pBlood->index] = -Random(0x10aaaa)-100;
        evPost(pBlood->index, 3, 8, CALLBACK_ID_6);
    }
}

void func_746D4(SPRITE *pSprite, int)
{
    if (pSprite->sectnum < 0 || pSprite->sectnum >= numsectors)
        return;
    int nSector = pSprite->sectnum;
    if (!FindSector(pSprite->x, pSprite->y, pSprite->z, &nSector))
        return;
    if (gbAdultContent && gGameOptions.nGameType <= GAMETYPE_0)
        return;
    SPRITE *pSpawn;
    if (pSprite->type == 221)
        pSpawn = gFX.fxSpawn(FX_53, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    else
        pSpawn = gFX.fxSpawn(FX_54, pSprite->sectnum, pSprite->x, pSprite->y, pSprite->z);
    if (pSpawn)
    {
        pSpawn->ang = 1024;
        xvel[pSpawn->index] = Random2(0x6aaaa);
        yvel[pSpawn->index] = Random2(0x6aaaa);
        int t = -Random(0x10aaaa) - 100;
        zvel[pSpawn->index] = t;
        evPost(pSpawn->index, 3, 8, CALLBACK_ID_18);
    }
}

void func_74818(SPRITE *pSprite, int z, int a3, int a4)
{
    int x = pSprite->x+mulscale28(pSprite->clipdist-4, Cos(pSprite->ang));
    int y = pSprite->y+mulscale28(pSprite->clipdist-4, Sin(pSprite->ang));
    x += mulscale30(a3, Cos(pSprite->ang+512));
    y += mulscale30(a3, Sin(pSprite->ang+512));
    SPRITE *pShell = gFX.fxSpawn((FX_ID)(FX_37+Random(3)), pSprite->sectnum, x, y, z, 0);
    if (pShell)
    {
        int t2 = Random2(((a4/4)<<18)/120);
        int nAngle = pSprite->ang+Random2(56)+512;
        xvel[pShell->index] = mulscale30((a4<<18)/120+t2, Cos(nAngle));
        yvel[pShell->index] = mulscale30((a4<<18)/120+t2, Sin(nAngle));
        zvel[pShell->index] = zvel[pSprite->index]-0x20000-(Random2(40)<<18)/120;
    }
}

void func_74A18(SPRITE *pSprite, int z, int a3, int a4)
{
    int x = pSprite->x+mulscale28(pSprite->clipdist-4, Cos(pSprite->ang));
    int y = pSprite->y+mulscale28(pSprite->clipdist-4, Sin(pSprite->ang));
    x += mulscale30(a3, Cos(pSprite->ang+512));
    y += mulscale30(a3, Sin(pSprite->ang+512));
    SPRITE *pShell = gFX.fxSpawn((FX_ID)(FX_40+Random(3)), pSprite->sectnum, x, y, z, 0);
    if (pShell)
    {
        int t2 = Random2(((a4/4)<<18)/120);
        int nAngle = pSprite->ang+Random2(56)+512;
        xvel[pShell->index] = mulscale30((a4<<18)/120+t2, Cos(nAngle));
        yvel[pShell->index] = mulscale30((a4<<18)/120+t2, Sin(nAngle));
        zvel[pShell->index] = zvel[pSprite->index]-0x20000-(Random2(20)<<18)/120;
    }
}

