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
dd offset ModeWDriver
dd offset ModeXDriver
dd offset ModeYDriver
dd offset ModeZDriver
VGT ENDS

VGTEND SEGMENT DWORD PUBLIC USE32 'DLL'
VGTEND ENDS

.DATA

EXTRN _gPages:DWORD
EXTRN _gPageTable:DWORD
EXTRN _gYLookup:DWORD
EXTRN _gColor:DWORD
EXTRN _gROP:DWORD

ModeWDriver	dd offset ModeXValid
			dd offset ModeWName
			dd 360
			dd 270
			dd 8
			dd 4
			dd offset ModeWInit
			dd offset ModeWSetMode
			dd offset ModeXSetPage
			dd offset ModeXGetPage
			dd offset ModeXClear
			dd offset ModeXSetPixel
			dd offset ModeXGetPixel
			dd offset ModeXHLine
			dd offset ModeXVLine
			dd offset ModeXLine
			dd offset ModeXFillBox
			dd offset ModeXHLineROP
			dd offset ModeXVLineROP
			dd offset ModeXBlitRLE2V
ModeWName	db 'Mode-W 360x270 256 Color', 0
ModeWPageTable	dd 1
				dd 0A0000h
				dd 5EECh
				dd 360
				dd 270
				dd 90
				dd 0, 0
				dd 1
				dd 0A5EECh
				dd 5EECh
				dd 360
				dd 270
				dd 90
				dd 0, 0
				dd 0
				dd 0ABDD8h
				dd 4218h
				dd 360
				dd 180
				dd 90
				dd 0, 0
word_1186A9		dw 6B00h, 5901h, 5A02h, 8E03h, 5E04h, 8A05h, 4006h, 0F007h
				dw 6109h, 2410h, 0AA11h, 1B12h, 2D13h, 14h, 1B15h, 3A16h, 0E317h

ModeXDriver	dd offset ModeXValid
			dd offset ModeXName
			dd 320
			dd 240
			dd 8
			dd 4
			dd offset ModeXInit
			dd offset ModeXSetMode
			dd offset ModeXSetPage
			dd offset ModeXGetPage
			dd offset ModeXClear
			dd offset ModeXSetPixel
			dd offset ModeXGetPixel
			dd offset ModeXHLine
			dd offset ModeXVLine
			dd offset ModeXLine
			dd offset ModeXFillBox
			dd offset ModeXHLineROP
			dd offset ModeXVLineROP
			dd offset ModeXBlitRLE2V
ModeXName	db 'Mode-X 320x240 256 Color', 0
ModeXPageTable	dd 1
				dd 0A0000h
				dd 4B00h
				dd 320
				dd 240
				dd 80
				dd 0, 0
				dd 1
				dd 0A4B00h
				dd 4B00h
				dd 320
				dd 240
				dd 80
				dd 0, 0
				dd 1
				dd 0A9600h
				dd 4B00h
				dd 320
				dd 240
				dd 80
				dd 0, 0
				dd 0
				dd 0AE100h
				dd 1EF0h
				dd 320
				dd 99
				dd 80
				dd 0, 0
word_1187B4		dw 0D06h, 3E07h, 4109h, 0EA10h, 0AC11h, 0DF12h, 14h, 0E715h, 616h, 0E317h

ModeYDriver	dd offset ModeXValid
			dd offset ModeYName
			dd 320
			dd 400
			dd 8
			dd 4
			dd offset ModeYInit
			dd offset ModeYSetMode
			dd offset ModeXSetPage
			dd offset ModeXGetPage
			dd offset ModeXClear
			dd offset ModeXSetPixel
			dd offset ModeXGetPixel
			dd offset ModeXHLine
			dd offset ModeXVLine
			dd offset ModeXLine
			dd offset ModeXFillBox
			dd offset ModeXHLineROP
			dd offset ModeXVLineROP
			dd offset ModeXBlitRLE2V
ModeYName	db 'Mode-Y 320x400 256 Color', 0
ModeYPageTable	dd 1
				dd 0A0000h
				dd 7D00h
				dd 320
				dd 400
				dd 80
				dd 0, 0
				dd 1
				dd 0A7D00h
				dd 7D00h
				dd 320
				dd 400
				dd 80
				dd 0, 0
				dd 0
				dd 0AFA00h
				dd 5F0h
				dd 320
				dd 19
				dd 80
				dd 0, 0
word_118891		dw 4009h, 14h, 0E317h

ModeZDriver	dd offset ModeXValid
			dd offset ModeZName
			dd 360
			dd 480
			dd 8
			dd 4
			dd offset ModeZInit
			dd offset ModeZSetMode
			dd offset ModeXSetPage
			dd offset ModeXGetPage
			dd offset ModeXClear
			dd offset ModeXSetPixel
			dd offset ModeXGetPixel
			dd offset ModeXHLine
			dd offset ModeXVLine
			dd offset ModeXLine
			dd offset ModeXFillBox
			dd offset ModeXHLineROP
			dd offset ModeXVLineROP
			dd offset ModeXBlitRLE2V
ModeZName	db 'Mode-Z 360x480 256 Color', 0
ModeZPageTable	dd 1
				dd 0A0000h
				dd 0A8C0h
				dd 360
				dd 480
				dd 90
				dd 0, 0
				dd 0
				dd 0AA8C0h
				dd 5730h
				dd 360
				dd 248
				dd 90
				dd 0, 0

word_118940		dw 6B00h, 5901h, 5A02h, 8E03h, 5E04h, 8A05h, 0D06h, 3E07h
				dw 4009h, 0EA10h, 0AC11h, 0DF12h, 2D13h, 14h, 0E715h, 616h, 0E317h

byte_118962	db 0Fh, 0Eh, 0Ch, 8
byte_118966	db 1, 3, 7, 0Fh

ModeXPage	dd 00

.CODE

ModeXValid PROC
	mov		eax, 1
	retn
ENDP

ModeWInit PROC
	push	esi
	push	edi
	push	ebx
	mov		_gPages, 3
	mov		ecx, 24
	mov		edi, offset _gPageTable
	mov		esi, offset ModeWPageTable
	rep		movsd
	mov		ebx, offset _gYLookup
	mov		ecx, 4B0h
	xor		eax, eax
	mov		edx, ModeWPageTable+14h
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

ModeWSetMode PROC
	push	esi
	push	edi
	push	ebx
	mov		ax, 13h
	int		10h
	mov		dx, 3C4h
	mov		ax, 604h
	out		dx, ax
	mov		ax, 100h
	out		dx, ax
	mov		dx, 3C2h
	mov		al, 0E7h
	out		dx, al
	mov		dx, 3C4h
	mov		ax, 300h
	out		dx, ax
	mov		dx, 3D4h
	mov		al, 11h
	out		dx, al
	inc		dx
	in 		al, dx
	and		al, 7Fh
	out		dx, al
	dec		dx
	mov		esi, offset word_1186A9
	mov		ecx, 11h
L2:
	lodsw
	out		dx, ax
	dec		ecx
	jnz		short L2
	mov		dx, 3C4h
	mov		ax, 0F02h
	out		dx, ax
	mov		edi, 0A0000h
	xor		eax, eax
	mov		ecx, 4000h
	rep		stosd
	push	0
	push	0
	push	0
	push	1
	call	ModeXSetPage
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeXInit PROC
	push	esi
	push	edi
	push	ebx
	mov		_gPages, 4
	mov		ecx, 20h
	mov		edi, offset _gPageTable
	mov		esi, offset ModeXPageTable
	rep		movsd
	mov		ebx, offset _gYLookup
	mov		ecx, 4B0h
	xor		eax, eax
	mov		edx, ModeXPageTable+14h
L3:
	mov		[ebx], eax
	add		ebx, 4
	add		eax, edx
	dec		ecx
	jnz		short L3
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeXSetMode PROC
	push	esi
	push	edi
	push	ebx
	mov		ax, 13h
	int		10h
	mov		dx, 3C4h
	mov		ax, 604h
	out		dx, ax
	mov		ax, 100h
	out		dx, ax
	mov		dx, 3C2h
	mov		al, 0E3h
	out		dx, al
	mov		dx, 3C4h
	mov		ax, 300h
	out		dx, ax
	mov		dx, 3D4h
	mov		al, 11h
	out		dx, al
	inc		dx
	in 		al, dx
	and		al, 7Fh
	out		dx, al
	dec		dx
	mov		esi, offset word_1186A9
	mov		ecx, 0Ah
L4:
	lodsw
	out		dx, ax
	dec		ecx
	jnz		short L4
	mov		dx, 3C4h
	mov		ax, 0F02h
	out		dx, ax
	mov		edi, 0A0000h
	xor		eax, eax
	mov		ecx, 4000h
	rep		stosd
	push	0
	push	0
	push	0
	push	1
	call	ModeXSetPage
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeYInit PROC
	push	esi
	push	edi
	push	ebx
	mov		_gPages, 3
	mov		ecx, 24
	mov		edi, offset _gPageTable
	mov		esi, offset ModeYPageTable
	rep		movsd
	mov		ebx, offset _gYLookup
	mov		ecx, 4B0h
	xor		eax, eax
	mov		edx, ModeYPageTable+14h
L5:
	mov		[ebx], eax
	add		ebx, 4
	add		eax, edx
	dec		ecx
	jnz		short L5
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeYSetMode PROC
	push	esi
	push	edi
	push	ebx
	mov		ax, 13h
	int		10h
	mov		dx, 3C4h
	mov		ax, 604h
	out		dx, ax
	mov		dx, 3D4h
	mov		esi, offset word_118891
	mov		ecx, 3
L6:
	lodsw
	out		dx, ax
	dec		ecx
	jnz		short L6
	mov		dx, 3C4h
	mov		ax, 0F02h
	out		dx, ax
	mov		edi, 0A0000h
	xor		eax, eax
	mov		ecx, 4000h
	rep		stosd
	push	0
	push	0
	push	0
	push	1
	call	ModeXSetPage
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeZInit PROC
	push	esi
	push	edi
	push	ebx
	mov		_gPages, 2
	mov		ecx, 16
	mov		edi, offset _gPageTable
	mov		esi, offset ModeZPageTable
	rep		movsd
	mov		ebx, offset _gYLookup
	mov		ecx, 4B0h
	xor		eax, eax
	mov		edx, ModeZPageTable+14h
L7:
	mov		[ebx], eax
	add		ebx, 4
	add		eax, edx
	dec		ecx
	jnz		short L7
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeZSetMode PROC
	push	esi
	push	edi
	push	ebx
	mov		ax, 13h
	int		10h
	mov		dx, 3C4h
	mov		ax, 604h
	out		dx, ax
	mov		ax, 100h
	out		dx, ax
	mov		dx, 3C2h
	mov		al, 0E7h
	out		dx, al
	mov		dx, 3C4h
	mov		ax, 300h
	out		dx, ax
	mov		dx, 3D4h
	mov		al, 11h
	out		dx, al
	inc		dx
	in 		al, dx
	and		al, 7Fh
	out		dx, al
	dec		dx
	mov		esi, offset word_118940
	mov		ecx, 11h
L8:
	lodsw
	out		dx, ax
	dec		ecx
	jnz		short L8
	mov		dx, 3C4h
	mov		ax, 0F02h
	out		dx, ax
	mov		edi, 0A0000h
	xor		eax, eax
	mov		ecx, 4000h
	rep		stosd
	push	0
	push	0
	push	0
	push	1
	call	ModeXSetPage
	pop		ebx
	pop		edi
	pop		esi
	retn
ENDP

ModeXSetPage PROC
	enter	0, 0
	push	edi
	push	ebx
	mov		ebx, [ebp+14h]
	mov		ModeXPage, ebx
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
L9:
	in		al, dx
	test	al, 1
	jnz		short L9
	pop		edx
	pop		eax
	cli
	mov		edx, 3D4h
	mov		eax, ebx
	out		dx, ax
	mov		eax, ecx
	out		dx, ax
	sti
	mov		eax, [ebp+8h]
	test	eax, eax
	jz		short L11
	push	eax
	push	edx
	mov		dx, 3DAh
L10:
	in		al, dx
	test	al, 8
	jz		short L10
	pop		edx
	pop		eax
L11:
	mov		edx, 3D4h
	pop		ecx
	shr		ecx, 1
	mov		ah, cl
	mov		al, 13h
	out		dx, ax
	mov		edx, 3DAh
	in		al, dx
	mov		edx, 3C0h
	mov		al, 33h
	out		dx, al
	mov		al, byte ptr [ebp+10h]
	and		al, 3
	add		al, al
	out		dx, al
	pop		ebx
	pop		edi
	leave
	retn	10h
ENDP

ModeXGetPage PROC
	mov		eax, ModeXPage
	retn
ENDP

ModeXClear PROC
	enter	0, 0
	push	edi
	push	ebx
	mov		dx, 3C5h
	mov		al, 0Fh
	out		dx, al
	mov		eax, [ebp+8]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		ecx, dword ptr (_gPageTable+8)[eax]
	xor		eax, eax
	test	ecx, ecx
	jz		short L13
	cmp		ecx, 0Ch
	jb		short L12
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
L12:
	shr		ecx, 1
	rep		stosw
	jnb		short L13
	mov		[edi], al
	inc		edi
L13:
	pop		ebx
	pop		edi
	leave
	retn	4
ENDP

ModeXSetPixel PROC
	enter	0, 0
	push	ebx
	mov		ebx, [ebp+0Ch]
	mov		cl, bl
	and		cl, 3
	mov		al, 1
	shl		al, cl
	mov		dx, 3C5h
	out		dx, al
	shr		ebx, 2
	mov		eax, [ebp+10h]
	shl		eax, 5
	add		ebx, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+8]
	add		ebx, _gYLookup[eax*4]
	mov		al, byte ptr _gColor
	mov		[ebx], al
	pop		ebx
	leave
	retn	0Ch
ENDP

ModeXGetPixel PROC
	enter	0, 0
	leave
	retn	0Ch
ENDP

ModeXHLine PROC
	enter	0, 0
	push	edi
	push	ebx
	mov		eax, [ebp+14h]
	shl		eax, 5
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+10h]
	add		edi, _gYLookup[eax*4]
	mov		eax, [ebp+0Ch]
	and		eax, 3
	mov		bh, byte_118962[eax]
	mov		eax, [ebp+8]
	and		eax, 3
	mov		bl, byte_118966[eax]
	mov		eax, [ebp+0Ch]
	mov		ecx, [ebp+8]
	shr		ecx, 2
	shr		eax, 2
	add		edi, eax
	sub		ecx, eax
	mov		ah, byte ptr _gColor
	mov		dx, 3C5h
	test	ecx, ecx
	jz		short L16
	mov		al, bh
	out		dx, al
	mov		al, ah
	mov		[edi], al
	inc		edi
	dec		ecx
	mov		al, 0Fh
	out		dx, al
	mov		al, ah
	test	ecx, ecx
	jz		short L15
	mov		ah, al
	cmp		ecx, 0Ch
	jb		short L14
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
L14:
	shr		ecx, 1
	rep		stosw
	jnb		short L15
	mov		[edi], al
	inc		edi
L15:
	mov		al, bl
	out		dx, al
	mov		al, ah
	mov		[edi], al
	pop		ebx
	pop		edi
	leave
	retn	10h
ALIGN 2
L16:
	and		bl, bh
	mov		al, bl
	out		dx, al
	mov		al, ah
	mov		[edi], al
	pop		ebx
	pop		edi
	leave
	retn	10h
ENDP

ModeXVLine PROC
	enter	0, 0
	push	ebx
	mov		ebx, [ebp+10h]
	mov		cl, bl
	and		cl, 3
	mov		al, 1
	shl		al, cl
	mov		dx, 3C5h
	out		dx, al
	shr		ebx, 2
	mov		eax, [ebp+14h]
	shl		eax, 5
	add		ebx, dword ptr (_gPageTable+4)[eax]
	mov		edx, dword ptr (_gPageTable+14h)[eax]
	mov		eax, [ebp+0Ch]
	add		ebx, _gYLookup[eax*4]
	mov		ecx, [ebp+8]
	sub		ecx, [ebp+0Ch]
	inc		ecx
	mov		al, byte ptr _gColor
L17:
	mov		[ebx], al
	add		ebx, edx
	dec		ecx
	jnz		short L17
	pop		ebx
	leave
	retn	10h
ENDP

ModeXLine PROC
	enter	0, 0
	push	esi
	push	edi
	push	ebx
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	14h
ENDP

ModeXFillBox PROC
	enter	10h, 0
	push	esi
	push	edi
	push	ebx
	mov		eax, [ebp+18h]
	shl		eax, 5
	mov		ecx, dword ptr (_gPageTable+14h)[eax]
	mov		[ebp-4], ecx
	mov		edi, dword ptr (_gPageTable+4)[eax]
	mov		eax, [ebp+10h]
	add		edi, _gYLookup[eax*4]
	mov		esi, [ebp+14h]
	and		esi, 3
	mov		bh, byte_118962[esi]
	mov		esi, [ebp+0Ch]
	and		esi, 3
	mov		bl, byte_118966[esi]
	mov		eax, [ebp+14h]
	shr		eax, 2
	mov		esi, [ebp+0Ch]
	shr		esi, 2
	add		edi, eax
	mov		[ebp-10h], edi
	sub		esi, eax
	mov		[ebp-0Ch], esi
	mov		ecx, [ebp+8]
	sub		ecx, [ebp+10h]
	inc		ecx
	mov		[ebp-8], ecx
	mov		ah, byte ptr _gColor
	mov		dx, 3C5h
	test	esi, esi
	jz		L23
	nop
	nop
	nop
	nop
	mov		al, bh
	out		dx, al
	mov		esi, [ebp-4]
L18:
	mov		[edi], ah
	add		edi, esi
	dec		ecx
	jnz		short L18
	mov		al, bl
	out		dx, al
	mov		edi, [ebp-10h]
	add		edi, [ebp-0Ch]
	mov		ecx, [ebp-8]
ALIGN 2
L19:
	mov		[edi], ah
	add		edi, esi
	dec		ecx
	jnz		short L19
	mov		al, 0Fh
	out		dx, al
	mov		edx, [ebp-0Ch]
	dec		edx
	mov		ebx, esi
	sub		ebx, edx
	mov		edi, [ebp-10h]
	inc		edi
	mov		al, ah
	mov		esi, [ebp-8]
	push	ax
	push	ax
	pop		eax
L20:
	mov		ecx, edx
	test	ecx, ecx
	jz		short L22
	cmp		ecx, 0Ch
	jb		short L21
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
L21:
	shr		ecx, 1
	rep		stosw
	jnb		short L22
	mov		[edi], al
	inc		edi
L22:
	add		edi, ebx
	dec		esi
	jnz		short L20
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn	14h
L23:
	and		bl, bh
	mov		al, bl
	out		dx, al
	mov		esi, [ebp-4]
L24:
	mov		[edi], ah
	add		edi, esi
	dec		ecx
	jnz		short L24
	pop		ebx
	pop		edi
	pop		esi
	leave
	retn		14h

ENDP

ModeXHLineROP PROC
	enter	0, 0
	push	edi
	push	ebx
	pop		ebx
	pop		edi
	leave
	retn	10h
ENDP

ModeXVLineROP PROC
	enter	0, 0
	push	ebx
	pop		ebx
	leave
	retn	14h
ENDP

ModeXBlitRLE2V PROC
	push	ebx
	pop		ebx
	retn
ENDP

END
