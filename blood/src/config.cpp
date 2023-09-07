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
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include "typedefs.h"
#include "types.h"
#include "config.h"
#include "control.h"
#include "file_lib.h"
#include "function.h"
#include "gamedefs.h"
#include "globals.h"
#include "scriplib.h"
#include "util_lib.h"

#include "_functio.h"

//
// Sound variables
//
int32 FXDevice;
int32 MusicDevice;
int32 FXVolume;
int32 MusicVolume;
int32 CDVolume = 255;
fx_blaster_config SBConfig;
int32 NumVoices;
int32 NumChannels;
int32 NumBits;
int32 MixRate;
int32 MidiPort;
int32 ReverseStereo;
controltype ControllerType;
int32 gMouseSensitivity;
int32 gMouseAiming;
BOOL gMouseAimingFlipped;
int32 gTurnSpeed = 92;

//
// Screen variables
//
int32 ScreenMode;
int32 ScreenWidth;
int32 ScreenHeight;

char SetupFilename[128] = {SETUPFILENAME};
static int32 scripthandle; // BUG, CONFIG_ReadSetup uses local variable

BOOL gViewInterpolate = TRUE;
BOOL gViewHBobbing = TRUE;
BOOL gViewVBobbing = TRUE;
BOOL gFollowMap = TRUE;
BOOL gAutoAim = TRUE;
BOOL gAutoRun = TRUE;
BOOL gSlopeTilting = TRUE;
BOOL gOverlayMap;
BOOL gRotateMap;
BOOL gInfiniteAmmo;
BOOL gFullMap;
BOOL gNoClip;
BOOL gAimReticle;
BOOL gShowWeapon;
BOOL gMouseAim;

BOOL gMessageState = TRUE;
int32 gMessageFont;
int32 gMessageCount = 4;
int32 gMessageTime = 5;
int32 gViewSize = 1;
int32 gDetail = 4;
int32 gDifficulty = 2;
int32 gSkill = 2;
BOOL gDoppler = TRUE;
BOOL gbAdultContent;
char gzAdultPassword[12];

int32 ComPort;
int32 IrqNumber;
int32 UartAddress;
int32 PortSpeed;
int32 ToneDial;

char ModemName[50];
char InitString[50];
char HangupString[50];
char DialoutString[50];

int32 SocketNumber;

char CommbatMacro[10][34];
char PhoneNames[16][16];
char PhoneNumbers[16][28];
char PhoneNumber[28];
char PlayerName[12];
char RTSName[16];
char UserLevel[16];
char RTSPath[48];
char UserPath[48];


void SCRIPT_GetBool(int32 scripthandle, char *section, char *entryname, BOOL *bool)
{
    boolean temp = *bool;
    SCRIPT_GetBoolean(scripthandle, section, entryname, &temp);
    *bool = temp;
}
/*
===================
=
= CONFIG_GetSetupFilename
=
===================
*/
#define MAXSETUPFILES 20
void CONFIG_GetSetupFilename( void )
   {
   struct find_t fblock;
   char extension[8];
   char * src;
   char * filenames[MAXSETUPFILES];
   int32 numfiles;
   int32 i;
   char buffer[144];

   strcpy(SetupFilename,SETUPFILENAME);

   // determine extension

   _splitpath(SetupFilename, NULL, NULL, NULL, extension);
   _makepath(buffer, NULL, NULL, "*", extension);

   numfiles=0;
   if (_dos_findfirst(buffer,0,&fblock)==0)
      {
      do
         {
         filenames[numfiles]=(char*)SafeMalloc(128);
         strcpy(filenames[numfiles],fblock.name);
         numfiles++;
         if (numfiles == MAXSETUPFILES)
            break;
         }
      while(!_dos_findnext(&fblock));
      }
   if (numfiles>1)
      {
      int32 time;
      int32 oldtime;
      int32 count;

      printf("\nMultiple Configuration Files Encountered\n");
      printf("========================================\n\n");
      printf("Please choose a configuration file from the following list by pressing its\n");
      printf("corresponding letter:\n\n");
      for (i=0;i<numfiles;i++)
         {
         if (strcmpi(filenames[i],SETUPFILENAME))
            {
            printf("%c. %s\n",'a'+i,filenames[i]);
            }
         else
            {
            printf("%c. %s <DEFAULT>\n",'a'+i,filenames[i]);
            }
         }
      printf("\n");
      printf("(%s will be used if no selection is made within 10 seconds.)\n\n",SETUPFILENAME);
      count = 9;
      oldtime = clock();
      time=clock()+(10*CLOCKS_PER_SEC);
      while (clock()<time)
         {
         if (clock()>oldtime)
            {
            printf("%ld seconds left. \r",count);
            fflush(stdout);
            oldtime = clock()+CLOCKS_PER_SEC;
            count--;
            }
         if (kbhit())
            {
            int32 ch = getch();
            if (ch == '\r')
                break;
            ch -='a';
            if (ch>=0 && ch<numfiles)
               {
               strcpy (SetupFilename, filenames[ch]);
               break;
               }
            }
         }
      printf("\n\n");
      }
   if (numfiles==1)
      strcpy (SetupFilename, filenames[0]);
   printf("Using Setup file: %s\n",SetupFilename);
   i=clock()+(3*CLOCKS_PER_SEC/4);
   while (clock()<i)
      {
      ;
      }
   for (i=0;i<numfiles;i++)
      {
      SafeFree(filenames[i]);
      }
   }

/*
===================
=
= CONFIG_FunctionNameToNum
=
===================
*/

int32 CONFIG_FunctionNameToNum( char * func )
   {
   int32 i;

   for (i=0;i<NUMGAMEFUNCTIONS;i++)
      {
      if (!stricmp(func,gamefunctions[i]))
         {
         return i;
         }
      }
   return -1;
   }

/*
===================
=
= CONFIG_FunctionNumToName
=
===================
*/

char * CONFIG_FunctionNumToName( int32 func )
   {
   if (func < NUMGAMEFUNCTIONS)
      {
      return gamefunctions[func];
      }
   else
      {
      return NULL;
      }
   }

/*
===================
=
= CONFIG_AnalogNameToNum
=
===================
*/


int32 CONFIG_AnalogNameToNum( char * func )
   {

   if (!stricmp(func,"analog_turning"))
      {
      return analog_turning;
      }
   if (!stricmp(func,"analog_strafing"))
      {
      return analog_strafing;
      }
   if (!stricmp(func,"analog_moving"))
      {
      return analog_moving;
      }
   if (!stricmp(func,"analog_lookingupanddown"))
      {
      return analog_lookingupanddown;
      }

   return -1;
   }


void CONFIG_ResetKeys( void )
   {
   int32 i;
   int32 numkeyentries;
   kb_scancode key1,key2;
   numkeyentries = NUMKEYENTRIES;

   for (i=0;i<numkeyentries;i++)
      {
      key1 = (byte) KB_StringToScanCode( keydefaults[i*3+1] );
      key2 = (byte) KB_StringToScanCode( keydefaults[i*3+2] );
      CONTROL_MapKey( i, key1, key2 );
      }
   }

/*
===================
=
= CONFIG_ReadKeys
=
===================
*/

void CONFIG_ReadKeys( int32 scripthandle )
   {
   int32 i;
   int32 numkeyentries;
   int32 function;
   char keyname1[80];
   char keyname2[80];
   kb_scancode key1,key2;

   numkeyentries = SCRIPT_NumberEntries( scripthandle, "KeyDefinitions" );

   for (i=0;i<numkeyentries;i++)
      {
      function = CONFIG_FunctionNameToNum(SCRIPT_Entry( scripthandle, "KeyDefinitions", i ));
      if (function != -1)
         {
         memset(keyname1,0,sizeof(keyname1));
         memset(keyname2,0,sizeof(keyname2));
         SCRIPT_GetDoubleString
            (
            scripthandle,
            "KeyDefinitions",
            SCRIPT_Entry( scripthandle,"KeyDefinitions", i ),
            keyname1,
            keyname2
            );
         key1 = 0;
         key2 = 0;
         if (keyname1[0])
            {
            key1 = (byte) KB_StringToScanCode( keyname1 );
            }
         if (keyname2[0])
            {
            key2 = (byte) KB_StringToScanCode( keyname2 );
            }
         CONTROL_MapKey( function, key1, key2 );
         }
      }
   }

void CONFIG_WriteKeys( int32 scripthandle )
   {
   int32 i;
   char *functionname;
   char *keyname1;
   char *keyname2;
   int32 key1,key2;

   for (i=0;i<NUMKEYENTRIES;i++)
      {
      functionname = CONFIG_FunctionNumToName( i );
      CONTROL_GetKeyMap( i, &key1, &key2 );
      keyname1 = KB_ScanCodeToString( key1 );
      keyname2 = KB_ScanCodeToString( key2 );
      SCRIPT_PutDoubleString
         (
         scripthandle,
         "KeyDefinitions",
         functionname,
         keyname1,
         keyname2
         );
      }
   }

void CONFIG_UpdateKeys( void )
   {
   CONFIG_WriteKeys( scripthandle );
   }

/*
===================
=
= CONFIG_SetupMouse
=
===================
*/

void CONFIG_SetupMouse( int32 scripthandle )
   {
   int32 i;
   char str[80];
   char temp[80];
   int32 function, scale;
   int32 var;

   for (i=0;i<MAXMOUSEBUTTONS;i++)
      {
      sprintf(str,"MouseButton%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, false );
      sprintf(str,"MouseButtonClicked%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, true );
      }
   // map over the axes
   for (i=0;i<MAXMOUSEAXES;i++)
      {
      sprintf(str,"MouseAnalogAxes%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_AnalogNameToNum(temp);
      if (function != -1)
         {
         CONTROL_MapAnalogAxis(i,function);
         }
      sprintf(str,"MouseDigitalAxes%ld_0",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 0 );
      sprintf(str,"MouseDigitalAxes%ld_1",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 1 );
      sprintf(str,"MouseAnalogScale%ld",i);
      SCRIPT_GetNumber(scripthandle, "Controls", str,&scale);
      CONTROL_SetAnalogAxisScale( i, scale );
      }

   SCRIPT_GetNumber( scripthandle, "Controls","MouseSensitivity",&gMouseSensitivity);
   CONTROL_SetMouseSensitivity(gMouseSensitivity);
   SCRIPT_GetNumber( scripthandle, "Controls","MouseAiming",&gMouseAiming);
   SCRIPT_GetNumber( scripthandle, "Controls","MouseAimingFlipped",&var);
   gMouseAimingFlipped = var;
   if (ControllerType == controltype_keyboardandexternal) // always flip for bmouse
      gMouseAimingFlipped = !gMouseAimingFlipped;
   }

/*
===================
=
= CONFIG_SetupGamePad
=
===================
*/

void CONFIG_SetupGamePad( int32 scripthandle )
   {
   int32 i;
   char str[80];
   char temp[80];
   int32 function;


   for (i=0;i<MAXJOYBUTTONS;i++)
      {
      sprintf(str,"JoystickButton%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, false );
      sprintf(str,"JoystickButtonClicked%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, true );
      }
   // map over the axes
   for (i=0;i<MAXGAMEPADAXES;i++)
      {
      sprintf(str,"GamePadDigitalAxes%ld_0",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 0 );
      sprintf(str,"GamePadDigitalAxes%ld_1",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 1 );
      }
   SCRIPT_GetNumber( scripthandle, "Controls","JoystickPort",&function);
   CONTROL_JoystickPort = function;
   }

/*
===================
=
= CONFIG_SetupJoystick
=
===================
*/

void CONFIG_SetupJoystick( int32 scripthandle )
   {
   int32 i;
   char str[80];
   char temp[80];
   int32 function, scale;

   for (i=0;i<MAXJOYBUTTONS;i++)
      {
      sprintf(str,"JoystickButton%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, false );
      sprintf(str,"JoystickButtonClicked%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString( scripthandle,"Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapButton( function, i, true );
      }
   // map over the axes
   for (i=0;i<MAXJOYAXES;i++)
      {
      sprintf(str,"JoystickAnalogAxes%ld",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_AnalogNameToNum(temp);
      if (function != -1)
         {
         CONTROL_MapAnalogAxis(i,function);
         }
      sprintf(str,"JoystickDigitalAxes%ld_0",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 0 );
      sprintf(str,"JoystickDigitalAxes%ld_1",i);
      memset(temp,0,sizeof(temp));
      SCRIPT_GetString(scripthandle, "Controls", str,temp);
      function = CONFIG_FunctionNameToNum(temp);
      if (function != -1)
         CONTROL_MapDigitalAxis( i, function, 1 );
      sprintf(str,"JoystickAnalogScale%ld",i);
      SCRIPT_GetNumber(scripthandle, "Controls", str,&scale);
      CONTROL_SetAnalogAxisScale( i, scale );
      }
   // read in JoystickPort
   SCRIPT_GetNumber( scripthandle, "Controls","JoystickPort",&function);
   CONTROL_JoystickPort = function;
   // read in rudder state
   SCRIPT_GetNumber( scripthandle, "Controls","EnableRudder",&CONTROL_RudderEnabled);
   }

/*
===================
=
= CONFIG_ReadSetup
=
===================
*/

void CONFIG_ReadSetup( void )
{
   int32 dummy, i;
   char commmacro[80];

   CONTROL_ClearAssignments();

   if (!SafeFileExists(SetupFilename))
      {
      Error("ReadSetup: %s does not exist\n"
            "           Please run SETUP.EXE\n",SetupFilename);
      }
   
   int32 scripthandle = SCRIPT_Load( SetupFilename );

   SCRIPT_GetNumber( scripthandle, "Screen Setup", "ScreenMode",&ScreenMode);
   SCRIPT_GetNumber( scripthandle, "Screen Setup", "ScreenWidth",&ScreenWidth);
   SCRIPT_GetNumber( scripthandle, "Screen Setup", "ScreenHeight",&ScreenHeight);
   SCRIPT_GetNumber( scripthandle, "Screen Setup", "Size",&gViewSize);
   SCRIPT_GetNumber( scripthandle, "Screen Setup", "Gamma",&gGamma);
   
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "FXDevice",&FXDevice);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "MusicDevice",&MusicDevice);
   if (FXDevice == NumSoundCards)
      FXDevice = -1;
   if (MusicDevice == NumSoundCards)
      MusicDevice = -1;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "FXVolume",&FXVolume);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "MusicVolume",&MusicVolume);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "CDVolume",&CDVolume);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "NumVoices",&NumVoices);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "NumChannels",&NumChannels);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "NumBits",&NumBits);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "MixRate",&MixRate);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "MidiPort",&MidiPort);
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterAddress",&dummy);
   SBConfig.Address = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterType",&dummy);
   SBConfig.Type = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterInterrupt",&dummy);
   SBConfig.Interrupt = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterDma8",&dummy);
   SBConfig.Dma8 = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterDma16",&dummy);
   SBConfig.Dma16 = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "BlasterEmu",&dummy);
   SBConfig.Emu = dummy;
   SCRIPT_GetNumber( scripthandle, "Sound Setup", "ReverseStereo",&ReverseStereo);

   SCRIPT_GetNumber( scripthandle, "Controls","ControllerType",(int32*)&ControllerType);
   SCRIPT_GetNumber( scripthandle, "Controls","TurnSpeed",&gTurnSpeed);

   SCRIPT_GetString( scripthandle, "Comm Setup","PlayerName",&PlayerName[0]);
   SCRIPT_GetString( scripthandle, "Comm Setup","RTSName",&RTSName[0]);
   SCRIPT_GetString( scripthandle, "Comm Setup","RTSPath",&RTSPath[0]);

   if (!UserPath[0])
      SCRIPT_GetString( scripthandle, "Comm Setup","UserPath",&UserPath[0]);

   for(i = 0;i < 10;i++)
   {
       sprintf(commmacro, "CommbatMacro#%ld", i);
       SCRIPT_GetString( scripthandle, "Comm Setup",commmacro,&CommbatMacro[i][0]);
   }

   CONTROL_ClearAssignments();

   CONFIG_ReadKeys( scripthandle );

   switch (ControllerType)
      {
      case controltype_keyboardandmouse:
         CONFIG_SetupMouse(scripthandle);
         break;
      default:
         CONFIG_SetupMouse(scripthandle);
         break;
      case controltype_keyboardandjoystick:
      case controltype_keyboardandflightstick:
      case controltype_keyboardandthrustmaster:
         CONFIG_SetupJoystick(scripthandle);
         break;
      case controltype_keyboardandgamepad:
         CONFIG_SetupGamePad(scripthandle);
         break;

      }
   
   SCRIPT_GetNumber( scripthandle, "Options","Detail",&gDetail);
   SCRIPT_GetBool( scripthandle, "Options","MouseAim",&gMouseAim);
   SCRIPT_GetBool( scripthandle, "Options","AutoRun",&gAutoRun);
   SCRIPT_GetBool( scripthandle, "Options","Interpolation",&gViewInterpolate);
   SCRIPT_GetBool( scripthandle, "Options","ViewHBobbing",&gViewHBobbing);
   SCRIPT_GetBool( scripthandle, "Options","ViewVBobbing",&gViewVBobbing);
   SCRIPT_GetBool( scripthandle, "Options","FollowMap",&gFollowMap);
   SCRIPT_GetBool( scripthandle, "Options","OverlayMap",&gOverlayMap);
   SCRIPT_GetBool( scripthandle, "Options","RotateMap",&gRotateMap);
   SCRIPT_GetBool( scripthandle, "Options","AimReticle",&gAimReticle);
   SCRIPT_GetBool( scripthandle, "Options","SlopeTilting",&gSlopeTilting);
   SCRIPT_GetBool( scripthandle, "Options","MessageState",&gMessageState);
   SCRIPT_GetNumber( scripthandle, "Options","MessageCount",&gMessageCount);
   SCRIPT_GetNumber( scripthandle, "Options","MessageTime",&gMessageTime);
   SCRIPT_GetNumber( scripthandle, "Options","MessageFont",&gMessageFont);
   SCRIPT_GetBool( scripthandle, "Options","AdultContent",&gbAdultContent);
   SCRIPT_GetString( scripthandle, "Options","AdultPassword",&gzAdultPassword[0]);
   SCRIPT_GetBool( scripthandle, "Options","Doppler",&gDoppler);
   SCRIPT_GetBool( scripthandle, "Options","ShowWeapon",&gShowWeapon);

   }

/*
===================
=
= CONFIG_WriteSetup
=
===================
*/

void CONFIG_WriteSetup( void )
   {
   SCRIPT_PutNumber( scripthandle, "Screen Setup", "Size",gViewSize,false,false);
   SCRIPT_PutNumber( scripthandle, "Screen Setup", "Gamma",gGamma,false,false);
   SCRIPT_PutNumber( scripthandle, "Sound Setup", "FXVolume",FXVolume,false,false);
   SCRIPT_PutNumber( scripthandle, "Sound Setup", "MusicVolume",MusicVolume,false,false);
   SCRIPT_PutNumber( scripthandle, "Sound Setup", "CDVolume",MusicVolume,false,false);

   switch (ControllerType)
      {
      case controltype_keyboardandmouse:
         SCRIPT_PutNumber( scripthandle, "Controls","MouseSensitivity", gMouseSensitivity,false,false);
         SCRIPT_PutNumber( scripthandle, "Controls","MouseAiming", gMouseAiming,false,false);
         SCRIPT_PutNumber( scripthandle, "Controls","MouseAimingFlipped", gMouseAimingFlipped,false,false);
         break;
      }

   SCRIPT_PutNumber( scripthandle, "Controls","TurnSpeed", gTurnSpeed,false,false);
   SCRIPT_PutNumber( scripthandle, "Options","Detail", gDetail,false,false);
   SCRIPT_PutBoolean( scripthandle, "Options","MouseAim", gMouseAim);
   SCRIPT_PutBoolean( scripthandle, "Options","AutoRun", gAutoRun);
   SCRIPT_PutBoolean( scripthandle, "Options","Interpolation", gViewInterpolate);
   SCRIPT_PutBoolean( scripthandle, "Options","ViewHBobbing", gViewHBobbing);
   SCRIPT_PutBoolean( scripthandle, "Options","ViewVBobbing", gViewVBobbing);
   SCRIPT_PutBoolean( scripthandle, "Options","FollowMap", gFollowMap);
   SCRIPT_PutBoolean( scripthandle, "Options","OverlayMap", gOverlayMap);
   SCRIPT_PutBoolean( scripthandle, "Options","RotateMap", gRotateMap);
   SCRIPT_PutBoolean( scripthandle, "Options","AimReticle", gAimReticle);
   SCRIPT_PutBoolean( scripthandle, "Options","SlopeTilting", gSlopeTilting);
   SCRIPT_PutBoolean( scripthandle, "Options","MessageState", gMessageState);
   SCRIPT_PutNumber( scripthandle, "Options","MessageCount", gMessageCount,false,false);
   SCRIPT_PutNumber( scripthandle, "Options","MessageTime", gMessageTime,false,false);
   SCRIPT_PutNumber( scripthandle, "Options","MessageFont", gMessageFont,false,false);
   SCRIPT_PutBoolean( scripthandle, "Options","AdultContent", gbAdultContent);
   SCRIPT_PutString( scripthandle, "Options","AdultPassword",gzAdultPassword);
   SCRIPT_PutBoolean( scripthandle, "Options","Doppler", gDoppler);
   SCRIPT_PutBoolean( scripthandle, "Options","ShowWeapon", gShowWeapon);

   CONFIG_WriteKeys( scripthandle );

   SCRIPT_Save (scripthandle, SetupFilename);
   SCRIPT_Free (scripthandle);
   }

void CONFIG_WriteAdultMode( void )
   {
   SCRIPT_PutBoolean( scripthandle, "Options","AdultContent", gbAdultContent);
   SCRIPT_PutString( scripthandle, "Options","AdultPassword",gzAdultPassword);

   SCRIPT_Save (scripthandle, SetupFilename);
   }
