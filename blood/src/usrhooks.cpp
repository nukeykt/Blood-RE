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
#include "typedefs.h"
extern "C" {
#include "usrhooks.h"
}
#include "resource.h"

int USRHOOKS_GetMem(void **ptr, unsigned long size)
{
    void *mem = Resource::Alloc(size);
    if (!mem)
        return -1;

    *ptr = mem;
    return 0;
}

int USRHOOKS_FreeMem(void *ptr)
{
    if (!ptr)
        return -1;
    Resource::Free(ptr);
    return 0;
}
