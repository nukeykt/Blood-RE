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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "typedefs.h"
#include "debug4g.h"
#include "error.h"
#include "qheap.h"

void InstallFenceposts(HEAPNODE *n)
{
    memset((byte*)n + 0x10, 0xcc, 0x10);
    memset((byte*)n + n->size - 0x10, 0xcc, 0x10);
}

void CheckFenceposts(HEAPNODE *n)
{
    int i;
    byte *data = (byte*)n + 0x10;
    for (i = 0; i < 0x10; i++, data++)
    {
        if (*data != 0xcc)
        {
            ThrowError(51)("Block underwritten");
        }
    }
    data = (byte*)n + n->size - 0x10;
    for (i = 0; i < 0x10; i++, data++)
    {
        if (*data != 0xcc)
        {
            ThrowError(58)("Block overwritten");
        }
    }
}

QHeap::QHeap(int heapSize)
{
    dassert(heapSize > 0, 67);
    size = heapSize;
    void *p = malloc(0x200000);
    while (size > 0 && (heapPtr = malloc(size)) == NULL)
    {
        size -= 0x1000;
    }
    free(p);
    if (!heapPtr)
    {
        ThrowError(91)("Allocation failure\n");
    }
    heap.isFree = FALSE;
    freeHeap.isFree = FALSE;
    HEAPNODE *node = (HEAPNODE*)(((int)heapPtr + 0xf) & ~0xf);
    heap.next = heap.prev = node;
    node->next = node->prev = &heap;
    freeHeap.freeNext = freeHeap.freePrev = node;
    node->freeNext = node->freePrev = &freeHeap;
    node->size = size & ~0xf;
    node->isFree = TRUE;
}

QHeap::~QHeap(void)
{
    Check();
    free(heapPtr);
    heapPtr = NULL;
}

void QHeap::Check(void)
{
    HEAPNODE *node = heap.next;
    while (node != &heap)
    {
        if (!node->isFree)
        {
            CheckFenceposts(node);
        }
        node = node->next;
    }
}

void QHeap::Debug(void)
{
    char s[4];
    FILE *f = fopen("MEMFRAG.TXT", "wt");
    if (!f)
    {
        return;
    }
    HEAPNODE *node = heap.next;
    while (node != &heap)
    {
        if (node->isFree)
        {
            fprintf(f, "%P %10d FREE", node, node->size);
        }
        else
        {
            byte *data = (byte*)node + 0x20;
            byte t0, t1, t2, t3;
            if (isalpha(data[0]))
                t0 = data[0];
            else
                t0 = '_';
            s[0] = t0;
            if (isalpha(data[1]))
                t1 = data[1];
            else
                t1 = '_';
            s[1] = t1;
            if (isalpha(data[2]))
                t2 = data[2];
            else
                t2 = '_';
            s[2] = t2;
            if (isalpha(data[3]))
                t3 = data[3];
            else
                t3 = '_';
            s[3] = t3;
            fprintf(f, "%P %10d %4s", node, node->size, s);
        }
        node = node->next;
    }
    fclose(f);
}

void *QHeap::Alloc(int blockSize)
{
    dassert(blockSize > 0, 184);
    dassert(heapPtr != NULL, 185);
    if (blockSize > 0)
    {
        blockSize = ((blockSize + 0xf) & ~0xf) + 0x30;
        HEAPNODE *freeNode = freeHeap.freeNext;
        while (freeNode != &freeHeap)
        {
            dassert(freeNode->isFree, 199);
            if (blockSize <= freeNode->size)
            {
                if (blockSize + 0x20 <= freeNode->size)
                {
                    freeNode->size -= blockSize;
                    HEAPNODE *nextNode = (HEAPNODE *)((byte*)freeNode + freeNode->size);
                    nextNode->size = blockSize;
                    nextNode->prev = freeNode;
                    nextNode->next = freeNode->next;
                    nextNode->prev->next = nextNode;
                    nextNode->next->prev = nextNode;
                    nextNode->isFree = FALSE;
                    void *mem = (void*)((byte*)nextNode + 0x20);
                    InstallFenceposts(nextNode);
                    return mem;
                }
                else
                {
                    freeNode->freePrev->freeNext = freeNode->freeNext;
                    freeNode->freeNext->freePrev = freeNode->freePrev;
                    freeNode->isFree = FALSE;
                    void *mem = (void*)((byte*)freeNode + 0x20);
                    InstallFenceposts(freeNode);
                    return mem;
                }
            }
            freeNode = freeNode->freeNext;
        }
    }
    return NULL;
}

int QHeap::Free(void *p)
{
    if (!p)
    {
        return 0;
    }
    dassert(heapPtr != NULL, 261);
    HEAPNODE *node = (HEAPNODE*)((byte*)p - 0x20);
    if (node->isFree)
    {
        ThrowError(271)("Free on bad or freed block");
    }
    CheckFenceposts(node);
    if (node->prev->isFree)
    {
        node->prev->size += node->size;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node = node->prev;
    }
    else
    {
        node->freeNext = freeHeap.freeNext;
        node->freePrev = &freeHeap;
        node->freePrev->freeNext = node;
        node->freeNext->freePrev = node;
        node->isFree = TRUE;
    }
    HEAPNODE *nextNode = node->next;
    if (nextNode->isFree)
    {
        node->size += nextNode->size;
        nextNode->freePrev->freeNext = nextNode->freeNext;
        nextNode->freeNext->freePrev = nextNode->freePrev;
        nextNode->prev->next = nextNode->next;
        nextNode->next->prev = nextNode->prev;
    }
    return node->size - 0x30;
}
