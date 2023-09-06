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
#ifndef _ENDGAME_H_
#define _ENDGAME_H_

#include "typedefs.h"
#include "build.h"
#include "globals.h"

class CEndGameMgr {
public:
    char at0;
    INPUT_MODE at1;
    CEndGameMgr();
    void Setup(void);
    void ProcessKeys(void);
    void Draw(void);
    void Finish(void);
    BOOL Active(void) { return at0; }
};

class CSecretMgr {
public:
    int at0, at4, at8;
    CSecretMgr();
    void Found(int);
    void Clear(void);
    void Draw(void);
    void SetCount(int);
};

class CKillMgr {
public:
    int at0, at4;
    CKillMgr();
    void SetCount(int);
    void func_263E0(int);
    void AddKill(SPRITE *pSprite);
    void CountTotalKills(void);
	void Clear(void);
    void Draw(void);
};

extern CEndGameMgr gEndGameMgr;
extern CSecretMgr gSecretMgr;
extern CKillMgr gKillMgr;

#endif // !_ENDGAME_H_
