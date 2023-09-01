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
#include "typedefs.h"
#include "build.h"
#include "callback.h"
#include "db.h"
#include "debug4g.h"
#include "error.h"
#include "eventq.h"
#include "globals.h"
#include "levels.h"
#include "loadsave.h"
#include "pqueue.h"
#include "triggers.h"

class EventQueue : public PriorityQueue
{
public:
    BOOL IsNotEmpty(int nTime)
    {
        return fNodeCount > 0 && nTime >= queueItems[1].at0;
    }
    EVENT ERemove(void)
    {
        ulong node = Remove();
        return *(EVENT*)&node;
    }
    void Kill(int, int);
    void Kill(int, int, CALLBACK_ID);

    void Insert(ulong a1, EVENT a2) { PriorityQueue::Insert(a1, *(ulong*)&a2); }
};

void EventQueue::Kill(int a1, int a2)
{
    EVENT evn = { a1, a2, 0, 0 };
    //evn.at0_0 = a1;
    //evn.at1_5 = a2;

    short vs = *(short*)&evn;
    for (int i = 1; i <= fNodeCount;)
    {
        if ((short)queueItems[i].at4 == vs)
            Delete(i);
        else
            i++;
    }
}

void EventQueue::Kill(int a1, int a2, CALLBACK_ID a3)
{
    EVENT evn = { a1, a2, kCommandCallback, a3 };
    int vc = *(int*)&evn;
    for (int i = 1; i <= fNodeCount;)
    {
        if (queueItems[i].at4 == vc)
            Delete(i);
        else
            i++;
    }
}

EventQueue eventQ;

struct RXBUCKET
{
    unsigned int at0_0 : 13;
    unsigned int at1_5 : 3;
};

RXBUCKET rxBucket[kMaxChannels];

int GetBucketChannel(RXBUCKET *pRX)
{
    int nIndex, nXIndex;
    switch (pRX->at1_5)
    {
    case 6:
        nIndex = pRX->at0_0;
        nXIndex = sector[nIndex].extra;
        dassert(nXIndex > 0, 99);
        return xsector[nXIndex].at8_0;
    case 0:
        nIndex = pRX->at0_0;
        nXIndex = wall[nIndex].extra;
        dassert(nXIndex > 0, 104);
        return xwall[nXIndex].at8_0;
    case 3:
        nIndex = pRX->at0_0;
        nXIndex = sprite[nIndex].extra;
        dassert(nXIndex > 0, 109);
        return xsprite[nXIndex].at5_2;
    default:
        ThrowError(113)("Unexpected rxBucket type %d, index %d", pRX->at1_5, pRX->at0_0);
        break;
    }
    return 0;
}

int CompareChannels(void const *a, void const *b)
{
    return GetBucketChannel((RXBUCKET*)a)-GetBucketChannel((RXBUCKET*)b);
}

unsigned short bucketHead[1024+1];

void evInit(void)
{
    int i;
    int nCount;
    eventQ.fNodeCount = nCount = 0;
    for (i = 0; i < numsectors; i++)
    {
        int nXSector = sector[i].extra;
        if (nXSector >= kMaxXSectors)
            ThrowError(146)("Invalid xsector reference in sector %d", i);
        if (nXSector > 0 && xsector[nXSector].at8_0 > 0)
        {
            dassert(nCount < kMaxChannels, 150);
            rxBucket[nCount].at1_5 = 6;
            rxBucket[nCount].at0_0 = i;
            nCount++;
        }
    }
    for (i = 0; i < numwalls; i++)
    {
        int nXWall = wall[i].extra;
        if (nXWall >= kMaxXWalls)
            ThrowError(161)("Invalid xwall reference in wall %d", i);
        if (nXWall > 0 && xwall[nXWall].at8_0 > 0)
        {
            dassert(nCount < kMaxChannels, 165);
            rxBucket[nCount].at1_5 = 0;
            rxBucket[nCount].at0_0 = i;
            nCount++;
        }
    }
    for (i = 0; i < kMaxSprites; i++)
    {
        if (sprite[i].statnum < kMaxStatus)
        {
            int nXSprite = sprite[i].extra;
            if (nXSprite >= kMaxXSprites)
                ThrowError(178)("Invalid xsprite reference in sprite %d", i);
            if (nXSprite > 0 && xsprite[nXSprite].at5_2 > 0)
            {
                dassert(nCount < kMaxChannels, 182);
                rxBucket[nCount].at1_5 = 3;
                rxBucket[nCount].at0_0 = i;
                nCount++;
            }
        }
    }
    qsort(rxBucket, nCount, sizeof(RXBUCKET), CompareChannels);
    int j = 0;
    for (i = 0; i < 1024; i++)
    {
        bucketHead[i] = j;
        while(j < nCount && GetBucketChannel(&rxBucket[j]) == i)
            j++;
    }
    bucketHead[i] = j;
}

BOOL evGetSourceState(int nType, int nIndex)
{
    int nXIndex;
    switch (nType)
    {
    case 6:
        nXIndex = sector[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXSectors, 220);
        return xsector[nXIndex].at1_6;
    case 0:
        nXIndex = wall[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXWalls, 225);
        return xwall[nXIndex].at1_6;
    case 3:
        nXIndex = sprite[nIndex].extra;
        dassert(nXIndex > 0 && nXIndex < kMaxXSprites, 230);
        return xsprite[nXIndex].at1_6;
    }
    return 0;
}

void evSend(int nIndex, int nType, int rxId, COMMAND_ID command)
{
    if (command == COMMAND_ID_2)
        command = evGetSourceState(nType, nIndex) ? COMMAND_ID_1 : COMMAND_ID_0;
    else if (command == COMMAND_ID_4)
        command = evGetSourceState(nType, nIndex) ? COMMAND_ID_0 : COMMAND_ID_1;
    EVENT evn;
    evn.at0_0 = nIndex;
    evn.at1_5 = nType;
    evn.at2_0 = command;
    if (rxId > 0)
    {
        switch (rxId)
        {
        case 8:
            break;
        case 9:
            break;


        case 7:
            break;
        case 10:
            break;
        case 3:
            if (command < COMMAND_ID_64)
                ThrowError(265)("Invalid TextOver command by xobject %d(type %d)", nIndex, nType);
            trTextOver(command-COMMAND_ID_64);
            return;
        case 4:
            levelEndLevel(0);
            return;
        case 5:
            levelEndLevel(1);
            return;
        case 1:
            if (command < COMMAND_ID_64)
                ThrowError(305)("Invalid SetupSecret command by xobject %d(type %d)", nIndex, nType);
            levelSetupSecret(command - COMMAND_ID_64);
            break;
        case 2:
            if (command < COMMAND_ID_64)
                ThrowError(311)("Invalid Secret command by xobject %d(type %d)", nIndex, nType);
            levelTriggerSecret(command - COMMAND_ID_64);
            break;
        case 90:
        case 91:
        case 92:
        case 93:
        case 94:
        case 95:
        case 96:
        case 97:
        {
            for (int nSprite = headspritestat[4]; nSprite >= 0; nSprite = nextspritestat[nSprite])
            {
                SPRITE *pSprite = &sprite[nSprite];
                if (pSprite->flags&kSpriteFlag5)
                    continue;
                int nXSprite = pSprite->extra;
                if (nXSprite > 0)
                {
                    XSPRITE *pXSprite = &xsprite[nXSprite];
                    if (pXSprite->at5_2 == rxId)
                        trMessageSprite(nSprite, evn);
                }
            }
            return;
        }
        case 80:
        case 81:
        {
            for (int nSprite = headspritestat[3]; nSprite >= 0; nSprite = nextspritestat[nSprite])
            {
                SPRITE *pSprite = &sprite[nSprite];
                if (pSprite->flags&kSpriteFlag5)
                    continue;
                int nXSprite = pSprite->extra;
                if (nXSprite > 0)
                {
                    XSPRITE *pXSprite = &xsprite[nXSprite];
                    if (pXSprite->at5_2 == rxId)
                        trMessageSprite(nSprite, evn);
                }
            }
            return;
        }
        }
    }
    for (int i = bucketHead[rxId]; i < bucketHead[rxId+1]; i++)
    {
        if (rxBucket[i].at1_5 == evn.at1_5 && rxBucket[i].at0_0 == evn.at0_0)
            continue;
        switch (rxBucket[i].at1_5)
        {
            case 6:
                trMessageSector(rxBucket[i].at0_0, evn);
                break;
            case 0:
                trMessageWall(rxBucket[i].at0_0, evn);
                break;
            case 3:
            {
                int nSprite = rxBucket[i].at0_0;
                SPRITE *pSprite = &sprite[nSprite];
                if (pSprite->flags&kSpriteFlag5)
                    continue;
                int nXSprite = pSprite->extra;
                if (nXSprite > 0)
                {
                    XSPRITE *pXSprite = &xsprite[nXSprite];
                    if (pXSprite->at5_2 > 0)
                        trMessageSprite(nSprite, evn);
                }
                break;
            }
        }
    }
}

void evPost(int nIndex, int nType, unsigned long nDelta, COMMAND_ID command)
{
    dassert(command != kCommandCallback, 408);
    if (command == COMMAND_ID_2)
        command = evGetSourceState(nType, nIndex) ? COMMAND_ID_1 : COMMAND_ID_0;
    else if (command == COMMAND_ID_4)
        command = evGetSourceState(nType, nIndex) ? COMMAND_ID_0 : COMMAND_ID_1;
    EVENT evn;
    evn.at0_0 = nIndex;
    evn.at1_5 = nType;
    evn.at2_0 = command;
    // Inlined?
    eventQ.Insert(gFrameClock+nDelta, evn);
}

void evPost(int nIndex, int nType, unsigned long nDelta, CALLBACK_ID callback)
{
    EVENT evn;
    evn.at0_0 = nIndex;
    evn.at1_5 = nType;
    evn.at2_0 = kCommandCallback;
    evn.funcID = callback;
    eventQ.Insert(gFrameClock+nDelta, evn);
}

void evProcess(unsigned long nTime)
{
    while(eventQ.IsNotEmpty(nTime))
    {
        EVENT event = eventQ.ERemove();
        if (event.at2_0 == kCommandCallback)
        {
            dassert(event.funcID < kCallbackMax, 452);
            dassert(gCallback[event.funcID] != NULL, 453);
            gCallback[event.funcID](event.at0_0);
        }
        else
        {
            switch (event.at1_5)
            {
            case 6:
                trMessageSector(event.at0_0, event);
                break;
            case 0:
                trMessageWall(event.at0_0, event);
                break;
            case 3:
                trMessageSprite(event.at0_0, event);
                break;
            }
        }
    }
}

void evKill(int a1, int a2)
{
    eventQ.Kill(a1, a2);
}

void evKill(int a1, int a2, CALLBACK_ID a3)
{
    eventQ.Kill(a1, a2, a3);
}

class EventQLoadSave : public LoadSave
{
public:
    virtual void Load();
    virtual void Save();
};

static EventQLoadSave myLoadSave;

void EventQLoadSave::Load()
{
    Read(&eventQ, sizeof(eventQ));
    Read(rxBucket, sizeof(rxBucket));
    Read(bucketHead, sizeof(bucketHead));
}

void EventQLoadSave::Save()
{
    Write(&eventQ, sizeof(eventQ));
    Write(rxBucket, sizeof(rxBucket));
    Write(bucketHead, sizeof(bucketHead));
}
