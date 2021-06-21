/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_timer.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_timer.c,v $
  Revision 1.11  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.9  2004/11/24 04:37:24  yada
  just fix typo

  Revision 1.8  2004/09/02 06:20:41  yada
  only change comment

  Revision 1.7  2004/03/01 12:17:27  yada
  英語の関数comment作成

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  2004/02/04 07:33:57  yada
  関数名で、～OsReserved～　のOsを削除

  Revision 1.4  2004/02/03 11:40:50  yada
  assertミス修正

  Revision 1.3  2004/02/03 11:21:45  yada
  システム予約かどうかのassertを追加

  Revision 1.2  2004/02/02 05:30:34  yada
  タイマのシステム予約機構を導入

  Revision 1.1  2004/01/27 01:47:32  yada
  inline見直しに伴いfirstRelease


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>


//----------------------------------------------------------------------
#define REG_OS_TMCNT_H_E_MASK      REG_OS_TM0CNT_H_E_MASK
#define REG_OS_TMCNT_H_I_MASK      REG_OS_TM0CNT_H_I_MASK
#define REG_OS_TMCNT_H_CH_MASK     REG_OS_TM1CNT_H_CH_MASK

//---- if OS reserved each timer, bit=1
static u16 OSi_TimerReserved = 0;

#define OSi_TIMER_RESERVED_0   1
#define OSi_TIMER_RESERVED_1   2
#define OSi_TIMER_RESERVED_2   4
#define OSi_TIMER_RESERVED_3   8

u16     OSi_IsTimerReserved(int timerNum);
void    OSi_SetTimerReserved(int timerNum);
void    OSi_UnsetTimerReserved(int timerNum);


//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_IsTimerReserved

  Description:  check if specified timer is reserved for OS

  Arguments:    timerNum : timerNo (0-3)

  Returns:      non-0 if reserved
 *---------------------------------------------------------------------------*/
u16 OSi_IsTimerReserved(int timerNum)
{
    return (u16)(OSi_TimerReserved & (1 << timerNum));
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetTimerReserved

  Description:  set specified timer to reserved for OS

  Arguments:    timerNum : timerNo (0-3)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_SetTimerReserved(int timerNum)
{
    OSi_TimerReserved |= (1 << timerNum);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_UnsetTimerReserved

  Description:  unset specified timer to reserved for OS

  Arguments:    timerNum : timerNo (0-3)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_UnsetTimerReserved(int timerNum)
{
    OSi_TimerReserved &= ~(1 << timerNum);
}

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_StartTimer

  Description:  set timer(s) and start

  Arguments:    id         timerNo
                count      count value to be set to timer
                preScale   preScale

  Returns:      None
 *---------------------------------------------------------------------------*/
//
//   use 1 timer, 16bit counter,  timer<id> interrupt occurs by overflow
//
void OS_StartTimer(OSTimer id, u16 count, OSTimerPrescaler preScale)
{
    SDK_ASSERT(OS_TIMER_0 <= id && id <= OS_TIMER_3);
    SDK_ASSERT(OS_TIMER_PRESCALER_1 <= preScale && preScale <= OS_TIMER_PRESCALER_1024);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));


    OS_SetTimerCount(id, (u16)~count);
    OS_SetTimerControl(id, (u16)(REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_I_MASK | preScale));
}

//
//   use 2 timers, 32bit counter, timer<id+1> interrupt occurs by overflow
//
void OS_StartTimer32(OSTimer32 id, u32 count, OSTimerPrescaler preScale)
{
    SDK_ASSERT(OS_TIMER32_01 <= id && id <= OS_TIMER32_23);
    SDK_ASSERT(OS_TIMER_PRESCALER_1 <= preScale && preScale <= OS_TIMER_PRESCALER_1024);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 1));


    OS_SetTimerCount((OSTimer)((int)id + 1), (u16)((~count >> 16) & 0xffff));
    OS_SetTimerCount((OSTimer)id, (u16)(~count & 0xffff));

    OS_SetTimerControl((OSTimer)((int)id + 1),
                       REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_I_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl((OSTimer)id, (u16)(REG_OS_TMCNT_H_E_MASK | preScale));
}

//
//   use 3 timers, 48bit counter, timer<id+2> interrupt occurs by overflow
//
void OS_StartTimer48(OSTimer48 id, u64 count, OSTimerPrescaler preScale)
{
    SDK_ASSERT(OS_TIMER48_012 <= id && id <= OS_TIMER48_123);
    SDK_ASSERT(OS_TIMER_PRESCALER_1 <= preScale && preScale <= OS_TIMER_PRESCALER_1024);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 1));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 2));


    OS_SetTimerCount((OSTimer)((int)id + 2), (u16)((~count >> 32) & 0xffff));
    OS_SetTimerCount((OSTimer)((int)id + 1), (u16)((~count >> 16) & 0xffff));
    OS_SetTimerCount((OSTimer)id, (u16)(~count & 0xffff));

    OS_SetTimerControl((OSTimer)((int)id + 2),
                       REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_I_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl((OSTimer)((int)id + 1), REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl((OSTimer)id, (u16)(REG_OS_TMCNT_H_E_MASK | preScale));
}

//
//   use all 4 timers, 64bit counter, timer3 interrupt occurs by overflow
//
void OS_StartTimer64(u64 count, OSTimerPrescaler preScale)
{
    SDK_ASSERT(OS_TIMER_PRESCALER_1 <= preScale && preScale <= OS_TIMER_PRESCALER_1024);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_0));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_1));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_2));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_3));


    OS_SetTimerCount(OS_TIMER_3, (u16)((~count >> 48) & 0xffff));
    OS_SetTimerCount(OS_TIMER_2, (u16)((~count >> 32) & 0xffff));
    OS_SetTimerCount(OS_TIMER_1, (u16)((~count >> 16) & 0xffff));
    OS_SetTimerCount(OS_TIMER_0, (u16)(~count & 0xffff));

    OS_SetTimerControl(OS_TIMER_3,
                       REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_I_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl(OS_TIMER_2, REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl(OS_TIMER_1, REG_OS_TMCNT_H_E_MASK | REG_OS_TMCNT_H_CH_MASK);
    OS_SetTimerControl(OS_TIMER_0, (u16)(REG_OS_TMCNT_H_E_MASK | preScale));
}


/*---------------------------------------------------------------------------*
  Name:         OS_StopTimer

  Description:  stop timer(s)

  Arguments:    id         timerNo

  Returns:      None
 *---------------------------------------------------------------------------*/
//
//  stop a timer
//
void OS_StopTimer(OSTimer id)
{
    SDK_ASSERT(OS_TIMER_0 <= id && id <= OS_TIMER_3);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));

    OS_SetTimerControl(id, 0);
}

//
//  stop 2 timers
//
void OS_StopTimer32(OSTimer32 id)
{
    SDK_ASSERT(OS_TIMER32_01 <= id && id <= OS_TIMER32_23);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 1));

    OS_StopTimer((OSTimer)((int)id + 1));
    OS_StopTimer((OSTimer)id);
}

//
//  stop 3 timers
//
void OS_StopTimer48(OSTimer48 id)
{
    SDK_ASSERT(OS_TIMER48_012 <= id && id <= OS_TIMER48_123);
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(id));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 1));
    SDK_ASSERT(!OSi_IsTimerReserved(id + 2));

    OS_StopTimer((OSTimer)((int)id + 2));
    OS_StopTimer((OSTimer)((int)id + 1));
    OS_StopTimer((OSTimer)id);
}

//
//  stop all 4 timers
//
void OS_StopTimer64(void)
{
    //---- check if system reserved
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_0));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_1));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_2));
    SDK_ASSERT(!OSi_IsTimerReserved(OS_TIMER_3));

    OS_StopTimer(OS_TIMER_3);
    OS_StopTimer(OS_TIMER_2);
    OS_StopTimer(OS_TIMER_1);
    OS_StopTimer(OS_TIMER_0);
}
