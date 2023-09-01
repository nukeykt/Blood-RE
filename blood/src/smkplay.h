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
#ifndef _SMKPLAY_H_
#define _SMKPLAY_H_



class CSMKPlayer {
public:
    CSMKPlayer();
    ~CSMKPlayer();

    int PlaySMK(char*);
    int PlaySMKWithWAV(char*, int);
    int PlaySMKWithWAV(char*, char*);

    int f_0;
    int f_4;

    char __f_8[4];

    BOOL f_c;
};

#endif // !_SMKPLAY_H_
