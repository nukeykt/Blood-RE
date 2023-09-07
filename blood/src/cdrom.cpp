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
#include <i86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"
#include "cdrom.h"
#include "globals.h"
#include "view.h"

int int_295330;
playinfo *pinfo;
int pinfo_descriptor;
volumeinfo *vinfo;
int vinfo_descriptor;

struct {
    unsigned short f_0;
    unsigned char f_2;
    unsigned short f_3;
    unsigned long f_5;
    unsigned char f_9;
    unsigned char f_a;
    unsigned char f_b;
    unsigned char f_c;
    unsigned char f_d;
    unsigned char f_e;
    unsigned long f_f;
    unsigned char f_13[7];
    unsigned char f_1a[6];
    unsigned long f_20;
    unsigned short f_24;
} cdrom_data;

tray_request_t *tray_request;
int tray_request_descriptor;
head_data_t *head_data;
int head_data_descriptor;
cd_request_t *cd_request;
int cd_request_descriptor;
upc_data_t *upc_data;
int upc_data_descriptor;
ioctli_t *ioctli;
int ioctli_descriptor;
track_data_t *track_data;
int track_data_descriptor;
track_control_t *track_control;
int track_control_descriptor;
cd_data_t *cd_data;
int cd_data_descriptor;
cd2_data_t *cd2_data;
int cd2_data_descriptor;
play_request_t *play_request;
int play_request_descriptor;
play2_request_t *play2_request;
int play2_request_descriptor;
stop_request_t *stop_request;
int stop_request_descriptor;
unsigned char *cd_mode;
int cd_mode_descriptor;
REGS inregs;
REGS outregs;
struct SREGS sregs;
int cdmem_allocated;
void *cdmem_ptr;
unsigned short cdmem_segment;

struct rminfo {
    unsigned int edi, esi, ebp, reserved, ebx, edx, ecx, eax;
    unsigned short flags, es, ds, fs, gs, ip, cs, sp, ss;
} RMI;

int dpmiAlloc(void **, int *segment, int size);
#pragma aux dpmiAlloc =\
"mov eax,0x100"\
"add ebx,15"\
"shr ebx,4"\
"int 0x31"\
"jb short L1"\
"movzx eax,ax"\
"shl eax,4"\
"mov [esi],eax"\
"mov [edi],edx"\
"sub eax,eax"\
"L1:"\
parm nomemory [esi][edi][ebx]\
modify exact [eax ebx edx esi edi]

int dpmiFree(int segment);
#pragma aux dpmiFree = \
"mov eax,0x101" \
"int 0x31" \
"jb short L1" \
"sub eax,eax" \
"L1:" \
parm nomemory [edx] \
modify exact [eax edx]


short CCDAudio::cdrom_setup(void)
{
    int status = 0;

    status |= dpmiAlloc((void**)&tray_request, &tray_request_descriptor, 30);
    status |= dpmiAlloc((void**)&head_data, &head_data_descriptor, 6);
    status |= dpmiAlloc((void**)&cd_request, &cd_request_descriptor, 24);
    status |= dpmiAlloc((void**)&upc_data, &upc_data_descriptor, 11);
    status |= dpmiAlloc((void**)&ioctli, &ioctli_descriptor, 26);
    status |= dpmiAlloc((void**)&track_data, &track_data_descriptor, 7);
    status |= dpmiAlloc((void**)&cd_data, &cd_data_descriptor, 5);
    status |= dpmiAlloc((void**)&cd2_data, &cd2_data_descriptor, 2);
    status |= dpmiAlloc((void**)&play_request, &play_request_descriptor, 24);
    status |= dpmiAlloc((void**)&play2_request, &play2_request_descriptor, 22);
    status |= dpmiAlloc((void**)&stop_request, &stop_request_descriptor, 13);
    status |= dpmiAlloc((void**)&cd_mode, &cd_mode_descriptor, 1);
    status |= dpmiAlloc((void**)&track_control, &track_control_descriptor, 7);
    status |= dpmiAlloc((void**)&pinfo, &pinfo_descriptor, 11);
    status |= dpmiAlloc((void**)&vinfo, &vinfo_descriptor, 9);

    return status == 0;
}

void CCDAudio::cdrom_shutdown(void)
{
    while (int_295330)
    {
        cd_lock(0);
    }

    dpmiFree(tray_request_descriptor);
    dpmiFree(head_data_descriptor);
    dpmiFree(cd_request_descriptor);
    dpmiFree(upc_data_descriptor);
    dpmiFree(ioctli_descriptor);
    dpmiFree(track_data_descriptor);
    dpmiFree(cd_data_descriptor);
    dpmiFree(cd2_data_descriptor);
    dpmiFree(play_request_descriptor);
    dpmiFree(play2_request_descriptor);
    dpmiFree(stop_request_descriptor);
    dpmiFree(cd_mode_descriptor);
    dpmiFree(track_control_descriptor);
    dpmiFree(vinfo_descriptor);
    dpmiFree(pinfo_descriptor);
}

void CCDAudio::device_request(void *a1)
{
    memset(&sregs, 0, sizeof(sregs));
    if (!cdmem_allocated)
    {
        inregs.x.eax = 0x100;
        inregs.x.ebx = 0x10;
        int386(0x31, &inregs, &outregs);
        if (outregs.x.cflag)
        {
            printf("DPMI_GetDOSMemory: CD memory allocation failed\n");
            exit(0);
        }
        cdmem_segment = outregs.w.ax;
        cdmem_ptr = (void*)(cdmem_segment << 4);
        cdmem_allocated = 1;
    }
    byte len = *(byte*)a1;
    memcpy(cdmem_ptr, a1, len);
    memset(&RMI, 0, sizeof(RMI));
    memset(&inregs, 0, sizeof(inregs));

    RMI.eax = 0x1510;
    RMI.ecx = cdrom_data.f_2;
    RMI.es = cdmem_segment;

    inregs.x.eax = 0x300;
    inregs.x.ebx = 0x2f;
    sregs.es = FP_SEG(&RMI);
    inregs.x.edi = FP_OFF(&RMI);
    int386x(0x31, &inregs, &outregs, &sregs);
    memcpy(a1, cdmem_ptr, len);
}

void CCDAudio::red_book(unsigned long a1, unsigned char *a2, unsigned char *a3, unsigned char *a4)
{
    *a4 = a1 & 0xff;
    *a3 = (a1 & 0xff00) >> 8;
    *a2 = (a1 & 0xff0000) >> 16;
}

unsigned long CCDAudio::hsg(unsigned long a1)
{
    unsigned char t1;
    unsigned char t2;
    unsigned char t3;
    red_book(a1, &t1, &t2, &t3);
    a1 = t1 * 4500;
    a1 += t2 * 75;
    a1 += t3 - 150;
    return a1;
}

unsigned long CCDAudio::cd_head_position(void)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)head_data << 12;
    tray_request->f_12 = 6;
    head_data->f_0 = 1;
    head_data->f_1 = 0;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    return head_data->f_2;
}

void CCDAudio::cd_get_volume(volumeinfo *a1)
{
    memcpy(vinfo, a1, 9);
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)vinfo << 12;
    tray_request->f_12 = 9;
    vinfo->f_0 = 4;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    memcpy(a1, vinfo, 9);
}

void CCDAudio::cd_set_volume(volumeinfo *a1)
{
    memcpy(vinfo, a1, 9);
    vinfo->f_0 = 3;
    cd_request->f_0 = 24;
    cd_request->f_1 = 0;
    cd_request->f_2 = 12;
    cd_request->f_d = 0;
    cd_request->f_e = (unsigned long)vinfo << 12;
    cd_request->f_12 = 9;
    device_request(cd_request);
    cdrom_data.f_24 = cd_request->f_3;
    memcpy(a1, vinfo, 9);
}

short CCDAudio::cd_getupc(void)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)upc_data << 12;
    tray_request->f_12 = 11;
    upc_data->f_0 = 0xe;
    upc_data->f_1 = 2;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    if (upc_data->f_1 == 0)
        memset(upc_data->f_2, 0, 7);
    memcpy(cdrom_data.f_13, upc_data->f_2, 7);
    return 1;
}

void CCDAudio::cd_get_audio_info(void)
{
    cdrom_data.f_a = 0;
    cdrom_data.f_b = 0;
    ioctli->f_0 = 26;
    ioctli->f_1 = 0;
    ioctli->f_2 = 3;
    ioctli->f_d = 0;
    ioctli->f_14 = 0;
    ioctli->f_16 = 0;
    ioctli->f_e = (long)track_data << 12;
    ioctli->f_12 = 7;
    track_data->f_0 = 10;
    device_request(ioctli);
    memcpy(cdrom_data.f_1a, &track_data->f_1, 6);
    cdrom_data.f_a = track_data->f_1;
    cdrom_data.f_b = track_data->f_2;
    red_book(track_data->f_3, &cdrom_data.f_c, &cdrom_data.f_d, &cdrom_data.f_e);
    cdrom_data.f_f = hsg(track_data->f_3);
    cdrom_data.f_24 = ioctli->f_3;
}

int CCDAudio::func_82258(void)
{
    return cdrom_data.f_b;
}

int CCDAudio::cd_check_audio_track(short a1)
{
    cd_get_audio_info();
    if (track_data->f_2 > 30)
        return 0;
    if (track_data->f_2 == 1)
        return 0;
    return 1;
}

void CCDAudio::cd_set_track(short a1)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)track_control << 12;
    tray_request->f_12 = 7;
    track_control->f_0 = 11;
    track_control->f_1 = a1;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    cdrom_data.f_5 = hsg(track_control->f_2);
    cdrom_data.f_3 = a1;
    cdrom_data.f_9 = track_control->f_6 & 0xd0;
}

unsigned long CCDAudio::get_track_length(short a1)
{
    unsigned long vdi;
    unsigned long vc;
    unsigned short vsi;
    vsi = cdrom_data.f_3;
    cd_set_track(a1);
    vdi = cdrom_data.f_5;
    if (a1 < cdrom_data.f_b)
    {
        cd_set_track(a1);
        vc = cdrom_data.f_5;
    }
    else
        vc = cdrom_data.f_f;
    cd_set_track(vsi);
    vc -= vdi;
    return vc;
}

void CCDAudio::cd_track_length(short a1, unsigned char *a2, unsigned char *a3, unsigned char *a4)
{
    unsigned long va = get_track_length(a1);
    va += 150;
    *a4 = va % 75;
    va -= *a4;
    va /= 75;
    *a3 = va % 60;
    va -= *a3;
    va /= 60;
    *a2 = va;
}

void CCDAudio::cd_status(void)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)cd_data << 12;
    tray_request->f_12 = 5;
    cd_data->f_0 = 6;
    device_request(tray_request);
    cdrom_data.f_20 = cd_data->f_1;
    cdrom_data.f_24 = tray_request->f_3;
}

void CCDAudio::cd_seek(unsigned long a1)
{
    play_request->f_0 = 24;
    play_request->f_1 = 0;
    play_request->f_2 = 131;
    play_request->f_d = 0;
    play_request->f_e = 0;
    play_request->f_12 = 0;
    play_request->f_14 = a1;
    device_request(play_request);
    cdrom_data.f_24 = play_request->f_3;
}

void CCDAudio::cd_play_audio(unsigned long a1, unsigned long a2)
{
    play2_request->f_0 = 22;
    play2_request->f_1 = 0;
    play2_request->f_2 = 132;
    play2_request->f_d = 0;
    play2_request->f_e = a1;
    play2_request->f_12 = a2-a1;
    device_request(play2_request);
    cdrom_data.f_24 = play2_request->f_3;
}

void CCDAudio::cd_stop_audio(void)
{
    stop_request->f_0 = 13;
    stop_request->f_1 = 0;
    stop_request->f_2 = 133;
    device_request(stop_request);
    cdrom_data.f_24 = stop_request->f_3;
}

void CCDAudio::cd_resume_audio(void)
{
    stop_request->f_0 = 13;
    stop_request->f_1 = 0;
    stop_request->f_2 = 136;
    device_request(stop_request);
    cdrom_data.f_24 = stop_request->f_3;
}

void CCDAudio::cd_cmd(unsigned char a1)
{
    *cd_mode = a1;
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_e = (unsigned long)cd_mode << 12;
    tray_request->f_12 = 1;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
}

void CCDAudio::cd_getpos(playinfo *a1)
{
    memcpy(pinfo, a1, 11);
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)pinfo << 12;
    tray_request->f_12 = 6;
    pinfo->f_0 = 4;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    memcpy(a1, pinfo, 11);
}

short CCDAudio::cdrom_installed(void)
{
    inregs.h.ah = 0x15;
    inregs.h.al = 0;
    inregs.w.bx = 0;
    int386(0x2f, &inregs, &outregs);
    if (outregs.w.bx == 0)
        return 0;
    cdrom_data.f_0 = outregs.w.bx;
    cdrom_data.f_2 = outregs.w.cx;
    cd_get_audio_info();
    int_148E14 = cdrom_data.f_2;
    return 1;
}

short CCDAudio::cd_done_play(void)
{
    cd_cmd(5);
    return (cdrom_data.f_24 & 0x200) == 0;
}

short CCDAudio::cd_mediach(void)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 3;
    tray_request->f_d = 0;
    tray_request->f_d = 0;
    tray_request->f_14 = 0;
    tray_request->f_16 = 0;
    tray_request->f_e = (unsigned long)cd2_data << 12;
    tray_request->f_12 = 2;
    cd2_data->f_0 = 9;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    return cd2_data->f_1;
}

void CCDAudio::cd_lock(unsigned char a1)
{
    tray_request->f_0 = 30;
    tray_request->f_1 = 0;
    tray_request->f_2 = 12;
    tray_request->f_d = 0;
    tray_request->f_e = (unsigned long)cd2_data << 12;
    tray_request->f_12 = 2;
    cd2_data->f_0 = 1;
    cd2_data->f_1 = a1;
    device_request(tray_request);
    cdrom_data.f_24 = tray_request->f_3;
    if (a1 == 1)
        int_295330++;
    else
        int_295330--;
    if (int_295330 < 0)
        int_295330 = 0;
}

short CCDAudio::play_song(short a1)
{
    f_19 = a1;
    f_e3e = 1;
    if (f_e36 != 2)
    {
        if (f_19 < cdrom_data.f_a || f_19 > cdrom_data.f_b)
            return 0;
        cd_set_track(f_19);
    }
    else
        cd_set_track((qrand() % cdrom_data.f_b) + 1);
    if (cdrom_data.f_9 == 0x40)
        return 0;

    unsigned long vdi = cdrom_data.f_5;
    unsigned long vb;
    if (f_e36 == 0)
    {
        vb = cdrom_data.f_f;
    }
    else
    {
        short va = cdrom_data.f_3;
        if (va + 1 > cdrom_data.f_b)
            vb = cdrom_data.f_f;
        else
        {
            cd_set_track(va + 1);
            vb = cdrom_data.f_5;
            cd_set_track(va);
        }
    }
    cd_lock(1);
    cd_seek(cdrom_data.f_5);
    cd_play_audio(vdi, vb);
    f_1d = 1;
    return 1;
}

void CCDAudio::pause_song(void)
{
    if (f_1f == 0)
    {
        cd_stop_audio();
        f_1f = 1;
    }
    else
    {
        cd_resume_audio();
        f_1f = 0;
    }
}

int CCDAudio::newdisk(void)
{
    short i;
    f_25 = 0;
    f_e40 = 1;
    f_e36 = 1;
    i = 0;
    do
    {
        cd_get_audio_info();
        i++;
        if (cdrom_data.f_24 != 0x8102)
            break;
    } while (i < 500);
    if (i == 500)
        return -1;
    if (cdrom_data.f_b > 30)
        cdrom_data.f_b = 35;
    for (f_19 = cdrom_data.f_a; f_19 <= cdrom_data.f_b; f_19++)
    {
        cd_track_length(f_19, &f_14, &f_15, &f_16);
        cd_set_track(f_19);
    }

    cd_get_volume(&f_b);
    return 1;
}

void CCDAudio::SetVolume(int a1)
{
    if (a1 > 255)
        a1 = 255;
    f_b.f_2 = a1;
    f_b.f_4 = a1;
    f_b.f_6 = a1;
    f_b.f_8 = a1;
    cd_set_volume(&f_b);
}

int CCDAudio::GetVolume(void)
{
    cd_get_volume(&f_b);
    return f_b.f_2;
}

void CCDAudio::func_82BB4(void)
{
    f_e42 = cdrom_data.f_20;
}

void CCDAudio::StopSong(void)
{
    cd_stop_audio();
    cd_lock(0);
    f_1d = 0;
}

int CCDAudio::preprocess(void)
{
    if (f_e46 < 120)
        return 1;
    cd_status();
    if (f_e42 == cdrom_data.f_20)
        return 1;
    f_e42 = cdrom_data.f_20;
    if (!(cdrom_data.f_20 & 1))
    {
        f_21 = cdrom_data.f_f;
        short i;

        i = 0;
        do
        {
            cd_get_audio_info();
            i++;
            if (cdrom_data.f_24 != 0x8102)
                break;
        } while (i < 2500);
        if (i == 2500)
            return 0;
        if (f_21 != cdrom_data.f_f)
            newdisk();
    }
    switch (gInputMode)
    {
        case INPUT_MODE_1:
        case INPUT_MODE_3:
            break;
        default:
            func_1EC78(0, "CD-Audio", "Restarting Track", NULL);
            break;
    }
    long oc = gGameClock;
    short t = f_19;
    play_song(t);
    cd_status();
    f_e42 = cdrom_data.f_20;
    gGameClock = oc;
    return 1;
}

void CCDAudio::postprocess(void)
{
    if (f_e46 < 120)
        f_e46++;
    else
        f_e46 = 0;
}

int CCDAudio::status(int a1)
{
    return cdrom_data.f_20 & a1;
}
