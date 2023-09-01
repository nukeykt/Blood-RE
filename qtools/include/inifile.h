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
#ifndef _INIFILE_H_
#define _INIFILE_H_

#include "typedefs.h"


struct IniNode {
    IniNode *next;
    char f_4[1];
};

class IniFile
{
public:
    IniFile(char *);
    IniFile(void *, int dummy);
    void Load(void *);
    void Load(void);
    void Save(void);
    BOOL FindSection(char *);
    BOOL FindKey(char*);
    void AddSection(char *);
    void AddKeyString(char *, char *);
    void ChangeKeyString(char *, char *);
    BOOL SectionExists(char *);
    BOOL KeyExists(char *, char *);
    void PutKeyString(char *, char *, char *);
    char *GetKeyString(char *, char *, char *);
    void PutKeyInt(char *, char *, int);
    int GetKeyInt(char *, char *, int);
    BOOL GetKeyBool(char *, char *, int);
    void PutKeyHex(char *, char *, int);
    int GetKeyHex(char *, char *, int);
    void RemoveKey(char *, char *);
    void RemoveSection(char *);
    ~IniFile(void);

    IniNode f_0;
    IniNode *curNode;
    IniNode *f_9;

    char *f_d;

    char f_11[144];

};


#endif
