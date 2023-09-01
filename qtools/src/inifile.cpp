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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include "typedefs.h"
#include "debug4g.h"
#include "inifile.h"
#include "misc.h"


IniFile::IniFile(char *file)
{
    f_0.next = &f_0;
    strcpy(f_11, file);
    Load();
}

IniFile::IniFile(void *buf, int dummy)
{
    f_0.next = &f_0;
    strcpy(f_11, "menus.mat");
    Load(buf);
}

void IniFile::Load(void *buf)
{
    char line[256];
    if (!buf)
        return;
    char *cbuf = (char*)buf;
    curNode = &f_0;
    while (ReadLine(line, 256, &cbuf))
    {
        char *vs = strchr(line, '\n');
        if (vs)
            *vs = 0;
        vs = strchr(line, '\r');
        if (vs)
            *vs = 0;

        vs = line;
        while (isspace(*vs))
        {
            vs++;
        }
        curNode->next = (IniNode*)malloc(sizeof(IniNode) + strlen(vs));
        dassert(curNode->next != NULL, 92);
        f_9 = curNode;
        curNode = curNode->next;
        strcpy(curNode->f_4, vs);
        switch (*vs)
        {
            case 0:
            case ';':
                break;
            case '[':
                if (!strchr(vs, ']'))
                {
                    free(curNode);
                    curNode = f_9;
                }
                break;
            default:
                if (strchr(vs, '=') <= vs)
                {
                    free(curNode);
                    curNode = f_9;
                }
                break;
        }
    }
    curNode->next = &f_0;
}

void IniFile::Load(void)
{
    char line[256];
    curNode = &f_0;
    FILE *hFile = fopen(f_11, "rt");
    if (hFile)
    {
        while (fgets(line, 256, hFile))
        {
            char *vs = strchr(line, '\n');
            if (vs)
                *vs = 0;
            vs = strchr(line, '\r');
            if (vs)
                *vs = 0;

            vs = line;
            while (isspace(*vs))
            {
                vs++;
            }
            curNode->next = (IniNode*)malloc(sizeof(IniNode) + strlen(vs));
            dassert(curNode->next != NULL, 193);
            f_9 = curNode;
            curNode = curNode->next;
            strcpy(curNode->f_4, vs);
            switch (*vs)
            {
                case 0:
                case ';':
                    break;
                case '[':
                    if (!strchr(vs, ']'))
                    {
                        free(curNode);
                        curNode = f_9;
                    }
                    break;
                default:
                    if (strchr(vs, '=') <= vs)
                    {
                        free(curNode);
                        curNode = f_9;
                    }
                    break;
            }
        }
        fclose(hFile);
    }
    curNode->next = &f_0;
}

void IniFile::Save(void)
{
    char line[256];
    int hFile = open(f_11, O_WRONLY | O_CREAT | O_TRUNC | O_TEXT, S_IRUSR | S_IWUSR);
    dassert(hFile != -1, 257);
    curNode = f_0.next;
    while (curNode != &f_0)
    {
        sprintf(line, "%s\n", curNode->f_4);
        write(hFile, line, strlen(line));
        curNode = curNode->next;
    }
    close(hFile);
}

BOOL IniFile::FindSection(char *section)
{
    char buffer[256];
    curNode = f_9 = &f_0;
    if (section)
    {
        sprintf(buffer, "[%s]", section);
        
        while (1)
        {
            f_9 = curNode;
            curNode = curNode->next;
            if (curNode == &f_0)
                return FALSE;
            if (!stricmp(curNode->f_4, buffer))
                return TRUE;
        }
    }
    return TRUE;
}

BOOL IniFile::FindKey(char *key)
{
    f_9 = curNode;
    curNode = curNode->next;
    while (curNode != &f_0)
    {
        if (curNode->f_4[0] == ';' || curNode->f_4[0] == '\0')
        {
            f_9 = curNode;
            curNode = curNode->next;
            continue;
        }
        if (curNode->f_4[0] == '[')
            break;

        char *pEqual = strchr(curNode->f_4, '=');
        dassert(pEqual != NULL, 325);
        char *vb = pEqual;
        while (isspace(*(vb-1)))
        {
            vb--;
        }
        char back = *vb;
        *vb = 0;
        if (!stricmp(key, curNode->f_4))
        {
            *vb = back;
            f_d = pEqual + 1;
            while (isspace(*f_d))
            {
                f_d++;
            }
            return TRUE;
        }
        *vb = back;
        f_9 = curNode;
        curNode = curNode->next;
    }
    return FALSE;
}

void IniFile::AddSection(char *section)
{
    char buf[256];
    if (f_9 != &f_0)
    {
        IniNode *newNode = (IniNode*)malloc(5);
        dassert(newNode != NULL, 363);
        newNode->f_4[0] = 0;
        newNode->next = f_9->next;
        f_9->next = newNode;
        f_9 = newNode;
    }
    sprintf(buf, "[%s]", section);
    IniNode *newNode = (IniNode*)malloc(5 + strlen(buf));
    dassert(newNode != NULL, 375);
    strcpy(newNode->f_4, buf);
    newNode->next = f_9->next;
    f_9->next = newNode;
    f_9 = newNode;
}

void IniFile::AddKeyString(char *key, char *val)
{
    char buf[256];
    sprintf(buf, "%s=%s", key, val);
    IniNode *newNode = (IniNode*)malloc(5 + strlen(buf));
    dassert(newNode != NULL, 375);
    strcpy(newNode->f_4, buf);
    newNode->next = f_9->next;
    f_9->next = newNode;
    f_9 = newNode;
}

void IniFile::ChangeKeyString(char *key, char *val)
{
    char buf[256];
    sprintf(buf, "%s=%s", key, val);
    IniNode *newNode = (IniNode*)realloc(curNode, 5 + strlen(buf));
    dassert(newNode != NULL, 375);
    strcpy(newNode->f_4, buf);
    f_9->next = newNode;
}

BOOL IniFile::SectionExists(char *section)
{
    return FindSection(section);
}

BOOL IniFile::KeyExists(char *section, char *key)
{
    if (FindSection(section) && FindKey(key))
        return TRUE;
    return FALSE;
}

void IniFile::PutKeyString(char *section, char *key, char *val)
{
    if (FindSection(section))
    {
        if (FindKey(key))
        {
            ChangeKeyString(key, val);
            return;
        }
    }
    else
        AddSection(section);
    AddKeyString(key, val);
}

char *IniFile::GetKeyString(char *section, char *key, char *val)
{
    if (FindSection(section) && FindKey(key))
        return f_d;
    return val;
}

void IniFile::PutKeyInt(char *section, char *key, int val)
{
    char buffer[256];
    itoa(val, buffer, 10);
    PutKeyString(section, key, buffer);
}

int IniFile::GetKeyInt(char *section, char *key, int val)
{
    if (FindSection(section) && FindKey(key))
        return strtol(f_d, NULL, 0);
    return val;
}

BOOL IniFile::GetKeyBool(char *section, char *key, int val)
{
    return GetKeyInt(section, key, val);
}

void IniFile::PutKeyHex(char *section, char *key, int val)
{
    char sValue[256] = "0x";
    itoa(val, sValue + 2, 16);
    PutKeyString(section, key, sValue);
}

int IniFile::GetKeyHex(char *section, char *key, int val)
{
    return GetKeyInt(section, key, val);
}

void IniFile::RemoveKey(char *section, char *key)
{
    if (FindSection(section) && FindKey(key))
    {
        f_9->next = curNode->next;
        free(curNode);
        curNode = f_9->next;
    }
}

void IniFile::RemoveSection(char *section)
{
    if (FindSection(section))
    {
        f_9 = curNode;
        curNode = curNode->next;
        while (curNode != &f_0)
        {
            if (curNode->f_4[0] == '[')
                break;
            f_9->next = curNode->next;
            free(curNode);
            curNode = f_9->next;
        }
    }
}

IniFile::~IniFile(void)
{
    curNode = f_0.next;
    while (curNode != &f_0)
    {
        f_9 = curNode;
        curNode = curNode->next;
        free(f_9);
    }
}
