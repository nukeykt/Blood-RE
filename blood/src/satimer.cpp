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
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"
#include "globals.h"
#include "satimer.h"
extern "C" {
#include "task_man.h"
}
#include "smack.h"

struct CLIENT_INFO {
    void (*pCall)(void);
    task *pTask;
};

static CLIENT_INFO client[16];
static int nClients;
static BOOL timerActive;

static ulong timeradjust;
static ulong lasttimerread;

ulong pascal TimerRead(void)
{
    ulong curtime = gGameClock;
    if (curtime < lasttimerread)
        timeradjust += lasttimerread-curtime;
    lasttimerread = curtime;
    return mult64anddiv(curtime, 1024, 120);
}

void pascal TimerDone(void)
{
}

void TimerSetup(void)
{
    if (!timeradjust)
    {
        SmackTimerDoneAddr = TimerDone;
        SmackTimerReadAddr = TimerRead;
        SmackTimerSetupAddr = TimerSetup;
    }
    timeradjust = -gGameClock;
}

void timerRemove(void)
{
    union REGS regs;
    regs.w.ax = 0x1600;
    int386(0x2f, &regs, &regs);
    if (timerActive)
    {
        for (int i = 0; i < nClients; i++)
        {
            TS_Terminate(client[i].pTask);
        }
        TS_Shutdown();
        nClients = 0;
        timerActive = FALSE;
        if (regs.h.al <= 1 || regs.h.ah == 0x80)
        {
            TimerDone();
        }
    }
}

void timerInstall(void)
{
    union REGS regs;
    regs.w.ax = 0x1600;
    int386(0x2f, &regs, &regs);
    if (!timerActive)
    {
        timerActive = TRUE;
        TS_Dispatch();
        if (regs.h.al <= 1 || regs.h.ah == 0x80)
        {
            TimerSetup();
        }
        atexit(timerRemove);
    }
}

typedef void (*task_cast)(task*);

void timerRegisterClient(void(*pCall)(void), int nRate)
{
    if (nClients >= 16)
        return;
    client[nClients].pCall = pCall;
    client[nClients].pTask = TS_ScheduleTask((task_cast)pCall, nRate, 1, NULL);
    nClients++;
}

void timerRemoveClient(void(*pCall)(void))
{
    int i;
    for (i = 0; i < nClients; i++)
    {
        if (client[i].pCall == pCall)
            break;
    }
    if (i >= nClients)
        return;
    _disable();
    client[i] = client[--nClients];
    _enable();
}

void timerSetClientRate(void(*pCall)(void), int nRate)
{
    int i;
    for (i = 0; i < nClients; i++)
    {
        if (client[i].pCall == pCall)
            break;
    }
    if (i >= nClients)
        return;
    TS_SetTaskRate(client[i].pTask, nRate);
}
