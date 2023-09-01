;
; Copyright (C) 2018, 2022 nukeykt
;
; This file is part of Blood-RE.
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
.386p
.MODEL SMALL

VGTGROUP	GROUP VGTBEGIN, VGT, VGTEND

VGTBEGIN SEGMENT DWORD PUBLIC USE32 'DLL'
PUBLIC _VGTBegin
LABEL _VGTBegin
VGTBEGIN ENDS

VGT SEGMENT DWORD PUBLIC USE32 'DLL'
VGT ENDS

VGTEND SEGMENT DWORD PUBLIC USE32 'DLL'

PUBLIC _VGTEnd
LABEL _VGTEnd

VGTEND ENDS

END
