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

.DATA

EXTRN _CoolTable:BYTE

.CODE

_CellularFrame PROC
PUBLIC _CellularFrame
    enter   0, 0
    push    esi
    push    edi
    push    ebp
    push    ebx
    mov     esi, [ebp+8]
    mov     edi, [ebp+0Ch]
    mov     eax, [ebp+10h]
    imul    edi
    mov     ebp, eax
    sub     eax, eax
    sub     ecx, ecx
    dec     esi
L1:
    lea     edx, [esi+edi]
    inc     esi
    mov     al, [edx]
    mov     cl, [edx+1]
    mov     ebx, eax
    add     ebx, ecx
    mov     al, [edx+2]
    add     edx, edi
    add     ebx, eax
    mov     cl, [edx+1]
    add     ebx, ecx
    cmp     cl, 60h
    ja      short L3
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jz      short L2
    lea     edx, [esi+edi]
    inc     esi
    mov     al, [edx]
    mov     cl, [edx+1]
    mov     ebx, eax
    add     ebx, ecx
    mov     al, [edx+2]
    add     edx, edi
    add     ebx, eax
    mov     cl, [edx+1]
    add     ebx, ecx
    cmp     cl, 60h
    ja      short L3
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jz      short L2
    lea     edx, [esi+edi]
    inc     esi
    mov     al, [edx]
    mov     cl, [edx+1]
    mov     ebx, eax
    add     ebx, ecx
    mov     al, [edx+2]
    add     edx, edi
    add     ebx, eax
    mov     cl, [edx+1]
    add     ebx, ecx
    cmp     cl, 60h
    ja      short L3
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jnz     short L1
L2:
    pop     ebx
    pop     ebp
    pop     edi
    pop     esi
    leave
    retn
L3:
    add     ebx, ecx
    mov     al, [edx]
    mov     cl, [edx+2]
    add     edx, edi
    add     ebx, eax
    add     ebx, ecx
    mov     al, [edx+1]
    add     ebx, eax
    shr     ebx, 1
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jz      short L2
    lea     edx, [esi+edi]
    inc     esi
    mov     al, [edx]
    mov     cl, [edx+1]
    mov     ebx, eax
    add     ebx, ecx
    mov     al, [edx+2]
    add     edx, edi
    add     ebx, eax
    mov     cl, [edx+1]
    add     ebx, ecx
    cmp     cl, 60h
    jbe     short L4
    add     ebx, ecx
    mov     al, [edx]
    mov     cl, [edx+2]
    add     edx, edi
    add     ebx, eax
    add     ebx, ecx
    mov     al, [edx+1]
    add     ebx, eax
    shr     ebx, 1
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jnz     L1
    pop     ebx
    pop     ebp
    pop     edi
    pop     esi
    leave
    retn
L4:
    mov     al, _CoolTable[ebx]
    dec     ebp
    mov     [esi], al
    jnz     L1
    pop     ebx
    pop     ebp
    pop     edi
    pop     esi
    leave
    retn
ENDP

END
