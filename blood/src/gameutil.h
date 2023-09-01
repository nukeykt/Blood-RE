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
#ifndef _GAMEUTIL_H_
#define _GAMEUTIL_H_

#include "typedefs.h"
#include "build.h"
#include "misc.h"

struct HITINFO {
    short hitsect;
    short hitwall;
    short hitsprite;
    int hitx;
    int hity;
    int hitz;
};

extern POINT2D baseWall[kMaxWalls];
extern POINT3D baseSprite[kMaxSprites];
extern long baseFloor[kMaxSectors];
extern long baseCeil[kMaxSectors];
extern short gUpperLink[kMaxSectors];
extern short gLowerLink[kMaxSectors];
extern long velFloor[kMaxSectors];
extern long velCeil[kMaxSectors];

extern HITINFO gHitInfo;

inline int Dist2d(int dx, int dy)
{
    dx >>= 4;
    dy >>= 4;
    return ksqrt(dx*dx+dy*dy);
}

inline int Dist3d(int dx, int dy, int dz)
{
    dx >>= 4;
    dy >>= 4;
    dz >>= 8;
    return ksqrt(dx*dx+dy*dy+dz*dz);
}

inline void GetSpriteExtents(SPRITE *pSprite, int *pTop, int *pBottom)
{
    *pTop = *pBottom = pSprite->z;
    if ((pSprite->cstat & kSpriteMask) == kSpriteFloor)
        return;
    int nTile = pSprite->picnum;
    *pTop -= (picanm[nTile].yoffset + tilesizy[nTile] / 2) * (pSprite->yrepeat << 2);
    *pBottom += (tilesizy[nTile] - (tilesizy[nTile] / 2 + picanm[nTile].yoffset)) * (pSprite->yrepeat << 2);
}

BOOL FindSector(int nX, int nY, int nZ, int *nSector);
BOOL FindSector(int nX, int nY, int *nSector);
uint ClipMove(long *x, long *y, long *z, int *nSector, long xv, long yv, int wd, int cd, int fd, ulong nMask);
void GetZRange(SPRITE *pSprite, long *ceilZ, long *ceilHit, long *floorZ, long *floorHit, int nDist, ulong nMask);
int GetWallAngle(int nWall);
void CalcFrameRate(void);
void GetWallNormal(int nWall, int *pX, int *pY);
int GetClosestSpriteSectors(int nSector, int x, int y, int nDist, short *pSectors, byte *pSectBit, short *a8);
BOOL CheckProximity(SPRITE *pSprite, int nX, int nY, int nZ, int nSector, int nDist);
void GetZRangeAtXYZ(long x, long y, long z, int nSector, long *ceilZ, long *ceilHit, long *floorZ, long *floorHit, int nDist, unsigned long nMask);
int HitScan(SPRITE *pSprite, int z, int dx, int dy, int dz, unsigned long nMask, int a8);
int VectorScan(SPRITE *pSprite, int nOffset, int nZOffset, int dx, int dy, int dz, int nRange, int ac);


#endif // !_GAMEUTIL_H_
