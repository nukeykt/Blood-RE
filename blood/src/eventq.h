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
#ifndef _EVENTQ_H_
#define _EVENTQ_H_

#include "callback.h"

#define kMaxChannels 4096

enum COMMAND_ID {
    COMMAND_ID_0 = 0,
    COMMAND_ID_1,
    COMMAND_ID_2,
    COMMAND_ID_3,
    COMMAND_ID_4,
    COMMAND_ID_5,

    kCommandCallback = 20,
    COMMAND_ID_21,

    COMMAND_ID_64 = 64,
};

struct EVENT {
    unsigned int at0_0 : 13; // index
    unsigned int at1_5 : 3; // type
    unsigned int at2_0 : 8; // cmd
    unsigned int funcID : 8; // callback
}; // <= 4 bytes

void evInit(void);
void evPost(int nIndex, int nType, unsigned long nDelta, COMMAND_ID command);
void evPost(int nIndex, int nType, unsigned long nDelta, CALLBACK_ID callback);
void evSend(int nIndex, int nType, int rxId, COMMAND_ID command);
void evKill(int a1, int a2);
void evKill(int a1, int a2, CALLBACK_ID a3);
void evProcess(ulong);

#endif // !_EVENTQ_H_
