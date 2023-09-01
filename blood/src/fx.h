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
#ifndef _FX_H_
#define _FX_H_

#include "typedefs.h"
#include "build.h"

enum FX_ID {
    FX_NONE = -1,
    FX_0 = 0,
    FX_1,
    FX_2,
    FX_3,
    FX_4,
    FX_5,
    FX_6,
    FX_7,
    FX_8,
    FX_9,
    FX_10,
    FX_11,
    FX_12,
    FX_13,
    FX_14,
    FX_15,
    FX_16,
    FX_17,
    FX_18,
    FX_19,
    FX_20,
    FX_21,
    FX_22,
    FX_23,
    FX_24,
    FX_25,
    FX_26,
    FX_27,
    FX_28,
    FX_29,
    FX_30,
    FX_31,
    FX_32,
    FX_33,
    FX_34,
    FX_35,
    FX_36,
    FX_37,
    FX_38,
    FX_39,
    FX_40,
    FX_41,
    FX_42,
    FX_43,
    FX_44,
    FX_45,
    FX_46,
    FX_47,
    FX_48,
    FX_49,
    FX_50,
    FX_51,
    FX_52,
    FX_53,
    FX_54,
    FX_55,
    FX_56,
    kFXMax
};

class CFX {
public:
    void func_73FB0(int);
    void func_73FFC(int);
    void fxKill(int);
    void fxFree(int);
    SPRITE * fxSpawn(FX_ID, int, int, int, int, unsigned int duration = 0);
    void fxProcess(void);
};

extern CFX gFX;

void fxSpawnBlood(SPRITE *pSprite, int a2);
void func_746D4(SPRITE *, int);
void func_74818(SPRITE *, int, int, int);
void func_74A18(SPRITE *, int, int, int);

#endif
