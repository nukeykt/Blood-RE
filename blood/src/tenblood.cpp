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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "typedefs.h"
#include "build.h"
#include "config.h"
#include "debuglib.h"
#include "error.h"
#include "gamemenu.h"
#include "globals.h"
#include "key.h"
#include "menu.h"
#include "misc.h"
#include "network.h"
#include "player.h"
#include "tenutils.h"
#include "textio.h"
#include "view.h"

int gTenActivated;
void *gTenLog;
int pgNumPlayers;
int pgRp1;
int pgRp2;

extern "C" {
int tenArRegisterGame(char *);
int tenArRegisterPlayer(char *);
int tenArIdleArena(void);
int tenArSetOption(int, int *, int);
int tenArInitArena(char *);
int tenArSetPregameHookRoutine(void (*)(char *, char *, char *, char *, char *, char *));
int tenArSetPlayerEnteredRoutine(void (*)(int, int, char *, char *, char *, long, char *));
int tenArSetPlayerLeftRoutine(void (*)(int));
int tenArSetAlertMessageRoutine(void (*)(int, int, char *));
int tenArSetIncomingPacketRoutine(void (*)(int, void *, unsigned int));
int tenArSendToPlayer(int, char*, int);
int tenArSendToOtherPlayers(char*, int);
int tenArSetGameState(char *);
};

class tenWParcel {
public:
    char *f_0;
    int f_4;
    int f_8;
    int f_c;

    tenWParcel(unsigned int a1) {
        f_0 = new char[a1];
        f_4 = a1;
        f_8 = a1;
        f_c = 0;
    }

    tenWParcel(tenWParcel &a1) {
        f_0 = new char[a1.f_4];
        f_4 = a1.f_4;
        f_c = a1.f_c;
        memcpy(f_0, a1.f_0, a1.f_c);
    }

    ~tenWParcel();

    tenWParcel &operator << (char);
};

class tenRParcel {
public:
    char *f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    tenRParcel(void*, unsigned int, int);

    tenRParcel(tenRParcel &a1) {
        f_0 = new char[a1.f_4];
        f_4 = a1.f_4;
        f_8 = a1.f_8;
        f_c = 0;
        f_10 = 0;
        memcpy(f_0, a1.f_0, a1.f_4);
    }

    ~tenRParcel();
    tenRParcel &operator >> (char &);
    tenRParcel &operator >> (long &);
};

#ifdef kDoVerifications
#define tenDbVerify2(x,y) ((x) ? 1 : tenDbVerifyFailed(#x, __FILE__, y))
#define tenDbVerifyNoErr2(x,y) (tenDbVerifyNoErrCore(x, #x, __FILE__, y))
#else
#define tenDbVerify2(x,y)
#define tenDbVerifyNoErr2(x,y) (x)
#endif /* kDoVerifications */

#define kMaxBloodPacketSize 128

typedef struct {
    int f_0;
    unsigned int f_4;
    char f_8[kMaxBloodPacketSize];
} queuedBloodPacket;

queuedBloodPacket pgInQueue[1024];
int pgGotPregameHook;
int pgDonePlayerRoundup;
int pgIsCreator;
long pgPrevClock;
unsigned long pgResetTime = tenUtCurMsecs();
char *pgGameOptions;
int pgDebugLevel;

void onDebugMessage(int, char *a2)
{
    tenDbLprintf(gTenLog, kDebugFatal, "debug message: %s", a2);
    tenDbCloseLogFile(gTenLog);
    gTenLog = NULL;
    if (!pgDonePlayerRoundup)
    {
        ThrowError(66)(a2);
        return;
    }
    printf("%s", a2);
    while (1)
    {
    }
}

void onAlert(int a1, int a2, char *a3)
{
    tenDbLprintf(gTenLog, 3, "alert: %s", a3);
    tenDbCloseLogFile(gTenLog);
    gTenLog = NULL;
    if (!pgDonePlayerRoundup)
    {
        ThrowError(83)(a3);
        return;
    }
    printf("%s", a3);
    while (1)
    {
    }
}

void onPregameHook(char *a1, char *gameTermOptions, char *a3, char *a4, char *a5, char *a6)
{
    char s[64];
    char buf[8];
    int v4;
    if (!strncmp(a1, "create", 5))
    {
        pgIsCreator = 1;
        tenArRegisterGame("");
    }
    sprintf(s, "name '%s' ", a4);
    tenArRegisterPlayer(s);
    strcpy(PlayerName, a4);
    pgGameOptions = (char*)malloc(strlen(gameTermOptions)+1);
    tenDbVerify2(pgGameOptions, 115);
    strcpy(pgGameOptions, gameTermOptions);
    tenDbVerifyNoErr2(tenUtGetStringParam(gameTermOptions, "pmax", buf, sizeof(buf)), 119);
    sscanf(buf, "%d", &v4);
    numplayers = v4;
    tioPrint("%d player TEN game", v4);
    pgGotPregameHook = 1;
}

void onPlayerEntered(int a1, int a2, char *options, char *, char *, long, char *a7)
{
    char nameBuf[64];
    pgNumPlayers++;
    if (a2)
        myconnectindex = a1;
    if (!strncmp(a7, "create", 5))
        connecthead = a1;
    tenDbVerifyNoErr2(tenUtGetStringParam(options, "name", nameBuf, sizeof(nameBuf)), 143);
    tioPrint("%s (player %d) is here.", nameBuf, a1 + 1);
}

void onPlayerLeft(int a1)
{
    if (pgDonePlayerRoundup)
        return;
    tioPrint("Player %d leaves.", a1 + 1);
}

void handlePluginPacket(void *a1, unsigned int a2)
{
    char buf[128];
    char v4;
    tenRParcel v1c(a1, a2, 0);
    v1c >> v4;
    switch (v4)
    {
        case 1:
        {
            long v8;
            v1c >> v8;
            int vd = v8 * gSyncRate * 4;
            gGameClock -= vd;
            pgPrevClock -= vd;
            sprintf(buf, "server timer adjust %ld clock %ld", v8, gGameClock);
            if (pgDebugLevel)
                viewSetMessage(buf);
            tenDbLprintf(gTenLog, 3, buf);
            pgResetTime += vd * 1000 / 120;
            tenDbVerify2(gGameClock >= 0, 176);
            tenDbVerify2(pgPrevClock >= 0, 177);
            break;
        }
    }
}

void onIncomingPacket(int a1, void *a2, unsigned int size)
{
    tenDbVerify2(size < kMaxBloodPacketSize, 185);
    pgInQueue[pgRp1].f_0 = a1;
    pgInQueue[pgRp1].f_4 = size;
    memcpy(pgInQueue[pgRp1].f_8, a2, size);
    pgRp1++;
    if (pgRp1 >= 1024)
        pgRp1 = 0;
    tenDbVerify2(pgRp1 != pgRp2, 193);
}

void tenPlayerRoundup(void)
{
    unsigned int vs = tenUtCurMsecs();
    tioPrint("TEN is waiting for other players to join.");
    while (pgNumPlayers < numplayers || !pgGotPregameHook)
    {
        if (tenUtCurMsecs() - vs >= 200)
        {
            vs = tenUtCurMsecs();
            tenDbVerifyNoErr2(tenArIdleArena(), 208);
            if (keyGet() == 1)
            {
                tioPrint("Quitting...");
                exit(0);
                break;
            }
        }
    }
    for (int i = 0; i < numplayers-1; i++)
        connectpoint2[i] = i+1;
    connectpoint2[i] = -1;
    pgDonePlayerRoundup = 1;
}

int tenBloodInit(void)
{
    int v4 = 1;
    remove("ten.log");
    tenArSetOption(5, &v4, 4);
    tenDbVerifyNoErr2(tenArInitArena("blood"), 234);
    tenDbLprintf(gTenLog, 5, "%20.20s", "tenBloodInit");
    gTenActivated = 1;
    tenDbSetDebugMsgRoutine(onDebugMessage);
    tenArSetPregameHookRoutine(onPregameHook);
    tenArSetPlayerEnteredRoutine(onPlayerEntered);
    tenArSetPlayerLeftRoutine(onPlayerLeft);
    tenArSetAlertMessageRoutine(onAlert);
    tenArSetIncomingPacketRoutine(onIncomingPacket);
    gSyncRate = 2;
    int_28E3D4 = 4;
    gPacketMode = PACKETMODE_2;
    tenPlayerRoundup();
    return 0;
}

void tenResetClock(void)
{
    tenWParcel parcel(16);

    parcel << (char)2;
    tenArSendToPlayer(15, parcel.f_0, parcel.f_c);
    pgResetTime = tenUtCurMsecs();
}

void localClockAdjust(void)
{
    char buf[128];
    int vd = (tenUtCurMsecs() - pgResetTime) * 120 / 1000;
    int va = vd - gGameClock;
    if (va > 0 || va < -32)
    {
        if (pgDonePlayerRoundup)
        {
            sprintf(buf, "adjust clock %ld  expected %ld  delta %ld", gGameClock, vd, va);
            tenDbLprintf(gTenLog, 5, "%s", buf);
            if (pgDebugLevel)
                viewSetMessage(buf);
            gGameClock = vd;
            pgPrevClock = vd;
        }
    }
}

int tenBloodSendPacket(int nIndex, char *bufptr, int buflen)
{
    if (nIndex == -1)
    {
        if (gGameClock < pgPrevClock)
            tenResetClock();
        pgPrevClock = gGameClock;
        if (bufptr[0] == 3 && buflen >= 12 && !memcmp(bufptr+5, "tendebug", 7))
        {
            pgDebugLevel = 1;
            return 0;
        }
        tenDbVerifyNoErr2(tenArSendToOtherPlayers(bufptr, buflen), 312);
    }
    else
        tenDbVerifyNoErr2(tenArSendToPlayer(nIndex, bufptr, buflen), 315);
    tenArIdleArena();
    localClockAdjust();
    return 0;
}

void tenBloodIdle(void)
{
    tenArIdleArena();
}

short tenBloodGetPacket(short *pSource, char *pBuffer)
{
    short nSize;
    localClockAdjust();
    tenArIdleArena();
    if (pgRp1 == pgRp2)
        return 0;
    if (pgInQueue[pgRp2].f_0 == 15)
    {
        nSize = 0;
        handlePluginPacket(pgInQueue[pgRp2].f_8, pgInQueue[pgRp2].f_4);
    }
    else
    {
        *pSource = pgInQueue[pgRp2].f_0;
        nSize = pgInQueue[pgRp2].f_4;
        memcpy(pBuffer, pgInQueue[pgRp2].f_8, nSize);
    }
    pgRp2++;
    if (pgRp2 >= 1024)
        pgRp2 = 0;
    return nSize;
}

int tenBloodScore(char *a1)
{
    char buf[32];
    int v8;
    int v4;
    sscanf(a1, "frag %d killed %d", &v8, &v4);
    v8--;
    v4--;
    sprintf(buf, "kill '%d %d' ", v8, v4);
    tenDbLprintf(gTenLog, 5, "%s", buf);
    tenArSetGameState(buf);
    return 0;
}

int tenLogData(char *a1, int a2, char *a3, unsigned int a4)
{
    char buffer[512];
    char *pos = buffer;
    for (unsigned int i = 0; i < a4; i++)
    {
        tenDbVerify2(pos - buffer < sizeof(buffer), 386);
        sprintf(pos, "%02x ", a3[i]);
        pos += 3;
    }
    tenDbLprintf(gTenLog, 5, "%-16.16s %6ld     %s", a1, a2, buffer);
    return 0;
}

byte func_86760(byte *a1)
{
    byte v = 0;
    unsigned int vs = 0;
    tenDbVerify2(gTenActivated, 400);
    while (vs++ < 16)
    {
        v += *a1++;
    }
    return v;
}

void tenCalculateJitter(void)
{
    static unsigned long lastJitterCalc;
    static unsigned long int_29DC88[10];
    static int int_29DCB0;
    unsigned int va = tenUtCurMsecs();
    unsigned int maxLag = 0;
    unsigned int vc = 0;
    if (va - lastJitterCalc < 500)
        return;
    if (!pgDonePlayerRoundup)
        return;
    lastJitterCalc = va;
    for (int i = connecthead; i >= 0; i = connectpoint2[i])
    {
        int vd = gNetFifoHead[myconnectindex] - gNetFifoHead[i];
        if (maxLag < vd)
            maxLag = vd;
    }
    tenDbVerify2(maxLag < 300, 300);
    int_29DC88[int_29DCB0++] = maxLag;
    if (int_29DCB0 >= 10)
        int_29DCB0 = 0;
    for (i = 0; i < 10; i++)
    {
        if (vc <= int_29DC88[i])
            vc = int_29DC88[i];
    }
    vc += gSyncRate;
    if (vc > 32)
        vc = 32;
    gBufferJitter = vc;
}

struct unk1 {
    char __f_0[12];
    short f_c;
    char __f_e[2];
    char f_10;
};

void func_86890(unk1 *a1)
{
    if (pgDonePlayerRoundup)
    {
        a1->f_c = 0;
        a1->f_10 = 0;
    }
}

int func_868B0(char *a1, int a2)
{
    char vs[16] = "";
    int v4;
    if (tenUtGetStringParam(pgGameOptions, a1, vs, 16))
        return a2;
    sscanf(vs, "%d", &v4);
    return v4;
}

void func_86910(void)
{
    int int_13C81C[3] = { 2,3,1 };
    int int_13C828[4] = { 1,2,3,0 };
    int int_13C838[3] = { 1,2,0 };
    char buffer[13];
    if (!pgIsCreator)
        return;
    tenDbVerify2(pgGotPregameHook, 511);
    tenDbVerify2(pgGameOptions, 512);
    gPacketStartGame.gameType = int_13C81C[func_868B0("game", 0)];
    gPacketStartGame.episodeId = func_868B0("epis", 0);
    gPacketStartGame.levelId = func_868B0("levl", 0);
    gPacketStartGame.difficulty = func_868B0("diff", 0);
    gPacketStartGame.monsterSettings = func_868B0("mstr", 0);
    gPacketStartGame.weaponSettings = int_13C828[func_868B0("wpns", 2)];
    gPacketStartGame.itemSettings = int_13C838[func_868B0("itms", 1)];
    gPacketStartGame.respawnSettings = 0;
    gPacketStartGame.unk = 0;
    gPacketStartGame.userMapName[0] = 0;
    int status = tenUtGetStringParam(pgGameOptions, "umap", buffer, 13);
    tenDbLprintf(gTenLog, 5, "options '%s'", pgGameOptions);
    tenDbLprintf(gTenLog, 5, "user map err %d name '%s'", status, buffer);
    if (status || strlen(buffer) <= 1)
    {
        strncpy(gPacketStartGame.userMapName, "", 13);
        gPacketStartGame.userMap = 0;
    }
    else
    {
        strncpy(gPacketStartGame.userMapName, buffer, 13);
        gPacketStartGame.userMapName[12] = 0;
        gPacketStartGame.userMap = 1;
    }
    netBroadcastNewGame();
    gStartNewGame = 1;
    gGameMenuMgr.Deactivate();
    gGameMenuMgr.Push(&menuLoading, -1);
}

struct playerLag {
    int f_0[32];
    int f_80;
    void putSample(int);
    void getMinMax(int *a1, int *a2);
};

void playerLag::putSample(int a1)
{
    f_0[f_80++] = a1;
    if (f_80 >= 32)
        f_80 = 0;
}

void playerLag::getMinMax(int *a1, int *a2)
{
    int vb = 500;
    int vd = 0;
    int i;
    for (i = 0; i < 32; i++)
    {
        vb = vb < f_0[i] ? vb : f_0[i];
        vd = vd > f_0[i] ? vd : f_0[i];
    }
    *a1 = vb;
    *a2 = vd;
}

void tenPlayerDebugInfo(char *a1, int pid)
{
    if (!pgDebugLevel || !pgDonePlayerRoundup)
    {
        sprintf(a1, "%2d", gPlayer[pid].at2c6);
        return;
    }
    tenDbVerify2(pid < 8 && pid >= 0, 599);
    if (pid == myconnectindex)
    {
        sprintf(a1, "jit %2d", gBufferJitter);
        return;
    }

    static playerLag lags[8];

    lags[pid].putSample(gNetFifoHead[myconnectindex] - gNetFifoHead[pid]);
    int minLag, maxLag;
    lags[pid].getMinMax(&minLag, &maxLag);
    sprintf(a1, "%2d - %2d", minLag, maxLag);
}
