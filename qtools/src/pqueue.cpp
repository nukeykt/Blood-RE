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
#include "debug4g.h"
#include "pqueue.h"

PriorityQueue::PriorityQueue()
{
    fNodeCount = 0;
}

void PriorityQueue::Upheap(void)
{
    uint x = fNodeCount;
    queueItem item = queueItems[fNodeCount];
    queueItems[0].at0 = 0;
    while (item.at0 < queueItems[x >> 1].at0)
    {
        queueItems[x] = queueItems[x >> 1];
        x >>= 1;
    }
    queueItems[x] = item;
}

void PriorityQueue::Downheap(uint n)
{
    queueItem item = queueItems[n];
    while (n <= fNodeCount / 2)
    {
        uint t = n * 2;
        if (t < fNodeCount && queueItems[t].at0 > queueItems[t + 1].at0)
            t++;
        if (item.at0 <= queueItems[t].at0)
            break;
        queueItems[n] = queueItems[t];
        n = t;
    }
    queueItems[n] = item;
}

void PriorityQueue::Delete(uint k)
{
    dassert(k <= fNodeCount, 75);
    queueItems[k] = queueItems[fNodeCount--];
    Downheap(k);
}

void PriorityQueue::Insert(ulong a1, ulong a2)
{
    dassert(fNodeCount < kPQueueSize, 84);
    fNodeCount++;
    queueItems[fNodeCount].at0 = a1;
    queueItems[fNodeCount].at4 = a2;
    Upheap();
}

ulong PriorityQueue::Remove(void)
{
    ulong data = queueItems[1].at4;
    queueItems[1] = queueItems[fNodeCount--];
    Downheap(1);
    return data;
}
