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
#include <stdio.h>
#include "typedefs.h"
#include "build.h"
#include "config.h"
#include "controls.h"
#include "debug4g.h"
#include "error.h"
#include "globals.h"
#include "key.h"
#include "levels.h"
#include "network.h"
#include "player.h"
#include "seq.h"
#include "sound.h"
#include "view.h"

int int_27B088;

char packet[576];

BOOL char_27B2CC;
BOOL gStartNewGame;

PKT_STARTGAME gPacketStartGame;
PACKETMODE gPacketMode;

int gNetFifoClock;
int gNetFifoTail;
int gNetFifoHead[kMaxPlayers];
int gPredictTail;
int gNetFifoMasterTail;

INPUT gFifoInput[256][kMaxPlayers];

int gNetSentSizeThreshold;

int myMinLag[kMaxPlayers];
int otherMinLag;
int myMaxLag;

ulong gChecksum[4];
ulong gCheckFifo[256][kMaxPlayers][4];
int gCheckHead[kMaxPlayers];
int gSendCheckTail;
int gCheckTail;

int gInitialNetPlayers;

int gBufferJitter;

int gPlayerReady[kMaxPlayers];

BOOL gRobust;
BOOL bOutOfSync;
BOOL ready2send;

struct struct28E3B0
{
    SYNCFLAGS at0;
    int at4;
    int at8;
    int atc;
    BUTTONFLAGS at10;
    KEYFLAGS at14;
    USEFLAGS at18;
    char at1c;
    int at1d;
};

struct28E3B0 char_28E3B0;

int int_28E3D4;

byte char_28E3D8[4000];

short short_28F378;
int int_28F37C;

const BLOODVERSION short_1328AC = { 20, 2 };

BOOL bNoResend = 1;
int gSyncRate = 1;
int int_13B860 = 2;

void func_83444(int nDest, char *pBuffer, int nSize);
short func_83E44(int nDest, char *pBuffer, int nSize);
void func_8364C(short nDest, char *pBuffer, short nSize);
int tenBloodSendPacket(int nIndex, char *bufptr, int buflen);

void netsendpacket(int nDest, char *pBuffer, int nSize)
{
    if (nSize > gNetSentSizeThreshold)
        gNetSentSizeThreshold = nSize;
    switch (int_28E3D4)
    {
    case 0:
        sendpacket(nDest, pBuffer, nSize);
        break;
    case 1:
        func_83444(nDest, pBuffer, nSize);
        break;
    case 2:
        func_83E44(nDest, pBuffer, nSize);
        break;
    case 3:
        func_8364C(nDest, pBuffer, nSize);
        break;
    case 4:
        tenBloodSendPacket(nDest, pBuffer, nSize);
        break;
    }
}

void netSendPacketAll(char *pBuffer, int nSize)
{
    switch (int_28E3D4)
    {
    case 0:
    {
        for (int p = connecthead; p >= 0; p = connectpoint2[p])
            if (p != myconnectindex)
                netsendpacket(p, pBuffer, nSize);
        break;
    }
    case 1:
        func_83444(-1, pBuffer, nSize);
        break;
    case 2:
        func_83E44(-1, pBuffer, nSize);
        break;
    case 3:
        func_8364C(-1, pBuffer, nSize);
        break;
    case 4:
        tenBloodSendPacket(-1, pBuffer, nSize);
        break;
    }
}

void func_79760(void)
{
    gNetFifoClock = gFrameClock = gGameClock = 0;
    gNetFifoMasterTail = 0;
    gPredictTail = 0;
    gNetFifoTail = 0;
    memset(gNetFifoHead, 0, sizeof(gNetFifoHead));
    memset(gCheckFifo, 0, sizeof(gCheckFifo));
    memset(myMinLag, 0, sizeof(myMinLag));
    otherMinLag = 0;
    myMaxLag = 0;
    memset(gCheckHead, 0, sizeof(gCheckHead));
    gSendCheckTail = 0;
    gCheckTail = 0;
    memset(&char_28E3B0, 0, sizeof(char_28E3B0));
    bOutOfSync = 0;
}

ulong inline Checksum(char *p, int l)
{
    int *pl = (int*)p;
    l >>= 2;
    int sum = 0;
    while (l--)
    {
        sum += *pl++;
    }
    return sum;
}

void CalcGameChecksum(void)
{
    memset(gChecksum, 0, sizeof(gChecksum));
    gChecksum[0] = rand();
    for (int p = connecthead; p >= 0; p = connectpoint2[p])
    {
        gChecksum[1] ^= Checksum((char*)&gPlayer[p].at22, 216*4);
        gChecksum[2] ^= Checksum((char*)gPlayer[p].pSprite, 44);
        gChecksum[3] ^= Checksum((char*)gPlayer[p].pXSprite, 56);
    }
}

byte func_86760(byte *);

void netCheckSync(void)
{
    int p;
    char v4;
    char buffer[80];
    if (gGameOptions.nGameType == GAMETYPE_0)
        return;
    if (numplayers == 1)
        return;
    if (bOutOfSync)
        return;
    while (1)
    {
        for (p = connecthead; p >= 0; p = connectpoint2[p])
        {
            if (gCheckHead[p] <= gCheckTail)
                return;
        }
        if (int_28E3D4 == 4)
            v4 = func_86760((byte*)gCheckFifo[gCheckTail&255][myconnectindex]);

        for (p = connecthead; p >= 0; p = connectpoint2[p])
        {
            if (p != myconnectindex)
            {
                int status;
                if (int_28E3D4 == 4)
                    status = v4 != gCheckFifo[gCheckTail & 255][p][0];
                else
                    status = memcmp(gCheckFifo[gCheckTail&255][p], gCheckFifo[gCheckTail&255][connecthead], 16);
                if (status)
                {
                    sprintf(buffer, "OUT OF SYNC (%d):", p);
                    char *pBuffer = buffer + strlen(buffer);
                    for (int i = 0; i < 4UL; i++)
                    {
                        if (gCheckFifo[gCheckTail&255][p][i] != gCheckFifo[gCheckTail&255][connecthead][i])
                            pBuffer += sprintf(pBuffer, " %d", i);
                    }
                    viewSetErrorMessage(buffer);
                    bOutOfSync = 1;
                }
            }
        }
        gCheckTail++;
    }
}

short func_8349C(short *pSource, char *pBuffer);
short func_83EB0(short *pSource, char *pBuffer);
short func_83700(short *pSource, char *pBuffer);
short tenBloodGetPacket(short *pSource, char *pBuffer);

short netGetPacket(short *pSource, char *pMessage)
{
    switch (int_28E3D4)
    {
    case 0:
        return getpacket(pSource, packet);
    case 1:
        return func_8349C(pSource, pMessage);
    case 2:
        return func_83EB0(pSource, pMessage);
    case 3:
        return func_83700(pSource, pMessage);
    case 4:
        return tenBloodGetPacket(pSource, pMessage);
    }
    return 0;
}

short func_79B08(short *a1, char *a2)
{
    if (int_28F37C == 0)
    {
        short vc = netGetPacket(a1, a2);
        if (vc != 0)
        {
            memcpy(char_28E3D8, a2, vc);
            int_28F37C++;
        }
        short_28F378 = vc;
    }
    else
    {
        memcpy(a2, char_28E3D8, short_28F378);
        int_28F37C++;
        if (int_28F37C == int_13B860)
            int_28F37C = 0;
    }
    return short_28F378;
}

void netGetPackets(void)
{
    short nPlayer;
    short nSize;
    int p;
    char *pPacket;
    char buffer[128];
    while ((nSize = netGetPacket(&nPlayer, packet)) > 0)
    {
        pPacket = packet;
        switch (GetPacketByte(pPacket))
        {
        case 0:
            for (p = connecthead; p >= 0; p = connectpoint2[p])
            {
                if (p != myconnectindex)
                {
                    INPUT *pInput = &gFifoInput[gNetFifoHead[p]&255][p];
                    memset(pInput, 0, sizeof(INPUT));
                    pInput->syncFlags.byte = GetPacketByte(pPacket);
                    pInput->forward = GetPacketByte(pPacket);
                    pInput->turn = GetPacketWord(pPacket);
                    pInput->strafe = GetPacketByte(pPacket);
                    if (pInput->syncFlags.buttonChange)
                        pInput->buttonFlags.byte = GetPacketByte(pPacket);
                    if (pInput->syncFlags.keyChange)
                        pInput->keyFlags.word = GetPacketWord(pPacket);
                    if (pInput->syncFlags.useChange)
                        pInput->useFlags.byte = GetPacketByte(pPacket);
                    if (pInput->syncFlags.weaponChange)
                        pInput->newWeapon = GetPacketByte(pPacket);
                    if (pInput->syncFlags.mlookChange)
                        pInput->mlook = GetPacketByte(pPacket);
                    gNetFifoHead[p]++;
                }
                else
                {
                    SYNCFLAGS syncFlags;
                    syncFlags.byte = GetPacketByte(pPacket);
                    pPacket += 1+2+1;
                    if (syncFlags.buttonChange)
                        pPacket++;
                    if (syncFlags.keyChange)
                        pPacket+=2;
                    if (syncFlags.useChange)
                        pPacket++;
                    if (syncFlags.weaponChange)
                        pPacket++;
                    if (syncFlags.mlookChange)
                        pPacket++;
                }
            }
            if (((gNetFifoHead[connecthead]-1)&15)==0)
            {
                for (p = connectpoint2[connecthead]; p >= 0; p = connectpoint2[p])
                {
                    int nLag = (signed char)GetPacketByte(pPacket);
                    if (p == myconnectindex)
                        otherMinLag = nLag;
                }
            }
            while (pPacket < packet+nSize)
            {
                int checkSum[4];
                GetPacketBuffer(pPacket, checkSum, sizeof(checkSum));
                for (p = connecthead; p >= 0; p = connectpoint2[p])
                {
                    if (p != myconnectindex)
                    {
                        memcpy(gCheckFifo[gCheckHead[p]&255][p], checkSum, sizeof(checkSum));
                        gCheckHead[p]++;
                    }
                }
            }
            break;
        case 1:
        {
            INPUT *pInput = &gFifoInput[gNetFifoHead[nPlayer]&255][nPlayer];
            memset(pInput, 0, sizeof(INPUT));
            pInput->syncFlags.byte = GetPacketByte(pPacket);
            pInput->forward = GetPacketByte(pPacket);
            pInput->turn = GetPacketWord(pPacket);
            pInput->strafe = GetPacketByte(pPacket);
            if (pInput->syncFlags.buttonChange)
                pInput->buttonFlags.byte = GetPacketByte(pPacket);
            if (pInput->syncFlags.keyChange)
                pInput->keyFlags.word = GetPacketWord(pPacket);
            if (pInput->syncFlags.useChange)
                pInput->useFlags.byte = GetPacketByte(pPacket);
            if (pInput->syncFlags.weaponChange)
                pInput->newWeapon = GetPacketByte(pPacket);
            if (pInput->syncFlags.mlookChange)
                pInput->mlook = GetPacketByte(pPacket);
            gNetFifoHead[nPlayer]++;
            while (pPacket < packet+nSize)
            {
                GetPacketBuffer(pPacket, gCheckFifo[gCheckHead[nPlayer]&255][nPlayer], 16);
                gCheckHead[nPlayer]++;
            }
            break;
        }
        case 2:
        {
            if (nPlayer == connecthead && (gNetFifoHead[nPlayer]&15) == 0 && int_28E3D4 != 4 && int_28E3D4 != 1)
            {
                for (p = connectpoint2[connecthead]; p >= 0; p = connectpoint2[p])
                {
                    int nLag = (signed char)GetPacketByte(pPacket);
                    if (p == myconnectindex)
                        otherMinLag = nLag;
                }
            }
            INPUT *pInput = &gFifoInput[gNetFifoHead[nPlayer]&255][nPlayer];
            memset(pInput, 0, sizeof(INPUT));
            pInput->syncFlags.byte = GetPacketByte(pPacket);
            pInput->forward = GetPacketByte(pPacket);
            pInput->turn = GetPacketWord(pPacket);
            pInput->strafe = GetPacketByte(pPacket);
            if (pInput->syncFlags.buttonChange)
                pInput->buttonFlags.byte = GetPacketByte(pPacket);
            if (pInput->syncFlags.keyChange)
                pInput->keyFlags.word = GetPacketWord(pPacket);
            if (pInput->syncFlags.useChange)
                pInput->useFlags.byte = GetPacketByte(pPacket);
            if (pInput->syncFlags.weaponChange)
                pInput->newWeapon = GetPacketByte(pPacket);
            if (pInput->syncFlags.mlookChange)
                pInput->mlook = GetPacketByte(pPacket);
            gNetFifoHead[nPlayer]++;
            for (p = gSyncRate; p > 1; p--)
            {
                INPUT *pInput2 = &gFifoInput[gNetFifoHead[nPlayer]&255][nPlayer];
                memcpy(pInput2, pInput, sizeof(INPUT));
                pInput2->keyFlags.word = 0;
                pInput2->useFlags.byte = 0;
                pInput2->newWeapon = 0;
                pInput2->syncFlags.weaponChange = 0;
                gNetFifoHead[nPlayer]++;
            }
            while (pPacket < packet+nSize)
            {
                if (int_28E3D4 == 4)
                {
                    gCheckFifo[gCheckHead[nPlayer]&255][nPlayer][0] = GetPacketByte(pPacket);
                }
                else
                {
                    GetPacketBuffer(pPacket, gCheckFifo[gCheckHead[nPlayer]&255][nPlayer], 16);
                }
                gCheckHead[nPlayer]++;
            }
            break;
        }
        case 3:
            pPacket += 4;
            if (*pPacket != '/' || (!*pPacket && !*(pPacket+1)) || (*(pPacket+1) >= '1' && *(pPacket+1) <= '8' && *(pPacket+1)-'1' == myconnectindex))
            {
                sprintf(buffer, "%s : %s", gProfile[nPlayer].name, pPacket);
                viewSetMessage(buffer);
                sndStartSample("DMRADIO", 128);
            }
            break;
        case 4:
            sndStartSample(4400+GetPacketByte(pPacket), 128, 1);
            break;
        case 7:
            pPacket += 4;
            dassert(nPlayer != myconnectindex, 646);
            netWaitForEveryone(0);
            netPlayerQuit(nPlayer);
            netWaitForEveryone(0);
            break;
        case 250:
            gPlayerReady[nPlayer]++;
            break;
        case 251:
            memcpy(&gProfile[nPlayer], pPacket, sizeof(PROFILE));
            break;
        case 252:
            pPacket += 4;
            memcpy(&gPacketStartGame, pPacket, sizeof(PKT_STARTGAME));
            if (gPacketStartGame.version != short_1328AC.w)
                ThrowError(678)("\nThese versions of Blood cannot play together.\n");
            gStartNewGame = 1;
            break;
        case 255:
            keystatus[1] = 1;
            break;
        }
    }
}

void netBroadcastMyLogoff(void)
{
    if (numplayers < 2)
        return;
    char *pPacket = packet;
    PutPacketByte(pPacket, 7);
    PutPacketDWord(pPacket, myconnectindex);
    netSendPacketAll(packet, pPacket-packet);
    netWaitForEveryone(0);
    gQuitGame = 1;
    ready2send = 0;
}

void netBroadcastPlayerInfo(int nPlayer)
{
    if (numplayers < 2)
        return;
    PROFILE *pProfile = &gProfile[nPlayer];
    strcpy(pProfile->name, PlayerName);
    pProfile->skill = gSkill;
    pProfile->at0 = gAutoAim;
    char *pPacket = packet;
    PutPacketByte(pPacket, 251);
    PutPacketBuffer(pPacket, pProfile, sizeof(PROFILE));
    netSendPacketAll(packet, pPacket-packet);
}

void netBroadcastNewGame(void)
{
    if (numplayers < 2)
        return;
    gPacketStartGame.version = short_1328AC.w;
    char *pPacket = packet;
    PutPacketByte(pPacket, 252);
    PutPacketDWord(pPacket, myconnectindex);
    PutPacketBuffer(pPacket, &gPacketStartGame, sizeof(PKT_STARTGAME));
    netSendPacketAll(packet, pPacket-packet);
}

void netBroadcastTaunt(int nPlayer, int nTaunt)
{
    if (numplayers > 1)
    {
        char *pPacket = packet;
        PutPacketByte(pPacket, 4);
        PutPacketByte(pPacket, nTaunt);
        netSendPacketAll(packet, pPacket-packet);
    }
    sndStartSample(4400+nTaunt, 128, 1);
}

void netBroadcastMsg(int nPlayer, char *pzMessage)
{
    if (numplayers > 1)
    {
        int nSize = strlen(pzMessage);
        char *pPacket = packet;
        PutPacketByte(pPacket, 3);
        PutPacketDWord(pPacket, nPlayer);
        PutPacketBuffer(pPacket, pzMessage, nSize+1);
        netSendPacketAll(packet, pPacket-packet);
    }
}

void netWaitForEveryone(BOOL a1)
{
    if (numplayers < 2)
        return;
    char *pPacket = packet;
    PutPacketByte(pPacket, 250);
    netSendPacketAll(packet, pPacket-packet);
    gPlayerReady[myconnectindex]++;
    int p;
    do
    {
        if (keystatus[1] && a1)
            exit(0);
        netGetPackets();
        for (p = connecthead; p >= 0; p = connectpoint2[p])
            if (gPlayerReady[p] < gPlayerReady[myconnectindex])
                break;
    } while (p >= 0);
}

int tenBloodScore(char *);

void func_7AC28(char *pzString)
{
    if (numplayers <= 1)
        return;
    if (pzString)
    {
        int nLength = strlen(pzString);
        if (nLength > 0)
        {
            if (int_28E3D4 == 4)
            {
                tenBloodScore(pzString);
            }
            else
            {
                char *pPacket = packet;
                PutPacketByte(pPacket, 5);
                PutPacketBuffer(pPacket, pzString, nLength+1);
                netSendPacketAll(packet, pPacket-packet);
            }
        }
    }
}

void netSendEmptyPackets(void)
{
    int nClock = gGameClock;
    char *pPacket = packet;
    PutPacketByte(pPacket, 254);
    for (int i = 0; i < 8; i++)
    {
        if (nClock <= gGameClock)
        {
            nClock = gGameClock+4;
            netSendPacketAll(packet, pPacket-packet);
        }
    }
}

void func_7AD90(INPUT *pInput)
{
    char_28E3B0.at0.byte |= pInput->syncFlags.byte;
    char_28E3B0.at4 += pInput->forward;
    char_28E3B0.at8 += pInput->turn;
    char_28E3B0.atc += pInput->strafe;
    char_28E3B0.at10.byte |= pInput->buttonFlags.byte;
    char_28E3B0.at14.word |= pInput->keyFlags.word;
    char_28E3B0.at18.byte |= pInput->useFlags.byte;
    if (pInput->newWeapon)
        char_28E3B0.at1c = pInput->newWeapon;
    char_28E3B0.at1d = pInput->mlook;
}

void func_7AE2C(INPUT *pInput)
{
    pInput->syncFlags = char_28E3B0.at0;
    pInput->forward = char_28E3B0.at4 / gSyncRate;
    pInput->turn = char_28E3B0.at8 / gSyncRate;
    pInput->strafe = char_28E3B0.atc / gSyncRate;
    pInput->buttonFlags = char_28E3B0.at10;
    pInput->keyFlags = char_28E3B0.at14;
    pInput->useFlags = char_28E3B0.at18;
    pInput->newWeapon = char_28E3B0.at1c;
    pInput->mlook = char_28E3B0.at1d;
    memset(&char_28E3B0, 0, sizeof(char_28E3B0));
}

int func_83500(void);
void tenCalculateJitter(void);

void netGetInput(void)
{
    int nPlayer;
    if (numplayers > 1)
        netGetPackets();
    if (int_28E3D4 == 1)
    {
        if (!func_83500())
            return;
    }
    else
    {
        for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
            if (gNetFifoHead[nPlayer] < gNetFifoHead[myconnectindex]-200)
                return;
    }
    ctrlGetInput();
    func_7AD90(&gInput);
    if (gNetFifoHead[myconnectindex]&(gSyncRate-1))
    {
        INPUT *pInput1 = &gFifoInput[gNetFifoHead[myconnectindex]&255][myconnectindex];
        memcpy(pInput1, &gFifoInput[(gNetFifoHead[myconnectindex]-1)&255][myconnectindex], sizeof(INPUT));
        pInput1->keyFlags.word = 0;
        pInput1->useFlags.byte = 0;
        pInput1->newWeapon = 0;
        pInput1->syncFlags.weaponChange = 0;
        gNetFifoHead[myconnectindex]++;
        return;
    }
    INPUT *pInput = &gFifoInput[gNetFifoHead[myconnectindex]&255][myconnectindex];
    func_7AE2C(pInput);
    memcpy(&gInput, pInput, sizeof(INPUT));
    gNetFifoHead[myconnectindex]++;
    if (gGameOptions.nGameType == GAMETYPE_0 || numplayers == 1)
    {
        for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
        {
            if (nPlayer != myconnectindex)
            {
                memcpy(&gFifoInput[gNetFifoHead[nPlayer]&255][nPlayer], &gFifoInput[(gNetFifoHead[nPlayer]-1)&255][nPlayer], sizeof(INPUT));
                gNetFifoHead[nPlayer]++;
            }
        }
        return;
    }
    for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
    {
        if (nPlayer != myconnectindex)
        {
            int nLag = gNetFifoHead[myconnectindex]-1-gNetFifoHead[nPlayer];
            myMinLag[nPlayer] = Min(myMinLag[nPlayer], nLag);
            myMaxLag = Max(myMaxLag, nLag);
        }
    }
    if ((gNetFifoHead[myconnectindex]&15) == 0)
    {
        int t = myMaxLag-gBufferJitter;
        myMaxLag = 0;
        if (t > 0)
            gBufferJitter += (2+t)>>2;
        else if (t < 0)
            gBufferJitter -= (2-t)>>2;
    }
    if (int_28E3D4 == 4)
        tenCalculateJitter();
    if (gPacketMode == PACKETMODE_2)
    {
        char *pPacket = packet;
        PutPacketByte(pPacket, 2);
        if (((gNetFifoHead[myconnectindex]-1)&15) == 0 && int_28E3D4 != 4 && int_28E3D4 != 1)
        {
            if (myconnectindex == connecthead)
            {
                for (nPlayer = connectpoint2[connecthead]; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                    PutPacketByte(pPacket, ClipRange(myMinLag[nPlayer], -128, 127));
            }
            else
            {
                int t = myMinLag[connecthead]-otherMinLag;
                if (klabs(t) > 2)
                {
                    if (klabs(t) > 8)
                    {
                        if (t < 0)
                            t++;
                        t >>= 1;
                    }
                    else
                        t = ksgn(t);
                    gGameClock -= t<<2;
                    otherMinLag += t;
                    myMinLag[connecthead] -= t;
                }
            }
            for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                myMinLag[nPlayer] = 0x7fffffff;
        }
        if (gInput.buttonFlags.byte)
            gInput.syncFlags.buttonChange = 1;
        if (gInput.keyFlags.word)
            gInput.syncFlags.keyChange = 1;
        if (gInput.useFlags.byte)
            gInput.syncFlags.useChange = 1;
        if (gInput.newWeapon)
            gInput.syncFlags.weaponChange = 1;
        if (gInput.mlook)
            gInput.syncFlags.mlookChange = 1;
        PutPacketByte(pPacket, gInput.syncFlags.byte);
        PutPacketByte(pPacket, gInput.forward);
        PutPacketWord(pPacket, gInput.turn);
        PutPacketByte(pPacket, gInput.strafe);
        if (gInput.syncFlags.buttonChange)
            PutPacketByte(pPacket, gInput.buttonFlags.byte);
        if (gInput.syncFlags.keyChange)
            PutPacketWord(pPacket, gInput.keyFlags.word);
        if (gInput.syncFlags.useChange)
            PutPacketByte(pPacket, gInput.useFlags.byte);
        if (gInput.syncFlags.weaponChange)
            PutPacketByte(pPacket, gInput.newWeapon);
        if (gInput.syncFlags.mlookChange)
            PutPacketByte(pPacket, gInput.mlook);
        while (gSendCheckTail != gCheckHead[myconnectindex])
        {
            if (int_28E3D4 == 4)
                PutPacketByte(pPacket, func_86760((byte*)gCheckFifo[gSendCheckTail&255][myconnectindex]));
            else
                PutPacketBuffer(pPacket, gCheckFifo[gSendCheckTail&255][myconnectindex], 16);
            gSendCheckTail++;
        }
        netSendPacketAll(packet, pPacket-packet);
        return;
    }
    if (myconnectindex != connecthead)
    {
        char *pPacket = packet;
        PutPacketByte(pPacket, 1);
        if (gInput.buttonFlags.byte)
            gInput.syncFlags.buttonChange = 1;
        if (gInput.keyFlags.word)
            gInput.syncFlags.keyChange = 1;
        if (gInput.useFlags.byte)
            gInput.syncFlags.useChange = 1;
        if (gInput.newWeapon)
            gInput.syncFlags.weaponChange = 1;
        if (gInput.mlook)
            gInput.syncFlags.mlookChange = 1;
        PutPacketByte(pPacket, gInput.syncFlags.byte);
        PutPacketByte(pPacket, gInput.forward);
        PutPacketWord(pPacket, gInput.turn);
        PutPacketByte(pPacket, gInput.strafe);
        if (gInput.syncFlags.buttonChange)
            PutPacketByte(pPacket, gInput.buttonFlags.byte);
        if (gInput.syncFlags.keyChange)
            PutPacketWord(pPacket, gInput.keyFlags.word);
        if (gInput.syncFlags.useChange)
            PutPacketByte(pPacket, gInput.useFlags.byte);
        if (gInput.syncFlags.weaponChange)
            PutPacketByte(pPacket, gInput.newWeapon);
        if (gInput.syncFlags.mlookChange)
            PutPacketByte(pPacket, gInput.mlook);
        if (((gNetFifoHead[myconnectindex]-1)&15) == 0)
        {
            int t = myMinLag[connecthead]-otherMinLag;
            if (klabs(t) > 2)
            {
                if (klabs(t) > 8)
                {
                    if (t < 0)
                        t++;
                    t >>= 1;
                }
                else
                    t = ksgn(t);
                gGameClock -= t<<2;
                otherMinLag += t;
                myMinLag[connecthead] -= t;
            }
            for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                myMinLag[nPlayer] = 0x7fffffff;
        }
        while (gSendCheckTail != gCheckHead[myconnectindex])
        {
            PutPacketBuffer(pPacket, gCheckFifo[gSendCheckTail&255][myconnectindex], 16);
            gSendCheckTail++;
        }
        netsendpacket(connecthead, packet, pPacket-packet);
        return;
    }
    char v4 = 0;
    do
    {
        for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
            if (gNetFifoHead[nPlayer] <= gNetFifoMasterTail)
            {
                if (!v4)
                {
                    char *pPacket = packet;
                    PutPacketByte(pPacket, 254);
                    for (; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                        netsendpacket(nPlayer, packet, pPacket-packet);
                }
                return;
            }
        v4 = 1;
        char *pPacket = packet;
        PutPacketByte(pPacket, 0);
        for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
        {
            INPUT *pInput = &gFifoInput[gNetFifoMasterTail&255][nPlayer];
            if (pInput->buttonFlags.byte)
                pInput->syncFlags.buttonChange = 1;
            if (pInput->keyFlags.word)
                pInput->syncFlags.keyChange = 1;
            if (pInput->useFlags.byte)
                pInput->syncFlags.useChange = 1;
            if (pInput->newWeapon)
                pInput->syncFlags.weaponChange = 1;
            if (pInput->mlook)
                pInput->syncFlags.mlookChange = 1;
            PutPacketByte(pPacket, pInput->syncFlags.byte);
            PutPacketByte(pPacket, pInput->forward);
            PutPacketWord(pPacket, pInput->turn);
            PutPacketByte(pPacket, pInput->strafe);
            if (pInput->syncFlags.buttonChange)
                PutPacketByte(pPacket, pInput->buttonFlags.byte);
            if (pInput->syncFlags.keyChange)
                PutPacketWord(pPacket, pInput->keyFlags.word);
            if (pInput->syncFlags.useChange)
                PutPacketByte(pPacket, pInput->useFlags.byte);
            if (pInput->syncFlags.weaponChange)
                PutPacketByte(pPacket, pInput->newWeapon);
            if (pInput->syncFlags.mlookChange)
                PutPacketByte(pPacket, pInput->mlook);
        }
        if ((gNetFifoMasterTail&15) == 0)
        {
            for (nPlayer = connectpoint2[connecthead]; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                PutPacketByte(pPacket, ClipRange(myMinLag[nPlayer], -128, 127));
            for (nPlayer = connecthead; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
                myMinLag[nPlayer] = 0x7fffffff;
        }
        while (gSendCheckTail != gCheckHead[myconnectindex])
        {
            PutPacketBuffer(pPacket, gCheckFifo[gSendCheckTail&255][myconnectindex], 16);
            gSendCheckTail++;
        }
        for (nPlayer = connectpoint2[connecthead]; nPlayer >= 0; nPlayer = connectpoint2[nPlayer])
            netsendpacket(nPlayer, packet, pPacket-packet);
        gNetFifoMasterTail++;
    } while (1);
}

void tenBloodIdle(void);

extern "C" void faketimerhandler(void)
{
    if (int_28E3D4 == 4)
    {
        if (int_27B088++ > 160)
        {
            int_27B088 = 0;
            tenBloodIdle();
        }
    }
    if (gGameClock >= gNetFifoClock && ready2send)
    {
        gNetFifoClock += 4;
        netGetInput();
    }
}

void netInitialize(void)
{
    memset(gPlayerReady, 0, sizeof(gPlayerReady));
    switch (int_28E3D4)
    {
    case 0:
        initmultiplayers(0, 0, 0);
        break;
    }
    gInitialNetPlayers = gNetPlayers = numplayers;
    if (gPacketMode == PACKETMODE_0)
    {
        int t = gInitialNetPlayers;
        if (t > 4)
            gPacketMode = PACKETMODE_1;
        else
            gPacketMode = PACKETMODE_2;
    }
    gNetSentSizeThreshold = 0;
    if (gSyncRate > 1 && bNoResend == 0)
        setpackettimeout(0x3fffffff, 0x3fffffff);
}

void netPlayerQuit(int nPlayer)
{
    char buffer[128];
    sprintf(buffer, "%s left the game with %d frags.", gProfile[nPlayer].name, gPlayer[nPlayer].at2c6);
    viewSetMessage(buffer);
    if (gGameStarted)
    {
        seqKill(3, gPlayer[nPlayer].pSprite->extra);
        actPostSprite(gPlayer[nPlayer].at5b, kStatFree);
        if (nPlayer == gViewIndex)
            gViewIndex = myconnectindex;
        gView = &gPlayer[gViewIndex];
    }
    if (nPlayer == connecthead)
    {
        connecthead = connectpoint2[connecthead];
        if (gPacketMode == PACKETMODE_1)
        {
            char_27B2CC = 1;
            gQuitGame = 1;
            gQuitRequest = 1;
        }
    }
    else
    {
        for (int p = connecthead; p >= 0; p = connectpoint2[p])
        {
            if (connectpoint2[p] == nPlayer)
            {
                connectpoint2[p] = connectpoint2[nPlayer];
                break;
            }
        }
    }
    gNetPlayers--;
    numplayers = ClipLow(numplayers-1, 1);
}

#if 0
void func_7AC28(char*) {}
void netBroadcastNewGame(void) {}
void netWaitForEveryone(BOOL) {}
void netBroadcastMsg(int, char*) {}
#endif
