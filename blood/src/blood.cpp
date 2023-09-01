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
#include <stdio.h>
#include <conio.h>
#include <i86.h>
#include <io.h>
#include "typedefs.h"
#include "build.h"
#include "debug4g.h"
#include "qheap.h"
#include "globals.h"
#include "resource.h"
#include "screen.h"

#include "sound.h"
#include "satimer.h"
#include "config.h"
#include "textio.h"
#include "misc.h"
#include "cdrom.h"
#include "error.h"
#include "tile.h"
#include "trig.h"
#include "credits.h"
#include "key.h"
#include "fire.h"
#include "view.h"
#include "weapon.h"
#include "controls.h"
#include "levels.h"
#include "loadsave.h"
#include "inifile.h"
#include "network.h"
#include "gamemenu.h"
#include "seq.h"
#include "messages.h"
#include "sfx.h"
#include "asound.h"
#include "demo.h"
#include "endgame.h"
#include "warp.h"
#include "sectorfx.h"
#include "eventq.h"
#include "actor.h"
#include "dude.h"
#include "mirrors.h"
#include "triggers.h"
#include "menu.h"
#include "function.h"
#include "control.h"
#include "endgame.h"
#include "getopt.h"
#include "gui.h"
#include "helix.h"
#include "choke.h"
#include "keyboard.h"


ErrorHandler prevErrorHandler;

BOOL gExplicitSetup;

char* pUserTiles;
char* szSoundRes;
char* pUserRFF;

char gUserMapFilename[144];

BOOL bAddUserMap;
BOOL char_148EE9;
BOOL bNoDemo;
BOOL char_148EEB;
BOOL char_148EEC;
BOOL char_148EED;
char char_148ef0[13];
BOOL bCustomName;
char zCustomName[12];

PLAYER gPlayerTemp[kMaxPlayers];
int gHealthTemp[kMaxPlayers];

static char buffer[256];

CCDAudio Redbook;
BOOL gRedBookInstalled;
BOOL bNoCDAudio;
BOOL gRestartGame;

REGS regs;

int gMaxAlloc = 0x2000000;

BOOL bQuickStart = 1;

BOOL CheckIfWindows(void)
{
    regs.w.ax = 0x1600;
    int386(0x2f, &regs, &regs);
    if (!(regs.h.al > 1 && regs.h.ah != 0x80))
        gInWindows = 0;
    else
        gInWindows = 1;
    return gInWindows;
}

int func_83370(void);
int func_83D80(void);
int tenBloodInit(void);
int func_835B0(void);

void func_10148(void)
{
    char temp[20];
    IniFile *iniFile = new IniFile(char_148ef0);
    if (!iniFile || !iniFile->SectionExists("options"))
        ThrowError(206)("Section [%s] expected in %s", "options", char_148ef0);
    char *val = iniFile->GetKeyString("options", "Online", "");
    strncpy(temp, val, 20);
    if (!stricmp(temp, "mpath"))
    {
        if (func_83370() < 0)
            ThrowError(219)("Can't initailize MPATH Service.  Please check your installation.");
        gSyncRate = 4;
    }
    else if (!stricmp(temp, "engage"))
    {
        if (func_83D80())
            ThrowError(233)("Can't initailize ENGAGE Service.  Please check your installation.");
    }
    else if (!stricmp(temp, "commit"))
    {
    }
    else if (!stricmp(temp, "ten"))
    {
        int status = tenBloodInit();
        if (status)
            ThrowError(252)("Error %d initializing TEN", status);
        else
            tioPrint("initialized TEN");
    }
    else if (!stricmp(temp, "commit"))
    {
    }
    else if (!stricmp(temp, "heat"))
    {
        if (func_835B0() < 0)
            ThrowError(269)("Cannot initialize the HEAT service.  Please check your installation.");
    }
    else
    {
        ThrowError(282)("Unknown Online Service specified in %s", char_148ef0);
    }
    delete iniFile;
}

void func_10324(void)
{
    IniFile *iniFile = new IniFile(char_148ef0);
    if (!iniFile || !iniFile->SectionExists("options"))
        ThrowError(300)("Section [%s] expected in %s", "options", char_148ef0);
    if (char_148EEC)
    {
        gPacketStartGame.gameType = iniFile->GetKeyHex("options", "GameType", 2);
        if (gPacketStartGame.gameType == 0)
            gPacketStartGame.gameType = 2;
        gPacketStartGame.episodeId = iniFile->GetKeyHex("options", "EpisodeID", 0);
        gPacketStartGame.levelId = iniFile->GetKeyHex("options", "LevelID", 0);
        gPacketStartGame.difficulty = iniFile->GetKeyHex("options", "GameDifficulty", 1);
        gPacketStartGame.monsterSettings = iniFile->GetKeyHex("options", "MonsterSettings", 1);
        gPacketStartGame.weaponSettings = iniFile->GetKeyHex("options", "WeaponSettings", 1);
        gPacketStartGame.itemSettings = iniFile->GetKeyHex("options", "ItemSettings", 1);
        gPacketStartGame.respawnSettings = iniFile->GetKeyHex("options", "RespawnSettings", 0);
        gPacketStartGame.unk = 0;
        gPacketStartGame.userMapName[0] = 0;
        char *um = iniFile->GetKeyString("options", "UserMapName", "");
        strncpy(gPacketStartGame.userMapName, um, 13);
        gPacketStartGame.userMapName[12] = 0;
        gPacketStartGame.userMap = iniFile->GetKeyHex("options", "UserMap", 0);
        netBroadcastNewGame();
        gStartNewGame = 1;
    }
    gGameMenuMgr.Deactivate();
    delete iniFile;
}

void CenterCenter(void)
{
    printf("Center the joystick and press a button\n");
}

void UpperLeft(void)
{
    printf("Move joystick to upper-left corner and press a button\n");
}

void LowerRight(void)
{
    printf("Move joystick to lower-right corner and press a button\n");
}

void CenterThrottle(void)
{
    printf("Center the throttle control and press a button\n");
}

void CenterRudder(void)
{
    printf("Center the rudder control and press a button\n");
}

void PreloadDudeCache(SPRITE *pSprite)
{
    DUDEINFO *pDudeInfo = &dudeInfo[pSprite->type-kDudeBase];
    seqCache(pDudeInfo->seqStartID);
    seqCache(pDudeInfo->seqStartID+5);
    seqCache(pDudeInfo->seqStartID+1);
    seqCache(pDudeInfo->seqStartID+2);
    switch (pSprite->type)
    {
    case 201:
    case 202:
    case 247:
    case 248:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        seqCache(pDudeInfo->seqStartID+9);
        seqCache(pDudeInfo->seqStartID+13);
        seqCache(pDudeInfo->seqStartID+14);
        seqCache(pDudeInfo->seqStartID+15);
        break;
    case 204:
    case 217:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        seqCache(pDudeInfo->seqStartID+9);
        seqCache(pDudeInfo->seqStartID+10);
        seqCache(pDudeInfo->seqStartID+11);
        break;
    case 208:
    case 209:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+6);
    case 206:
    case 207:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        seqCache(pDudeInfo->seqStartID+9);
        break;
    case 210:
        seqCache(pDudeInfo->seqStartID + 6);
        seqCache(pDudeInfo->seqStartID + 7);
        seqCache(pDudeInfo->seqStartID + 8);
        break;
    case 211:
        seqCache(pDudeInfo->seqStartID + 6);
        seqCache(pDudeInfo->seqStartID + 7);
        seqCache(pDudeInfo->seqStartID + 8);
        break;
    case 213:
    case 214:
    case 215:
    case 216:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        break;
    case 229:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        break;
    case 227:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
    case 220:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        break;
    case 212:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        break;
    case 218:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        break;
    case 219:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        break;
    case 249:
        seqCache(pDudeInfo->seqStartID+6);
        break;
    case 205:
        seqCache(pDudeInfo->seqStartID+12);
        seqCache(pDudeInfo->seqStartID+9);
    case 244:
        seqCache(pDudeInfo->seqStartID+10);
    case 203:
        seqCache(pDudeInfo->seqStartID+6);
        seqCache(pDudeInfo->seqStartID+7);
        seqCache(pDudeInfo->seqStartID+8);
        seqCache(pDudeInfo->seqStartID+11);
        seqCache(pDudeInfo->seqStartID+13);
        seqCache(pDudeInfo->seqStartID+14);
        break;
    }
}

void PreloadThingCache(SPRITE *pSprite)
{
    switch (pSprite->type)
    {
    case 406:
    case 407:
        seqCache(12);
        break;
    case 410:
        seqCache(15);
        break;
    case 411:
        seqCache(21);
        break;
    case 412:
        seqCache(25);
        seqCache(26);
        break;
    case 413:
        seqCache(38);
        seqCache(40);
        seqCache(28);
        break;
    case 416:
        break;
    default:
        tilePreloadTile(pSprite->picnum);
        break;
    }
    seqCache(3);
    seqCache(4);
    seqCache(5);
    seqCache(9);
}

void PreloadTiles(void)
{
    int i;
    int skyTile = -1;
    memset(gotpic,0,sizeof(gotpic));
    for (i = 0; i < numsectors; i++)
    {
        tilePrecacheTile(sector[i].floorpicnum);
        tilePrecacheTile(sector[i].ceilingpicnum);
        if ((sector[i].ceilingstat&kSectorStat0) && skyTile == -1)
            skyTile = sector[i].ceilingpicnum;
    }
    for (i = 0; i < numwalls; i++)
    {
        tilePrecacheTile(wall[i].picnum);
        if (wall[i].overpicnum >= 0)
            tilePrecacheTile(wall[i].overpicnum);
    }
    for (i = 0; i < kMaxSprites; i++)
    {
        if (sprite[i].statnum < kMaxStatus)
        {
            SPRITE *pSprite = &sprite[i];
            switch (pSprite->statnum)
            {
            case 6:
                PreloadDudeCache(pSprite);
                break;
            case 4:
                PreloadThingCache(pSprite);
                break;
            default:
                tilePrecacheTile(pSprite->picnum);
                break;
            }
        }
    }
    if (numplayers > 1)
    {
        seqCache(dudeInfo[31].seqStartID+6);
        seqCache(dudeInfo[31].seqStartID+7);
        seqCache(dudeInfo[31].seqStartID+8);
        seqCache(dudeInfo[31].seqStartID+9);
        seqCache(dudeInfo[31].seqStartID+10);
        seqCache(dudeInfo[31].seqStartID+14);
        seqCache(dudeInfo[31].seqStartID+15);
        seqCache(dudeInfo[31].seqStartID+12);
        seqCache(dudeInfo[31].seqStartID+16);
        seqCache(dudeInfo[31].seqStartID+17);
        seqCache(dudeInfo[31].seqStartID+18);
    }
    if (skyTile > -1 && skyTile < kMaxTiles)
    {
        for (i = 1; i < gSkyCount; i++)
            tilePrecacheTile(skyTile+i);
    }
    netGetPackets();
}

void PreloadCache(void)
{
    PreloadTiles();
    for (int i = 0; i < kMaxTiles; i++)
    {
        if (TestBitString(gotpic, i))
        {
            tileLoadTile(i);
            netGetPackets();
        }
    }
    memset(gotpic,0,sizeof(gotpic));
}

void EndLevel(void)
{
    gViewPos = VIEWPOS_0;
    gGameMessageMgr.Clear();
    sndKillAllSounds();
    sfxKillAllSounds();
    ambKillAll();
    seqKillAll();
    if (gRedBookInstalled)
        Redbook.StopSong();
}

void StartLevel(GAMEOPTIONS *gameOptions)
{
    int i;
    EndLevel();
    gStartNewGame = 0;
    ready2send = 0;
    if (gDemo.RecordStatus() && gGameStarted)
        gDemo.Close();
    netWaitForEveryone(0);
    if (gGameOptions.nGameType == GAMETYPE_0)
    {
        if (!(gGameOptions.uGameFlags&1))
            levelSetupOptions(gGameOptions.nEpisode, gGameOptions.nLevel);
        if (gEpisodeInfo[gGameOptions.nEpisode].cutALevel == gGameOptions.nLevel
            && gEpisodeInfo[gGameOptions.nEpisode].at8f08[0])
            gGameOptions.uGameFlags |= 4;
        if ((gGameOptions.uGameFlags&4) && !gDemo.PlaybackStatus())
            levelPlayIntroScene(gGameOptions.nEpisode);
    }
    else if (gGameOptions.nGameType > GAMETYPE_0 && !(gGameOptions.uGameFlags&1))
    {
#if 0
        if (!gDemo.PlaybackStatus())
        {
#endif
        gGameOptions.nEpisode = gPacketStartGame.episodeId;
        gGameOptions.nLevel = gPacketStartGame.levelId;
        gGameOptions.nGameType = (GAMETYPE)gPacketStartGame.gameType;
        gGameOptions.nDifficulty = (DIFFICULTY)gPacketStartGame.difficulty;
        gGameOptions.nMonsterSettings = (MONSTERSETTINGS)gPacketStartGame.monsterSettings;
        gGameOptions.nWeaponSettings = (WEAPONSETTINGS)gPacketStartGame.weaponSettings;
        gGameOptions.nItemSettings = (ITEMSETTINGS)gPacketStartGame.itemSettings;
        gGameOptions.nRespawnSettings = (RESPAWNSETTINGS)gPacketStartGame.respawnSettings;
        if (gPacketStartGame.userMap)
            levelAddUserMap(gPacketStartGame.userMapName);
        else
            levelSetupOptions(gGameOptions.nEpisode, gGameOptions.nLevel);
#if 0
        }
#endif
        ///
#if 0
        if (!gDemo.RecordStatus() && !gDemo.PlaybackStatus())
        {
            strcpy(buffer, levelGetFilename(gGameOptions.nEpisode, gGameOptions.nLevel));
            ChangeExtension(buffer, ".DEM");
            gDemo.Create(buffer);
        }
#endif
        ///
    }
    if (gameOptions->uGameFlags&1)
    {
        for (i = connecthead; i >= 0; i = connectpoint2[i])
        {
            PLAYER *pPlayer = &gPlayer[i];
            XSPRITE *pXSprite = &xsprite[pPlayer->pSprite->extra];
            gPlayerTemp[i] = *pPlayer;
            gHealthTemp[i] = pXSprite->health;
        }
    }
    memset(xsprite,0,sizeof(xsprite));
    memset(sprite,0,sizeof(sprite));
    func_5A828();
    dbLoadMap(gameOptions->zLevelName,&startposx,&startposy,&startposz,&startang,&startsectnum,&gameOptions->uMapCRC);
    srand(gameOptions->uMapCRC);
    gKillMgr.Clear();
    gSecretMgr.Clear();
    automapping = 1;
    for (i = 0; i < kMaxSprites; i++)
    {
        SPRITE *pSprite = &sprite[i];
        if (pSprite->statnum < kMaxStatus && pSprite->extra > 0)
        {
            XSPRITE *pXSprite = &xsprite[pSprite->extra];
            if ((pXSprite->ate_7&(1<<gameOptions->nDifficulty))
            || (pXSprite->atf_4 && gameOptions->nGameType == GAMETYPE_0)
            || (pXSprite->atf_5 && gameOptions->nGameType == GAMETYPE_2)
            || (pXSprite->atb_7 && gameOptions->nGameType == GAMETYPE_3)
            || (pXSprite->atf_6 && gameOptions->nGameType == GAMETYPE_1))
                DeleteSprite(i);
        }
    }
    scrLoadPLUs();
    startposz = getflorzofslope(startsectnum,startposx,startposy);
    for (i = 0; i < kMaxPlayers; i++)
    {
        gStartZone[i].x = startposx;
        gStartZone[i].y = startposy;
        gStartZone[i].z = startposz;
        gStartZone[i].sectnum = startsectnum;
        gStartZone[i].ang = startang;
    }
    InitSectorFX();
    warpInit();
    actInit();
    evInit();
    for (i = connecthead; i >= 0; i = connectpoint2[i])
    {
        if (!(gameOptions->uGameFlags&1))
        {
            if (numplayers == 1)
            {
                gProfile[i].skill = gSkill;
                gProfile[i].at0 = gAutoAim;
            }
            playerInit(i,0);
        }
        playerStart(i);
    }
    if (gameOptions->uGameFlags&1)
    {
        for (i = connecthead; i >= 0; i = connectpoint2[i])
        {
            PLAYER *pPlayer = &gPlayer[i];
            pPlayer->pXSprite->health = gHealthTemp[i];
            pPlayer->at26 = gPlayerTemp[i].at26;
            pPlayer->atbd = gPlayerTemp[i].atbd;
            pPlayer->atc3 = gPlayerTemp[i].atc3;
            pPlayer->atc7 = gPlayerTemp[i].atc7;
            pPlayer->at2a = gPlayerTemp[i].at2a;
            pPlayer->at1b1 = gPlayerTemp[i].at1b1;
            pPlayer->atbf = gPlayerTemp[i].atbf;
            pPlayer->atbe = gPlayerTemp[i].atbe;
        }
    }
    gameOptions->uGameFlags &= ~3;
    scrSetDac();
    PreloadCache();
    InitMirrors();
    gFrameClock = 0;
    trInit();
    ambInit();
    func_79760();
    gCacheMiss = 0;
    gFrame = 0;
    if (!gDemo.PlaybackStatus())
        gGameMenuMgr.Deactivate();
    if (!gRedBookInstalled)
        sndPlaySong(gGameOptions.zLevelSong,1);
    if (!bNoCDAudio && gRedBookInstalled && Redbook.preprocess() && !gDemo.PlaybackStatus())
    {
        Redbook.play_song(gGameOptions.nTrackNumber);
        Redbook.cd_status();
        Redbook.func_82BB4();
    }
    viewSetMessage("");
    viewSetErrorMessage("");
    viewResizeView(gViewSize);
    if (gGameOptions.nGameType == GAMETYPE_3)
        gGameMessageMgr.SetCoordinates(gViewX0S+1,gViewY0S+15);
    netWaitForEveryone(0);
    gGameClock = 0;
    gPaused = 0;
    gGameStarted = 1;
    ready2send = 1;
}

void StartNetworkLevel(void)
{
    if (gDemo.RecordStatus())
        gDemo.Close();
    if (!(gGameOptions.uGameFlags&1))
    {
        gGameOptions.nEpisode = gPacketStartGame.episodeId;
        gGameOptions.nLevel = gPacketStartGame.levelId;
        gGameOptions.nGameType = (GAMETYPE)gPacketStartGame.gameType;
        gGameOptions.nDifficulty = (DIFFICULTY)gPacketStartGame.difficulty;
        gGameOptions.nMonsterSettings = (MONSTERSETTINGS)gPacketStartGame.monsterSettings;
        gGameOptions.nWeaponSettings = (WEAPONSETTINGS)gPacketStartGame.weaponSettings;
        gGameOptions.nItemSettings = (ITEMSETTINGS)gPacketStartGame.itemSettings;
        gGameOptions.nRespawnSettings = (RESPAWNSETTINGS)gPacketStartGame.respawnSettings;
        if (gPacketStartGame.userMap)
            levelAddUserMap(gPacketStartGame.userMapName);
        else
            levelSetupOptions(gGameOptions.nEpisode, gGameOptions.nLevel);
    }
    StartLevel(&gGameOptions);
}

void LocalKeys(void)
{
    byte key;
    BOOL alt = keystatus[bsc_LAlt] | keystatus[bsc_RAlt];
    BOOL ctrl = keystatus[bsc_LCtrl] | keystatus[bsc_RCtrl];
    BOOL shift = keystatus[bsc_LShift] | keystatus[bsc_RShift];
    if (BUTTON(gamefunc_See_Chase_View) && !alt && !shift)
    {
        CONTROL_ClearButton(gamefunc_See_Chase_View);
        if (gViewPos > 0)
            gViewPos = VIEWPOS_0;
        else
            gViewPos = VIEWPOS_1;
    }
    if (BUTTON(gamefunc_See_Coop_View))
    {
        CONTROL_ClearButton(gamefunc_See_Coop_View);
        if (gGameOptions.nGameType == GAMETYPE_1)
        {
            gViewIndex = connectpoint2[gViewIndex];
            if (gViewIndex == -1)
                gViewIndex = connecthead;
            gView = &gPlayer[gViewIndex];
        }
        else if (gGameOptions.nGameType == GAMETYPE_3)
        {
            int oldViewIndex = gViewIndex;
            PLAYER *vb = gMe;
            do
            {
                gViewIndex = connectpoint2[gViewIndex];
                if (gViewIndex == -1)
                    gViewIndex = connecthead;
                if (oldViewIndex == gViewIndex || gPlayer[gViewIndex].at2ea == vb->at2ea)
                    break;
            } while (oldViewIndex != gViewIndex);
            gView = &gPlayer[gViewIndex];
        }
    }
    while ((key = keyGet()) != 0)
    {
        if ((alt || shift) && gGameOptions.nGameType > GAMETYPE_0)
        {
            switch (key)
            {
                case bsc_F1:
                case bsc_F2:
                case bsc_F3:
                case bsc_F4:
                case bsc_F5:
                case bsc_F6:
                case bsc_F7:
                case bsc_F8:
                case bsc_F9:
                case bsc_F10:
                {
                    int t = key - bsc_F1;
                    if (alt)
                    {
                        netBroadcastTaunt(myconnectindex, t);
                    }
                    else
                    {
                        gPlayerMsg.Set(CommbatMacro[t]);
                        gPlayerMsg.Send();
                    }
                    keyFlushStream();
                    keystatus[key] = 0;
                    CONTROL_ClearButton(gamefunc_See_Chase_View);
                    return;
                }
            }
        }
        switch (key)
        {
        case bsc_Pad_Period:
        case bsc_Del:
            if (ctrl && alt)
            {
                gQuitGame = 1;
                return;
            }
            break;
        case bsc_Esc:
            keyFlushStream();
            if (gGameStarted && gPlayer[myconnectindex].pXSprite->health != 0)
            {
                if (!gGameMenuMgr.Active())
                    gGameMenuMgr.Push(&menuMainWithSave);
            }
            else
            {
                if (!gGameMenuMgr.Active())
                    gGameMenuMgr.Push(&menuMain);
            }
            return;
        case bsc_F1:
            keyFlushStream();
            if (gGameOptions.nGameType == GAMETYPE_0)
                gGameMenuMgr.Push(&menuOrder);
            break;
        case bsc_F2:
            keyFlushStream();
            if (!gGameMenuMgr.Active() && gGameOptions.nGameType == GAMETYPE_0)
                gGameMenuMgr.Push(&menuSaveGame);
            break;
        case bsc_F3:
            keyFlushStream();
            if (!gGameMenuMgr.Active() && gGameOptions.nGameType == GAMETYPE_0)
                gGameMenuMgr.Push(&menuLoadGame);
            break;
        case bsc_F4:
            keyFlushStream();
            if (!gGameMenuMgr.Active())
                gGameMenuMgr.Push(&menuSounds);
            return;
        case bsc_F5:
            keyFlushStream();
            if (!gGameMenuMgr.Active())
                gGameMenuMgr.Push(&menuOptions);
            return;
        case bsc_F6:
            keyFlushStream();
            if (gGameStarted && !gGameMenuMgr.Active() && gPlayer[myconnectindex].pXSprite->health != 0)
            {
                if (gQuickSaveSlot != -1)
                {
                    QuickSaveGame();
                    return;
                }
                gGameMenuMgr.Push(&menuSaveGame);
            }
            break;
        case bsc_F8:
            keyFlushStream();
            gGameMenuMgr.Push(&menuOptions);
            break;
        case bsc_F9:
            keyFlushStream();
            if (!gGameMenuMgr.Active())
            {
                if (gQuickLoadSlot != -1)
                {
                    QuickLoadGame();
                    return;
                }
                if (gQuickLoadSlot == -1 && gQuickSaveSlot != -1)
                {
                    gQuickLoadSlot = gQuickSaveSlot;
                    QuickLoadGame();
                    return;
                }
                gGameMenuMgr.Push(&menuLoadGame);
            }
            break;
        case bsc_F10:
            keyFlushStream();
            if (!gGameMenuMgr.Active())
                gGameMenuMgr.Push(&menuQuit);
            break;
        case bsc_F11:
            if (gGamma == gGammaLevels-1)
                gGamma = 0;
            else
                gGamma = ClipHigh(gGamma+1,gGammaLevels-1);
            scrSetGamma(gGamma);

            sprintf(buffer,"Gamma correction level %i",gGamma);
            viewSetMessage(buffer);
            break;
        case bsc_F12:
        {
            static char name[16] = "";
            if (!name[0])
            {
                for (int i = 0; 1; i++)
                {
                    sprintf(name,"SS%02d0000.PCX",i);
                    if (access(name, 4) == -1)
                        break;
                }
            }
            screencapture(name, 0);
            break;
        }
        }
    }
    int powerCount = powerupCheck(gView, 28);
    if (powerCount)
    {
        static int timer = 0;
        timer += 4;
        int tilt1 = 170, tilt2 = 170, pitch = 20;
        if (powerCount < 512)
        {
            int powerScale = (powerCount<<16) / 512;
            tilt1 = mulscale16(tilt1, powerScale);
            tilt2 = mulscale16(tilt2, powerScale);
            pitch = mulscale16(pitch, powerScale);
        }
        gScreenTilt = mulscale30(Sin(timer * 2) / 2 + Sin(timer * 3) / 2,tilt1);
        deliriumTurn = mulscale30(Sin(timer * 3) / 2 + Sin(timer * 4) / 2,tilt2);
        deliriumPitch = mulscale30(Sin(timer * 4) / 2 + Sin(timer * 5) / 2,pitch);
    }
    else
    {
        gScreenTilt = ((gScreenTilt+1024)&2047)-1024;
        if (gScreenTilt > 0)
            gScreenTilt = ClipLow(gScreenTilt - 8, 0);
        else if (gScreenTilt < 0)
            gScreenTilt = ClipHigh(gScreenTilt + 8, 0);
    }
}

void ProcessFrame(void)
{
    if (gDemo.RecordStatus())
        gDemo.Write(gFifoInput[gNetFifoTail&255]);
    for (int i = connecthead; i >= 0; i = connectpoint2[i])
    {
        gPlayer[i].atc.buttonFlags = gFifoInput[gNetFifoTail&255][i].buttonFlags;
        gPlayer[i].atc.keyFlags.word |= gFifoInput[gNetFifoTail&255][i].keyFlags.word;
        gPlayer[i].atc.useFlags.byte |= gFifoInput[gNetFifoTail&255][i].useFlags.byte;
        if (gFifoInput[gNetFifoTail&255][i].newWeapon != 0)
            gPlayer[i].atc.newWeapon = gFifoInput[gNetFifoTail&255][i].newWeapon;
        gPlayer[i].atc.forward = gFifoInput[gNetFifoTail&255][i].forward;
        gPlayer[i].atc.strafe = gFifoInput[gNetFifoTail&255][i].strafe;
        gPlayer[i].atc.turn = gFifoInput[gNetFifoTail&255][i].turn;
        gPlayer[i].atc.mlook = gFifoInput[gNetFifoTail&255][i].mlook;
    }
    gNetFifoTail++;
    if ((gFrame&((gSyncRate<<3)-1)) == 0)
    {
        CalcGameChecksum();
        memcpy(gCheckFifo[gCheckHead[myconnectindex]&255][myconnectindex],gChecksum,sizeof(gChecksum));
        gCheckHead[myconnectindex]++;
    }
    for (i = connecthead; i >= 0; i = connectpoint2[i])
    {
        if (gPlayer[i].atc.keyFlags.quit && i == myconnectindex)
        {
            gPlayer[i].atc.keyFlags.quit = 0;
            netBroadcastMyLogoff();
            return;
        }
        if (gPlayer[i].atc.keyFlags.restart)
        {
            gPlayer[i].atc.keyFlags.restart = 0;
            levelRestart();
            return;
        }
        if (gPlayer[i].atc.keyFlags.pause)
        {
            gPlayer[i].atc.keyFlags.pause = 0;
            gPaused = !gPaused;
            if (gPaused && gGameOptions.nGameType > GAMETYPE_0 && numplayers > 1)
            {
                sprintf(buffer,"%s paused the game",gProfile[i].name);
                viewSetMessage(buffer);
            }
        }
    }
    viewClearInterpolations();
    if (!gDemo.PlaybackStatus())
    {
        if (gPaused || gEndGameMgr.Active() || (gGameOptions.nGameType == GAMETYPE_0 && gGameMenuMgr.Active()))
            return;
    }
    for (i = connecthead; i >= 0; i = connectpoint2[i])
    {
        viewBackupView(i);
        playerProcess(&gPlayer[i]);
    }
    trProcessBusy();
    evProcess(gFrameClock);
    seqProcess(4);
    DoSectorPanning();
    actProcessSprites();
    actPostProcess();
    viewCorrectPrediction();
    sndProcess();
    ambProcess();
    sfxUpdate3DSounds();
    gFrame++;
    gFrameClock += 4;
    if ((gGameOptions.uGameFlags&1) != 0 && !gStartNewGame)
    {
        ready2send = 0;
        if (gDemo.RecordStatus())
            gDemo.Close();
        sndFadeSong(4000);
        seqKillAll();
        if (gGameOptions.uGameFlags&2)
        {
            if (gGameOptions.nGameType == GAMETYPE_0)
            {
                if (gGameOptions.uGameFlags&8)
                    levelPlayEndScene(gGameOptions.nEpisode);
                gGameMenuMgr.Deactivate();
                gGameMenuMgr.Push(&menuCredits);
            }
            gGameOptions.uGameFlags &= ~3;
            gRestartGame = 1;
            gQuitGame = 1;
        }
        else
        {
            gEndGameMgr.Setup();
            viewResizeView(gViewSize);
        }
    }
#if 0
    {
        static FILE *test;
        if (!test) test = fopen("test.txt", "w");
        //SPRITE *pSprite = gPlayer[1].pSprite;
        SPRITE *pSprite = &sprite[0x42];
        if (!pSprite)
            return;
        fprintf(test, "%i x=%i y=%i z=%i ang=%i zvel=%i aim.dz=%i\n", gFrame, pSprite->x, pSprite->y, pSprite->z, pSprite->ang, zvel[pSprite->index], gPlayer[1].at1be.dz);
    }
#endif
}

void GameErrorHandler(const Error &err)
{
    if (gGameOptions.nGameType > GAMETYPE_0 && numplayers > 1)
    {
        sendlogoff();
        uninitmultiplayers();
    }
    sndTerm();
    timerRemove();
    ctrlTerm();
    UnlockClockStrobe();
    uninitengine();
    setvmode(gOldDisplayMode);
    prevErrorHandler(err);
}

void BannerToTIO(void)
{
    sprintf(buffer, "One Unit: WHOLE BLOOD %s [%s] -- DO NOT DISTRIBUTE", GetVersionString(), gBuildDate);
    tioCenterString(0, 0, tioScreenCols-1, buffer, 0x4e);
    tioCenterString(tioScreenRows-1, 0, tioScreenCols-1, "Copyright (c)1994-1997 Monolith Productions Inc.", 0x4e);
    tioWindow(1, 0, tioScreenRows-3, tioScreenCols);
}

void Banner8250(void)
{
    tioPrint("===========================================================================");
    tioPrint("WARNING: 8250 UART detected.");
    tioPrint("Music is being disabled and lower quality sound is being set.  We apologize");
    tioPrint("for this, but it is necessary to maintain high frame rates while trying to");
    tioPrint("play the game on an 8250.  We suggest upgrading to a 16550 or better UART");
    tioPrint("for maximum performance.  Press any key to continue.");
    tioPrint("===========================================================================");
    while (!keyGet())
        netGetPackets();
}

void ShowUsage(void)
{
    puts("Blood Command-line Options:");
    puts("-?            This help");
    puts("-8250         Enforce obsolete UART I/O");
    puts("-auto         Automatic Network start. Implies -quick");
    puts("-getopt       Use network game options from file.  Implies -auto");
    puts("-broadcast    Set network to broadcast packet mode");
    puts("-masterslave  Set network to master/slave packet mode");
    puts("-net          Net mode game");
    puts("-noaim        Disable auto-aiming");
    puts("-nocd         Disable CD audio");
    puts("-nodudes      No monsters");
    puts("-nodemo       No Demos");
    puts("-robust       Robust network sync checking");
    puts("-skill        Set player handicap; Range:0..4; Default:2; (NOT difficulty level.)");
    puts("-quick        Skip Intro screens and get right to the game");
    puts("-pname        Override player name setting from config file");
    puts("-map          Specify a user map");
    puts("-playback     Play back a demo");
    puts("-record       Record a demo");
    puts("-art          Specify an art base file name");
    puts("-snd          Specify an RFF Sound file name");
    puts("-RFF          Specify an RFF file for Blood game resources");
    puts("-ini          Specify an INI file name (default is blood.ini)");
    exit(0);
}

SWITCH switches[] = {
    { "?", 0, 0 },
    { "broadcast", 1, 0 },
    { "map", 2, 1 },
    { "masterslave", 3, 0 },
    { "net", 4, 1 },
    { "nodudes", 5, 0 },
    { "playback", 6, 1 },
    { "record", 7, 1 },
    { "robust", 8, 0 },
    { "setupfile", 9, 1 },
    { "skill", 10, 1 },
    { "nocd", 11, 0 },
    { "8250", 12, 0 },
    { "ini", 13, 1 },
    { "noaim", 14, 0 },
    { "f", 15, 1 },
    { "control", 16, 1 },
    { "vector", 17, 1 },
    { "quick", 18, 0 },
    { "getopt", 19, 1 },
    { "auto", 20, 1 },
    { "pname", 21, 1 },
    { "noresend", 22, 0 },
    { "silentaim", 23, 0 },
    { "ten", 24, 0 },
    { "nodemo", 25, 0 },
    { "art", 26, 1 },
    { "snd", 27, 1 },
    { "RFF", 28, 1 },
    { "MAXALLOC", 29, 1 },
    { 0 }
};

void ParseOptions(void)
{
    int option;
    while ((option = GetOptions(switches)) != -1)
    {
        switch (option)
        {
        case -3:
            ThrowError(1824)("Invalid argument: %s", OptFull);
        case 29:
            if (OptArgc < 1)
                ThrowError(1828)("Missing argument");
            gMaxAlloc = atoi(OptArgv[0]) << 20;
            if (!gMaxAlloc)
                gMaxAlloc = 0x2000000;
            break;
        case 24:
            char_148EED = 1;
            bQuickStart = 1;
            char_148EEB = 1;
            gGameOptions.nGameType = GAMETYPE_2;
            break;
        case 0:
            ShowUsage();
            break;
        case 19:
            char_148EEC = 1;
        case 20:
            if (OptArgc < 1)
                ThrowError(1863)("Missing argument");
            strncpy(char_148ef0,OptArgv[0],13);
            char_148ef0[12] = 0;
            bQuickStart = 1;
            char_148EEB = 1;
            if (gGameOptions.nGameType == GAMETYPE_0)
                gGameOptions.nGameType = GAMETYPE_2;
            break;
        case 25:
            bNoDemo = 1;
            break;
        case 18:
            bQuickStart = 1;
            break;
        case 12:
            gUse8250 = 1;
            break;
        case 1:
            gPacketMode = PACKETMODE_2;
            break;
        case 21:
            if (OptArgc < 1)
                ThrowError(1894)("Missing argument");
            strcpy(zCustomName, OptArgv[0]);
            bCustomName = 1;
            break;
        case 2:
            if (OptArgc < 1)
                ThrowError(1901)("Missing argument");
            strcpy(gUserMapFilename, OptArgv[0]);
            bAddUserMap = 1;
            bNoDemo = 1;
            break;
        case 3:
            if (gSyncRate != 1)
                gPacketMode = PACKETMODE_2;
            else
                gPacketMode = PACKETMODE_1;
            break;
        case 4:
            if (OptArgc < 1)
                ThrowError(1917)("Missing argument for -net parameter");
            if (gGameOptions.nGameType == GAMETYPE_0)
                gGameOptions.nGameType = GAMETYPE_2;
            break;
        case 14:
            gAutoAim = 0;
            break;
        case 22:
            bNoResend = 0;
            break;
        case 23:
            bSilentAim = 1;
            break;
        case 5:
            gGameOptions.nMonsterSettings = MONSTERSETTINGS_0;
            break;
        case 6:
            if (OptArgc < 1)
                gDemo.SetupPlayback(NULL);
            else
                gDemo.SetupPlayback(OptArgv[0]);
            break;
        case 7:
            if (OptArgc < 1)
                gDemo.Create(NULL);
            else
                gDemo.Create(OptArgv[0]);
            break;
        case 8:
            gRobust = 1;
            break;
        case 13:
            if (OptArgc < 1)
                ThrowError(1962)("Missing argument");
            func_269D8(OptArgv[0]);
            bNoDemo = 1;
            break;
        case 26:
            if (OptArgc < 1)
                ThrowError(1972)("Missing argument");
            pUserTiles = (char*)malloc(strlen(OptArgv[0]+1));
            if (!pUserTiles)
                return;
            strcpy(pUserTiles,OptArgv[0]);
            break;
        case 27:
            if (OptArgc < 1)
                ThrowError(1984)("Missing argument");
            szSoundRes = (char*)malloc(strlen(OptArgv[0]+1));
            if (!szSoundRes)
                return;
            strcpy(szSoundRes,OptArgv[0]);
            break;
        case 28:
            if (OptArgc < 1)
                ThrowError(1996)("Missing argument");
            pUserRFF = (char*)malloc(strlen(OptArgv[0]+1));
            if (!pUserRFF)
                return;
            strcpy(pUserRFF,OptArgv[0]);
            break;
        case 9:
            if (OptArgc < 1)
                ThrowError(2008)("Missing argument");
            gExplicitSetup = 1;
            strcpy(SetupFilename,OptArgv[0]);
            break;
        case 10:
            if (OptArgc < 1)
                ThrowError(2018)("Missing argument");
            gSkill = ClipRange(strtoul(OptArgv[0],NULL,0), 0, 4);
            break;
        case 15:
            if (OptArgc < 1)
                ThrowError(2030)("Missing argument");
            gSyncRate = ClipRange(strtoul(OptArgv[0],NULL,0), 1, 4);
            if (gPacketMode == PACKETMODE_1)
                gSyncRate = 1;
            else if (gSyncRate == 3)
                gSyncRate = 1;
            break;
        case -2:
            strcpy(gUserMapFilename,OptFull);
            bAddUserMap = 1;
            bNoDemo = 1;
            break;
        case 11:
            bNoCDAudio = 1;
            break;
        }
    }
    if (bAddUserMap)
    {
        char buf1[148];
        char *pNode, *pDir, *pFName, *pExt;
        _splitpath2(gUserMapFilename,buf1,&pNode,&pDir,&pFName,&pExt);
        strcpy(UserPath, pNode);
        strcat(UserPath, pDir);
        strcpy(gUserMapFilename, pFName);
    }
}

void func_86910(void);

void main(void)
{
    CheckIfWindows();
    if (_grow_handles(40) < 40)
        ThrowError(2089)("Not enough file handles available.\nIncrease FILES=## value in CONFIG.SYS.");
    memcpy(&gGameOptions, &gSingleGameOptions, sizeof(GAMEOPTIONS));
    ParseOptions();
    func_26988();
    if (!gExplicitSetup)
        CONFIG_GetSetupFilename();
    CONFIG_ReadSetup();
    if (bCustomName)
        strcpy(PlayerName, zCustomName);
    ulong memoryAvailable = func_A8B50();
    ulong allocMemory;
    if (memoryAvailable > gMaxAlloc)
        allocMemory = gMaxAlloc;
    else
        allocMemory = memoryAvailable-0x20000;
    Resource::heap = new QHeap(allocMemory);

    strcpy(buffer, UserPath);
    strcat(buffer, "*.map");
    if (pUserRFF)
        gSysRes.Init(pUserRFF, buffer);
    else
        gSysRes.Init("BLOOD.RFF", buffer);
    gGuiRes.Init("GUI.RFF", "*.*");

    gOldDisplayMode = getvmode();

    tioInit(1);
    keyInstall();
    BannerToTIO();
    tioPrint("");

    allocMemory += 0x466000;
    if (allocMemory > func_A8B30())
        allocMemory = func_A8B30()-0x20000;

    sprintf(buffer, "%ld MB Memory Used", allocMemory >> 20);
    tioPrint(buffer);
    if (allocMemory < 0x17f8680)
    {
        tioPrint("");
        sprintf(buffer, "LOW MEMORY WARNING: Blood requires %dmb of free memory", 23);
        tioPrint(buffer);
        tioPrint("You may experience problems when running Blood with low memory");
        if (gInWindows && int_28E3D4 != 1)
            tioPrint("Since you are in Windows, try restarting in MS-DOS mode");
        tioPrint("");
        if (!char_148EE9)
        {
            tioPrint("Press a key to enter Blood...");
            while (!keyGet()) {};
        }
    }

    tioPrint("Initializing Build 3D engine");
    scrInit();
    tioPrint("Creating standard color lookups");
    scrCreateStdColors();
    tioPrint("Loading tiles");
    if (pUserTiles)
    {
        strcpy(buffer,pUserTiles);
        strcat(buffer,"%03i.ART");
        if (!tileInit(0,buffer))
            ThrowError(2243)("User specified ART files not found");
    }
    else
    {
        if (!tileInit(0,NULL))
            ThrowError(2248)("TILES###.ART files not found");
    }
    powerupInit();
    tioPrint("Loading cosine table");
    trigInit(gSysRes);
    tioPrint("Initializing view subsystem");
    viewInit();
    tioPrint("Initializing dynamic fire");
    FireInit();
    tioPrint("Initializing weapon animations");
    WeaponInit();
    prevErrorHandler = errSetHandler(GameErrorHandler);
    LoadSavedInfo();
    gDemo.LoadDemoInfo();
    sprintf(buffer, "There are %d demo(s) in the loop", gDemo.DemoCount());
    tioPrint(buffer);
    if (!gDemo.RecordStatus() && gDemo.DemoCount() > 0 && gGameOptions.nGameType == GAMETYPE_0 && !bNoDemo)
        gDemo.SetupPlayback(NULL);
    tioPrint("Loading control setup");
    ctrlInit();
    tioPrint("Installing timer");
    LockClockStrobe();
    timerRegisterClient(ClockStrobe, 120);
    timerInstall();
    int_148E14 = -1;
    if (Redbook.cdrom_setup() == 0 || !Redbook.cdrom_installed() || bNoCDAudio || gGameOptions.nGameType != GAMETYPE_0)
    {
        gRedBookInstalled = 0;
        tioPrint("CD-ROM drive not initialized.");
    }
    else
    {
        tioPrint("CD-ROM drive initialized.");
        Redbook.newdisk();
        if (Redbook.cd_check_audio_track(2) == 1)
        {
            tioPrint("Audio track(s) found.");
            gRedBookInstalled = 1;
            Redbook.f_1f = 0;
            Redbook.f_1b = 0;
            Redbook.f_1d = 0;
            Redbook.cd_status();
            Redbook.func_82BB4();
            Redbook.preprocess();
            Redbook.SetVolume(255);
            Redbook.cd_lock(1);
        }
        else
        {
            tioPrint("Audio track(s) not found.");
            gRedBookInstalled = 0;
        }
    }
    if (char_148EED)
        tenBloodInit();
    else if (char_148EEC || char_148EEB)
    {
        tioPrint("Initializing External Network Service");
        func_10148();
    }
    tioPrint("Initializing network users");
    netInitialize();
    gMe = gView = &gPlayer[myconnectindex];
    gViewIndex = myconnectindex;
    tioPrint("Initializing sound system");
    sndInit(numplayers > 1 ? TRUE : FALSE);
    sfxInit();
    if (MusicDevice != -1 && gUse8250 && numplayers > 1)
        Banner8250();
    func_2906C();
    netBroadcastPlayerInfo(myconnectindex);
    tioPrint("Waiting for network players!");
    netWaitForEveryone(0);
    if (CONTROL_JoystickEnabled)
        CONTROL_CenterJoystick(CenterCenter, UpperLeft, LowerRight, CenterThrottle, CenterRudder);
    scrSetGameMode(ScreenMode, ScreenWidth, ScreenHeight);
    scrSetGamma(gGamma);
    viewResizeView(gViewSize);
    gWeather.Initialize((char*)frameplace, windowx1, windowy1, windowx2 - windowx1 + 1, windowy2 - windowy1 + 1, gYLookup, 0, 32, -1);
    gChoke.func_83ff0(518, func_84230);
    levelLoadDefaults();
    if (bAddUserMap)
    {
        levelAddUserMap(gUserMapFilename);
        gStartNewGame = 1;
    }
    SetupMenus();
_RESTART:
    setview(0,0,xdim-1,ydim-1);
    if (!bQuickStart)
        credLogos();
    scrSetDac();
_RESTARTNOLOGO:
    scrSetGameMode(ScreenMode,ScreenWidth,ScreenHeight);
    scrSetGamma(gGamma);
    viewResizeView(gViewSize);
    gQuitGame = 0;
    gRestartGame = 0;
    if (gGameOptions.nGameType > GAMETYPE_0)
    {
        KB_ClearKeysDown();
        KB_FlushKeyboardQueue();
        keyFlushStream();
    }
    else if (gDemo.PlaybackStatus() && !bAddUserMap && !bNoDemo)
        gDemo.Playback();
    if (gDemo.DemoCount() > 0)
        gGameMenuMgr.Deactivate();
    if (!bAddUserMap && !char_148EEB && !gGameStarted)
        gGameMenuMgr.Push(&menuMain);
    else if (char_148EEB)
        func_1EC78(2518,"Starting Game","Auto-Starting Network Game",0);
    ready2send = 1;
    if (char_148EED)
        func_86910();
    else if (char_148EEC || char_148EEB)
    {
        func_10324();
        func_1EC78(2518,"Starting Game","Auto-Starting Network Game",0);
    }
    while (!gQuitGame && !gTenQuit)
    {
        if (gRedBookInstalled)
            Redbook.preprocess();
        switch (gInputMode)
        {
        case INPUT_MODE_1:
            if (gGameMenuMgr.Active())
                gGameMenuMgr.Process();
            break;
        case INPUT_MODE_0:
            LocalKeys();
            break;
        }
        if (gQuitGame)
            continue;
        if (gRedBookInstalled)
            Redbook.postprocess();
        if (gGameStarted)
        {
            if (numplayers > 1)
                netGetPackets();
            while (gPredictTail < gNetFifoHead[myconnectindex] && !gPaused)
            {
                viewUpdatePrediction(&gFifoInput[gPredictTail&255][myconnectindex]);
            }
            if (numplayers == 1)
                gBufferJitter = 0;
            while (gNetFifoHead[myconnectindex]-gNetFifoTail > gBufferJitter && !gStartNewGame && !gQuitGame)
            {
                for (int i = connecthead; i >= 0; i = connectpoint2[i])
                    if (gNetFifoHead[i] == gNetFifoTail)
                        break;
                if (i >= 0)
                    break;
                faketimerhandler();
                ProcessFrame();
            }
            if (gQuitRequest && gQuitGame)
                clearview(0);
            else
            {
                netCheckSync();
                viewDrawScreen();
            }
        }
        else
        {
            clearview(0);
            rotatesprite(160<<16,100<<16,65536,0,2518,0,0,0x4a,0,0,xdim-1,ydim-1);
            netGetPackets();
            if (gQuitRequest && !gQuitGame)
                netBroadcastMyLogoff();
        }
        switch (gInputMode)
        {
        case INPUT_MODE_1:
            if (gGameMenuMgr.Active())
                gGameMenuMgr.Draw();
            break;
        case INPUT_MODE_2:
            gPlayerMsg.ProcessKeys();
            gPlayerMsg.Draw();
            break;
        case INPUT_MODE_3:
            gEndGameMgr.ProcessKeys();
            gEndGameMgr.Draw();
            break;
        }
        scrNextPage();
        if (TestBitString(gotpic, 2342))
        {
            FireProcess();
            ClearBitString(gotpic, 2342);
        }
        if (char_148E29 && gStartNewGame)
        {
            gStartNewGame = 0;
            gQuitGame = 1;
        }
        if (gStartNewGame)
            StartLevel(&gGameOptions);
    }
    ready2send = 0;
    if (gDemo.RecordStatus())
        gDemo.Close();
    if (gRestartGame)
    {
        gQuitGame = 0;
        gRestartGame = 0;
        gGameStarted = 0;
        levelSetupOptions(0,0);
        while (gGameMenuMgr.Active())
        {
            gGameMenuMgr.Process();
            clearview(0);
            netGetPackets();
            gGameMenuMgr.Draw();
            scrNextPage();
        }
        if (gGameOptions.nGameType != GAMETYPE_0)
            goto _RESTARTNOLOGO;
        if (!gDemo.RecordStatus() && gDemo.DemoCount() > 0 && gGameOptions.nGameType == GAMETYPE_0 && !bNoDemo)
            gDemo.NextDemo();
        goto _RESTART;
    }
    if (gGameOptions.nGameType > GAMETYPE_0 && numplayers > 1)
    {
        netSendEmptyPackets();
        uninitmultiplayers();
    }
    setvmode(gOldDisplayMode);
    sndTerm();
    timerRemove();
    ctrlTerm();
    UnlockClockStrobe();
    uninitengine();
    CONFIG_WriteSetup();
    if (syncstate)
        printf("A packet was lost! (syncstate)\n");
    for (int i = 0; i < 10; i++)
    {
        if (gSaveGamePic[i])
            Resource::Free(gSaveGamePic[i]);
    }
    if (gRedBookInstalled)
    {
        Redbook.StopSong();
        Redbook.cdrom_shutdown();
    }
    errSetHandler(prevErrorHandler);
    if (int_148E0C)
        system(int_148E0C);
    else if (int_148E10)
        system(int_148E10);
    if (char_27B2CC && !gTenQuit && !char_148E29)
        printf("Blood exited the network game because the master computer quit.\n");
}

class BloodLoadSave : public LoadSave {
public:
    void Load(void);
    void Save(void);
};

void BloodLoadSave::Load(void)
{
}
void BloodLoadSave::Save(void)
{
}

static BloodLoadSave myLoadSave;

