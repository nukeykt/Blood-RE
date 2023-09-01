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
#ifndef _CRC32_H_
#define _CRC32_H_
#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

ulong CRC32(void *data, ulong len);
#pragma aux CRC32 parm [esi][ecx] modify [eax ebx edx ecx esi]

#ifdef __cplusplus
}
#endif

#endif