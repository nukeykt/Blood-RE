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
#pragma initialize library
#include <stdio.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <sys\stat.h>
#include "typedefs.h"
#include "asound.h"
#include "build.h"
#include "cdrom.h"
#include "config.h"
#include "db.h"
#include "debug4g.h"
#include "error.h"
#include "gameutil.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "network.h"
#include "messages.h"
#include "menu.h"
#include "resource.h"
#include "screen.h"
#include "sectorfx.h"
#include "seq.h"
#include "sfx.h"
#include "sound.h"
#include "view.h"

GAMEOPTIONS gSaveGameOptions[10];
byte *gSaveGamePic[10];
unsigned int gSavedOffset;

unsigned int int_27AA38 = 0;
unsigned int int_27AA3C = 0;
unsigned int int_27AA40 = 0;
byte *int_27AA44 = NULL;

LoadSave LoadSave::head(123);
int LoadSave::hFile = -1;

short short_27AA54 = 0;

void func_76FD4(void)
{
    if (!int_27AA44)
        int_27AA44 = (byte*)Resource::Alloc(0x186a0);
}

void LoadSave::Save(void)
{
    ThrowError(66)("Pure virtual function called");
}

void LoadSave::Load(void)
{
    ThrowError(72)("Pure virtual function called");
}

void LoadSave::Read(void *pData, int nSize)
{
    int_27AA38 += nSize;
    dassert(hFile != -1, 82);
    if (read(hFile, pData, nSize) == -1)
        ThrowError(84)("File error #%d reading save file.", errno);
}

void LoadSave::Write(void *pData, int nSize)
{
    int_27AA38 += nSize;
    int_27AA3C += nSize;
    dassert(hFile != -1, 95);
    if (write(hFile, pData, nSize) == -1)
        ThrowError(97)("File error #%d writing save file.", errno);
    gSavedOffset += nSize;
}

void PreloadCache(void);

void LoadSave::LoadGame(char *pzFile)
{
    sndKillAllSounds();
    sfxKillAllSounds();
    ambKillAll();
    seqKillAll();
    if (gRedBookInstalled)
        Redbook.StopSong();
    if (gGameStarted == 0)
    {
        memset(xsprite, 0, sizeof(xsprite));
        memset(sprite, 0, sizeof(sprite));
        automapping = 1;
    }
    hFile = open(pzFile, O_BINARY);
    if (hFile == -1)
        ThrowError(125)("File error #%d loading save file.", errno);
    LoadSave *rover = head.next;
    while (rover != &head)
    {
        rover->Load();
        rover = rover->next;
    }
    close(hFile);
    hFile = -1;
    if (gGameStarted == 0)
        scrLoadPLUs();
    InitSectorFX();
    viewInitializePrediction();
    PreloadCache();
    ambInit();
    memset(myMinLag, 0, sizeof(myMinLag));
    otherMinLag = 0;
    myMaxLag = 0;
    gNetFifoClock = 0;
    gNetFifoTail = 0;
    memset(gNetFifoHead, 0, sizeof(gNetFifoHead));
    gPredictTail = 0;
    gNetFifoMasterTail = 0;
    memset(gFifoInput, 0, sizeof(gFifoInput));
    memset(gChecksum, 0, sizeof(gChecksum));
    memset(gCheckFifo, 0, sizeof(gCheckFifo));
    memset(gCheckHead, 0, sizeof(gCheckHead));
    gSendCheckTail = 0;
    gCheckTail = 0;
    gBufferJitter = 0;
    bOutOfSync = 0;
    viewSetMessage("");
    viewSetErrorMessage("");
    if (gGameStarted == 0)
    {
        netWaitForEveryone(0);
        memset(gPlayerReady, 0, sizeof(gPlayerReady));
    }
    gFrameTicks = 0;
    gFrame = 0;
    gCacheMiss = 0;
    gFrameRate = 0;
    gGameClock = 0;
    gPaused = 0;
    gGameStarted = 1;
    if (!bNoCDAudio && gRedBookInstalled && Redbook.preprocess())
    {
        Redbook.play_song(gGameOptions.nTrackNumber);
        Redbook.cd_status();
        Redbook.func_82BB4();
    }
    else
        sndPlaySong(gGameOptions.zLevelSong, 1);
}

void LoadSave::SaveGame(char *pzFile)
{
    hFile = open(pzFile, O_BINARY | O_TRUNC | O_CREAT | O_WRONLY, 0660);
    if (hFile == -1)
        ThrowError(219)("File error #%d creating save file.", errno);
    int_27AA38 = 0;
    int_27AA40 = 0;
    LoadSave *rover = head.next;
    while (rover != &head)
    {
        rover->Save();
        if (int_27AA38 > int_27AA40)
            int_27AA40 = int_27AA38;
        int_27AA38 = 0;
        rover = rover->next;
    }
    close(hFile);
    hFile = -1;
}

class MyLoadSave : public LoadSave
{
public:
    virtual void Load(void);
    virtual void Save(void);
};

void MyLoadSave::Load(void)
{
    int id;
    Read(&id, sizeof(id));
    if (id != 'DULB')
        ThrowError(291)("Old saved game found");
    ushort version;
    Read(&version, sizeof(version));
    if (version != gGameVersion.w)
        ThrowError(296)("Incompatible version of saved game found!");
    int release;
    Read(&release, sizeof(release));
    id = 4;
    if (release != id)
        ThrowError(314)("Saved game is from another release of Blood");
    Read(&gGameOptions, sizeof(gGameOptions));
    Read(&numsectors, sizeof(numsectors));
    Read(&numwalls, sizeof(numwalls));
    int nNumSprites;
    Read(&nNumSprites, sizeof(nNumSprites));
    memset(sector, 0, sizeof(sector));
    memset(wall, 0, sizeof(wall));
    memset(sprite, 0, sizeof(sprite));
    Read(sector, sizeof(sector[0])*numsectors);
    Read(wall, sizeof(wall[0])*numwalls);
    Read(sprite, sizeof(sprite));
    Read(&randomseed, sizeof(randomseed));
    Read(&parallaxtype, sizeof(parallaxtype));
    Read(&showinvisibility, sizeof(showinvisibility));
    Read(&parallaxyoffs, sizeof(parallaxyoffs));
    Read(&parallaxyscale, sizeof(parallaxyscale));
    Read(&visibility, sizeof(visibility));
    Read(&parallaxvisibility, sizeof(parallaxvisibility));
    Read(pskyoff, sizeof(pskyoff));
    Read(&pskybits, sizeof(pskybits));
    Read(headspritesect, sizeof(headspritesect));
    Read(headspritestat, sizeof(headspritestat));
    Read(prevspritesect, sizeof(prevspritesect));
    Read(prevspritestat, sizeof(prevspritestat));
    Read(nextspritesect, sizeof(nextspritesect));
    Read(nextspritestat, sizeof(nextspritestat));
    Read(show2dsector, sizeof(show2dsector));
    Read(show2dwall, sizeof(show2dwall));
    Read(show2dsprite, sizeof(show2dsprite));
    Read(&automapping, sizeof(automapping));
    Read(gotpic, sizeof(gotpic));
    Read(gotsector, sizeof(gotsector));
    Read(&gFrameClock, sizeof(gFrameClock));
    Read(&gFrameTicks, sizeof(gFrameTicks));
    Read(&gFrame, sizeof(gFrame));
    int nGameClock;
    Read(&nGameClock, sizeof(nGameClock));
    gGameClock = nGameClock;
    Read(&gPaused, sizeof(gPaused));
    Read(&gAdultContent, sizeof(gAdultContent));
    Read(baseWall, sizeof(baseWall[0])*numwalls);
    Read(baseSprite, sizeof(baseSprite[0])*nNumSprites);
    Read(baseFloor, sizeof(baseFloor[0])*numsectors);
    Read(baseCeil, sizeof(baseCeil[0])*numsectors);
    Read(velFloor, sizeof(velFloor[0])*numsectors);
    Read(velCeil, sizeof(velCeil[0])*numsectors);
    Read(&gHitInfo, sizeof(gHitInfo));
    Read(&char_1A76C6, sizeof(char_1A76C6));
    Read(&char_1A76C8, sizeof(char_1A76C8));
    Read(&char_1A76C7, sizeof(char_1A76C7));
    Read(&char_19AE44, sizeof(char_19AE44));
    Read(gStatCount, sizeof(gStatCount));
    Read(nextXSprite, sizeof(nextXSprite));
    Read(nextXWall, sizeof(nextXWall));
    Read(nextXSector, sizeof(nextXSector));
    memset(xsprite, 0, sizeof(xsprite));
    for (int nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus)
        {
            int nXSprite = sprite[nSprite].extra;
            if (nXSprite > 0)
                Read(&xsprite[nXSprite], sizeof(XSPRITE));
        }
    }
    memset(xwall, 0, sizeof(xwall));
    for (int nWall = 0; nWall < numwalls; nWall++)
    {
        int nXWall = wall[nWall].extra;
        if (nXWall > 0)
            Read(&xwall[nXWall], sizeof(XWALL));
    }
    memset(xsector, 0, sizeof(xsector));
    for (int nSector = 0; nSector < numsectors; nSector++)
    {
        int nXSector = sector[nSector].extra;
        if (nXSector > 0)
            Read(&xsector[nXSector], sizeof(XSECTOR));
    }
    Read(xvel, nNumSprites*sizeof(xvel[0]));
    Read(yvel, nNumSprites*sizeof(yvel[0]));
    Read(zvel, nNumSprites*sizeof(zvel[0]));
    Read(&gMapRev, sizeof(gMapRev));
    Read(&gSongId, sizeof(gSongId));
    Read(&gSkyCount, sizeof(gSkyCount));
    Read(&gFogMode, sizeof(gFogMode));
    gCheatMgr.func_5BCF4();
}
void MyLoadSave::Save(void)
{
    int nNumSprites = 0;
    int id = 'DULB';
    Write(&id, sizeof(id));
    ushort version = gGameVersion.w;
    Write(&version, sizeof(version));
    id = 4;
    Write(&id, sizeof(id));
    for (int nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus && nSprite > nNumSprites)
            nNumSprites = nSprite;
    }
    nNumSprites += 2;
    Write(&gGameOptions, sizeof(gGameOptions));
    Write(&numsectors, sizeof(numsectors));
    Write(&numwalls, sizeof(numwalls));
    Write(&nNumSprites, sizeof(nNumSprites));
    Write(sector, sizeof(sector[0])*numsectors);
    Write(wall, sizeof(wall[0])*numwalls);
    Write(sprite, sizeof(sprite));
    Write(&randomseed, sizeof(randomseed));
    Write(&parallaxtype, sizeof(parallaxtype));
    Write(&showinvisibility, sizeof(showinvisibility));
    Write(&parallaxyoffs, sizeof(parallaxyoffs));
    Write(&parallaxyscale, sizeof(parallaxyscale));
    Write(&visibility, sizeof(visibility));
    Write(&parallaxvisibility, sizeof(parallaxvisibility));
    Write(pskyoff, sizeof(pskyoff));
    Write(&pskybits, sizeof(pskybits));
    Write(headspritesect, sizeof(headspritesect));
    Write(headspritestat, sizeof(headspritestat));
    Write(prevspritesect, sizeof(prevspritesect));
    Write(prevspritestat, sizeof(prevspritestat));
    Write(nextspritesect, sizeof(nextspritesect));
    Write(nextspritestat, sizeof(nextspritestat));
    Write(show2dsector, sizeof(show2dsector));
    Write(show2dwall, sizeof(show2dwall));
    Write(show2dsprite, sizeof(show2dsprite));
    Write(&automapping, sizeof(automapping));
    Write(gotpic, sizeof(gotpic));
    Write(gotsector, sizeof(gotsector));
    Write(&gFrameClock, sizeof(gFrameClock));
    Write(&gFrameTicks, sizeof(gFrameTicks));
    Write(&gFrame, sizeof(gFrame));
    int nGameClock = gGameClock;
    Write(&nGameClock, sizeof(nGameClock));
    Write(&gPaused, sizeof(gPaused));
    Write(&gAdultContent, sizeof(gAdultContent));
    Write(baseWall, sizeof(baseWall[0])*numwalls);
    Write(baseSprite, sizeof(baseSprite[0])*nNumSprites);
    Write(baseFloor, sizeof(baseFloor[0])*numsectors);
    Write(baseCeil, sizeof(baseCeil[0])*numsectors);
    Write(velFloor, sizeof(velFloor[0])*numsectors);
    Write(velCeil, sizeof(velCeil[0])*numsectors);
    Write(&gHitInfo, sizeof(gHitInfo));
    Write(&char_1A76C6, sizeof(char_1A76C6));
    Write(&char_1A76C8, sizeof(char_1A76C8));
    Write(&char_1A76C7, sizeof(char_1A76C7));
    Write(&char_19AE44, sizeof(char_19AE44));
    Write(gStatCount, sizeof(gStatCount));
    Write(nextXSprite, sizeof(nextXSprite));
    Write(nextXWall, sizeof(nextXWall));
    Write(nextXSector, sizeof(nextXSector));
    for (nSprite = 0; nSprite < kMaxSprites; nSprite++)
    {
        if (sprite[nSprite].statnum < kMaxStatus)
        {
            int nXSprite = sprite[nSprite].extra;
            if (nXSprite > 0)
                Write(&xsprite[nXSprite], sizeof(XSPRITE));
        }
    }
    for (int nWall = 0; nWall < numwalls; nWall++)
    {
        int nXWall = wall[nWall].extra;
        if (nXWall > 0)
            Write(&xwall[nXWall], sizeof(XWALL));
    }
    for (int nSector = 0; nSector < numsectors; nSector++)
    {
        int nXSector = sector[nSector].extra;
        if (nXSector > 0)
            Write(&xsector[nXSector], sizeof(XSECTOR));
    }
    Write(xvel, nNumSprites*sizeof(xvel[0]));
    Write(yvel, nNumSprites*sizeof(yvel[0]));
    Write(zvel, nNumSprites*sizeof(zvel[0]));
    Write(&gMapRev, sizeof(gMapRev));
    Write(&gSongId, sizeof(gSongId));
    Write(&gSkyCount, sizeof(gSkyCount));
    Write(&gFogMode, sizeof(gFogMode));
}

void LoadSavedInfo(void)
{
    struct find_t find;
    int nStatus = _dos_findfirst("GAME*.SAV", 0, &find);
    int nCount = 0;
    while (!nStatus && nCount < 10)
    {
        int hFile = open(find.name, O_BINARY);
        if (hFile == -1)
            ThrowError(694)("File error #%d loading save file header.", errno);
        int vc, v8;
        ushort v4;
        vc = 0;
        v8 = 0;
        v4 = short_27AA54;
        if (read(hFile, &vc, sizeof(vc)) == -1)
        {
            close(hFile);
            nCount++; nStatus = _dos_findnext(&find);
            continue;
        }
        if (vc != 'DULB')
        {
            close(hFile);
            nCount++; nStatus = _dos_findnext(&find);
            continue;
        }
        read(hFile, &v4, sizeof(v4));
        if (v4 != gGameVersion.w)
        {
            close(hFile);
            nCount++; nStatus = _dos_findnext(&find);
            continue;
        }
        read(hFile, &v8, sizeof(v8));
        vc = 4;
        if (v8 != vc)
        {
            close(hFile);
            nCount++; nStatus = _dos_findnext(&find);
            continue;
        }
        if (read(hFile, &gSaveGameOptions[nCount], sizeof(gSaveGameOptions[0])) == -1)
            ThrowError(752)("File error #%d reading save file.", errno);
        close(hFile);
        strcpy(strRestoreGameStrings[gSaveGameOptions[nCount].nSaveGameSlot], gSaveGameOptions[nCount].szUserGameName);
        nCount++; nStatus = _dos_findnext(&find);
    }
}

void UpdateSavedInfo(int nSlot)
{
    strcpy(strRestoreGameStrings[gSaveGameOptions[nSlot].nSaveGameSlot], gSaveGameOptions[nSlot].szUserGameName);
}

static MyLoadSave myLoadSave;
