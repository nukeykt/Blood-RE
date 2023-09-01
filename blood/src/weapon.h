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
#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "typedefs.h"
#include "player.h"

void WeaponDraw(PLAYER *,int,int,int,int);

void func_51340(SPRITE *, int);

void WeaponInit(void);

void WeaponRaise(PLAYER *);

void WeaponLower(PLAYER *);

byte WeaponUpgrade(PLAYER *, BOOL);

void WeaponProcess(PLAYER*);

#endif // !_WEAPON_H_
