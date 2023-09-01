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
#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "typedefs.h"

typedef struct {
    unsigned char red, green, blue;
} RGB;

extern RGB curDAC[256];
extern RGB baseDAC[256];

extern byte gStdColor[32];
extern BOOL gFogMode;

extern int gGammaLevels;

void scrSetGamma(int nGamma);
void scrNextPage(void);
void scrInit(void);
void scrCreateStdColors(void);
void scrSetGameMode(int vidMode, int XRes, int YRes);
void scrSetMessage(char *pMessage);
void scrDisplayMessage(int unk);
void scrSetupFade(byte red, byte green, byte blue);
void scrFadeAmount(int amount);
void scrSetupUnfade(void);
void scrSetDac(void);
void scrSetPalette(int palId);
void scrLoadPLUs(void);

#endif // !_SCREEN_H_
