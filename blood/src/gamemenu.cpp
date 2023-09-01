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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "typedefs.h"
#include "types.h"
#include "build.h"
#include "control.h"
#include "config.h"
#include "debug4g.h"
#include "error.h"
#include "gamemenu.h"
#include "globals.h"
#include "gui.h"
#include "key.h"
#include "keyboard.h"
#include "levels.h"
#include "menu.h"
#include "misc.h"
#include "qav.h"
#include "resource.h"
#include "view.h"

BOOL CGameMenuMgr::m_bInitialized = FALSE;
BOOL CGameMenuMgr::m_bActive = FALSE;

CGameMenuMgr gGameMenuMgr;
CMenuTextMgr gMenuTextMgr;

static char buffer[21][45];

CMenuTextMgr::CMenuTextMgr()
{
    at0 = -1;
}

CMenuTextMgr::~CMenuTextMgr()
{
    at0 = -1;
}

void CMenuTextMgr::DrawText(char *pString, int nFont, int x, int y, int nShade, int nPalette, BOOL shadow )
{
    viewDrawText(nFont, pString, x, y, nShade, nPalette, 0, shadow);
}

void CMenuTextMgr::GetFontInfo(int nFont, char *pString, int *pXSize, int *pYSize)
{
    if (nFont < 0 || nFont >= 5)
        return;
    viewGetFontInfo(nFont, pString, pXSize, pYSize);
}

CGameMenuMgr::CGameMenuMgr()
{
    dassert(!m_bInitialized, 90);
    m_bInitialized = TRUE;
    Clear();
}

CGameMenuMgr::~CGameMenuMgr()
{
    m_bInitialized = FALSE;
    Clear();
}

void CGameMenuMgr::InitializeMenu(void)
{
    if (pActiveMenu)
    {
        CGameMenuEvent event;
        event.at0 = 0x8000;
        event.at2 = 0;
        pActiveMenu->Event(event);
    }
}

void CGameMenuMgr::func_7DF1C(void)
{
    if (pActiveMenu)
    {
        CGameMenuEvent event;
        event.at0 = 0x8001;
        event.at2 = 0;
        pActiveMenu->Event(event);
    }
}

BOOL CGameMenuMgr::Push(CGameMenu *pMenu, int nItem)
{
    dassert(pMenu != NULL, 137);
    if (nMenuPointer == 8)
        return FALSE;
    pActiveMenu = pMenu;
    pMenuStack[nMenuPointer] = pMenu;
    nMenuPointer++;
    if (nItem >= 0)
        pMenu->SetFocusItem(nItem);
    m_bActive = TRUE;
    gInputMode = INPUT_MODE_1;
    InitializeMenu();
    return TRUE;
}

void CGameMenuMgr::Pop(void)
{
    if (nMenuPointer > 0)
    {
        func_7DF1C();
        nMenuPointer--;
        if (nMenuPointer == 0)
            Deactivate();
        else
            pActiveMenu = pMenuStack[nMenuPointer-1];
    }
}

void CGameMenuMgr::Draw(void)
{
    if (pActiveMenu)
    {
        pActiveMenu->Draw();
        viewUpdatePages();
    }
}

void CGameMenuMgr::Clear(void)
{
    pActiveMenu = NULL;
    nMenuPointer = 0;
    memset(pMenuStack, 0, sizeof(pMenuStack));
}

void CGameMenuMgr::Process(void)
{
    if (!pActiveMenu)
        return;
    CGameMenuEvent event;
    event.at0 = 0;
    event.at2 = 0;
    BYTE key = keyGet();
    if ( key != 0 )
    {
        keyFlushStream();
        event.at2 = key;
        switch (key)
        {
        case bsc_Esc:
            event.at0 = 7;
            break;
        case bsc_Tab:
            if (keystatus[bsc_LShift] || keystatus[bsc_RShift])
                event.at0 = 2;
            else
                event.at0 = 3;
            break;
        case bsc_Up:
        case bsc_Pad_8:
            event.at0 = 2;
            break;
        case bsc_Down:
        case bsc_Pad_2:
            event.at0 = 3;
            break;
        case bsc_Enter:
        case bsc_Pad_Enter:
            event.at0 = 6;
            break;
        case bsc_SpaceBar:
            event.at0 = 8;
            break;
        case bsc_Left:
        case bsc_Pad_4:
            event.at0 = 4;
            break;
        case bsc_Right:
        case bsc_Pad_6:
            event.at0 = 5;
            break;
        case bsc_Del:
        case bsc_Pad_Period:
            event.at0 = 10;
            break;
        case bsc_Backspace:
            event.at0 = 9;
            break;
        default:
            event.at0 = 1;
            break;
        }
    }
    if (pActiveMenu->Event(event))
        Pop();
}

void CGameMenuMgr::Deactivate(void)
{
    Clear();
    keyFlushStream();
    m_bActive = FALSE;

    gInputMode = INPUT_MODE_0;
}

CGameMenu::CGameMenu()
{
    m_nItems = 0;
    m_nFocus = at8 = -1;
    atc = 0;
}

CGameMenu::CGameMenu(int unk)
{
    m_nItems = 0;
    m_nFocus = at8 = -1;
    atc = unk;
}

CGameMenu::~CGameMenu()
{
    if (!atc)
        return;
    for (int i = 0; i < m_nItems; i++)
    {
        if (pItemList[i] != &itemBloodQAV)
            delete pItemList[i];
        pItemList[i] = NULL;
    }
}

void CGameMenu::InitializeItems(CGameMenuEvent &event)
{
    for (int i = 0; i < m_nItems; i++)
    {
        pItemList[i]->Event(event);
    }
}

void CGameMenu::Draw(void)
{
    for (int i = 0; i < m_nItems; i++)
    {
        if (i == m_nFocus || (i != m_nFocus && !pItemList[i]->Can3()))
            pItemList[i]->Draw();
    }
}

BOOL CGameMenu::Event(CGameMenuEvent &event)
{
    BOOL ret = 1;
    if (m_nItems <= 0)
        return TRUE;
    switch (event.at0)
    {
    case 0x8000:
    case 0x8001:
        if (at8 >= 0)
            m_nFocus = at8;
        InitializeItems(event);
        return FALSE;
    }
    if (m_nFocus >= 0)
        ret = pItemList[m_nFocus]->Event(event);
    return ret;
}

void CGameMenu::Add(CGameMenuItem *pItem, BOOL active)
{
    dassert(pItem != NULL, 390);
    dassert(m_nItems < kMaxGameMenuItems, 391);
    pItemList[m_nItems] = pItem;
    pItem->pMenu = this;
    if (active)
        m_nFocus = at8 = m_nItems;
    m_nItems++;
}

void CGameMenu::SetFocusItem(int nItem)
{
    dassert(nItem >= 0 && nItem < m_nItems && nItem < kMaxGameMenuItems, 408);
    if (CanSelectItem(nItem))
        m_nFocus = at8 = nItem;
}

BOOL CGameMenu::CanSelectItem(int nItem)
{
    dassert(nItem >= 0 && nItem < m_nItems && nItem < kMaxGameMenuItems, 418);
    CGameMenuItem *pItem = pItemList[nItem];
    if (pItem->CanShow() && pItem->CanFocus())
        return 1;
    return 0;
}

void CGameMenu::FocusPrevItem(void)
{
    dassert(m_nFocus >= -1 && m_nFocus < m_nItems && m_nFocus < kMaxGameMenuItems, 432);
    int t = m_nFocus;
    do
    {
        m_nFocus = DecRotate(m_nFocus, m_nItems);
        if (CanSelectItem(m_nFocus))
            break;
    } while(t != m_nFocus);
}

void CGameMenu::FocusNextItem(void)
{
    dassert(m_nFocus >= -1 && m_nFocus < m_nItems && m_nFocus < kMaxGameMenuItems, 443);
    int t = m_nFocus;
    do
    {
        m_nFocus = IncRotate(m_nFocus, m_nItems);
        if (CanSelectItem(m_nFocus))
            break;
    } while(t != m_nFocus);
}

BOOL CGameMenu::IsFocusItem(CGameMenuItem *pItem)
{
    if (m_nFocus < 0)
        return FALSE;
    dassert(m_nFocus >= 0 && m_nFocus < m_nItems && m_nFocus < kMaxGameMenuItems, 457);
    return (pItemList[m_nFocus] == pItem) ? 1 : 0;
}

CGameMenuItem::CGameMenuItem()
{
    func_81910(NULL);
    //func_81900(0, 0, 0);
    atc = at10 = at14 = 0;
    Set0();
    Set1();
    Clear3();
    func_818F0(-1);
    func_81920(NULL);
}

BOOL CGameMenuItem::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 7:
        r = 1;
        break;
    case 2:
        pMenu->FocusPrevItem();
        break;
    case 3:
        pMenu->FocusNextItem();
        break;
    }
    return r;
}

CGameMenuItemText::CGameMenuItemText()
{
    at4 = 0;
    Clear1();
}

CGameMenuItemText::CGameMenuItemText(char *a1, int a2, int a3, int a4, int a5)
{
    //at14 = 0;
    at4 = a1;
    at8 = a2;
    func_81900(a3, a4, 0);
    //atc = a3;
    //at10 = a4;
    SetVal(a5);
    Clear1();
}

void CGameMenuItemText::Draw(void)
{
    if (at4)
    {
        int width2;
        int width1;
        int x = atc;
        int s = -128;
        switch (at20)
        {
        case 0:
            x = atc;
            break;
        case 1:
            gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
            x = atc-width1/2;
            break;
        case 2:
            gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
            x = atc-width2;
            break;
        }
        gMenuTextMgr.DrawText(at4,at8, x, at10, s, 0, FALSE);
    }
}

BOOL CGameMenuItemText::Event(CGameMenuEvent &event)
{
    return CGameMenuItem::Event(event);
}

CGameMenuItemTitle::CGameMenuItemTitle()
{
    at4 = 0;
    Clear1();
}

CGameMenuItemTitle::CGameMenuItemTitle(char *a1, int a2, int a3, int a4, int a5)
{
    //at14 = 0;
    at4 = a1;
    at8 = a2;
    func_81900(a3, a4, 0);
    //atc = a3;
    //at10 = a4;
    SetVal(a5);
    Clear1();
}

void CGameMenuItemTitle::Draw(void)
{
    int v4 = -128;
    if (at4)
    {
        int height;
        gMenuTextMgr.GetFontInfo(at8, NULL, NULL, &height);
        rotatesprite(320<<15, at10<<16, 65536, 0, at20, v4, 0, 78, 0, 0, xdim-1, ydim-1);
        viewDrawText(at8, at4, atc, at10-height/2, v4, 0, 1, FALSE);
    }
}

BOOL CGameMenuItemTitle::Event(CGameMenuEvent &event)
{
    return CGameMenuItem::Event(event);
}

static char *CGameMenuItemZBool::m_pzOnDefault = "On";
static char *CGameMenuItemZBool::m_pzOffDefault = "Off";

CGameMenuItemZBool::CGameMenuItemZBool()
{
    at20 = FALSE;
    at4 = 0;
    at21 = m_pzOnDefault;
    at25 = m_pzOffDefault;
}

CGameMenuItemZBool::CGameMenuItemZBool(char *a1, int a2, int a3, int a4, int a5, BOOL a6, void(*a7)(CGameMenuItemZBool *), char *a8, char *a9)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a6;
    at29 = a7;
    at21 = !a8 ? m_pzOnDefault : a8;
    at25 = !a9 ? m_pzOffDefault : a9;
}

void CGameMenuItemZBool::Draw(void)
{
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int vc = atc;
    int v4 = at10;
    if (at4)
        gMenuTextMgr.DrawText(at4, at8, vc, v4, shade, 0, FALSE);
    char *value = at20 ? at21 : at25;
    int width;
    gMenuTextMgr.GetFontInfo(at8, value, &width, NULL);
    vc = at14 - 1 + atc - width;
    gMenuTextMgr.DrawText(value, at8, vc, v4, shade, 0, FALSE);
}

BOOL CGameMenuItemZBool::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 6:
    case 8:
        at20 = !at20;
        if (at29)
            at29(this);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemChain::CGameMenuItemChain()
{
    at4 = NULL;
    at24 = NULL;
    at28 = -1;
    at2c = NULL;
    at30 = 0;
}

CGameMenuItemChain::CGameMenuItemChain(char *a1, int a2, int a3, int a4, int a5, int a6, CGameMenu *a7, int a8, void(*a9)(CGameMenuItemChain *), int a10)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a6;
    at24 = a7;
    at28 = a8;
    at2c = a9;
    at30 = a10;
}

void CGameMenuItemChain::Draw(void)
{
    if (!at4) return;
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int width2;
    int width1;
    int x = atc;
    switch (at20)
    {
    case 0:
        x = atc;
        break;
    case 1:
        gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
        x = atc+at14/2-width1/2;
        break;
    case 2:
        gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
        x = atc+(at14-1)-width2;
        break;
    default:
        break;
    }
    gMenuTextMgr.DrawText(at4, at8, x, at10, shade, 0, TRUE);
}
BOOL CGameMenuItemChain::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 6:
        if (at2c)
            at2c(this);
        if (at24)
            gGameMenuMgr.Push(at24, at28);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItem7EA1C::CGameMenuItem7EA1C()
{
    at4 = NULL;
    at24 = NULL;
    at28 = -1;
    at2c = NULL;
    at30 = 0;
    at34 = NULL;
    at38[0] = 0;
    at48[0] = 0;
}

CGameMenuItem7EA1C::CGameMenuItem7EA1C(char *a1, int a2, int a3, int a4, int a5, char *a6, char *a7, int a8, int a9, void(*a10)(CGameMenuItem7EA1C *), int a11)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a8;
    at28 = a9;
    at2c = a10;
    at30 = a11;
    strncpy(at38, a6, 15);
    strncpy(at48, a7, 15);
}

void CGameMenuItem7EA1C::Draw(void)
{
    if (!at4) return;
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int width2;
    int width1;
    int x = atc;
    switch (at20)
    {
    case 0:
        x = atc;
        break;
    case 1:
        gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
        x = atc+at14/2-width1/2;
        break;
    case 2:
        gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
        x = atc+(at14-1)-width2;
        break;
    default:
        break;
    }
    gMenuTextMgr.DrawText(at4, at8, x, at10, shade, 0, TRUE);
}

void CGameMenuItem7EA1C::Setup(void)
{
    if (!at34 || !at24)
        return;
    if (!at34->SectionExists(at48))
        return;
    char *title = at34->GetKeyString(at48, "Title", at48);
    CGameMenuItemTitle *pItem = new CGameMenuItemTitle(title, 1, 160, 20, 2038);
    at24->Add(pItem, FALSE);
    at24->Add(&itemSorryPicCycle, TRUE);
    int y = 40;
    for (int i = 0; i < 21; i++)
    {
        sprintf(buffer[i], "Line%ld", i+1);
        if (!at34->KeyExists(at48, buffer[i]))
            break;
        char *line = at34->GetKeyString(at48, buffer[i], NULL);
        if (line)
        {
            if (!*line)
            {
                y += 10;
                continue;
            }
            CGameMenuItemText *pItem = new CGameMenuItemText(line, 1, 160, y, 1);
            at24->Add(pItem, FALSE);
            y += 20;
        }
    }
    at24->Add(&itemBloodQAV, FALSE);
}

BOOL CGameMenuItem7EA1C::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 6:
    {
        if (at2c)
            at2c(this);
        if (at24)
            delete at24;
        at24 = new CGameMenu(1);
        DICTNODE *pRes = gGuiRes.Lookup(at38, "MNU");
        if (pRes)
        {
            at34 = new IniFile(gGuiRes.Load(pRes), 1);
            Setup();
        }
        if (at24)
            gGameMenuMgr.Push(at24, at28);
        break;
    }
    case 0x8001:
        if (at34)
        {
            delete at34;
            at34 = NULL;
        }
        if (at24)
        {
            delete at24;
            at24 = NULL;
        }
        break;
    default:
        r =  CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItem7EE34::CGameMenuItem7EE34()
{
    at4 = NULL;
    at28 = NULL;
    at20 = -1;
    at2c = NULL;
}

CGameMenuItem7EE34::CGameMenuItem7EE34(char *a1, int a2, int a3, int a4, int a5, int a6)
{
    at4 = NULL;
    at28 = NULL;
    at20 = -1;
    at2c = NULL;
    at8 = a2;
    atc = a3;
    at4 = a1;
    at10 = a4;
    at14 = a5;
    at24 = a6;
}

void CGameMenuItem7EE34::Draw(void)
{
    if (!at4) return;
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int width2;
    int width1;
    int x = atc;
    switch (at24)
    {
    case 0:
        x = atc;
        break;
    case 1:
        gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
        x = atc+at14/2-width1/2;
        break;
    case 2:
        gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
        x = atc+(at14-1)-width2;
        break;
    default:
        break;
    }
    gMenuTextMgr.DrawText(at4, at8, x, at10, shade, 0, TRUE);
}

void CGameMenuItem7EE34::Setup(void)
{
    if (!at28)
        return;
    CGameMenuItemTitle *pTitle = new CGameMenuItemTitle("Video Mode", 1, 160, 20, 2038);
    at28->Add(pTitle, FALSE);
    if (!at2c)
    {
        at2c = new CGameMenu(1);
        pTitle = new CGameMenuItemTitle(" Mode Change ", 1, 160, 20, 2038);
        at2c->Add(pTitle, FALSE);
        at2c->Add(&itemSorryPicCycle, TRUE);
        CGameMenuItem *pItem1 = new CGameMenuItemText("VIDEO MODE WAS SET", 1, 160, 90, 1);
        CGameMenuItem *pItem2 = new CGameMenuItemText("NOT ALL MODES Work correctly", 1, 160, 110, 1);
        CGameMenuItem *pItem3 = new CGameMenuItemText("Press ESC to exit", 3, 160, 140, 1);
        at2c->Add(pItem1, FALSE);
        pItem1->Clear1();
        at2c->Add(pItem2, FALSE);
        pItem1->Clear1();
        at2c->Add(pItem3, TRUE);
        pItem3->Set1();
        at2c->Add(&itemBloodQAV, FALSE);
    }
    getvalidvesamodes();
    sprintf(buffer[0], "320 x 200 (default)");
    int y = 40;
    CGameMenuItemChain *pChain = new CGameMenuItemChain(buffer[0], 3, 0, y, 320, 1, at2c, -1, SetVideoMode, validmodecnt);
    at28->Add(pChain, TRUE);
    y += 20;
    for (int i = 0; i < validmodecnt && i < 20; i++)
    {
        sprintf(buffer[i+1], "%d x %d", validmodexdim[i], validmodeydim[i]);
        pChain = new CGameMenuItemChain(buffer[i+1], 3, 0, y, 320, 1, at2c, -1, SetVideoMode, i);
        at28->Add(pChain, FALSE);
        if (validmodecnt > 10)
            y += 7;
        else
            y += 15;
    }
    at28->Add(&itemBloodQAV, FALSE);
}

BOOL CGameMenuItem7EE34::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 6:
        if (at28)
            delete at28;
        at28 = new CGameMenu(1);
        Setup();
        if (at28)
            gGameMenuMgr.Push(at28, at20);
        break;
    case 0x8001:
        if (at28)
        {
            delete at28;
            at28 = 0;
        }
        if (at2c)
        {
            delete at2c;
            at2c = 0;
        }
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemChain7F2F0::CGameMenuItemChain7F2F0()
{
    at34 = -1;
}

CGameMenuItemChain7F2F0::CGameMenuItemChain7F2F0(char *a1, int a2, int a3, int a4, int a5, int a6, CGameMenu *a7, int a8, void(*a9)(CGameMenuItemChain *), int a10, int a11) :
    CGameMenuItemChain(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)
{
    at34 = a11;
}

void CGameMenuItemChain7F2F0::Draw(void)
{
    CGameMenuItemChain::Draw();
}

BOOL CGameMenuItemChain7F2F0::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 6:
        if (at34 > -1)
            gGameOptions.nEpisode = at34;
        CGameMenuItemChain::Event(event);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemBitmap::CGameMenuItemBitmap()
{
    at4 = NULL;
}

CGameMenuItemBitmap::CGameMenuItemBitmap(char *a1, int a2, int a3, int a4, int a5)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at20 = a5;
}

void CGameMenuItemBitmap::Draw(void)
{
    int shade = 32;
    if (CanFocus() && pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int x = atc;
    int y = at10;
    if (at4)
    {
        int height;
        gMenuTextMgr.DrawText(at4, at8, x, y, shade, 0, FALSE);
        gMenuTextMgr.GetFontInfo(at8, NULL, NULL, &height);
        y += height + 2;
    }
    rotatesprite(x<<15,y<<15, 65536, 0, at20, 0, 0, 82, 0, 0, xdim-1,ydim-1);
}

BOOL CGameMenuItemBitmap::Event(CGameMenuEvent &event)
{
    if (CanFocus() && pMenu->IsFocusItem(this))
        pMenu->FocusNextItem();
    return CGameMenuItem::Event(event);
}

CGameMenuItemBitmapLS::CGameMenuItemBitmapLS()
{
    at4 = NULL;
}

CGameMenuItemBitmapLS::CGameMenuItemBitmapLS(char *a1, int a2, int a3, int a4, int a5)
{
    at24 = -1;
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at28 = a5;
}

void CGameMenuItemBitmapLS::Draw(void)
{
    int x, y;

    int shade;

    int picnum;

    short ang;
    byte stat;



    shade = 32;
    if (CanFocus() && pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    x = atc;
    y = at10;
    if (at4)
    {
        int height;
        gMenuTextMgr.DrawText(at4, at8, x, y, shade, 0, FALSE);
        gMenuTextMgr.GetFontInfo(at8, NULL, NULL, &height);
        y += height + 2;
    }


    if (at24 == -1)
    {
        picnum = at28;
        ang = 0;
        stat = 66;
    }
    else
    {
        picnum = at24;
        ang = 512;
        stat = 70;
    }
    rotatesprite(200<<15,215<<15, 32768, ang, picnum, 0, 0, stat, 0, 0, xdim-1, ydim-1);
}

BOOL CGameMenuItemBitmapLS::Event(CGameMenuEvent &event)
{
    if (CanFocus() && pMenu->IsFocusItem(this))
        pMenu->FocusNextItem();
    return CGameMenuItem::Event(event);
}

CGameMenuItemKeyList::CGameMenuItemKeyList()
{
    at4 = NULL;
    at8 = 3;
    atc = 0;
    at10 = 0;
    at28 = 0;
    at2c = 0;
    at30 = 0;
    at34 = 0;
    at38 = FALSE;
}

CGameMenuItemKeyList::CGameMenuItemKeyList(char *a1, int a2, int a3, int a4, int a5, int a6, int a7, void(*a8)(CGameMenuItemKeyList *))
{
    at2c = 0;
    at30 = 0;
    at38 = FALSE;
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at28 = a6;
    at20 = a8;
    at34 = a7;
}

void CGameMenuItemKeyList::Scan(void)
{
    KB_FlushKeyboardQueue();
    KB_ClearKeysDown();
    KB_ClearLastScanCode();
    at38 = TRUE;
}
void CGameMenuItemKeyList::Draw(void)
{
    char buffer[40], buffer2[40];
    int height;
    int y;
    int k;
    int i;

    gMenuTextMgr.GetFontInfo(at8, NULL, NULL, &height);
    i = 0;
    y = at10;
    k = at30 - at2c;
    for (; i < at28; i++)
    {
        int32 key1, key2;
        CONTROL_GetKeyMap(k, &key1, &key2);
        char *sKey1 = KB_ScanCodeToString(key1);
        char *sKey2 = KB_ScanCodeToString(key2);
        sprintf(buffer, "%s", CONFIG_FunctionNumToName(k));
        if (key2 == 0)
        {
            if (key1 == 0)
                sprintf(buffer2, "????");
            else
                sprintf(buffer2, "%s", sKey1);
        }
        else
            sprintf(buffer2, "%s or %s\0", sKey1, sKey2);
        
        if (k == at30)
        {
            int shade;
            char* sVal;
            int width1;
            shade = 32;
            if (pMenu->IsFocusItem(this))
                shade = 32 - (totalclock&63);
            viewDrawText(3, buffer, atc, y, shade, 0, 0, FALSE);
            if (at38 && (gGameClock & 32))
                sVal = "____";
            else
                sVal = buffer2;
            gMenuTextMgr.GetFontInfo(at8, sVal, &width1, 0);
            int x = atc+(at14-1)-width1;
            viewDrawText(at8, sVal, x, y, shade, 0, 0, FALSE);
        }
        else
        {
            int width2;
            viewDrawText(3, buffer, atc, y, 24, 0, 0, FALSE);
            gMenuTextMgr.GetFontInfo(at8, buffer2, &width2, 0);
            int x = atc+(at14-1)-width2;
            viewDrawText(3, buffer2, x, y, 24, 0, 0, FALSE);
        }

        k++;
        y += height;
    }
}

BOOL CGameMenuItemKeyList::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    if (at38)
    {
        if (KB_GetLastScanCode() != 0 && KB_GetLastScanCode() != sc_Pause)
        {
            if (KB_KeyWaiting())
                KB_Getch();
            int32 k = KB_GetLastScanCode();
            int32 key1, key2;
            CONTROL_GetKeyMap(at30, &key1, &key2);
            if (key1 > 0 && key2 != k)
                key2 = key1;
            key1 = k;
            if (key1 == key2)
                key2 = 0;
            CONTROL_MapKey(at30, key1, key2);
            KB_FlushKeyboardQueue();
            KB_ClearKeysDown();
            keyFlushStream();
            at38 = 0;
        }
        return FALSE;
    }
    switch (event.at0)
    {
    case 2:
        if (event.at2 == bsc_Tab || at30 == 0)
        {
            pMenu->FocusPrevItem();
            break;
        }
        at30--;
        if (at2c > 0)
            at2c--;
        break;
    case 3:
        if (event.at2 == bsc_Tab || at30 == at34-1)
        {
            pMenu->FocusNextItem();
            break;
        }
        at30++;
        if (at2c+1 < at28)
            at2c++;
        break;
    case 6:
        if (at20)
            at20(this);
        Scan();
        break;
    case 10:
        if (keystatus[bsc_LCtrl] || keystatus[bsc_RCtrl])
        {
            CONTROL_MapKey(at30, 0, 0);
        }
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemSlider::CGameMenuItemSlider()
{
    at4 = NULL;
    at8 = -1;
    atc = 0;
    at10 = 0;
    at24 = 0;
    at28 = 0;
    at2c = 0;
    at30 = 0;
    at34 = NULL;
    at20 = NULL;
    at38 = 2204;
    at3c = 2028;
}

CGameMenuItemSlider::CGameMenuItemSlider(char *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, void(*a10)(CGameMenuItemSlider *), int a11, int a12)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at28 = a7;
    at2c = a8;
    at30 = a9;
    SetValue(a6);
    at34 = a10;
    at38 = 2204;
    at3c = 2028;
    if (a11 >= 0)
        at38 = a11;
    if (a12 >= 0)
        at3c = a12;
}

CGameMenuItemSlider::CGameMenuItemSlider(char *a1, int a2, int a3, int a4, int a5, int *pnValue, int a7, int a8, int a9, void(*a10)(CGameMenuItemSlider *), int a11, int a12)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at28 = a7;
    at2c = a8;
    at30 = a9;
    dassert(pnValue != NULL, 1703);
    at20 = pnValue;
    SetValue(*pnValue);
    at34 = a10;
    at38 = 2204;
    at3c = 2028;
    if (a11 >= 0)
        at38 = a11;
    if (a12 >= 0)
        at3c = a12;
}

void CGameMenuItemSlider::Draw(void)
{
    int x;
    int height;
    int shade;
    int sliderX;
    int nRange;
    int nValue;
    int nWidth;
    int cursorX;
    at24 = !at20 ? at24 : *at20;
    gMenuTextMgr.GetFontInfo(at8, NULL, NULL, &height);
    shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    sliderX = atc;
    if (at4 != NULL)
        gMenuTextMgr.DrawText(at4, at8, sliderX, at10, shade, 0, FALSE);
    sliderX = atc+(at14-1)-tilesizx[at38]/2;
    rotatesprite(sliderX<<16, (at10+height/2)<<16, 65536, 0, at38, 0, 0, 10, 0, 0, xdim-1, ydim-1);
    nRange = at2c - at28;
    dassert(nRange > 0, 1744);
    nValue = at24 - at28;
    nWidth = tilesizx[at38]-8;
    cursorX = sliderX + ksgn(at30)*(nValue * nWidth / nRange - nWidth / 2);
    rotatesprite(cursorX<<16, (at10+height/2)<<16, 65536, 0, at3c, 0, 0, 10, 0, 0, xdim-1, ydim-1);
}

BOOL CGameMenuItemSlider::Event(CGameMenuEvent &event)
{
    at24 = !at20 ? at24 : *at20;
    BOOL r = 0;
    switch (event.at0)
    {
    case 2:
        pMenu->FocusPrevItem();
        break;
    case 3:
        pMenu->FocusNextItem();
        break;
    case 4:
        if (at24 > 0)
            at24 = DecBy(at24, at30);
        else
            at24 = IncBy(at24, -at30);
        at24 = ClipRange(at24, at28, at2c);
        if (at34)
            at34(this);
        break;
    case 5:
        if (at24 >= 0)
            at24 = IncBy(at24, at30);
        else
            at24 = DecBy(at24, -at30);
        at24 = ClipRange(at24, at28, at2c);
        if (at34)
            at34(this);
        break;
    case 6:
        if (at34)
            at34(this);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemZEdit::CGameMenuItemZEdit()
{
    at4 = NULL;
    at8 = -1;
    atc = 0;
    at10 = 0;
    at20 = NULL;
    at24 = 0;
    at32 = 0;
    at2c = 0;
    at30 = 0;
    at28 = 0;
    at31 = 1;
}

CGameMenuItemZEdit::CGameMenuItemZEdit(char *a1, int a2, int a3, int a4, int a5, char *a6, int a7, BOOL a8, void(*a9)(CGameMenuItemZEdit *, CGameMenuEvent *), int a10)
{
    at30 = 0;
    at31 = 1;
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a6;
    at24 = a7;
    at32 = a8;
    at2c = a9;
    at28 = a10;
}

void CGameMenuItemZEdit::AddChar(char ch)
{
    int i = strlen(at20);
    if (i + 1 < at24)
    {
        at20[i++] = ch;
        at20[i] = 0;
    }
}

void CGameMenuItemZEdit::BackChar(void)
{
    int i = strlen(at20);
    if (i > 0)
        at20[--i] = 0;
}

void CGameMenuItemZEdit::Draw(void)
{
    int width1, width;
    gMenuTextMgr.GetFontInfo(at8, NULL, &width1, NULL);
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    if (at30)
        shade = -128;
    if (at4)
        gMenuTextMgr.DrawText(at4, at8, atc, at10, shade, 0, FALSE);
    int x = (atc+(at14-1))-(at24+1)*width1;
    if (at20 && *at20)
    {
        gMenuTextMgr.GetFontInfo(at8, at20, &width, NULL);
        int shade2;
        if (at32)
        {
            shade2 = at30 ? -128 : shade;
        }
        else
        {
            shade2 = at30 ? shade : 32;
        }
        gMenuTextMgr.DrawText(at20, at8, x, at10, shade2, 0, FALSE);
        x += width;
    }
    if (at30 && (gGameClock & 32))
        gMenuTextMgr.DrawText("_", at8, x, at10, shade, 0, FALSE);
}

BOOL CGameMenuItemZEdit::Event(CGameMenuEvent &event)
{
    static char buffer[256] = "";
    BOOL r = 0;
    switch (event.at0)
    {
    case 7:
        if (at30)
        {
            strncpy(at20, buffer, at24);
            *(at20 + at24 - 1) = 0;
            at30 = 0;
            break;
        }
        r = 1;
        break;
    case 6:
        if (!at31)
        {
            if (at2c)
                at2c(this, &event);
            break;
        }
        if (at30)
        {
            if (at2c)
                at2c(this, &event);
            at30 = 0;
            break;
        }
        strncpy(buffer, at20, at24);
        buffer[at24-1] = 0;
        at30 = 1;
        break;
    case 9:
        if (at30)
            BackChar();
        break;
    case 1:
    case 8:
    {
        BYTE key;
        if (keystatus[bsc_LShift] || keystatus[bsc_RShift])
            key = ScanToAsciiShifted[event.at2];
        else
            key = ScanToAscii[event.at2];
        if (at30 && (isalnum(key) || isspace(key) || ispunct(key)))
        {
            AddChar(key);
            break;
        }
        r = CGameMenuItem::Event(event);
        break;
    }
    case 2:
        if (at30)
            break;
        r = CGameMenuItem::Event(event);
        break;
    case 3:
        if (at30)
            break;
        r = CGameMenuItem::Event(event);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemZEditBitmap::CGameMenuItemZEditBitmap()
{
    at4 = NULL;
    at8 = -1;
    atc = 0;
    at10 = 0;
    at20 = NULL;
    at24 = 0;
    at36 = 0;
    at30 = NULL;
    at2c = NULL;
    at34 = 0;
    at28 = 0;
    at37 = 0;
    at35 = 1;
}

CGameMenuItemZEditBitmap::CGameMenuItemZEditBitmap(char *a1, int a2, int a3, int a4, int a5, char *a6, int a7, char a8, void(*a9)(CGameMenuItemZEditBitmap *, CGameMenuEvent *), int a10)
{
    at2c = NULL;
    at34 = 0;
    at35 = 1;
    at37 = 0;
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a6;
    at24 = a7;
    at36 = a8;
    at30 = a9;
    at28 = a10;
}

void CGameMenuItemZEditBitmap::AddChar(char ch)
{
    int i = strlen(at20);
    if (i + 1 < at24)
    {
        at20[i++] = ch;
        at20[i] = 0;
    }
}

void CGameMenuItemZEditBitmap::BackChar(void)
{
    int i = strlen(at20);
    if (i > 0)
        at20[--i] = 0;
}

void CGameMenuItemZEditBitmap::Draw(void)
{
    int width1, width;
    gMenuTextMgr.GetFontInfo(at8, NULL, &width1, NULL);
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    at2c->at24 = -1;
    if (at34)
        shade = -128;
    if (at4)
        gMenuTextMgr.DrawText(at4, at8, atc, at10, shade, 0, FALSE);
    int x = atc+(at14-1)-(at24+1)*width1;
    if (at20 && *at20)
    {
        gMenuTextMgr.GetFontInfo(at8, at20, &width, NULL);
        int shade2;
        if (at36)
        {
            shade2 = at34 ? -128 : shade;
        }
        else
        {
            shade2 = at34 ? shade : 32;
        }
        gMenuTextMgr.DrawText(at20, at8, x, at10, shade2, 0, FALSE);
        x += width;
    }
    if (at34 && (gGameClock & 32))
        gMenuTextMgr.DrawText("_", at8, x, at10, shade, 0, FALSE);
}

BOOL CGameMenuItemZEditBitmap::Event(CGameMenuEvent &event)
{
    static char buffer[256] = "";
    BOOL r;
    byte key;
    r = 0;
    switch (event.at0)
    {
    case 7:
        if (at34)
        {
            strncpy(at20, buffer, at24);
            *(at20+at24-1) = 0;
            at34 = 0;
            gSaveGameActive = FALSE;
        }
        else
        {
            gSaveGameActive = FALSE;
            r = 1;
        }
        break;
    case 6:
        if (!at35)
        {
            if (at30)
                at30(this, &event);
            gSaveGameActive = FALSE;
        }
        else if (at34)
        {
            if (at30)
                at30(this, &event);
            at34 = 0;
            gSaveGameActive = FALSE;
        }
        else
        {
            strncpy(buffer, at20, at24);
            char t = at37;
            if (t)
                *at20 = 0;
            buffer[at24-1] = 0;
            at34 = 1;
        }
        break;
    case 9:
        if (!at34)
            break;
        BackChar();
        break;
    case 1:
    case 8:
    {
        if (keystatus[bsc_LShift] || keystatus[bsc_RShift])
            key = ScanToAsciiShifted[event.at2];
        else
            key = ScanToAscii[event.at2];
        if (at34 && (isalnum(key) || isspace(key) || ispunct(key)))
        {
            AddChar(key);
            break;
        }
        r = CGameMenuItem::Event(event);
        break;
    }
    case 2:
        if (at34)
            break;
        r = CGameMenuItem::Event(event);
        break;
    case 3:
        if (at34)
            break;
        r = CGameMenuItem::Event(event);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemQAV::CGameMenuItemQAV()
{
    at20 = NULL;
    at24 = NULL;
    at28 = 0;
    Clear1();
}

CGameMenuItemQAV::CGameMenuItemQAV(char *a1, int a2, int a3, int a4, char *a5)
{
    at14 = 0;
    at4 = a1;
    at8 = a2;
    at10 = a4;
    at20 = a5;
    atc = a3;
    Clear1();
}

void CGameMenuItemQAV::Draw(void)
{
    int backFC;
    int nTicks;
    int t1, t2;
    int wx1, wy1, wx2, wy2;
    if (!at24)
        return;
    backFC = gFrameClock;
    gFrameClock = gGameClock;
    nTicks = totalclock - at30;
    at30 = totalclock;
    at2c -= nTicks;
    if (at2c <= 0 || at2c > at28->at10)
    {
        at2c = at28->at10;
    }
    t2 = at28->at10 - at2c;
    t1 = t2 - nTicks;
    at28->Play(t1, t2, -1, NULL);
    wx1 = windowx1;
    wy1 = windowy1;
    wx2 = windowx2;
    wy2 = windowy2;

    windowx1 = 0;
    windowy1 = 0;
    windowx2 = xdim-1;
    windowy2 = ydim-1;
    at28->Draw(t2, 10, 0, 0);

    windowx1 = wx1;
    windowy1 = wy1;
    windowx2 = wx2;
    windowy2 = wy2;
    gFrameClock = backFC;
}

BOOL CGameMenuItemQAV::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 4:
    case 9:
        pMenu->FocusPrevItem();
        break;
    case 5:
        pMenu->FocusNextItem();
        break;
    case 6:
    case 8:
        pMenu->FocusNextItem();
        break;
    case 0x8000:
        if (at20)
        {
            if (!at28)
            {
                at24 = gSysRes.Lookup(at20, "QAV");
                if (!at24)
                    ThrowError(2336)("Could not load QAV %s\n", at20);
                at28 = (QAV*)gSysRes.Lock(at24);
                at28->x = atc;
                at28->y = at10;
                at28->Preload();
                at2c = at28->at10;
                at30 = totalclock;
                break;
            }
            gSysRes.Lock(at24);
        }
        break;
    case 0x8001:
        if (at20 && at28)
        {
            gSysRes.Unlock(at24);
            if (at24->lockCount == 0)
                at28 = NULL;
        }
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

void CGameMenuItemQAV::Reset(void)
{
    at2c = at28->at10;
    at30 = totalclock;
}

CGameMenuItemZCycle::CGameMenuItemZCycle()
{
    at4 = NULL;
    at24 = 0;
    m_nItems = 0;
    atb4 = 0;
    at2c = 0;
    at30 = 0;
}

CGameMenuItemZCycle::CGameMenuItemZCycle(char *a1, int a2, int a3, int a4, int a5, int a6, void(*a7)(CGameMenuItemZCycle *), char **a8, int a9, int a10)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at24 = 0;
    at14 = a5;
    at28 = a6;
    atb4 = a7;
    m_nItems = 0;
    SetTextArray(a8, a9, a10);
}

CGameMenuItemZCycle::~CGameMenuItemZCycle()
{
    at4 = NULL;
    at24 = 0;
    m_nItems = 0;
    atb4 = 0;
    memset(at34, 0, sizeof(at34));
    at2c = 0;
    at30 = 0;
}

void CGameMenuItemZCycle::Draw(void)
{
    int width3;
    int width2;
    int width1;
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int x = atc;
    int y = at10;
    if (at4)
    {
        switch (at28)
        {
        case 0:
            x = atc;
            break;
        case 1:
            gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
            x = atc+at14/2-width1/2;
            break;
        case 2:
            gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
            x = atc+(at14-1)-width2;
            break;
        default:
            break;
        }
        gMenuTextMgr.DrawText(at4, at8, x, y, shade, 0, FALSE);
    }
    char *pzText = !m_nItems ? "????" : at34[at24];
    dassert(pzText != NULL, 2463);
    gMenuTextMgr.GetFontInfo(at8, pzText, &width3, NULL);
    x = atc + (at14 - 1) - width3;
    gMenuTextMgr.DrawText(pzText, at8, x, y, shade, 0, FALSE);
}

BOOL CGameMenuItemZCycle::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 5:
    case 6:
    case 8:
        Next();
        if (atb4)
            atb4(this);
        break;
    case 4:
        Prev();
        if (atb4)
            atb4(this);
        break;
    case 7:
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

void CGameMenuItemZCycle::Add(char *pItem, BOOL active)
{
    dassert(pItem != NULL, 2504);
    dassert(m_nItems < kMaxGameCycleItems, 2505);
    at34[m_nItems] = pItem;
    if (active)
        at24 = m_nItems;
    m_nItems++;
}

void CGameMenuItemZCycle::Next(void)
{
    if (m_nItems > 0)
    {
        at24 = IncRotate(at24, m_nItems);
    }
}

void CGameMenuItemZCycle::Prev(void)
{
    if (m_nItems > 0)
    {
        at24 = DecRotate(at24, m_nItems);
    }
}

void CGameMenuItemZCycle::Clear(void)
{
    m_nItems = at24 = 0;
    memset(at34, 0, sizeof(at34));
    at2c = 0;
    at30 = 0;
}

void CGameMenuItemZCycle::SetTextArray(char **pTextArray, int nTextPtrCount, int nIndex)
{
    Clear();
    at30 = pTextArray;
    at2c = nTextPtrCount;
    dassert(nTextPtrCount <= kMaxGameCycleItems, 2547);
    for (int i = 0; i < nTextPtrCount; i++)
        Add(pTextArray[i], FALSE);
    SetTextIndex(nIndex);
}

void CGameMenuItemZCycle::SetTextIndex(int nIndex)
{
    at24 = ClipRange(nIndex, 0, m_nItems);
}

CGameMenuItemYesNoQuit::CGameMenuItemYesNoQuit()
{
    at4 = NULL;
    at24 = -1;
    at28 = 0;
}

CGameMenuItemYesNoQuit::CGameMenuItemYesNoQuit(char *a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
    at14 = a5;
    at20 = a6;
    at24 = a7;
    at28 = a8;
}

void CGameMenuItemYesNoQuit::Draw(void)
{
    if (!at4) return;
    int shade = 32;
    if (pMenu->IsFocusItem(this))
        shade = 32-(totalclock&63);
    int width2;
    int width1;
    int x = atc;
    switch (at20)
    {
    case 0:
        x = atc;
        break;
    case 1:
        gMenuTextMgr.GetFontInfo(at8, at4, &width1, NULL);
        x = atc+at14/2-width1/2;
        break;
    case 2:
        gMenuTextMgr.GetFontInfo(at8, at4, &width2, NULL);
        x = atc+(at14-1)-width2;
        break;
    default:
        break;
    }
    gMenuTextMgr.DrawText(at4, at8, x, at10, shade, 0, TRUE);
}

BOOL CGameMenuItemYesNoQuit::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 1:
        if (event.at2 == bsc_Y)
            Quit(NULL);
        else if (event.at2 == bsc_N)
            gGameMenuMgr.Pop();
        break;
    case 6:
        Quit(NULL);
        break;
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

CGameMenuItemPicCycle::CGameMenuItemPicCycle()
{
    at4 = NULL;
    at24 = 0;
    m_nItems = 0;
    atb0 = 0;
    at2c = 0;
    atb4 = 0;
}

CGameMenuItemPicCycle::CGameMenuItemPicCycle(int a1, int a2, void(*a3)(CGameMenuItemPicCycle *), int *a4, int a5, int a6)
{
    at14 = 0;
    at24 = 0;
    m_nItems = 0;
    atc = a1;
    at10 = a2;
    atb0 = a3;
    atb4 = 0;
    SetPicArray(a4, a5, a6);
}

void CGameMenuItemPicCycle::Draw(void)
{
    setview(0, 0, xdim - 1, ydim - 1);
    if (atb4)
        rotatesprite(0, 0, 65536, 0, atb4, 0, 0, 82, 0, 0, xdim - 1, ydim - 1);
    if (at30[at24])
        rotatesprite(0, 0, 65536, 0, at30[at24], 0, 0, 82, 0, 0, xdim - 1, ydim - 1);
}

BOOL CGameMenuItemPicCycle::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (event.at0)
    {
    case 5:
    case 6:
    case 8:
        Next();
        if (atb0)
            atb0(this);
        break;
    case 4:
        Prev();
        if (atb0)
            atb0(this);
        break;
    case 7:
    default:
        r = CGameMenuItem::Event(event);
        break;
    }
    return r;
}

void CGameMenuItemPicCycle::Add(int nItem, BOOL active)
{
    dassert(m_nItems < kMaxPicCycleItems, 2736);
    at30[m_nItems] = nItem;
    if (active)
        at24 = m_nItems;
    m_nItems++;
}

void CGameMenuItemPicCycle::Next(void)
{
    if (m_nItems > 0)
    {
        at24 = IncRotate(at24, m_nItems);
    }
}

void CGameMenuItemPicCycle::Prev(void)
{
    if (m_nItems > 0)
    {
        at24 = DecRotate(at24, m_nItems);
    }
}

void CGameMenuItemPicCycle::Clear(void)
{
    m_nItems = at24 = 0;
    memset(at30, 0, sizeof(at30));
    at2c = 0;
}

void CGameMenuItemPicCycle::SetPicArray(int *pArray, int nTileCount, int nIndex)
{
    Clear();
    at2c = nTileCount;
    dassert(nTileCount <= kMaxPicCycleItems, 2776);
    for (int i = 0; i < nTileCount; i++)
        Add(pArray[i], FALSE);
    SetPicIndex(nIndex);
}

void CGameMenuItemPicCycle::SetPicIndex(int nIndex)
{
    at24 = ClipRange(nIndex, 0, m_nItems);
}

CGameMenuItemPassword::CGameMenuItemPassword()
{
    at37 = 0;
    at4 = NULL;
    at36 = 0;
    at32 = 0;
    at5b = 0;
}

CGameMenuItemPassword::CGameMenuItemPassword(char *a1, int a2, int a3, int a4)
{
    at37 = 0;
    at14 = 0;
    at36 = 0;
    at32 = 0;
    at5b = 0;
    at4 = a1;
    at8 = a2;
    atc = a3;
    at10 = a4;
}

char *kCheckPasswordMsg = "ENTER PASSWORD: ";
char *kOldPasswordMsg = "ENTER OLD PASSWORD: ";
char *kNewPasswordMsg = "ENTER NEW PASSWORD: ";
char *kInvalidPasswordMsg = "INVALID PASSWORD.";

void CGameMenuItemPassword::Draw(void)
{
    BOOL focus = pMenu->IsFocusItem(this);
    int shade = 32;
    int shadef = 32-(totalclock&63);
    int width;
    int x;
    int i;
    switch (at37)
    {
    case 1:
    case 2:
    case 3:
        switch (at37)
        {
        case 1:
            strcpy(at3b, kCheckPasswordMsg);
            break;
        case 2:
            strcpy(at3b, kOldPasswordMsg);
            break;
        case 3:
            strcpy(at3b, kNewPasswordMsg);
            break;
        }
        for (i = 0; i < at32; i++)
            strcat(at3b, "*");
        strcat(at3b, "_");
        gMenuTextMgr.GetFontInfo(at8, at3b, &width, NULL);
        x = atc - width / 2;
        gMenuTextMgr.DrawText(at3b, at8, x, at10+20, shadef, 0, FALSE);
        shadef = 32;
        break;
    case 4:
        if ((totalclock - at5b) & 32)
        {
            gMenuTextMgr.GetFontInfo(at8, kInvalidPasswordMsg, &width, NULL);
            x = atc - width / 2;
            gMenuTextMgr.DrawText(kInvalidPasswordMsg, at8, x, at10 + 20, shade, 0, FALSE);
        }
        if (at5b && totalclock-at5b > 256)
        {
            at5b = 0;
            at37 = 0;
        }
        break;
    }
    gMenuTextMgr.GetFontInfo(at8, at4, &width, NULL);
    x = atc - width / 2;
    gMenuTextMgr.DrawText(at4, at8, x, at10, focus ? shadef : shade, 0, FALSE);
}

enum {
    kPasswordState0 = 0,
    kPasswordState1 = 1,
    kPasswordState2 = 2,
    kPasswordState3 = 3,
    kPasswordState4 = 4,
};

BOOL CGameMenuItemPassword::Event(CGameMenuEvent &event)
{
    BOOL r = 0;
    switch (at37)
    {
    case kPasswordState0:
    case kPasswordState4:
        if (event.at0 == (ushort)kMenuEvent6)
        {
            at29[0] = 0;
            at32 = 0;
            if (strcmp(at20, ""))
                at37 = 2;
            else
                at37 = 3;
        }
        else
            r = CGameMenuItem::Event(event);
        break;
    case kPasswordState1:
    case kPasswordState2:
    case kPasswordState3:
        switch (event.at0)
        {
        case 6:
            switch (at37)
            {
            case 1:
                at36 = strcmp(at20,at29) == 0;
                at37 = at36 ? kPasswordState0 : kPasswordState4;
                if (!at36)
                {
                    at5b = totalclock;
                    pMenu->FocusPrevItem();
                }
                else
                {
                    at5f->at20 = 0;
                    at5f->Draw();
                    gbAdultContent = FALSE;
                    CONFIG_WriteAdultMode();
                    pMenu->FocusPrevItem();
                }
                break;
            case 2:
                at36 = strcmp(at20,at29) == 0;
                at37 = at36 ? kPasswordState0 : kPasswordState4;
                if (at36)
                {
                    strcpy(at20, "");
                    strcpy(gzAdultPassword, "");
                    CONFIG_WriteAdultMode();
                    at37 = 0;
                }
                else
                    at5b = totalclock;
                break;
            case 3:
                strcpy(at20, at29);
                strcpy(gzAdultPassword, at29);
                CONFIG_WriteAdultMode();
                at37 = 0;
                break;
            }
            break;
        case 7:
            at37 = 0;
            Draw();
            break;
        case 1:
            if (at32 < 8)
            {
                byte key = ScanToAsciiShifted[event.at2];
                if (isalnum(key) || isspace(key) || ispunct(key))
                {
                    at29[at32++] = key;
                    at29[at32] = 0;
                }
            }
            break;
        case 9:
            if (at32 > 0)
                at29[--at32] = 0;
            break;
        case 4:
        case 5:
        case 8:
            break;
        default:
            r = CGameMenuItem::Event(event);
            break;
        }
        break;
    }
    return r;
}
