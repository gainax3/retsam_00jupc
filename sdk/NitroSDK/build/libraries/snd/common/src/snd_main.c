/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_main.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_main.c,v $
  Revision 1.2  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.1  2005/04/12 06:16:12  ida
  SNDをNITRO-SDKへ移設

  Revision 1.4  2005/02/14 08:29:42  ida
  ２重初期化チェック追加

  Revision 1.3  2005/02/02 05:31:57  ida
  著作権年次表記を更新

  Revision 1.2  2005/02/02 05:09:21  ida
  ARM7ではミューテックスロックを行わないようにした

  Revision 1.1  2004/11/22 04:30:00  ida
  ARM7/src -> common/src

  Revision 1.17  2004/10/18 01:28:22  ida
  remove debug code

  Revision 1.16  2004/10/04 09:00:20  ida
  add SND_SetThreadPriority

  Revision 1.15  2004/08/19 06:44:17  ida
  for midi

  Revision 1.14  2004/08/02 08:30:01  ida
  fix wrong #include

  Revision 1.13  2004/08/02 05:30:22  ida
  workaround channel status

  Revision 1.12  2004/07/16 06:42:49  ida
  fix Project name

  Revision 1.11  2004/06/28 07:13:55  ida
  add SND_AlarmInit

  Revision 1.10  2004/06/23 06:24:23  ida
  channel -> exchannel

  Revision 1.9  2004/06/21 01:14:04  ida
  for new alarm and capture interface

  Revision 1.8  2004/06/11 01:52:24  ida
  port -> command

  Revision 1.7  2004/05/11 08:07:54  ida
  fix wrong comment

  Revision 1.6  2004/05/07 07:34:28  ida
  update random seed

  Revision 1.5  2004/05/06 01:40:47  ida
  use doPeriodicProc flag

  Revision 1.4  2004/04/28 01:26:17  ida
  add comments

  Revision 1.3  2004/04/26 02:56:10  ida
  for SND_RequestSpPortProc

  Revision 1.2  2004/04/05 02:55:08  ida
  unuse OS_Printf

  Revision 1.1  2004/03/26 06:25:52  ida
  create snd_drv

  Revision 1.5  2004/03/22 02:07:53  ida
  add alarm functions

  Revision 1.3  2004/03/18 13:01:26  yada
  include修正 …make通るように

  Revision 1.2  2004/03/10 06:31:24  ida
  SND_Snd -> SND_

  Revision 1.1  2004/03/08 05:40:27  ida
  rename files

  Revision 1.4  2004/03/05 06:31:57  ida
  using thread

  Revision 1.3  2004/02/24 08:08:10  ida
  workaround recv port

  Revision 1.2  2004/02/19 07:19:44  ida
  NNS_ -> SND_

  Revision 1.1  2004/02/19 07:07:52  ida
  add NITRO System snd libraries

  Revision 1.3  2004/02/19 01:44:25  ida
  add comments

  Revision 1.2  2004/02/16 06:22:07  ida
  (none)

  Revision 1.1  2004/02/13 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/main.h>

#include <nitro/os.h>
#include <nitro/snd/common/global.h>
#include <nitro/snd/common/exchannel.h>
#include <nitro/snd/common/seq.h>
#include <nitro/snd/common/alarm.h>
#include <nitro/snd/common/command.h>
#include <nitro/snd/common/util.h>
#include <nitro/snd/common/work.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#ifdef SDK_ARM7

#define SND_THREAD_STACK_SIZE      1024
#define SND_THREAD_MESSAGE_BUFSIZE 8

#define SND_ALARM_COUNT_P1  0x10000

#endif /* SDK_ARM7 */

/******************************************************************************
	static variables
 ******************************************************************************/

#ifdef SDK_ARM9

static OSMutex sSndMutex;

#else  /* SDK_ARM7 */

static OSThread sndThread;
static u64 sndStack[SND_THREAD_STACK_SIZE / sizeof(u64)];
static OSAlarm sndAlarm;
static OSMessageQueue sndMesgQueue;
static OSMessage sndMesgBuffer[SND_THREAD_MESSAGE_BUFSIZE];

#endif /* SDK_ARM9 */


/******************************************************************************
	static function declarations
 ******************************************************************************/

#ifdef SDK_ARM7

static void SndThread(void *arg);
static void SndAlarmCallback(void *arg);

#endif /* SDK_ARM7 */

/******************************************************************************
	public functions
 ******************************************************************************/

#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         SND_Init

  Description:  サウンドライブラリの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_Init(void)
{
    {
        static BOOL initialized = FALSE;
        if (initialized)
            return;
        initialized = TRUE;
    }

    OS_InitMutex(&sSndMutex);
    SND_CommandInit();
    SND_AlarmInit();
}

#else  /* SDK_ARM7 */

/*---------------------------------------------------------------------------*
  Name:         SND_Init

  Description:  サウンドを初期化し、サウンドスレッドを起動する

  Arguments:    threadPrio - thread priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_Init(u32 threadPrio)
{
    {
        static BOOL initialized = FALSE;
        if (initialized)
            return;
        initialized = TRUE;
    }

    SND_CommandInit();

    SND_CreateThread(threadPrio);
}

/*---------------------------------------------------------------------------*
  Name:         SND_CreateThread

  Description:  サウンドスレッドの起動

  Arguments:    threadPrio - thread priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_CreateThread(u32 threadPrio)
{
    OS_CreateThread(&sndThread,
                    SndThread,
                    NULL,
                    sndStack + SND_THREAD_STACK_SIZE / sizeof(u64),
                    SND_THREAD_STACK_SIZE, threadPrio);
    OS_WakeupThreadDirect(&sndThread);
}

/*---------------------------------------------------------------------------*
  Name:         SND_SetThreadPriority

  Description:  サウンドスレッドの優先度の設定

  Arguments:    prio - スレッドプライオリティ

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL SND_SetThreadPriority(u32 prio)
{
    return OS_SetThreadPriority(&sndThread, prio);
}

/*---------------------------------------------------------------------------*
  Name:         SND_InitIntervalTimer

  Description:  インターバルタイマーの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_InitIntervalTimer(void)
{
    OS_InitMessageQueue(&sndMesgQueue, sndMesgBuffer, SND_THREAD_MESSAGE_BUFSIZE);

    OS_CreateAlarm(&sndAlarm);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StartIntervalTimer

  Description:  インターバルタイマーの起動

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StartIntervalTimer(void)
{
    OS_SetPeriodicAlarm(&sndAlarm,
                        OS_GetTick() + SND_ALARM_COUNT_P1,
                        SND_PROC_INTERVAL, &SndAlarmCallback, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StopIntervalTimer

  Description:  インターバルタイマーの停止

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StopIntervalTimer(void)
{
    OS_CancelAlarm(&sndAlarm);
}

/*---------------------------------------------------------------------------*
  Name:         SND_WaitForIntervalTimer

  Description:  インターバルタイマー待ち

  Arguments:    None.

  Returns:      メッセージを返す
 *---------------------------------------------------------------------------*/
OSMessage SND_WaitForIntervalTimer(void)
{
    OSMessage message;

    (void)OS_ReceiveMessage(&sndMesgQueue, &message, OS_MESSAGE_BLOCK);

    return message;
}

/*---------------------------------------------------------------------------*
  Name:         SND_SendWakeupMessage

  Description:  サウンドスレッドを起こすメッセージ送信

  Arguments:    なし

  Returns:      メッセージの送信に成功したかどうかのフラグ
 *---------------------------------------------------------------------------*/
BOOL SND_SendWakeupMessage(void)
{
    return OS_SendMessage(&sndMesgQueue, (OSMessage)SND_MESSAGE_WAKEUP_THREAD, OS_MESSAGE_NOBLOCK);
}

#endif /* SDK_ARM7 */

/******************************************************************************
	private function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SNDi_LockMutex

  Description:  サウンドミューテックスにロックをかけます

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SNDi_LockMutex(void)
{
#ifdef SDK_ARM9
    OS_LockMutex(&sSndMutex);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         SNDi_UnlockMutex

  Description:  サウンドミューテックスのロックを解除します

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SNDi_UnlockMutex(void)
{
#ifdef SDK_ARM9
    OS_UnlockMutex(&sSndMutex);
#endif
}

/******************************************************************************
	static funtions
 ******************************************************************************/

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         SndAlarmCallback

  Description:  アラーム周期で呼びだされるコールバック

  Arguments:    arg - ユーザーデータ（未使用）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SndAlarmCallback(void * /*arg */ )
{
    if (!OS_SendMessage(&sndMesgQueue, (OSMessage)SND_MESSAGE_PERIODIC, OS_MESSAGE_NOBLOCK))
    {
        OS_PutString("Failed sound alarm OS_SendMessage\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         SndThread

  Description:  サウンドスレッド関数

  Arguments:    arg - ユーザーデータ（未使用）

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SndThread(void * /*arg */ )
{
    SND_InitIntervalTimer();
    SND_ExChannelInit();
    SND_SeqInit();
    SND_AlarmInit();
    SND_Enable();
    SND_SetOutputSelector(SND_OUTPUT_MIXER,
                          SND_OUTPUT_MIXER, SND_CHANNEL_OUT_MIXER, SND_CHANNEL_OUT_MIXER);
    SND_SetMasterVolume(SND_MASTER_VOLUME_MAX);

    SND_StartIntervalTimer();

    while (1)
    {
        OSMessage message;
        BOOL    doPeriodicProc = FALSE;

        //-----------------------------------------------------------------------------
        // wait interval timer

        message = SND_WaitForIntervalTimer();

        switch ((u32)message)
        {
        case SND_MESSAGE_PERIODIC:
            doPeriodicProc = TRUE;
            break;
        case SND_MESSAGE_WAKEUP_THREAD:
            break;
        }

        //-----------------------------------------------------------------------------
        // update registers

        SND_UpdateExChannel();

        //-----------------------------------------------------------------------------
        // ARM9 command proc

        SND_CommandProc();

        //-----------------------------------------------------------------------------
        // framework

        SND_SeqMain(doPeriodicProc);
        SND_ExChannelMain(doPeriodicProc);

        SND_UpdateSharedWork();

        (void)SND_CalcRandom();
    }

    SND_Disable();
    SND_StopIntervalTimer();
}

#endif /* SDK_ARM7 */

/*====== End of snd_main.c ======*/
