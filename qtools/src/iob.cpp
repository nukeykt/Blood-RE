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
#include <string.h>
#include "typedefs.h"
#include "iob.h"
#include "error.h"


void IOBuffer::Read(void* pData, int nSize)
{
    if (nSize <= nRemain)
    {
        memcpy(pData, pBuffer, nSize);
        nRemain -= nSize;
        pBuffer += nSize;
    }
    else
    {
        ThrowError(23)("Read buffer overflow");
    }
}

void IOBuffer::Write(void* pData, int nSize)
{
    if (nSize <= nRemain)
    {
        memcpy(pBuffer, pData, nSize);
        nRemain -= nSize;
        pBuffer += nSize;
    }
    else
    {
        ThrowError(35)("Write buffer overflow");
    }
}

void IOBuffer::Skip(int nSize)
{
    if (nSize <= nRemain)
    {
        nRemain -= nSize;
        pBuffer += nSize;
    }
    else
    {
        ThrowError(46)("Skip overflow");
    }
}
