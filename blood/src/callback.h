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
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

enum CALLBACK_ID {
    CALLBACK_ID_NONE = -1,
    CALLBACK_ID_0 = 0,
    CALLBACK_ID_1,
    CALLBACK_ID_2,
    CALLBACK_ID_3,
    CALLBACK_ID_4,
    CALLBACK_ID_5,
    CALLBACK_ID_6,
    CALLBACK_ID_7,
    CALLBACK_ID_8,
    CALLBACK_ID_9,
    CALLBACK_ID_10,
    CALLBACK_ID_11,
    CALLBACK_ID_12,
    CALLBACK_ID_13,
    CALLBACK_ID_14,
    CALLBACK_ID_15,
    CALLBACK_ID_16,
    CALLBACK_ID_17,
    CALLBACK_ID_18,
    CALLBACK_ID_19,
    CALLBACK_ID_20,
    CALLBACK_ID_21,
    kCallbackMax
};

extern void (*gCallback[kCallbackMax])(int);

#endif
