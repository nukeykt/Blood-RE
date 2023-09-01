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
#ifndef _MENU_H_

#include "typedefs.h"
#include "gamemenu.h"

extern char strRestoreGameStrings[][16];

void MenuSetupEpisodeInfo(void);
void SetVideoMode(CGameMenuItemChain *pItem);
void Quit(CGameMenuItemChain *pItem);;

extern CGameMenuItemPicCycle itemSorryPicCycle;
extern CGameMenuItemQAV itemBloodQAV;
extern CGameMenu menuMain;
extern CGameMenu menuMainWithSave;
extern CGameMenu menuOrder;
extern CGameMenu menuSaveGame;
extern CGameMenu menuLoadGame;
extern CGameMenu menuSounds;
extern CGameMenu menuOptions;
extern CGameMenu menuQuit;
extern CGameMenu menuCredits;
extern CGameMenu menuLoading;

extern short gQuickLoadSlot;
extern short gQuickSaveSlot;

void QuickLoadGame(void);
void QuickSaveGame(void);
void SetupMenus(void);

void func_5A828(void);

#endif
