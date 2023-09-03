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
#include "build.h"
#include "config.h"
#include "debug4g.h"
#include "error.h"
#include "eventq.h"
#include "fx.h"
#include "gameutil.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "misc.h"
#include "player.h"
#include "qav.h"
#include "resource.h"
#include "seq.h"
#include "sfx.h"
#include "sound.h"
#include "trig.h"
#include "weapon.h"

#define kQAVEnd 125

QAV* weaponQAV[kQAVEnd];

static void FirePitchfork(int, PLAYER *pPlayer);
static void FireSpray(int, PLAYER *pPlayer);
static void ThrowCan(int, PLAYER *pPlayer);
static void DropCan(int, PLAYER *pPlayer);
static void ExplodeCan(int, PLAYER *pPlayer);
static void ThrowBundle(int, PLAYER *pPlayer);
static void DropBundle(int, PLAYER *pPlayer);
static void ExplodeBundle(int, PLAYER *pPlayer);
static void ThrowProx(int, PLAYER *pPlayer);
static void DropProx(int, PLAYER *pPlayer);
static void ThrowRemote(int, PLAYER *pPlayer);
static void DropRemote(int, PLAYER *pPlayer);
static void FireRemote(int, PLAYER *pPlayer);
static void FireShotgun(int nTrigger, PLAYER *pPlayer);
static void EjectShell(int, PLAYER *pPlayer);
static void FireTommy(int nTrigger, PLAYER *pPlayer);
static void FireSpread(int nTrigger, PLAYER *pPlayer);
static void AltFireSpread(int nTrigger, PLAYER *pPlayer);
static void AltFireSpread2(int nTrigger, PLAYER *pPlayer);
static void FireFlare(int nTrigger, PLAYER *pPlayer);
static void AltFireFlare(int nTrigger, PLAYER *pPlayer);
static void FireVoodoo(int nTrigger, PLAYER *pPlayer);
static void AltFireVoodoo(int nTrigger, PLAYER *pPlayer);
static void DropVoodoo(int nTrigger, PLAYER *pPlayer);
static void FireTesla(int nTrigger, PLAYER *pPlayer);
static void AltFireTesla(int nTrigger, PLAYER *pPlayer);
static void FireNapalm(int nTrigger, PLAYER *pPlayer);
static void FireNapalm2(int nTrigger, PLAYER *pPlayer);
static void AltFireNapalm(int nTrigger, PLAYER *pPlayer);
static void FireLifeLeech(int nTrigger, PLAYER *pPlayer);
static void AltFireLifeLeech(int nTrigger, PLAYER *pPlayer);
static void FireBeast(int nTrigger, PLAYER * pPlayer);

typedef void(*QAVTypeCast)(int, void *);

static int nClientFirePitchfork = qavRegisterClient((QAVTypeCast)FirePitchfork);
static int nClientFireSpray = qavRegisterClient((QAVTypeCast)FireSpray);
static int nClientThrowCan = qavRegisterClient((QAVTypeCast)ThrowCan);
static int nClientDropCan = qavRegisterClient((QAVTypeCast)DropCan);
static int nClientExplodeCan = qavRegisterClient((QAVTypeCast)ExplodeCan);
static int nClientThrowBundle = qavRegisterClient((QAVTypeCast)ThrowBundle);
static int nClientDropBundle = qavRegisterClient((QAVTypeCast)DropBundle);
static int nClientExplodeBundle = qavRegisterClient((QAVTypeCast)ExplodeBundle);
static int nClientThrowProx = qavRegisterClient((QAVTypeCast)ThrowProx);
static int nClientDropProx = qavRegisterClient((QAVTypeCast)DropProx);
static int nClientThrowRemote = qavRegisterClient((QAVTypeCast)ThrowRemote);
static int nClientDropRemote = qavRegisterClient((QAVTypeCast)DropRemote);
static int nClientFireRemote = qavRegisterClient((QAVTypeCast)FireRemote);
static int nClientFireShotgun = qavRegisterClient((QAVTypeCast)FireShotgun);
static int nClientEjectShell = qavRegisterClient((QAVTypeCast)EjectShell);
static int nClientFireTommy = qavRegisterClient((QAVTypeCast)FireTommy);
static int nClientAltFireSpread2 = qavRegisterClient((QAVTypeCast)AltFireSpread2);
static int nClientFireSpread = qavRegisterClient((QAVTypeCast)FireSpread);
static int nClientAltFireSpread = qavRegisterClient((QAVTypeCast)AltFireSpread);
static int nClientFireFlare = qavRegisterClient((QAVTypeCast)FireFlare);
static int nClientAltFireFlare = qavRegisterClient((QAVTypeCast)AltFireFlare);
static int nClientFireVoodoo = qavRegisterClient((QAVTypeCast)FireVoodoo);
static int nClientAltFireVoodoo = qavRegisterClient((QAVTypeCast)AltFireVoodoo);
static int nClientFireTesla = qavRegisterClient((QAVTypeCast)FireTesla);
static int nClientAltFireTesla = qavRegisterClient((QAVTypeCast)AltFireTesla);
static int nClientFireNapalm = qavRegisterClient((QAVTypeCast)FireNapalm);
static int nClientFireNapalm2 = qavRegisterClient((QAVTypeCast)FireNapalm2);
static int nClientFireLifeLeech = qavRegisterClient((QAVTypeCast)FireLifeLeech);
static int nClientFireBeast = qavRegisterClient((QAVTypeCast)FireBeast);
static int nClientAltFireLifeLeech = qavRegisterClient((QAVTypeCast)AltFireLifeLeech);
static int nClientDropVoodoo = qavRegisterClient((QAVTypeCast)DropVoodoo);
static int nClientAltFireNapalm = qavRegisterClient((QAVTypeCast)AltFireNapalm);

void QAV::PlaySound(int nSound)
{
    sndStartSample(nSound, -1);
}

void QAV::PlaySound3D(SPRITE *pSprite, int nSound, int a3, int a4)
{
    sfxPlay3DSound(pSprite, nSound, a3, a4);
}

static BOOL func_4B1A4(PLAYER *pPlayer)
{
    switch (pPlayer->atbd)
    {
    case 7:
        if (pPlayer->atc3 == 5 || pPlayer->atc3 == 6)
            return 1;
        break;
    case 6:
        if (pPlayer->atc3 == 4 || pPlayer->atc3 == 5 || pPlayer->atc3 == 6)
            return 1;
        break;
    }
    return 0;
}

static BOOL BannedUnderwater(int nWeapon)
{
    return nWeapon == 7 || nWeapon == 6;
}

static BOOL func_4B1FC(PLAYER *pPlayer, int a2, int a3, int a4 = 1)
{
    if (gInfiniteAmmo)
        return 1;
    if (a3 == -1)
        return 1;
    if (a2 == 12 && pPlayer->atc7 == 11 && pPlayer->atc3 == 11)
        return 1;
    if (a2 == 9 && pPlayer->pXSprite->health > 0)
        return 1;
    return pPlayer->at181[a3] >= a4;
}

static BOOL CheckAmmo(PLAYER *pPlayer, int a2, int a3 = 1)
{
    if (gInfiniteAmmo)
        return 1;
    if (a2 == -1)
        return 1;
    if (pPlayer->atbd == 12 && pPlayer->atc7 == 11 && pPlayer->atc3 == 11)
        return 1;
    if (pPlayer->atbd == 9 && pPlayer->pXSprite->health >= (a3<<4))
        return 1;
    return pPlayer->at181[a2] >= a3;
}

static BOOL func_4B2C8(PLAYER *pPlayer, int a2, int a3 = 1)
{
    if (gInfiniteAmmo)
        return 1;
    if (a2 == -1)
        return 1;
    return pPlayer->at181[a2] >= a3;
}

void SpawnBulletEject(PLAYER *pPlayer, int a2, int a3)
{
    POSTURE *pPosture = &gPosture[pPlayer->at5f][pPlayer->at2f];
    pPlayer->at67 = pPlayer->pSprite->z-pPosture->at24;
    int t = pPlayer->at6f - pPlayer->at67;
    int dz = pPlayer->at6f-t/2;
    func_74818(pPlayer->pSprite, dz, a2, a3);
}

void SpawnShellEject(PLAYER *pPlayer, int a2, int a3)
{
    POSTURE *pPosture = &gPosture[pPlayer->at5f][pPlayer->at2f];
    pPlayer->at67 = pPlayer->pSprite->z-pPosture->at24;
    int t = pPlayer->at6f - pPlayer->at67;
    int dz = pPlayer->at6f-t+(t>>2);
    func_74A18(pPlayer->pSprite, dz, a2, a3);
}

void WeaponInit(void)
{
    for (int i = 0; i < kQAVEnd; i++)
    {
        DICTNODE *hRes = gSysRes.Lookup(i, "QAV");
        if (!hRes)
            ThrowError(513)("Could not load QAV %d\n", i);
        weaponQAV[i] = (QAV*)gSysRes.Lock(hRes);
    }
}

void WeaponDraw(PLAYER *pPlayer, int a2, int a3, int a4, int a5)
{
    dassert(pPlayer != NULL, 529);
    if (pPlayer->at26 == -1)
        return;
    QAV *pQAV = weaponQAV[pPlayer->at26];
    int v4;
    if (pPlayer->atbf == 0)
        v4 = gGameClock%pQAV->at10;
    else
        v4 = pQAV->at10-pPlayer->atbf;
    pQAV->x = a3;
    pQAV->y = a4;
    int flags = 2;
    int nInv = powerupCheck(pPlayer, 13);
    if (nInv >= 120*8 || (nInv != 0 && (gGameClock&32)))
    {
        a2 = -128;
        flags |= 1;
    }
    pQAV->Draw(v4, flags, a2, a5);
}

void WeaponPlay(PLAYER *pPlayer)
{
    dassert(pPlayer != NULL, 561);
    if (pPlayer->at26 == -1)
        return;
    QAV *pQAV = weaponQAV[pPlayer->at26];
    int nTicks = pQAV->at10 - pPlayer->atbf;
    pQAV->pSprite = pPlayer->pSprite;
    pQAV->Play(nTicks-4, nTicks, pPlayer->at2a, pPlayer);
}

static void StartQAV(PLAYER *pPlayer, int nWeaponQAV, int a3 = -1, BOOL a4 = 0)
{
    dassert(nWeaponQAV < kQAVEnd, 578);
    pPlayer->at26 = nWeaponQAV;
    pPlayer->atbf = weaponQAV[nWeaponQAV]->at10;
    pPlayer->at2a = a3;
    pPlayer->at1b1 = a4;
    weaponQAV[nWeaponQAV]->Preload();
    WeaponPlay(pPlayer);
    pPlayer->atbf -= 4;
}

struct t_WeaponModes
{
    int at0;
    int at4;
};

t_WeaponModes weaponModes[] = {
    { 0, -1 },
    { 1, -1 },
    { 1, 1 },
    { 1, 2 },
    { 1, 3 },
    { 1, 4 },
    { 1, 5 },
    { 1, 6 },
    { 1, 7 },
    { 1, 8 },
    { 1, 9 },
    { 1, 10 },
    { 1, 11 },
    { 0, -1 },
};

struct WEAPONTRACK
{
    int at0; // x aim speed
    int at4; // y aim speed
    int at8; // angle range
    int atc;
    int at10; // predict
};

int OrderNext[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 1 };
int OrderPrev[] = { 12, 12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1 };

WEAPONTRACK gWeaponTrack[] = {
    { 0, 0, 0, 0, 0 },
    { 0x6000, 0x6000, 0x71, 0x55, 0x111111 },
    { 0x8000, 0x8000, 0x71, 0x55, 0x2aaaaa },
    { 0x10000, 0x10000, 0x38, 0x1c, 0 },
    { 0x6000, 0x8000, 0x38, 0x1c, 0 },
    { 0x6000, 0x6000, 0x38, 0x1c, 0x2aaaaa },
    { 0x6000, 0x6000, 0x71, 0x55, 0 },
    { 0x6000, 0x6000, 0x71, 0x38, 0 },
    { 0x8000, 0x10000, 0x71, 0x55, 0x255555 },
    { 0x10000, 0x10000, 0x71, 0, 0 },
    { 0x10000, 0x10000, 0xaa, 0, 0 },
    { 0x6000, 0x6000, 0x71, 0x55, 0 },
    { 0x6000, 0x6000, 0x71, 0x55, 0 },
    { 0x6000, 0x6000, 0x71, 0x55, 0 },
};

static void UpdateAimVector(PLAYER * pPlayer)
{
    dassert(pPlayer != NULL, 628);
    SPRITE *pPSprite = pPlayer->pSprite;
    int x = pPSprite->x;
    int y = pPSprite->y;
    int z = pPlayer->at6f;
    VECTOR3D aim;
    aim.dx = Cos(pPSprite->ang)>>16;
    aim.dy = Sin(pPSprite->ang)>>16;
    aim.dz = pPlayer->at83;
    WEAPONTRACK *pWeaponTrack = &gWeaponTrack[pPlayer->atbd];
    int nTarget = -1;
    pPlayer->at1da = 0;
    if (gProfile[pPlayer->at57].at0 || pPlayer->atbd == 10 || pPlayer->atbd == 9)
    {
        int nClosest = 0x7fffffff;
        for (short nSprite = headspritestat[6]; nSprite >= 0; nSprite = nextspritestat[nSprite])
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite == pPSprite)
                continue;
            if (pSprite->flags&kSpriteFlag5)
                continue;
            if (!(pSprite->flags&kSpriteFlag3))
                continue;
            int x2 = pSprite->x;
            int y2 = pSprite->y;
            int z2 = pSprite->z;
            int nDist = approxDist(x2-x, y2-y);
            if (nDist == 0 || nDist > 51200)
                continue;
            if (pWeaponTrack->at10)
            {
                int t = divscale(nDist,pWeaponTrack->at10, 12);
                x2 += (xvel[nSprite]*t)>>12;
                y2 += (yvel[nSprite]*t)>>12;
                z2 += (zvel[nSprite]*t)>>8;
            }
            int lx = x + mulscale30(Cos(pPSprite->ang), nDist);
            int ly = y + mulscale30(Sin(pPSprite->ang), nDist);
            int lz = z + mulscale(pPlayer->at83, nDist, 10);
            int zRange = mulscale(9460, nDist, 10);
            int top, bottom;
            GetSpriteExtents(pSprite, &top, &bottom);
            if (bottom<lz-zRange || top>lz+zRange)
                continue;
            int angle = getangle(x2-x,y2-y);
            if (klabs(((angle-pPSprite->ang+1024)&2047)-1024) > pWeaponTrack->at8)
                continue;
            if (pPlayer->at1da < 16 && cansee(x,y,z,pPSprite->sectnum,x2,y2,z2,pSprite->sectnum))
            {
                pPlayer->at1de[pPlayer->at1da] = nSprite;
                pPlayer->at1da++;
            }
            int nDist2 = Dist3d(lx-x2,ly-y2,lz-z2);
            if (nDist2 >= nClosest)
                continue;
            DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
            int dzCenter = (z2-((pDudeInfo->atf*pSprite->yrepeat)<<2))-z;
            if (cansee(x, y, z, pPSprite->sectnum, x2, y2, z2, pSprite->sectnum))
            {
                nClosest = nDist2;
                aim.dx = Cos(angle)>>16;
                aim.dy = Sin(angle)>>16;
                aim.dz = divscale(dzCenter, nDist, 10);
                nTarget = nSprite;
            }
        }
        if (pWeaponTrack->atc > 0)
        {
            for (nSprite = headspritestat[4]; nSprite >= 0; nSprite = nextspritestat[nSprite])
            {
                SPRITE *pSprite = &sprite[nSprite];
                if (!(pSprite->flags&kSpriteFlag3))
                    continue;
                int x2 = pSprite->x;
                int y2 = pSprite->y;
                int z2 = pSprite->z;
                int dx = x2-x;
                int dy = y2-y;
                int dz = z2-z;
                int nDist = approxDist(dx, dy);
                if (nDist == 0 || nDist > 51200)
                    continue;
                int lx = x + mulscale30(Cos(pPSprite->ang), nDist);
                int ly = y + mulscale30(Sin(pPSprite->ang), nDist);
                int lz = z + mulscale(pPlayer->at83, nDist, 10);
                int zRange = mulscale(9460, nDist, 10);
                int top, bottom;
                GetSpriteExtents(pSprite, &top, &bottom);
                if (bottom<lz-zRange || top>lz+zRange)
                    continue;
                int angle = getangle(dx,dy);
                if (klabs(((angle-pPSprite->ang+1024)&2047)-1024) > pWeaponTrack->atc)
                    continue;
                if (pPlayer->at1da < 16 && cansee(x,y,z,pPSprite->sectnum,pSprite->x,pSprite->y,pSprite->z,pSprite->sectnum))
                {
                    pPlayer->at1de[pPlayer->at1da] = nSprite;
                    pPlayer->at1da++;
                }
                int nDist2 = Dist3d(lx-x2,ly-y2,lz-z2);
                if (nDist2 >= nClosest)
                    continue;
                if (cansee(x, y, z, pPSprite->sectnum, pSprite->x, pSprite->y, pSprite->z, pSprite->sectnum))
                {
                    nClosest = nDist2;
                    aim.dx = Cos(angle)>>16;
                    aim.dy = Sin(angle)>>16;
                    aim.dz = divscale(dz, nDist, 10);
                    nTarget = nSprite;
                }
            }
        }
    }
    VECTOR3D aim2 = aim;
    RotateVector(&aim2.dx, &aim2.dy, -pPSprite->ang);
    aim2.dz -= pPlayer->at83;
    pPlayer->at1ca.dx = interpolate16(pPlayer->at1ca.dx, aim2.dx, pWeaponTrack->at0);
    pPlayer->at1ca.dy = interpolate16(pPlayer->at1ca.dy, aim2.dy, pWeaponTrack->at0);
    pPlayer->at1ca.dz = interpolate16(pPlayer->at1ca.dz, aim2.dz, pWeaponTrack->at4);
    pPlayer->at1be = pPlayer->at1ca;
    RotateVector(&pPlayer->at1be.dx, &pPlayer->at1be.dy, pPSprite->ang);
    pPlayer->at1be.dz += pPlayer->at83;
    pPlayer->at1d6 = nTarget;
}

void WeaponRaise(PLAYER *pPlayer)
{
    dassert(pPlayer != 0, 825);
    int prevWeapon = pPlayer->atbd;
    pPlayer->atbd = pPlayer->atc.newWeapon;
    pPlayer->atc.newWeapon = 0;
    pPlayer->atc7 = weaponModes[pPlayer->atbd].at4;
    switch (pPlayer->atbd)
    {
    case 1: // pitchfork
        pPlayer->atc3 = 0;
        StartQAV(pPlayer, 0);
        break;
    case 7: // spraycan
        if (pPlayer->atc3 == 2)
        {
            pPlayer->atc3 = 3;
            StartQAV(pPlayer, 8);
        }
        else
        {
            pPlayer->atc3 = 0;
            StartQAV(pPlayer, 4);
        }
        break;
    case 6: // dynamite
        if (gInfiniteAmmo || func_4B2C8(pPlayer, 5))
        {
            pPlayer->atc3 = 3;
            if (prevWeapon == 7)
                StartQAV(pPlayer, 16);
            else
                StartQAV(pPlayer, 18);
        }
        break;
    case 11: // proximity
        if (gInfiniteAmmo || func_4B2C8(pPlayer, 10))
        {
            pPlayer->atc3 = 7;
            StartQAV(pPlayer, 25);
        }
        break;
    case 12: // remote
        if (gInfiniteAmmo || func_4B2C8(pPlayer, 11))
        {
            pPlayer->atc3 = 10;
            StartQAV(pPlayer, 31);
        }
        else
        {
            StartQAV(pPlayer, 32);
            pPlayer->atc3 = 11;
        }
        break;
    case 3: // sawed off
        if (powerupCheck(pPlayer, 17))
        {
            if (gInfiniteAmmo || pPlayer->at181[2] >= 4)
                StartQAV(pPlayer, 59);
            else
                StartQAV(pPlayer, 50);
            if (gInfiniteAmmo || pPlayer->at181[2] >= 4)
                pPlayer->atc3 = 7;
            else if (pPlayer->at181[2] > 1)
                pPlayer->atc3 = 3;
            else if (pPlayer->at181[2] > 0)
                pPlayer->atc3 = 2;
            else
                pPlayer->atc3 = 1;
        }
        else
        {
            if (gInfiniteAmmo || pPlayer->at181[2] > 1)
                pPlayer->atc3 = 3;
            else if (pPlayer->at181[2] > 0)
                pPlayer->atc3 = 2;
            else
                pPlayer->atc3 = 1;
            StartQAV(pPlayer, 50);
        }
        break;
    case 4: // tommy gun
        if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 3, 2))
        {
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 69);
        }
        else
        {
            pPlayer->atc3 = 0;
            StartQAV(pPlayer, 64);
        }
        break;
    case 10: // voodoo
        if (gInfiniteAmmo || func_4B2C8(pPlayer, 9))
        {
            pPlayer->atc3 = 2;
            StartQAV(pPlayer, 100);
        }
        break;
    case 2: // flaregun
        if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 1, 2))
        {
            StartQAV(pPlayer, 45);
            pPlayer->atc3 = 3;
        }
        else
        {
            StartQAV(pPlayer, 41);
            pPlayer->atc3 = 2;
        }
        break;
    case 8: // tesla cannon
        if (func_4B2C8(pPlayer, 7))
        {
            pPlayer->atc3 = 2;
            if (powerupCheck(pPlayer, 17))
                StartQAV(pPlayer, 82);
            else
                StartQAV(pPlayer, 74);
        }
        else
        {
            pPlayer->atc3 = 3;
            StartQAV(pPlayer, 74);
        }
        break;
    case 5: // napalm
        if (powerupCheck(pPlayer, 17))
        {
            StartQAV(pPlayer, 120);
            pPlayer->atc3 = 3;
        }
        else
        {
            StartQAV(pPlayer, 89);
            pPlayer->atc3 = 2;
        }
        break;
    case 9: // life leech
        pPlayer->atc3 = 2;
        StartQAV(pPlayer, 111);
        break;
    case 13: // beast
        pPlayer->atc3 = 2;
        StartQAV(pPlayer, 93);
        break;
    }
}

void WeaponLower(PLAYER *pPlayer)
{
    dassert(pPlayer != 0, 1009);
    if (func_4B1A4(pPlayer))
        return;
    pPlayer->at1ba = 0;
    int t = pPlayer->atbd;
    int vc = pPlayer->atc3;
    switch (t)
    {
    case 1:
        StartQAV(pPlayer, 3);
        break;
    case 7:
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
        switch (vc)
        {
        case 1:
            StartQAV(pPlayer, 7);
            break;
        case 2:
            pPlayer->atc3 = 1;
            WeaponRaise(pPlayer);
            return;
        case 4:
_goto1:
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 11);
            pPlayer->atc.newWeapon = 0;
            WeaponLower(pPlayer);
            break;
        case 3:
            if (pPlayer->atc.newWeapon == 6)
            {
                pPlayer->atc3 = 2;
                StartQAV(pPlayer, 11);
                return;
            }
            else if (pPlayer->atc.newWeapon == 7)
            {
                goto _goto1;
            }
            else
            {
                pPlayer->atc3 = 1;
                StartQAV(pPlayer, 11);
            }
            break;
        }
        break;
    case 6:
        switch (vc)
        {
        case 1:
            StartQAV(pPlayer, 7);
            break;
        case 2:
            WeaponRaise(pPlayer);
            break;
        case 3:
            if (pPlayer->atc.newWeapon == 7)
            {
                pPlayer->atc3 = 2;
                StartQAV(pPlayer, 17);
            }
            else
            {
                StartQAV(pPlayer, 19);
            }
            break;
        default:
            break;
        }
        break;
    case 11:
        switch (vc)
        {
        case 7:
            StartQAV(pPlayer, 26);
            break;
        }
        break;
    case 12:
        switch (vc)
        {
        case 10:
            StartQAV(pPlayer, 34);
            break;
        case 11:
            StartQAV(pPlayer, 35);
            break;
        }
        break;
    case 3:
        if (powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 63);
        else
            StartQAV(pPlayer, 58);
        break;
    case 4:
        if (powerupCheck(pPlayer, 17) && pPlayer->atc3 == 1)
            StartQAV(pPlayer, 72);
        else
            StartQAV(pPlayer, 68);
        break;
    case 2:
        if (powerupCheck(pPlayer, 17) && vc == 3)
            StartQAV(pPlayer, 49);
        else
            StartQAV(pPlayer, 44);
        break;
    case 10:
        StartQAV(pPlayer, 109);
        break;
    case 8:
        if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 88);
        else
            StartQAV(pPlayer, 81);
        break;
    case 5:
        if (powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 124);
        else
            StartQAV(pPlayer, 92);
        break;
    case 9:
        StartQAV(pPlayer, 119);
        break;
    case 13:
        StartQAV(pPlayer, 99);
        break;
    }
    pPlayer->atbd = 0;
    pPlayer->at1b1 = 0;
}

void WeaponUpdateState(PLAYER *pPlayer)
{
    static int lastWeapon = 0;
    static int lastState = 0;
    XSPRITE *pXSprite = pPlayer->pXSprite;
    int va = pPlayer->atbd;
    int vb = pPlayer->atc3;
    if (va != lastWeapon || vb != lastState)
    {
        lastWeapon = va;
        lastState = vb;
    }
    switch (va)
    {
    case 1:
        pPlayer->at26 = 1;
        break;
    case 7:
        switch (vb)
        {
        case 0:
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 5);
            break;
        case 1:
            if (CheckAmmo(pPlayer, 6))
            {
                pPlayer->atc3 = 3;
                StartQAV(pPlayer, 8);
            }
            else
                pPlayer->at26 = 6;
            break;
        case 3:
            pPlayer->at26 = 9;
            break;
        case 4:
            if (CheckAmmo(pPlayer, 6))
            {
                pPlayer->at26 = 9;
                pPlayer->atc3 = 3;
            }
            else
            {
                pPlayer->atc3 = 1;
                StartQAV(pPlayer, 11);
            }
            sfxKill3DSound(pPlayer->pSprite, -1, 441);
            break;
        }
        break;
    case 6:
        switch (vb)
        {
        case 1:
            if (pPlayer->atc7 == 5 && CheckAmmo(pPlayer, 5))
            {
                pPlayer->atc3 = 3;
                StartQAV(pPlayer, 16);
            }
            break;
        case 0:
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 5);
            break;
        case 2:
            if (pPlayer->at181[5] > 0)
            {
                pPlayer->atc3 = 3;
                StartQAV(pPlayer, 16);
            }
            else
                pPlayer->at26 = 6;
            break;
        case 3:
            pPlayer->at26 = 20;
            break;
        }
        break;
    case 11:
        switch (vb)
        {
        case 7:
            pPlayer->at26 = 27;
            break;
        case 8:
            pPlayer->atc3 = 7;
            StartQAV(pPlayer, 25);
            break;
        }
        break;
    case 12:
        switch (vb)
        {
        case 10:
            pPlayer->at26 = 36;
            break;
        case 11:
            pPlayer->at26 = 37;
            break;
        case 12:
            if (pPlayer->at181[11] > 0)
            {
                pPlayer->atc3 = 10;
                StartQAV(pPlayer, 31);
            }
            else
                pPlayer->atc3 = -1;
            break;
        }
        break;
    case 3:
        switch (vb)
        {
        case 6:
            if (powerupCheck(pPlayer, 17) && (gInfiniteAmmo || CheckAmmo(pPlayer, 2, 4)))
                pPlayer->atc3 = 7;
            else
                pPlayer->atc3 = 1;
            break;
        case 7:
            pPlayer->at26 = 60;
            break;
        case 1:
            if (CheckAmmo(pPlayer, 2))
            {
                sfxPlay3DSound(pPlayer->pSprite, 410, 3, 2);
                StartQAV(pPlayer, 57, nClientEjectShell);
                if (gInfiniteAmmo || pPlayer->at181[2] > 1)
                    pPlayer->atc3 = 3;
                else
                    pPlayer->atc3 = 2;
            }
            else
                pPlayer->at26 = 51;
            break;
        case 2:
            pPlayer->at26 = 52;
            break;
        case 3:
            pPlayer->at26 = 53;
            break;
        }
        break;
    case 4:
        if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 3, 2))
        {
            pPlayer->at26 = 70;
            pPlayer->atc3 = 1;
        }
        else
        {
            pPlayer->at26 = 65;
            pPlayer->atc3 = 0;
        }
        break;
    case 2:
        if (powerupCheck(pPlayer, 17))
        {
            if (vb == 3 && func_4B2C8(pPlayer, 1, 2))
                pPlayer->at26 = 46;
            else
            {
                pPlayer->at26 = 42;
                pPlayer->atc3 = 2;
            }
        }
        else
            pPlayer->at26 = 42;
        break;
    case 10:
        if (pXSprite->at30_0 < 256 && klabs(pPlayer->at4f) > 768)
            pPlayer->at26 = 102;
        else
            pPlayer->at26 = 101;
        break;
    case 8:
        switch (vb)
        {
        case 2:
            if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
                pPlayer->at26 = 83;
            else
                pPlayer->at26 = 75;
            break;
        case 3:
            pPlayer->at26 = 76;
            break;
        }
        break;
    case 5:
        switch (vb)
        {
        case 3:
            if (powerupCheck(pPlayer, 17) && (gInfiniteAmmo || CheckAmmo(pPlayer,4, 4)))
                pPlayer->at26 = 121;
            else
                pPlayer->at26 = 90;
            break;
        case 2:
            pPlayer->at26 = 90;
            break;
        }
        break;
    case 9:
        switch (vb)
        {
        case 2:
            pPlayer->at26 = 112;
            break;
        }
        break;
    case 13:
        pPlayer->at26 = 94;
        break;
    }
}

static void FirePitchfork(int, PLAYER *pPlayer)
{
    VECTOR3D *aim = &pPlayer->at1be;
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(2000);
    for (int i = 0; i < 4; i++)
        actFireVector(pPlayer->pSprite, (2*i-3)*40, pPlayer->at6f-pPlayer->pSprite->z, aim->dx+r1, aim->dy+r2, aim->dz+r3, kVectorPitchfork);
}

static void FireSpray(int, PLAYER *pPlayer)
{
    playerFireMissile(pPlayer, 0, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 304);
    UseAmmo(pPlayer, 6, 4);
    if (CheckAmmo(pPlayer, 6))
        sfxPlay3DSound(pPlayer->pSprite, 441, 1, 2);
    else
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
}

static void ThrowCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    int nSpeed = mulscale16(pPlayer->at1ba, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, -9460, 420, nSpeed);
    if (pSprite)
    {
        sfxPlay3DSound(pSprite, 441, 0);
        pSprite->shade = -128;
        evPost(pSprite->index, 3, pPlayer->at1b2, COMMAND_ID_1);
        int nXSprite = pSprite->extra;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        pXSprite->ate_0 = 1;
        UseAmmo(pPlayer, 6, gAmmoItemData[0].at8);
        pPlayer->at1ba = 0;
    }
}

static void DropCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 420, 0);
    if (pSprite)
    {
        evPost(pSprite->index, 3, pPlayer->at1b2, COMMAND_ID_1);
        UseAmmo(pPlayer, 6, gAmmoItemData[0].at8);
    }
}

static void ExplodeCan(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, -1, 441);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 420, 0);
    evPost(pSprite->index, 3, 0, COMMAND_ID_1);
    UseAmmo(pPlayer, 6, gAmmoItemData[0].at8);
    StartQAV(pPlayer, 15, -1);
    pPlayer->atbd = 0;
    pPlayer->at1ba = 0;
}

static void ThrowBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16);
    int nSpeed = mulscale16(pPlayer->at1ba, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, -9460, 419, nSpeed);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    if (pPlayer->at1b2 < 0)
        pXSprite->ate_0 = 1;
    else
        evPost(pSprite->index, 3, pPlayer->at1b2, COMMAND_ID_1);
    UseAmmo(pPlayer, 5, 1);
    pPlayer->at1ba = 0;
}

static void DropBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 419, 0);
    evPost(pSprite->index, 3, pPlayer->at1b2, COMMAND_ID_1);
    UseAmmo(pPlayer, 5, 1);
}

static void ExplodeBundle(int, PLAYER *pPlayer)
{
    sfxKill3DSound(pPlayer->pSprite, 16);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 419, 0);
    evPost(pSprite->index, 3, 0, COMMAND_ID_1);
    UseAmmo(pPlayer, 5, 1);
    StartQAV(pPlayer, 24);
    pPlayer->atbd = 0;
    pPlayer->at1ba = 0;
}

static void ThrowProx(int, PLAYER *pPlayer)
{
    int nSpeed = mulscale16(pPlayer->at1ba, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, -9460, 401, nSpeed);
    evPost(pSprite->index, 3, 240, COMMAND_ID_1);
    UseAmmo(pPlayer, 10, 1);
    pPlayer->at1ba = 0;
}

static void DropProx(int, PLAYER *pPlayer)
{
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 401, 0);
    evPost(pSprite->index, 3, 240, COMMAND_ID_1);
    UseAmmo(pPlayer, 10, 1);
}

static void ThrowRemote(int, PLAYER *pPlayer)
{
    int nSpeed = mulscale16(pPlayer->at1ba, 0x177777)+0x66666;
    sfxPlay3DSound(pPlayer->pSprite, 455, 1, 0);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, -9460, 402, nSpeed);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    pXSprite->at5_2 = 90+(pPlayer->pSprite->type-kDudePlayer1);
    UseAmmo(pPlayer, 11, 1);
    pPlayer->at1ba = 0;
}

static void DropRemote(int, PLAYER *pPlayer)
{
    SPRITE *pSprite = playerFireThing(pPlayer, 0, 0, 402, 0);
    int nXSprite = pSprite->extra;
    XSPRITE *pXSprite = &xsprite[nXSprite];
    pXSprite->at5_2 = 90+(pPlayer->pSprite->type-kDudePlayer1);
    UseAmmo(pPlayer, 11, 1);
}

static void FireRemote(int, PLAYER *pPlayer)
{
    evSend(0, 0, 90+(pPlayer->pSprite->type-kDudePlayer1), COMMAND_ID_1);
}

#define kMaxShotgunBarrels 4

static void FireShotgun(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxShotgunBarrels, 1640);
    SPRITE *pSprite = pPlayer->pSprite;
    if (nTrigger == 1)
    {
        sfxPlay3DSound(pSprite, 411, 2);
        pPlayer->at35e = 30;
        pPlayer->at362 = 20;
    }
    else
    {
        sfxPlay3DSound(pSprite, 412, 2);
        pPlayer->at35e = 50;
        pPlayer->at362 = 40;
    }
    int n = nTrigger<<4;
    for (int i = 0; i < n; i++)
    {
        VECTOR_TYPE nType;
        int r1, r2, r3;
        if (nTrigger == 1)
        {
            r1 = Random3(1500);
            r2 = Random3(1500);
            r3 = Random3(500);
            nType = kVectorShot;
        }
        else
        {
            r1 = Random3(2500);
            r2 = Random3(2500);
            r3 = Random3(1500);
            nType = kVectorShotgun;
        }
        actFireVector(pPlayer->pSprite, 0, pPlayer->at6f-pPlayer->pSprite->z, pPlayer->at1be.dx+r1, pPlayer->at1be.dy+r2, pPlayer->at1be.dz+r3, nType);
    }
    UseAmmo(pPlayer, pPlayer->atc7, nTrigger);
    pPlayer->at37b = 1;
}

static void EjectShell(int, PLAYER *pPlayer)
{
    SpawnShellEject(pPlayer, 25, 35);
    SpawnShellEject(pPlayer, 48, 35);
}

static void FireTommy(int nTrigger, PLAYER *pPlayer)
{
    VECTOR3D *aim = &pPlayer->at1be;
    SPRITE *pSprite = pPlayer->pSprite;
    sfxPlay3DSound(pSprite, 431);
    switch (nTrigger)
    {
    case 1:
    {
        actFireVector(pPlayer->pSprite, 0, pPlayer->at6f-pPlayer->pSprite->z, aim->dx+Random3(1200), aim->dy+Random3(1200), aim->dz+Random3(400), kVectorTommy);
        SpawnBulletEject(pPlayer, -15, -45);
        pPlayer->at362 = 20;
        break;
    }
    case 2:
    {
        actFireVector(pPlayer->pSprite, -120, pPlayer->at6f-pPlayer->pSprite->z, aim->dx+Random3(1200), aim->dy+Random3(1200), aim->dz+Random3(400), kVectorTommy);
        SpawnBulletEject(pPlayer, -140, -45);
        actFireVector(pPlayer->pSprite, 120, pPlayer->at6f-pPlayer->pSprite->z, aim->dx+Random3(1200), aim->dy+Random3(1200), aim->dz+Random3(400), kVectorTommy);
        SpawnBulletEject(pPlayer, 140, 45);
        pPlayer->at362 = 30;
        break;
    }
    }
    UseAmmo(pPlayer, pPlayer->atc7, nTrigger);
    pPlayer->at37b = 1;
}

#define kMaxSpread 14

static void FireSpread(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread, 1734);
    VECTOR3D *aim = &pPlayer->at1be;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    SPRITE *pSprite = pPlayer->pSprite;
    sfxPlay3DSound(pSprite, 431);
    actFireVector(pPlayer->pSprite, 0, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
    SpawnBulletEject(pPlayer, Random2(30), Random2(90));
    pPlayer->at362 = 20;
    UseAmmo(pPlayer, pPlayer->atc7, 1);
    pPlayer->at37b = 1;
}

static void AltFireSpread(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread, 1760);
    VECTOR3D *aim = &pPlayer->at1be;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    SPRITE *pSprite = pPlayer->pSprite;
    sfxPlay3DSound(pSprite, 431);
    actFireVector(pPlayer->pSprite, -120, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
    SpawnBulletEject(pPlayer, Random2(120), Random2(45));
    actFireVector(pPlayer->pSprite, 120, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
    SpawnBulletEject(pPlayer, Random2(-120), Random2(-45));
    pPlayer->at35e = 20;
    pPlayer->at362 = 30;
    UseAmmo(pPlayer, pPlayer->atc7, 2);
    pPlayer->at37b = 1;
}

static void AltFireSpread2(int nTrigger, PLAYER *pPlayer)
{
    dassert(nTrigger > 0 && nTrigger <= kMaxSpread, 1792);
    VECTOR3D *aim = &pPlayer->at1be;
    int angle = (getangle(aim->dx, aim->dy)+((112*(nTrigger-1))/14-56))&2047;
    int dx = Cos(angle)>>16;
    int dy = Sin(angle)>>16;
    SPRITE *pSprite = pPlayer->pSprite;
    sfxPlay3DSound(pSprite, 431);
    if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 3, 2))
    {
        actFireVector(pPlayer->pSprite, -120, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
        SpawnBulletEject(pPlayer, Random2(120), Random2(45));
        actFireVector(pPlayer->pSprite, 120, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
        SpawnBulletEject(pPlayer, Random2(-120), Random2(-45));
        pPlayer->at35e = 30;
        pPlayer->at362 = 45;
        UseAmmo(pPlayer, pPlayer->atc7, 2);
    }
    else
    {
        actFireVector(pPlayer->pSprite, 0, pPlayer->at6f-pPlayer->pSprite->z, dx+Random3(600), dy+Random3(600), aim->dz+Random3(300), kVectorTommySpread);
        SpawnBulletEject(pPlayer, Random2(30), Random2(90));
        pPlayer->at35e = 20;
        pPlayer->at362 = 30;
        UseAmmo(pPlayer, pPlayer->atc7, 1);
    }
    pPlayer->at37b = 1;
    if (!func_4B2C8(pPlayer, 3))
    {
        WeaponLower(pPlayer);
        pPlayer->atc3 = -1;
    }
}

static void FireFlare(int nTrigger, PLAYER *pPlayer)
{
    SPRITE *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -120;
        break;
    case 3:
        offset = 120;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 301);
    UseAmmo(pPlayer, 1, 1);
    sfxPlay3DSound(pSprite, 420, 2);
    pPlayer->at362 = 30;
    pPlayer->at37b = 1;
}

static void AltFireFlare(int nTrigger, PLAYER *pPlayer)
{
    SPRITE *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -120;
        break;
    case 3:
        offset = 120;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 303);
    UseAmmo(pPlayer, 1, 8);
    sfxPlay3DSound(pSprite, 420, 2);
    pPlayer->at362 = 45;
    pPlayer->at37b = 1;
}

static void FireVoodoo(int nTrigger, PLAYER *pPlayer)
{
    nTrigger--;
    int nSprite = pPlayer->at5b;
    SPRITE *pSprite = pPlayer->pSprite;
    if (nTrigger == 4)
    {
        actDamageSprite(nSprite, pSprite, kDamageBullet, 1<<4);
        return;
    }
    dassert(pPlayer->playerVoodooTarget >= 0, 1916);
    SPRITE *pTarget = &sprite[pPlayer->playerVoodooTarget];
    switch (nTrigger)
    {
    case 0:
    {
        sfxPlay3DSound(pSprite, 460, 2);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, kDamageSpirit, 17<<4);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 1:
    {
        sfxPlay3DSound(pSprite, 460, 2);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, kDamageSpirit, 9<<4);
        if (pTarget->type >= kDudePlayer1 && pTarget->type <= kDudePlayer8)
            WeaponLower(&gPlayer[pTarget->type-kDudePlayer1]);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 3:
    {
        sfxPlay3DSound(pSprite, 463, 2);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, kDamageSpirit, 49<<4);
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    case 2:
    {
        sfxPlay3DSound(pSprite, 460, 2);
        fxSpawnBlood(pTarget, 17<<4);
        int nDamage = actDamageSprite(nSprite, pTarget, kDamageSpirit, 11<<4);
        if (pTarget->type >= kDudePlayer1 && pTarget->type <= kDudePlayer8)
        {
            PLAYER *pOtherPlayer = &gPlayer[pTarget->type-kDudePlayer1];
            pOtherPlayer->at36a = 128;
        }
        UseAmmo(pPlayer, 9, nDamage/4);
        break;
    }
    }
}

static void AltFireVoodoo(int nTrigger, PLAYER *pPlayer)
{
    if (nTrigger != 2)
        return;
    int nAmmo = pPlayer->at181[9];
    int nCount = nAmmo < pPlayer->at1da ? nAmmo : pPlayer->at1da;
    if (nCount > 0)
    {
        int v4 = nAmmo - (nAmmo / nCount)*nCount;
        for (int i = 0; i < pPlayer->at1da; i++)
        {
            int nTarget = pPlayer->at1de[i];
            SPRITE *pTarget = &sprite[nTarget];
            if (v4 > 0)
                v4--;
            SPRITE *pSprite = pPlayer->pSprite;
            int dx = pTarget->x - pSprite->x;
            int dy = pTarget->y - pSprite->y;
            int nDist = approxDist(dx, dy);
            if (nDist > 0 && nDist < 51200)
            {
                int t = pPlayer->at181[9];
                int nDamage = ((t<<1)+Random2(t>>3))<<4;
                nDamage = actDamageSprite(pPlayer->at5b, pTarget, kDamageSpirit, (nDamage*((51200-nDist)+1))/51200);
                UseAmmo(pPlayer, 9, nDamage);
                if (pTarget->type >= kDudePlayer1 && pTarget->type <= kDudePlayer8)
                {
                    PLAYER *pOtherPlayer = &gPlayer[pTarget->type-kDudePlayer1];
                    if (!pOtherPlayer->at31a || !powerupCheck(pOtherPlayer,14))
                        powerupActivate(pOtherPlayer, 28);
                }
                fxSpawnBlood(pTarget, 0);
            }
        }
    }
    UseAmmo(pPlayer, 9, pPlayer->at181[9]);
    pPlayer->atcb[10] = 0;
    pPlayer->atc3 = -1;
}

static void DropVoodoo(int nTrigger, PLAYER *pPlayer)
{
    if (nTrigger != 2)
        return;
    sfxPlay3DSound(pPlayer->pSprite, 455, 2);
    SPRITE *pSprite = playerFireThing(pPlayer, 0, -4730, 432, 0xccccc);
    if (pSprite)
    {
        int nXSprite = pSprite->extra;
        XSPRITE *pXSprite = &xsprite[nXSprite];
        pXSprite->at10_0 = pPlayer->at181[9];
        evPost(pSprite->index, 3, 90, CALLBACK_ID_21);
        UseAmmo(pPlayer, 9, pPlayer->at181[9]);
        pPlayer->atcb[10] = 0;
    }
}

struct TeslaMissile
{
    int at0; // offset
    int at4; // id
    int at8; // ammo use
    int atc; // sound
    int at10; // light
    int at14; // weapon flash
};

static void FireTesla(int nTrigger, PLAYER *pPlayer)
{
    TeslaMissile teslaMissile[6] = 
    {
        { 0, 306, 1, 470, 20, 1 },
        { -140, 306, 1, 470, 30, 1 },
        { 140, 306, 1, 470, 30, 1 },
        { 0, 302, 35, 471, 40, 1 },
        { -140, 302, 35, 471, 50, 1 },
        { 140, 302, 35, 471, 50, 1 },
    };
    if (nTrigger > 0 && nTrigger <= 6)
    {
        nTrigger--;
        SPRITE *pSprite = pPlayer->pSprite;
        TeslaMissile *pMissile = &teslaMissile[nTrigger];
        if (!func_4B2C8(pPlayer, 7, pMissile->at8))
        {
            pMissile = &teslaMissile[0];
            if (!func_4B2C8(pPlayer, 7, pMissile->at8))
            {
                pPlayer->atc3 = -1;
                pPlayer->at26 = 76;
                pPlayer->at37b = 0;
                return;
            }
        }
        playerFireMissile(pPlayer, pMissile->at0, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, pMissile->at4);
        UseAmmo(pPlayer, pPlayer->atc7, pMissile->at8);
        sfxPlay3DSound(pSprite, pMissile->atc, 1);
        pPlayer->at362 = pMissile->at10;
        pPlayer->at37b = pMissile->at14;
    }
}

static void AltFireTesla(int nTrigger, PLAYER *pPlayer)
{
    SPRITE *pSprite = pPlayer->pSprite;
    playerFireMissile(pPlayer, 0, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 302);
    UseAmmo(pPlayer, pPlayer->atc7, 35);
    sfxPlay3DSound(pSprite, 471, 2);
    pPlayer->at362 = 40;
    pPlayer->at37b = 1;
}

static void FireNapalm(int nTrigger, PLAYER *pPlayer)
{
    SPRITE *pSprite = pPlayer->pSprite;
    int offset = 0;
    switch (nTrigger)
    {
    case 2:
        offset = -50;
        break;
    case 3:
        offset = 50;
        break;
    }
    playerFireMissile(pPlayer, offset, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 312);
    sfxPlay3DSound(pSprite, 480, 2);
    UseAmmo(pPlayer, 4, 1);
    pPlayer->at37b = 1;
}

static void FireNapalm2(int nTrigger, PLAYER *pPlayer)
{
    SPRITE *pSprite = pPlayer->pSprite;
    playerFireMissile(pPlayer, -120, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 312);
    playerFireMissile(pPlayer, 120, pPlayer->at1be.dx, pPlayer->at1be.dy, pPlayer->at1be.dz, 312);
    sfxPlay3DSound(pSprite, 480, 2);
    UseAmmo(pPlayer, 4, 2);
    pPlayer->at37b = 1;
}

static void AltFireNapalm(int nTrigger, PLAYER *pPlayer)
{
    char bAkimbo = powerupCheck(pPlayer, 17);
    int nSpeed = mulscale16(0x8000, 0x177777)+0x66666;
    SPRITE *pMissile = playerFireThing(pPlayer, 0, -4730, 428, nSpeed);
    if (pMissile)
    {
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pXSprite->at18_2 = ClipHigh(pPlayer->at181[4], 12);
        UseAmmo(pPlayer, 4, pXSprite->at18_2);
        seqSpawn(22, 3, pMissile->extra);
        actBurnSprite(actSpriteIdToOwnerId(pPlayer->pSprite->index), pXSprite, 600);
        evPost(pMissile->index, 3, 0, CALLBACK_ID_0);
        SPRITE *pSprite = pPlayer->pSprite;
        sfxPlay3DSound(pSprite, 480, 2);
        pPlayer->at362 = 30;
        pPlayer->at37b = 1;
    }
}

static void FireLifeLeech(int nTrigger, PLAYER *pPlayer)
{
    if (!CheckAmmo(pPlayer, 8))
        return;
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(1000);
    SPRITE *pMissile = playerFireMissile(pPlayer, 0, pPlayer->at1be.dx+r1, pPlayer->at1be.dy+r2, pPlayer->at1be.dz+r3, 315);
    if (pMissile)
    {
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pXSprite->target = pPlayer->at1d6;
        pMissile->ang = (nTrigger==2) ? 1024 : 0;
    }
    if (func_4B2C8(pPlayer, 8))
        UseAmmo(pPlayer, 8, 1);
    else
        actDamageSprite(pPlayer->at5b, pPlayer->pSprite, kDamageSpirit, 16);
    pPlayer->at362 = ClipHigh(pPlayer->at362+5, 50);
}

static void AltFireLifeLeech(int nTrigger, PLAYER *pPlayer)
{
    sfxPlay3DSound(pPlayer->pSprite, 455, 2);
    SPRITE *pMissile = playerFireThing(pPlayer, 0, -4730, 431, 0x19999);
    if (pMissile)
    {
        XSPRITE *pXSprite = &xsprite[pMissile->extra];
        pMissile->cstat |= 4096;
        pXSprite->atd_6 = 1;
        pXSprite->ate_4 = 1;
        pXSprite->atf_7 = 1;
        pXSprite->at32_0 = 1;
        evPost(pMissile->index, 3, 120, CALLBACK_ID_20);
        if (gGameOptions.nGameType <= GAMETYPE_1)
        {
            int nAmmo = pPlayer->at181[8];
            if (nAmmo < 25 && pPlayer->pXSprite->health > ((25-nAmmo)<<4))
            {
                actDamageSprite(pPlayer->at5b, pPlayer->pSprite, kDamageSpirit, ((25-nAmmo)<<4));
                nAmmo = 25;
            }
            pXSprite->at14_0 = nAmmo;
            UseAmmo(pPlayer, 8, nAmmo);
        }
        else
        {
            pXSprite->at14_0 = pPlayer->at181[8];
            pPlayer->at181[8] = 0;
        }
        pPlayer->atcb[9] = 0;
    }
}

static void FireBeast(int nTrigger, PLAYER * pPlayer)
{
    int r1 = Random2(2000);
    int r2 = Random2(2000);
    int r3 = Random2(2000);
    actFireVector(pPlayer->pSprite, 0, pPlayer->at6f-pPlayer->pSprite->z, pPlayer->at1be.dx+r1, pPlayer->at1be.dy+r2, pPlayer->at1be.dz+r3, kVectorFireBeast);
}

BOOL gWeaponUpgrade[][13] = {
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
};

byte WeaponUpgrade(PLAYER *pPlayer, BOOL newWeapon)
{
    char weapon = pPlayer->atbd;
    if (!func_4B1A4(pPlayer) && gWeaponUpgrade[pPlayer->atbd][newWeapon])
        return newWeapon;
    return weapon;
}

byte WeaponFindNext(PLAYER *pPlayer, int *a2, BOOL bDir)
{
    int weapon = pPlayer->atbd;
    do
    {
        weapon = bDir ? OrderNext[weapon] : OrderPrev[weapon];
        if (weaponModes[weapon].at0 && pPlayer->atcb[weapon])
        {
            int t = weaponModes[weapon].at4;
            if (weapon == 9)
            {
                if (CheckAmmo(pPlayer, t))
                    break;
            }
            else
            {
                if (func_4B2C8(pPlayer, t))
                    break;
            }
        }
    } while (pPlayer->atbd != weapon);
    if (pPlayer->atbd == weapon)
    {
        if (!weaponModes[weapon].at0 || !CheckAmmo(pPlayer, weaponModes[weapon].at4))
            weapon = 1;
    }
    if (a2)
        *a2 = 0;
    return weapon;
}

byte WeaponFindLoaded(PLAYER *pPlayer, int *a2)
{
    char v4 = 1;
    int v14 = 0;
    if (weaponModes[pPlayer->atbd].at0 > 1)
    {
        for (int i = 0; i < weaponModes[pPlayer->atbd].at0; i++)
        {
            int t = weaponModes[pPlayer->atbd].at4;
            if (CheckAmmo(pPlayer, t))
            {
                v4 = pPlayer->atbd;
                v14 = i;
                break;
            }
        }
    }
    if (v4 == 1)
    {
        int vc = 0;
        for (int i = 0; i < 14; i++)
        {
            int weapon = pPlayer->at111[vc][i];
            if (pPlayer->atcb[weapon])
            {
                for (int j = 0; j < weaponModes[weapon].at0; j++)
                {
                    int t = weaponModes[weapon].at4;
                    if (func_4B1FC(pPlayer, weapon, t))
                    {
                        v4 = weapon;
                        v14 = j;
                        if (a2)
                            *a2 = v14;
                        return v4;
                    }
                }
            }
        }
    }
    if (a2)
        *a2 = v14;
    return v4;
}

BOOL func_4F0E0(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 5:
        if (!pPlayer->atc.buttonFlags.shoot2)
            pPlayer->atc3 = 6;
        return 1;
    case 6:
        if (pPlayer->atc.buttonFlags.shoot2)
        {
            pPlayer->atc3 = 3;
            pPlayer->at1b2 = pPlayer->atbf;
            StartQAV(pPlayer, 13, nClientDropCan);
            return 1;
        }
        else if (pPlayer->atc.buttonFlags.shoot)
        {
            pPlayer->atc3 = 7;
            pPlayer->at1b2 = 0;
            pPlayer->at1b6 = gFrameClock;
        }
        return 1;
    case 7:
    {
        pPlayer->at1ba = ClipHigh(divscale16(gFrameClock-pPlayer->at1b6,240), 65536);
        if (!pPlayer->atc.buttonFlags.shoot)
        {
            if (!pPlayer->at1b2)
                pPlayer->at1b2 = pPlayer->atbf;
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 14, nClientThrowCan);
        }
        return 1;
    }
    }
    return 0;
}

BOOL func_4F200(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 4:
        if (!pPlayer->atc.buttonFlags.shoot2)
            pPlayer->atc3 = 5;
        return 1;
    case 5:
        if (pPlayer->atc.buttonFlags.shoot2)
        {
            pPlayer->atc3 = 1;
            pPlayer->at1b2 = pPlayer->atbf;
            StartQAV(pPlayer, 22, nClientDropBundle, 0);
            return 1;
        }
        else if (pPlayer->atc.buttonFlags.shoot)
        {
            pPlayer->atc3 = 6;
            pPlayer->at1b2 = 0;
            pPlayer->at1b6 = gFrameClock;
        }
        return 1;
    case 6:
    {
        pPlayer->at1ba = ClipHigh(divscale16(gFrameClock-pPlayer->at1b6,240), 65536);
        if (!pPlayer->atc.buttonFlags.shoot)
        {
            if (!pPlayer->at1b2)
                pPlayer->at1b2 = pPlayer->atbf;
            pPlayer->atc3 = 1;
            StartQAV(pPlayer, 23, nClientThrowBundle, 0);
        }
        return 1;
    }
    }
    return 0;
}

BOOL func_4F320(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 9:
        pPlayer->at1ba = ClipHigh(divscale16(gFrameClock-pPlayer->at1b6,240), 65536);
        pPlayer->atbf = 0;
        if (!pPlayer->atc.buttonFlags.shoot)
        {
            pPlayer->atc3 = 8;
            StartQAV(pPlayer, 29, nClientThrowProx, 0);
        }
        return 1;
    }
    return 0;
}

BOOL func_4F3A0(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 13:
        pPlayer->at1ba = ClipHigh(divscale16(gFrameClock-pPlayer->at1b6,240), 65536);
        if (!pPlayer->atc.buttonFlags.shoot)
        {
            pPlayer->atc3 = 11;
            StartQAV(pPlayer, 39, nClientThrowRemote, 0);
        }
        return 1;
    }
    return 0;
}

BOOL func_4F414(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 4:
        pPlayer->atc3 = 6;
        StartQAV(pPlayer, 114, nClientFireLifeLeech, 1);
        return 1;
    case 6:
        if (pPlayer->atc.buttonFlags.shoot2)
            break;
    case 8:
        pPlayer->atc3 = 2;
        StartQAV(pPlayer, 118, -1, 0);
        return 1;
    }
    return 0;
}

BOOL func_4F484(PLAYER *pPlayer)
{
    switch (pPlayer->atc3)
    {
    case 4:
        pPlayer->atc3 = 5;
        if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 84, nClientFireTesla, 1);
        else
            StartQAV(pPlayer, 77, nClientFireTesla, 1);
        return 1;
    case 5:
        if (pPlayer->atc.buttonFlags.shoot)
            break;
        pPlayer->atc3 = 2;
        if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 87);
        else
            StartQAV(pPlayer, 80);
        return 1;
    case 7:
        pPlayer->atc3 = 2;
        if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
            StartQAV(pPlayer, 87);
        else
            StartQAV(pPlayer, 80);
        return 1;
    }
    return 0;
}

inline int VoodooAnim(int *a)
{
    int v = rand() << 1;
    int i;
    for (i = 0; a[i] < v; i++) {}
    return i;
}

void WeaponProcess(PLAYER *pPlayer)
{
    if (pPlayer->at37b > 0) pPlayer->at37b--;
    if (pPlayer->pXSprite->health == 0)
    {
        pPlayer->at1b1 = 0;
        sfxKill3DSound(pPlayer->pSprite, 1, -1);
    }
    if (pPlayer->at87 && BannedUnderwater(pPlayer->atbd))
    {
        if (func_4B1A4(pPlayer))
        {
            if (pPlayer->atbd == 7)
            {
                pPlayer->at1b2 = pPlayer->atbf;
                DropCan(1, pPlayer);
                pPlayer->atc3 = 3;
            }
            else if (pPlayer->atbd == 6)
            {
                pPlayer->at1b2 = pPlayer->atbf;
                DropBundle(1, pPlayer);
                pPlayer->atc3 = 1;
            }
        }
        WeaponLower(pPlayer);
        pPlayer->at1ba = 0;
    }
    WeaponPlay(pPlayer);
    UpdateAimVector(pPlayer);
    pPlayer->atbf -= 4;
    BOOL bShoot = pPlayer->atc.buttonFlags.shoot ? 1 : 0;
    BOOL bShoot2 = pPlayer->atc.buttonFlags.shoot2 ? 1 : 0;
    if (pPlayer->at1b1 && pPlayer->pXSprite->health > 0)
    {
        if (bShoot && CheckAmmo(pPlayer, pPlayer->atc7, 1))
        {
            while (pPlayer->atbf <= 0)
                pPlayer->atbf += weaponQAV[pPlayer->at26]->at10;
        }
        else
        {
            pPlayer->atbf = 0;
            pPlayer->at1b1 = 0;
        }
        return;
    }
    pPlayer->atbf = ClipLow(pPlayer->atbf, 0);
    switch (pPlayer->atbd)
    {
    case 7:
        if (func_4F0E0(pPlayer))
            return;
        break;
    case 6:
        if (func_4F200(pPlayer))
            return;
        break;
    case 11:
        if (func_4F320(pPlayer))
            return;
        break;
    case 12:
        if (func_4F3A0(pPlayer))
            return;
        break;
    }
    if (pPlayer->atbf > 0)
        return;
    if (pPlayer->pXSprite->health == 0 || pPlayer->atbd == 0)
        pPlayer->at26 = -1;
    switch (pPlayer->atbd)
    {
    case 9:
        if (func_4F414(pPlayer))
            return;
        break;
    case 8:
        if (func_4F484(pPlayer))
            return;
        break;
    }
    if (pPlayer->atbe)
    {
        sfxKill3DSound(pPlayer->pSprite, -1, 441);
        pPlayer->atc3 = 0;
        pPlayer->atc.newWeapon = pPlayer->atbe;
        pPlayer->atbe = 0;
    }
    if (pPlayer->atc.keyFlags.nextWeapon)
    {
        pPlayer->atc3 = 0;
        pPlayer->atc.keyFlags.nextWeapon = 0;
        pPlayer->atbe = 0;
        int t;
        char weapon;
        weapon = WeaponFindNext(pPlayer, &t, 1);
        pPlayer->atd9[weapon] = t;
        if (pPlayer->atbd != 0)
        {
            WeaponLower(pPlayer);
            pPlayer->atbe = weapon;
            return;
        }
        pPlayer->atc.newWeapon = weapon;
    }
    if (pPlayer->atc.keyFlags.prevWeapon)
    {
        pPlayer->atc3 = 0;
        pPlayer->atc.keyFlags.prevWeapon = 0;
        pPlayer->atbe = 0;
        int t;
        char weapon;
        weapon = WeaponFindNext(pPlayer, &t, 0);
        pPlayer->atd9[weapon] = t;
        if (pPlayer->atbd != 0)
        {
            WeaponLower(pPlayer);
            pPlayer->atbe = weapon;
            return;
        }
        pPlayer->atc.newWeapon = weapon;
    }
    if (pPlayer->atc3 == -1)
    {
        pPlayer->atc3 = 0;
        int t;
        char weapon;
        weapon = WeaponFindLoaded(pPlayer, &t);
        pPlayer->atd9[weapon] = t;
        if (pPlayer->atbd != 0)
        {
            WeaponLower(pPlayer);
            pPlayer->atbe = weapon;
            return;
        }
        pPlayer->atc.newWeapon = weapon;
    }
    if (pPlayer->atc.newWeapon)
    {
        if (pPlayer->atc.newWeapon == 6)
        {
            if (pPlayer->atbd == 6)
            {
                if (func_4B2C8(pPlayer, 10))
                    pPlayer->atc.newWeapon = 11;
                else if (func_4B2C8(pPlayer, 11))
                    pPlayer->atc.newWeapon = 12;
            }
            else if (pPlayer->atbd == 11)
            {
                if (func_4B2C8(pPlayer, 11))
                    pPlayer->atc.newWeapon = 12;
                else if (func_4B2C8(pPlayer, 5) && !pPlayer->at87)
                    pPlayer->atc.newWeapon = 6;
            }
            else if (pPlayer->atbd == 12)
            {
                if (func_4B2C8(pPlayer, 5) && !pPlayer->at87)
                    pPlayer->atc.newWeapon = 6;
                else if (func_4B2C8(pPlayer, 10))
                    pPlayer->atc.newWeapon = 11;
            }
            else
            {
                if (func_4B2C8(pPlayer, 5) && !pPlayer->at87)
                    pPlayer->atc.newWeapon = 6;
                else if (func_4B2C8(pPlayer, 10))
                    pPlayer->atc.newWeapon = 11;
                else if (func_4B2C8(pPlayer, 11))
                    pPlayer->atc.newWeapon = 12;
            }
        }
        if (pPlayer->pXSprite->health == 0 || !pPlayer->atcb[pPlayer->atc.newWeapon])
        {
            pPlayer->atc.newWeapon = 0;
            return;
        }
        if (pPlayer->at87 && BannedUnderwater(pPlayer->atc.newWeapon) && !func_4B1A4(pPlayer))
        {
            pPlayer->atc.newWeapon = 0;
            return;
        }
        int nWeapon = pPlayer->atc.newWeapon;
        int v4c = weaponModes[nWeapon].at0;
        if (pPlayer->atbd == 0)
        {
            int nAmmoType = weaponModes[nWeapon].at4;
            if (v4c > 1)
            {
                if (CheckAmmo(pPlayer, nAmmoType, 1) || nAmmoType == 11)
                    WeaponRaise(pPlayer);
            }
            else
            {
                if (func_4B1FC(pPlayer, nWeapon, nAmmoType))
                    WeaponRaise(pPlayer);
                else
                {
                    pPlayer->atc3 = 0;
                    int t;
                    char weapon;
                    weapon = WeaponFindLoaded(pPlayer, &t);
                    pPlayer->atd9[weapon] = t;
                    if (pPlayer->atbd != 0)
                    {
                        WeaponLower(pPlayer);
                        pPlayer->atbe = weapon;
                        return;
                    }
                    pPlayer->atc.newWeapon = weapon;
                }
                return;
            }
        }
        else
        {
            if (nWeapon == pPlayer->atbd && v4c <= 1)
            {
                pPlayer->atc.newWeapon = 0;
                return;
            }
            int i = 0;
            if (nWeapon == pPlayer->atbd)
                i = 1;
            for (; i <= v4c; i++)
            {
                int v6c = (pPlayer->atd9[nWeapon]+i)%v4c;
                int t = weaponModes[nWeapon].at4;
                if (func_4B1FC(pPlayer, nWeapon, t))
                {
                    WeaponLower(pPlayer);
                    pPlayer->atd9[nWeapon] = v6c;
                    return;
                }
            }
        }
        pPlayer->atc.newWeapon = 0;
        return;
    }
    if (pPlayer->atbd != 0 && !CheckAmmo(pPlayer, pPlayer->atc7, 1) && pPlayer->atc7 != 11)
    {
        pPlayer->atc3 = -1;
        return;
    }
    if (bShoot)
    {
        switch (pPlayer->atbd)
        {
        case 1:
            StartQAV(pPlayer, 2, nClientFirePitchfork);
            return;
        case 7:
            if (pPlayer->atc3 == 3)
            {
                pPlayer->atc3 = 4;
                StartQAV(pPlayer, 10, nClientFireSpray, 1);
                return;
            }
            break;
        case 6:
            switch (pPlayer->atc3)
            {
            case 3:
                pPlayer->atc3 = 6;
                pPlayer->at1b6 = gFrameClock;
                pPlayer->at1b2 = -1;
                StartQAV(pPlayer, 21, nClientExplodeBundle);
                return;
            }
            break;
        case 11:
            switch (pPlayer->atc3)
            {
            case 7:
                pPlayer->at26 = 27;
                pPlayer->atc3 = 9;
                pPlayer->at1b6 = gFrameClock;
                return;
            }
            break;
        case 12:
            switch (pPlayer->atc3)
            {
            case 10:
                pPlayer->at26 = 36;
                pPlayer->atc3 = 13;
                pPlayer->at1b6 = gFrameClock;
                return;
            case 11:
                pPlayer->atc3 = 12;
                StartQAV(pPlayer, 40, nClientFireRemote);
                return;
            }
            break;
        case 3:
            switch (pPlayer->atc3)
            {
            case 7:
                pPlayer->atc3 = 6;
                StartQAV(pPlayer, 61, nClientFireShotgun);
                return;
            case 3:
                pPlayer->atc3 = 2;
                StartQAV(pPlayer, 54, nClientFireShotgun);
                return;
            case 2:
                pPlayer->atc3 = 1;
                StartQAV(pPlayer, 55, nClientFireShotgun);
                return;
            }
            break;
        case 4:
            if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 3, 2))
                StartQAV(pPlayer, 71, nClientFireTommy, 1);
            else
                StartQAV(pPlayer, 66, nClientFireTommy, 1);
            return;
        case 2:
            if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 1, 2))
                StartQAV(pPlayer, 48, nClientFireFlare);
            else
                StartQAV(pPlayer, 43, nClientFireFlare);
            return;
        case 10:
        {
            static int nChance[] = { 0xa000, 0xc000, 0xe000, 0x10000 };
            int vc = VoodooAnim(nChance);
            pPlayer->playerVoodooTarget = pPlayer->at1d6;
            if (pPlayer->playerVoodooTarget == -1 || sprite[pPlayer->playerVoodooTarget].statnum != 6)
                vc = 4;
            StartQAV(pPlayer,103+vc, nClientFireVoodoo);
            return;
        }
        case 8:
            switch (pPlayer->atc3)
            {
            case 2:
                pPlayer->atc3 = 4;
                if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
                    StartQAV(pPlayer, 84, nClientFireTesla);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla);
                return;
            case 5:
                if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
                    StartQAV(pPlayer, 84, nClientFireTesla);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla);
                return;
            }
            break;
        case 5:
            if (powerupCheck(pPlayer, 17))
                StartQAV(pPlayer, 122, nClientFireNapalm);
            else
                StartQAV(pPlayer, 91, nClientFireNapalm);
            return;
        case 9:
            sfxPlay3DSound(pPlayer->pSprite, 494, 2);
            StartQAV(pPlayer, 116, nClientFireLifeLeech);
            return;
        case 13:
            StartQAV(pPlayer, 95+Random(4), nClientFireBeast);
            return;
        }
    }
    if (bShoot2)
    {
        switch (pPlayer->atbd)
        {
        case 1:
            StartQAV(pPlayer, 2, nClientFirePitchfork);
            return;
        case 7:
            if (pPlayer->atc3 == 3)
            {
                pPlayer->atc3 = 5;
                StartQAV(pPlayer, 12, nClientExplodeCan);
                return;
            }
            break;
        case 6:
            switch (pPlayer->atc3)
            {
            case 3:
                pPlayer->atc3 = 4;
                StartQAV(pPlayer, 21, nClientExplodeBundle);
                return;
            case 7:
                pPlayer->atc3 = 8;
                StartQAV(pPlayer, 28, nClientDropProx);
                return;
            case 10:
                pPlayer->atc3 = 11;
                StartQAV(pPlayer, 38, nClientDropRemote);
                return;
            case 11:
                if (pPlayer->at181[11] > 0)
                {
                    pPlayer->atc3 = 10;
                    StartQAV(pPlayer, 30);
                }
                return;
            }
            break;
        case 11:
            switch (pPlayer->atc3)
            {
            case 7:
                pPlayer->atc3 = 8;
                StartQAV(pPlayer, 28, nClientDropProx);
                return;
            }
            break;
        case 12:
            switch (pPlayer->atc3)
            {
            case 10:
                pPlayer->atc3 = 11;
                StartQAV(pPlayer, 38, nClientDropRemote);
                return;
            case 11:
                if (pPlayer->at181[11] > 0)
                {
                    pPlayer->atc3 = 10;
                    StartQAV(pPlayer, 30);
                }
                return;
            }
            break;
        case 3:
            switch (pPlayer->atc3)
            {
            case 7:
                pPlayer->atc3 = 6;
                StartQAV(pPlayer, 62, nClientFireShotgun);
                return;
            case 3:
                pPlayer->atc3 = 1;
                StartQAV(pPlayer, 56, nClientFireShotgun);
                return;
            case 2:
                pPlayer->atc3 = 1;
                StartQAV(pPlayer, 55, nClientFireShotgun);
                return;
            }
            break;
        case 4:
            if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 3, 2))
                StartQAV(pPlayer, 73, nClientAltFireSpread2);
            else
                StartQAV(pPlayer, 67, nClientAltFireSpread2);
            return;
        case 10:
            sfxPlay3DSound(pPlayer->pSprite, 461, 2);
            StartQAV(pPlayer, 110, nClientAltFireVoodoo);
            return;
        case 8:
            if (func_4B2C8(pPlayer, 7, 35))
            {
                if (func_4B2C8(pPlayer, 7, 70) && powerupCheck(pPlayer, 17))
                    StartQAV(pPlayer, 85, nClientFireTesla);
                else
                    StartQAV(pPlayer, 78, nClientFireTesla);
            }
            else
            {
                if (func_4B2C8(pPlayer, 7, 10) && powerupCheck(pPlayer, 17))
                    StartQAV(pPlayer, 84, nClientFireTesla);
                else
                    StartQAV(pPlayer, 77, nClientFireTesla);
            }
            return;
        case 5:
            if (powerupCheck(pPlayer, 17))
                StartQAV(pPlayer, 122, nClientAltFireNapalm);
            else
                StartQAV(pPlayer, 91, nClientAltFireNapalm);
            return;
        case 2:
            if (CheckAmmo(pPlayer, 1, 8))
            {
                if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 1, 16))
                    StartQAV(pPlayer, 48, nClientAltFireFlare);
                else
                    StartQAV(pPlayer, 43, nClientAltFireFlare);
            }
            else
            {
                if (powerupCheck(pPlayer, 17) && func_4B2C8(pPlayer, 1, 2))
                    StartQAV(pPlayer, 48, nClientFireFlare);
                else
                    StartQAV(pPlayer, 43, nClientFireFlare);
            }
            return;
        case 9:
            if (gGameOptions.nGameType <= GAMETYPE_1 && !func_4B2C8(pPlayer, 8) && pPlayer->pXSprite->health < (25 << 4))
            {
                sfxPlay3DSound(pPlayer->pSprite, 494, 2);
                StartQAV(pPlayer, 116, nClientFireLifeLeech);
            }
            else
            {
                StartQAV(pPlayer, 119);
                AltFireLifeLeech(1, pPlayer);
                pPlayer->atc3 = -1;
            }
            return;
        }
    }
    WeaponUpdateState(pPlayer);
}

void func_51340(SPRITE *pMissile, int a2)
{
    byte va4[(kMaxSectors+7)>>3];
    int x = pMissile->x;
    int y = pMissile->y;
    int z = pMissile->z;
    int nDist = 300;
    int nSector = pMissile->sectnum;
    int nOwner = actSpriteOwnerToSpriteId(pMissile);
    gAffectedSectors[0] = -1;
    gAffectedXWalls[0] = -1;
    GetClosestSpriteSectors(nSector, x, y, nDist, gAffectedSectors, va4, gAffectedXWalls);
    BOOL v4 = 1;
    int v24 = -1;
    actHitcodeToData(a2, &gHitInfo, &v24, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (a2 == 3 && v24 >= 0 && sprite[v24].statnum == 6)
        v4 = 0;
    for (int nSprite = headspritestat[6]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        if (nSprite != nOwner || v4)
        {
            SPRITE *pSprite = &sprite[nSprite];
            if (pSprite->flags&kSpriteFlag5)
                continue;
            if (TestBitString(va4, pSprite->sectnum) && CheckProximity(pSprite, x, y, z, nSector, nDist))
            {
                int dx = pMissile->x-pSprite->x;
                int dy = pMissile->y-pSprite->y;
                int nDamage = ClipLow((nDist-(ksqrt(dx*dx+dy*dy)>>4)+20)>>1, 10);
                if (nSprite == nOwner)
                    nDamage /= 2;
                actDamageSprite(nOwner, pSprite, kDamageTesla, nDamage<<4);
            }
        }
    }
    for (nSprite = headspritestat[4]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (pSprite->flags&kSpriteFlag5)
            continue;
        if (TestBitString(va4, pSprite->sectnum) && CheckProximity(pSprite, x, y, z, nSector, nDist))
        {
            XSPRITE *pXSprite = &xsprite[pSprite->extra];
            if (!pXSprite->at17_5)
            {
                int dx = pMissile->x-pSprite->x;
                int dy = pMissile->y-pSprite->y;
                int nDamage = ClipLow(nDist-(ksqrt(dx*dx+dy*dy)>>4)+20, 20);
                actDamageSprite(nOwner, pSprite, kDamageTesla, nDamage<<4);
            }
        }
    }
}

class WeaponLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

void WeaponLoadSave::Load()
{
}

void WeaponLoadSave::Save()
{
}

static WeaponLoadSave myLoadSave;
