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
#include <dos.h>
#include <stdlib.h>
#include "typedefs.h"
extern "C" {
#include "fx_man.h"
#include "music.h"
}
#include "types.h"
#include "config.h"
#include "debug4g.h"
#include "error.h"
#include "globals.h"
#include "misc.h"
#include "resource.h"
#include "sound.h"


BOOL sndActive;
BOOL bNetGame;

DICTNODE *hSong;
byte *pSong;
int songSize;

Resource gSoundRes;

int soundRates[13] = {
    11025,
    11025,
    11025,
    11025,
    11025,
    22050,
    22050,
    22050,
    22050,
    44100,
    44100,
    44100,
    44100,
};

#define kMaxChannels 32

int GetRate(int format)
{
    if (format < 13)
        return soundRates[format];
    return 11025;
}

SAMPLE2D Channel[kMaxChannels];

SAMPLE2D *FindChannel(void)
{
    for (int i = kMaxChannels-1; Channel[i].at5 != 0; i--)
        if (i < 0)
            ThrowError(74)("No free channel available for sample");
    return &Channel[i];
}

void sndPlaySong(char *songName, BOOL bLoop)
{
    if (MusicDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    if (*pSong)
        sndStopSong();
    if (!songName || strlen(songName) == 0)
        return;
    hSong = gSoundRes.Lookup(songName, "MID");
    if (!hSong)
        return;
    songSize = Resource::Size(hSong);
    if (songSize > 65535)
        return;
    gSoundRes.Load(hSong, pSong);
    MUSIC_SetVolume(MusicVolume);
    MUSIC_PlaySong(pSong, bLoop);
    // printf("%i", MusicVolume);
}

BOOL sndIsSongPlaying(void)
{
    if (MusicDevice != -1)
    {
        if (gUse8250 && bNetGame)
            return FALSE;
        return MUSIC_SongPlaying();
    }
    return FALSE;
}

void sndFadeSong(int nTime)
{
    if (MusicDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    MUSIC_FadeVolume(0, nTime);
}

void sndSetMusicVolume(int nVolume)
{
    if (MusicDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    MusicVolume = nVolume;
    MUSIC_SetVolume(nVolume);
}

void sndSetFXVolume(int nVolume)
{
    if (FXDevice == -1)
        return;
    FXVolume = nVolume;
    FX_SetVolume(nVolume);
}

void sndStopSong(void)
{
    if (MusicDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    if (!pSong)
        return;
    MUSIC_StopSong();
    hSong = NULL;
    *pSong = 0;
}

void SoundCallback(ulong val)
{
    *(int*)val = 0;
}

void sndStartSample(char *pzSound, int nVolume, int nChannel)
{
    if (FXDevice == -1)
        return;
    if (!strlen(pzSound))
        return;
    dassert(nChannel >= -1 && nChannel < kMaxChannels, 191);
    SAMPLE2D *pChannel;
    if (nChannel == -1)
        pChannel = FindChannel();
    else
        pChannel = &Channel[nChannel];
    if (pChannel->at0 > 0)
        sndKillSound(pChannel);
    pChannel->at5 = gSoundRes.Lookup(pzSound, "RAW");
    if (!pChannel->at5)
        return;
    int nSize = Resource::Size(pChannel->at5);
    char *pData = (char*)gSoundRes.Lock(pChannel->at5);
    pChannel->at0 = FX_PlayRaw(pData, nSize, GetRate(1), 0, nVolume, nVolume, nVolume, nVolume, (ulong)pChannel);
}

void sndStartSample(ulong nSound, int nVolume, int nChannel, BOOL bLoop)
{
    if (FXDevice == -1)
        return;
    dassert(nChannel >= -1 && nChannel < kMaxChannels, 230);
    DICTNODE *hSfx = gSoundRes.Lookup(nSound, "SFX");
    if (!hSfx)
        return;
    SFX *pEffect = (SFX*)gSoundRes.Lock(hSfx);
    dassert(pEffect != NULL, 242);
    SAMPLE2D *pChannel;
    if (nChannel == -1)
        pChannel = FindChannel();
    else
        pChannel = &Channel[nChannel];
    if (pChannel->at0 > 0)
        sndKillSound(pChannel);
    pChannel->at5 = gSoundRes.Lookup(pEffect->rawName, "RAW");
    if (!pChannel->at5)
        return;
    if (nVolume < 0)
        nVolume = pEffect->relVol;
    nVolume *= 80;
    int nSize = Resource::Size(pChannel->at5);
    int nLoopEnd = ClipLow(nSize-1, 0);
    if (nSize <= 0)
        return;
    char *pData = (char*)gSoundRes.Lock(pChannel->at5);
    if (nChannel < 0)
        bLoop = FALSE;
    if (bLoop)
    {
        int nLoopStart = pEffect->loopStart;
        int rate = GetRate(pEffect->format);
        pChannel->at0 = FX_PlayLoopedRaw(pData, nSize, pData+nLoopStart, pData+nLoopEnd, rate,
            0, nVolume, nVolume, nVolume, nVolume, (ulong)pChannel);
        pChannel->at4 |= 1;
    }
    else
    {
        pChannel->at0 = FX_PlayRaw(pData, nSize, GetRate(pEffect->format), 0, nVolume, nVolume, nVolume, nVolume, (ulong)pChannel);
        pChannel->at4 &= ~1;
    }
}

void sndStartWavID(ulong nSound, int nVolume, int nChannel)
{
    if (FXDevice == -1)
        return;
    dassert(nChannel >= -1 && nChannel < kMaxChannels, 332);
    SAMPLE2D *pChannel;
    if (nChannel == -1)
        pChannel = FindChannel();
    else
        pChannel = &Channel[nChannel];
    if (pChannel->at0 > 0)
        sndKillSound(pChannel);
    pChannel->at5 = gSoundRes.Lookup(nSound, "WAV");
    if (!pChannel->at5)
        return;
    char *pData = (char*)gSoundRes.Lock(pChannel->at5);
    pChannel->at0 = FX_PlayWAV(pData, 0, nVolume, nVolume, nVolume, nVolume, (ulong)pChannel);
}

void sndKillSound(SAMPLE2D *pChannel)
{
    if (pChannel->at4 & 1)
    {
        FX_EndLooping(pChannel->at0);
        pChannel->at4 &= ~1;
    }
    FX_StopSound(pChannel->at0);
}

void sndStartWavDisk(char *pzFile, int nVolume, int nChannel)
{
    if (FXDevice == -1)
        return;
    dassert(nChannel >= -1 && nChannel < kMaxChannels, 378);
    SAMPLE2D *pChannel;
    if (nChannel == -1)
        pChannel = FindChannel();
    else
        pChannel = &Channel[nChannel];
    if (pChannel->at0 > 0)
        sndKillSound(pChannel);
    struct find_t dosFind;
    if (_dos_findfirst(pzFile, NULL, &dosFind) != 0)
        return;
    char *pData = (char*)gSoundRes.Alloc(dosFind.size);
    if (!pData)
        return;
    if (!FileLoad(pzFile, pData, dosFind.size))
        return;
    pChannel->at5 = (DICTNODE*)pData;
    pChannel->at4 |= 2;
    pChannel->at0 = FX_PlayWAV(pData, 0, nVolume, nVolume, nVolume, nVolume, (ulong)pChannel);
}

void sndKillAllSounds(void)
{
    if (FXDevice == -1)
        return;
    for (int i = 0; i < kMaxChannels; i++)
    {
        SAMPLE2D *pChannel = &Channel[i];
        if (pChannel->at0 > 0)
            sndKillSound(pChannel);
        if (pChannel->at5)
        {
            if (pChannel->at4 & 2)
            {
                free(pChannel->at5); // BUG
                pChannel->at4 &= ~2;
            }
            else
            {
                gSoundRes.Unlock(pChannel->at5);
            }
            pChannel->at5 = 0;
        }
    }
}

void sndProcess(void)
{
    if (FXDevice == -1)
        return;
    for (int i = 0; i < kMaxChannels; i++)
    {
        if (Channel[i].at0 <= 0 && Channel[i].at5)
        {
            if (Channel[i].at4 & 2)
            {
                gSoundRes.Free(Channel[i].at5);
            }
            else
            {
                gSoundRes.Unlock(Channel[i].at5);
            }
            Channel[i].at5 = 0;
        }
    }
}

void InitSoundDevice(void)
{
    if (FXDevice == -1)
        return;
    int nMaxVoices;
    int nMaxBits;
    int nMaxChannels;
    fx_device sDevice;
    int nStatus;
    if (FXDevice == 0 || FXDevice == 6)
        nStatus = FX_SetupSoundBlaster(SBConfig, &nMaxVoices, &nMaxBits, &nMaxChannels);
    else
        nStatus = FX_SetupCard(FXDevice, &sDevice);
    if (nStatus != 0)
        ThrowError(508)(FX_ErrorString(nStatus));
    if (gUse8250 && bNetGame)
        nStatus = FX_Init(FXDevice, ClipHigh(NumVoices, 4), 1, 8, 8000);
    else
        nStatus = FX_Init(FXDevice, NumVoices, NumChannels, NumBits, MixRate);
    if (nStatus != 0)
        ThrowError(516)(FX_ErrorString(nStatus));
    FX_SetVolume(FXVolume);
    if (ReverseStereo == 1)
        FX_SetReverseStereo(!FX_GetReverseStereo());
    nStatus = FX_SetCallBack(SoundCallback);
    if (nStatus != 0)
        ThrowError(527)(FX_ErrorString(nStatus));
}

void TermSoundDevice(void)
{
    if (FXDevice == -1)
        return;
    int nStatus = FX_Shutdown();
    if (nStatus != 0)
        ThrowError(545)(FX_ErrorString(nStatus));
}

void InitMusicDevice(void)
{
    if (MusicDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    int nMaxVoices;
    int nMaxBits;
    int nMaxChannels;
    SBConfig.Midi = MidiPort;
    if (FXDevice == 0 || FXDevice == 6)
        FX_SetupSoundBlaster(SBConfig, &nMaxVoices, &nMaxBits, &nMaxChannels);
    if (MusicDevice == 6)
        MusicDevice = 0;
    int nStatus = MUSIC_Init(MusicDevice, MidiPort);
    if (nStatus != 0)
        ThrowError(464)(MUSIC_ErrorString(nStatus));
    DICTNODE *hTmb = gSoundRes.Lookup("GMTIMBRE", "TMB");
    if (hTmb)
        MUSIC_RegisterTimbreBank((byte*)gSoundRes.Load(hTmb));
    MUSIC_SetVolume(MusicVolume);
}

void TermMusicDevice(void)
{
    if (FXDevice == -1)
        return;
    if (gUse8250 && bNetGame)
        return;
    FX_StopAllSounds();
    int nStatus = MUSIC_Shutdown();
    if (nStatus != 0)
        ThrowError(486)(MUSIC_ErrorString(MUSIC_ErrorCode));
}

void sndTerm(void)
{
    if (!sndActive)
        return;
    sndActive = FALSE;
    sndStopSong();
    Resource::Free(pSong);
    TermSoundDevice();
    TermMusicDevice();
}

extern char *szSoundRes;

void sndInit(BOOL netgame)
{
    bNetGame = netgame;
    char *res = szSoundRes;
    if (!res)
        res = "SOUNDS.RFF";
    gSoundRes.Init(res, NULL);
    memset(Channel, 0, sizeof(Channel));
    pSong = (byte*)Resource::Alloc(65535);
    if (pSong)
        *pSong = 0;
    InitMusicDevice();
    InitSoundDevice();
    atexit(sndTerm);
    sndActive = TRUE;
}
