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
#ifndef _LOADSAVE_H_
#define _LOADSAVE_H_

#include "typedefs.h"
#include "levels.h"

extern GAMEOPTIONS gSaveGameOptions[10];
extern unsigned int gSavedOffset;
extern byte *gSaveGamePic[10];

void UpdateSavedInfo(int);

class LoadSave {
public:
    static LoadSave head;
    static int hFile;
    LoadSave *prev;
    LoadSave *next;
    LoadSave() {
        prev = head.prev;
        prev->next = this;
        next = &head;
        next->prev = this;
    }
    LoadSave(int dummy)
    {
#if 0
        next = prev = &head;
        head.next = head.prev = this;
#endif
        next = prev = this;
    }
    //~LoadSave() { }
    virtual void Load(void);
    virtual void Save(void);
    void Read(void *, int);
    void Write(void *, int);
    static void LoadGame(char *);
    static void SaveGame(char *);
};

void LoadSavedInfo(void);

#endif
