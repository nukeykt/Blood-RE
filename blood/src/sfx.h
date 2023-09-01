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
#ifndef _SFX_H_
#define _SFX_H_
#include "typedefs.h"
#include "build.h"

void sfxPlay3DSound(SPRITE *, int, int a3 = -1, int a4 = 0);
void sfxPlay3DSound(int x, int y, int z, int soundId, int nSector);
void sfxKillAllSounds(void);
void sfxKill3DSound(SPRITE *pSprite, int a2 = -1, int a3 = -1);
void sfxSetReverb(BOOL);
void sfxSetReverb2(BOOL);
void sfxUpdate3DSounds(void);
void sfxInit(void);

#endif
