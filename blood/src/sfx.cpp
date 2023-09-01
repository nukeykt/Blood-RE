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
#include <i86.h>
#include "typedefs.h"
#include "build.h"
#include "config.h"
#include "error.h"
#include "fx_man.h"
#include "gameutil.h"
#include "misc.h"
#include "player.h"
#include "resource.h"
#include "sfx.h"
#include "sound.h"
#include "trig.h"

POINT2D earL, earR, earL0, earR0; // Ear position
VECTOR2D earVL, earVR; // Ear velocity ?
int lPhase, rPhase, lVol, rVol, lPitch, rPitch;
int int_20E12C;

struct BONKLE
{
    int at0;
    int at4;
    DICTNODE *at8;
    int atc;
    SPRITE *at10;
    int at14;
    int at18;
    int at1c;
    POINT3D at20;
    POINT3D at2c;
    //int at20;
    //int at24;
    //int at28;
    //int at2c;
    //int at30;
    //int at34;
    int at38;
    int at3c;
};

BONKLE Bonkle[256];
BONKLE *BonkleCache[256];

int nBonkles;

void sfxInit(void)
{
    for (int i = 0; i < 256; i++)
        BonkleCache[i] = &Bonkle[i];
    nBonkles = 0;
    if (FXDevice != 8) // SoundScape
        int_20E12C = FX_GetMaxReverbDelay();
}

void sfxTerm()
{
}

static int Vol3d(int angle, int dist)
{
    return dist - mulscale16(dist, 0x2000-mulscale30(0x2000, Cos(angle)));
}

inline int Freq3d(int base, int m1, int m2)
{
    return kscale(base, m1 + 5853, m2 + 5853);
}

enum {
    kFormat0 = 0,
    kFormat1 = 1,
};

static void Calc3DValues(BONKLE *pBonkle)
{
    int dx, dy, dz;
    int angle;
    int dist;
    int phaseLeft;
    int phaseRight;
    int v8;
    int va;

    dx = pBonkle->at20.x - gMe->pSprite->x;
    dy = pBonkle->at20.y - gMe->pSprite->y;
    dz = pBonkle->at20.z - gMe->pSprite->z;
    angle = getangle(dx, dy);
    dist = Dist3d(dx, dy, dz);
    dist = ClipLow((dist>>2)+(dist>>3), 64);
    
    int v18 = kscale(pBonkle->at1c, 80, dist);
    int cosVal = Cos(angle);
    int sinVal = Sin(angle);
    v8 = dmulscale30r(cosVal, pBonkle->at20.x-pBonkle->at2c.x, sinVal, pBonkle->at20.y-pBonkle->at2c.y);
    
    int distanceL = approxDist(pBonkle->at20.x-earL.x, pBonkle->at20.y-earL.y);
    lVol = Vol3d(angle-(gMe->pSprite->ang-85), v18);
    if (pBonkle->at3c == kFormat1)
        phaseLeft = mulscale16r(distanceL, 4114);
    else
        phaseLeft = mulscale16r(distanceL, 8228);
    va = dmulscale30r(cosVal, earVL.dx, sinVal, earVL.dy);
    lPitch = Freq3d(pBonkle->at18, va, v8);

    int distanceR = approxDist(pBonkle->at20.x-earR.x, pBonkle->at20.y-earR.y);
    rVol = Vol3d(angle-(gMe->pSprite->ang+85), v18);
    if (pBonkle->at3c == kFormat1)
        phaseRight = mulscale16r(distanceR, 4114);
    else
        phaseRight = mulscale16r(distanceR, 8228);
    va = dmulscale30r(cosVal, earVR.dx, sinVal, earVR.dy);
    rPitch = Freq3d(pBonkle->at18, va, v8);
    
    int phaseMin = Min(phaseLeft, phaseRight);
    lPhase = phaseRight-phaseMin;
    rPhase = phaseLeft-phaseMin;
}

void sfxPlay3DSound(int x, int y, int z, int soundId, int nSector)
{
    if (soundId < 0)
        ThrowError(207)("Invalid sound ID");
    
    DICTNODE *hRes = gSoundRes.Lookup(soundId, "SFX");
    if (!hRes)
        return;

    SFX *pEffect = (SFX*)gSoundRes.Load(hRes);
    hRes = gSoundRes.Lookup(pEffect->rawName, "RAW");
    if (!hRes)
        return;

    int v1c, v18;
    v1c = v18 = mulscale16(pEffect->pitch, GetRate(pEffect->format));
    if (FXDevice == -1)
        return;
    if (nBonkles >= 256)
        return;
    BONKLE *pBonkle = BonkleCache[nBonkles++];
    pBonkle->at10 = NULL;
    pBonkle->at20.x = x;
    pBonkle->at20.y = y;
    pBonkle->at20.z = z;
    pBonkle->at38 = nSector;
    FindSector(x, y, z, &pBonkle->at38);
    pBonkle->at2c = pBonkle->at20;
    pBonkle->atc = soundId;
    pBonkle->at8 = hRes;
    pBonkle->at1c = pEffect->relVol;
    pBonkle->at18 = v18;
    pBonkle->at3c = pEffect->format;
    int size = Resource::Size(hRes);
    char *pData = (char*)gSoundRes.Lock(hRes);
    Calc3DValues(pBonkle);
    int priority = 1;
    if (priority < lVol)
        priority = lVol;
    if (priority < rVol)
        priority = rVol;
    if (gDoppler)
    {
        _disable();
        pBonkle->at0 = FX_PlayRaw(pData + lPhase, size - lPhase, lPitch, 0, lVol, lVol, 0, priority, (long)&pBonkle->at0);
        pBonkle->at4 = FX_PlayRaw(pData + rPhase, size - rPhase, rPitch, 0, rVol, 0, rVol, priority, (long)&pBonkle->at4);
        _enable();
    }
    else
    {
        pBonkle->at0 = FX_PlayRaw(pData + lPhase, size - lPhase, v1c, 0, lVol, lVol, rVol, priority, (long)&pBonkle->at0);
        pBonkle->at4 = 0;
    }
}

void sfxPlay3DSound(SPRITE *pSprite, int soundId, int a3, int a4)
{
    if (!pSprite)
        return;
    if (soundId < 0)
        return;
    DICTNODE *hRes = gSoundRes.Lookup(soundId, "SFX");
    if (!hRes)
        return;

    SFX *pEffect = (SFX*)gSoundRes.Load(hRes);
    DICTNODE *hSample = gSoundRes.Lookup(pEffect->rawName, "RAW");
    if (!hSample)
        return;
    int size = Resource::Size(hSample);
    if (size <= 0)
        return;
    int v14;
    v14 = mulscale16(pEffect->pitch, GetRate(pEffect->format));
    if (FXDevice == -1)
        return;
    BONKLE *pBonkle;
    if (a3 >= 0)
    {
        int i;
        for (i = 0; i < nBonkles; i++)
        {
            pBonkle = BonkleCache[i];
            if (pBonkle->at14 == a3 && (pBonkle->at10 == pSprite || (a4&1) != 0))
            {
                if ((a4&4) != 0 && pBonkle->at14 == a3)
                    return;
                if ((a4&2) != 0 && pBonkle->atc == soundId)
                    return;
                if (pBonkle->at0 > 0)
                    FX_StopSound(pBonkle->at0);
                if (pBonkle->at4 > 0)
                    FX_StopSound(pBonkle->at4);
                if (pBonkle->at8)
                {
                    gSoundRes.Unlock(pBonkle->at8);
                    pBonkle->at8 = NULL;
                }
                break;
            }
        }
        if (i == nBonkles)
        {
            if (nBonkles >= 256)
                return;
            pBonkle = BonkleCache[nBonkles++];
        }
        pBonkle->at10 = pSprite;
        pBonkle->at14 = a3;
    }
    else
    {
        if (nBonkles >= 256)
            return;
        pBonkle = BonkleCache[nBonkles++];
        pBonkle->at10 = NULL;
    }
    pBonkle->at20.x = pSprite->x;
    pBonkle->at20.y = pSprite->y;
    pBonkle->at20.z = pSprite->z;
    pBonkle->at38 = pSprite->sectnum;
    pBonkle->at2c = pBonkle->at20;
    pBonkle->atc = soundId;
    pBonkle->at8 = hSample;
    pBonkle->at1c = pEffect->relVol;
    pBonkle->at18 = v14;
    Calc3DValues(pBonkle);
    int priority = 1;
    if (priority < lVol)
        priority = lVol;
    if (priority < rVol)
        priority = rVol;
    int loopStart = pEffect->loopStart;
    int loopEnd = ClipLow(size-1, 0);
    if (a3 < 0)
        loopStart = -1;
    char *pData = (char*)gSoundRes.Lock(hSample);
    int vc = pBonkle->at4;
    _disable();
    if (loopStart >= 0)
    {
        if (gDoppler)
        {
            pBonkle->at0 = FX_PlayLoopedRaw(pData+lPhase, size-lPhase, pData+loopStart, pData+loopEnd, lPitch, 0, lVol, lVol, 0, priority, (long)&pBonkle->at0);
            pBonkle->at4 = FX_PlayLoopedRaw(pData+rPhase, size-rPhase, pData+loopStart, pData+loopEnd, rPitch, 0, rVol, 0, rVol, priority, (long)&pBonkle->at4);
        }
        else
        {
            pBonkle->at0 = FX_PlayLoopedRaw(pData+lPhase, size-lPhase, pData+loopStart, pData+loopEnd, v14, 0, lVol, lVol, rVol, priority, (long)&pBonkle->at0);
            pBonkle->at4 = 0;
        }
    }
    else
    {
        char *pData = (char*)gSoundRes.Lock(pBonkle->at8);
        if (gDoppler)
        {
            pBonkle->at0 = FX_PlayRaw(pData+lPhase, size-lPhase, lPitch, 0, lVol, lVol, 0, priority, (long)&pBonkle->at0);
            pBonkle->at4 = FX_PlayRaw(pData+rPhase, size-rPhase, rPitch, 0, rVol, 0, rVol, priority, (long)&pBonkle->at4);
        }
        else
        {
            pBonkle->at0 = FX_PlayRaw(pData+lPhase, size-lPhase, v14, 0, lVol, lVol, rVol, priority, (long)&pBonkle->at0);
            pBonkle->at4 = 0;
        }
    }
    _enable();
}

void sfxKill3DSound(SPRITE *pSprite, int a2, int a3)
{
    if (FXDevice == -1)
        return;
    if (!pSprite)
        return;
    for (int i = nBonkles - 1; i >= 0; i--)
    {
        BONKLE *pBonkle = BonkleCache[i];
        if (pBonkle->at10 == pSprite && (a2 < 0 || a2 == pBonkle->at14) && (a3 < 0 || a3 == pBonkle->atc))
        {
            if (pBonkle->at0 > 0)
            {
                FX_EndLooping(pBonkle->at0);
                FX_StopSound(pBonkle->at0);
            }
            if (pBonkle->at4 > 0)
            {
                FX_EndLooping(pBonkle->at4);
                FX_StopSound(pBonkle->at4);
            }
            if (pBonkle->at8)
            {
                gSoundRes.Unlock(pBonkle->at8);
                pBonkle->at8 = NULL;
            }
            BonkleCache[i] = BonkleCache[--nBonkles];
            BonkleCache[nBonkles] = pBonkle;
            break;
        }
    }
}

void sfxKillAllSounds(void)
{
    if (FXDevice == -1)
        return;
    for (int i = nBonkles - 1; i >= 0; i--)
    {
        BONKLE *pBonkle = BonkleCache[i];
        if (pBonkle->at0 > 0)
        {
            FX_EndLooping(pBonkle->at0);
            FX_StopSound(pBonkle->at0);
        }
        if (pBonkle->at4 > 0)
        {
            FX_EndLooping(pBonkle->at4);
            FX_StopSound(pBonkle->at4);
        }
        if (pBonkle->at8)
        {
            gSoundRes.Unlock(pBonkle->at8);
            pBonkle->at8 = NULL;
        }
        BonkleCache[i] = BonkleCache[--nBonkles];
        BonkleCache[nBonkles] = pBonkle;
    }
}

void sfxUpdate3DSounds(void)
{
    int dx = mulscale30(Cos(gMe->pSprite->ang+512), 43);
    int dy = mulscale30(Sin(gMe->pSprite->ang+512), 43);
    earL0 = earL;
    earR0 = earR;
    earL.x = gMe->pSprite->x-dx;
    earL.y = gMe->pSprite->y-dy;
    earR.x = gMe->pSprite->x+dx;
    earR.y = gMe->pSprite->y+dy;
    earVL.dx = earL.x-earL0.x;
    earVL.dy = earL.y-earL0.y;
    earVR.dx = earR.x-earR0.x;
    earVR.dy = earR.y-earR0.y;
    for (int i = nBonkles - 1; i >= 0; i--)
    {
        BONKLE *pBonkle = BonkleCache[i];
        if (pBonkle->at0 > 0 || pBonkle->at4 > 0)
        {
            if (!pBonkle->at8)
                continue;
            SPRITE *pSprite = pBonkle->at10;
            if (pSprite)
            {
                pBonkle->at2c = pBonkle->at20;
                pBonkle->at20.x = pSprite->x;
                pBonkle->at20.y = pSprite->y;
                pBonkle->at20.z = pSprite->z;
                pBonkle->at38 = pSprite->sectnum;
            }
            Calc3DValues(pBonkle);
            _disable();
            if (pBonkle->at0 > 0)
            {
                if (pBonkle->at4 > 0)
                {
                    FX_SetPan(pBonkle->at0, lVol, lVol, 0);
                    FX_SetFrequency(pBonkle->at0, lPitch);
                }
                else
                    FX_SetPan(pBonkle->at0, lVol, lVol, rVol);
            }
            if (pBonkle->at4 > 0)
            {
                FX_SetPan(pBonkle->at4, rVol, 0, rVol);
                FX_SetFrequency(pBonkle->at4, rPitch);
            }
            _enable();
        }
        else
        {
            gSoundRes.Unlock(pBonkle->at8);
            pBonkle->at8 = NULL;
            BonkleCache[i] = BonkleCache[--nBonkles];
            BonkleCache[nBonkles] = pBonkle;
        }
    }
}

void sfxSetReverbDelay(int delay);

void sfxSetReverb(BOOL toggle)
{
    if (FXDevice == 8)
        return;
    if (toggle)
    {
        FX_SetFastReverb(1);
        sfxSetReverbDelay(10);
    }
    else
        FX_SetFastReverb(0);
}

void sfxSetReverb2(BOOL toggle)
{
    if (FXDevice == 8)
        return;
    if (toggle)
    {
        FX_SetFastReverb(1);
        sfxSetReverbDelay(20);
    }
    else
        FX_SetFastReverb(0);
}

void sfxSetReverbDelay(int delay)
{
    if (FXDevice == 8)
        return;
    FX_SetReverbDelay(delay);
}
