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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>
#include "typedefs.h"
#include "debug4g.h"
#include "misc.h"


char *ReadLine(char *line, int bytes, char **buf)
{
    int i = 0;
    if (!buf || !*buf || !**buf)
        return NULL;
    while (i < bytes && **buf != '\0' && **buf != '\n')
    {
        line[i] = **buf;
        (*buf)++;
        i++;
    }
    if (**buf == '\n' && i < bytes)
    {
        line[i++] = **buf;
        (*buf)++;
    }
    else
    {
        while (**buf != '\0' && **buf != '\n')
        {
            (*buf)++;
        }
        if (**buf == '\n')
            (*buf)++;
    }
    if (i < bytes)
        line[i] = '\0';
    return *buf;
}

BOOL FileRead(int hFile, void *buffer, ulong length)
{
    return read(hFile, buffer, length) == length;
}

BOOL FileWrite(int hFile, void *buffer, ulong length)
{
    return write(hFile, buffer, length) == length;
}

BOOL FileLoad(char *name, void *buffer, ulong length)
{
    dassert(buffer != NULL, 98);
    int hFile = open(name, O_BINARY);
    if (hFile == -1)
        return FALSE;
    ulong l = read(hFile, buffer, length);
    close(hFile);
    return l == length;
}

BOOL FileSave(char *name, void *buffer, ulong length)
{
    dassert(buffer != NULL, 121);
    int hFile = open(name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IWUSR);
    if (hFile == -1)
        return FALSE;
    ulong l = write(hFile, buffer, length);
    close(hFile);
    return l == length;
}

struct MEMINFO {
    uint f_0;
    char __f_4[0x4];
    uint f_8;
    char __f_c[0x8];
    uint f_14;
    uint f_18;
    uint f_1c;
    char __f_20[0x10];
};

void dpmiMemInfo(MEMINFO far *);

#pragma aux dpmiMemInfo = \
"push es" \
"mov es,dx" \
"mov ax,0x500" \
"int 0x31" \
"pop es" \
parm nomemory [dx edi]

ulong func_A8B30(void)
{
    MEMINFO meminfo;
    dpmiMemInfo(&meminfo);
    return meminfo.f_18 << 12;
}

ulong func_A8B50(void)
{
    MEMINFO meminfo;
    dpmiMemInfo(&meminfo);
    return meminfo.f_1c << 12;
}

ulong dpmiDetermineMaxRealAlloc(void)
{
    MEMINFO meminfo;
    dpmiMemInfo(&meminfo);
    if (meminfo.f_14 > 0)
        return meminfo.f_8 << 12;
    return meminfo.f_0;
}

void AddExtension(char *name, char *ext)
{
    char buf[148];
    char *dir, *fn, *oext, *drive;
    _splitpath2(name, buf, &drive, &dir, &fn, &oext);
    if (!*oext)
        oext = ext;
    _makepath(name, drive, dir, fn, oext);
}

void ChangeExtension(char *name, char *ext)
{
    char buf[148];
    char *dir, *fn, *oext, *drive;
    _splitpath2(name, buf, &drive, &dir, &fn, &oext);
    oext = ext;
    _makepath(name, drive, dir, fn, oext);
}

ulong randSeed = 1;

#ifdef RAND_DBG
ulong wrandomseed;
#endif

ulong randStep(ulong seed);

#pragma aux randStep = \
"add eax,eax" \
"jnb short L1" \
"xor eax, 0x20000004" \
"or eax,1" \
"L1:" \
parm nomemory [eax] \
value [eax]

ulong qrand(void)
{
    randSeed = randStep(randSeed);
    return randSeed & 0x7fff;
}

struct DPMI_RMINFO {
    uint edi, esi, ebp, reserved, ebx, edx, ecx, eax;
    ushort flags, es, ds, fs, gs, ip, cs, sp, ss;
};

ushort dpmiAlloc(int size, ushort *segment, void *mem);
#pragma aux dpmiAlloc = \
"xor eax,eax" \
"mov [esi],ax" \
"mov [edi],eax" \
"mov [edi+4],ax" \
"add ebx,15" \
"shr ebx,4" \
"mov ax,0x100" \
"xor edx,edx" \
"int 0x31" \
"jb short L1" \
"mov [esi],ax" \
"mov [edi+4],dx" \
"xor ax,ax" \
"L1:" \
parm nomemory [ebx][esi][edi]

ushort dpmiFree(ushort segment);
#pragma aux dpmiFree = \
"mov ax,0x101" \
"int 0x31" \
"jb short L1" \
"xor ax,ax" \
"L1:" \
parm nomemory [dx]

DPMI_RMINFO RMI;

int dpmiInt(byte bl);
#pragma aux dpmiInt = \
"push es" \
"mov ax,seg RMI" \
"mov es,ax" \
"mov edi,offset RMI" \
"mov ax,0x300" \
"mov bh,0" \
"mov cx,0" \
"int 0x31" \
"pop es" \
"xor eax,eax" \
"adc eax,0" \
parm nomemory [bl]

void biosReadSector(uint a1, uint a2, uint a3, uint a4, byte *buffer)
{
    void far *mem;
    ushort segment;
    int result;
    dassert(buffer != 0, 357);
    result = dpmiAlloc(512, &segment, &mem);
    dassert(result == 0, 370);
    RMI.eax = 0x201;
    RMI.ecx = (a3 << 8) | a4;
    RMI.edx = (a2 << 8) | a1;
    RMI.ebx = 0;
    RMI.es = segment;
    result = dpmiInt(0x13);
    dassert(result == 0, 385);
    _fmemcpy(buffer, mem, 512);
    dpmiFree(segment);
}