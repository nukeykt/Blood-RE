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
#include "build.h"
#include "db.h"
#include "debug4g.h"
#include "helix.h"
#include "error.h"
#include "gameutil.h"
#include "globals.h"
#include "loadsave.h"
#include "mirrors.h"
#include "player.h"
#include "trig.h"
#include "view.h"

struct MIRROR
{
    int at0;
    int at4;
    int at8;
    int atc;
    int at10;
    int at14;
};

static MIRROR mirror[16];

static int mirrorcnt, mirrorsector, mirrorwall[4];

void InitMirrors(void)
{
    int i, j;
    mirrorcnt = 0;
    tilesizx[504] = 0;
    tilesizy[504] = 0;
    
    for (i = 0; i < 16; i++)
        tilesizx[4080+i] = 0, tilesizy[4080+i] = 0;
    for (i = numwalls - 1; i >= 0; i--)
    {
        if (mirrorcnt == 16)
            break;
        if (wall[i].overpicnum == 504)
        {
            if (wall[i].extra > 0 && wall[i].type == 501)
            {
                wall[i].overpicnum = 4080+mirrorcnt;
                wall[i].cstat |= 32;
                mirror[mirrorcnt].at0 = 0;
                mirror[mirrorcnt].at14 = i;
                int data = xwall[wall[i].extra].at4_0;
                for (j = numwalls - 1; j >= 0; j--)
                {
                    if (j == i)
                        continue;
                    if (wall[j].extra > 0 && wall[j].type == 501)
                    {
                        if (data != xwall[wall[j].extra].at4_0)
                            continue;
                        wall[i].hitag = j;
                        wall[j].hitag = i;
                        mirror[mirrorcnt].at4 = j;
                        break;
                    }
                }
                if (j < 0)
                    ThrowError(20)("wall[%d] has no matching wall link! (data=%d)\n", i, data);
                mirrorcnt++;
            }
            continue;
        }
        if (wall[i].picnum == 504)
        {
            wall[i].picnum = 4080+mirrorcnt;
            wall[i].cstat |= 32;
            mirror[mirrorcnt].at0 = 0;
            mirror[mirrorcnt].at4 = i;
            mirror[mirrorcnt].at14 = i;
            mirrorcnt++;
            continue;
        }
    }
    for (i = numsectors - 1; i >= 0; i--)
    {
        if (mirrorcnt >= 15)
            break;

        if (sector[i].floorpicnum == 504)
        {
            int nLink = gUpperLink[i];
            if (nLink < 0)
                continue;
            int nLink2 = sprite[nLink].owner & kSpriteOwnerMask;
            j = sprite[nLink2].sectnum;
            if (sector[j].ceilingpicnum != 504)
                ThrowError(163)("Lower link sector %d doesn't have mirror picnum!\n", j);
            mirror[mirrorcnt].at0 = 2;
            mirror[mirrorcnt].at4 = j;
            mirror[mirrorcnt].at8 = sprite[nLink2].x-sprite[nLink].x;
            mirror[mirrorcnt].atc = sprite[nLink2].y-sprite[nLink].y;
            mirror[mirrorcnt].at10 = sprite[nLink2].z-sprite[nLink].z;
            mirror[mirrorcnt].at14 = i;
            sector[i].floorpicnum = 4080+mirrorcnt;
            mirrorcnt++;
            mirror[mirrorcnt].at0 = 1;
            mirror[mirrorcnt].at4 = i;
            mirror[mirrorcnt].at8 = sprite[nLink].x-sprite[nLink2].x;
            mirror[mirrorcnt].atc = sprite[nLink].y-sprite[nLink2].y;
            mirror[mirrorcnt].at10 = sprite[nLink].z-sprite[nLink2].z;
            mirror[mirrorcnt].at14 = j;
            sector[j].ceilingpicnum = 4080+mirrorcnt;
            mirrorcnt++;
        }
    }
    mirrorsector = numsectors;
    for (i = 0; i < 4; i++)
    {
        mirrorwall[i] = numwalls+i;
        wall[mirrorwall[i]].picnum = 504;
        wall[mirrorwall[i]].overpicnum = 504;
        wall[mirrorwall[i]].point2 = numwalls;
        wall[mirrorwall[i]].cstat = 0;
        wall[mirrorwall[i]].nextsector = -1;
        wall[mirrorwall[i]].nextwall = -1;
    }
    wall[mirrorwall[3]].point2 = mirrorwall[0];
    sector[mirrorsector].ceilingpicnum = 504;
    sector[mirrorsector].floorpicnum = 504;
    sector[mirrorsector].wallptr = mirrorwall[0];
    sector[mirrorsector].wallnum = 4;
}

void TranslateMirrorColors(int nShade, int nPalette)
{
    nShade = ClipRange(nShade, 0, 63);
    byte *pMap = palookup[nPalette] + (nShade<<8);
    byte *pFrame = (byte*)gPageTable[0].begin;
    for (int i = 0; i < gPageTable[0].size; i++, pFrame++)
    {
        *pFrame = pMap[*pFrame];
    }
}

void func_5571C(BOOL mode)
{
    for (int i = mirrorcnt-1; i >= 0; i--)
    {
        if (TestBitString(gotpic, 4080+i))
        {
            int nSector = mirror[i].at14;
            switch (mirror[i].at0)
            {
                case 1:
                    if (mode)
                        sector[nSector].ceilingstat |= 1;
                    else
                        sector[nSector].ceilingstat &= ~1;
                    break;
                case 2:
                    if (mode)
                        sector[nSector].floorstat |= 1;
                    else
                        sector[nSector].floorstat &= ~1;
                    break;
            }
        }
    }
}

void func_557C4(long x, long y, int interpolate)
{
    int nTSprite;
    int nViewSprites = spritesortcnt;
    for (nTSprite = nViewSprites-1; nTSprite >= 0; nTSprite--)
    {
        SPRITE *pTSprite = &tsprite[nTSprite];
        pTSprite->xrepeat = pTSprite->yrepeat = 0;
    }
    for (int i = mirrorcnt-1; i >= 0; i--)
    {
        if (TestBitString(gotpic, 4080+i))
        {
            if (mirror[i].at0 == 1 || mirror[i].at0 == 2)
            {
                int nSector = mirror[i].at4;
                int nSector2 = mirror[i].at14;
                for (int nSprite = headspritesect[nSector]; nSprite >= 0; nSprite = nextspritesect[nSprite])
                {
                    SPRITE *pSprite = &sprite[nSprite];
                    if (pSprite == gView->pSprite)
                        continue;
                    int top, bottom;
                    GetSpriteExtents(pSprite, &top, &bottom);
                    int zCeil, zFloor;
                    getzsofslope(nSector, pSprite->x, pSprite->y, &zCeil, &zFloor);
                    if (pSprite->statnum == 6 && (top < zCeil || bottom > zFloor))
                    {
                        int j;
                        if (mirror[i].at0 == 2)
                            j = i + 1;
                        else
                            j = i - 1;
                        int dx = mirror[j].at8;
                        int dy = mirror[j].atc;
                        int dz = mirror[j].at10;
                        SPRITE *pTSprite = &tsprite[spritesortcnt];
                        memset(pTSprite, 0, sizeof(SPRITE));
                        pTSprite->type = pSprite->type;
                        pTSprite->index = pSprite->index;
                        pTSprite->sectnum = nSector2;
                        pTSprite->x = pSprite->x+dx;
                        pTSprite->y = pSprite->y+dy;
                        pTSprite->z = pSprite->z+dz;
                        pTSprite->ang = pSprite->ang;
                        pTSprite->picnum = pSprite->picnum;
                        pTSprite->shade = pSprite->shade;
                        pTSprite->pal = pSprite->pal;
                        pTSprite->xrepeat = pSprite->xrepeat;
                        pTSprite->yrepeat = pSprite->yrepeat;
                        pTSprite->xoffset = pSprite->xoffset;
                        pTSprite->yoffset = pSprite->yoffset;
                        pTSprite->cstat = pSprite->cstat;
                        pTSprite->statnum = 0;
                        pTSprite->owner = pSprite->index;
                        pTSprite->extra = pSprite->extra;
                        pTSprite->flags = pSprite->flags|0x200;
                        LOCATION *pLocation = &gPrevSpriteLoc[pSprite->index];
                        pTSprite->x = dx+interpolate16(pLocation->x, pSprite->x, interpolate);
                        pTSprite->y = dy+interpolate16(pLocation->y, pSprite->y, interpolate);
                        pTSprite->z = dz+interpolate16(pLocation->z, pSprite->z, interpolate);
                        int delta = ((pSprite->ang-pLocation->ang+1024)&2047)-1024;
                        pTSprite->ang = pLocation->ang+mulscale16(delta, interpolate);
                        spritesortcnt++;
                    }
                }
            }
        }
    }
    for (nTSprite = spritesortcnt-1; nTSprite >= nViewSprites; nTSprite--)
    {
        SPRITE *pTSprite = &tsprite[nTSprite];
        int nAnim = 0;
        switch (picanm[pTSprite->picnum].at3_4)
        {
            case 1:
            {
                long dX, dY;
                dX = x - pTSprite->x;
                dY = y - pTSprite->y;
                RotateVector(&dX, &dY, -pTSprite->ang+128);
                int nOctant = GetOctant(dX, dY);
                if (nOctant <= 4)
                {
                    nAnim = nOctant;
                    pTSprite->cstat &= ~4;
                }
                else
                {
                    nAnim = 8 - nOctant;
                    pTSprite->cstat |= 4;
                }
                break;
            }
            case 2:
            {
                long dX, dY;
                dX = x - pTSprite->x;
                dY = y - pTSprite->y;
                RotateVector(&dX, &dY, -pTSprite->ang+128);
                int nOctant = GetOctant(dX, dY);
                nAnim = nOctant;
                break;
            }
        }
        for (; nAnim > 0; nAnim--)
        {
            pTSprite->picnum += (short)(1+picanm[pTSprite->picnum].animframes);
        }
    }
}

void DrawMirrors(long x, long y, long z, int a, long horiz)
{
    int i;
    int cx, cy;
    short ca;
    for (i = mirrorcnt - 1; i >= 0; i--)
    {
        if (TestBitString(gotpic, 4080+i))
        {
            ClearBitString(gotpic, 4080+i);
            switch (mirror[i].at0)
            {
                case 0:
                {
                    int nWall = mirror[i].at4;
                    WALL *pWall = &wall[nWall];
                    int nSector = (short)sectorofwall(nWall);
                    int nNextWall = pWall->nextwall;
                    int nNextSector = pWall->nextsector;
                    pWall->nextwall = mirrorwall[0];
                    pWall->nextsector = mirrorsector;
                    wall[mirrorwall[0]].nextwall = nWall;
                    wall[mirrorwall[0]].nextsector = nSector;
                    wall[mirrorwall[0]].x = wall[pWall->point2].x;
                    wall[mirrorwall[0]].y = wall[pWall->point2].y;
                    wall[mirrorwall[1]].x = pWall->x;
                    wall[mirrorwall[1]].y = pWall->y;
                    wall[mirrorwall[2]].x = wall[mirrorwall[1]].x+(wall[mirrorwall[1]].x-wall[mirrorwall[0]].x)*16;
                    wall[mirrorwall[2]].y = wall[mirrorwall[1]].y+(wall[mirrorwall[1]].y-wall[mirrorwall[0]].y)*16;
                    wall[mirrorwall[3]].x = wall[mirrorwall[0]].x+(wall[mirrorwall[0]].x-wall[mirrorwall[1]].x)*16;
                    wall[mirrorwall[3]].y = wall[mirrorwall[0]].y+(wall[mirrorwall[0]].y-wall[mirrorwall[1]].y)*16;
                    sector[mirrorsector].floorz = sector[nSector].floorz;
                    sector[mirrorsector].ceilingz = sector[nSector].ceilingz;
                    if (pWall->type == 501)
                    {
                         cx = x - (wall[pWall->hitag].x-wall[pWall->point2].x);
                         cy = y - (wall[pWall->hitag].y-wall[pWall->point2].y);
                         ca = a;
                    }
                    else
                    {
                        preparemirror(x,y,z,a,horiz,nWall,mirrorsector,&cx,&cy,&ca);
                    }
                    drawrooms(cx, cy, z, ca,horiz,mirrorsector|kMaxSectors);
                    viewProcessSprites(cx,cy,z);
                    drawmasks();
                    if (pWall->type != 501)
                        completemirror();
                    if (wall[nWall].pal != 0 || wall[nWall].shade)
                        TranslateMirrorColors(wall[nWall].shade, wall[nWall].pal);
                    pWall->nextwall = nNextWall;
                    pWall->nextsector = nNextSector;
                    return;
                }
                case 1:
                {
                    int nSector = mirror[i].at4;
                    drawrooms(x+mirror[i].at8, y+mirror[i].atc, z+mirror[i].at10, a, horiz, nSector|kMaxSectors);
                    viewProcessSprites(x+mirror[i].at8, y+mirror[i].atc, z+mirror[i].at10);
                    short fstat = sector[nSector].floorstat;
                    sector[nSector].floorstat |= 1;
                    drawmasks();
                    sector[nSector].floorstat = fstat;
                    return;
                }
                case 2:
                {
                    int nSector = mirror[i].at4;
                    drawrooms(x+mirror[i].at8, y+mirror[i].atc, z+mirror[i].at10, a, horiz, nSector|kMaxSectors);
                    viewProcessSprites(x+mirror[i].at8, y+mirror[i].atc, z+mirror[i].at10);
                    short cstat = sector[nSector].ceilingstat;
                    sector[nSector].ceilingstat |= 1;
                    drawmasks();
                    sector[nSector].ceilingstat = cstat;
                    return;
                }
            }
            break;
        }
    }
}

class MirrorLoadSave : public LoadSave {
public:
    void Load(void);
    void Save(void);
};

void MirrorLoadSave::Load(void)
{
    int i;
    Read(&mirrorcnt,sizeof(mirrorcnt));
    Read(&mirrorsector,sizeof(mirrorsector));
    Read(mirror, sizeof(mirror));
    Read(mirrorwall, sizeof(mirrorwall));
    tilesizx[504] = 0;
    tilesizy[504] = 0;

    for (i = 0; i < 16; i++)
        tilesizx[4080 + i] = 0, tilesizy[4080 + i] = 0;
    for (i = 0; i < 4; i++)
    {
        wall[mirrorwall[i]].picnum = 504;
        wall[mirrorwall[i]].overpicnum = 504;
        wall[mirrorwall[i]].point2 = numwalls;
        wall[mirrorwall[i]].cstat = 0;
        wall[mirrorwall[i]].nextsector = -1;
        wall[mirrorwall[i]].nextwall = -1;
    }
    wall[mirrorwall[3]].point2 = mirrorwall[0];
    sector[mirrorsector].ceilingpicnum = 504;
    sector[mirrorsector].floorpicnum = 504;
    sector[mirrorsector].wallptr = mirrorwall[0];
    sector[mirrorsector].wallnum = 4;
}

void MirrorLoadSave::Save(void)
{
    Write(&mirrorcnt,sizeof(mirrorcnt));
    Write(&mirrorsector,sizeof(mirrorsector));
    Write(mirror, sizeof(mirror));
    Write(mirrorwall, sizeof(mirrorwall));
}

static MirrorLoadSave myLoadSave;
