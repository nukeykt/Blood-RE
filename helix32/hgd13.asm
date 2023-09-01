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

VGTBEGIN SEGMENT DWORD PUBLIC USE32 'DLL'
VGTBEGIN ENDS

VGT SEGMENT DWORD PUBLIC USE32 'DLL'
dd offset MCGADriver
VGT ENDS

VGTEND SEGMENT DWORD PUBLIC USE32 'DLL'
VGTEND ENDS

.DATA

EXTRN _gPages:DWORD
EXTRN _gPageTable:DWORD
EXTRN _gYLookup:DWORD
EXTRN _gColor:DWORD
EXTRN _gROP:DWORD

MCGADriver	dd offset MCGAValid
			dd offset MCGAName
			dd 320
			dd 200
			dd 8
			dd 3
			dd offset MCGAInit
			dd offset MCGASetMode
			dd offset MCGASetPage
			dd offset MCGAGetPage
			dd offset MCGAClear
			dd offset MCGASetPixel
			dd offset MCGAGetPixel
			dd offset MCGAHLine
			dd offset MCGAVLine
			dd offset MCGALine
			dd offset MCGAFillBox
			dd offset MCGAHLineROP
			dd offset MCGAVLineROP
			dd offset MCGABlitRLE2V
			dd offset MCGABlitV2M
			dd offset MCGABlitM2V
			dd offset MCGABlitMT2V
			dd offset MCGABlitMono
MCGAName	db 'MCGA 320x200 256 Color', 0
MCGAPageTable	dd 1
				dd 0A0000h
				dd 0FA00h
				dd 320
				dd 200
				dd 320
				dd 0, 0
				dd 0
				dd 0AFA00h
				dd 500h
				dd 320
				dd 4
				dd 320
				dd 0, 0
MCGAPage		dd 0

				db 90h

LINEVAR1		dd 0
				dd 0
LINEVAR2		dd 0
				dd 0

linecalldefine MACRO number
	dd linecall_&number
ENDM

linecall LABEL
INDEX = 0
REPT 64
linecalldefine %INDEX
INDEX = INDEX + 1
ENDM

byte_1185D4		db 88h
				db 20h
				db 08h
				db 30h
				db 88h
				db 07h
				db 20h
				db 07h
				db 08h
				db 07h
				db 30h
				db 07h

.CODE

MCGAValid PROC
	mov		eax, 1
	retn
ENDP

MCGAInit PROC
	push	esi
	push	edi
	push	ebx
	mov		_gPages, 2
	mov		edi, offset _gPageTable
	mov		esi, offset MCGAPageTable
	mov		ecx, 16
	rep		movsd
	mov		ebx, offset _gYLookup
	mov		ecx, 1200
	xor		eax, eax
	mov		edx, MCGAPageTable+14h
L1:
	mov		[ebx], eax
	add		ebx, 4
	add		eax, edx
	dec		ecx
	jnz		short L1
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

MCGASetMode PROC
	mov		ax, 13h
	int		10h
	push	0
	push	0
	push	0
	push	1
	call	MCGASetPage
	retn
ENDP

MCGASetPage PROC
	enter	0, 0
	push	edi
	push	ebx
	mov		ebx, [ebp+14h]
	mov		MCGAPage, ebx
	shl		ebx, 5
	mov		ecx, dword ptr (_gPageTable+14h)[ebx]
	push	ecx
	mov		eax, [ebp+0Ch]
	mov		edi, _gYLookup[eax*4]
	mov		eax, [ebp+10h]
	shr		eax, 2
	add		edi, eax
	add		edi, dword ptr (_gPageTable+4)[ebx]
	mov		eax, edi
	mov		bh, al
	mov		bl, 0Dh
	mov		ch, ah
	mov		cl, 0Ch
	push	eax
	push	edx
	mov		dx, 3DAh
L2:
	in		al, dx
	test	al, 1
	jnz		short L2
	pop		edx
	pop		eax
	cli
	mov		edx, 3D4h
	mov		eax, ebx
	out		dx, ax
	mov		eax, ecx
	out		dx, ax
	sti
	mov		eax, [ebp+8]
	test	eax, eax
	jz		short L4
	push	eax
	push	edx
	mov		dx, 3DAh
L3:
	in		al, dx
	test	al, 8
	jz		short L3
	pop		edx
	pop		eax
L4:
	mov		edx, 3D4h
	pop		ecx
	shr		ecx, 3
	mov		ah, cl
	mov		al, 13h
	out		dx, ax
	mov		edx, 3DAh
	in		al, dx
	mov		edx, 3C0h
	mov		al, 33h
	out		dx, al
	mov		eax, [ebp+10h]
	and		al, 3
	add		al, al
	out		dx, al
	pop		ebx
	pop		edi
	leave
	retn	10h
ENDP

MCGAGetPage PROC
	mov		eax, MCGAPage
	retn
ENDP

MCGAClear PROC
	enter	0, 0
	push	edi
	mov		eax, [ebp+8]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		ecx, dword ptr (_gPageTable+8)[eax]
	xor		eax, eax
	test	ecx, ecx
	jz		short L6
	cmp		ecx, 0Ch
	jb		short L5
	push	ebx
	mov		ebx, ecx
	mov		ecx, edi
	neg		ecx
	and		ecx, 3
	sub		ebx, ecx
	rep		stosb
	mov		ecx, ebx
	shr		ecx, 2
	rep		stosd
	mov		ecx, ebx
	and		ecx, 3
	pop		ebx
L5:
	shr		ecx, 1
	rep		stosw
	jnb		short L6
	mov		[edi], al
	inc		edi
L6:
	pop		edi
	leave
	retn	4
ENDP

MCGASetPixel PROC
	enter	0, 0
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		ebx, _gYLookup[eax*4]
	add		ebx, [ebp+0Ch]
	mov		al, byte ptr _gColor
	mov		[ebx], al
	pop		ebx
	leave
	retn	0Ch
ENDP

MCGAGetPixel PROC
	enter	0, 0
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		ebx, _gYLookup[eax*4]
	add		ebx, [ebp+0Ch]
	mov		ax, [ebx]
	pop		ebx
	leave
	retn	0Ch
ENDP

MCGAHLine PROC
	enter	0, 0
	push	edi
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+10h]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+0Ch]
	mov		al, byte ptr _gColor
	mov		ecx, [ebp+8]
	sub		ecx, [ebp+0Ch]
	inc		ecx
	test	ecx, ecx
	jz		short L8
	mov		ah, al
	cmp		ecx, 0Ch
	jz		short L7
	push	ebx
	mov		ebx, ecx
	mov		ecx, edi
	neg		ecx
	and		ecx, 3
	sub		ebx, ecx
	rep		stosb
	mov		ecx, ebx
	push	ax
	push	ax
	pop		eax
	shr		ecx, 2
	rep		stosd
	mov		ecx, ebx
	and		ecx, 3
	pop		ebx
L7:
	shr		ecx, 1
	rep		stosw
	jnb		short L8
	mov		[edi], al
	inc		edi
L8:
	pop		edi
	leave
	retn	10h
ENDP

MCGAVLine PROC
	enter	0, 0
	push	ebx
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+4)[eax]
	mov		edx, dword ptr (_gPageTable+14h)[eax]
	mov		eax, [ebp+0Ch]
	add		ebx, _gYLookup[eax*4]
	add		ebx, [ebp+10h]
	mov		ecx, [ebp+8]
	sub		ecx, eax
	inc		ecx
	mov		al, byte ptr _gColor
ALIGN 4
L9:
	mov		[ebx], al
	add		ebx, edx
	dec		ecx
	jnz		short L9
	pop		ebx
	leave
	retn	10h
ENDP

MCGAFillBox PROC
	enter	0, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+18h]
	shl		eax, 5
	mov		esi, dword ptr (_gPageTable+14h)[eax]
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+10h]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+14h]
	mov		al, byte ptr  _gColor
	mov		ah, al
	push	ax
	push	ax
	pop		eax
	mov		ebx, [ebp+0Ch]
	sub		ebx, [ebp+14h]
	mov		edx, [ebp+8]
	sub		edx, [ebp+10h]
	sub		esi, ebx
L10:
	mov		ecx, ebx
	test	ecx, ecx
	jz		short L12
	cmp		ecx, 0Ch
	jz		short L11
	push	ebx
	mov		ebx, ecx
	mov		ecx, edi
	neg		ecx
	and		ecx, 3
	sub		ebx, ecx
	rep		stosb
	mov		ecx, ebx
	shr		ecx, 2
	rep		stosd
	mov		ecx, ebx
	and		ecx, 3
	pop		ebx
L11:
	shr		ecx, 1
	rep		stosw
	jnb		short L12
	mov		[edi], al
	inc		edi
L12:
	add		edi, esi
	dec		edx
	jnz		short L10
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	14h
ENDP

MCGALine PROC
	enter	0, 0
	push	esi
	push	edi
	push	ebx
	mov		ebx, [ebp+18h]
	shl		ebx, 5
	mov		al, byte ptr _gColor
	mov		byte ptr ds:(OFFSET L18+1), al
	mov		eax, [ebp+10h]
	mov		edx, [ebp+8]
	mov		ecx, [ebp+14h]
	mov		esi, [ebp+0Ch]
	cmp		eax, edx
	jb		short L13
	xchg	ecx, esi
	xchg	eax, edx
L13:
	mov		edi, dword ptr (_gPageTable+4)[ebx]
	add		edi, _gYLookup[eax*4]
	add		edi, ecx
	mov		ebx, edx
	sub		ebx, eax
	sub		esi, ecx
	mov		eax, 1
	jnb		short L14
	neg		eax
	neg		esi
L14:
	cmp		esi, ebx
	jnb		short L15
	mov		edx, esi
	mov		esi, eax
	mov		eax, 320
	jmp		short L16
L15:
	mov		edx, ebx
	mov		ebx, esi
	mov		esi, 320
L16:
	mov		ecx, ebx
	or		ecx, ecx
	jnz		short L17
	jmp		L19
L17:
	mov		(LINEVAR1+4), eax
	add		esi, eax
	mov		LINEVAR1, esi
	mov		esi, ebx
	neg		esi
	mov		eax, esi
	add		eax, eax
	mov		LINEVAR2, eax
	add		edx, edx
	inc		ecx
	mov		ebx, ecx
	add		ecx, 3Fh
	shr		ecx, 6
	and		ebx, 3Fh
L18:
	mov		al, 0FFh
	jmp		linecall[ebx*4]

ALIGN 4

linecalllabel MACRO number
linecall_&number:
ENDM

INDEX = 0
REPT 64
linecalllabel %INDEX
	mov		[edi], al
	add		esi, edx
	sbb		ebx, ebx
	shl		ebx, 2
	add		edi, (LINEVAR1+4)[ebx]
	add		esi, (LINEVAR2+4)[ebx]
INDEX = INDEX+1
ENDM
	dec		ecx
	jz		short L19
	jmp		linecall_0
L19:
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	14h
ENDP

MCGAHLineROP PROC
	enter	0, 0
	push	edi
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+10h]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+0Ch]
	mov		ecx, [ebp+8]
	sub		ecx, [ebp+10h]
	inc		ecx
	mov		eax, _gROP
	mov		al, byte_1185D4[eax]
	mov		byte ptr ds:(OFFSET L20), al
	jmp		short $+2
	mov		al, byte ptr _gColor
	ALIGN 4
L20:
	mov		[edi], al
	inc		edi
	dec		ecx
	jnz		short L20
	pop		edi
	leave
	retn	10h
ENDP

MCGAVLineROP PROC
	enter	0, 0
	push	ebx
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+4)[eax]
	mov		edx, dword ptr (_gPageTable+14h)[eax]
	mov		eax, [ebp+0Ch]
	add		ebx, _gYLookup[eax*4]
	add		ebx, [ebp+10h]
	mov		ecx, [ebp+8]
	sub		ecx, eax
	inc		ecx
	mov		eax, _gROP
	mov		al, byte_1185D4[eax]
	mov		byte ptr ds:(OFFSET L21), al
	jmp		short $+2
	mov		al, byte ptr _gColor
	ALIGN 4
L21:
	mov		[ebx], al
	add		ebx, edx
	dec		ecx
	jnz		short L21
	pop		ebx
	leave
	retn	10h
	
ENDP

MCGABlitRLE2V PROC
	enter	0, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+14h)[eax]
	mov		edx, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		edx, _gYLookup[eax*4]
	add		edx, [ebp+0Ch]
	mov		esi, [ebp+14h]
	xor		eax, eax
	mov		ax, [esi+4]
	inc		eax
	lea		esi, [esi+0Ch]
	xor		ecx, ecx
L22:
	mov		edi, edx
	add		edx, ebx
	dec		eax
	jz		short L26
L23:
	mov		cl, [esi]
	inc		esi
	add		edi, ecx
	mov		cl, [esi]
	inc		esi
	jecxz	short L22
	cmp		ecx, 0Ch
	jbe		short L25
	test	esi, 1
	jz		short L24
	movsb
	dec		ecx
L24:
	push	ecx
	shr		ecx, 2
	rep		movsd
	pop     ecx
	and     ecx, 3
L25:
	rep		movsb
	jmp		short L23
L26:
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	10h
ENDP

func_6EC52 PROC
	enter	8, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, dword ptr (_gPageTable+14h)[eax]
	mov		[ebp-8], eax
	mov		eax, [ebp+0Ch]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+10h]
	mov		[ebp-4], edi
	mov		esi, [ebp+18h]
	xor		edx, edx
	mov		dx, [esi+4]
	inc		edx
	lea		esi, [esi+0Ch]
	mov		ebx, [ebp+8]
	xor		ecx, ecx
L27:
	mov		edi, edx
	add		edx, [ebp-8]
	dec		edx
	jz		short L31
L28:
	mov		cl, [esi]
	inc		esi
	add		edi, ecx
	mov		cl, [esi]
	inc		esi
	jecxz	short L27
	cmp		ecx, 0Ch
	jbe		short L30
	test	esi, 1
	jz		short L29
	movsb
	dec		ecx
L29:
	push	ecx
	shr		ecx, 2
	rep		movsd
	pop		ecx
	and		ecx, 3
L30:
	rep		movsb
	jmp		short L28
L31:
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn    14h
ENDP

MCGABlitV2M PROC
	enter	10h, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+24h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+14h)[eax]
	mov		[ebp-8], ebx
	mov		esi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+1Ch]
	add		esi, _gYLookup[eax*4]
	add		esi, [ebp+20h]
	mov		edi, [ebp+10h]
	mov		eax, [ebp+8]
	movzx	ecx, word ptr [edi+6]
	mov		[ebp-4], ecx
	imul	ecx
	lea		edi, [edi+0Ch]
	add		edi, eax
	mov		eax, [ebp+14h]
	sub		eax, [ebp+1Ch]
	mov		[ebp-0Ch], eax
	mov		eax, [ebp+18h]
	sub		eax, [ebp+20h]
	mov		[ebp-10h], eax
	sub		[ebp-4], eax
	sub		[ebp-8], eax
	mov		edx, [ebp-0Ch]
	xchg	ebx, ebx
L32:
	mov		ecx, [ebp-10h]
	cmp		ecx, 0Ch
	jbe		short L34
	test	esi, 1
	jz		short L33
	movsb
	dec		ecx
L33:
	push	ecx
	shr		ecx, 2
	rep		movsd
	pop		ecx
	and		ecx, 3
L34:
	rep		movsb
	add		esi, [ebp-8]
	add		edi, [ebp-4]
	dec		edx
	jnz		short L32
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	20h
ENDP

MCGABlitM2V PROC
	enter	4, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+14h)[eax]
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+0Ch]
	mov		eax, [ebp+1Ch]
	mov		ecx, [ebp+18h]
	sub		ebx, ecx
	sub		eax, ecx
	mov		[ebp-4], eax
	mov		edx, [ebp+14h]
	mov		esi, [ebp+20h]
ALIGN 4
L35:
	mov		ecx, [ebp+18h]
	cmp		ecx, 0Ch
	jbe		short L37
	test	esi, 1
	jz		short L36
	movsb
	dec		ecx
L36:
	push	ecx
	shr		ecx, 2
	rep		movsd
	pop     ecx
	and     ecx, 3
L37:
	rep		movsb
	add		esi, [ebp-4]
	add		edi, ebx
	dec		edx
	jnz		short L35
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	1Ch
ENDP

MCGABlitMT2V PROC
	enter	4, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+14h)[eax]
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+0Ch]
	mov		eax, [ebp+1Ch]
	mov		ecx, [ebp+18h]
	sub		ebx, ecx
	sub		eax, ecx
	mov		[ebp-4], eax
	mov		edx, [ebp+14h]
	mov		esi, [ebp+24h]
	mov		ah, [ebp+20h]
	xchg	ebx, ebx
ALIGN 4
L38:
	mov		ecx, [ebp+18h]
L39:
	mov		al, [esi]
	inc		esi
	cmp		al, ah
	jz		short L40
	mov		[edi], al
L40:
	inc		edi
	dec		ecx
	jnz		short L39
	add		esi, [ebp-4]
	add		edi, ebx
	dec		edx
	jnz		short L38
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	20h
ENDP

MCGABlitMono PROC
	enter	0, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+10h]
	shl		eax, 5
	mov		ebx, dword ptr (_gPageTable+14h)[eax]
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		edi, _gYLookup[eax*4]
	add		edi, [ebp+0Ch]
	mov		eax, [ebp+1Ch]
	mov		ecx, [ebp+18h]
	sub		ebx, ecx
	mov		edx, [ebp+14h]
	mov		esi, [ebp+24h]
	mov		ah, [ebp+20h]
	mov		al, byte ptr _gColor
	mov		byte ptr ds:(OFFSET L43 + 2), al
	jmp		short $+2
	xchg	ebx, ebx
L41:
	mov		ecx, [ebp+18h]
L42:
	mov		al, [esi]
	inc		esi
	test	al, ah
	jz		short L44
L43:
	mov		byte ptr [edi], 12h
L44:
	inc		edi
	dec		ecx
	jnz		short L42
	sub		esi, [ebp+18h]
	add		edi, ebx
	rol		ah, 1
	jnb		short L45
	add		esi, [ebp+1Ch]
L45:
	dec		edx
	jnz		short L41
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	20h
ENDP

END