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
// function.h

// file created by makehead.exe
// these headers contain default key assignments, as well as
// default button assignments and game function names
// axis defaults are also included


#ifndef _function_public_
#define _function_public_
#ifdef __cplusplus
extern "C" {
#endif

#define NUMGAMEFUNCTIONS 54

extern char * gamefunctions[];

enum
   {
   gamefunc_Move_Forward,
   gamefunc_Move_Backward,
   gamefunc_Turn_Left,
   gamefunc_Turn_Right,
   gamefunc_Turn_Around,
   gamefunc_Strafe,
   gamefunc_Strafe_Left,
   gamefunc_Strafe_Right,
   gamefunc_Jump,
   gamefunc_Crouch,
   gamefunc_Run,
   gamefunc_AutoRun,
   gamefunc_Open,
   gamefunc_Weapon_Fire,
   gamefunc_Weapon_Special_Fire,
   gamefunc_Aim_Up,
   gamefunc_Aim_Down,
   gamefunc_Aim_Center,
   gamefunc_Look_Up,
   gamefunc_Look_Down,
   gamefunc_Tilt_Left,
   gamefunc_Tilt_Right,
   gamefunc_Weapon_1,
   gamefunc_Weapon_2,
   gamefunc_Weapon_3,
   gamefunc_Weapon_4,
   gamefunc_Weapon_5,
   gamefunc_Weapon_6,
   gamefunc_Weapon_7,
   gamefunc_Weapon_8,
   gamefunc_Weapon_9,
   gamefunc_Weapon_10,
   gamefunc_Inventory_Use,
   gamefunc_Inventory_Left,
   gamefunc_Inventory_Right,
   gamefunc_Map_Toggle,
   gamefunc_Map_Follow_Mode,
   gamefunc_Shrink_Screen,
   gamefunc_Enlarge_Screen,
   gamefunc_Send_Message,
   gamefunc_See_Coop_View,
   gamefunc_See_Chase_View,
   gamefunc_Mouse_Aiming,
   gamefunc_Toggle_Crosshair,
   gamefunc_Next_Weapon,
   gamefunc_Previous_Weapon,
   gamefunc_Holster_Weapon,
   gamefunc_Show_Opponents_Weapon,
   gamefunc_BeastVision,
   gamefunc_CrystalBall,
   gamefunc_JumpBoots,
   gamefunc_MedKit,
   gamefunc_ProximityBombs,
   gamefunc_RemoteBombs,
   };
#ifdef __cplusplus
};
#endif
#endif
