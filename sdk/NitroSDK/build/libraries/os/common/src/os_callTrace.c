/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_callTrace.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_callTrace.c,v $
  Revision 1.26  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.25  2005/09/07 00:05:09  yada
  consider for const OSThread*.

  Revision 1.24  2005/09/06 05:10:36  yada
  consider for const OSThread*.

  Revision 1.23  2005/03/04 11:09:27  yasu
  __declspec(weak) を SDK_WEAK_SYMBOL に置き換え

  Revision 1.22  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.21  2005/02/18 08:51:35  yasu
  著作年度変更

  Revision 1.20  2005/02/18 06:29:02  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.19  2004/10/01 12:36:24  yada
  %08% -> %08x
  fix underflow check in __PROFILE_EXIT

  Revision 1.18  2004/06/07 00:15:43  yada
  small fix

  Revision 1.17  2004/06/03 06:59:41  yada
  fix indent processing

  Revision 1.16  2004/06/03 04:20:25  yada
  member 'calltrace' becomes 'profiler'

  Revision 1.15  2004/05/24 04:49:46  yada
  fix profile pragma

  Revision 1.14  2004/05/20 04:29:56  yada
  consider for SDK_NO_THREAD

  Revision 1.13  2004/05/12 11:30:23  yada
  change better at stack overfull displaying.

  Revision 1.12  2004/05/12 10:58:53  yada
  optional r0,r1,r2,r3 each

  Revision 1.11  2004/05/12 05:08:33  yada
  modify SDK_XXX / OS_XXX option
  make __PROFILE_ENTRY/EXIT be weak

  Revision 1.10  2004/05/10 11:46:25  yada
  consider for functionCost

  Revision 1.9  2004/04/26 01:45:13  yada
  OSi->OS: about OS_Enable/Disable/RestoreCallTrace

  Revision 1.8  2004/04/23 00:29:38  yada
  logモードに対応

  Revision 1.7  2004/04/15 02:26:38  yada
  add code32 include

  Revision 1.6  2004/04/15 01:05:59  yada
  small fix: (display string, how to get lr)

  Revision 1.5  2004/04/14 02:57:50  yada
  CALLSTACK->CALLTRACE

  Revision 1.4  2004/04/14 00:04:29  yada
  fix to make in THUMB

  Revision 1.3  2004/04/13 12:08:28  yada
  ASSERT定義ミス修正

  Revision 1.2  2004/04/13 06:21:21  yada
  dump修正

  Revision 1.1  2004/04/12 12:12:08  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

#if defined(OS_PROFILE_AVAILABLE) && defined(OS_PROFILE_CALL_TRACE)

#ifdef OS_NO_CALLTRACE
//---- dummy function
SDK_WEAK_SYMBOL asm void __PROFILE_ENTRY( void ){  bx   lr }
SDK_WEAK_SYMBOL asm void __PROFILE_EXIT( void ){   bx   lr }
void OS_DumpThreadCallTrace(const OSThread *)
{
}
#else  // ifdef OS_NO_CALLTRACE

//---- function prototype declaration
OSCallTraceInfo *OSi_GetCallTraceInfo(void);
void    OSi_DumpCurrentLr(u32 lr, int strIndex);
void    OSi_DumpOneInfo(OSCallTrace *p);
void    OSi_DumpFullInfo(OSCallTraceInfo *info);
void    OSi_DumpCallTrace_core(u32 returnAddress);
void    OSi_DumpThreadCallTrace_core(const OSThread *thread, u32 returnAddress);
void    OSi_Abort_CallTraceFull(u32 name, u32 returnAddress, u32 r0, u32 sp);
void    OSi_Abort_CallTraceLess(u32 returnAddress);

//---- pointer to callTraceInfo at not using threadSystem
static OSCallTraceInfo *OSi_DefaultCallTraceInfo;

#define OSi_STR_DUMPCALLTRACE          0
#define OSi_STR_DUMPTHREADCALLTRACE    1

extern BOOL OSi_IsThreadInitialized;

//================================================================================
//              ENABLE/DISABLE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableCallTrace

  Description:  enable callTrace

  Arguments:    None

  Returns:      previous status. TRUE if enable
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_EnableCallTrace( void )
{
    //---- check if callTrace not available
    stmfd   sp!, {lr}
    bl      OSi_GetCallTraceInfo
    ldmfd   sp!, {lr}
    cmp     r0, #0
    bxeq    lr

    mov     r2, r0
    ldrh    r0, [ r2, #OSCallTraceInfo.enable ]
    mov     r1, #1
    strh    r1, [ r2, #OSCallTraceInfo.enable ]

    bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_DisableCallTrace

  Description:  disble callTrace

  Arguments:    None

  Returns:      previous status. TRUE if enable
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_DisableCallTrace( void )
{
    //---- check if callTrace not available
    stmfd   sp!, {lr}
    bl      OSi_GetCallTraceInfo
    ldmfd   sp!, {lr}
    cmp     r0, #0
    bxeq    lr

    mov     r2, r0
    ldrh    r0, [ r2, #OSCallTraceInfo.enable ]
    mov     r1, #0
    strh    r1, [ r2, #OSCallTraceInfo.enable ]

    bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_RestoreCallTrace

  Description:  set status of callTrace

  Arguments:    enable :  TRUE if set to be enable, FALSE if not

  Returns:      previous status. TRUE if enable
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_RestoreCallTrace( BOOL )
{
    //---- check if callTrace not available
    stmfd   sp!, {r0, lr}
    bl      OSi_GetCallTraceInfo
    mov     r1, r0
    ldmfd   sp!, {r0, lr}
    cmp     r1, #0
    bxeq    lr

    ldrh    r2, [ r1, #OSCallTraceInfo.enable ]
    strh    r0, [ r1, #OSCallTraceInfo.enable ]
    mov     r0, r2

    bx      lr
}
#include <nitro/codereset.h>


//================================================================================
//              SETTING
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitCallTrace

  Description:  Initialize callTrace stack,
                and start recording trace.

  Arguments:    buf     address to record trace infomation
                size    size of buffer (by byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_InitCallTrace(void *buf, u32 size, OSCallTraceMode mode)
{
    OSCallTraceInfo **infoPtr;

    SDK_ASSERTMSG(((u32)buf & 3) == 0, "CallTrace buffer must be aligned by 4");
    SDK_ASSERTMSG((size >= OSi_TRACEINFO_SIZEOF_HEADERPART + sizeof(OSCallTrace)),
                  "to small CallTrace buffer");

#ifdef SDK_NO_THREAD
    //---- pointer to callTraceInfo is in static valiable
    infoPtr = &OSi_DefaultCallTraceInfo;
#else
    SDK_ASSERTMSG(OS_IsThreadAvailable(), "CallTrace need thread system.");
    //---- pointer to callTraceInfo is in OSThread structure
    infoPtr = (OSCallTraceInfo **)&(OS_GetCurrentThread()->profiler);
#endif

    *infoPtr = (OSCallTraceInfo *)buf;
    (*infoPtr)->current = (*infoPtr)->array;
    (*infoPtr)->limit = (OSCallTrace *)((u32)buf
                                        + OSi_TRACEINFO_SIZEOF_HEADERPART
                                        + OS_CalcCallTraceLines(size) * sizeof(OSCallTrace));
    (*infoPtr)->enable = (u16)TRUE;
    (*infoPtr)->circular = (u16)mode;
#ifdef OS_CALLTRACE_LEVEL_AVAILABLE
    (*infoPtr)->level = 0;
#endif

    //---- clear buffer if used for logging
    if (mode == OS_CALLTRACE_LOG)
    {
        OS_ClearCallTraceBuffer();
    }
}

#pragma profile reset

//================================================================================
//              UTILITY
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_CalcCallTraceLines

  Description:  calculate number of lines to be able to be allocated

  Arguments:    size :    CallTrace buffer size
                          same as argument of OS_InitCallTrace()'s size

  Returns:      number of lines to be able to allocate in 'size' byte
 *---------------------------------------------------------------------------*/
int OS_CalcCallTraceLines(u32 size)
{
    int     n = (int)((size - OSi_TRACEINFO_SIZEOF_HEADERPART) / sizeof(OSCallTrace));

    return (n < 0) ? 0 : n;
}

/*---------------------------------------------------------------------------*
  Name:         OS_CalcCallTraceBufferSize

  Description:  calculate buffer size to allocate specified lines

  Arguments:    lines :   lines to want to allocate

  Returns:      buffer size to need
 *---------------------------------------------------------------------------*/
u32 OS_CalcCallTraceBufferSize(int lines)
{
    SDK_ASSERT(lines >= 0);
    return OSi_TRACEINFO_SIZEOF_HEADERPART + lines * sizeof(OSCallTrace);
}

/*---------------------------------------------------------------------------*
  Name:         OS_ClearCallTraceBuffer

  Description:  clear buffer if used for logging

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_ClearCallTraceBuffer(void)
{

    OSCallTraceInfo *info = OSi_GetCallTraceInfo();

    if (info && info->circular)
    {
        OSCallTrace *p;

        info->current = info->array;
        for (p = info->current; p < info->limit; p++)
        {
            p->name = (u32)NULL;
        }
    }
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_SetCallTraceEntry

  Description:  store dummy string pointer

  Arguments:    name  : pointer to string to be stored as function name
                lr    : value to be stored as lr

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_SetCallTraceEntry(const char *name, u32 lr)
{

    OSCallTraceInfo *info = OSi_GetCallTraceInfo();

    if (info && info->circular)
    {
        OSCallTrace *p = info->current;

        p->name = (u32)name;
        p->returnAddress = lr;

#ifdef OS_CALLTRACE_RECORD_R0
        p->r0 = 0;
#endif
#ifdef OS_CALLTRACE_RECORD_R1
        p->r1 = 0;
#endif
#ifdef OS_CALLTRACE_RECORD_R2
        p->r2 = 0;
#endif
#ifdef OS_CALLTRACE_RECORD_R3
        p->r3 = 0;
#endif
        p++;

        if ((u32)p >= (u32)info->limit)
        {
            p = info->array;
        }

        info->current = p;
    }
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_GetCallTraceInfo

  Description:  get pointer to callTraceInfo structure

  Arguments:    None

  Returns:      pointer.
                NULL if not callTrace initialized
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm OSCallTraceInfo* OSi_GetCallTraceInfo( void )
{
#ifdef SDK_NO_THREAD
    ldr     r0, =OSi_DefaultCallTraceInfo
    ldr     r0, [ r0, #0 ]
    bx      lr
#else
    ldr     r0, =OSi_ThreadInfo
    ldr     r0, [ r0, #OSThreadInfo.current ]         // r0 = currentThread. 0 if no currentThread
    cmp     r0, #0
    ldrne   r0, [ r0, #OSThread.profiler ]            // r0 = callTraceInfo
    bx      lr
#endif
}
#include <nitro/codereset.h>

//================================================================================
//              PROFILE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         __PROFILE_ENTRY

  Description:  entry of function in profiler

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void __PROFILE_ENTRY( void )
{
#ifndef SDK_NO_THREAD
    stmfd  sp!, {r0}
    ldr    r0, =OSi_IsThreadInitialized
    ldr    r0, [ r0, #0 ]
    cmp    r0, #0
    ldmfd  sp!, {r0}
    bxeq   lr
#endif

    stmfd  sp!, {r1-r3}

    //---- check if callTrace not available
    stmfd  sp!, {r0, lr}
    bl     OSi_GetCallTraceInfo
    mov    r1, r0                                // r1 = callTraceInfo
    ldmfd  sp!, {r0, lr}
    cmp    r1, #0
    beq    _exit

    //---- check if callTrace master enable
    ldrh    r2, [ r1, #OSCallTraceInfo.enable ]
    cmp     r2, #0
    beq    _exit

    //---- get currentPtr of CallTrace
    ldr    r2, [ r1, #OSCallTraceInfo.current ]  // r2 = currentPtr of trace
    cmp    r2, #0
    beq    _exit

    //---- if circular
    ldrh    r3, [ r1, #OSCallTraceInfo.circular ]
    cmp     r3, #0
    bne     _circular_skip_overcheck

    //---- check if trace buffer is over limit
#ifdef OS_CALLTRACE_CHECK_OVERSTACK
    ldr    r3, [ r1, #OSCallTraceInfo.limit ]
    cmp    r2, r3
    ldrpl  r1, [ sp, #16 ] // lr
    ldrpl  r2, [ sp, #12 ] // r0
    movpl  r3, sp          // sp ( r1,r2,r3 stored )
    bpl    OSi_Abort_CallTraceFull
#endif

_circular_skip_overcheck:
    //---- store pointer to function name string
    str    r0, [ r2, #OSCallTrace.name ]

    //---- store lr register
    ldr    r0, [ sp, #16 ]
    str    r0, [ r2, #OSCallTrace.returnAddress ]

#ifdef OS_CALLTRACE_RECORD_R0
    //---- store r0 register
    ldr    r0, [ sp, #12 ]
    str    r0, [ r2, #OSCallTrace.r0 ]
#endif

#ifdef OS_CALLTRACE_RECORD_R1
    //---- store r1 register
    ldr    r0, [ sp, #0 ]
    str    r0, [ r2, #OSCallTrace.r1 ]
#endif

#ifdef OS_CALLTRACE_RECORD_R2
    //---- store r2 register
    ldr    r0, [ sp, #4 ]
    str    r0, [ r2, #OSCallTrace.r2 ]
#endif

#ifdef OS_CALLTRACE_RECORD_R3
    //---- store r3 register
    ldr    r0, [ sp, #8 ]
    str    r0, [ r2, #OSCallTrace.r3 ]
#endif

#ifdef OS_CALLTRACE_LEVEL_AVAILABLE
    //---- store call level
    ldr    r0, [ r1, #OSCallTraceInfo.level ]
    str    r0, [ r2, #OSCallTrace.level ]
    add    r0, r0, #1
    str    r0, [ r1, #OSCallTraceInfo.level ]
#endif

    //---- increment pointer
    add    r2, r2, #OSi_SIZEOF_CALLTRACE

    //---- if circular then buffer is ring-buf
    ldrh   r3, [ r1, #OSCallTraceInfo.circular ]
    cmp    r3, #0
    beq    _notcircular_skip_ring
    ldr    r3, [ r1, #OSCallTraceInfo.limit ]
    cmp    r2, r3
    bmi    _store_current
    add    r2, r1, #OSCallTraceInfo.array

_notcircular_skip_ring:
_store_current:
    str    r2, [ r1, #OSCallTraceInfo.current ]

_exit:
    ldmfd  sp!, {r1-r3}
    bx     lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         __PROFILE_EXIT

  Description:  exit of function in profiler

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void __PROFILE_EXIT( void )
{
#ifdef OS_CALLTRACE_CHECK_OVERSTACK
    stmfd  sp!, {r0-r2, lr}
#else
    stmfd  sp!, {r0-r1, lr}
#endif

#ifndef SDK_NO_THREAD
    ldr    r0, =OSi_IsThreadInitialized
    ldr    r0, [ r0, #0 ]
    cmp    r0, #0
    beq    _exit
#endif

    //---- check if callTrace not available
    bl     OSi_GetCallTraceInfo
    cmp    r0, #0
    beq    _exit
    
    //---- check if callTrace master enable
    ldrh    r1, [ r0, #OSCallTraceInfo.enable ]
    cmp     r1, #0
    beq     _exit
    
#ifdef OS_CALLTRACE_LEVEL_AVAILABLE
    //---- decrement level
    ldr     r1, [ r0, #OSCallTraceInfo.level ]
    sub     r1, r1, #1
    movmi   r1, #0
    str     r1, [ r0, #OSCallTraceInfo.level ]
#endif

    //---- if circular then do nothing
    ldrh    r1, [ r0, #OSCallTraceInfo.circular ]
    cmp     r1, #0
    bne     _exit
    
    //---- decrement current pointer
    ldr    r1, [ r0, #OSCallTraceInfo.current ]        // r1 = current
    sub    r1, r1, #OSi_SIZEOF_CALLTRACE
    
    //---- check if trace buffer is over limit
#ifdef OS_CALLTRACE_CHECK_OVERSTACK
    add    r2, r0, #OSCallTraceInfo.array
    
    cmp    r1, r2
    ldrmi  r0, [ sp, #12 ] // lr
    bmi    OSi_Abort_CallTraceLess
#endif
    
    //---- store current pointer
    str    r1, [ r0, #OSCallTraceInfo.current ]
    
_exit:
#ifdef OS_CALLTRACE_CHECK_OVERSTACK
    ldmfd  sp!, {r0-r2, lr}
#else
    ldmfd  sp!, {r0-r1, lr}
#endif
    bx     lr
}
#include <nitro/codereset.h>

//================================================================================
//              DUMP
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DumpCurrentLr

  Description:  dump current lr register

  Arguments:    lr     value of lr register

  Returns:      None
 *---------------------------------------------------------------------------*/
const char *OSi_DumpCurrentLr_str[] = {
    "OS_DumpCallTrace", "OS_DumpThreadCallTrace",
};
#pragma profile off
void OSi_DumpCurrentLr(u32 lr, int strIndex)
{
    BOOL    e = OS_DisableCallTrace();
    OS_Printf("%s: lr=%08x\n", OSi_DumpCurrentLr_str[strIndex], lr);
    (void)OS_RestoreCallTrace(e);
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_DumpOneInfo

  Description:  dump one OSCallTrace line
                normaly, only call from OSi_DumpFullInfo

  Arguments:    p    OSCallTrace pointer

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
#define OSi_CALLTRACE_MAX_INDENT 10
void OSi_DumpOneInfo(OSCallTrace *p)
{

#ifdef OS_CALLTRACE_LEVEL_AVAILABLE
    {
        int     n = (int)p->level;
        if (n > OSi_CALLTRACE_MAX_INDENT)
        {
            n = OSi_CALLTRACE_MAX_INDENT;
        }

        while (n)
        {
            int     space = n;
            if (space >= 10)
            {
                space = 10;
            }
            OS_Printf("%s", &("          \0"[10 - space]));
            n -= space;
        }
    }
#endif

    OS_Printf("%s: lr=%08x"
#ifdef OS_CALLTRACE_RECORD_R0
              ", r0=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R1
              ", r1=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R2
              ", r2=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R3
              ", r3=%08x"
#endif
              "\n", (char *)(p->name), p->returnAddress
#ifdef OS_CALLTRACE_RECORD_R0
              , p->r0
#endif
#ifdef OS_CALLTRACE_RECORD_R1
              , p->r1
#endif
#ifdef OS_CALLTRACE_RECORD_R2
              , p->r2
#endif
#ifdef OS_CALLTRACE_RECORD_R3
              , p->r3
#endif
        );
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_DumpFullInfo

  Description:  dump one OSCallTrace line
                normaly, only call from OSi_DumpCallTrace_core and OSi_DumpThreadCallTrace_core

  Arguments:    info    OSCallTraceInfo pointer

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_DumpFullInfo(OSCallTraceInfo *info)
{
    if (info && info->current)
    {
        OSCallTrace *p = info->current - 1;

        while (1)
        {
            if (info->circular && (u32)p < (u32)info->array)
            {
                p = info->limit - 1;
            }

            if ((u32)p < (u32)info->array
                || 0x2000000 > (u32)(p->name) || 0x2400000 < (u32)(p->name))
            {
                break;
            }

            OSi_DumpOneInfo(p);

            if ((u32)p == (u32)(info->current))
            {
                break;
            }

            p--;
        }
    }
}

#pragma profile reset


/*---------------------------------------------------------------------------*
  Name:         OS_DumpCallTrace

  Description:  dump callStack

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void OS_DumpCallTrace( void )
{
    mov    r0, lr
    b      OSi_DumpCallTrace_core
    
    // no 'bx lr'
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OSi_DumpCallTrace_core

  Description:  dump callTrace (core function)

  Arguments:    returnAddress :  lr (for display)

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_DumpCallTrace_core(u32 returnAddress)
{
    BOOL    e = OS_DisableCallTrace();

    OSCallTraceInfo *info = OSi_GetCallTraceInfo();
    SDK_ASSERTMSG(info && info->current, "Not Initialize functionTrace");

    if (returnAddress)
    {
        OSi_DumpCurrentLr(returnAddress, OSi_STR_DUMPCALLTRACE);
    }
    OSi_DumpFullInfo(info);

    (void)OS_RestoreCallTrace(e);
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OS_DumpThreadCallTrace

  Description:  dump callTrace of thread

  Arguments:    thread :    thread

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void OS_DumpThreadCallTrace( const OSThread* thread )
{
    mov   r1, lr
    b     OSi_DumpThreadCallTrace_core
    
    // no 'bx lr'
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OSi_DumpThreadCallTrace_core

  Description:  dump callTrace of thread

  Arguments:    thread :         thread
                returnAddress :  lr (for display)

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_DumpThreadCallTrace_core(const OSThread *thread, u32 returnAddress)
{
    OSCallTraceInfo *info;
    BOOL    e;

#ifdef SDK_NO_THREAD
    return;
#endif

    //---- null means current thread
    if (!thread)
    {
        thread = OS_GetCurrentThread();
    }

    //---- get callTraceInfo
    if (thread)
    {
        info = (OSCallTraceInfo *)thread->profiler;
    }

    //---- check callTraceInfo available
    if (!thread || !info || !info->current)
    {
        return;
    }

    //---- arrange returnAddress
    if (thread != OS_GetCurrentThread())
    {
        returnAddress = thread->context.r[14];
    }

    e = OS_DisableCallTrace();

    if (returnAddress)
    {
        OSi_DumpCurrentLr(returnAddress, OSi_STR_DUMPTHREADCALLTRACE);
    }
    OSi_DumpFullInfo(info);

    (void)OS_RestoreCallTrace(e);
}

//================================================================================
//              ERROR ABORT
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_Abort_CallTraceFull

  Description:  display error in stack overflow

  Arguments:    name            function name pointer
                returnAddress   lr
                r0              r0 (if use)

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_Abort_CallTraceFull(u32 name, u32 returnAddress, u32 r0, u32 sp)
{
#pragma unused( r0 )

    BOOL    e = OS_DisableCallTrace(); // not restore status in this function.

    OS_Printf("***Error: CallTrace stack overflow");
    OS_Printf(" in %s: lr=%08x"
#ifdef OS_CALLTRACE_RECORD_R0
              ", r0=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R1
              ", r1=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R2
              ", r2=%08x"
#endif
#ifdef OS_CALLTRACE_RECORD_R3
              ", r3=%08x"
#endif
              "\n", (char *)(name), returnAddress
#ifdef OS_CALLTRACE_RECORD_R0
              , r0
#endif
#ifdef OS_CALLTRACE_RECORD_R1
              , *((u32 *)sp)
#endif
#ifdef OS_CALLTRACE_RECORD_R2
              , *((u32 *)sp + 1)
#endif
#ifdef OS_CALLTRACE_RECORD_R3
              , *((u32 *)sp + 2)
#endif
        );

#ifdef SDK_NO_THREAD
    OSi_DumpCallTrace_core(0);
#else
    OSi_DumpThreadCallTrace_core(NULL, 0);
#endif

    OS_Terminate();
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_Abort_CallTraceLess

  Description:  display error in stack underflow

  Arguments:    returnAddress   lr

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_Abort_CallTraceLess(u32 returnAddress)
{
    BOOL    e = OS_DisableCallTrace(); // not restore status in this function.

    OS_Printf("***Error: CallTrace stack underflow.");
    OS_Printf(" (lr=%08x)\n", returnAddress);

    OS_Terminate();
}

#pragma profile reset

#endif // ifdef OS_NO_CALLTRACE
#endif // defined(OS_PROFILE_AVAILABLE) && defined(OS_PROFILE_CALL_TRACE)
