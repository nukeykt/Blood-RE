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
#ifndef _IOB_H_
#define _IOB_H_

#include "typedefs.h"

class IOBuffer
{
public:
    IOBuffer(int _nRemain, byte* _pBuffer) {
        nRemain = _nRemain;
        pBuffer = _pBuffer;
    }
    int nRemain;
    byte* pBuffer;
    void Read(void*, int);
    void Write(void*, int);
    void Skip(int);
};
#endif
