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
#ifndef _WARP_H_

#include "typedefs.h"

struct ZONE {
	int x, y, z;
	short sectnum, ang;
};

extern ZONE gStartZone[];

int CheckLink(SPRITE *pSprite);
int CheckLink(long *x, long *y, long *z, int *nSector);
void warpInit(void);

#endif
