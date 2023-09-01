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
#ifndef _SEQ_H_
#include "typedefs.h"
#include "resource.h"

struct SEQFRAME {
    unsigned int tile : 12;
    unsigned int at1_4 : 1; // transparent
    unsigned int at1_5 : 1; // transparent
    unsigned int at1_6 : 1; // blockable
    unsigned int at1_7 : 1; // hittable
    unsigned int at2_0 : 8; // xrepeat
    unsigned int at3_0 : 8; // yrepeat
    signed int at4_0 : 8; // shade
    unsigned int at5_0 : 5; // palette
    unsigned int at5_5 : 1; //
    unsigned int at5_6 : 1; //
    unsigned int at5_7 : 1; //
    unsigned int at6_0 : 1; //
    unsigned int at6_1 : 1; //
    unsigned int at6_2 : 1; // invisible
    unsigned int at6_3 : 1; //
    unsigned int at6_4 : 1; //
    unsigned int pad : 11;
};

struct Seq {
    char signature[4];
    short version;
    short nFrames; // at6
    short at8;
    short ata;
    byte atc;
    byte __f_d[3];
    SEQFRAME frames[1]; // at10
    void Preload(void);
    void Precache(void);
};

enum {
    kSeqFlag0 = 1,
    kSeqFlag1 = 2,
};

struct ACTIVE
{
    byte type;
    ushort xindex;
};

struct SEQINST
{
    DICTNODE *hSeq;
    Seq *pSequence; // at4
    int at8;
    int atc;
    short at10;
    byte frameIndex; // at12
    char at13;
    void Update(ACTIVE *pActive);
};

int seqRegisterClient(void(*pClient)(int, int));
void seqPreloadId(int id);
SEQINST * GetInstance(int a1, int a2);
void UnlockInstance(SEQINST *pInst);
void seqSpawn(int a1, int a2, int a3, int a4 = -1);
void seqKill(int a1, int a2);
void seqKillAll(void);
int seqGetStatus(int a1, int a2);
int seqGetID(int a1, int a2);
void seqProcess(int a1);
void seqCache(int);

#endif
