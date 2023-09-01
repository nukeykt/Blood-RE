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
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "typedefs.h"
#include "qheap.h"

struct RFFHeader
{
    char sign[4];
    short version;
    short pad1;
    ulong offset;
    ulong filenum;
    int pad2[4];
};


struct CACHENODE
{
    void* ptr;
    CACHENODE* prev;
    CACHENODE* next;
    int lockCount;
};

struct DICTNODE : CACHENODE
{
    ulong offset;
    ulong size;
    int pad1[2];
    byte flags;
    char type[3];
    char name[8];
    int id;
};

enum {
    kResourceFlag1 = 1,
    kResourceFlag2 = 2,
    kResourceFlag3 = 4,
    kResourceFlag4 = 8,
    kResourceFlag5 = 16,
};

class Resource
{
public:
    Resource(void);
    ~Resource(void);

    void Init(char *filename, char *external);
    static void Flush(CACHENODE *h);
    void Purge(void);
    DICTNODE **Probe(char *fname, char *type);
    DICTNODE **Probe(unsigned long id, char *type);
    void Reindex(void);
    void Grow(void);
    void AddExternalResource(char *name, char *type, int size);
    static void *Alloc(long nSize);
    static void Free(void *p);
    DICTNODE *Lookup(char *name, char *type);
    DICTNODE *Lookup(unsigned long id, char *type);
    void Read(DICTNODE *n);
    void Read(DICTNODE *n, void *p);
    void *Load(DICTNODE *h);
    void *Load(DICTNODE *h, void *p);
    void *Lock(DICTNODE *h);
    void Unlock(DICTNODE *h);
    void Crypt(byte *p, long length, int key);
    static inline void AddMRU(CACHENODE* h);
    static inline void RemoveMRU(CACHENODE* h);
    static inline int Size(DICTNODE *n) { return n->size; }

    DICTNODE *dict;
    DICTNODE **indexName;
    DICTNODE **indexId;
    int buffSize;
    int count;
    int handle;
    BOOL crypt;
    BOOL f_19;
    char ext[144];

    static QHeap *heap;
    static CACHENODE purgeHead;
};

static inline void Resource::AddMRU(CACHENODE* h)
{
    h->prev = purgeHead.prev;
    h->prev->next = h;
    h->next = &purgeHead;
    h->next->prev = h;
}

static inline void Resource::RemoveMRU(CACHENODE* h)
{
    h->prev->next = h->next;
    h->next->prev = h->prev;
}
#endif
