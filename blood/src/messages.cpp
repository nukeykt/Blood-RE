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
#include <stdio.h>
#include "typedefs.h"
#include "actor.h"
#include "callback.h"
#include "control.h"
#include "debug4g.h"
#include "demo.h"
#include "eventq.h"
#include "function.h"
#include "gamemenu.h"
#include "globals.h"
#include "key.h"
#include "levels.h"
#include "loadsave.h"
#include "messages.h"
#include "misc.h"
#include "network.h"
#include "player.h"
#include "view.h"

struct hackstruct {
    int f_0;
};

const hackstruct hackvar = { 1 };

CPlayerMsg gPlayerMsg;
CCheatMgr gCheatMgr;

void func_5A928(void)
{
    for (int i = 0; i < NUMGAMEFUNCTIONS; i++)
        CONTROL_ClearButton(i);
}

void func_5A944(byte key)
{
    for (int i = 0; i < NUMGAMEFUNCTIONS; i++)
    {
        int32 key1, key2;
        CONTROL_GetKeyMap(i, &key1, &key2);
        if (key1 == key || key2 == key)
            CONTROL_ClearButton(i);
    }
}

static void SetGodMode(BOOL god)
{
    playerSetGodMode(gMe, god);
    if (gMe->at31a)
        viewSetMessage("You are immortal.");
    else
        viewSetMessage("You are mortal.");
}

static void SetClipMode(BOOL noclip)
{
    gNoClip = noclip;
    if (gNoClip)
        viewSetMessage("Unclipped movement.");
    else
        viewSetMessage("Normal movement.");
}

void packStuff(PLAYER *pPlayer)
{
    for (int i = 0; i < 5; i++)
        packAddItem(pPlayer, i);
}

void packClear(PLAYER *pPlayer)
{
    pPlayer->at321 = 0;
    for (int i = 0; i < 5; i++)
    {
        pPlayer->packInfo[i].at0 = 0;
        pPlayer->packInfo[i].at1 = 0;
    }
}

static void SetAmmo(BOOL stat)
{
    if (stat)
    {
        for (int i = 0; i < 12; i++)
            gMe->at181[i] = gAmmoInfo[i].at0;
        viewSetMessage("You have full ammo.");
    }
    else
    {
        for (int i = 0; i < 12; i++)
            gMe->at181[i] = 0;
        viewSetMessage("You have no ammo.");
    }
}

static void SetWeapons(BOOL stat)
{
    for (int i = 0; i < 14; i++)
    {
        gMe->atcb[i] = stat;
    }
    SetAmmo(stat);
    if (stat)
        viewSetMessage("You have all weapons.");
    else
        viewSetMessage("You have no weapons.");
}

static void SetToys(BOOL stat)
{
    if (stat)
    {
        packStuff(gMe);
        viewSetMessage("Your inventory is full.");
    }
    else
    {
        packClear(gMe);
        viewSetMessage("Your inventory is empty.");
    }
}

static void SetArmor(BOOL stat)
{
    int nAmount;
    if (stat)
    {
        viewSetMessage("You have full armor.");
        nAmount = 3200;
    }
    else
    {
        viewSetMessage("You have no armor.");
        nAmount = 0;
    }
    for (int i = 0; i < 3; i++)
        gMe->at33e[i] = nAmount;
}

static void SetKeys(BOOL stat)
{
    for (int i = 1; i <= 6; i++)
        gMe->at88[i] = stat;
    if (stat)
        viewSetMessage("You have all keys.");
    else
        viewSetMessage("You have no keys.");
}

static void SetInfiniteAmmo(BOOL stat)
{
    gInfiniteAmmo = stat;
    if (gInfiniteAmmo)
        viewSetMessage("You have infinite ammo.");
    else
        viewSetMessage("You have limited ammo.");
}

static void SetMap(BOOL stat)
{
    gFullMap = stat;
    if (gFullMap)
        viewSetMessage("You have the map.");
    else
        viewSetMessage("You have no map.");
}

void SetWooMode(BOOL stat)
{
    if (stat)
    {
        if (!powerupCheck(gMe, 17))
            powerupActivate(gMe, 17);
    }
    else
    {
        if (powerupCheck(gMe, 17))
            powerupDeactivate(gMe, 17);
    }
}

void ToggleWooMode(void)
{
    BOOL t = powerupCheck(gMe, 17) ? 1 : 0;
    SetWooMode(!t);
}

void ToggleBoots(void)
{
    if (powerupCheck(gMe, 15))
    {
        viewSetMessage("You have no Jumping Boots.");
        powerupDeactivate(gMe, 15);
    }
    else
    {
        viewSetMessage("You have the Jumping Boots.");
        powerupActivate(gMe, 15);
    }
}

void ToggleInvisibility(void)
{
    if (powerupCheck(gMe, 13))
    {
        viewSetMessage("You are visible.");
        powerupDeactivate(gMe, 13);
    }
    else
    {
        viewSetMessage("You are invisible.");
        powerupActivate(gMe, 13);
    }
}

void ToggleInvulnerability(void)
{
    if (powerupCheck(gMe, 14))
    {
        viewSetMessage("You are vulnerable.");
        powerupDeactivate(gMe, 14);
    }
    else
    {
        viewSetMessage("You are invulnerable.");
        powerupActivate(gMe, 14);
    }
}

void ToggleDelirium(void)
{
    if (powerupCheck(gMe, 28))
    {
        viewSetMessage("You are not delirious.");
        powerupDeactivate(gMe, 28);
    }
    else
    {
        viewSetMessage("You are delirious.");
        powerupActivate(gMe, 28);
    }
}

void StartLevel(GAMEOPTIONS *gameOptions);
void LevelWarp(int nEpisode, int nLevel)
{
    levelSetupOptions(nEpisode, nLevel);
    StartLevel(&gGameOptions);
    viewResizeView(gViewSize);
}

void LevelWarpAndRecord(int nEpisode, int nLevel)
{
    char buffer[144];
    levelSetupOptions(nEpisode, nLevel);
    gGameStarted = FALSE;
    strcpy(buffer, levelGetFilename(nEpisode, nLevel));
    ChangeExtension(buffer, ".DEM");
    gDemo.Create(buffer);
    StartLevel(&gGameOptions);
    viewResizeView(gViewSize);
}

CGameMessageMgr::CGameMessageMgr()
{
    at1 = 1;
    at5 = 0;
    at9 = 0;
    atd = 0;
    at11 = 0;
    at15 = 8;
    at19 = 4;
    at1d = 5;
    at21 = 15;
    at22 = 0;
    at2a = at26 = 0;
}

void CGameMessageMgr::SetState(byte state)
{
    if (at0 && !state)
    {
        at0 = 0;
        Clear();
    }
    else if (!at0 && state)
        at0 = 1;
}

void CGameMessageMgr::Add(char *a1, byte a2)
{
    if (a2 && at21)
    {
        messageStruct *pMessage = &at2e[at2a];
        strncpy(pMessage->at4, a1, 80);
        pMessage->at4[80] = 0;
        pMessage->at0 = gFrameClock + at1d*120;
        at2a = (at2a+1)%16;
        at22++;
        if (at22 > at19)
        {
            at26 = (at26+1)%16;
            atd = 0;
            at22 = at19;
            at9 = at15;
        }
    }
}

void CGameMessageMgr::Display(void)
{
    if (at22 && at0 && gInputMode != INPUT_MODE_2)
    {
        int v10 = at22;
        int v18 = at26;
        int vc = ClipHigh(v10*8, 48);
        int v14 = gViewMode == 3 ? gViewX0S : 0;
        int v8 = (gViewMode == 3 ? at5 : 0) + at9;
        for (int i = 0; i < v10; i++)
        {
            messageStruct *pMessage = &at2e[(v18+i)%16];
            if (gFrameClock >= pMessage->at0)
            {
                at26 = (at26+1)%16;
                at22--;
                continue;
            }
            viewDrawText(at11, pMessage->at4, v14+1, v8, vc, 0);
            if (gViewMode == 3)
            {
                int height;
                gMenuTextMgr.GetFontInfo(at11, pMessage->at4, &height, NULL);
                if (v14+height > gViewX1S)
                    viewUpdatePages();
            }
            v8 += at15;
            vc = ClipLow(vc-64/v10, -128);
        }
        if (at9)
        {
            at9 = at15*atd/120;
            atd += gFrameTicks;
        }
    }
}

void CGameMessageMgr::Clear(void)
{
    at26 = at2a = at22 = 0;
}

void CGameMessageMgr::SetMaxMessages(int nMessages)
{
    at19 = ClipRange(nMessages, 1, 16);
}

void CGameMessageMgr::SetFont(int nFont)
{
    FONT *pFont = &gFont[nFont];
    at11 = nFont;
    at15 = pFont->ySize;
}

void CGameMessageMgr::SetCoordinates(int x, int y)
{
    at1 = ClipRange(x, 0, gViewX1S);
    at5 = ClipRange(y, 0, gViewY1S);
}

void CGameMessageMgr::SetMessageTime(int nTime)
{
    at1d = ClipRange(nTime, 1, 8);
}

void CGameMessageMgr::SetMessageFlags(unsigned int nFlags)
{
    at21 = nFlags&0xf;
}

void CPlayerMsg::Clear(void)
{
    at4[0] = 0;
    at0 = 0;
}

void CPlayerMsg::Term(void)
{
    Clear();
    gInputMode = INPUT_MODE_0;
}

void CPlayerMsg::Draw(void)
{
    char buffer[44];
    strcpy(buffer, at4);
    if (gGameClock & 16)
        strcat(buffer, "_");
    int x = gViewMode == 3 ? gViewX0S : 0;
    int y = gViewMode == 3 ? gViewY0S : 0;
    if (gViewSize >= 1)
        y += tilesizy[2229]*((gNetPlayers+3)/4);
    viewDrawText(0, buffer, x+1,y+1, -128, 0);
    viewUpdatePages();
}

BOOL CPlayerMsg::AddChar(char ch)
{
    if (at0 < 40)
    {
        at4[at0] = ch;
        at4[++at0] = 0;
        return 1;
    }
    return 0;
}

void CPlayerMsg::DelChar(void)
{
    if (at0 > 0)
        at4[--at0] = 0;
}

void CPlayerMsg::Set(char * pzString)
{
    strncpy(at4, pzString, 40);
    at0 = ClipHigh(strlen(pzString), 40);
    at4[at0] = 0;
}

void CPlayerMsg::Send(void)
{
    netBroadcastMsg(myconnectindex, at4);
    viewSetMessage(at4);
    Term();
    keyFlushStream();
}

void CPlayerMsg::ProcessKeys(void)
{
    byte key;
    if ((key = keyGet()) != 0)
    {
        BOOL alt = keystatus[bsc_LAlt] | keystatus[bsc_RAlt];
        BOOL ctrl = keystatus[bsc_LCtrl] | keystatus[bsc_RCtrl];
        BOOL shift = keystatus[bsc_LShift] | keystatus[bsc_RShift];
        switch (key)
        {
            case bsc_Esc:
                Term();
                break;
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
                CONTROL_ClearButton(gamefunc_See_Chase_View);
                Set(CommbatMacro[key-bsc_F1]);
                Send();
                keystatus[key] = 0;
                break;
            case bsc_Backspace:
                if (ctrl)
                    Clear();
                else
                    DelChar();
                break;
            case bsc_Enter:
            case bsc_Pad_Enter:
                if (gCheatMgr.Check(at4))
                    Term();
                else
                    Send();
                break;
            default:
            {
                byte ch = shift ? ScanToAsciiShifted[key] : ScanToAscii[key];
                if (!ch)
                    break;
                AddChar(ch);
                break;
            }
        }
        func_5A944(key);
    }
}

CCheatMgr::CHEATINFO CCheatMgr::s_CheatInfo[] = {
    {"NQLGB", kCheat5, 0 }, // MPKFA
    {"DBQJONZBTT", kCheat6, 0 }, // CAPINMYASS
    {"OPDBQJONZBTT", kCheat7, 0 }, // NOCAPINMYASS
    {"J!XBOOB!CF!MJLF!LFWJO", kCheat7, 0 }, // I WANNA BE LIKE KEVIN
    {"JEBIP", kCheat8, 0 }, // IDAHO
    {"NPOUBOB", kCheat31, 0 }, // MONTANA
    {"HSJTXPME", kCheat2, 0 }, // GRISWOLD
    {"FENBSL", kCheat11, 0 }, // EDMARK
    {"UFRVJMB", kCheat20, 0 }, // TEQUILA
    {"CVO[", kCheat32, 0 }, // BUNZ
    {"GVOLZ!TIPFT", kCheat21, 0 }, // FUNKY SHOES
    {"HBUFLFFQFS", kCheat26, 0 },
    {"LFZNBTUFS", kCheat22, 0 },
    {"KPKP", kCheat25, 0 },
    {"TBUDIFM", kCheat3, 0 },
    {"TQPSL", kCheat15, 0 },
    {"POFSJOH", kCheat23, 0 },
    {"NBSJP", kCheat28, 1 },
    {"DBMHPO", kCheat28, 1 },
    {"LFWPSLJBO", kCheat9, 0 },
    {"NDHFF", kCheat10, 0 },
    {"LSVFHFS", kCheat12, 0 },
    {"DIFFTFIFBE", kCheat19, 0 },
    {"DPVTUFBV", kCheat33, 0 },
    {"WPPSIFFT", kCheat24, 0 },
    {"MBSB!DSPGU", kCheat29, 0 },
    {"IPOHLPOH", kCheat30, 0 },
    {"GSBOLFOTUFJO", kCheat18, 0 },
    {"TUFSOP", kCheat13, 0 },
    {"DMBSJDF", kCheat17, 0 },
    {"GPSL!ZPV", kCheat34, 0 },
    {"MJFCFSNBO", kCheat35, 0 },
    {"FWB!HBMMJ", kCheat4, 0 },
    {"SBUF", kCheat27, 0 },
    {"HPPOJFT", kCheat16, 0 },
    {"TQJFMCFSH", kCheat36, 1 },
};

unsigned long kCheatFlagsNone;

BOOL CCheatMgr::m_bPlayerCheated;

BOOL CCheatMgr::Check(char *pzString)
{
    int i;
    char buffer[80];
    strcpy(buffer, pzString);
    strupr(buffer);
    for (i = 0; i < strlen(pzString); i++)
        buffer[i]++;
    for (i = 0; i < 36UL; i++)
    {
        int nCheatLen = strlen(s_CheatInfo[i].pzString);
        if (s_CheatInfo[i].flags & 1)
        {
            if (!strncmp(buffer, s_CheatInfo[i].pzString, nCheatLen))
            {
                Process(s_CheatInfo[i].id, buffer+nCheatLen);
                return 1;
            }
        }
        if (!strcmp(buffer, s_CheatInfo[i].pzString))
        {
            Process(s_CheatInfo[i].id, NULL);
            return 1;
        }
    }
    return 0;
}

int parseArgs(char *pzArgs, int *nArg1, int *nArg2)
{
    if (!nArg1 || !nArg2)
        return -1;
    int nLength = strlen(pzArgs);
    for (int i = 0; i < nLength; i++)
        pzArgs[i]--;
    int stat = sscanf(pzArgs, " %d %d", nArg1, nArg2);
    if (stat == 2 && (*nArg1 == 0 || *nArg2 == 0))
        return -1;
    *nArg1 = ClipRange(*nArg1-1, 0, gEpisodeCount-1);
    int nLevels = gEpisodeInfo[*nArg1].nLevels;
    *nArg2 = ClipRange(*nArg2-1, 0, nLevels-1);
    return stat;
}

void CCheatMgr::Process(CCheatMgr::CHEATCODE nCheatCode, char *pzArgs)
{
    dassert(nCheatCode > kCheatNone && nCheatCode < kCheatMax, 886);

    BOOL t = gDemo.at0;
    if (t) return;
    if (nCheatCode == kCheat27)
    {
        gShowFrameRate = !gShowFrameRate;
        return;
    }
    if (gGameOptions.nGameType != GAMETYPE_0)
        return;
    switch (nCheatCode)
    {
    case kCheat36:
    {
        int nEpisode, nLevel;
        if (parseArgs(pzArgs, &nEpisode, &nLevel) == 2)
            LevelWarpAndRecord(nEpisode, nLevel);
        break;
    }
    case kCheat1:
        SetAmmo(1);
        break;
    case kCheat2:
        SetArmor(1);
        break;
    case kCheat3:
        SetToys(1);
        break;
    case kCheat4:
        SetClipMode(!gNoClip);
        break;
    case kCheat5:
        SetGodMode(!gMe->at31a);
        break;
    case kCheat6:
        SetGodMode(0);
        break;
    case kCheat7:
        SetGodMode(1);
        break;
    case kCheat8:
        SetWeapons(1);
        break;
    case kCheat9:
        actDamageSprite(gMe->at5b, gMe->pSprite, kDamageBullet, 8000);
        viewSetMessage("Kevorkian approves.");
        break;
    case kCheat10:
    {
        if (!actGetBurnTime(gMe->pXSprite))
            evPost(gMe->at5b, 3, 0, CALLBACK_ID_0);
        int owner = actSpriteIdToOwnerId(gMe->at5b);
        actBurnSprite(owner, gMe->pXSprite, 2400);
        viewSetMessage("You're fired!");
        break;
    }
    case kCheat11:
        actDamageSprite(gMe->at5b, gMe->pSprite, kDamageExplode, 8000);
        viewSetMessage("Ahhh...those were the days.");
        break;
    case kCheat12:
    {
        actHealDude(gMe->pXSprite, 200, 200);
        gMe->at33e[1] = 200;
        if (!actGetBurnTime(gMe->pXSprite))
            evPost(gMe->at5b, 3, 0, CALLBACK_ID_0);
        int owner = actSpriteIdToOwnerId(gMe->at5b);
        actBurnSprite(owner, gMe->pXSprite, 2400);
        viewSetMessage("Flame retardant!");
        break;
    }
    case kCheat13:
        gMe->at36a = 250;
        break;
    case kCheat14:
        gMe->at35a = 360;
        break;
    case kCheat15:
        actHealDude(gMe->pXSprite, 200, 200);
        break;
    case kCheat16:
        SetMap(!gFullMap);
        break;
    case kCheat18:
        gMe->packInfo[0].at1 = 100;
        break;
    case kCheat19:
        gMe->packInfo[1].at1 = 100;
        break;
    case kCheat20:
        ToggleWooMode();
        break;
    case kCheat21:
        ToggleBoots();
        break;
    case kCheat22:
        SetKeys(1);
        break;
    case kCheat23:
        ToggleInvisibility();
        break;
    case kCheat24:
        ToggleInvulnerability();
        break;
    case kCheat25:
        ToggleDelirium();
        break;
    case kCheat27:
        return;
    case kCheat28:
    {
        int nEpisode, nLevel;
        if (parseArgs(pzArgs, &nEpisode, &nLevel) == 2)
            LevelWarp(nEpisode, nLevel);
        break;
    }
    case kCheat29:
        SetInfiniteAmmo(!gInfiniteAmmo);
        SetWeapons(gInfiniteAmmo);
        break;
    case kCheat30:
        SetWeapons(1);
        SetInfiniteAmmo(1);
        break;
    case kCheat31:
        SetWeapons(1);
        SetToys(1);
        break;
    case kCheat32:
        SetWeapons(1);
        SetWooMode(1);
        break;
    case kCheat33:
        actHealDude(gMe->pXSprite, 200, 200);
        gMe->packInfo[1].at1 = 100;
        break;
    case kCheat34:
        SetInfiniteAmmo(0);
        SetMap(0);
        SetWeapons(0);
        SetAmmo(0);
        SetArmor(0);
        SetToys(0);
        SetKeys(0);
        SetWooMode(1);
        powerupActivate(gMe, 28);
        gMe->pXSprite->health = 16;
        gMe->atcb[1] = 1;
        gMe->atbd = 0;
        gMe->atbe = 1;
        break;
    }
    m_bPlayerCheated = 1;
}

void CCheatMgr::func_5BCF4(void)
{
    m_bPlayerCheated = 0;
    playerSetGodMode(gMe, 0);
    gNoClip = 0;
    packClear(gMe);
    gInfiniteAmmo = 0;
    gFullMap = 0;
}

class MessagesLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

void MessagesLoadSave::Load()
{
    Read(&CCheatMgr::m_bPlayerCheated, sizeof(CCheatMgr::m_bPlayerCheated));
}

void MessagesLoadSave::Save()
{
    Write(&CCheatMgr::m_bPlayerCheated, sizeof(CCheatMgr::m_bPlayerCheated));
}

static MessagesLoadSave myLoadSave;
