/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_alarm.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_alarm.c,v $
  Revision 1.3  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.2  2005/04/13 00:13:29  ida
  著作権年次表記を更新

  Revision 1.1  2005/04/12 06:16:12  ida
  SNDをNITRO-SDKへ移設

  Revision 1.5  2004/08/19 06:49:48  ida
  work around alarm

  Revision 1.4  2004/08/02 05:30:56  ida
  workaround sound alarm

  Revision 1.3  2004/07/16 10:11:43  ida
  remove EnableIrq

  Revision 1.2  2004/07/16 06:42:49  ida
  fix Project name

  Revision 1.1  2004/06/28 07:12:54  ida
  ARM7/src/snd_alarm.c -> common/src/snd_alarm.c

  Revision 1.3  2004/06/21 01:11:39  ida
  change alarm interface

  Revision 1.2  2004/04/05 02:55:08  ida
  unuse OS_Printf

  Revision 1.1  2004/03/26 06:25:52  ida
  create snd_drv

  Revision 1.1  2004/03/22 02:05:44  ida
  add alarm

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/alarm.h>

#include <nitro/pxi.h>
#include <nitro/snd/common/work.h>

/******************************************************************************
	structure definition
 ******************************************************************************/

#ifdef SDK_ARM9

typedef struct AlarmCallbackInfo
{
    void    (*func) (void *);
    void   *arg;
    u8      id;
    u8      pad;
    u16     pad2;
}
AlarmCallbackInfo;

#endif

/******************************************************************************
	static variable
 ******************************************************************************/

#ifdef SDK_ARM9

static AlarmCallbackInfo sCallbackTable[SND_ALARM_NUM];

#endif

/******************************************************************************
	static function declaration
 ******************************************************************************/

#ifdef SDK_ARM7

static void AlarmHandler(void *arg);

#endif

/******************************************************************************
	public function
 ******************************************************************************/

#ifdef SDK_ARM9

void SND_AlarmInit(void)
{
    int     alarmNo;

    for (alarmNo = 0; alarmNo < SND_ALARM_NUM; alarmNo++)
    {
        AlarmCallbackInfo *info;

        info = &sCallbackTable[alarmNo];

        info->func = NULL;
        info->arg = NULL;
        info->id = 0;
    }
}

#else

void SND_AlarmInit(void)
{
    int     alarmNo;

    for (alarmNo = 0; alarmNo < SND_ALARM_NUM; alarmNo++)
    {
        SNDi_Work.alarm[alarmNo].enable = FALSE;
        SNDi_Work.alarm[alarmNo].id = 0;
    }
}

void SND_SetupAlarm(int alarmNo, OSTick tick, OSTick period, int id)
{
    SNDAlarm *alarm;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);
    SDK_MINMAX_ASSERT(id, 0x00, 0xff);

    alarm = &SNDi_Work.alarm[alarmNo];

    if (alarm->enable)
    {
        OS_CancelAlarm(&alarm->alarm);
        alarm->enable = FALSE;
    }

    alarm->setting.tick = tick;
    alarm->setting.period = period;
    alarm->id = (u8)id;
}

void SND_StartAlarm(int alarmNo)
{
    OSTick  tick;
    OSTick  period;
    SNDAlarm *alarm;
    void   *arg;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);

    alarm = &SNDi_Work.alarm[alarmNo];

    if (alarm->enable)
    {
        OS_CancelAlarm(&alarm->alarm);
        alarm->enable = FALSE;
    }

    tick = alarm->setting.tick;
    period = alarm->setting.period;
    arg = (void *)((alarm->id << 8) | alarmNo);

    OS_CreateAlarm(&alarm->alarm);

    if (period == 0)
    {
        OS_SetAlarm(&alarm->alarm, tick, AlarmHandler, arg);
    }
    else
    {
        OS_SetPeriodicAlarm(&alarm->alarm, OS_GetTick() + tick, period, AlarmHandler, arg);
    }

    alarm->enable = TRUE;
}

void SND_StopAlarm(int alarmNo)
{
    SNDAlarm *alarm;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);

    alarm = &SNDi_Work.alarm[alarmNo];

    if (!alarm->enable)
        return;

    OS_CancelAlarm(&alarm->alarm);

    alarm->id++;
    alarm->enable = FALSE;
}
#endif

/******************************************************************************
	static function
 ******************************************************************************/

#ifdef SDK_ARM7

static void AlarmHandler(void *arg)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, (u32)arg, FALSE) < 0)
    {
        // retry
    }
}

#endif

/******************************************************************************
	private function
 ******************************************************************************/

#ifdef SDK_ARM9

void SNDi_IncAlarmId(int alarmNo)
{
    AlarmCallbackInfo *info;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);

    info = &sCallbackTable[alarmNo];

    info->id++;
}

u8 SNDi_SetAlarmHandler(int alarmNo, SNDAlarmHandler handler, void *arg)
{
    AlarmCallbackInfo *info;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);

    info = &sCallbackTable[alarmNo];

    info->func = handler;
    info->arg = arg;
    info->id++;

    return info->id;
}

void SNDi_CallAlarmHandler(int msg)
{
    AlarmCallbackInfo *info;
    int     alarmNo = msg & 0xff;
    int     id = (msg >> 8) & 0xff;

    SDK_MINMAX_ASSERT(alarmNo, SND_ALARM_MIN, SND_ALARM_MAX);

    info = &sCallbackTable[alarmNo];

    if (id == info->id)
    {
        if (info->func != NULL)
        {
            info->func(info->arg);
        }
    }
}

#endif

/*====== End of snd_alarm.c ======*/
