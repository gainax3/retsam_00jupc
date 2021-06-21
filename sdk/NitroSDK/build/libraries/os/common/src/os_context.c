/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_context.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_context.c,v $
  Revision 1.30  2006/08/09 06:53:50  takano_makoto
  delete unused macro SDK_CP_NO_SAFE
  change CP_RestoreContext to CPi_RestoreContext

  Revision 1.29  2006/04/04 08:09:52  yada
  stack address in initializing context is set to 8byte-alignment

  Revision 1.28  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.27  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.26  2005/01/12 11:11:30  takano_makoto
  fix copyright header.

  Revision 1.25  2005/01/12 01:03:58  yada
  add about SDK_TCM_APPLY

  Revision 1.24  2004/11/02 09:51:24  yada
  just fix comment

  Revision 1.23  2004/06/15 12:39:24  yada
  only arrange tab

  Revision 1.22  2004/06/11 06:19:20  yada
  remove unused prototype declaration

  Revision 1.21  2004/06/03 04:21:24  yada
  move functionCost routine away

  Revision 1.20  2004/05/18 06:13:50  yada
  consider for thread interval of functionCost

  Revision 1.19  2004/05/06 10:50:00  takano_makoto
  Modify include file.

  Revision 1.18  2004/05/06 02:47:39  takano_makoto
  CP_Div & CP_Sqrt change to thread Safe.

  Revision 1.17  2004/02/09 11:17:05  yasu
  include code32.h

  Revision 1.16  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.15  2003/12/25 07:31:26  yada
  型ルール統一による変更

  Revision 1.14  2003/12/17 08:20:42  yasu
  OS_InitContext の引数変更

  Revision 1.13  2003/12/17 06:06:39  yasu
  定数名変更

  Revision 1.12  2003/12/16 07:15:19  yada
  スレッド範囲情報はコンテキストではなく、スレッド構造体に移した

  Revision 1.11  2003/12/16 00:16:51  yada
  threadに、スタック範囲情報を含めた。

  Revision 1.10  2003/12/12 04:40:19  yada
  プロセッサモードの定義変更に伴う修正

  Revision 1.9  2003/12/11 00:41:10  yasu
  include の変更 mmap_stack.h -> memorymap.h

  Revision 1.8  2003/12/08 10:02:37  yasu
  スタックサイズを mmap_stack.h から取得

  Revision 1.7  2003/12/08 01:25:05  yasu
  context に SP_svc を追加する

  Revision 1.6  2003/12/01 09:51:04  yasu
  context にセーブする値を PC->PC+4 へ

  Revision 1.5  2003/12/01 04:15:23  yasu
  マクロ USER_MODE -> NO_INTERWORKING に変更

  Revision 1.4  2003/11/30 05:11:05  yasu
  不要コードの削除

  Revision 1.3  2003/11/30 01:46:26  yasu
  OS_CreateThread 内での context->lr 書き換えへの対策

  Revision 1.2  2003/11/29 11:06:56  yasu
  OS_InitContext の修正

  Revision 1.1  2003/11/29 01:25:46  yada
  ファイル名称変更

  Revision 1.9  2003/11/28 10:30:00  yasu
  irq 用 OS_LoadContext のテスト

  Revision 1.8  2003/11/28 08:31:27  yada
  デバッグ中

  Revision 1.7  2003/11/28 02:52:13  yada
  OS_LoadContext() では、spsr_irq を経由し、ルーチンの終わりで movs r15, xxx ...
  を行い cpsr を変更するようにした。

  Revision 1.6  2003/11/27 13:42:30  yasu
  スレッドを USER->SYSTEM モードへ変更

  Revision 1.5  2003/11/25 09:29:26  yasu
  OS_IntrSwitchThread のコメントアウト

  Revision 1.4  2003/11/25 09:27:23  yasu
  SaveContext 修正，コメント追加, OS_DumpContext 追加

  Revision 1.3  2003/11/25 05:52:16  yada
  OS_SaveContext()、OS_LoadContext() 追加

  Revision 1.2  2003/11/25 05:03:05  yasu
  ヘッダファイル読み込みを os.h に変更

  Revision 1.1  2003/11/25 04:31:57  yasu
  初版追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/memorymap.h>

#ifdef SDK_ARM9
#include <nitro/cp/context.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_InitContext

  Description:  Initialize context

  Arguments:    context     context to be initialized
                newpc       program counter
                newsp       stack pointer

  Returns:      none
 *---------------------------------------------------------------------------*/
#include    <nitro/code32.h>
asm void OS_InitContext(
    register OSContext* context,
    register u32         newpc,
    register u32         newsp
    )
{
    // ---- 実行位置保存 (r0 = context)
    add   newpc, newpc, #4
    str   newpc, [ context, #OS_CONTEXT_PC_PLUS4 ]
    
    // ---- スタック保存
#ifdef  SDK_CONTEXT_HAS_SP_SVC
    str   newsp, [ context, #OS_CONTEXT_SP_SVC ]
    sub   newsp, newsp,     #HW_SVC_STACK_SIZE
#endif
    tst   newsp, #4
    subne newsp, newsp, #4 // for 8byte-alignment
    str   newsp, [ context, #OS_CONTEXT_SP ]
        
    // ---- ステータス作成
    ands  r1, newpc, #1
    movne r1, #HW_PSR_SYS_MODE|HW_PSR_THUMB_STATE
    moveq r1, #HW_PSR_SYS_MODE|HW_PSR_ARM_STATE
    str   r1, [ context, #OS_CONTEXT_CPSR ]
    
    // ---- 他のレジスタをクリア
    mov   r1, #0
    str   r1, [ context, #OS_CONTEXT_R0 ]
    str   r1, [ context, #OS_CONTEXT_R1 ]
    str   r1, [ context, #OS_CONTEXT_R2 ]
    str   r1, [ context, #OS_CONTEXT_R3 ]
    str   r1, [ context, #OS_CONTEXT_R4 ]
    str   r1, [ context, #OS_CONTEXT_R5 ]
    str   r1, [ context, #OS_CONTEXT_R6 ]
    str   r1, [ context, #OS_CONTEXT_R7 ]
    str   r1, [ context, #OS_CONTEXT_R8 ]
    str   r1, [ context, #OS_CONTEXT_R9 ]
    str   r1, [ context, #OS_CONTEXT_R10 ]
    str   r1, [ context, #OS_CONTEXT_R11 ]
    str   r1, [ context, #OS_CONTEXT_R12 ]
    str   r1, [ context, #OS_CONTEXT_LR  ]

    bx    lr    // start here and swicth arm/thumb mode
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         OS_SaveContext

  Description:  Save current context into specified memory

  Arguments:    context   pointer to the memory to be stored the current context

  Returns:      0    saving a context (normal)
                1    if context are reloaded via OS_LoadContext
 *---------------------------------------------------------------------------*/
asm BOOL OS_SaveContext( register OSContext* context )
{
#if defined(SDK_ARM9)
    stmfd   sp!, { lr, r0 }
    add     r0, r0, #OS_CONTEXT_CP_CONTEXT
    ldr     r1, =CP_SaveContext
    blx     r1
    ldmfd   sp!, { lr, r0 }
#endif

    add     r1, r0, #OS_CONTEXT_CPSR
    
    //---- Save CPSR
    mrs     r2, cpsr
    str     r2, [ r1 ], #OS_CONTEXT_R0-OS_CONTEXT_CPSR  // r1 moved to context.r0
        
#ifdef  SDK_CONTEXT_HAS_SP_SVC
    //---- Save SP_svc
    mov     r0, #HW_PSR_SVC_MODE|HW_PSR_IRQ_DISABLE|HW_PSR_FIQ_DISABLE|HW_PSR_ARM_STATE
    msr     cpsr_c, r0
    str     sp, [ r1, #OS_CONTEXT_SP_SVC - OS_CONTEXT_R0 ]
    msr     cpsr_c, r2
#endif

    //---- Save others
    mov     r0, #1              // return value via OS_LoadContext
    stmia   r1, {r0-r14}        // save R0-R14
    add     r0, pc, #8          // set  PC_plus4 to do ("bx lr" + 4)
    str     r0, [r1, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_R0 ]
    
    mov     r0, #0              // regular return value
    bx      lr
}


#define OFFSETOF(x,y) (int)(&(((x*)0)->y))

/*---------------------------------------------------------------------------*
  Name:         OS_LoadContext

  Description:  Reload specified context as current context

  Arguments:    context   pointer to the memory to switch to the context

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_LoadContext( register OSContext* context )
{
#if OS_CONTEXT_CPSR != 0
#pragma message(has changed!!!)
    add   r0, r0, #OS_CONTEXT_CPSR
#endif
    
#if defined(SDK_ARM9)
    // call CPi_RestoreContext
    stmfd     sp!, { lr, r0 }
    add       r0, r0, #OS_CONTEXT_CP_CONTEXT

    ldr       r1, =CPi_RestoreContext
    blx       r1
    ldmfd     sp!, { lr, r0 }
#endif
    
    //---- モードを svc に
    mrs       r1, cpsr
    bic       r1, r1, #HW_PSR_CPU_MODE_MASK
    orr       r1, r1, #HW_PSR_SVC_MODE|HW_PSR_IRQ_DISABLE|HW_PSR_FIQ_DISABLE
    msr       cpsr_c, r1

    //---- Load cpcr to spsr
    ldr       r1, [ r0 ], #OS_CONTEXT_R0-OS_CONTEXT_CPSR
    msr       spsr_fsxc, r1

#ifdef  SDK_CONTEXT_HAS_SP_SVC
    //---- Load SP_svc
    ldr       sp, [ r0, #OS_CONTEXT_SP_SVC   - OS_CONTEXT_R0 ]
#endif

    //---- Load r0-r14
    ldr       lr, [ r0, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_R0 ]
    ldmia     r0, { r0 - r14 }^
    nop
    
#if defined(SDK_ARM9)
#if 0 // don't need, because still spend more than 34 cycle for divider.
    //---- CP_WaitDiv
    stmfd     sp!, { r0-r1 }
    ldr       r0, =REG_DIVCNT_ADDR
@00:
    ldr       r1, [r0]
    and       r1, r1, #REG_CP_DIVCNT_BUSY_MASK
    bne       @00
    ldmfd     sp!, { r0-r1 }
#endif
#endif

    //---- Switch cpsr and Jump to (context->pc_plus4 - 4)
    subs      pc, lr, #4
}
#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif

#include    <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OS_DumpContext

  Description:  Performs exception initialization.
                - installs the first level exception handlers
                - set up exception table and common exception handler

  Arguments:    installDBIntegrator if TRUE, copy OSDBIntegrator into
                                    low memory.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_DumpContext(OSContext *context)
{
#ifndef SDK_FINALROM
    s32     i;

    OS_Printf("context=%08x\n", context);
    if (context)
    {
        OS_Printf("CPSR  %08x\n", context->cpsr);
        for (i = 0; i < 13; i++)
        {
            OS_Printf("R%02d   %08x\n", i, context->r[i]);
        }
        OS_Printf("SP    %08x\n", context->sp);
        OS_Printf("LR    %08x\n", context->lr);
        OS_Printf("PC+4  %08x\n", context->pc_plus4);
#ifdef  SDK_CONTEXT_HAS_SP_SVC
        OS_Printf("SPsvc %08x\n", context->sp_svc);
#endif
    }
#else
    (void)context;                     // avoiding to unused warning
#endif
}
