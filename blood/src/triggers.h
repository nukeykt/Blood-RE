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
#ifndef _TRIGGERS_H_
#define _TRIGGERS_H_

#include "typedefs.h"
#include "db.h"
#include "eventq.h"

void trTextOver(int);
void trMessageSprite(unsigned int, EVENT);
void trMessageSector(unsigned int, EVENT);
void trMessageWall(unsigned int, EVENT);

void trTriggerSprite(unsigned int, XSPRITE*, int);
void trTriggerSector(unsigned int, XSECTOR*, int);
void trTriggerWall(unsigned int, XWALL*, int);

void trInit(void);

void trProcessBusy(void);

#endif
