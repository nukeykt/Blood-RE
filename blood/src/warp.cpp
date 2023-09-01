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
#include "build.h"
#include "db.h"
#include "debug4g.h"
#include "gameutil.h"
#include "levels.h"
#include "loadsave.h"
#include "misc.h"
#include "view.h"
#include "warp.h"

ZONE gStartZone[8];

void warpInit(void)
{
    int nSprite, nXSprite;
    for (int i = 0; i < kMaxSectors; i++)
    {
        gUpperLink[i] = -1;
        gLowerLink[i] = -1;
    }
    for (nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus)
        {
            SPRITE *pSprite = &sprite[nSprite];
            nXSprite = pSprite->extra;
            if (nXSprite > 0)
            {
                XSPRITE *pXSprite = &xsprite[nXSprite];
                switch (pSprite->type)
                {
                case 1:
                    if (gGameOptions.nGameType < GAMETYPE_2 && pXSprite->at10_0 >= 0 && pXSprite->at10_0 < 8)
                    {
                        ZONE *pZone = &gStartZone[pXSprite->at10_0];
                        pZone->x = pSprite->x;
                        pZone->y = pSprite->y;
                        pZone->z = pSprite->z;
                        pZone->sectnum = pSprite->sectnum;
                        pZone->ang = pSprite->ang;
                    }
                    DeleteSprite(nSprite);
                    break;
                case 2:
                    if (gGameOptions.nGameType >= GAMETYPE_2 && pXSprite->at10_0 >= 0 && pXSprite->at10_0 < 8)
                    {
                        ZONE *pZone = &gStartZone[pXSprite->at10_0];
                        pZone->x = pSprite->x;
                        pZone->y = pSprite->y;
                        pZone->z = pSprite->z;
                        pZone->sectnum = pSprite->sectnum;
                        pZone->ang = pSprite->ang;
                    }
                    DeleteSprite(nSprite);
                    break;
                case 7:
                    gUpperLink[pSprite->sectnum] = nSprite;
                    pSprite->cstat |= kSpriteStat31;
                    pSprite->cstat &= ~(kSpriteStat0 | kSpriteStat8);
                    break;
                case 6:
                    gLowerLink[pSprite->sectnum] = nSprite;
                    pSprite->cstat |= kSpriteStat31;
                    pSprite->cstat &= ~(kSpriteStat0 | kSpriteStat8);
                    break;
                case 9:
                case 11:
                case 13:
                    gUpperLink[pSprite->sectnum] = nSprite;
                    pSprite->cstat |= kSpriteStat31;
                    pSprite->cstat &= ~(kSpriteStat0 | kSpriteStat8);
                    pSprite->z = getflorzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
                    break;
                case 10:
                case 12:
                case 14:
                    gLowerLink[pSprite->sectnum] = nSprite;
                    pSprite->cstat |= kSpriteStat31;
                    pSprite->cstat &= ~(kSpriteStat0 | kSpriteStat8);
                    pSprite->z = getceilzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
                    break;
                }
            }
        }
    }
    for (int nSector = 0; nSector < kMaxSectors; nSector++)
    {
        if (gUpperLink[nSector] >= 0)
        {
            SPRITE *pSprite = &sprite[gUpperLink[nSector]];
            nXSprite = pSprite->extra;
            dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 127);
            XSPRITE *pXSprite = &xsprite[nXSprite];
            int nLink = pXSprite->at10_0;
            for (int nSector2 = 0; nSector2 < kMaxSectors; nSector2++)
            {
                if (gLowerLink[nSector2] >= 0)
                {
                    SPRITE *pSprite2 = &sprite[gLowerLink[nSector2]];
                    nXSprite = pSprite2->extra;
                    dassert(nXSprite > 0 && nXSprite < kMaxXSprites, 137);
                    XSPRITE *pXSprite2 = &xsprite[nXSprite];
                    if (pXSprite2->at10_0 == nLink)
                    {
                        pSprite->owner = gLowerLink[nSector2];
                        pSprite2->owner = gUpperLink[nSector];
                    }
                }
            }
        }
    }
}

int CheckLink(SPRITE *pSprite)
{
    int z1, z2;
    int nUpper = gUpperLink[pSprite->sectnum];
    int nLower = gLowerLink[pSprite->sectnum];
    if (nUpper >= 0)
    {
        SPRITE *pUpper = &sprite[nUpper];
        if (pUpper->type == 7)
            z1 = pUpper->z;
        else
            z1 = getflorzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
        if (z1 <= pSprite->z)
        {
            nLower = pUpper->owner;
            dassert(nLower >= 0 && nLower < kMaxSprites, 169);
            SPRITE *pLower = &sprite[nLower];
            dassert(pLower->sectnum >= 0 && pLower->sectnum < kMaxSectors, 171);
            ChangeSpriteSect(pSprite->index, pLower->sectnum);
            pSprite->x += pLower->x-pUpper->x;
            pSprite->y += pLower->y-pUpper->y;
            if (pLower->type == kMarker6)
                z2 = pLower->z;
            else
                z2 = getceilzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
            pSprite->z += z2-z1;
            ClearBitString(gInterpolateSprite, pSprite->index);
            return pUpper->type;
        }
    }
    if (nLower >= 0)
    {
        SPRITE *pLower = &sprite[nLower];
        if (pLower->type == 6)
            z2 = pLower->z;
        else
            z2 = getceilzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
        if (z2 >= pSprite->z)
        {
            nUpper = pLower->owner;
            dassert(nUpper >= 0 && nUpper < kMaxSprites, 198);
            SPRITE *pUpper = &sprite[nUpper];
            dassert(pUpper->sectnum >= 0 && pUpper->sectnum < kMaxSectors, 200);
            ChangeSpriteSect(pSprite->index, pUpper->sectnum);
            pSprite->x += pUpper->x-pLower->x;
            pSprite->y += pUpper->y-pLower->y;
            if (pUpper->type == kMarker7)
                z1 = pUpper->z;
            else
                z1 = getflorzofslope(pSprite->sectnum, pSprite->x, pSprite->y);
            pSprite->z += z1-z2;
            ClearBitString(gInterpolateSprite, pSprite->index);
            return pLower->type;
        }
    }
    return 0;
}

int CheckLink(long *x, long *y, long *z, int *nSector)
{
    int z1, z2;
    int nUpper = gUpperLink[*nSector];
    int nLower = gLowerLink[*nSector];
    if (nUpper >= 0)
    {
        SPRITE *pUpper = &sprite[nUpper];
        if (pUpper->type == kMarker7)
            z1 = pUpper->z;
        else
            z1 = getflorzofslope(*nSector, *x, *y);
        if (z1 <= *z)
        {
            nLower = pUpper->owner;
            dassert(nLower >= 0 && nLower < kMaxSprites, 235);
            SPRITE *pLower = &sprite[nLower];
            dassert(pLower->sectnum >= 0 && pLower->sectnum < kMaxSectors, 237);
            *nSector = pLower->sectnum;
            *x += pLower->x-pUpper->x;
            *y += pLower->y-pUpper->y;
            if (pLower->type == kMarker6)
                z2 = pLower->z;
            else
                z2 = getceilzofslope(*nSector, *x, *y);
            *z += z2-z1;
            return pUpper->type;
        }
    }
    if (nLower >= 0)
    {
        SPRITE *pLower = &sprite[nLower];
        if (pLower->type == kMarker6)
            z2 = pLower->z;
        else
            z2 = getceilzofslope(*nSector, *x, *y);
        if (z2 >= *z)
        {
            nUpper = pLower->owner;
            dassert(nUpper >= 0 && nUpper < kMaxSprites, 263);
            SPRITE *pUpper = &sprite[nUpper];
            dassert(pUpper->sectnum >= 0 && pUpper->sectnum < kMaxSectors, 265);
            *nSector = pUpper->sectnum;
            *x += pUpper->x-pLower->x;
            *y += pUpper->y-pLower->y;
            if (pUpper->type == kMarker7)
                z1 = pUpper->z;
            else
                z1 = getflorzofslope(*nSector, *x, *y);
            *z += z1-z2;
            return pLower->type;
        }
    }
    return 0;
}

class WarpLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

void WarpLoadSave::Load()
{
    Read(gStartZone, sizeof(gStartZone));
    Read(gUpperLink, sizeof(gUpperLink));
    Read(gLowerLink, sizeof(gLowerLink));
}

void WarpLoadSave::Save()
{
    Write(gStartZone, sizeof(gStartZone));
    Write(gUpperLink, sizeof(gUpperLink));
    Write(gLowerLink, sizeof(gLowerLink));
}

static WarpLoadSave myLoadSave;

