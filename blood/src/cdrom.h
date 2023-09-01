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
#ifndef _CDROM_H_
#define _CDROM_H_

#include "typedefs.h"


struct tray_request_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char __f_5[8];
    unsigned char f_d;
    unsigned long f_e;
    unsigned short f_12;
    unsigned short f_14;
    unsigned long f_16;
    char __f_1a[4];
};

struct head_data_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned long f_2;
};

struct cd_request_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char __f_5[8];
    unsigned char f_d;
    unsigned long f_e;
    unsigned short f_12;
    char __f_14[4];
};

struct upc_data_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2[7];
    unsigned char f_9;
    unsigned char f_a;
};

struct ioctli_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char f_5[8];
    unsigned char f_d;
    long f_e;
    short f_12;
    short f_14;
    int f_16;
};

struct track_data_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned long f_3;
};

struct track_control_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned long f_2;
    unsigned char f_6;
};

struct cd_data_t {
    unsigned char f_0;
    unsigned long f_1;
};

struct cd2_data_t {
    unsigned char f_0;
    unsigned char f_1;
};

struct play_request_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char f_5[8];
    unsigned char f_d;
    unsigned long f_e;
    unsigned short f_12;
    unsigned long f_14;
};

struct play2_request_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char f_5[8];
    unsigned char f_d;
    unsigned long f_e;
    unsigned long f_12;
};

struct stop_request_t {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned short f_3;
    char f_5[8];
};

struct playinfo {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned char f_3;
    unsigned char f_4;
    unsigned char f_5;
    unsigned char f_6;
    unsigned char f_7;
    unsigned char f_8;
    unsigned char f_9;
    unsigned char f_a;
};

struct volumeinfo {
    unsigned char f_0;
    unsigned char f_1;
    unsigned char f_2;
    unsigned char f_3;
    unsigned char f_4;
    unsigned char f_5;
    unsigned char f_6;
    unsigned char f_7;
    unsigned char f_8;
};

class CCDAudio
{
public:
    CCDAudio() { f_e42 = 0; f_e36 = 1; f_e46 = 0; }
    ~CCDAudio() {}

    short cdrom_setup(void);
    void cdrom_shutdown(void);
    void device_request(void*);
    void red_book(unsigned long, unsigned char *, unsigned char *, unsigned char *);
    unsigned long hsg(unsigned long);
    unsigned long cd_head_position(void);
    void cd_get_volume(volumeinfo*);
    void cd_set_volume(volumeinfo*);
    short cd_getupc(void);
    void cd_get_audio_info(void);
    int func_82258(void);
    int cd_check_audio_track(short);
    void cd_set_track(short);
    unsigned long get_track_length(short);
    void cd_track_length(short a1, unsigned char *a2, unsigned char *a3, unsigned char *a4);
    void cd_status(void);
    void cd_seek(unsigned long);
    void cd_play_audio(unsigned long, unsigned long);
    void cd_stop_audio(void);
    void cd_resume_audio(void);
    void cd_cmd(unsigned char);
    void cd_getpos(playinfo*);
    short cdrom_installed(void);
    short cd_done_play(void);
    short cd_mediach(void);
    void cd_lock(unsigned char);

    short play_song(short);
    void pause_song(void);
    int newdisk(void);
    void SetVolume(int);
    int GetVolume(void);
    void func_82BB4(void);
    void StopSong(void);
    int preprocess(void);
    void postprocess(void);
    int status(int);

    char __f_0[0xb];

    volumeinfo f_b;

    unsigned char f_14;
    unsigned char f_15;
    unsigned char f_16;

    char __f_17[2];

    short f_19;

    short f_1b;

    short f_1d;
    short f_1f;
    int f_21;
    int f_25;

    char __f_29[0xe36 - 0x29];

    char f_e36;

    char __f_e38[0xe3e - 0xe37];
    
    short f_e3e;
    short f_e40;
    int f_e42;
    short f_e46;
};

extern CCDAudio Redbook;
extern BOOL gRedBookInstalled;
extern BOOL bNoCDAudio;

#endif // !_CDROM_H_
