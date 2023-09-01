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
#include "asound.h"
#include "build.h"
#include "db.h"
#include "error.h"
extern "C" {
#include "fx_man.h"
}
#include "gameutil.h"
#include "misc.h"
#include "player.h"
#include "resource.h"
#include "sound.h"

struct AMB_CHANNEL
{
    int at0;
    int at4;
    int at8;
    DICTNODE *atc;
    char *at10;
    int at14;
    int at18;
};

AMB_CHANNEL ambChannels[16];
int nAmbChannels = 0;

void ambProcess(void)
{
    int i;
    for (int nSprite = headspritestat[12]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        int nXSprite = pSprite->extra;
        if (nXSprite <= 0 || nXSprite >= kMaxXSprites)
            continue;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        if (pXSprite->at1_6)
        {
            int dx = pSprite->x-gMe->pSprite->x;
            int dy = pSprite->y-gMe->pSprite->y;
            int dz = pSprite->z-gMe->pSprite->z;
            int nDist = Dist3d(dx, dy, dz);
            int vs = mulscale16(pXSprite->at18_2, pXSprite->at1_7);
            int t = ClipRange(scale(nDist, pXSprite->at10_0, pXSprite->at12_0, vs, 0), 0, vs);
            ambChannels[pSprite->owner].at4 += t;
        }
    }
    AMB_CHANNEL *pChannel = ambChannels;
    for (i = 0; i < nAmbChannels; i++, pChannel++)
    {
        if (pChannel->at0 > 0)
            FX_SetPan(pChannel->at0, pChannel->at4, pChannel->at4, pChannel->at4);
        else
        {
            int end = ClipLow(pChannel->at14-1, 0);
            pChannel->at0 = FX_PlayLoopedRaw(pChannel->at10, pChannel->at14, pChannel->at10, pChannel->at10+end, GetRate(pChannel->at18), 0,
                pChannel->at4, pChannel->at4, pChannel->at4, pChannel->at4, (ulong)&pChannel->at0);
        }
        pChannel->at4 = 0;
    }
}

void ambKillAll(void)
{
    AMB_CHANNEL *pChannel = ambChannels;
    for (int i = 0; i < nAmbChannels; i++, pChannel++)
    {
        if (pChannel->at0 > 0)
        {
            FX_EndLooping(pChannel->at0);
            FX_StopSound(pChannel->at0);
        }
        if (pChannel->atc)
        {
            gSoundRes.Unlock(pChannel->atc);
            pChannel->atc = NULL;
        }
    }
    nAmbChannels = 0;
}

void ambInit(void)
{
    ambKillAll();
    memset(ambChannels, 0, sizeof(ambChannels));
    for (int nSprite = headspritestat[12]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        int nXSprite = pSprite->extra;
        if (nXSprite < 0 || nXSprite >= kMaxXSprites)
            continue;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        if (pXSprite->at12_0 <= pXSprite->at10_0)
            continue;
        AMB_CHANNEL *pChannel = ambChannels;
        for (int i = 0; i < nAmbChannels; i++, pChannel++)
            if (pChannel->at8 == pXSprite->at14_0)
                break;
        if (i == nAmbChannels)
        {
            if (i >= 16)
                continue;
            int nSFX = pXSprite->at14_0;
            DICTNODE *pSFXNode = gSoundRes.Lookup(nSFX, "SFX");
            if (!pSFXNode)
                ThrowError(203)("Missing sound #%d used in ambient sound generator %d\n", nSFX, nSprite);
            SFX *pSFX = (SFX*)gSoundRes.Load(pSFXNode);
            DICTNODE *pRAWNode = gSoundRes.Lookup(pSFX->rawName, "RAW");
            if (!pRAWNode)
                ThrowError(208)("Missing RAW sound '%s' used in ambient sound generator %d\n", pSFX->rawName, nSprite);
            if (Resource::Size(pRAWNode) <= 0)
                continue;
            pChannel->at14 = Resource::Size(pRAWNode);
            pChannel->at8 = nSFX;
            pChannel->atc = pRAWNode;
            pChannel->at14 = Resource::Size(pRAWNode);
            pChannel->at10 = (char*)gSoundRes.Lock(pRAWNode);
            pChannel->at18 = pSFX->format;
            nAmbChannels++;
        }
        pSprite->owner = i;
    }
}