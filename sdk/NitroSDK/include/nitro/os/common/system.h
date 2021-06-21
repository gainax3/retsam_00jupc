/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     system.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: system.h,v $
  Revision 1.30  2006/01/18 02:12:28  kitase_hirotake
  do-indent

  Revision 1.29  2005/10/07 08:22:03  yada
  add OS_WaitVBlankIntr()

  Revision 1.28  2005/08/31 12:03:27  yada
  add OS_Exit()

  Revision 1.27  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.26  2004/11/24 04:36:10  yada
  just fix typo

  Revision 1.25  2004/09/02 06:27:05  yada
  only change comment

  Revision 1.24  2004/05/24 04:48:29  yada
  OS_WaitIntr() -> OS_WaitInterrupt()

  Revision 1.23  2004/05/10 11:21:49  yada
  add OS_WaitIntr()

  Revision 1.22  2004/04/07 02:03:17  yada
  fix header comment

  Revision 1.21  2004/04/06 01:38:13  yada
  OS_Halt() の挙動変更

  Revision 1.20  2004/03/04 09:48:20  yada
  OSDisableInterrupt() を IRQ 禁止にした。
  IRQ/FIQ 禁止には別関数を用意した。

  Revision 1.19  2004/02/27 01:55:00  yasu
  add OS_SpinWait, move OS_Halt into os_system.c

  Revision 1.18  2004/02/13 08:45:00  yasu
  clean up comments

  Revision 1.17  2004/02/13 02:16:43  yada
  EnableInterrupts, GetProcMode for ARM7 (temporary)

  Revision 1.16  2004/02/13 01:51:33  yada
  DisableInterrupts, RestoreInterrupts for ARM7 (temporary)

  Revision 1.15  2004/02/12 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.14  2004/02/10 12:11:23  yada
  OS_GetDTCMAddress, OS_GetITCMAddress を tcm.hに移行

  Revision 1.13  2004/02/10 11:35:06  yada
  only arrange tab & space

  Revision 1.12  2004/02/10 11:19:21  yada
  Interrupt 関係のinline をやめる

  Revision 1.11  2004/02/09 11:38:18  yada
  OS_GetDTCMAddress, OS_GetITCMAddress 作成

  Revision 1.10  2004/02/09 11:17:22  yasu
  include code32.h

  Revision 1.9  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.8  2004/01/18 04:47:31  yada
  インデント等の整形

  Revision 1.7  2004/01/17 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.6  2004/01/14 08:41:19  yada
  OSIntrMode_Irq に伴う変更

  Revision 1.5  2004/01/14 01:41:35  yada
  CPSRのinterrupt 操作を IRQのみから IRQとFIQ に変更

  Revision 1.4  2003/12/25 07:30:07  yada
  型ルール統一による変更

  Revision 1.3  2003/12/22 13:41:45  yasu
  include ガードの修正

  Revision 1.2  2003/12/12 05:06:39  yada
  プロセッサモードの定義変更に伴う修正

  Revision 1.1  2003/12/12 01:25:31  yada
  os/cpsr.h → os/system.h と名称変更

  Revision 1.7  2003/12/08 12:08:02  yada
  SDK_ASMが定義されているときは define以外含まれないようにした

  Revision 1.6  2003/12/08 01:25:05  yasu
  context に SP_svc を追加する

  Revision 1.5  2003/12/01 09:51:56  yasu
  warning 対策のため DEBUG 時のみ変数の初期化を行なう

  Revision 1.4  2003/12/01 04:15:53  yasu
  OS_RestoreInterrupts の実装誤り修正

  Revision 1.3  2003/11/30 10:24:09  yasu
  前回の修正で入れてしまったバグの修正

  Revision 1.2  2003/11/30 10:13:40  yasu
  asm 表現法の変更

  Revision 1.1  2003/11/30 04:17:43  yasu
  新規作成 CPU CPSR 設定関係の関数

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_OS_SYSTEM_H_
#define NITRO_OS_SYSTEM_H_

#ifndef SDK_ASM
#include  <nitro/types.h>
#include  <nitro/os/common/systemCall.h>
#endif

#include  <nitro/hw/common/armArch.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
#ifndef SDK_ASM
#define		OS_EXIT_STRING_1		"*** Exit nitro program (status="
#define		OS_EXIT_STRING_2		"%d).\n"
#define		OS_EXIT_STRING			OS_EXIT_STRING_1 OS_EXIT_STRING_2

typedef enum
{
    OS_PROCMODE_USER = HW_PSR_USER_MODE,
    OS_PROCMODE_FIQ = HW_PSR_FIQ_MODE,
    OS_PROCMODE_IRQ = HW_PSR_IRQ_MODE,
    OS_PROCMODE_SVC = HW_PSR_SVC_MODE,
    OS_PROCMODE_ABORT = HW_PSR_ABORT_MODE,
    OS_PROCMODE_UNDEF = HW_PSR_UNDEF_MODE,
    OS_PROCMODE_SYS = HW_PSR_SYS_MODE
}
OSProcMode;


typedef enum
{
    OS_INTRMODE_IRQ_DISABLE = HW_PSR_IRQ_DISABLE,
    OS_INTRMODE_IRQ_ENABLE = 0
}
OSIntrMode_Irq;

typedef enum
{
    OS_INTRMODE_FIQ_DISABLE = HW_PSR_FIQ_DISABLE,
    OS_INTRMODE_FIQ_ENABLE = 0
}
OSIntrMode_Fiq;

typedef u32 OSIntrMode;

//============================================================================
//          CONTROL INTERRUPT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableInterrupts

  Description:  Set CPSR to enable irq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_EnableInterrupts(void);

/*---------------------------------------------------------------------------*
  Name:         OS_DisableInterrupts

  Description:  Set CPSR to disable irq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_DisableInterrupts(void);

/*---------------------------------------------------------------------------*
  Name:         OS_RestoreInterrupts

  Description:  Restore CPSR irq interrupt

  Arguments:    state of irq interrupt bit

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_RestoreInterrupts(OSIntrMode state);

/*---------------------------------------------------------------------------*
  Name:         OS_EnableInterrupts_IrqAndFiq

  Description:  Set CPSR to enable irq and fiq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_EnableInterrupts_IrqAndFiq(void);

/*---------------------------------------------------------------------------*
  Name:         OS_DisableInterrupts_IrqAndFiq

  Description:  Set CPSR to disable irq and fiq interrupts

  Arguments:    None.

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_DisableInterrupts_IrqAndFiq(void);

/*---------------------------------------------------------------------------*
  Name:         OS_RestoreInterrupts_IrqAndFiq

  Description:  Restore CPSR irq and fiq interrupt

  Arguments:    state of irq and fiq interrupt bit

  Returns:      last state of HW_PSR_IRQ_DISABLE & HW_PSR_FIQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode state);

/*---------------------------------------------------------------------------*
  Name:         OS_GetCpsrIrq

  Description:  Get current state of CPSR irq interrupt

  Arguments:    None

  Returns:      last state of HW_PSR_IRQ_DISABLE
 *---------------------------------------------------------------------------*/
extern OSIntrMode_Irq OS_GetCpsrIrq(void);

//============================================================================
//          PROCESSER MODE
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetProcMode

  Description:  Get processor mode from CPSR

  Arguments:    None.

  Returns:      CPU processor mode (field 0x10-0x1f)
 *---------------------------------------------------------------------------*/
extern OSProcMode OS_GetProcMode(void);


//============================================================================
//          TERMINATE and HALT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_Terminate

  Description:  Halt CPU and loop
  
  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
extern void OS_Terminate(void);


/*---------------------------------------------------------------------------*
  Name:         OS_Halt

  Description:  Halt CPU
  
  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9
extern void OS_Halt(void);
#else
static inline void OS_Halt(void)
{
    SVC_Halt();
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_Exit

  Description:  Display exit string and Terminate.
                This is useful for 'loadrun' tool command.                 
  
  Arguments:    status : exit status

  Returns:      --  (Never return)
 *---------------------------------------------------------------------------*/
extern void OS_Exit(int status);

//============================================================================
//          WAIT
//============================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SpinWait

  Description:  Spin and Wait for specified CPU cycles at least

  Arguments:    cycles      waiting CPU cycle

  Returns:      None
 *---------------------------------------------------------------------------*/
extern void OS_SpinWait(u32 cycle);

/*---------------------------------------------------------------------------*
  Name:         OS_WaitInterrupt

  Description:  wait specified interrupt
                the difference between OS_WaitIrq and OS_WaitInterrupt,
                in waiting interrupt
                OS_WaitIrq does switch thread,
                OS_WaitInterrupt doesn't switch thread.
                OS_WaitIrq wait by using OS_SleepThread() with threadQueue,
                OS_WaitInterrupt wait by using OS_Halt().
                if SDK_NO_THREAD defined, 2 functions become same.

  Arguments:    clear      TRUE if want to clear interrupt flag before wait.
                           FALSE if not.
                irqFlags   bit of interrupts to wait for

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifndef OSi_OSIRQMASK_DEFINED
typedef u32 OSIrqMask;
#define OSi_OSIRQMASK_DEFINED
#endif
extern void OS_WaitInterrupt(BOOL clear, OSIrqMask irqFlags);

/*---------------------------------------------------------------------------*
  Name:         OS_WaitVBlankIntr

  Description:  wait till vblank interrupt occurred.
				the difference between SVC_WaitVBlankIntr and OS_WaitVBlankIntr is:
                OS_WaitVBlankIntr does switch thread,
                SVC_WaitVBlankIntr doesn't switch thread.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
extern void OS_WaitVBlankIntr(void);


#endif /* SDK_ASM */

#ifdef __cplusplus
} /* extern "C" */
#endif

/*NITRO_OS_SYSTEM_H_*/
#endif
