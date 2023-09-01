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
#include <io.h>
#include "typedefs.h"
#include "cdrom.h"
#include "config.h"
#include "control.h"
#include "debug4g.h"
#include "demo.h"
#include "gamemenu.h"
#include "globals.h"
#include "inifile.h"
#include "levels.h"
#include "loadsave.h"
#include "menu.h"
#include "messages.h"
#include "network.h"
#include "resource.h"
#include "screen.h"
#include "sound.h"
#include "view.h"

void SaveGame(CGameMenuItemZEditBitmap *, CGameMenuEvent *);

void SaveGameProcess(CGameMenuItemChain *);
void SetDifficultyAndStart(CGameMenuItemChain *);
void SetDetail(CGameMenuItemSlider *);
void SetGamma(CGameMenuItemSlider *);
void SetMusicVol(CGameMenuItemSlider *);
void SetSoundVol(CGameMenuItemSlider *);
void SetCDVol(CGameMenuItemSlider *);
void SetDoppler(CGameMenuItemZBool *);
void SetCrosshair(CGameMenuItemZBool *);
void SetShowWeapons(CGameMenuItemZBool *);
void SetSlopeTilting(CGameMenuItemZBool *);
void SetViewBobbing(CGameMenuItemZBool *);
void SetViewSwaying(CGameMenuItemZBool *);
void SetMouseSensitivity(CGameMenuItemSlider *);
void SetMouseAimFlipped(CGameMenuItemZBool *);
void SetTurnSpeed(CGameMenuItemSlider *);
void ResetKeys(CGameMenuItemChain *);
void SetMessages(CGameMenuItemZBool *);
void LoadGame(CGameMenuItemZEditBitmap *, CGameMenuEvent *);
void SetupNetLevels(CGameMenuItemZCycle *);
void StartNetGame(CGameMenuItemChain *);
void SetParentalLock(CGameMenuItemZBool *);
void TenProcess(CGameMenuItemChain *);
void SetupLevelMenuItem(int);

short gQuickLoadSlot = -1;
short gQuickSaveSlot = -1;

char strRestoreGameStrings[][16] = 
{
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
    "<Empty>",
};

char *zNetGameTypes[] =
{
    "Cooperative",
    "Bloodbath",
    "Teams",
};

char *zMonsterStrings[] =
{
    "None",
    "Bring 'em on",
    "Respawn",
};

char *zWeaponStrings[] =
{
    "Do not Respawn",
    "Are Permanent",
    "Respawn",
    "Respawn with Markers",
};

char *zItemStrings[] =
{
    "Do not Respawn",
    "Respawn",
    "Respawn with Markers",
};

char *zRespawnStrings[] =
{
    "At Random Locations",
    "Close to Weapons",
    "Away from Enemies",
};

char *zDiffStrings[] =
{
    "STILL KICKING",
    "PINK ON THE INSIDE",
    "LIGHTLY BROILED",
    "WELL DONE",
    "EXTRA CRISPY",
};

char zUserMapName[13];
char *zEpisodeNames[6];
char *zLevelNames[6][16];

CGameMenu menuMain;
CGameMenu menuMainWithSave;
CGameMenu menuNetMain;
CGameMenu menuNetStart;
CGameMenu menuEpisode;
CGameMenu menuDifficulty;
CGameMenu menuOptions;
CGameMenu menuControls;
CGameMenu menuMessages;
CGameMenu menuKeys;
CGameMenu menuSaveGame;
CGameMenu menuLoadGame;
CGameMenu menuLoading;
CGameMenu menuSounds;
CGameMenu menuQuit;
CGameMenu menuCredits;
CGameMenu menuOrder;
CGameMenu menuOnline;
CGameMenu menuParentalLock;
CGameMenu menuSorry;
CGameMenu menuSorry2;

CGameMenuItemQAV itemBloodQAV("", 3, 160, 100, "BDRIP");
CGameMenuItemQAV itemCreditsQAV("", 3, 160, 100, "CREDITS");
CGameMenuItemQAV itemHelp3QAV("", 3, 160, 100, "HELP3");
CGameMenuItemQAV itemHelp3BQAV("", 3, 160, 100, "HELP3B");
CGameMenuItemQAV itemHelp4QAV("", 3, 160, 100, "HELP4");
CGameMenuItemQAV itemHelp5QAV("", 3, 160, 100, "HELP5");

CGameMenuItemTitle itemMainTitle("BLOOD", 1, 160, 20, 2038);
CGameMenuItemChain itemMain1("NEW GAME", 1, 0, 45, 320, 1, &menuEpisode, -1, NULL, 0);
CGameMenuItemChain itemMain2("PLAY ONLINE", 1, 0, 65, 320, 1, &menuOnline, -1, NULL, 0);
CGameMenuItemChain itemMain3("OPTIONS", 1, 0, 85, 320, 1, &menuOptions, -1, NULL, 0);
CGameMenuItemChain itemMain4("LOAD GAME", 1, 0, 105, 320, 1, &menuLoadGame, -1, NULL, 0);
CGameMenuItemChain itemMain5("HELP", 1, 0, 125, 320, 1, &menuOrder, -1, NULL, 0);
CGameMenuItemChain itemMain6("CREDITS", 1, 0, 145, 320, 1, &menuCredits, -1, NULL, 0);
CGameMenuItemChain itemMain7("QUIT", 1, 0, 165, 320, 1, &menuQuit, -1, NULL, 0);

CGameMenuItemTitle itemMainSaveTitle("BLOOD", 1, 160, 20, 2038);
CGameMenuItemChain itemMainSave1("NEW GAME", 1, 0, 45, 320, 1, &menuEpisode, -1, NULL, 0);
CGameMenuItemChain itemMainSave2("PLAY ONLINE", 1, 0, 60, 320, 1, &menuOnline, -1, NULL, 0);
CGameMenuItemChain itemMainSave3("OPTIONS", 1, 0, 75, 320, 1, &menuOptions, -1, NULL, 0);
CGameMenuItemChain itemMainSave4("SAVE GAME", 1, 0, 90, 320, 1, &menuSaveGame, -1, SaveGameProcess, 0);
CGameMenuItemChain itemMainSave5("LOAD GAME", 1, 0, 105, 320, 1, &menuLoadGame, -1, NULL, 0);
CGameMenuItemChain itemMainSave6("HELP", 1, 0, 120, 320, 1, &menuOrder, -1, NULL, 0);
CGameMenuItemChain itemMainSave7("CREDITS", 1, 0, 135, 320, 1, &menuCredits, -1, NULL, 0);
CGameMenuItemChain itemMainSave8("QUIT", 1, 0, 150, 320, 1, &menuQuit, -1, NULL, 0);

CGameMenuItemTitle itemEpisodeTitle("EPISODES", 1, 160, 20, 2038);
CGameMenuItemChain7F2F0 itemEpisodes[6];

CGameMenuItemTitle itemDifficultyTitle("DIFFICULTY", 1, 160, 20, 2038);
CGameMenuItemChain itemDifficulty1("STILL KICKING", 1, 0, 60, 320, 1, NULL, -1, SetDifficultyAndStart, 0);
CGameMenuItemChain itemDifficulty2("PINK ON THE INSIDE", 1, 0, 80, 320, 1, NULL, -1, SetDifficultyAndStart, 1);
CGameMenuItemChain itemDifficulty3("LIGHTLY BROILED", 1, 0, 100, 320, 1, NULL, -1, SetDifficultyAndStart, 2);
CGameMenuItemChain itemDifficulty4("WELL DONE", 1, 0, 120, 320, 1, NULL, -1, SetDifficultyAndStart, 3);
CGameMenuItemChain itemDifficulty5("EXTRA CRISPY", 1, 0, 140, 320, 1, NULL, -1, SetDifficultyAndStart, 4);

CGameMenuItemTitle itemOptionsTitle("OPTIONS", 1, 160, 20, 2038);
CGameMenuItemChain itemOption1("CONTROLS...", 3, 0, 40, 320, 1, &menuControls, -1, NULL, 0);
CGameMenuItemSlider sliderDetail("DETAIL:", 3, 66, 50, 180, gDetail, 0, 4, 1, SetDetail, -1, -1);
CGameMenuItemSlider sliderGamma("GAMMA:", 3, 66, 60, 180, gGamma, 0, 15, 2, SetGamma, -1, -1);
CGameMenuItemSlider sliderMusic("MUSIC:", 3, 66, 70, 180, MusicVolume, 0, 256, 48, SetMusicVol, -1, -1);
CGameMenuItemSlider sliderSound("SOUND:", 3, 66, 80, 180, FXVolume, 0, 256, 48, SetSoundVol, -1, -1);
CGameMenuItemSlider sliderCDAudio("CD AUDIO:", 3, 66, 90, 180, CDVolume, 0, 256, 48, SetCDVol, -1, -1);
CGameMenuItemZBool boolDoppler("3D AUDIO:", 3, 66, 100, 180, gDoppler, SetDoppler, NULL, NULL);
CGameMenuItemZBool boolCrosshair("CROSSHAIR:", 3, 66, 110, 180, gAimReticle, SetCrosshair, NULL, NULL);
CGameMenuItemZBool boolShowWeapons("SHOW WEAPONS:", 3, 66, 120, 180, gShowWeapon, SetShowWeapons, NULL, NULL);
CGameMenuItemZBool boolSlopeTilting("SLOPE TILTING:", 3, 66, 130, 180, gSlopeTilting, SetSlopeTilting, NULL, NULL);
CGameMenuItemZBool boolViewBobbing("VIEW BOBBING:", 3, 66, 140, 180, gViewVBobbing, SetViewBobbing, NULL, NULL);
CGameMenuItemZBool boolViewSwaying("VIEW SWAYING:", 3, 66, 150, 180, gViewHBobbing, SetViewSwaying, NULL, NULL);
CGameMenuItem7EE34 itemVideoMode("VIDEO MODE...", 3, 0, 160, 320, 1);
CGameMenuItemChain itemChainParentalLock("PARENTAL LOCK", 3, 0, 170, 320, 1, &menuParentalLock, -1, NULL, 0);

CGameMenuItemTitle itemControlsTitle("CONTROLS", 1, 160, 20, 2038);
CGameMenuItemSlider sliderMouseSpeed("Mouse Sensitivity:", 1, 10, 70, 300, gMouseSensitivity, 0, 0x20000, 0x1000, SetMouseSensitivity, -1,-1);
CGameMenuItemZBool boolMouseFlipped("Invert Mouse Aim:", 1, 10, 90, 300, gMouseAimingFlipped, SetMouseAimFlipped, NULL, NULL);
CGameMenuItemSlider sliderTurnSpeed("Key Turn Speed:", 1, 10, 110, 300, gTurnSpeed, 64, 128, 4, SetTurnSpeed, -1, -1);
CGameMenuItemChain itemChainKeyList("Configure Keys...", 1, 0, 130, 320, 1, &menuKeys, -1, NULL, 0);
CGameMenuItemChain itemChainKeyReset("Reset Keys...", 1, 0, 160, 320, 1, &menuKeys, -1, ResetKeys, 0);

CGameMenuItemTitle itemMessagesTitle("MESSAGES", 1, 160, 20, 2038);
CGameMenuItemZBool boolMessages("MESSAGES:", 3, 66, 70, 180, 0, SetMessages, NULL, NULL);
CGameMenuItemSlider sliderMsgCount("MESSAGE COUNT:", 3, 66, 80, 180, gMessageCount, 1, 16, 1, NULL, -1, -1);
CGameMenuItemSlider sliderMsgTime("MESSAGE TIME:", 3, 66, 90, 180, gMessageTime, 1, 8, 1, NULL, -1, -1);
CGameMenuItemZBool boolMsgFont("LARGE FONT:", 3, 66, 100, 180, 0, 0, NULL, NULL);
CGameMenuItemZBool boolMsgIncoming("INCOMING:", 3, 66, 110, 180, 0, 0, NULL, NULL);
CGameMenuItemZBool boolMsgSelf("SELF PICKUP:", 3, 66, 120, 180, 0, 0, NULL, NULL);
CGameMenuItemZBool boolMsgOther("OTHER PICKUP:", 3, 66, 130, 180, 0, 0, NULL, NULL);
CGameMenuItemZBool boolMsgRespawn("RESPAWN:", 3, 66, 140, 180, 0, 0, NULL, NULL);

CGameMenuItemTitle itemKeysTitle("KEY SETUP", 1, 160, 20, 2038);
CGameMenuItemKeyList itemKeyList("", 3, 56, 40, 200, 16, 54, 0);

CGameMenuItemTitle itemSaveTitle("Save Game", 1, 160, 20, 2038);
CGameMenuItemZEditBitmap itemSaveGame1(NULL, 3, 20, 60, 320, strRestoreGameStrings[0], 16, 1, SaveGame, 0);
CGameMenuItemZEditBitmap itemSaveGame2(NULL, 3, 20, 70, 320, strRestoreGameStrings[1], 16, 1, SaveGame, 1);
CGameMenuItemZEditBitmap itemSaveGame3(NULL, 3, 20, 80, 320, strRestoreGameStrings[2], 16, 1, SaveGame, 2);
CGameMenuItemZEditBitmap itemSaveGame4(NULL, 3, 20, 90, 320, strRestoreGameStrings[3], 16, 1, SaveGame, 3);
CGameMenuItemZEditBitmap itemSaveGame5(NULL, 3, 20, 100, 320, strRestoreGameStrings[4], 16, 1, SaveGame, 4);
CGameMenuItemZEditBitmap itemSaveGame6(NULL, 3, 20, 110, 320, strRestoreGameStrings[5], 16, 1, SaveGame, 5);
CGameMenuItemZEditBitmap itemSaveGame7(NULL, 3, 20, 120, 320, strRestoreGameStrings[6], 16, 1, SaveGame, 6);
CGameMenuItemZEditBitmap itemSaveGame8(NULL, 3, 20, 130, 320, strRestoreGameStrings[7], 16, 1, SaveGame, 7);
CGameMenuItemZEditBitmap itemSaveGame9(NULL, 3, 20, 140, 320, strRestoreGameStrings[8], 16, 1, SaveGame, 8);
CGameMenuItemZEditBitmap itemSaveGame10(NULL, 3, 20, 150, 320, strRestoreGameStrings[9], 16, 1, SaveGame, 9);
CGameMenuItemBitmapLS itemSaveGamePic(NULL, 3, 0, 0, 2050);

CGameMenuItemTitle itemLoadTitle("Load Game", 1, 160, 20, 2038);
CGameMenuItemZEditBitmap itemLoadGame1(NULL, 3, 20, 60, 320, strRestoreGameStrings[0], 16, 1, LoadGame, 0);
CGameMenuItemZEditBitmap itemLoadGame2(NULL, 3, 20, 70, 320, strRestoreGameStrings[1], 16, 1, LoadGame, 1);
CGameMenuItemZEditBitmap itemLoadGame3(NULL, 3, 20, 80, 320, strRestoreGameStrings[2], 16, 1, LoadGame, 2);
CGameMenuItemZEditBitmap itemLoadGame4(NULL, 3, 20, 90, 320, strRestoreGameStrings[3], 16, 1, LoadGame, 3);
CGameMenuItemZEditBitmap itemLoadGame5(NULL, 3, 20, 100, 320, strRestoreGameStrings[4], 16, 1, LoadGame, 4);
CGameMenuItemZEditBitmap itemLoadGame6(NULL, 3, 20, 110, 320, strRestoreGameStrings[5], 16, 1, LoadGame, 5);
CGameMenuItemZEditBitmap itemLoadGame7(NULL, 3, 20, 120, 320, strRestoreGameStrings[6], 16, 1, LoadGame, 6);
CGameMenuItemZEditBitmap itemLoadGame8(NULL, 3, 20, 130, 320, strRestoreGameStrings[7], 16, 1, LoadGame, 7);
CGameMenuItemZEditBitmap itemLoadGame9(NULL, 3, 20, 140, 320, strRestoreGameStrings[8], 16, 1, LoadGame, 8);
CGameMenuItemZEditBitmap itemLoadGame10(NULL, 3, 20, 150, 320, strRestoreGameStrings[9], 16, 1, LoadGame, 9);
CGameMenuItemBitmapLS itemLoadGamePic(NULL, 3, 0, 0, 2518);

CGameMenuItemTitle itemNetStartTitle("NETWORK GAME", 1, 160, 20, 2038);
CGameMenuItemZCycle itemNetStart1("GAME", 1, 20, 35, 280, 0, 0, zNetGameTypes, 3, 0);
CGameMenuItemZCycle itemNetStart2("EPISODE", 1, 20, 50, 280, 0, SetupNetLevels, NULL, 0, 0);
CGameMenuItemZCycle itemNetStart3("LEVEL", 1, 20, 65, 280, 0, NULL, NULL, 0, 0);
CGameMenuItemZCycle itemNetStart4("DIFFICULTY", 1, 20, 80, 280, 0, 0, zDiffStrings, 5, 0);
CGameMenuItemZCycle itemNetStart5("MONSTERS", 1, 20, 95, 280, 0, 0, zMonsterStrings, 3, 0);
CGameMenuItemZCycle itemNetStart6("WEAPONS", 1, 20, 110, 280, 0, 0, zWeaponStrings, 4, 0);
CGameMenuItemZCycle itemNetStart7("ITEMS", 1, 20, 125, 280, 0, 0, zItemStrings, 3, 0);
CGameMenuItemZEdit itemNetStart9("USER MAP:", 1, 20, 155, 280, zUserMapName, 13, 0, NULL, 0);
CGameMenuItemChain itemNetStart10("START GAME", 1, 20, 170, 280, 0, 0, -1, StartNetGame, 0);

CGameMenuItemText itemLoadingText("LOADING...", 1, 160, 100, 1);

CGameMenuItemTitle itemSoundsTitle("SOUNDS", 1, 160, 20, 2038);
CGameMenuItemSlider itemSoundsMusic("MUSIC:", 3, 40, 60, 180, MusicVolume, 0, 256, 48, SetMusicVol, -1, -1);
CGameMenuItemSlider itemSoundsSound("SOUND:", 3, 40, 70, 180, FXVolume, 0, 256, 48, SetSoundVol, -1, -1);
CGameMenuItemSlider itemSoundsCDAudio("CD AUDIO:", 3, 40, 80, 180, CDVolume, 0, 256, 48, SetCDVol, -1, -1);
CGameMenuItemZBool itemSoundsDoppler("3D SOUND:", 3, 40, 90, 180, gDoppler, SetDoppler, NULL, NULL);

CGameMenuItemTitle itemQuitTitle("QUIT", 1, 160, 20, 2038);
CGameMenuItemText itemQuitText1("Do you really want to quit?", 0, 160, 100, 1);
CGameMenuItemYesNoQuit itemQuitYesNo("[Y/N]", 0, 20, 110, 280, 1, -1, 0);

CGameMenuItemPicCycle itemCreditsPicCycle(0, 0, NULL, NULL, 0, 0);
CGameMenuItemPicCycle itemOrderPicCycle(0, 0, NULL, NULL, 0, 0);

CGameMenuItemTitle itemParentalLockTitle("PARENTAL LOCK", 1, 160, 20, 2038);
CGameMenuItemZBool itemParentalLockToggle("LOCK:", 3, 66, 70, 180, 0, SetParentalLock, NULL, NULL);
CGameMenuItemPassword itemParentalLockPassword("SET PASSWORD:", 3, 160, 80);

CGameMenuItemPicCycle itemSorryPicCycle(0, 0, NULL, NULL, 0, 0);

CGameMenuItemText itemSorryText1("Loading and saving games", 0, 160, 90, 1);
CGameMenuItemText itemSorryText2("not supported", 0, 160, 100, 1);
CGameMenuItemText itemSorryText3("in this demo version of Blood.", 0, 160, 110, 1);

CGameMenuItemText itemSorry2Text1("Buy the complete version of", 0, 160, 90, 1);
CGameMenuItemText itemSorry2Text2("Blood for three new episodes", 0, 160, 100, 1);
CGameMenuItemText itemSorry2Text3("plus eight BloodBath-only levels!", 0, 160, 110, 1);

CGameMenuItemTitle itemOnlineTitle(" ONLINE ", 1, 160, 20, 2038);
CGameMenuItem7EA1C itemOnline1("DWANGO", 1, 0, 45, 320, "matt", "DWANGO", 1, -1, NULL, NULL);
CGameMenuItem7EA1C itemOnline2("RTIME", 1, 0, 65, 320, "matt", "RTIME", 1, -1, NULL, NULL);
CGameMenuItem7EA1C itemOnline3("HEAT", 1, 0, 85, 320, "matt", "HEAT", 1, -1, NULL, NULL);
CGameMenuItem7EA1C itemOnline4("KALI", 1, 0, 105, 320, "matt", "KALI", 1, -1, NULL, NULL);
CGameMenuItem7EA1C itemOnline5("MPATH", 1, 0, 125, 320, "matt", "MPATH", 1, -1, NULL, NULL);
CGameMenuItemChain itemOnline6("TEN", 1, 0, 145, 320, 1, NULL, -1, TenProcess, NULL);

void SetupLoadingScreen(void)
{
    menuLoading.Add(&itemLoadingText, 1);
}

void SetupKeyListMenu(void)
{
    menuKeys.Add(&itemKeysTitle, 0);
    menuKeys.Add(&itemKeyList, 1);
    menuKeys.Add(&itemBloodQAV, 0);
}

void SetupMessagesMenu(void)
{
    menuMessages.Add(&itemMessagesTitle, 0);
    menuMessages.Add(&boolMessages, 1);
    menuMessages.Add(&sliderMsgCount, 0);
    menuMessages.Add(&sliderMsgTime, 0);
    menuMessages.Add(&boolMsgFont, 0);
    menuMessages.Add(&boolMsgIncoming, 0);
    menuMessages.Add(&boolMsgSelf, 0);
    menuMessages.Add(&boolMsgOther, 0);
    menuMessages.Add(&boolMsgRespawn, 0);
    menuMessages.Add(&itemBloodQAV, 0);
}

void SetupControlsMenu(void)
{
    sliderMouseSpeed.at24 = ClipRange(gMouseSensitivity, sliderMouseSpeed.at28, sliderMouseSpeed.at2c);
    sliderTurnSpeed.at24 = ClipRange(gTurnSpeed, sliderTurnSpeed.at28, sliderTurnSpeed.at2c);
    boolMouseFlipped.at20 = gMouseAimingFlipped;
    menuControls.Add(&itemControlsTitle, 0);
    menuControls.Add(&sliderMouseSpeed, 1);
    menuControls.Add(&boolMouseFlipped, 0);
    menuControls.Add(&sliderTurnSpeed, 0);
    menuControls.Add(&itemChainKeyList, 0);
    menuControls.Add(&itemChainKeyReset, 0);
    menuControls.Add(&itemBloodQAV, 0);
}

void SetupOptionsMenu(void)
{
    sliderDetail.at24 = ClipRange(gDetail, sliderDetail.at28, sliderDetail.at2c);
    sliderGamma.at24 = ClipRange(gGamma, sliderGamma.at28, sliderGamma.at2c);
    sliderMusic.at24 = ClipRange(MusicVolume, sliderMusic.at28, sliderMusic.at2c);
    sliderSound.at24 = ClipRange(FXVolume, sliderSound.at28, sliderSound.at2c);
    sliderCDAudio.at24 = ClipRange(Redbook.GetVolume(), sliderCDAudio.at28, sliderCDAudio.at2c);
    boolDoppler.at20 = gDoppler;
    boolCrosshair.at20 = gAimReticle;
    boolShowWeapons.at20 = gShowWeapon;
    boolSlopeTilting.at20 = gSlopeTilting;
    boolViewBobbing.at20 = gViewVBobbing;
    boolViewSwaying.at20 = gViewHBobbing;
    boolMessages.at20 = gGameMessageMgr.at0;
    menuOptions.Add(&itemOptionsTitle, 0);
    menuOptions.Add(&itemOption1, 1);
    menuOptions.Add(&sliderDetail, 0);
    menuOptions.Add(&sliderGamma, 0);
    menuOptions.Add(&sliderMusic, 0);
    menuOptions.Add(&sliderSound, 0);
    menuOptions.Add(&sliderCDAudio, 0);
    menuOptions.Add(&boolDoppler, 0);
    menuOptions.Add(&boolCrosshair, 0);
    menuOptions.Add(&boolShowWeapons, 0);
    menuOptions.Add(&boolSlopeTilting, 0);
    menuOptions.Add(&boolViewBobbing, 0);
    menuOptions.Add(&boolViewSwaying, 0);
    menuOptions.Add(&itemVideoMode, 0);
    menuOptions.Add(&itemChainParentalLock, 0);
    menuOptions.Add(&itemBloodQAV, 0);
}

void SetupDifficultyMenu(void)
{
    menuDifficulty.Add(&itemDifficultyTitle, 0);
    menuDifficulty.Add(&itemDifficulty1, 0);
    menuDifficulty.Add(&itemDifficulty2, 0);
    menuDifficulty.Add(&itemDifficulty3, 1);
    menuDifficulty.Add(&itemDifficulty4, 0);
    menuDifficulty.Add(&itemDifficulty5, 0);
    menuDifficulty.Add(&itemBloodQAV, 0);
}

void SetupLevelMenuItem(int nEpisode)
{
    dassert(nEpisode >= 0 && nEpisode < gEpisodeCount, 471);
    EPISODEINFO *pEpisdeInfo = &gEpisodeInfo[nEpisode];
    itemNetStart3.SetTextArray(zLevelNames[nEpisode], pEpisdeInfo->nLevels, 0);
}

void SetupEpisodeMenu(void)
{
    menuEpisode.Add(&itemEpisodeTitle, 0);
    BOOL unk = 0;
    int height;
    gMenuTextMgr.GetFontInfo(1, NULL, NULL, &height);
    int j = 0;
    for (int i = 0; i < 6; i++)
    {
        EPISODEINFO *pEpisode = &gEpisodeInfo[i];
        if (!pEpisode->bloodbath || gGameOptions.nGameType != GAMETYPE_0)
        {
            if (j < gEpisodeCount)
            {
                int t = 55+(height+8)*j;
                CGameMenuItemChain7F2F0 *pEpisodeItem = &itemEpisodes[j];
                pEpisodeItem->at4 = pEpisode->at0;
                pEpisodeItem->at8 = 1;
                pEpisodeItem->atc = 0;
                pEpisodeItem->at10 = t;
                pEpisodeItem->at14 = 320;
                pEpisodeItem->at20 = 1;
                pEpisodeItem->at34 = i;
                if (!unk || j == 0)
                {
                    pEpisodeItem = &itemEpisodes[j];
                    pEpisodeItem->at24 = &menuDifficulty;
                    pEpisodeItem->at28 = 3;
                }
                else
                {
                    pEpisodeItem->at24 = &menuSorry2;
                    pEpisodeItem->at28 = 1;
                }
                pEpisodeItem = &itemEpisodes[j];
                pEpisodeItem->at18 |= 3;
                BOOL first = j == 0 ? 1 : 0;
                menuEpisode.Add(&itemEpisodes[j], first);
                if (first)
                    SetupLevelMenuItem(j);
            }
            j++;
        }
    }
    menuEpisode.Add(&itemBloodQAV, 0);
}

void SetupMainMenu(void)
{
    if (int_28E3D4 == 3)
    {
        menuMain.Add(&itemMainTitle, 0);
        menuMain.Add(&itemMain3, 1);
        menuMain.Add(&itemMain5, 0);
        menuMain.Add(&itemMain6, 0);
        menuMain.Add(&itemMain7, 0);
        menuMain.Add(&itemBloodQAV, 0);
    }
    else
    {
        menuMain.Add(&itemMainTitle, 0);
        menuMain.Add(&itemMain1, 1);
        if (gGameOptions.nGameType > GAMETYPE_0)
        {
            itemMain1.at24 = &menuNetStart;
            itemMain1.at28 = 2;
        }
        menuMain.Add(&itemMain2, 0);
        menuMain.Add(&itemMain3, 0);
        menuMain.Add(&itemMain4, 0);
        menuMain.Add(&itemMain5, 0);
        menuMain.Add(&itemMain6, 0);
        menuMain.Add(&itemMain7, 0);
        menuMain.Add(&itemBloodQAV, 0);
    }
}

void SetupMainMenuWithSave(void)
{
    if (int_28E3D4 == 3)
    {
        menuMainWithSave.Add(&itemMainSaveTitle, 0);
        menuMainWithSave.Add(&itemMainSave3, 1);
        menuMainWithSave.Add(&itemMainSave6, 0);
        menuMainWithSave.Add(&itemMainSave7, 0);
        menuMainWithSave.Add(&itemMainSave8, 0);
        menuMainWithSave.Add(&itemBloodQAV, 0);
    }
    else
    {
        menuMainWithSave.Add(&itemMainSaveTitle, 0);
        menuMainWithSave.Add(&itemMainSave1, 1);
        if (gGameOptions.nGameType > GAMETYPE_0)
        {
            itemMainSave1.at24 = &menuNetStart;
            itemMainSave1.at28 = 2;
        }
        menuMainWithSave.Add(&itemMainSave2, 0);
        menuMainWithSave.Add(&itemMainSave3, 0);
        menuMainWithSave.Add(&itemMainSave4, 0);
        menuMainWithSave.Add(&itemMainSave5, 0);
        menuMainWithSave.Add(&itemMainSave6, 0);
        menuMainWithSave.Add(&itemMainSave7, 0);
        menuMainWithSave.Add(&itemMainSave8, 0);
        menuMainWithSave.Add(&itemBloodQAV, 0);
    }
}

void SetupNetStartMenu(void)
{
    BOOL oneEpisode = 0;
    menuNetStart.Add(&itemNetStartTitle, 0);
    menuNetStart.Add(&itemNetStart1, 1);
    for (int i = 0; i < (oneEpisode ? 1 : 6); i++)
    {
        EPISODEINFO *pEpisode = &gEpisodeInfo[i];
        if (i < gEpisodeCount)
            itemNetStart2.Add(pEpisode->at0, i == 0 ? 1 : 0);
    }
    menuNetStart.Add(&itemNetStart2, 0);
    menuNetStart.Add(&itemNetStart3, 0);
    menuNetStart.Add(&itemNetStart4, 0);
    menuNetStart.Add(&itemNetStart5, 0);
    menuNetStart.Add(&itemNetStart6, 0);
    menuNetStart.Add(&itemNetStart7, 0);
    menuNetStart.Add(&itemNetStart9, 0);
    menuNetStart.Add(&itemNetStart10, 0);
    itemNetStart1.SetTextIndex(1);
    itemNetStart4.SetTextIndex(2);
    itemNetStart5.SetTextIndex(0);
    itemNetStart6.SetTextIndex(1);
    itemNetStart7.SetTextIndex(1);
    menuNetStart.Add(&itemBloodQAV, 0);
}

void SetupSaveGameMenu(void)
{
    menuSaveGame.Add(&itemSaveTitle, 0);
    menuSaveGame.Add(&itemSaveGame1, 1);
    menuSaveGame.Add(&itemSaveGame2, 0);
    menuSaveGame.Add(&itemSaveGame3, 0);
    menuSaveGame.Add(&itemSaveGame4, 0);
    menuSaveGame.Add(&itemSaveGame5, 0);
    menuSaveGame.Add(&itemSaveGame6, 0);
    menuSaveGame.Add(&itemSaveGame7, 0);
    menuSaveGame.Add(&itemSaveGame8, 0);
    menuSaveGame.Add(&itemSaveGame9, 0);
    menuSaveGame.Add(&itemSaveGame10, 0);
    menuSaveGame.Add(&itemSaveGamePic, 0);
    menuSaveGame.Add(&itemBloodQAV, 0);

    itemSaveGame1.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[0], "<Empty>"))
        itemSaveGame1.at37 = 1;

    itemSaveGame2.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[1], "<Empty>"))
        itemSaveGame2.at37 = 1;

    itemSaveGame3.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[2], "<Empty>"))
        itemSaveGame3.at37 = 1;

    itemSaveGame4.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[3], "<Empty>"))
        itemSaveGame4.at37 = 1;

    itemSaveGame5.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[4], "<Empty>"))
        itemSaveGame5.at37 = 1;

    itemSaveGame6.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[5], "<Empty>"))
        itemSaveGame6.at37 = 1;

    itemSaveGame7.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[6], "<Empty>"))
        itemSaveGame7.at37 = 1;

    itemSaveGame8.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[7], "<Empty>"))
        itemSaveGame8.at37 = 1;

    itemSaveGame9.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[8], "<Empty>"))
        itemSaveGame9.at37 = 1;

    itemSaveGame10.at2c = &itemSaveGamePic;
    if (!strcmp(strRestoreGameStrings[9], "<Empty>"))
        itemSaveGame10.at37 = 1;
}

void SetupLoadGameMenu(void)
{
    menuLoadGame.Add(&itemLoadTitle, 0);
    menuLoadGame.Add(&itemLoadGame1, 1);
    menuLoadGame.Add(&itemLoadGame2, 0);
    menuLoadGame.Add(&itemLoadGame3, 0);
    menuLoadGame.Add(&itemLoadGame4, 0);
    menuLoadGame.Add(&itemLoadGame5, 0);
    menuLoadGame.Add(&itemLoadGame6, 0);
    menuLoadGame.Add(&itemLoadGame7, 0);
    menuLoadGame.Add(&itemLoadGame8, 0);
    menuLoadGame.Add(&itemLoadGame9, 0);
    menuLoadGame.Add(&itemLoadGame10, 0);
    menuLoadGame.Add(&itemLoadGamePic, 0);
    itemLoadGame1.at35 = 0;
    itemLoadGame2.at35 = 0;
    itemLoadGame3.at35 = 0;
    itemLoadGame4.at35 = 0;
    itemLoadGame5.at35 = 0;
    itemLoadGame6.at35 = 0;
    itemLoadGame7.at35 = 0;
    itemLoadGame8.at35 = 0;
    itemLoadGame9.at35 = 0;
    itemLoadGame10.at35 = 0;
    itemLoadGame1.at2c = &itemLoadGamePic;
    itemLoadGame2.at2c = &itemLoadGamePic;
    itemLoadGame3.at2c = &itemLoadGamePic;
    itemLoadGame4.at2c = &itemLoadGamePic;
    itemLoadGame5.at2c = &itemLoadGamePic;
    itemLoadGame6.at2c = &itemLoadGamePic;
    itemLoadGame7.at2c = &itemLoadGamePic;
    itemLoadGame8.at2c = &itemLoadGamePic;
    itemLoadGame9.at2c = &itemLoadGamePic;
    itemLoadGame10.at2c = &itemLoadGamePic;
    menuLoadGame.Add(&itemBloodQAV, 0);
}

void SetupSoundsMenu(void)
{
    itemSoundsMusic.at24 = ClipRange(MusicVolume, itemSoundsMusic.at28, itemSoundsMusic.at2c);
    itemSoundsSound.at24 = ClipRange(FXVolume, itemSoundsSound.at28, itemSoundsSound.at2c);
    itemSoundsCDAudio.at24 = ClipRange(Redbook.GetVolume(), itemSoundsCDAudio.at28, itemSoundsCDAudio.at2c);
    menuSounds.Add(&itemSoundsTitle, 0);
    menuSounds.Add(&itemSoundsMusic, 1);
    menuSounds.Add(&itemSoundsSound, 0);
    menuSounds.Add(&itemSoundsCDAudio, 0);
    menuSounds.Add(&itemSoundsDoppler, 0);
    menuSounds.Add(&itemBloodQAV, 0);
}

void SetupQuitMenu(void)
{
    menuQuit.Add(&itemQuitTitle, 0);
    menuQuit.Add(&itemQuitText1, 0);
    menuQuit.Add(&itemQuitYesNo, 1);
    menuQuit.Add(&itemBloodQAV, 0);
}

void SetupHelpOrderMenu(void)
{
    menuOrder.Add(&itemHelp4QAV, 1);
    menuOrder.Add(&itemHelp5QAV, 0);
    menuOrder.Add(&itemHelp3QAV, 0);
    menuOrder.Add(&itemHelp3BQAV, 0);
    itemHelp4QAV.at18 |= 10;
    itemHelp5QAV.at18 |= 10;
    itemHelp3QAV.at18 |= 10;
    itemHelp3BQAV.at18 |= 10;
}

void SetupCreditsMenu(void)
{
    menuCredits.Add(&itemCreditsQAV, 1);
    itemCreditsQAV.at18 |= 10;
}

void SetupParentalLockMenu(void)
{
    itemParentalLockToggle.at20 = gbAdultContent;
    strcpy(itemParentalLockPassword.at20, gzAdultPassword);
    menuParentalLock.Add(&itemParentalLockTitle, 0);
    menuParentalLock.Add(&itemParentalLockToggle, 1);
    menuParentalLock.Add(&itemParentalLockPassword, 0);
    menuParentalLock.Add(&itemBloodQAV, 0);
}

void SetupOnlineMenu(void)
{
    menuOnline.Add(&itemOnlineTitle, 0);
    menuOnline.Add(&itemOnline1, 1);
    menuOnline.Add(&itemOnline2, 0);
    menuOnline.Add(&itemOnline3, 0);
    menuOnline.Add(&itemOnline4, 0);
    menuOnline.Add(&itemOnline5, 0);
    menuOnline.Add(&itemOnline6, 0);
    menuOnline.Add(&itemBloodQAV, 0);
}

void SetupSorryMenu(void)
{
    menuSorry.Add(&itemSorryPicCycle, 1);
    menuSorry.Add(&itemSorryText1, 0);
    menuSorry.Add(&itemSorryText2, 0);
    menuSorry.Add(&itemSorryText3, 0);
    menuSorry.Add(&itemBloodQAV, 0);
}

void SetupSorry2Menu(void)
{
    menuSorry2.Add(&itemSorryPicCycle, 1);
    menuSorry2.Add(&itemSorry2Text1, 0);
    menuSorry2.Add(&itemSorry2Text2, 0);
    menuSorry2.Add(&itemSorry2Text3, 0);
    menuSorry2.Add(&itemBloodQAV, 0);
}

void SetupMenus(void)
{
    SetupLoadingScreen();
    SetupKeyListMenu();
    SetupMessagesMenu();
    SetupControlsMenu();
    SetupSaveGameMenu();
    SetupLoadGameMenu();
    SetupOptionsMenu();
    SetupCreditsMenu();
    SetupHelpOrderMenu();
    SetupSoundsMenu();
    SetupDifficultyMenu();
    SetupEpisodeMenu();
    SetupMainMenu();
    SetupMainMenuWithSave();
    SetupNetStartMenu();
    SetupQuitMenu();
    SetupParentalLockMenu();
    SetupSorryMenu();
    SetupSorry2Menu();
    SetupOnlineMenu();
    gQuickLoadSlot = -1;
    gQuickSaveSlot = -1;
}

void SetDoppler(CGameMenuItemZBool *pItem)
{
    gDoppler = pItem->at20;
}

void SetCrosshair(CGameMenuItemZBool *pItem)
{
    gAimReticle = pItem->at20;
}

void ResetKeys(CGameMenuItemChain *)
{
    CONFIG_ResetKeys();
}

void SetShowWeapons(CGameMenuItemZBool *pItem)
{
    gShowWeapon = pItem->at20;
}

void SetSlopeTilting(CGameMenuItemZBool *pItem)
{
    gSlopeTilting = pItem->at20;
}

void SetViewBobbing(CGameMenuItemZBool *pItem)
{
    gViewVBobbing = pItem->at20;
}

void SetViewSwaying(CGameMenuItemZBool *pItem)
{
    gViewHBobbing = pItem->at20;
}

void SetDetail(CGameMenuItemSlider *pItem)
{
    gDetail = pItem->at24;
}

void SetGamma(CGameMenuItemSlider *pItem)
{
    gGamma = pItem->at24;
    scrSetGamma(gGamma);
}

void SetMusicVol(CGameMenuItemSlider *pItem)
{
    sndSetMusicVolume(pItem->at24);
}

void SetSoundVol(CGameMenuItemSlider *pItem)
{
    sndSetFXVolume(pItem->at24);
}

void SetCDVol(CGameMenuItemSlider *pItem)
{
    if (gRedBookInstalled)
    {
        Redbook.SetVolume(pItem->at24);
        CDVolume = pItem->at24;
    }
}

void SetMessages(CGameMenuItemZBool *pItem)
{
    BOOL t = pItem->at20;
    gGameMessageMgr.SetState(t);
}

void SetMouseSensitivity(CGameMenuItemSlider *pItem)
{
    gMouseSensitivity = pItem->at24;
    CONTROL_SetMouseSensitivity(gMouseSensitivity);
}

void SetMouseAimFlipped(CGameMenuItemZBool *pItem)
{
    gMouseAimingFlipped = pItem->at20;
}

void SetTurnSpeed(CGameMenuItemSlider *pItem)
{
    gTurnSpeed = pItem->at24;
}

void SetDifficultyAndStart(CGameMenuItemChain *pItem)
{
    DIFFICULTY t = (DIFFICULTY)pItem->at30;
    gSkill = gGameOptions.nDifficulty = t;
    gGameOptions.nLevel = 0;
    BOOL s = gDemo.at1;
    if (s)
        gDemo.StopPlayback();
    gStartNewGame = 1;
    gCheatMgr.func_5BCF4();
    gGameMenuMgr.Deactivate();
}

void SetVideoMode(CGameMenuItemChain *pItem)
{
    int t = pItem->at30;
    if (t == validmodecnt)
    {
        ScreenMode = 2;
        ScreenWidth = 640;
        ScreenHeight = 480;
    }
    else
    {
        ScreenMode = 1;
        ScreenWidth = validmodexdim[t];
        ScreenHeight = validmodeydim[t];
    }
    scrSetGameMode(ScreenMode, ScreenWidth, ScreenHeight);
    scrSetDac();
    viewResizeView(gViewSize);
}

void SaveGameProcess(CGameMenuItemChain *)
{
}

extern "C" {
void tenBnSetCommandLine(void (*a1)(char *));
int tenBnStart(void);
};

void TenButtonCommand(char *);

void TenProcess(CGameMenuItemChain *)
{
    tenBnSetCommandLine(TenButtonCommand);
    int_148E0C = 0;
    int ret = tenBnStart();
    if (ret == 0 && int_148E0C)
    {
        gQuitGame = TRUE;
        gTenQuit = TRUE;
        gGameMenuMgr.Deactivate();
    }
    switch (ret)
    {
        case 0xedd:
            gGameMenuMgr.Deactivate();
            viewSetMessage("Could not find an installed version of TEN on this computer.");
            break;
        case 0xedb:
            gGameMenuMgr.Deactivate();
            viewSetMessage("The TEN setup files could not be found.");
            break;
        case 0xed9:
            gGameMenuMgr.Deactivate();
            viewSetMessage("You can only run TEN from Windows 95.");
            break;
    }
}

void func_5A164(void)
{
    char buffer[144];
    IniFile bloodIni("BlOOD.INI");
    if (!bloodIni.KeyExists("INSTALL", "SourcePath"))
        return;
    char *vs = bloodIni.GetKeyString("INSTALL", "SourcePath", NULL);
    strcpy(buffer, vs);
    strcpy(buffer, "heat01bl.exe");
    strcpy(int_148E10, buffer);
    char_148E29 = TRUE;
    gQuitGame = TRUE;
    gGameMenuMgr.Deactivate();
}

void SaveGame(CGameMenuItemZEditBitmap *pItem, CGameMenuEvent *event)
{
    char strSaveGameName[13] = "";
    int nSlot = pItem->at28;
    if (gGameOptions.nGameType > GAMETYPE_0 || gGameStarted == 0)
        return;
    switch (event->at0)
    {
        case 6:
            if (strSaveGameName[0])
                break;
            sprintf(strSaveGameName, "GAME00%02d.SAV", nSlot);
            strcpy(gGameOptions.szUserGameName, strRestoreGameStrings[nSlot]);
            sprintf(gGameOptions.szSaveGameName, strSaveGameName);
            gGameOptions.nSaveGameSlot = nSlot;
            if (!gSaveGamePic[nSlot])
                gSaveGamePic[nSlot] = (byte*)Resource::Alloc(0xfa00);
            func_1EC78(2518, "Saving", "Saving Your Game", strRestoreGameStrings[nSlot]);
            gSaveGameNum = nSlot;
            LoadSave::SaveGame(strSaveGameName);
            gQuickSaveSlot = nSlot;
            break;
    }
    gGameMenuMgr.Deactivate();
}

void QuickSaveGame(void)
{
    char strSaveGameName[13] = "";
    if (gGameOptions.nGameType > GAMETYPE_0 || gGameStarted == 0)
        return;
    if (strSaveGameName[0])
    {
        gGameMenuMgr.Deactivate();
        return;
    }
    sprintf(strSaveGameName, "GAME00%02d.SAV", gQuickSaveSlot);
    strcpy(gGameOptions.szUserGameName, strRestoreGameStrings[gQuickSaveSlot]);
    sprintf(gGameOptions.szSaveGameName, strSaveGameName);
    gGameOptions.nSaveGameSlot = gQuickSaveSlot;
    func_1EC78(2518, "Saving", "Saving Your Game", strRestoreGameStrings[gQuickSaveSlot]);
    LoadSave::SaveGame(strSaveGameName);
    gGameOptions.picEntry = gSavedOffset;
    memcpy(&gSaveGameOptions[gQuickSaveSlot], &gGameOptions, sizeof(GAMEOPTIONS));
    UpdateSavedInfo(gQuickSaveSlot);
    gGameMenuMgr.Deactivate();
}

void LoadGame(CGameMenuItemZEditBitmap *pItem, CGameMenuEvent *event)
{
    if (gGameOptions.nGameType > GAMETYPE_0)
        return;
    int nSlot = pItem->at28;
    char strLoadGameName[13] = "";
    sprintf(strLoadGameName, "GAME00%02d.SAV", nSlot);
    if (access(strLoadGameName, 4) == -1)
        return;
    BOOL t = gDemo.at1;
    if (t)
        gDemo.Close();
    func_1EC78(2518, "Loading", "Loading Saved Game", strRestoreGameStrings[nSlot]);
    LoadSave::LoadGame(strLoadGameName);
    gGameMenuMgr.Deactivate();
    gQuickLoadSlot = nSlot;
}

void QuickLoadGame(void)
{
    if (gGameOptions.nGameType > GAMETYPE_0)
        return;
    char strLoadGameName[13] = "";
    sprintf(strLoadGameName, "GAME00%02d.SAV", gQuickLoadSlot);
    if (access(strLoadGameName, 4) == -1)
        return;
    func_1EC78(2518, "Loading", "Loading Saved Game", strRestoreGameStrings[gQuickLoadSlot]);
    LoadSave::LoadGame(strLoadGameName);
    gGameMenuMgr.Deactivate();
}

void SetupNetLevels(CGameMenuItemZCycle *pItem)
{
    SetupLevelMenuItem(pItem->at24);
}

void StartNetGame(CGameMenuItemChain *pItem)
{
    gPacketStartGame.gameType = itemNetStart1.at24+1;
    if (gPacketStartGame.gameType == 0)
        gPacketStartGame.gameType = 2;
    gPacketStartGame.episodeId = itemNetStart2.at24;
    gPacketStartGame.levelId = itemNetStart3.at24;
    gPacketStartGame.difficulty = itemNetStart4.at24;
    gPacketStartGame.monsterSettings = itemNetStart5.at24;
    gPacketStartGame.weaponSettings = itemNetStart6.at24;
    gPacketStartGame.itemSettings = itemNetStart7.at24;
    gPacketStartGame.respawnSettings = 0;
    gPacketStartGame.unk = 0;
    gPacketStartGame.userMapName[0] = 0;
    strncpy(gPacketStartGame.userMapName, zUserMapName, 13);
    gPacketStartGame.userMapName[12] = 0;
    gPacketStartGame.userMap = gPacketStartGame.userMapName[0] != 0;

    netBroadcastNewGame();
    gStartNewGame = 1;
    gGameMenuMgr.Deactivate();
}

void Quit(CGameMenuItemChain *pItem)
{
    if (gGameOptions.nGameType == GAMETYPE_0 || numplayers == 1)
        gQuitGame = 1;
    else
        gQuitRequest = 1;
    gGameMenuMgr.Deactivate();
}

void SetParentalLock(CGameMenuItemZBool *pItem)
{
    BOOL t = pItem->at20;
    if (!t)
    {
        pItem->at20 = 1;
        pItem->Draw();
        if (strcmp(itemParentalLockPassword.at20, ""))
        {
            itemParentalLockPassword.pMenu->FocusNextItem();
            itemParentalLockPassword.at32 = 0;
            itemParentalLockPassword.at37 = 1;
            itemParentalLockPassword.at5f = pItem;
            itemParentalLockPassword.at29[0] = 0;
            return;
        }
        else
        {
            pItem->at20 = 0;
            pItem->Draw();
            gbAdultContent = 0;
        }
    }
    else
        gbAdultContent = 1;
    CONFIG_WriteAdultMode();
}


void MenuSetupEpisodeInfo(void)
{
    memset(zEpisodeNames, 0, sizeof(zEpisodeNames));
    memset(zLevelNames, 0, sizeof(zLevelNames));
    for (int i = 0; i < 6; i++)
    {
        if (i < gEpisodeCount)
        {
            EPISODEINFO *pEpisode = &gEpisodeInfo[i];
            zEpisodeNames[i] = pEpisode->at0;
            for (int j = 0; j < 16; j++)
            {
                if (j < pEpisode->nLevels)
                {
                    zLevelNames[i][j] = pEpisode->at28[j].at90;
                }
            }
        }
    }
}

void TenButtonCommand(char *a1)
{
    int_148E0C = a1;
}

void func_5A828(void)
{
    char buffer[80];
    if (gGameOptions.nGameType == GAMETYPE_0)
    {
        BOOL t = gDemo.at1;
        if (t)
            sprintf(buffer, "Loading Demo");
        else
            sprintf(buffer, "Loading Level");
    }
    else
        sprintf(buffer, "%s", zNetGameTypes[gGameOptions.nGameType-1]);
    func_1EC78(2049, buffer, levelGetTitle(), NULL);
}
