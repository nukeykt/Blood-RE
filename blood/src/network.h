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
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <string.h>
#include <stdlib.h>
#include "typedefs.h"
#include "controls.h"
#include "debug4g.h"

enum PACKETMODE {
    PACKETMODE_0 = 0,
    PACKETMODE_1,
    PACKETMODE_2,
};

extern char packet[576];

struct PKT_STARTGAME {
    ushort version;
    schar gameType;
    char difficulty, monsterSettings, weaponSettings, itemSettings, respawnSettings;
    schar episodeId, levelId;
    int unk;
    char userMap, userMapName[13];
};

extern PKT_STARTGAME gPacketStartGame;

extern int gPredictTail;
extern int gNetFifoTail;

extern int gNetFifoHead[kMaxPlayers];

extern INPUT gFifoInput[256][kMaxPlayers];

extern int int_28E3D4;

extern int gInitialNetPlayers;

extern BOOL bOutOfSync;
extern int gBufferJitter;
extern int gCheckTail;
extern ulong gCheckFifo[256][kMaxPlayers][4];
extern ulong gChecksum[4];
extern int gCheckHead[kMaxPlayers];
extern int gSendCheckTail;
extern int gNetFifoMasterTail;
extern int myMaxLag;
extern int otherMinLag;
extern int myMinLag[kMaxPlayers];
extern int gPlayerReady[kMaxPlayers];

extern BOOL ready2send;

void netBroadcastNewGame(void);
void netWaitForEveryone(BOOL);
void netBroadcastMsg(int, char*);
void netPlayerQuit(int);
void netGetPackets(void);
void func_79760(void);
void netBroadcastTaunt(int nPlayer, int nTaunt);
void CalcGameChecksum(void);
void netBroadcastMyLogoff(void);

// extern int gNetPlayers;

extern int gNetFifoClock;
extern BOOL gStartNewGame;

extern int gSyncRate;

extern BOOL bNoResend;
extern BOOL gRobust;

extern PACKETMODE gPacketMode;

extern BOOL char_27B2CC;


inline void PutPacketBuffer(char *&p, const void *pBuffer, int size)
{
    dassert(p + size < packet + sizeof(packet), 167);
    memcpy(p, pBuffer, size);
    p += size;
}

inline void GetPacketBuffer(char*& p, void* pBuffer, int size)
{
    dassert(p + size < packet + sizeof(packet), 174);
    memcpy(pBuffer, p, size);
    p += size;
}

inline void PutPacketByte(char *&p, int a2)
{
    dassert(p - packet + 1 < sizeof(packet), 184);
    *p++ = a2;
}

inline char GetPacketByte(char *&p)
{
    return *p++;
}

inline void PutPacketWord(char *&p, int a2)
{
    dassert(p - packet + 2 < sizeof(packet), 197);
    *(short*)p = a2;
    p += 2;
}

inline short GetPacketWord(char *&p)
{
    short t = *(short*)p;
    p += 2;
    return t;
}

inline void PutPacketDWord(char *&p, int a2)
{
    dassert(p - packet + 4 < sizeof(packet), 211);
    *(int*)p = a2;
    p += 4;
}

inline int GetPacketDWord(char *&p)
{
    int t = *(int*)p;
    p += 4;
    return t;
}

void func_7AC28(char *);

void netInitialize(void);
void netBroadcastPlayerInfo(int);
void netCheckSync(void);
void netSendEmptyPackets(void);

#endif
