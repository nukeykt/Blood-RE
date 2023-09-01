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
#ifndef _SOUND_H_
#define _SOUND_H_
#include "typedefs.h"
#include "resource.h"

struct SAMPLE2D
{
    int at0;
    byte at4;
    DICTNODE *at5;
}; // 9 bytes

struct SFX
{
    int relVol;
    int pitch;
    int pitchRange;
    int format;
    int loopStart;
    char rawName[9];
};

extern Resource gSoundRes;

int GetRate(int format);
void sndStopSong(void);
void sndKillSound(SAMPLE2D *pChannel);
void sndInit(BOOL netgame);
void sndPlaySong(char *songName, BOOL bLoop);
void sndStartSample(char *pzSound, int nVolume, int nChannel = -1);
void sndFadeSong(int nTime);
void sndStartWavID(ulong nSound, int nVolume, int nChannel);
void sndKillAllSounds(void);
void sndStartWavDisk(char *pzFile, int nVolume, int nChannel);
void sndSetMusicVolume(int nVolume);
void sndSetFXVolume(int nVolume);
void sndStartSample(ulong nSound, int nVolume, int nChannel = -1, BOOL bLoop = FALSE);
void sndProcess(void);
void sndTerm(void);

#endif // !_SOUND_H_
