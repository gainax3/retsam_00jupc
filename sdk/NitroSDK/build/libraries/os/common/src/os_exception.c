/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_exception.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_exception.c,v $
  Revision 1.41  2007/03/26 00:06:13  yasu
  著作年度修正

  Revision 1.40  2007/02/23 02:29:07  yosizaki
  small fix about OSi_DisplayExContext.

  Revision 1.39  2007/02/23 01:10:50  yosizaki
  add OS_EnableUserExceptionHandlerOnDebugger.

  Revision 1.38  2006/05/24 07:54:06  kitase_hirotake
  例外ハンドラの設定条件の変更

  Revision 1.37  2006/05/08 09:26:16  okubata_ryoma
  FINALROMの場合はデバッガ上でもOS_CONSOLE_NITROを返すことに関する修正

  Revision 1.36  2006/04/04 06:05:53  yosizaki
  コールスタックの8バイト整合に関する修正.

  Revision 1.35  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.34  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.33  2004/11/02 10:35:45  yada
  just fix comment

  Revision 1.32  2004/10/03 09:59:16  yasu
  Avoid sdkindent in inline-asm{}

  Revision 1.31  2004/09/02 02:53:42  yada
  only change comment

  Revision 1.30  2004/08/26 00:20:10  yada
  change method to distinguish running on debugger

  Revision 1.29  2004/06/11 06:24:43  yada
  close SDK_BB support

  Revision 1.28  2004/06/10 06:25:06  yada
  consider for occurring exception in THUMB mode

  Revision 1.27  2004/06/07 10:56:35  yada
  make difference to set handler according to exec environment

  Revision 1.26  2004/05/12 11:00:48  yada
  OSi_ExPrintf always uses OS_Printf

  Revision 1.25  2004/05/12 05:34:29  yada
  some fix

  Revision 1.24  2004/05/12 05:31:46  yada
  modification for DEBUGGER

  Revision 1.23  2004/05/07 02:11:06  takano_makoto
  Change OSiExContext offset definition.

  Revision 1.22  2004/04/16 09:36:19  yada
  fix temporary exception vector
  0x27fffd9c->0x027ffd9c

  Revision 1.21  2004/04/14 12:48:20  yada
  IS-NITRO-DEBUGGERへの暫定対応

  Revision 1.20  2004/03/30 06:09:29  yada
  ldconst や lda を ldr = を使用する記述にした。

  Revision 1.19  2004/03/18 12:56:43  yada
  例外ベクタ設定のミス修正

  Revision 1.18  2004/03/11 09:47:14  yada
  例外時にFIQ,IRQ割り込み禁止していたのをIRQ割り込み禁止のみにした。

  Revision 1.17  2004/03/09 02:37:59  yada
  debuggerモニタハンドラ呼び出し判定部分修正

  Revision 1.16  2004/03/08 02:54:13  yada
  デバッガモニタの例外ハンドラを考慮

  Revision 1.15  2004/03/01 11:54:24  yada
  英語の関数comment作成

  Revision 1.14  2004/02/19 08:43:05  yada
  ユーザ例外ルーチンの型追加

  Revision 1.13  2004/02/17 06:04:38  yada
  ARM7への対応

  Revision 1.12  2004/02/16 09:40:12  yada
  ARM7側の割り込みもフックするようにした。

  Revision 1.11  2004/02/16 00:49:30  yasu
  Don't switch ISD/noISD by SDK_LINK_ISD.
  Please use stubs library to resolve it at link time.

  Revision 1.10  2004/02/13 04:05:28  yada
  ARM9とAMR7 の場合わけ考慮

  Revision 1.9  2004/02/12 11:08:30  yada
  ARM9/ARM7 振り分け作業

  Revision 1.8  2004/02/09 11:17:22  yasu
  include code32.h

  Revision 1.7  2004/02/05 10:50:59  yada
  未定義命令で停止するようにした

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  2004/02/02 05:34:54  yada
  OS_SetIrqMask()→OS_EnableIrqMask() へ

  Revision 1.4  2004/01/30 08:05:36  yada
  FINALROMでは例外表示をしないように修正

  Revision 1.3  2004/01/30 04:31:47  yada
  例外表示の組み込み

  Revision 1.2  2004/01/16 01:18:05  yasu
  Support SDK_BB

  Revision 1.1  2004/01/14 12:03:05  yada
  初版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/hw/common/armArch.h>
#include <nitro/memorymap.h>
#include <nitro/os.h>


//---- displaying function OSi_ExPrintf
#ifdef SDK_ARM9
#define OSi_ExPrintf OS_Printf
//#define OSi_ExPrintf ISDPrintf
//void ISDPrintf( const char *pBuf, ... );
#else
#define OSi_ExPrintf OS_Printf
#endif


static asm void OSi_ExceptionHandler( void );
static asm void OSi_GetAndDisplayContext( void );
static asm void OSi_SetExContext( void );
static     void OSi_DisplayExContext( void );


//---- context for exception display
typedef struct
{
    OSContext   context;
    u32         cp15;
    u32         spsr;
    u32         exinfo;
    u32         debug[4];
} OSiExContext;

static OSiExContext OSi_ExContext;

//---- user's exception handler
static OSExceptionHandler OSi_UserExceptionHandler;
static void *OSi_UserExceptionHandlerArg;

//---- debugger's exception handler
static void *OSi_DebuggerHandler = NULL;

//================================================================================
//  exception vector
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorUpper

  Description:  set exception vector to high address

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void OS_SetExceptionVectorUpper( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #HW_C1_EXCEPT_VEC_UPPER
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetExceptionVectorLower

  Description:  set exception vector to low address

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_SetExceptionVectorLower( void )
{
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #HW_C1_EXCEPT_VEC_UPPER
    mcr     p15, 0, r0, c1, c0, 0
    bx      lr
}
#include <nitro/codereset.h>


//================================================================================
//  exception handling
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitException

  Description:  Initialize exception handling system

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/

// 将来的には HW_EXCP_VECTOR_BUF をこの値に。
#define HW_EXCP_VECTOR_BUF_FOR_DEBUGGER  0x027ffd9c

void OS_InitException(void)
{
    //---- consider for debugger exception handler
    if (0x2600000 <= *(u32 *)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER
        && *(u32 *)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER < 0x2800000)
    {
        OSi_DebuggerHandler = *(void **)HW_EXCP_VECTOR_BUF_FOR_DEBUGGER;
    }
    else
    {
        OSi_DebuggerHandler = NULL;
    }

    //---- exception hook
    if (!OSi_DebuggerHandler)
    {
        *(u32 *)(HW_EXCP_VECTOR_BUF_FOR_DEBUGGER) = (u32)OSi_ExceptionHandler;

        // for not-debugger, store handler address into original hook
        *(u32 *)(HW_EXCP_VECTOR_BUF) = (u32)OSi_ExceptionHandler;
    }

    //---- user's handler
    OSi_UserExceptionHandler = NULL;
}

#if defined(SDK_ARM9)
/*
 * デバッガへ制御を渡す前に自分で判定してユーザハンドラを呼び出す.
 * ただし以下の未定義命令はブレークポイントなので無視する.
 * +-------------------+------------+--------+
 * |     debugger      |    ARM     | THUMB  |
 * +-------------------+------------+--------+
 * | CodeWarrior -1.1  | 0xE6000010 | 0xBE00 |
 * | CodeWarrior  1.2- | 0xE7FDDEFE | 0xDEFE |
 * | IS-NITRO-DEBUGGER | 0xE7FFFFFF | 0xEFFF |
 * +-------------------+------------+--------+
 */
/*---------------------------------------------------------------------------*
  Name:         OSi_DebuggerExceptionHook

  Description:  exception hook which calls user-handler before debugger-handler.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
static u32 OSi_ExceptionHookStack[8];
asm static void OSi_DebuggerExceptionHook(void)
{
    /* spを変更しないよう注意深くワークレジスタ退避 */
    ldr       r12, =OSi_ExceptionHookStack
    stmia     r12, {r0-r4,sp,lr}
    mrs       r4, CPSR
    /*
     * ユーザへ通知すべき例外かどうか判定.
     * - ユーザへ通知すべき例外はABT(アボート)またはUND(未定義命令).
     * - UNDはブレークポイントによって発生する可能性がある.
     * - UNDかつブレークポイント命令であればユーザへ通知しない.
     */
    mrs       r0, CPSR
    and       r0, r0, #0x1f
    teq       r0, #0x17
    beq       user_exception
    teq       r0, #0x1b
    bne       user_exception_end
is_und:
    /* ARM/THUMBおよびIS-NITRO-DEBUGGER/CodeWarriorごとの判定 */
    bic       r0, sp, #1
    ldr       r1, [r0, #4]
    ldr       r0, [r0, #12]
    tst       r1, #0x20
    beq       is_und_arm
is_und_thumb:
    bic       r0, r0, #1
    ldrh      r0, [r0, #-2]
    ldr       r1, =0x0000EFFF
    cmp       r0, r1
    beq       user_exception_end
    ldr       r1, =0x0000DEFE
    cmp       r0, r1
    beq       user_exception_end
    ldr       r1, =0x0000BE00
    cmp       r0, r1
    beq       user_exception_end
    b         user_exception
is_und_arm:
    bic       r0, r0, #3
    ldr       r0, [r0, #-4]
    ldr       r1, =0xE7FFFFFF
    cmp       r0, r1
    beq       user_exception_end
    ldr       r1, =0xE7FDDEFE
    cmp       r0, r1
    beq       user_exception_end
    ldr       r1, =0xE6000010
    cmp       r0, r1
    beq       user_exception_end
    b         user_exception
user_exception:
    /* ユーザの例外ハンドラへ通知 */
    ldmia     r12, {r0-r1}
    ldr       r12, =HW_ITCM_END
    stmfd     r12!, {r0-r3,sp,lr}
    and       r0, sp, #1
    mov       sp, r12
    bl        OSi_GetAndDisplayContext
    ldmfd     sp!, {r0-r3,r12,lr}
    mov       sp, r12
user_exception_end:
    /* 退避したワークレジスタを復元してデバッガへ本来の制御を渡す */
    msr       CPSR_cxsf, r4
    ldr       r12, =OSi_ExceptionHookStack
    ldmia     r12, {r0-r4,sp,lr}
    ldr       r12, =OSi_DebuggerHandler
    ldr       r12, [r12]
    cmp       r12, #0
    bxne      r12
    bx        lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_EnableUserExceptionHandlerOnDebugger

  Description:  enable user exception handler even if running on the debugger.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_EnableUserExceptionHandlerOnDebugger(void)
{
    if (OSi_DebuggerHandler)
    {
        *(u32 *)(HW_EXCP_VECTOR_BUF_FOR_DEBUGGER) = (u32)OSi_DebuggerExceptionHook;
    }
}
#endif /* defined(SDK_ARM9) */

/*---------------------------------------------------------------------------*
  Name:         OS_SetUserExceptionHandler

  Description:  set user exception handler and its argument

  Arguments:    handler    exception hander
                arg        its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetUserExceptionHandler(OSExceptionHandler handler, void *arg)
{
    OSi_UserExceptionHandler = handler;
    OSi_UserExceptionHandlerArg = arg;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ExceptionHandler

  Description:  system exception handler
                user handler is called from here

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//
//      status of stack is {cp15,spsr,r12,lr}
//      LSB = 1 means coming by reset
//
#include <nitro/code32.h>
asm void OSi_ExceptionHandler( void )
{
    //---- call debugger monitor handler (if exists)
    ldr       r12, =OSi_DebuggerHandler // r12のみ破壊可
    ldr       r12, [r12]
    cmp       r12, #0
    movne     lr, pc
    bxne      r12

    //---- setting stack pointer <------------------------- consider later
#ifdef SDK_ARM9
    //---- ARM9 stack
    ldr       r12, =HW_ITCM_END
#else
    //---- ARM7 stack
    ldr       r12, =0x3806000
#endif
    stmfd     r12!, {r0-r3,sp,lr}

    and       r0, sp, #1
    mov       sp, r12
    
    mrs       r1, CPSR
    and       r1, r1, #0x1f

    //---- if ABORT exception, stop
    teq       r1, #0x17
    bne       @10
    bl        OSi_GetAndDisplayContext
    b         usr_return

@10:
    //---- if UNDEF exception, stop
    teq       r1, #0x1b
    bne       usr_return
    bl        OSi_GetAndDisplayContext

usr_return:
    ldr       r12, =OSi_DebuggerHandler
    ldr       r12, [r12]
    cmp       r12, #0
@1: beq       @1

//( stop now even if debugger )
@2: 
    mov       r0,r0 // nop
    b         @2

    ldmfd     sp!, {r0-r3, r12, lr}
    mov       sp, r12
    bx        lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetAndDisplayContext

  Description:  stop after displaying registers

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static asm void OSi_GetAndDisplayContext( void )
{
    stmfd     sp!, {r0, lr} /* コールスタックを 8 バイト整合 */

    //---- set exception context 
    bl        OSi_SetExContext
    //---- display exception context (and call user callback)
    bl        OSi_DisplayExContext

    ldmfd     sp!, {r0, lr} /* コールスタックを 8 バイト整合 */
    bx        lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetExContext

  Description:  set context when exception occurred

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
//       explanation registers at the top of this function.
//
//       in r12, {r0-r3,sp} (sp is exception sp) stored.
//       in this sp, {cp15,spsr,r12,lr} (cp15,spsr,r12,lr is registers when exception occurred) stored.
//       r4-r11 is registers then exception occurred.
//       if you want to know spsr,sp,lr, please switch bank and read.
//
static asm void OSi_SetExContext( void )
{
    //---- pointer ExContext structure
    ldr    r1, =OSi_ExContext;

    mrs    r2, CPSR
    str    r2,  [r1, #OSiExContext.debug[1] ]

    //---- store bit which means which is the reason, reset or exception
    str    r0, [r1, #OSiExContext.exinfo ]

    //---- store r0 - r3
    ldr    r0, [r12,#0]
    str    r0, [r1, #OS_CONTEXT_R0]
    ldr    r0, [r12,#4]
    str    r0, [r1, #OS_CONTEXT_R1]
    ldr    r0, [r12,#8]
    str    r0, [r1, #OS_CONTEXT_R2]
    ldr    r0, [r12, #12]
    str    r0, [r1, #OS_CONTEXT_R3]
    ldr    r2, [r12, #16]
    bic    r2, r2, #1

    //---- store r4 - r11
    add    r0, r1, #OS_CONTEXT_R4
    stmia  r0, {r4-r11}

    //---- (for debug)
    str    r12, [r1, #OSiExContext.debug[0] ]

#ifdef SDK_ARM9
    //---- get {cp15,cpsr,r12,pc} from stack
    ldr    r0, [r2, #0]
    str    r0, [r1, #OSiExContext.cp15 ]
    ldr    r3, [r2, #4]
    str    r3, [r1, #OS_CONTEXT_CPSR]
    ldr    r0, [r2, #8]
    str    r0, [r1, #OS_CONTEXT_R12]
    ldr    r0, [r2, #12]
    str    r0, [r1, #OS_CONTEXT_PC_PLUS4]
#else // ifdef SDK_ARM9
    //---- get {cpsr,r12,pc} from stack
    mov    r0, #0
    str    r0, [r1, #OSiExContext.cp15]
    ldr    r3, [r2, #0]
    str    r3, [r1, #OS_CONTEXT_CPSR]
    ldr    r0, [r2, #4]
    str    r0, [r1, #OS_CONTEXT_R12]
    ldr    r0, [r2, #8]
    str    r0, [r1, #OS_CONTEXT_PC_PLUS4]
#endif // ifdef SDK_ARM9

    //---- set mode to one which exception occurred
    //     but, disable IRQ
    mrs    r0, CPSR
    orr    r3, r3, #0x80
    bic    r3, r3, #0x20
    msr    CPSR_cxsf, r3
   
    //---- get sp, lr, spsr
    str    sp, [r1, #OS_CONTEXT_R13]
    str    lr, [r1, #OS_CONTEXT_R14]
    mrs    r2, SPSR

    //---- debugger
    str    r2, [r1, #OSiExContext.debug[3] ]

    //---- restore mode
    msr    CPSR_cxsf, r0
    bx     lr
}

/*---------------------------------------------------------------------------*
  Name:         OSi_DisplayExContext

  Description:  stop after display exception context

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_DisplayExContext()
{
#ifndef SDK_FINALROM
    int     i;

    OSi_ExPrintf("**** Exception Occurred ****\n");

    //---------------- displaying registers
    //---- for R0-15 Registers
    for (i = 0; i < 13; i++)
    {
        OSi_ExPrintf("R%02d=%08X  %c", i, OSi_ExContext.context.r[i], ((i & 3) == 3) ? '\n' : ' ');
    }
    OSi_ExPrintf("SP =%08X   ", OSi_ExContext.context.sp);
    OSi_ExPrintf("LR =%08X   ", OSi_ExContext.context.lr);
    OSi_ExPrintf("PC =%08X\n", OSi_ExContext.context.pc_plus4);

    //---- for status Registers
#ifdef SDK_ARM9
    OSi_ExPrintf("  CPSR=%08X  SPSR=%08X  CP15=%08X\n",
                 OSi_ExContext.context.cpsr, OSi_ExContext.spsr, OSi_ExContext.cp15);
#else
    OSi_ExPrintf("  CPSR=%08X  SPSR=%08X\n", OSi_ExContext.context.cpsr, OSi_ExContext.spsr);
#endif

    //---- for Debug Values
#if 0
    for (i = 0; i < 4; i++)
    {
        OSi_ExPrintf("DEBUG%02d=%08X  ", i, OSi_ExContext.debug[i]);
    }
#endif
    OSi_ExPrintf("\n\n");
#endif

    //---------------- user's callback
    if (OSi_UserExceptionHandler)
    {
        //---- force to become SYS mode
        //     but, use current stack not SYS mode stack
        asm
        {
          /* *INDENT-OFF* */
          /* sp,cpsrを復帰させていなかった点などを全体修正 */
          mrs      r2, CPSR
          mov      r0, sp
          ldr      r1, =0x9f
          msr      CPSR_cxsf, r1
          mov      r1, sp
          mov      sp, r0
          stmfd    sp!, {r1,r2}
#ifdef SDK_ARM9
          bl       OS_EnableProtectionUnit
#endif
          ldr      r0, =OSi_ExContext
          ldr      r1, =OSi_UserExceptionHandlerArg
          ldr      r1, [r1]
          ldr      r12, =OSi_UserExceptionHandler
          ldr      r12, [r12]
          ldr      lr, =@1
          bx       r12
@1:
#ifdef SDK_ARM9
          bl       OS_DisableProtectionUnit
#endif
          ldmfd    sp!, {r1,r2}
          mov      sp, r1
          msr      CPSR_cxsf, r2
          /* *INDENT-ON* */
        }
    }

}

#include <nitro/codereset.h>
