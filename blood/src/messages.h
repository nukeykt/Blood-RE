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
#ifndef _MESSAGES_H_
#define _MESSAGES_H_

class CGameMessageMgr {
public:
    struct messageStruct
    {
        int at0;
        char at4[81];
    };
    char at0;
    int at1;
    int at5;
    int at9;
    int atd;
    int at11;
    int at15;
    int at19;
    int at1d;
    char at21;
    int at22;
    int at26;
    int at2a;
    messageStruct at2e[16];
    CGameMessageMgr();
	void SetCoordinates(int,int);
    void SetState(byte);
    void SetFont(int);
    void SetMaxMessages(int);
    void SetMessageTime(int);
    void Add(char *, byte);
    void Display(void);
    void Clear();
    void SetMessageFlags(unsigned int nFlags);
};

class CPlayerMsg
{
public:
    int at0;
    char at4[42];
    CPlayerMsg() { at4[0] = 0; }
    void Clear(void);
    void Term(void);
    void Draw(void);
    BOOL AddChar(char);
    void DelChar(void);
    void Set(char *pzString);
    void Send(void);
    void ProcessKeys(void);
};

class CCheatMgr
{
public:
    static BOOL m_bPlayerCheated;
    enum CHEATCODE
    {
        kCheatNone = 0,
        kCheat1,
        kCheat2,
        kCheat3,
        kCheat4,
        kCheat5,
        kCheat6,
        kCheat7,
        kCheat8,
        kCheat9,
        kCheat10,
        kCheat11,
        kCheat12,
        kCheat13,
        kCheat14,
        kCheat15,
        kCheat16,
        kCheat17,
        kCheat18,
        kCheat19,
        kCheat20,
        kCheat21,
        kCheat22,
        kCheat23,
        kCheat24,
        kCheat25,
        kCheat26,
        kCheat27,
        kCheat28,
        kCheat29,
        kCheat30,
        kCheat31,
        kCheat32,
        kCheat33,
        kCheat34,
        kCheat35,
        kCheat36,
        kCheatMax
    };
    static unsigned long kCheatFlagsNone;
    struct CHEATINFO
    {
        char *pzString;
        CHEATCODE id;
        int flags;
    };
    static CHEATINFO s_CheatInfo[];
    CCheatMgr() {}
    void func_5BCF4(void);
    BOOL Check(char *);
    void Process(CHEATCODE nCheatCode, char* pzArgs);
};

extern CGameMessageMgr gGameMessageMgr;
extern CPlayerMsg gPlayerMsg;
extern CCheatMgr gCheatMgr;

#endif
