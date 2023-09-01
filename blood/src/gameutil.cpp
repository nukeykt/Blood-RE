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
#include "db.h"
#include "debug4g.h"
#include "gameutil.h"
#include "globals.h"
#include "misc.h"
#include "tile.h"
#include "trig.h"

POINT2D baseWall[kMaxWalls];
POINT3D baseSprite[kMaxSprites];
long baseFloor[kMaxSectors];
long baseCeil[kMaxSectors];
long velFloor[kMaxSectors];
long velCeil[kMaxSectors];
short gUpperLink[kMaxSectors];
short gLowerLink[kMaxSectors];
HITINFO gHitInfo;

BOOL AreSectorsNeighbors(int sect1, int sect2)
{
    int i;
    int nWall;
    int nWallNum1;
    int nWallNum2;
    dassert(sect1 >= 0 && sect1 < kMaxSectors, 43);
    dassert(sect2 >= 0 && sect2 < kMaxSectors, 44);
    nWallNum1 = sector[sect1].wallnum;
    nWallNum2 = sector[sect2].wallnum;
    if (nWallNum1 < nWallNum2)
    {
        for (i = 0, nWall = sector[sect1].wallptr; i < nWallNum1; i++, nWall++)
        {
            if (wall[nWall].nextsector == sect2)
            {
                return 1;
            }
        }
    }
    else
    {
        for (i = 0, nWall = sector[sect2].wallptr; i < nWallNum2; i++, nWall++)
        {
            if (wall[nWall].nextsector == sect1)
            {
                return 1;
            }
        }
    }
    return 0;
}

BOOL FindSector(int nX, int nY, int nZ, int* nSector)
{
    int i;
    int nZCeil;
    int nZFloor;
    dassert(*nSector >= 0 && *nSector < kMaxSectors, 82);
    if (inside(nX, nY, *nSector))
    {
        getzsofslope(*nSector, nX, nY, &nZCeil, &nZFloor);
        if (nZ >= nZCeil && nZ <= nZFloor)
        {
            return 1;
        }
    }
    WALL *pWall = &wall[sector[*nSector].wallptr];
    for (i = sector[*nSector].wallnum; i > 0; i--, pWall++)
    {
        short nOSector = pWall->nextsector;
        if (nOSector >= 0 && inside(nX, nY, nOSector))
        {
            getzsofslope(nOSector, nX, nY, &nZCeil, &nZFloor);
            if (nZ >= nZCeil && nZ <= nZFloor)
            {
                *nSector = nOSector;
                return 1;
            }
        }
    }
    for (i = 0; i < numsectors; i++)
    {
        if (inside(nX, nY, i))
        {
            getzsofslope(i, nX, nY, &nZCeil, &nZFloor);
            if (nZ >= nZCeil && nZ <= nZFloor)
            {
                *nSector = i;
                return 1;
            }
        }
    }
    return 0;
}

BOOL FindSector(int nX, int nY, int *nSector)
{
    int i;
    dassert(*nSector >= 0 && *nSector < kMaxSectors, 127);
    if (inside(nX, nY, *nSector))
    {
        return 1;
    }
    WALL *pWall = &wall[sector[*nSector].wallptr];
    for (i = sector[*nSector].wallnum; i > 0; i--, pWall++)
    {
        short nOSector = pWall->nextsector;
        if (nOSector >= 0 && inside(nX, nY, nOSector))
        {
            *nSector = nOSector;
            return 1;
        }
    }
    for (i = 0; i < numsectors; i++)
    {
        if (inside(nX, nY, i))
        {
            *nSector = i;
            return 1;
        }
    }
    return 0;
}

void CalcFrameRate(void)
{
    static long ticks[64];
    static long index;
    if (gFrameClock != ticks[index])
    {
        gFrameRate = (120*64)/(gFrameClock-ticks[index]);
        ticks[index] = gFrameClock;
    }
    index = (index+1) & 63;
}

BOOL CheckProximity(SPRITE *pSprite, int nX, int nY, int nZ, int nSector, int nDist)
{
    dassert(pSprite != NULL, 183);
    int oX = klabs(nX-pSprite->x)>>4;
    if (oX < nDist)
    {
        int oY = klabs(nY-pSprite->y)>>4;
        if (oY < nDist)
        {
            int oZ = klabs(nZ-pSprite->z)>>8;
            if (oZ < nDist)
            {
                if (approxDist(oX, oY) < nDist)
                {
                    int top, bottom;
                    GetSpriteExtents(pSprite, &top, &bottom);
                    if (cansee(pSprite->x, pSprite->y, pSprite->z, pSprite->sectnum, nX, nY, nZ, nSector))
                        return 1;
                    if (cansee(pSprite->x, pSprite->y, top, pSprite->sectnum, nX, nY, nZ, nSector))
                        return 1;
                    if (cansee(pSprite->x, pSprite->y, bottom, pSprite->sectnum, nX, nY, nZ, nSector))
                        return 1;
                }
            }
        }
    }
    return 0;
}

BOOL CheckProximityPoint(int nX1, int nY1, int nZ1, int nX2, int nY2, int nZ2, int nDist)
{
    int oX = klabs(nX2-nX1)>>4;
    if (oX < nDist)
    {
        int oY = klabs(nY2-nY1)>>4;
        if (oY < nDist)
        {
            int oZ = klabs(nZ2-nZ1)>>8;
            if (oZ < nDist)
            {
                if (approxDist(oX, oY) < nDist)
                    return 1;
            }
        }
    }
    return 0;
}

BOOL CheckProximityWall(int nWall, int x, int y, int nDist)
{
    int y1;
    int x1;
    int y2;
    int x2;

    int dx;
    int dy;
    int px;
    int py;


    x1 = wall[nWall].x;
    y1 = wall[nWall].y;
    x2 = wall[wall[nWall].point2].x;
    y2 = wall[wall[nWall].point2].y;
    nDist <<= 4;
    if (x1 < x2)
    {
        if (x <= x1 - nDist || x >= x2 + nDist)
            return 0;
    }
    else
    {
        if (x <= x2 - nDist || x >= x1 + nDist)
            return 0;
        if (x1 == x2)
        {
            if (y2 > y1)
            {
                if (y <= y1 - nDist || y >= y2 + nDist)
                {
                    return 0;
                }
                if (y < y1)
                {
                    return (x - x1) * (x - x1) + (y - y1) * (y - y1) < nDist* nDist;
                }
                if (y > y2)
                {
                    return (x - x2) * (x - x2) + (y - y2) * (y - y2) < nDist* nDist;
                }
            }
            else
            {
                if (y <= y2 - nDist || y >= y1 + nDist)
                {
                    return 0;
                }
                if (y < y2)
                {
                    return (x - x2) * (x - x2) + (y - y2) * (y - y2) < nDist* nDist;
                }
                if (y > y1)
                {
                    return (x - x1) * (x - x1) + (y - y1) * (y - y1) < nDist* nDist;
                }
            }
            return 1;
        }
    }
    if (y2 > y1)
    {
        if (y <= y1 - nDist || y >= y2 + nDist)
        {
            return 0;
        }
    }
    else
    {
        if (y <= y2 - nDist || y >= y1 + nDist)
        {
            return 0;
        }
        if (y1 == y2)
        {
            if (x1 < x2)
            {
                if (x <= x1 - nDist || x >= x2 + nDist)
                {
                    return 0;
                }
                if (x < x1)
                {
                    return (x - x1) * (x - x1) + (y - y1) * (y - y1) < nDist * nDist;
                }
                if (x > x2)
                {
                    return (x - x2) * (x - x2) + (y - y2) * (y - y2) < nDist * nDist;
                }
            }
            else
            {
                if (x <= x2 - nDist || x >= x1 + nDist)
                {
                    return 0;
                }
                if (x < x2)
                {
                    return (x - x2) * (x - x2) + (y - y2) * (y - y2) < nDist * nDist;
                }
                if (x > x1)
                {
                    return (x - x1) * (x - x1) + (y - y1) * (y - y1) < nDist * nDist;
                }
            }
        }
    }

    nDist = nDist * nDist;

    dx = x2 - x1;
    dy = y2 - y1;
    px = x - x2;
    py = y - y2;
    if (dx * px + dy * py >= 0)
    {
        return px * px + py * py < nDist;
    }
    px = x - x1;
    py = y - y1;
    if (dx * px + dy * py <= 0)
    {
        return px * px + py * py < nDist;
    }
    //int check1 = dy * px - dx * py;
    //int check2 = dx * dx + dy * dy;
    return (dy * px - dx * py) * (dy * px - dx * py) < (dx* dx + dy * dy) * nDist;
}

int GetWallAngle(int nWall)
{
    int dx = wall[wall[nWall].point2].x - wall[nWall].x;
    int dy = wall[wall[nWall].point2].y - wall[nWall].y;
    return getangle(dx, dy);
}

void GetWallNormal(int nWall, int *pX, int *pY)
{
    dassert(nWall >= 0 && nWall < kMaxWalls, 484);
    int dX = -(wall[wall[nWall].point2].y - wall[nWall].y) >> 4;
    int dY = (wall[wall[nWall].point2].x - wall[nWall].x) >> 4;
    int nLength = ksqrt(dX*dX+dY*dY);
    if (nLength <= 0)
        nLength = 1;
    *pX = divscale16(dX, nLength);
    *pY = divscale16(dY, nLength);
}

BOOL IntersectRay(long wx, long wy, long wdx, long wdy, long x1, long y1, long z1, long x2, long y2, long z2, long *ix, long *iy, long *iz)
{
    int topt, topu, t;
    int dX = x1 - x2;
    int dY = y1 - y2;
    int dZ = z1 - z2;
    int side = wdx * dY - wdy * dX;
    if (side >= 0)
    {
        if (side == 0)
            return 0;
        int dX2 = x1 - wx;
        int dY2 = y1 - wy;
        topt = dX2 * dY - dY2 * dX;
        if (topt < 0)
            return 0;
        topu = wdx * dY2 - wdy * dX2;
        if (topu < 0 || topu >= side)
            return 0;
    }
    else
    {
        int dX2 = x1 - wx;
        int dY2 = y1 - wy;
        topt = dX2 * dY - dY2 * dX;
        if (topt > 0)
            return 0;
        topu = wdx * dY2 - wdy * dX2;
        if (topu > 0 || topu <= side)
            return 0;
    }
    t = divscale16(topu, side);
    *ix = x1 + mulscale16(dX, t);
    *iy = y1 + mulscale16(dY, t);
    *iz = z1 + mulscale16(dZ, t);
    return 1;
}

int HitScan(SPRITE *pSprite, int z, int dx, int dy, int dz, unsigned long nMask, int a8)
{
    dassert(pSprite != NULL, 555);
    dassert(dx != 0 || dy != 0, 556);
    gHitInfo.hitsect = -1;
    gHitInfo.hitwall = -1;
    gHitInfo.hitsprite = -1;
    int x = pSprite->x;
    int y = pSprite->y;
    short nSector = pSprite->sectnum;
    ushort bakCstat = pSprite->cstat;
    pSprite->cstat &= ~256;
    if (a8)
    {
        hitscangoalx = x + mulscale30(a8 << 4, Cos(pSprite->ang));
        hitscangoaly = y + mulscale30(a8 << 4, Sin(pSprite->ang));
    }
    else
    {
        hitscangoalx = hitscangoaly = 0x1fffffff;
    }
    hitscan(x, y, z, nSector, dx, dy, dz << 4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite, &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, nMask);
    hitscangoalx = hitscangoaly = 0x1fffffff;
    pSprite->cstat = bakCstat;
    if (gHitInfo.hitsprite >= kMaxSprites || gHitInfo.hitwall >= kMaxWalls || gHitInfo.hitsect >= kMaxSectors)
        return -1;
    if (gHitInfo.hitsprite >= 0)
        return 3;
    if (gHitInfo.hitwall >= 0)
    {
        short nextSector = wall[gHitInfo.hitwall].nextsector;
        if (nextSector == -1)
            return 0;
        else
        {
            int nZCeil, nZFloor;
            getzsofslope(nextSector, gHitInfo.hitx, gHitInfo.hity, &nZCeil, &nZFloor);
            if (gHitInfo.hitz <= nZCeil || gHitInfo.hitz >= nZFloor)
                return 0;
            return 4;
        }
    }
    if (gHitInfo.hitsect >= 0)
    {
        return z < gHitInfo.hitz ? 2 : 1;
    }
    return -1;
}

int VectorScan(SPRITE *pSprite, int nOffset, int nZOffset, int dx, int dy, int dz, int nRange, int ac)
{
    int nNum = 256;
    dassert(pSprite != NULL, 646);
    gHitInfo.hitsect = -1;
    gHitInfo.hitwall = -1;
    gHitInfo.hitsprite = -1;
    int x1 = pSprite->x+mulscale30(nOffset, Cos(pSprite->ang+512));
    int y1 = pSprite->y+mulscale30(nOffset, Sin(pSprite->ang+512));
    int z1 = pSprite->z+nZOffset;
    short nSector = pSprite->sectnum;
    ushort bakCstat = pSprite->cstat;
    pSprite->cstat &= ~kSpriteStat8;
    if (nRange)
    {
        hitscangoalx = x1+mulscale30(nRange<<4, Cos(pSprite->ang));
        hitscangoaly = y1+mulscale30(nRange<<4, Sin(pSprite->ang));
    }
    else
    {
        hitscangoalx = hitscangoaly = 0x1fffffff;
    }
    hitscan(x1, y1, z1, nSector, dx, dy, dz << 4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite, &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, CLIPMASK1);
    pSprite->cstat = bakCstat;
retry:
    if (nNum-- == 0)
        return -1;
    if (gHitInfo.hitsprite >= kMaxSprites || gHitInfo.hitwall >= kMaxWalls || gHitInfo.hitsect >= kMaxSectors)
        return -1;
    if (nRange && approxDist(gHitInfo.hitx - pSprite->x, gHitInfo.hity - pSprite->y) > nRange)
        return -1;
    if (gHitInfo.hitsprite >= 0)
    {
        SPRITE *pOther = &sprite[gHitInfo.hitsprite];
        if ((pOther->flags & kSpriteFlag3) && !(ac & 1))
            return 3;
        switch (pOther->cstat & kSpriteMask)
        {
            case kSpriteFace:
            {
                int nPicnum = pOther->picnum;
                if (!tilesizx[nPicnum] || !tilesizy[nPicnum])
                    return 3;
                int height = (tilesizy[nPicnum]*pOther->yrepeat)<<2;
                int otherZ = pOther->z;
                if (pOther->cstat & kSpriteStat7)
                    otherZ += height / 2;
                if (picanm[nPicnum].yoffset)
                    otherZ -= (picanm[nPicnum].yoffset*pOther->yrepeat)<<2;
                dassert(height > 0, 735);
                int height2 = kscale(otherZ-gHitInfo.hitz, tilesizy[nPicnum], height);
                if (!(pOther->cstat & kSpriteStat3))
                    height2 = tilesizy[nPicnum] - height2;
                if (height2 >= 0 && height2 < tilesizy[nPicnum])
                {
                    int width = (tilesizx[nPicnum]*pOther->xrepeat)>>2;
                    width = (width*3)/4;
                    int top = dx * (y1 - pOther->y) - dy * (x1 - pOther->x);
                    int bot = ksqrt(dx * dx + dy * dy);
                    int check1 = top / bot;
                    dassert(width > 0, 748);
                    int width2 = kscale(check1, tilesizx[nPicnum], width);
                    width2 += tilesizx[nPicnum] / 2 + picanm[nPicnum].xoffset;
                    if (width2 >= 0 && width2 < tilesizx[nPicnum])
                    {
                        byte *pData = tileLoadTile(nPicnum);
                        byte nPixel = pData[width2*tilesizy[nPicnum]+height2];
                        if (nPixel != 255)
                            return 3;
                    }
                }
                gHitInfo.hitsect = -1;
                gHitInfo.hitwall = -1;
                gHitInfo.hitsprite = -1;
                x1 = gHitInfo.hitx;
                y1 = gHitInfo.hity;
                z1 = gHitInfo.hitz;
                nSector = pOther->sectnum;
                ushort bakCstat = pOther->cstat;
                pOther->cstat &= ~kSpriteStat8;
                hitscan(x1, y1, z1, nSector,
                    dx, dy, dz << 4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite, &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, CLIPMASK1);
                pOther->cstat = bakCstat;
                goto retry;
            }
        }
        return 3;
    }
    if (gHitInfo.hitwall >= 0)
    {
        WALL *pWall = &wall[gHitInfo.hitwall];
        short nextSector = pWall->nextsector;
        if (nextSector == -1)
            return 0;
        else
        {
            SECTOR *pSector = &sector[gHitInfo.hitsect];
            SECTOR *pSectorNext = &sector[nextSector];
            int nZCeil, nZFloor;
            getzsofslope(nextSector, gHitInfo.hitx, gHitInfo.hity, &nZCeil, &nZFloor);
            if (gHitInfo.hitz <= nZCeil)
                return 0;
            if (gHitInfo.hitz >= nZFloor)
            {
                if ((pSector->floorstat&kSectorStat0) && (pSectorNext->floorstat&kSectorStat0))
                    return 2;
                else
                    return 0;
            }
            if (!(pWall->cstat & (kWallStat4|kWallStat5)))
                return 0;
            int nOffset;
            if (pWall->cstat & kWallStat2)
                nOffset = ClipHigh(pSector->floorz, pSectorNext->floorz);
            else
                nOffset = ClipLow(pSector->ceilingz, pSectorNext->ceilingz);
            int nOffset2 = (gHitInfo.hitz - nOffset) >> 8;
            if (pWall->cstat & kWallStat8)
                nOffset2 = -nOffset2;

            int nPicnum = pWall->overpicnum;
            int nSizX = tilesizx[nPicnum];
            int nSizY = tilesizy[nPicnum];
            if (!nSizX || !nSizY)
                return 0;

            BOOL potX = (1<<(picsiz[nPicnum]&15)) == nSizX;
            BOOL potY = (1<<(picsiz[nPicnum]>>4)) == nSizY;

            int nVOffset = (nOffset2*pWall->yrepeat) / 8 + (pWall->ypanning*nSizY) / 256;
            int nLength = approxDist(pWall->x - wall[pWall->point2].x, pWall->y - wall[pWall->point2].y);
            int d;
            if (pWall->cstat & kWallStat3)
                d = approxDist(gHitInfo.hitx - wall[pWall->point2].x, gHitInfo.hity - wall[pWall->point2].y);
            else
                d = approxDist(gHitInfo.hitx - pWall->x, gHitInfo.hity - pWall->y);

            int nHOffset = ((d*pWall->xrepeat) << 3) / nLength + pWall->xpanning;
            if (potX)
                nHOffset &= nSizX - 1;
            else
                nHOffset %= nSizX;
            if (potY)
                nVOffset &= nSizY - 1;
            else
                nVOffset %= nSizY;
            byte *pData = tileLoadTile(nPicnum);
            byte nPixel;
            if (potY)
                nPixel = pData[(nHOffset<<(picsiz[nPicnum]>>4)) + nVOffset];
            else
                nPixel = pData[nHOffset * tilesizy[nPicnum] + nVOffset];

            if (nPixel == 255)
            {
                ushort bakCstat = pWall->cstat;
                pWall->cstat &= ~kWallStat6;
                ushort bakCstat2 = wall[pWall->nextwall].cstat;
                wall[pWall->nextwall].cstat &= ~kWallStat6;
                gHitInfo.hitsect = -1;
                gHitInfo.hitwall = -1;
                gHitInfo.hitsprite = -1;
                x1 = gHitInfo.hitx;
                y1 = gHitInfo.hity;
                z1 = gHitInfo.hitz;
                nSector = pWall->nextsector;
                hitscan(x1, y1, z1, nSector,
                    dx, dy, dz << 4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite, &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, CLIPMASK1);
                pWall->cstat = bakCstat;
                wall[pWall->nextwall].cstat = bakCstat2;
                goto retry;
            }
            return 4;
        }
    }
    if (gHitInfo.hitsect >= 0)
    {
        if (dz > 0)
        {
            int nSprite = gUpperLink[gHitInfo.hitsect];
            if (nSprite >= 0)
            {
                gHitInfo.hitsect = -1;
                gHitInfo.hitwall = -1;
                gHitInfo.hitsprite = -1;
                int nLink = sprite[nSprite].owner;
                x1 = gHitInfo.hitx + sprite[nLink & 0xfff].x - sprite[nSprite].x;
                y1 = gHitInfo.hity + sprite[nLink & 0xfff].y - sprite[nSprite].y;
                z1 = gHitInfo.hitz + sprite[nLink & 0xfff].z - sprite[nSprite].z;
                nSector = sprite[nLink & 0xfff].sectnum;
                hitscan(x1, y1, z1, nSector, dx, dy, dz<<4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite,
                    &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, CLIPMASK1);
                goto retry;
            }
            return 2;
        }
        else
        {
            int nSprite = gLowerLink[gHitInfo.hitsect];
            if (nSprite >= 0)
            {
                gHitInfo.hitsect = -1;
                gHitInfo.hitwall = -1;
                gHitInfo.hitsprite = -1;
                int nLink = sprite[nSprite].owner;
                x1 = gHitInfo.hitx + sprite[nLink & 0xfff].x - sprite[nSprite].x;
                y1 = gHitInfo.hity + sprite[nLink & 0xfff].y - sprite[nSprite].y;
                z1 = gHitInfo.hitz + sprite[nLink & 0xfff].z - sprite[nSprite].z;
                nSector = sprite[nLink & 0xfff].sectnum;
                hitscan(x1, y1, z1, nSector, dx, dy, dz<<4, &gHitInfo.hitsect, &gHitInfo.hitwall, &gHitInfo.hitsprite,
                    &gHitInfo.hitx, &gHitInfo.hity, &gHitInfo.hitz, CLIPMASK1);
                goto retry;
            }
            return 1;
        }
    }
    return -1;
}

void GetZRange(SPRITE *pSprite, long *ceilZ, long *ceilHit, long *floorZ, long *floorHit, int nDist, ulong nMask)
{
    dassert(pSprite != NULL, 1018);
    long nTemp1, nTemp2;
    short bakCstat = pSprite->cstat;
    pSprite->cstat &= ~(kSpriteStat8 | kSpriteStat0);
    getzrange(pSprite->x, pSprite->y, pSprite->z, pSprite->sectnum, ceilZ, ceilHit, floorZ, floorHit, nDist, nMask);
    if (((*floorHit) & 0xe000) == 0x4000)
    {
        int nSector = (*floorHit) & 0x1fff;
        if ((nMask & 0x2000) == 0 && (sector[nSector].floorstat & kSectorStat0))
            *floorZ = 0x7fffffff;
        int nXSector = sector[nSector].extra;
        if (nXSector > 0)
        {
            XSECTOR *pXSector = &xsector[nXSector];
            *floorZ += pXSector->at13_5 << 10;
        }
        int nSprite = gUpperLink[nSector];
        if (nSprite >= 0)
        {
            int nLink = sprite[nSprite].owner;
            int t = nLink & 0xfff;
            getzrange(pSprite->x+sprite[t].x-sprite[nSprite].x, pSprite->y+sprite[t].y-sprite[nSprite].y,
                pSprite->z+sprite[t].z-sprite[nSprite].z, sprite[t].sectnum, &nTemp1, &nTemp2, floorZ, floorHit,
                nDist, nMask);
            *floorZ -= sprite[t].z - sprite[nSprite].z;
        }
    }
    if (((*ceilHit) & 0xe000) == 0x4000)
    {
        int nSector = (*ceilHit) & 0x1fff;
        if ((nMask & 0x1000) == 0 && (sector[nSector].ceilingstat & kSectorStat0))
            *ceilZ = 0x80000000;
        int nSprite = gLowerLink[nSector];
        if (nSprite >= 0)
        {
            int nLink = sprite[nSprite].owner;
            int t = nLink & 0xfff;
            getzrange(pSprite->x+sprite[t].x-sprite[nSprite].x, pSprite->y+sprite[t].y-sprite[nSprite].y,
                pSprite->z+sprite[t].z-sprite[nSprite].z, sprite[t].sectnum, ceilZ, ceilHit, &nTemp1, &nTemp2,
                nDist, nMask);
            *ceilZ -= sprite[t].z - sprite[nSprite].z;
        }
    }
    pSprite->cstat = bakCstat;
}

void GetZRangeAtXYZ(long x, long y, long z, int nSector, long *ceilZ, long *ceilHit, long *floorZ, long *floorHit, int nDist, unsigned long nMask)
{
    long nTemp1, nTemp2;
    getzrange(x, y, z, nSector, ceilZ, ceilHit, floorZ, floorHit, nDist, nMask);
    if (((*floorHit) & 0xe000) == 0x4000)
    {
        int nSector = (*floorHit) & 0x1fff;
        if ((nMask & 0x2000) == 0 && (sector[nSector].floorstat & kSectorStat0))
            *floorZ = 0x7fffffff;
        int nXSector = sector[nSector].extra;
        if (sector[nSector].extra > 0)
        {
            XSECTOR* pXSector = &xsector[nXSector];
            *floorZ += pXSector->at13_5 << 10;
        }
        int nSprite = gUpperLink[nSector];
        if (nSprite >= 0)
        {
            int nLink = sprite[nSprite].owner;
            int t = nLink & 0xfff;
            getzrange(x+sprite[t].x-sprite[nSprite].x, y+sprite[t].y-sprite[nSprite].y,
                z+sprite[t].z-sprite[nSprite].z, sprite[t].sectnum, &nTemp1, &nTemp2, floorZ, floorHit,
                nDist, nMask);
            *floorZ -= sprite[t].z - sprite[nSprite].z;
        }
    }
    if (((*ceilHit) & 0xe000) == 0x4000)
    {
        int nSector = (*ceilHit) & 0x1fff;
        if ((nMask & 0x1000) == 0 && (sector[nSector].ceilingstat & kSectorStat0))
            *ceilZ = 0x80000000;
        int nSprite = gLowerLink[nSector];
        if (nSprite >= 0)
        {
            int nLink = sprite[nSprite].owner;
            int t = nLink & 0xfff;
            getzrange(x+sprite[t].x-sprite[nSprite].x, y+sprite[t].y-sprite[nSprite].y,
                z+sprite[t].z-sprite[nSprite].z, sprite[t].sectnum, ceilZ, ceilHit, &nTemp1, &nTemp2,
                nDist, nMask);
            *ceilZ -= sprite[t].z - sprite[nSprite].z;
        }
    }
}

int GetDistToLine(int x1, int y1, int x2, int y2, int x3, int y3)
{
    int dx = x3 - x2;
    int dy = y3 - y2;
    int px = x1 - x2;
    int py = y1 - y3;
    if (px * dy > py * dx)
        return -1;
    int v8 = dmulscale(px,dx,py,dy,4);
    int vv = dmulscale(dx,dx,dy,dy,4);
    int t1, t2;
    if (v8 <= 0)
    {
        t1 = x2;
        t2 = x3;
    }
    else if (v8 < vv)
    {
        t1 = x2+kscale(dx,v8,vv);
        t2 = y2+kscale(dy,v8,vv);
    }
    else
    {
        t1 = x3;
        t2 = y3;
    }
    return approxDist(t1-x1, t2-y1);
}

uint ClipMove(long *x, long *y, long *z, int *nSector, long xv, long yv, int wd, int cd, int fd, ulong nMask)
{
    int bakX = *x;
    int bakY = *y;
    int bakZ = *z;
    short bakSect = *nSector;
    uint nRes = clipmove(x, y, z, &bakSect, xv<<14, yv<<14, wd, cd, fd, nMask);
    if (bakSect == -1)
    {
        *x = bakX; *y = bakY; *z = bakZ;
    }
    else
    {
        *nSector = bakSect;
    }
    return nRes;
}

BOOL cansee(long x1, long y1, long z1, short sect1, long x2, long y2, long z2, short sect2) // UNUSED
{
    int x, y, z, ceilz, floorz, bot, t;
    short sectlist[512];

    if (x1 == x2 && y1 == y2)
        return sect1 == sect2;
    int x21 = x2 - x1;
    int y21 = y2 - y1;
    int z21 = z2 - z1;
    sectlist[0] = sect1;
    int n = 1;
    for (int i = 0; i < n; i++)
    {
        short cursect = sectlist[i];
        SECTOR *psect = &sector[cursect];
        WALL *pwall = &wall[psect->wallptr];
        for (int walnum = psect->wallnum; walnum > 0; walnum--, pwall++)
        {
            WALL *pwall2 = &wall[pwall->point2];
            int x31 = pwall->x - x1;
            int x34 = pwall->x - pwall2->x;
            int y31 = pwall->y - y1;
            int y34 = pwall->y - pwall2->y;
            bot = y21 * x34 - x21 * y34;
            if (bot <= 0)
                continue;
            //t = y21 * x31 - x21 * y31;
            //if (t < 0 || t >= bot)
            if ((unsigned)bot <= (unsigned)(y21 * x31 - x21 * y31))
                continue;
            t = y31 * x34 - x31 * y34;
            //if (t < 0 || t >= bot)
            if ((unsigned)t >= (unsigned)bot)
                continue;
            t = divscale24(t, bot);
            x = x1 + mulscale24(x21, t);
            y = y1 + mulscale24(y21, t);
            z = z1 + mulscale24(z21, t);
            short newsector;
            getzsofslope(cursect, x, y, &ceilz, &floorz);
            if (z <= ceilz || z >= floorz)
            {
                if (z <= ceilz)
                {
                    int nSprite = gLowerLink[cursect];
                    if (nSprite < 0)
                        return 0;
                    int nLink = sprite[nSprite].owner;
                    int t = nLink & 0xfff;
                    newsector = sprite[t].sectnum;
                }
                if (z >= floorz)
                {
                    int nSprite = gUpperLink[cursect];
                    if (nSprite < 0)
                        return 0;
                    int nLink = sprite[nSprite].owner;
                    int t = nLink & 0xfff;
                    newsector = sprite[t].sectnum;
                }
            }
            else
            {
                newsector = pwall->nextsector;
                if (newsector < 0 || (pwall->cstat & kWallStat5))
                    return 0;
                getzsofslope(newsector, x, y, &ceilz, &floorz);
                if (z <= ceilz || z >= floorz)
                    return 0;
            }
            for (int j = n - 1; j >= 0; j--)
            {
                if (sectlist[j] == newsector) break;
            }
            if (j < 0)
            {
                sectlist[n++] = newsector;
            }
        }
    }
    for (i = n - 1; i >= 0; i--)
    {
        if (sectlist[i] == sect2)
            return 1;
    }
    return 0;
}

int GetClosestSectors(int nSector, int x, int y, int nDist, short *pSectors, byte *pSectBit)
{
    byte sectbits[(kMaxSectors+7)>>3];
    dassert(pSectors != NULL, 1359);
    memset(sectbits, 0, sizeof(sectbits));
    pSectors[0] = nSector;
    SetBitString(sectbits, nSector);
    int n = 1;
    int m = 1;
    int i = 0;
    if (pSectBit)
    {
        memset(pSectBit, 0, (kMaxSectors+7)>>3);
        SetBitString(pSectBit, nSector);
    }
    for (; i < n; i++)
    {
        int nCurSector = pSectors[i];
        int nStartWall = sector[nCurSector].wallptr;
        int nEndWall = nStartWall + sector[nCurSector].wallnum;
        int j = nStartWall;
        WALL *pWall = &wall[j];
        for (; j < nEndWall; j++, pWall++)
        {
            int nNextSector = pWall->nextsector;
            if (nNextSector < 0)
                continue;
            if (!TestBitString(sectbits, nNextSector))
            {
                SetBitString(sectbits, nNextSector);
                int dx = klabs(wall[wall[j].point2].x - x) >> 4;
                if (dx >= nDist)
                    continue;
                int dy = klabs(wall[wall[j].point2].y - y) >> 4;
                if (dy >= nDist || approxDist(dx, dy) >= nDist)
                    continue;
                else
                {
                    if (pSectBit != NULL)
                    {
                        SetBitString(pSectBit, nNextSector);
                    }
                    pSectors[n++] = nNextSector;
                    m++;
                }
            }
        }
    }
    pSectors[m] = -1;
    return m;
}

int GetClosestSpriteSectors(int nSector, int x, int y, int nDist, short *pSectors, byte *pSectBit, short *a8)
{
    byte sectbits[(kMaxSectors+7)>>3];
    dassert(pSectors != NULL, 1359);
    int n = 0;
    int m = 1;
    memset(sectbits, 0, sizeof(sectbits));
    pSectors[n++] = nSector;
    SetBitString(sectbits, nSector);
    int i = 0;
    int k = 0;
    if (pSectBit)
    {
        memset(pSectBit, 0, (kMaxSectors+7)>>3);
        SetBitString(pSectBit, nSector);
    }
    for (; i < n; i++)
    {
        int nCurSector = pSectors[i];
        int nStartWall = sector[nCurSector].wallptr;
        int nEndWall = nStartWall + sector[nCurSector].wallnum;
        int j = nStartWall;
        WALL *pWall = &wall[j];
        for (; j < nEndWall; j++, pWall++)
        {
            int nNextSector = pWall->nextsector;
            if (nNextSector < 0)
                continue;
            if (!TestBitString(sectbits, nNextSector))
            {
                SetBitString(sectbits, nNextSector);
                if (CheckProximityWall(wall[j].point2, x, y, nDist))
                {
                    if (pSectBit)
                        SetBitString(pSectBit, nNextSector);
                    pSectors[n++] = nNextSector;
                    m++;
                    if (a8 && pWall->extra > 0)
                    {
                        XWALL* pXWall = &xwall[pWall->extra];
                        if (pXWall->at10_6 && !pXWall->at10_1 && !pXWall->at1_6)
                        {
                            a8[k] = j;
                            k++;
                        }
                    }
                }
            }
        }
    }
    pSectors[m] = -1;
    if (a8)
        a8[k] = -1;
    return m;
}
