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
extern "C" {
#include "types.h"
#include "fx_man.h"
}
#include "typedefs.h"
#include "build.h"
#include "db.h"
#include "endgame.h"
#include "error.h"
#include "gamemenu.h"
#include "globals.h"
#include "key.h"
#include "levels.h"
#include "loadsave.h"
#include "network.h"
#include "messages.h"
#include "player.h"
#include "sound.h"
#include "view.h"

CEndGameMgr gEndGameMgr;
CSecretMgr gSecretMgr;
CKillMgr gKillMgr;

CEndGameMgr::CEndGameMgr()
{
    at0 = 0;
}

void CEndGameMgr::Draw(void)
{
    clearview(0);
    rotatesprite(160<<16, 100<<16, 65536, 0, 2049, 0, 0, 74, 0, 0, xdim-1, ydim-1);
    int nHeight;
    gMenuTextMgr.GetFontInfo(1, NULL, NULL, &nHeight);
    rotatesprite(160<<16, 20<<16, 65536, 0, 2038, -128, 0, 6, 0, 0, xdim-1, ydim-1);
    int nY = 20 - nHeight / 2;
    if (gGameOptions.nGameType == GAMETYPE_0)
    {
        viewDrawText(1, "LEVEL STATS", 160, nY, -128, 0, 1, 0);
        if (CCheatMgr::m_bPlayerCheated)
        {
            viewDrawText(3, ">>> YOU CHEATED! <<<", 160, 32, -128, 0, 1, 1);
        }
        gKillMgr.Draw();
        gSecretMgr.Draw();
    }
    else
    {
        viewDrawText(1, "FRAG STATS", 160, nY, -128, 0, 1, 0);
        gKillMgr.Draw();
    }
    if (int_28E3D4 != 1 && (gGameClock&32))
    {
        viewDrawText(3, "PRESS A KEY TO CONTINUE", 160, 134, -128, 0, 1, 1);
    }
}

void CEndGameMgr::ProcessKeys(void)
{
    if (int_28E3D4 == 1)
    {
        if (gGameOptions.nGameType > GAMETYPE_0 || numplayers > 1)
            netWaitForEveryone(0);
        Finish();
    }
    else
    {
        byte ch = keyGet();
        if (ch == 0)
            return;
        BOOL bAlt = keystatus[bsc_LAlt] | keystatus[bsc_RAlt];
        BOOL bCtrl = keystatus[bsc_LCtrl] | keystatus[bsc_RCtrl];
        BOOL bShift = keystatus[bsc_LShift] | keystatus[bsc_RShift];
        if (gGameOptions.nGameType > GAMETYPE_0 || numplayers > 1)
            netWaitForEveryone(0);
        Finish();
    }
}

void EndLevel(void);

void CEndGameMgr::Setup(void)
{
    at1 = gInputMode;
    gInputMode = INPUT_MODE_3;
    at0 = 1;
    EndLevel();
    sndStartSample(268, 128, -1, 1);
}

void CEndGameMgr::Finish(void)
{
    levelSetupOptions(gGameOptions.nEpisode, gNextLevel);
    gInitialNetPlayers = numplayers;
    if (FXDevice != -1)
        FX_StopAllSounds();
    sndKillAllSounds();
    gStartNewGame = 1;
    gInputMode = at1;
    at0 = 0;
}

CKillMgr::CKillMgr()
{
    Clear();
}

BOOL CKillMgr::AllowedType(SPRITE *pSprite)
{
    if (!pSprite)
        return 0;
    if (pSprite->statnum != 6)
        return 0;
    return pSprite->type != 219 && pSprite->type != 220 && pSprite->type != 245 && pSprite->type != 239;
}

void CKillMgr::SetCount(int nCount)
{
    at0 = nCount;
}

void CKillMgr::AddCount(int nCount)
{
    at0 += nCount;
}

void CKillMgr::AddKill(SPRITE *pSprite)
{
    if (AllowedType(pSprite)) // check type before adding to enemy kills
        at4++;
}

void CKillMgr::CountTotalKills(void)
{
    at0 = 0;
    for (int nSprite = headspritestat[6]; nSprite >= 0; nSprite = nextspritestat[nSprite])
    {
        SPRITE *pSprite = &sprite[nSprite];
        if (!IsDudeSprite(pSprite))
            ThrowError(209)("Non-enemy sprite (%d) in the enemy sprite list.\n", nSprite);
        if (AllowedType(pSprite))
            at0++;
    }
}

void CKillMgr::Draw(void)
{
    char buffer[40];
    if (gGameOptions.nGameType == GAMETYPE_0)
    {
        viewDrawText(1, "KILLS:", 75, 50, -128, 0, 0, 1);
        sprintf(buffer, "%2d", at4);
        viewDrawText(1, buffer, 160, 50, -128, 0, 0, 1);
        viewDrawText(1, "OF", 190, 50, -128, 0, 0, 1);
        sprintf(buffer, "%2d", at0);
        viewDrawText(1, buffer, 220, 50, -128, 0, 0, 1);
    }
    else
    {
        viewDrawText(3, "#", 85, 35, -128, 0, 0, 1);
        viewDrawText(3, "NAME", 100, 35, -128, 0, 0, 1);
        viewDrawText(3, "FRAGS", 210, 35, -128, 0, 0, 1);
        int nStart = 0;
        int nEnd = gInitialNetPlayers;
        if (int_28E3D4 == 1)
        {
            nStart++;
            nEnd++;
        }
        for (int i = nStart; i < nEnd; i++)
        {
            sprintf(buffer, "%-2d", i);
            viewDrawText(3, buffer, 85, 50+8*i, -128, 0, 0, 1);
            sprintf(buffer, "%s", gProfile[i].name);
            viewDrawText(3, buffer, 100, 50+8*i, -128, 0, 0, 1);
            sprintf(buffer, "%d", gPlayer[i].at2c6);
            viewDrawText(3, buffer, 210, 50+8*i, -128, 0, 0, 1);
        }
    }
}

void CKillMgr::Clear(void)
{
    at0 = at4 = 0;
}

CSecretMgr::CSecretMgr(void)
{
    Clear();
}

void CSecretMgr::SetCount(int nCount)
{
    at0 = nCount;
}

void CSecretMgr::Found(int nType)
{
    if (nType < 0)
        ThrowError(298)("Invalid secret type %d triggered.", nType);
    if (nType == 0)
        at4++;
    else
        at8++;
    if (gGameOptions.nGameType == GAMETYPE_0)
    {
        switch (Random(2))
        {
        case 0:
            viewSetMessage("A secret is revealed.");
            break;
        case 1:
            viewSetMessage("You found a secret.");
            break;
        }
    }
}

void CSecretMgr::Draw(void)
{
    char buffer[40];
    viewDrawText(1, "SECRETS:", 75, 70, -128, 0, 0, 1);
    sprintf(buffer, "%2d", at4);
    viewDrawText(1, buffer, 160, 70, -128, 0, 0, 1);
    viewDrawText(1, "OF", 190, 70, -128, 0, 0, 1);
    sprintf(buffer, "%2d", at0);
    viewDrawText(1, buffer, 220, 70, -128, 0, 0, 1);
    if (at8 > 0)
        viewDrawText(1, "YOU FOUND A SUPER SECRET!", 160, 100, -128, 2, 1, 1);
}

void CSecretMgr::Clear(void)
{
    at0 = at4 = at8 = 0;
}

class EndGameLoadSave : public LoadSave {
public:
    void Load(void);
    void Save(void);
};

void EndGameLoadSave::Load(void)
{
    Read(&gSecretMgr.at0, 4);
    Read(&gSecretMgr.at4, 4);
    Read(&gSecretMgr.at8, 4);
    Read(&gKillMgr.at0, 4);
    Read(&gKillMgr.at4, 4);
}

void EndGameLoadSave::Save(void)
{
    Write(&gSecretMgr.at0, 4);
    Write(&gSecretMgr.at4, 4);
    Write(&gSecretMgr.at8, 4);
    Write(&gKillMgr.at0, 4);
    Write(&gKillMgr.at4, 4);
}

static EndGameLoadSave myLoadSave;
