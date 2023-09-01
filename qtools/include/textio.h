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
#ifndef _TEXTIO_H_
#define _TEXTIO_H_

#include "typedefs.h"

extern int tioScreenRows;
extern int tioScreenCols;

void tioInit(int a1);
void tioCursorOff(void);
byte tioSetAttribute(byte);
void tioCenterString(int a1, int a2, int a3, char* a4, byte a5);
void tioWindow(int, int, int, int);
void tioClearWindow(void);
void tioTerm(void);
void tioPrint(char *s, ...);

#endif
