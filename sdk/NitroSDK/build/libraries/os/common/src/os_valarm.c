/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_valarm.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_valarm.c,v $
  Revision 1.23  2007/11/15 05:17:30  seiki_masashi
  デバッガで中断した場合などに周期 VAlarm の一部が発生しなくなる場合があるのを修正

  Revision 1.22  2007/08/20 05:05:39  okubata_ryoma
  OS_SetPeriodicVAlarm 関数の引数 delay のアサートの閾値を修正

  Revision 1.21  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.20  2005/06/08 00:53:21  yada
  add assert (check if the library is initialized)

  Revision 1.19  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.18  2005/02/18 08:51:35  yasu
  著作年度変更

  Revision 1.17  2005/02/18 06:20:37  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.16  2004/11/17 08:22:21  yada
  fix insert valarm subroutine

  Revision 1.15  2004/11/02 10:37:32  yada
  add some assert, and fix comment

  Revision 1.14  2004/10/21 13:41:49  yasu
  厳しい条件下での動作の検討を追加

  Revision 1.13  2004/10/21 06:11:14  yada
  Fix a lot.  Consider for delay.

  Revision 1.12  2004/09/28 12:41:35  yada
  suppress string in SDK_FINALROM

  Revision 1.11  2004/09/02 06:21:03  yada
  only change comment

  Revision 1.10  2004/08/30 00:33:54  yada
  in case alarm already set when set, force to call OS_Panic()

  Revision 1.9  2004/03/08 05:35:03  yada
  周期VCountの遅延許容範囲を設定出来るようにした。

  Revision 1.8  2004/03/02 11:04:36  yada
  V割り込みの遅延を考慮した。

  Revision 1.7  2004/03/02 04:38:21  yada
  複数の同値周期Vアラームは連続して起動するようにした。

  Revision 1.6  2004/03/02 04:10:31  yada
  複数の周期Vアラームがうまくかからなかったのを修正

  Revision 1.5  2004/02/24 08:23:20  yada
  ハンドル内で周期アラーム二度起動チェック部分修正

  Revision 1.4  2004/02/24 04:49:18  yada
  only arrange tag

  Revision 1.3  2004/02/23 12:29:56  yada
  DISPSTAT操作をGX関数に

  Revision 1.2  2004/02/23 12:07:01  yada
  ARM7用定義追加

  Revision 1.1  2004/02/23 11:58:06  yada
  firstRelease


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define OSi_VHIGHT              HW_LCD_LINES

#define OSi_VALARM_LATER        0
#define OSi_VALARM_NOW          1
#define OSi_VALARM_TIMEOUT      2

//---- valarm queue
static struct OSiVAlarmQueue
{
    OSVAlarm *head;
    OSVAlarm *tail;
}
OSi_VAlarmQueue;


//---- flag for initialization or not
static u16 OSi_UseVAlarm = FALSE;

//---- frame count
static s32 OSi_VFrameCount;
static s32 OSi_PreviousVCount;

static void OSi_InsertVAlarm(OSVAlarm *alarm);
static void OSi_SetNextVAlarm(OSVAlarm *alarm);
static void OSi_AppendVAlarm(OSVAlarm *alarm);
static void OSi_VAlarmHandler(void *arg);
static int OSi_CompareVCount(OSVAlarm *alarm, s32 currentVCount, s32 currentVFrame);
static s32 OSi_GetVFrame(s32 vcount);


//----------------------------------------------------------------
//  arrange vcount
static int OSi_VCountArr(int count)
{
    while (count < 0)
    {
        count += OSi_VHIGHT;
    }
    while (count >= OSi_VHIGHT)
    {
        count -= OSi_VHIGHT;
    }
    return count;
}

//----------------------------------------------------------------
//  subtruction for vcount (a-b)
#define OSi_VCountSub( a, b )  OSi_VCountArr( (int)((a) - (b)) )

//----------------------------------------------------------------
//  check if a vcount is near to c vcount than b vcount
#define OSi_IsNearVCount( a, b, c ) ( ( OSi_VCountSub( (a), (c) )  < OSi_VCountSub( (b), (c) ) )? TRUE: FALSE )


//================================================================================
//        INITIALIZE/FINALIZE VALARM SYSTEM
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitVAlarm

  Description:  Initialize v-alarm module

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitVAlarm(void)
{
    if (!OSi_UseVAlarm)
    {
        OSi_UseVAlarm = TRUE;

        //---- clear valarm list
        OSi_VAlarmQueue.head = NULL;
        OSi_VAlarmQueue.tail = NULL;

        //---- Vcount IRQ Disable
        (void)OS_DisableIrqMask(OS_IE_V_COUNT);

        //---- init frame count
        OSi_VFrameCount = 0;
        OSi_PreviousVCount = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_EndVAlarm

  Description:  end v-alarm system

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_EndVAlarm(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(OSi_UseVAlarm);
    enabled = OS_DisableInterrupts();

    //---- check if any valarm exists
    if (OSi_UseVAlarm)
    {
        SDK_ASSERTMSG(!OSi_VAlarmQueue.head,
                      "OS_EndVAlarm: Cannot end v-alarm system while using v-alarm.");
        OSi_UseVAlarm = FALSE;
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsVAlarmAvailable

  Description:  check v-alarm system is available

  Arguments:    None

  Returns:      if available, TRUE.
 *---------------------------------------------------------------------------*/
BOOL OS_IsVAlarmAvailable(void)
{
    return OSi_UseVAlarm;
}

//================================================================================
//        OPERATION OF VALARM LIST
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_InsertVAlarm

  Description:  Insert v-alarm. Needs to be called interrupts disabled.

  Arguments:    alarm        : pointer to v-alarm to be set

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_InsertVAlarm(OSVAlarm *alarm)
{
    OSVAlarm *prev;
    OSVAlarm *next;

    //---- insert to list
    for (next = OSi_VAlarmQueue.head; next; next = next->next)
    {
        //---- check 'alarm' should be after than 'next'.
        if ((next->frame < alarm->frame) ||
            (next->frame == alarm->frame) && (next->fire <= alarm->fire))
        {
            continue;
        }

        prev = next->prev;

        //---- insert valarm before 'next'
        //---- set alarm link
        alarm->prev = prev;
        alarm->next = next;

        //---- set next alarm link
        next->prev = alarm;

        //---- set previous alarm link
        if (prev)
        {
            prev->next = alarm;
        }
        else
        {
            OSi_VAlarmQueue.head = alarm;
            OSi_SetNextVAlarm(alarm);
        }

        return;
    }

    //---- append valarm to tail
    OSi_AppendVAlarm(alarm);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_AppendVAlarm

  Description:  Append v-alarm. Needs to be called interrupts disabled.
                alarm is put into last position of list.

  Arguments:    alarm       pointer to v-alarm to be set

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_AppendVAlarm(OSVAlarm *alarm)
{
    OSVAlarm *prev = OSi_VAlarmQueue.tail;

    //---- set alarm link
    alarm->prev = prev;
    alarm->next = NULL;

    //---- fix tail link
    OSi_VAlarmQueue.tail = alarm;

    //---- fix old last alarm link
    if (prev)
    {
        prev->next = alarm;
    }
    else
    {
        OSi_VAlarmQueue.head = alarm;
        OSi_SetNextVAlarm(alarm);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_DetachAlarm

  Description:  detach head v-alarm. Needs to be called interrupts disabled.

  Arguments:    alarm       pointer to v-alarm to be detach

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_DetachVAlarm(OSVAlarm *alarm)
{
    OSVAlarm *prev;
    OSVAlarm *next;

    if (!alarm)
    {
        return;
    }

    prev = alarm->prev;
    next = alarm->next;

    //---- fix next alarm link
    if (next)
    {
        next->prev = prev;
    }
    else
    {
        OSi_VAlarmQueue.tail = prev;
    }

    //---- fix previous alarm link
    if (prev)
    {
        prev->next = next;
    }
    else
    {
        OSi_VAlarmQueue.head = next;
    }
}

//================================================================================
//        SET VALARM
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_CreateVAlarm

  Description:  Create v-alarm

  Arguments:    alarm       pointer to v-alarm to be initialized

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CreateVAlarm(OSVAlarm *alarm)
{
    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERT(alarm);

    alarm->handler = 0;
    alarm->tag = 0;

    alarm->finish = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetVAlarm

  Description:  Set v-alarm as a relative time

  Arguments:    alarm       pointer to v-alarm to be set
                count       count to fire
                delay       permissible range of delay
                handler     v-alarm handler to be called
                arg         argument of handler

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetVAlarm(OSVAlarm *alarm, s16 count, s16 delay, OSVAlarmHandler handler, void *arg)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    s32     currentVCount;
    s32     currentVFrame;

    //OS_Printf( "**OS_SetVAlarm alarm=%x count=%d\n", alarm, count);
    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERTMSG(handler, "OS_SetVAlarm: handler must not be NULL.");
    if (!alarm || alarm->handler)
    {
#ifndef SDK_FINALROM
        OS_Panic("v-alarm could be already used.");
#else
        OS_Panic("");
#endif
    }

    //---- get current vframe and vcount
    currentVCount = GX_GetVCount();
    currentVFrame = OSi_GetVFrame(currentVCount);

    //---- clear periodic info
    alarm->period = FALSE;

    //---- set fire time
    alarm->fire = count;
    alarm->frame = (u32)((count > currentVCount) ? currentVFrame : (currentVFrame + 1));

    //---- permissible range of delay
    alarm->delay = delay;

    //---- set handler
    alarm->handler = handler;
    alarm->arg = arg;

    //---- cancel flag
    alarm->canceled = FALSE;

    //---- insert periodic valarm
    OSi_InsertVAlarm(alarm);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetPeriodicVAlarm

  Description:  set periodic v-alarm

  Arguments:    alarm          pointer to v-alarm to be set
                count          count to fire cyclicly
                delay          permissible range of delay
                handler        v-alarm handler to be called
                arg            argument of handler

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetPeriodicVAlarm(OSVAlarm *alarm, s16 count, s16 delay, OSVAlarmHandler handler, void *arg)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    s32     currentVCount;
    s32     currentVFrame;

    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERTMSG(handler, "OS_SetPeriodicVAlarm: handler must not be NULL\n");
    SDK_ASSERTMSG(0 <= delay
                  && delay <= OSi_VHIGHT,
                  "OS_SetPeriodicVAlarm:  illegal permissible range of delay.");
    if (!alarm || alarm->handler)
    {
#ifndef SDK_FINALROM
        OS_Panic("v-alarm could be already used.");
#else
        OS_Panic("");
#endif
    }

    //---- get current frame and vcount
    currentVCount = GX_GetVCount();
    currentVFrame = OSi_GetVFrame(currentVCount);

    //---- set periodic info
    alarm->period = TRUE;

    //---- set fire time
    alarm->fire = count;
    alarm->frame = (u32)((count > currentVCount) ? currentVFrame : (currentVFrame + 1));

    //---- permissible range of delay
    alarm->delay = delay;

    //---- set handler
    alarm->handler = handler;
    alarm->arg = arg;

    //---- cancel flag
    alarm->canceled = FALSE;

    //---- insert periodic valarm
    OSi_InsertVAlarm(alarm);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetNextVAlarm

  Description:  set VCount

  Arguments:    alarm       pointer of v-alarm struct

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_SetNextVAlarm(OSVAlarm *alarm)
{
    //---- set interrupt callback
    OS_SetIrqFunction(OS_IE_V_COUNT, (void (*)())OSi_VAlarmHandler);

    //---- set vcount line
    GX_SetVCountEqVal(alarm->fire);

    //---- set interrupt
    GX_VCountEqIntr(TRUE);
    (void)OS_EnableIrqMask(OS_IE_V_COUNT);
}

//================================================================================
//        ALARM TAG
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetVAlarmTag

  Description:  set tag which is used OS_CancelVAlarms

  Arguments:    alarm        alarm to be set tag
                tag          tagNo

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetVAlarmTag(OSVAlarm *alarm, u32 tag)
{
#ifdef SDK_DEBUG
    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERT(alarm);
    SDK_ASSERTMSG(tag > 0, "OSSetVAlarmTag: Tag must be >0.");
#else
    if (tag == 0)
    {
        OS_Panic("");
    }
#endif

    if (alarm)
    {
        alarm->tag = tag;
    }
}

//================================================================================
//        CANCEL VALARM
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_CancelVAlarm

  Description:  Cancel v-alarm

  Arguments:    alarm       pointer to v-alarm to be canceled

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelVAlarm(OSVAlarm *alarm)
{
    OSIntrMode enabled = OS_DisableInterrupts();;

    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERT(alarm);

    alarm->canceled = TRUE;
    if (alarm->handler == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return;
    }

    //---- detach valarm
    OSi_DetachVAlarm(alarm);

    //---- clear hander
    alarm->handler = NULL;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_CancelAllVAlarms

  Description:  cancel all v-alarms

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelAllVAlarms(void)
{
    OSIntrMode enabled;
    OSVAlarm *alarm;
    OSVAlarm *next;

    SDK_ASSERT(OSi_UseVAlarm);
    enabled = OS_DisableInterrupts();

    for (alarm = OSi_VAlarmQueue.head, next = alarm ? alarm->next : NULL;
         alarm; alarm = next, next = alarm ? alarm->next : NULL)
    {
        //---- cancel valarm
        OS_CancelVAlarm(alarm);
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_CancelVAlarms

  Description:  cancel v-alarms which have specified tag

  Arguments:    tag          tagNo. to be cancelled. not 0

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelVAlarms(u32 tag)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    OSVAlarm *alarm;
    OSVAlarm *next;

#ifdef SDK_DEBUG
    SDK_ASSERT(OSi_UseVAlarm);
    SDK_ASSERTMSG(tag > 0, "OSCancelVAlarms: Tag must be >0.");
#else
    if (tag == 0)
    {
        OS_Panic("");
    }
#endif

    for (alarm = OSi_VAlarmQueue.head, next = alarm ? alarm->next : NULL;
         alarm; alarm = next, next = alarm ? alarm->next : NULL)
    {
        if (alarm->tag == tag)
        {
            //---- cancel valarm
            OS_CancelVAlarm(alarm);
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}

//================================================================================
//        ALARM HANDLER
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_VAlarmHandler

  Description:  handler vcount interrupt

  Arguments:    not use

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_VAlarmHandler(void *)
{
    OSVAlarm *alarm;
    OSVAlarmHandler handler;
    int     check;
    s32     currentVCount;
    s32     currentVFrame;

    //---- To be vcount-irq Disable
    (void)OS_DisableIrqMask(OS_IE_V_COUNT);
    GX_VCountEqIntr(FALSE);

    //---- set check flag vcount interrupt
    OS_SetIrqCheckFlag(OS_IE_V_COUNT);

    //---- arrange vframe
    // vcount '-1' for care of periodic alarm which interrupts in same line every frame
    currentVCount = GX_GetVCountEqVal();
    currentVFrame = OSi_GetVFrame(currentVCount - 1);


    while (NULL != (alarm = OSi_VAlarmQueue.head))
    {
        //---- get current vcount and vframe
        currentVCount = GX_GetVCount();
        currentVFrame = OSi_GetVFrame(currentVCount);

        //---- compare current vcount with alarm fire vcount
        check = OSi_CompareVCount(alarm, currentVFrame, currentVCount);

#ifdef DEBUGPRINT
        OS_Printf("[%d:%d %d] %d %d check=%d \n",
                  alarm->arg, alarm->frame, alarm->fire, currentVFrame, currentVCount, check);
#endif

        switch (check)
        {
        case OSi_VALARM_LATER:
            // Set next vcount interrupt
            OSi_SetNextVAlarm(alarm);

            //
            // Check if time is up (vcount == intrvcount) during setting next interrupt
            //
            if (alarm->fire != GX_GetVCount() || alarm->frame != currentVFrame)
            {
                return;
            }

            // Cancel next vcount interrupt
            (void)OS_DisableIrqMask(OS_IE_V_COUNT);
            GX_VCountEqIntr(FALSE);
            (void)OS_ResetRequestIrqMask(OS_IE_V_COUNT);

            // Go through (no need 'break') to call valarm handler

        case OSi_VALARM_NOW:
            //---- detach alarm
            handler = alarm->handler;
            OSi_DetachVAlarm(alarm);
            alarm->handler = NULL;

            //---- call user valarm handler
            if (handler)
            {
                (handler) (alarm->arg);
            }

            //---- move to tail or so
            if (alarm->period && !alarm->canceled)
            {
                alarm->handler = handler;
                alarm->frame = (u32)OSi_VFrameCount + 1;
                OSi_InsertVAlarm(alarm);
            }
            break;

        case OSi_VALARM_TIMEOUT:
            //---- move to tail or so
            OSi_DetachVAlarm(alarm);
            alarm->frame = (u32)OSi_VFrameCount + 1;
            OSi_InsertVAlarm(alarm);
            break;

        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_CompareVCount

  Description:  compare specified alarm vcount with current vcount.
                check if time is to fire. consider for delay

  Arguments:    alarm         : alarm to check
                currentVFrame : current vframe
                currentVCount : current vcount

  Returns:      OSi_VALARM_LATER    : there is enough time to fire next alarm, so do fire later
                OSi_VALARM_NOW      : it is time to fire alarm
                OSi_VALARM_TIMEOUT  : alarm time has passed already

 *---------------------------------------------------------------------------*/
static int OSi_CompareVCount(OSVAlarm *alarm, s32 currentVFrame, s32 currentVCount)
{
    s32     delayVFrame;
    s32     delayVCount;

    delayVFrame = currentVFrame - (s32)alarm->frame;    // get diff for safety against overflow
    delayVCount = currentVCount - (s32)alarm->fire;

    //
    // Check if having reached the time yet
    //
    if (delayVFrame < 0 || (delayVFrame == 0 && delayVCount < 0))
    {
        return OSi_VALARM_LATER;
    }

    //
    // Time is passed, execute alarm or timed out
    //
    if (delayVCount < 0)
    {
        delayVCount += OSi_VHIGHT;     // Care for looping vcount 263->0
    }

    // Check if timeout or not
    return (delayVCount <= alarm->delay) ? OSi_VALARM_NOW : OSi_VALARM_TIMEOUT;
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetVFrame

  Description:  Get current vframe. If need, increase frame count.

  Arguments:    vcount  : vcount

  Returns:      current vframe
 *---------------------------------------------------------------------------*/
static s32 OSi_GetVFrame(s32 vcount)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- count up vframe if need
    if (vcount < OSi_PreviousVCount)
    {
        OSi_VFrameCount++;
    }
    OSi_PreviousVCount = vcount;

    (void)OS_RestoreInterrupts(enabled);
    return OSi_VFrameCount;
}



//================================================================================
//        FOR DEBUG
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_DumpVAlarm

  Description:  Dump all valarm resisterd currently
                This is for debug function.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_DumpVAlarm(void);
void OS_DumpVAlarm(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    OSVAlarm *alarm;
    OSVAlarm *next;

    OS_Printf("---------------- current vcount = %d\n", (s16)GX_GetVCount());
    for (alarm = OSi_VAlarmQueue.head; alarm; alarm = next)
    {
        next = alarm->next;

        OS_Printf("%08x fire=%d:%d delay=%d arg=%2d prev=%08x next=%08x\n", alarm,
                  alarm->frame, alarm->fire, alarm->delay, alarm->arg, alarm->prev, alarm->next);
    }

    (void)OS_RestoreInterrupts(enabled);
}
