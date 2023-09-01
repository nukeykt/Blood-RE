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
#ifndef _GETOPT_H_
#define _GETOPT_H_

#include "typedefs.h"

extern char *OptArgv[16];
extern int OptArgc;
extern char *OptFull;

struct SWITCH {
    const char *name;
    int at4, at8;
};

int GetOptions(SWITCH *switches);

#endif
