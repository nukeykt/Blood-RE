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
#include <stdio.h>
#include "typedefs.h"
#include "build.h"
#include "config.h"
#include "gameutil.h"
#include "levels.h"
#include "map2d.h"
#include "misc.h"
#include "player.h"
#include "trig.h"
#include "view.h"


CViewMap gViewMap;

int drawoverheadmap(long cposx, long cposy, long czoom, short cang)
{
        long i, j, k, l, x1, y1, x2, y2, x3, y3, x4, y4, ox, oy, xoff, yoff;
        long dax, day, cosang, sinang, xspan, yspan, sprx, spry;
        long xrepeat, yrepeat, z1, z2, startwall, endwall, tilenum, daang;
        long xvect, yvect, xvect2, yvect2;
        short p;
        char col;
        WALL *wal, *wal2;
        SPRITE *spr;

        xvect = sintable[(-cang)&2047] * czoom;
        yvect = sintable[(1536-cang)&2047] * czoom;
        xvect2 = mulscale16(xvect,yxaspect);
        yvect2 = mulscale16(yvect,yxaspect);

                //Draw red lines
        for(i=0;i<numsectors;i++)
        {
                if (!gFullMap && !(show2dsector[i>>3]&(1<<(i&7)))) continue;

                startwall = sector[i].wallptr;
                endwall = sector[i].wallptr + sector[i].wallnum;

                z1 = sector[i].ceilingz; z2 = sector[i].floorz;

                for(j=startwall,wal=&wall[startwall];j<endwall;j++,wal++)
                {
                        k = wal->nextwall; if (k < 0) continue;

                        //if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;
                        //if ((k > j) && ((show2dwall[k>>3]&(1<<(k&7))) > 0)) continue;

                        if (sector[wal->nextsector].ceilingz == z1)
                                if (sector[wal->nextsector].floorz == z2)
                                        if (((wal->cstat|wall[wal->nextwall].cstat)&(16+32)) == 0) continue;

                        col = 139; //red
                        if ((wal->cstat|wall[wal->nextwall].cstat)&1) col = 234; //magenta

                        if (!gFullMap && !(show2dsector[wal->nextsector>>3]&(1<<(wal->nextsector&7))))
                                col = 24;
            else continue;

                        ox = wal->x-cposx; oy = wal->y-cposy;
                        x1 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
                        y1 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

                        wal2 = &wall[wal->point2];
                        ox = wal2->x-cposx; oy = wal2->y-cposy;
                        x2 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
                        y2 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

                        drawline256(x1,y1,x2,y2,col);
                }
        }

                //Draw sprites
        k = gView->pSprite->index;
        for(i=0;i<numsectors;i++)
        {
                if (!gFullMap && !(show2dsector[i>>3]&(1<<(i&7)))) continue;
                for(j=headspritesect[i];j>=0;j=nextspritesect[j])
                        //if ((show2dsprite[j>>3]&(1<<(j&7))) > 0)
                        {
                spr = &sprite[j];

                if (j == k || (spr->cstat&0x8000) || spr->cstat == 257 || spr->xrepeat == 0) continue;

                                col = 71; //cyan
                                if (spr->cstat&1) col = 234; //magenta

                                sprx = spr->x;
                                spry = spr->y;

                                if ((spr->cstat & 257) != 0) switch (spr->cstat & 48)
                                {
                                case 0:
                                    break;
                                case 16:
                                    break;
                                case 32:
                                    break;
                                }
#if 0
                if( (spr->cstat&257) != 0) switch (spr->cstat&48)
                                {
                    case 0: break;

                                                ox = sprx-cposx; oy = spry-cposy;
                                                x1 = dmulscale16(ox,xvect,-oy,yvect);
                                                y1 = dmulscale16(oy,xvect2,ox,yvect2);

                                                ox = (sintable[(spr->ang+512)&2047]>>7);
                                                oy = (sintable[(spr->ang)&2047]>>7);
                                                x2 = dmulscale16(ox,xvect,-oy,yvect);
                                                y2 = dmulscale16(oy,xvect,ox,yvect);

                                                x3 = mulscale16(x2,yxaspect);
                                                y3 = mulscale16(y2,yxaspect);

                                                drawline256(x1-x2+(xdim<<11),y1-y3+(ydim<<11),
                                                                                x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                                                drawline256(x1-y2+(xdim<<11),y1+x3+(ydim<<11),
                                                                                x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                                                drawline256(x1+y2+(xdim<<11),y1-x3+(ydim<<11),
                                                                                x1+x2+(xdim<<11),y1+y3+(ydim<<11),col);
                        break;

                                        case 16:
                        if( spr->picnum == LASERLINE )
                        {
                            x1 = sprx; y1 = spry;
                            tilenum = spr->picnum;
                            xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
                            if ((spr->cstat&4) > 0) xoff = -xoff;
                            k = spr->ang; l = spr->xrepeat;
                            dax = sintable[k&2047]*l; day = sintable[(k+1536)&2047]*l;
                            l = tilesizx[tilenum]; k = (l>>1)+xoff;
                            x1 -= mulscale16(dax,k); x2 = x1+mulscale16(dax,l);
                            y1 -= mulscale16(day,k); y2 = y1+mulscale16(day,l);

                            ox = x1-cposx; oy = y1-cposy;
                            x1 = dmulscale16(ox,xvect,-oy,yvect);
                            y1 = dmulscale16(oy,xvect2,ox,yvect2);

                            ox = x2-cposx; oy = y2-cposy;
                            x2 = dmulscale16(ox,xvect,-oy,yvect);
                            y2 = dmulscale16(oy,xvect2,ox,yvect2);

                            drawline256(x1+(xdim<<11),y1+(ydim<<11),
                                                                                x2+(xdim<<11),y2+(ydim<<11),col);
                        }

                        break;

                    case 32:
                                                tilenum = spr->picnum;
                                                xoff = (long)((signed char)((picanm[tilenum]>>8)&255))+((long)spr->xoffset);
                                                yoff = (long)((signed char)((picanm[tilenum]>>16)&255))+((long)spr->yoffset);
                                                if ((spr->cstat&4) > 0) xoff = -xoff;
                                                if ((spr->cstat&8) > 0) yoff = -yoff;

                                                k = spr->ang;
                                                cosang = sintable[(k+512)&2047]; sinang = sintable[k];
                                                xspan = tilesizx[tilenum]; xrepeat = spr->xrepeat;
                                                yspan = tilesizy[tilenum]; yrepeat = spr->yrepeat;

                                                dax = ((xspan>>1)+xoff)*xrepeat; day = ((yspan>>1)+yoff)*yrepeat;
                                                x1 = sprx + dmulscale16(sinang,dax,cosang,day);
                                                y1 = spry + dmulscale16(sinang,day,-cosang,dax);
                                                l = xspan*xrepeat;
                                                x2 = x1 - mulscale16(sinang,l);
                                                y2 = y1 + mulscale16(cosang,l);
                                                l = yspan*yrepeat;
                                                k = -mulscale16(cosang,l); x3 = x2+k; x4 = x1+k;
                                                k = -mulscale16(sinang,l); y3 = y2+k; y4 = y1+k;

                                                ox = x1-cposx; oy = y1-cposy;
                                                x1 = dmulscale16(ox,xvect,-oy,yvect);
                                                y1 = dmulscale16(oy,xvect2,ox,yvect2);

                                                ox = x2-cposx; oy = y2-cposy;
                                                x2 = dmulscale16(ox,xvect,-oy,yvect);
                                                y2 = dmulscale16(oy,xvect2,ox,yvect2);

                                                ox = x3-cposx; oy = y3-cposy;
                                                x3 = dmulscale16(ox,xvect,-oy,yvect);
                                                y3 = dmulscale16(oy,xvect2,ox,yvect2);

                                                ox = x4-cposx; oy = y4-cposy;
                                                x4 = dmulscale16(ox,xvect,-oy,yvect);
                                                y4 = dmulscale16(oy,xvect2,ox,yvect2);

                                                drawline256(x1+(xdim<<11),y1+(ydim<<11),
                                                                                x2+(xdim<<11),y2+(ydim<<11),col);

                                                drawline256(x2+(xdim<<11),y2+(ydim<<11),
                                                                                x3+(xdim<<11),y3+(ydim<<11),col);

                                                drawline256(x3+(xdim<<11),y3+(ydim<<11),
                                                                                x4+(xdim<<11),y4+(ydim<<11),col);

                                                drawline256(x4+(xdim<<11),y4+(ydim<<11),
                                                                                x1+(xdim<<11),y1+(ydim<<11),col);

                                                break;
                                }
#endif
                        }
        }

                //Draw white lines
        for(i=0;i<numsectors;i++)
        {
                if (!gFullMap && !(show2dsector[i>>3]&(1<<(i&7)))) continue;

                startwall = sector[i].wallptr;
                endwall = sector[i].wallptr + sector[i].wallnum;

                k = -1;
                for(j=startwall,wal=&wall[startwall];j<endwall;j++,wal++)
                {
                        if (wal->nextwall >= 0) continue;

                        //if ((show2dwall[j>>3]&(1<<(j&7))) == 0) continue;

                        if (tilesizx[wal->picnum] == 0) continue;
                        if (tilesizy[wal->picnum] == 0) continue;

                        if (j == k)
                                { x1 = x2; y1 = y2; }
                        else
                        {
                                ox = wal->x-cposx; oy = wal->y-cposy;
                                x1 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
                                y1 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);
                        }

                        k = wal->point2; wal2 = &wall[k];
                        ox = wal2->x-cposx; oy = wal2->y-cposy;
                        x2 = dmulscale16(ox,xvect,-oy,yvect)+(xdim<<11);
                        y2 = dmulscale16(oy,xvect2,ox,yvect2)+(ydim<<11);

                        drawline256(x1,y1,x2,y2,24);
                }
        }

         for(p=connecthead;p >= 0;p=connectpoint2[p])
         {
          if(!gViewMap.Mode() && p == gView->at57) continue;

          PLAYER *pPlayer = &gPlayer[p];
          SPRITE *spr = pPlayer->pSprite;
          ox = spr->x-cposx; oy = spr->y-cposy;
                  daang = (spr->ang-cang)&2047;
                  if (p == gView->at57) { ox = 0; oy = 0; daang = 0; }
                  x1 = mulscale(ox,xvect,16) - mulscale(oy,yvect,16);
                  y1 = mulscale(oy,xvect2,16) + mulscale(ox,yvect2,16);

          if(p == gView->at57 || gGameOptions.nGameType == GAMETYPE_1 )
          {
                i = spr->picnum;
                long ceilZ, ceilHit, floorZ, floorHit;
                GetZRange(spr, &ceilZ, &ceilHit, &floorZ, &floorHit, (spr->clipdist<<2)+16, 0x13001);
                int nTop, nBottom;
                GetSpriteExtents(spr, &nTop, &nBottom);

                j = klabs(floorZ-nBottom)>>8;
                j = mulscale(czoom*(spr->yrepeat+j),yxaspect,16);

                if(j < 8000) j = 8000;
                else if(j > 65536) j = 65536;

                if (gView->at87)
                    j = 8000;

                rotatesprite((x1<<4)+(xdim<<15),(y1<<4)+(ydim<<15),j,
                    daang,i,spr->shade,spr->pal,
                    (spr->cstat&2)>>1,windowx1,windowy1,windowx2,windowy2);
          }
         }
    return 0;
}

CViewMap::CViewMap()
{
    bActive = FALSE;
}

void CViewMap::func_25C38(int a1, int a2, int a3, short a4, BOOL a5)
{
    bActive = TRUE;
    SetPos(a1, a2);
    SetAngle(a4);
    SetZoom(a3);
    FollowMode(a5);
    SetInput(0, 0, 0);
}

void CViewMap::func_25C74(void)
{
    char buffer[128];
    if (!bActive)
        return;
    BOOL tm = 0;
    int vdi = gViewSize;
    if (gViewSize > 2)
    {
        viewResizeView(2);
        tm = 1;
    }
    clearview(0);
    drawmapview(x,y,nZoom>>2,angle);
    drawoverheadmap(x,y,nZoom>>2,angle);
    char *pTitle = levelGetTitle();
    char *pFilename = levelGetFilename(gGameOptions.nEpisode, gGameOptions.nLevel);
    if (pTitle)
        sprintf(buffer, "%s: %s", pFilename, pTitle);
    else
        sprintf(buffer, "%s", pFilename);
    int nViewY = gViewSize > 3 ? gViewY1S-16 : gViewY0S+1;
    viewDrawText(3, buffer, gViewX1S, nViewY, -128, 0, 2, 0);

    char *pMsg = gViewMap.Mode() ? "MAP FOLLOW MODE" : "MAP SCROLL MODE";
    viewDrawText(3, pMsg, gViewX1S, nViewY+8, -128, 0, 2, 0);

    if (tm)
        viewResizeView(vdi);
}

void CViewMap::func_25DB0(SPRITE *pSprite)
{
    SetZoom(gZoom);
    if (Mode())
    {
        SetPos(pSprite->x, pSprite->y);
        SetAngle(pSprite->ang);
    }
    else
    {
        angle += turn>>3;
        angle &= 2047;
        x += mulscale24(forward, Cos(angle));
        y += mulscale24(forward, Sin(angle));
        x += mulscale24(strafe, Cos(angle+512));
        y += mulscale24(strafe, Sin(angle+512));
    }
    func_25C74();
}

void CViewMap::func_25E84(int *_x, int *_y)
{
    if (_x)
        *_x = x;
    if (_y)
        *_y = y;
}

void CViewMap::FollowMode(BOOL a1)
{
    bFollowMode = a1;
}
