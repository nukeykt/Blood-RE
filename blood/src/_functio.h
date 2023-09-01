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
// _functio.h

// file created by makehead.exe
// these headers contain default key assignments, as well as
// default button assignments and game function names
// axis defaults are also included


#ifndef _function_private_
#define _function_private_
#ifdef __cplusplus
extern "C" {
#endif
char * gamefunctions[] =
   {
   "Move_Forward",
   "Move_Backward",
   "Turn_Left",
   "Turn_Right",
   "Turn_Around",
   "Strafe",
   "Strafe_Left",
   "Strafe_Right",
   "Jump",
   "Crouch",
   "Run",
   "AutoRun",
   "Open",
   "Weapon_Fire",
   "Weapon_Special_Fire",
   "Aim_Up",
   "Aim_Down",
   "Aim_Center",
   "Look_Up",
   "Look_Down",
   "Tilt_Left",
   "Tilt_Right",
   "Weapon_1",
   "Weapon_2",
   "Weapon_3",
   "Weapon_4",
   "Weapon_5",
   "Weapon_6",
   "Weapon_7",
   "Weapon_8",
   "Weapon_9",
   "Weapon_10",
   "Inventory_Use",
   "Inventory_Left",
   "Inventory_Right",
   "Map_Toggle",
   "Map_Follow_Mode",
   "Shrink_Screen",
   "Enlarge_Screen",
   "Send_Message",
   "See_Coop_View",
   "See_Chase_View",
   "Mouse_Aiming",
   "Toggle_Crosshair",
   "Next_Weapon",
   "Previous_Weapon",
   "Holster_Weapon",
   "Show_Opponents_Weapon",
   "BeastVision",
   "CrystalBall",
   "JumpBoots",
   "MedKit",
   "ProximityBombs",
   "RemoteBombs",
   };

#define NUMKEYENTRIES 54

static char * keydefaults[] =
   {
   "Move_Forward", "Up", "Kpad8",
   "Move_Backward", "Down", "Kpad2",
   "Turn_Left", "Left", "Kpad4",
   "Turn_Right", "Right", "KPad6",
   "Turn_Around", "BakSpc", "",
   "Strafe", "LAlt", "RAlt",
   "Strafe_Left", ",", "",
   "Strafe_Right", ".", "",
   "Jump", "A", "/",
   "Crouch", "Z", "",
   "Run", "LShift", "RShift",
   "AutoRun", "CapLck", "",
   "Open", "Space", "",
   "Weapon_Fire", "LCtrl", "RCtrl",
   "Weapon_Special_Fire", "X", "",
   "Aim_Up", "Home", "KPad7",
   "Aim_Down", "End", "Kpad1",
   "Aim_Center", "KPad5", "",
   "Look_Up",  "PgUp", "Kpad9",
   "Look_Down", "PgDn", "Kpad3",
   "Tilt_Left", "Insert", "Kpad0",
   "Tilt_Right", "Delete", "Kpad.",
   "Weapon_1", "1", "",
   "Weapon_2", "2", "",
   "Weapon_3", "3", "",
   "Weapon_4", "4", "",
   "Weapon_5", "5", "",
   "Weapon_6", "6", "",
   "Weapon_7", "7", "",
   "Weapon_8", "8", "",
   "Weapon_9", "9", "",
   "Weapon_10", "0", "",
   "Inventory_Use", "Enter", "KpdEnt",
   "Inventory_Left", "[", "",
   "Inventory_Right", "]", "",
   "Map_Toggle", "Tab", "",
   "Map_Follow_Mode", "F", "",
   "Shrink_Screen", "-", "Kpad-",
   "Enlarge_Screen", "=", "Kpad+",
   "Send_Message", "T", "",
   "See_Coop_View", "K", "",
   "See_Chase_View", "F7", "",
   "Mouse_Aiming", "U", "",
   "Toggle_Crosshair", "I", "",
   "Next_Weapon", "'", "",
   "Previous_Weapon", ";", "",
   "Holster_Weapon", "ScrLck", "",
   "Show_Opponents_Weapon", "W", "",
   "BeastVision", "B", "",
   "CrystalBall", "C", "",
   "JumpBoots", "J", "",
   "MedKit", "M", "",
   "ProximityBombs", "P", "",
   "RemoteBombs", "R", "",
   };


static char * mousedefaults[] =
   {
   "Weapon_Fire",
   "Move_Forward",
   "Weapon_Special_Fire",
   };


static char * mouseclickeddefaults[] =
   {
   "",
   "Open",
   "",
   };


static char * joystickdefaults[] =
   {
   "Weapon_Fire",
   "Strafe",
   "Run",
   "Open",
   "Aim_Down",
   "",
   "Aim_Up",
   "",
   };


static char * joystickclickeddefaults[] =
   {
   "",
   "Inventory_Use",
   "Jump",
   "Crouch",
   "",
   "",
   "",
   "",
   };


static char * mouseanalogdefaults[] =
   {
   "analog_turning",
   "analog_moving",
   };


static char * mousedigitaldefaults[] =
   {
   "",
   "",
   "",
   "",
   };


static char * gamepaddigitaldefaults[] =
   {
   "Turn_Left",
   "Turn_Right",
   "Move_Forward",
   "Move_Backward",
   };


static char * joystickanalogdefaults[] =
   {
   "analog_turning",
   "analog_moving",
   "analog_strafing",
   "",
   };


static char * joystickdigitaldefaults[] =
   {
   "",
   "",
   "",
   "",
   "",
   "",
   "Run",
   "",
   };
#ifdef __cplusplus
};
#endif
#endif
