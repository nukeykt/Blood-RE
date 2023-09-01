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
#ifndef _config_public
#define _config_public

#include "types.h"
extern "C" {
#include "fx_man.h"
}
#include "control.h"

#define SETUPNAMEPARM "SETUPFILE"

extern int32 MusicDevice;
extern int32 MusicVolume;
extern int32 FXVolume;
extern int32 FXDevice;
extern int32 CDVolume;
extern int32 NumVoices;
extern int32 ReverseStereo;
extern int32 MidiPort;
extern int32 NumChannels;
extern int32 NumBits;
extern int32 MixRate;
extern int32 gMouseSensitivity;
extern int32 ScreenMode;
extern int32 ScreenHeight;
extern int32 ScreenWidth;
extern fx_blaster_config SBConfig;
extern BOOL gDoppler;
extern int32 gViewSize;
extern controltype ControllerType;
extern BOOL gFollowMap;
extern BOOL gShowWeapon;
extern int32 gMouseAiming;
extern BOOL gMouseAim;
extern BOOL gMouseAimingFlipped;
extern BOOL gAutoRun;
extern int32 gTurnSpeed;
extern BOOL gAimReticle;
extern BOOL gbAdultContent;
extern BOOL gNoClip;
extern BOOL gMessageState;
extern int32 gMessageFont;
extern int32 gMessageCount;
extern int32 gMessageTime;
extern int32 gDetail;
extern int32 gSkill;
extern BOOL gViewInterpolate;
extern BOOL gShowWeapon;
extern BOOL gViewHBobbing;
extern BOOL gViewVBobbing;
extern BOOL gSlopeTilting;
extern BOOL gOverlayMap;
extern char gzAdultPassword[12];
extern BOOL gFullMap;
extern BOOL gInfiniteAmmo;
extern BOOL gAutoAim;

extern char CommbatMacro[10][34];

extern char UserPath[];

extern char PlayerName[];

extern char SetupFilename[128];

void CONFIG_GetSetupFilename( void );
void CONFIG_ReadSetup( void );
char * CONFIG_FunctionNumToName( int32 func );
void CONFIG_WriteAdultMode( void );
void CONFIG_ResetKeys( void );
void CONFIG_WriteSetup( void );

#endif
