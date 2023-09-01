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

.CODE
_gGetMode PROC
PUBLIC _gGetMode
	push	esi
	push	edi
	push	ebx
	mov		ah, 0Fh
	int		10h
	and		eax, 0FFh
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

_gSetMode PROC
PUBLIC _gSetMode
	enter	4, 0
	push	esi
	push	edi
	push	ebx
	mov		ah, 0Fh
	int		10h
	and		eax, 0FFh
	mov		[ebp-4], eax
	mov		eax, [ebp+8]
	test	eax, eax
	jz		short L1
	xor		ah, ah
	int		10h
L1:
	mov		eax, [ebp-4]
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn
ENDP

END