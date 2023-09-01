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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include "typedefs.h"
#include "crc32.h"
#include "debug4g.h"
#include "error.h"
#include "misc.h"
#include "qheap.h"
#include "resource.h"

CACHENODE Resource::purgeHead = { NULL, &purgeHead, &purgeHead, 0 };
QHeap *Resource::heap;

Resource::Resource(void)
{
    dict = NULL;
    indexName = NULL;
    indexId = NULL;
    buffSize = 0;
    count = 0;
    handle = -1;
    crypt = TRUE;
    ext[0] = 0;
}

Resource::~Resource(void)
{
    if (dict)
    {
        Free(dict);
        dict = NULL;
        buffSize = 0;
        count = 0;
    }
    if (handle != -1)
    {
        close(handle);
        handle = -1;
    }
}

void Resource::Init(char *filename, char *external)
{
    char out[148];
    struct find_t info;
    char *type;
    char *dir;
    char *node;
    char *fname;
    RFFHeader header;
    dassert(heap != NULL, 63);

    if (filename)
    {
        handle = open(filename, O_BINARY);
        if (handle != -1)
        {
            int nFileLength = filelength(handle);
            dassert(nFileLength != -1, 74);
            if (!FileRead(handle, &header, sizeof(RFFHeader))
             || memcmp(header.sign, "RFF\x1a", 4))
            {
                close(handle);
                ThrowError(81)("RFF header corrupted");
            }
            if ((header.version & 0xff00) != 0x300)
            {
                if ((header.version & 0xff00) != 0x200)
                {
                    close(handle);
                    ThrowError(91)("Unknown RFF version");
                }
                else
                    crypt = 0;
            }
            else
                crypt = 1;
            count = header.filenum;
            if (count)
            {
                buffSize = 1;
                while (count * 2 >= buffSize)
                {
                    buffSize *= 2;
                }
                dict = (DICTNODE*)Alloc(buffSize * sizeof(DICTNODE));
                memset(dict, 0, buffSize * sizeof(DICTNODE));
                int r = lseek(handle, header.offset, SEEK_SET);
                dassert(r != -1, 112);
                if (!FileRead(handle, dict, count * sizeof(DICTNODE)))
                {
                    close(handle);
                    ThrowError(116)("RFF dictionary corrupted");
                }
                if (crypt)
                {
                    Crypt((byte*)dict, count * sizeof(DICTNODE), header.offset + (header.version & 0xff) * header.offset);
                }
            }
        }
    }
    if (!dict)
    {
        buffSize = 16;
        dict = (DICTNODE*)Alloc(buffSize * sizeof(DICTNODE));
        memset(dict, 0, buffSize * sizeof(DICTNODE));
    }
    Reindex();
    if (external)
    {
        _splitpath2(external, out, &dir, &node, NULL, NULL);
        _makepath(ext, dir, node, NULL, NULL);
        int status = _dos_findfirst(external, 0, &info);
        while (!status)
        {
            _splitpath2(info.name, out, NULL, NULL, &fname, &type);
            if (*type == '.')
            {
                AddExternalResource(fname, (char*)(type + 1), info.size);
            }
            else
            {
                AddExternalResource(fname, "", info.size);
            }
            status = _dos_findnext(&info);
        }
        _dos_findclose(&info);
    }
    for (int i = 0; i < count; i++)
    {
        if (dict[i].flags & kResourceFlag4)
        {
            Lock(&dict[i]);
        }
    }
    for (i = 0; i < count; i++)
    {
        if (dict[i].flags & kResourceFlag3)
        {
            Load(&dict[i]);
        }
    }
}

void Resource::Flush(CACHENODE *h)
{
    if (h->ptr)
    {
        heap->Free(h->ptr);
        h->ptr = NULL;
        if (h->lockCount == 0)
        {
            RemoveMRU((CACHENODE*)h);
            return;
        }
        h->lockCount = 0;
    }
}

void Resource::Purge(void)
{
    for (int i = 0; i < count; i++)
    {
        if (dict[i].ptr)
        {
            Flush((CACHENODE *)&dict[i]);
        }
    }
}

DICTNODE **Resource::Probe(char *fname, char *type)
{
    char name[11];
    dassert(indexName != NULL, 228);
    memset(name, 0, sizeof(name));
    strncpy(name, type, 3);
    strncpy(name + 3, fname, 8);
    dassert(dict != NULL, 234);
    int i;
    int hash = i = CRC32(name, sizeof(name)) & (buffSize - 1);
    while (1)
    {
        if (!indexName[i])
        {
            return &indexName[i];
        }
        if (!memcmp((*indexName[i]).type, name, 3)
         && !memcmp((*indexName[i]).name, name + 3, 8))
        {
            return &indexName[i];
        }
        if (++i == buffSize)
        {
            i = 0;
        }
        if (i == hash)
        {
            break;
        }
    }
    ThrowError(254)("Linear probe failed to find match or unused node!");
    return NULL;
}

DICTNODE **Resource::Probe(unsigned long id, char *type)
{
    char name[7];
    dassert(indexName != NULL, 264);
    memset(name, 0, sizeof(name));
    strncpy(name, type, 3);
    *(int*)(name + 3) = id;
    dassert(dict != NULL, 274);
    int i;
    int hash = i =  CRC32(name, sizeof(name)) & (buffSize - 1);
    while (1)
    {
        if (!indexId[i])
        {
            return &indexId[i];
        }
        if (!memcmp((*indexId[i]).type, name, 3)
         && (*indexId[i]).id  == *(int*)(name + 3))
        {
            return &indexId[i];
        }
        if (++i == buffSize)
        {
            i = 0;
        }
        if (i == hash)
        {
            break;
        }
    }
    ThrowError(294)("Linear probe failed to find match or unused node!");
    return NULL;
}

void Resource::Reindex(void)
{
    if (indexName)
    {
        Free(indexName);
    }
    indexName = (DICTNODE **)Alloc(buffSize * sizeof(DICTNODE*));
    memset(indexName, 0, buffSize * sizeof(DICTNODE*));
    for (int i = 0; i < count; i++)
    {
        DICTNODE **node = Probe(dict[i].name, dict[i].type);
        *node = &dict[i];
    }

    if (indexId)
    {
        Free(indexId);
    }
    indexId = (DICTNODE **)Alloc(buffSize * sizeof(DICTNODE*));
    memset(indexId, 0, buffSize * sizeof(DICTNODE*));
    for (i = 0; i < count; i++)
    {
        if (dict[i].flags & kResourceFlag1)
        {
            DICTNODE **node = Probe(dict[i].id, dict[i].type);
            *node = &dict[i];
        }
    }
}

void Resource::Grow(void)
{
    buffSize *= 2;
    void *p = Alloc(buffSize * sizeof(DICTNODE));
    memset(p, 0, buffSize * sizeof(DICTNODE));
    memcpy(p, dict, count * sizeof(DICTNODE));
    Free(dict);
    dict = (DICTNODE*)p;
    Reindex();
}

void Resource::AddExternalResource(char *name, char *type, int size)
{
    dassert(dict != NULL, 352);
    DICTNODE **index = Probe(name, type);
    dassert(index != NULL, 354);
    DICTNODE *node = *index;
    strupr(name);
    strupr(type);
    if (!node)
    {
        if (2 * count >= buffSize)
        {
            Grow();
        }
        node = &dict[count++];
        index = Probe(name, type);
        *index = node;
        strncpy(node->type, type, 3);
        strncpy(node->name, name, 8);
    }
    node->size = size;
    node->flags |= kResourceFlag2;
    Flush((CACHENODE*)node);
}

static void *Resource::Alloc(long nSize)
{
    dassert(heap != NULL, 382);
    dassert(nSize != NULL, 383);
    void *p = heap->Alloc(nSize);
    if (p)
    {
        return p;
    }
    for (CACHENODE *node = purgeHead.next; node != &purgeHead; node = node->next)
    {
        dassert(node->lockCount == 0, 392);
        dassert(node->ptr != NULL, 395);
        long nFree = heap->Free(node->ptr);
        node->ptr = NULL;
        RemoveMRU(node);
        if (nSize <= nFree)
        {
            p = heap->Alloc(nSize);
            dassert(p != NULL, 406);
            return p;
        }
    }
    ThrowError(416)("Out of memory!");
    return NULL;
}

static void Resource::Free(void *p)
{
    dassert(heap != NULL, 423);
    dassert(p != NULL, 424);
    heap->Free(p);
}

DICTNODE *Resource::Lookup(char *name, char *type)
{
    dassert(name != NULL, 432);
    dassert(type != NULL, 433);
    return *Probe(strupr(name), strupr(type));
}

DICTNODE *Resource::Lookup(unsigned long id, char *type)
{
    dassert(type != NULL, 441);
    return *Probe(id, strupr(type));
}

void Resource::Read(DICTNODE *n)
{
    dassert(n != NULL, 449);
    Read(n, n->ptr);
}

void Resource::Read(DICTNODE *n, void *p)
{
    char buf[144];
    dassert(n != NULL, 456);
    if (!p)
        return;
    if (n->flags & kResourceFlag2)
    {
        sprintf(buf, "%s%.8s.%.3s", ext, n->name, n->type);
        if (!FileLoad(buf, p, n->size))
        {
            ThrowError(472)("Error reading external resource (%i)", *__get_errno_ptr());
        }
    }
    else
    {
        int r = lseek(handle, n->offset, SEEK_SET);
        if (r == -1)
        {
            ThrowError(479)("Error seeking to resource!");
        }
        if (!FileRead(handle, p, n->size))
        {
            ThrowError(482)("Error loading resource!");
        }
        if (n->flags & kResourceFlag5)
        {
            Crypt((byte*)n->ptr, n->size > 256 ? 256 : n->size, 0);
        }
    }
}

void *Resource::Load(DICTNODE *h)
{
    dassert(h != NULL, 506);
    if (h->ptr)
    {
        if (!h->lockCount)
        {
            RemoveMRU((CACHENODE*)h);
            AddMRU(h);
        }
        return h->ptr;
    }
    h->ptr = Alloc(h->size);
    Read(h);
    AddMRU(h);
    return h->ptr;
}

void *Resource::Load(DICTNODE *h, void *p)
{
    dassert(h != NULL, 530);
    if (p)
    {
        Read(h, p);
    }
    return p;
}

void *Resource::Lock(DICTNODE *h)
{
    dassert(h != NULL, 545);
    if (h->ptr)
    {
        if (h->lockCount == 0)
        {
            RemoveMRU((CACHENODE*)h);
        }
    }
    else
    {
        h->ptr = Alloc(h->size);
        Read(h);
    }

    h->lockCount++;
    return h->ptr;
}

void Resource::Unlock(DICTNODE *h)
{
    dassert(h != NULL, 566);
    dassert(h->ptr != NULL, 567);
    if (h->lockCount > 0)
    {
        h->lockCount--;
        if (h->lockCount == 0)
        {
            AddMRU(h);
        }
    }
}

void Resource::Crypt(byte *p, long length, int key)
{
    for (int i = 0; i < length; i++, key++)
    {
        byte xr = ((ushort)key / 2);
        p[i] ^= xr;
    }
}
