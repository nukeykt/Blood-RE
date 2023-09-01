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
#ifndef _QHEAP_H_
#define _QHEAP_H_

#include "typedefs.h"

struct HEAPNODE
{
    HEAPNODE* prev;
    HEAPNODE* next;
    int size;
    BOOL isFree;
    HEAPNODE* freePrev;
    HEAPNODE* freeNext;
};

class QHeap
{
public:
    QHeap(int heapSize);
    ~QHeap(void);

    void Check(void);
    void Debug(void);
    void *Alloc(int);
    int Free(void *p);

    void *heapPtr;
    HEAPNODE heap;
    HEAPNODE freeHeap;
    int size;
};

#endif
