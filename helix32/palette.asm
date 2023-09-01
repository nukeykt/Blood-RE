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
_gSetDACRange PROC
PUBLIC _gSetDACRange
	enter	0, 0
	push	ebx
	mov		ecx, [ebp+0Ch]
	mov		ebx, [ebp+10h]
	mov		dx, 3C8H
	mov		al, [ebp+8]
	out		dx, al
	mov		dx, 3C9H
L1:
	mov		al, [ebx]
	shr		al, 2
	inc		ebx
	out		dx, al
	mov		al, [ebx]
	shr		al, 2
	inc		ebx
	out		dx, al
	mov		al, [ebx]
	shr		al, 2
	inc		ebx
	out		dx, al
	dec		ecx
	jnz		short L1
	pop		ebx
	leave
	retn
ENDP

_gSetDAC PROC
PUBLIC _gSetDAC
	enter	0, 0
	push	ebx
	mov		dx, 3C8H
	mov		al, [ebp+8]
	out		dx, al
	mov		dx, 3C9H
	mov		al, [ebp+0Ch]
	shr		al, 2
	out		dx, al
	mov		al, [ebp+10h]
	shr		al, 2
	out		dx, al
	mov		al, [ebp+14h]
	shr		al, 2
	out		dx, al
	pop		ebx
	leave
	retn
ENDP

END
