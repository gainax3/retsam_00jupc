/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_functionCost.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_functionCost.c,v $
  Revision 1.23  2007/02/20 00:28:10  kitase_hirotake
  indent source

  Revision 1.22  2006/08/31 01:22:42  okubata_ryoma
  not output check number

  Revision 1.21  2006/08/23 09:49:47  yada
  write the checkcode in clearing the function cost buffer

  Revision 1.20  2006/08/22 07:19:54  yada
  add the feature which is the check if the buffer overflowed the limit

  Revision 1.19  2006/06/06 02:26:15  yada
  In __PROFILE_ENTRY and __PROFILE_EXIT, let to be disable interrupt
  for more accurate measurement of statistics

  Revision 1.18  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.17  2005/03/04 11:09:27  yasu
  __declspec(weak) ‚ð SDK_WEAK_SYMBOL ‚É’u‚«Š·‚¦

  Revision 1.16  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.15  2004/11/04 04:02:27  yada
  fix a little

  Revision 1.14  2004/11/02 09:52:05  yada
  just fix comment

  Revision 1.13  2004/10/07 12:15:19  yada
  fix around OS_CalcThreadStatistics()

  Revision 1.12  2004/07/12 00:57:52  yada
  fix OSi_SetStatistics

  Revision 1.11  2004/07/10 07:28:34  yada
  treat tick value as u16->u32.
  fix bug about statistics

  Revision 1.10  2004/06/03 07:35:02  yada
  enclose OSi_CalcThreadInterval() by code32.h and codereset.h

  Revision 1.9  2004/06/03 04:22:36  yada
  move functionCost routine into

  Revision 1.8  2004/05/24 04:49:46  yada
  fix profile pragma

  Revision 1.7  2004/05/20 04:29:56  yada
  consider for SDK_NO_THREAD

  Revision 1.6  2004/05/18 06:35:54  yada
  consider for thread that is not current

  Revision 1.5  2004/05/18 06:11:12  yada
  consider for thread interval

  Revision 1.4  2004/05/12 10:59:42  yada
  fix structure offset description

  Revision 1.3  2004/05/12 05:08:33  yada
  modify SDK_XXX / OS_XXX option
  make __PROFILE_ENTRY/EXIT be weak

  Revision 1.2  2004/05/10 12:36:19  yada
  fix code32 mistake

  Revision 1.1  2004/05/10 11:38:08  yada
  first release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

#if defined(OS_PROFILE_AVAILABLE) && defined(OS_PROFILE_FUNCTION_COST)

#ifdef OS_NO_FUNCTIONCOST
//---- dummy function
SDK_WEAK_SYMBOL asm void __PROFILE_ENTRY( void ){  bx   lr }
SDK_WEAK_SYMBOL asm void __PROFILE_EXIT( void ){   bx   lr }
#else  // ifdef OS_NO_FUNCTIONCOST

//---- function prototype declaration
extern u32 OSi_GetTick_noProfile(void);
extern void OSi_SetSystemCallbackInSwitchThread(void *callback);

//---- flag for thread initializing
extern BOOL OSi_IsThreadInitialized;

static OSFunctionCostInfo *OSi_GetFunctionCostInfo(void);
static void OSi_SetStatistics(void *statBuf, OSFunctionCost *entry, OSFunctionCost *exit);
static void OSi_CalcThreadInterval(register OSThread *saveThread, register OSThread *loadThread);

//---- pointer to functionCost at not using threadSystem
static OSFunctionCostInfo *OSi_DefaultFunctionCostInfo;

//---- buffer check number
#define OSi_FUNCTIONCOST_CHECKNUM_BUFFER	0xfddb597d

//================================================================================
//              ENABLE/DISABLE
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableFunctionCost

  Description:  enable functionCost

  Arguments:    None

  Returns:      previous status. TRUE if enable 
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_EnableFunctionCost( void )
{
  //---- check if functionCost not available
  stmfd   sp!, {lr}
  bl      OSi_GetFunctionCostInfo
  ldmfd   sp!, {lr}
  cmp     r0, #0
  bxeq    lr

  mov     r2, r0
  ldrh    r0, [ r2, #OSFunctionCostInfo.enable ]
  mov     r1, #1
  strh    r1, [ r2, #OSFunctionCostInfo.enable ]

  bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_DisableFunctionCost

  Description:  disble functionCost

  Arguments:    None

  Returns:      previous status. TRUE if enable 
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_DisableFunctionCost( void )
{
  //---- check if functionCost not available
  stmfd   sp!, {lr}
  bl      OSi_GetFunctionCostInfo
  ldmfd   sp!, {lr}
  cmp     r0, #0
  bxeq    lr

  mov     r2, r0
  ldrh    r0, [ r2, #OSFunctionCostInfo.enable ]
  mov     r1, #0
  strh    r1, [ r2, #OSFunctionCostInfo.enable ]

  bx      lr
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OS_RestoreFunctionCost

  Description:  set status of functionCost

  Arguments:    enable :  TRUE if set to be enable, FALSE if not

  Returns:      previous status. TRUE if enable 
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm BOOL OS_RestoreFunctionCost( BOOL )
{
  //---- check if functionCost not available
  stmfd   sp!, {r0, lr}
  bl      OSi_GetFunctionCostInfo
  mov     r1, r0
  ldmfd   sp!, {r0, lr}
  cmp     r1, #0
  bxeq    lr

  ldrh    r2, [ r1, #OSFunctionCostInfo.enable ]
  strh    r0, [ r1, #OSFunctionCostInfo.enable ]
  mov     r0, r2

  bx      lr
}
#include <nitro/codereset.h>

//================================================================================
//             SETTING
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitFunctionCost

  Description:  Initialize functionCost buffer,
                and start recording.

  Arguments:    buf    address to buffer for recording entering and exitting function.
                size   size of buffer (by byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_InitFunctionCost(void *buf, u32 size)
{
    OSFunctionCostInfo **infoPtr;

    SDK_ASSERTMSG(((u32)buf & 3) == 0, "FunctionCost buffer must be aligned by 4");
    SDK_ASSERTMSG((size >= OSi_COSTINFO_SIZEOF_HEADERPART + sizeof(OSFunctionCost)),
                  "to small FunctionCost buffer");

    //---- need Tick
    if (!OS_IsTickAvailable())
    {
        OS_Panic("OS_InitFunctionCost: need tick.\n");
    }

#ifdef SDK_NO_THREAD
    //---- pointer to functionCostInfo is in static valiable
    infoPtr = &OSi_DefaultFunctionCostInfo;
#else
    //---- pointer to requiredCostInfo is in OSThread structure
    SDK_ASSERTMSG(OS_IsThreadAvailable(), "OS_InitFunctionCost: thread system not initialized.");
    infoPtr = (OSFunctionCostInfo **)&(OS_GetCurrentThread()->profiler);

    OSi_SetSystemCallbackInSwitchThread(OSi_CalcThreadInterval);
#endif

    *infoPtr = (OSFunctionCostInfo *)buf;
    //(*infoPtr)->current = (OSFunctionCost *)(*infoPtr)->array; // not need because clear function includes this.
    (*infoPtr)->limit = (OSFunctionCost *)((u32)buf
                                           + OSi_COSTINFO_SIZEOF_HEADERPART
                                           + OS_CalcFunctionCostLines(size) * sizeof(OSFunctionCost));
    (*infoPtr)->enable = (u16)TRUE;

#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL
    (*infoPtr)->breakTime = 0;
#endif

    //---- clear function cost buffer
    OSi_ClearThreadFunctionCostBuffer(NULL);
}

#pragma profile reset

//================================================================================
//              UTILITY
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_CalcFunctionCostLines

  Description:  calculate number of lines to be able to be allocated

  Arguments:    size :    FunctionCost buffer size
                          same as argument of OS_InitFunctionCost()'s size

  Returns:      number of lines to be able to allocate in 'size' byte
 *---------------------------------------------------------------------------*/
int OS_CalcFunctionCostLines(u32 size)
{
    int     n = (int)((size - OSi_COSTINFO_SIZEOF_HEADERPART) / sizeof(OSFunctionCost));
    return (n < 0) ? 0 : n;
}

/*---------------------------------------------------------------------------*
  Name:         OS_CalcFunctionCostBufferSize

  Description:  calculate buffer size to allocate specified lines

  Arguments:    lines :   lines to want to allocate

  Returns:      buffer size to need
 *---------------------------------------------------------------------------*/
u32 OS_CalcFunctionCostBufferSize(int lines)
{
    SDK_ASSERT(lines >= 0);

    return OSi_COSTINFO_SIZEOF_HEADERPART + lines * sizeof(OSFunctionCost);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_ClearThreadFunctionCostBuffer

  Description:  clear function cost buffer of specified thread

  Arguments:    thread : NULL means current thread.
                         Other is thread pointer.
  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OSi_ClearThreadFunctionCostBuffer(OSThread *thread)
{
    OSFunctionCostInfo *infoPtr;

    if (!thread)
    {
        infoPtr = OSi_GetFunctionCostInfo();
    }
    else
    {
        infoPtr = (OSFunctionCostInfo *)(thread->profiler);
    }

    if (infoPtr)
    {
        infoPtr->current = infoPtr->array;

        //---- for check overflowing buffer limit
        *(u32 *)(infoPtr->limit - 1) = OSi_FUNCTIONCOST_CHECKNUM_BUFFER;
    }
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OS_GetFunctionCostInfo

  Description:  get pointer to functionCostInfo structure

  Arguments:    None

  Returns:      pointer.
                NULL if not functionCost initialized
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
static asm OSFunctionCostInfo* OSi_GetFunctionCostInfo( void )
{
#ifdef SDK_NO_THREAD
  ldr     r0, =OSi_DefaultFunctionCostInfo
  ldr     r0, [ r0, #0 ]
  bx      lr
#else
  ldr    r0, =OSi_ThreadInfo
  ldr    r0, [ r0, #OSThreadInfo.current ]           // r0 = currentThread. 0 if no currentThread
  cmp    r0, #0
  ldrne  r0, [ r0, #OSThread.profiler ]              // r0 = functionCostInfo
  bx     lr
#endif
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OS_CheckFunctionCostBuffer

  Description:  check if function cost buffer overflows the limit.

  Arguments:    None

  Returns:      FALSE if overflowed. TRUE if not
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_FUNCTIONCOST
#pragma profile off
BOOL OS_CheckFunctionCostBuffer(void *buf)
{
    OSFunctionCostInfo *infoPtr = buf;

    if (*(u32 *)(infoPtr->limit - 1) != OSi_FUNCTIONCOST_CHECKNUM_BUFFER)
    {
        //OS_Printf("OVER!\n");
        return FALSE;
    }

    return TRUE;
}

#pragma profile reset
#endif

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
  //---- check if thread system not initialized
  stmfd  sp!, {r0}
  ldr    r0, =OSi_IsThreadInitialized
  ldr    r0, [ r0, #0 ]
  cmp    r0, #0
  ldmfd  sp!, {r0}
  bxeq   lr
#endif

  stmfd  sp!, {r1-r4, lr}

  //---- disable interrupt
  mrs     r4, cpsr
  orr     r1, r4, #HW_PSR_IRQ_DISABLE
  msr     cpsr_c, r1

  //---- check if functionCost not available
  stmfd  sp!, {r0}
  bl     OSi_GetFunctionCostInfo
  mov    r1, r0
  ldmfd  sp!, {r0}
  cmp    r1, #0
  beq    _exit

  //---- check if functionCost master enable
  ldrh   r2, [ r1, #OSFunctionCostInfo.enable ]
  cmp    r2, #0
  beq    _exit

  //---- get currentPtr of FunctionCost
  ldr    r2, [ r1, #OSFunctionCostInfo.current ]
  cmp    r2, #0
  beq    _exit

  //---- check if buffer is over limit
  ldr    r3, [ r1, #OSFunctionCostInfo.limit ]
  cmp    r2, r3
  bpl    _exit

  //---- store pointer to function name string
  str    r0, [ r2, #OSFunctionCost.entry.name ]

  //---- store time
  stmfd  sp!, {r1-r2}
  bl     OSi_GetTick_noProfile
  ldmfd  sp!, {r1-r2}
  str    r0, [ r2, #OSFunctionCost.entry.time ]

#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL
  //---- clear interval time
  mov    r3, #0
  str    r3, [ r2, #OSFunctionCost.exit.interval ]
#endif

  //---- increment pointer
  add    r2, r2, #OSi_SIZEOF_FUNCTIONCOST
  str    r2, [ r1, #OSFunctionCostInfo.current ]

_exit:
  //---- restore interrupt
  msr     cpsr_c, r4

  ldmfd  sp!, {r1-r4, lr}
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
  stmfd  sp!, {r0-r3, lr}

#ifndef SDK_NO_THREAD
  //---- check if thread system not initialized
  ldr    r0, =OSi_IsThreadInitialized
  ldr    r0, [ r0, #0 ]
  cmp    r0, #0
  beq    _exit
#endif

  //---- disable interrupt
  mrs    r3, cpsr
  orr    r1, r3, #HW_PSR_IRQ_DISABLE
  msr    cpsr_c, r1

  //---- check if functionCost not available
  bl     OSi_GetFunctionCostInfo
  mov    r1, r0
  cmp    r1, #0
  beq    _exit_ri

  //---- check if functionCost master enable
  ldrh    r2, [ r1, #OSFunctionCostInfo.enable ]
  cmp     r2, #0
  beq     _exit_ri

  //---- get currentPtr of functionCost
  ldr    r2, [ r1, #OSFunctionCostInfo.current ]  // r2 = currentPtr of trace
  cmp    r2, #0
  beq    _exit_ri

  //---- check if buffer is over limit
  ldr    r0, [ r1, #OSFunctionCostInfo.limit ]
  cmp    r2, r0
  bpl    _exit_ri

  //---- store endmark
  ldr    r0, =OSi_FUNCTIONCOST_EXIT_TAG
  str    r0, [ r2, #OSFunctionCost.exit.tag ]

  //---- store time
  stmfd  sp!, {r1-r3}  
  bl     OSi_GetTick_noProfile
  ldmfd  sp!, {r1-r3}
  str    r0, [ r2, #OSFunctionCost.exit.time ]

  //---- increment pointer
  add    r2, r2, #OSi_SIZEOF_FUNCTIONCOST
  str    r2, [ r1, #OSFunctionCostInfo.current ]

_exit_ri:
  //---- restore interrupt
  msr    cpsr_c, r3

_exit:
  ldmfd  sp!, {r0-r3, lr}
  bx     lr
}
#include <nitro/codereset.h>

//================================================================================
//              STATISTICS
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitStatistics

  Description:  initialize statistics buffer

  Arguments:    statBuf  pointer to statistics buffer
                size     size of statBuf

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_InitStatistics(void *statBuf, u32 size)
{
    OSFunctionCostStatisticsInfo *infoPtr = statBuf;
    u32    *p;

    if (!infoPtr || size <= OSi_STATISTICS_LEAST_SIZE)
    {
        return;
    }

    //---- buffersize
    infoPtr->size = ((size - OSi_STATISTICS_SIZEOF_HEADERPART) / sizeof(OSFunctionCostStatistics))
        * sizeof(OSFunctionCostStatistics) + OSi_STATISTICS_SIZEOF_HEADERPART;
    infoPtr->limit = (OSFunctionCostStatistics *)((u32)infoPtr + infoPtr->size);

    p = (u32 *)infoPtr->array;
    while ((u32)p < (u32)infoPtr->limit)
    {
        *p++ = 0;
    }

    //---- for check overflowing buffer limit
    *(u32 *)((OSFunctionCostStatistics *)p - 1) = OSi_FUNCTIONCOST_CHECKNUM_BUFFER;
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OSi_SetStatistics

  Description:  modify result to statBuf

  Arguments:    statBuf    pointer to statistics buffer
                entry      OSFunctionCost pointer of entering function
                exit       OSFunctionCost pointer of exiting function

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
static void OSi_SetStatistics(void *statBuf, OSFunctionCost *entry, OSFunctionCost *exit)
{
    OSFunctionCostStatisticsInfo *infoPtr = statBuf;
    OSFunctionCostStatistics *p = infoPtr->array;
    u32     time;

    //---- calc cost
    //    (variables are u32, so even if entry>exit, it's ok)
    time = exit->exit.time - entry->entry.time;

#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL
    //---- consider for interval by switching thread
    //     (maybe always time>interval)
    time = (time > entry->entry.interval) ? (u32)(time - entry->entry.interval) : 0;
#endif

    while ((u32)p < (u32)infoPtr->limit)
    {
        if (!p->name)
        {
            p->name = entry->entry.name;
            p->count = 1;
            p->time = time;
            break;
        }

        if (p->name == entry->entry.name)
        {
            p->count++;
            p->time += time;
            break;
        }
        p++;
    }
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OS_CalcThreadStatistics

  Description:  calculate statistics to statBuf

  Arguments:    statBuf :  pointer to statistics buffer
                thread     thread to calculate.
                           if current or no thread then specify NULL

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_CalcThreadStatistics(void *statBuf, OSThread *thread)
{
    OSFunctionCostInfo *infoPtr;
    OSFunctionCost *p, *array, *limit;

    if (!thread)
    {
        infoPtr = OSi_GetFunctionCostInfo();
    }
    else
    {
        infoPtr = (OSFunctionCostInfo *)(thread->profiler);
    }
    if (!infoPtr || !(limit = infoPtr->current))
    {
        return;
    }

    p = array = (OSFunctionCost *)&(infoPtr->array);
    while (p < limit)
    {
        u32     name = p->entry.name;

        if (name == OSi_FUNCTIONCOST_EXIT_TAG)
        {
            int     cnt = 0;
            OSFunctionCost *bp = p - 1;


#if 0
            //---- for debug
            if (bp->entry.name == OSi_FUNCTIONCOST_EXIT_TAG)
            {
#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL
                OS_Printf("----: %x %x\n", bp->entry.time, bp->entry.interval);
#else
                OS_Printf("----: %x\n", bp->entry.time);
#endif
            }
            else
            {
#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL
                OS_Printf("%s: %x %x\n", bp->entry.name, bp->entry.time, bp->entry.interval);
#else
                OS_Printf("%s: %x\n", bp->entry.name, bp->entry.time);
#endif
            }
#endif


            while (bp >= array)
            {
                if (bp->entry.name == OSi_FUNCTIONCOST_EXIT_TAG)
                {
                    cnt++;
                }
                else
                {
                    if (--cnt < 0)
                    {
                        OSi_SetStatistics(statBuf, bp, p);
                        break;
                    }
                }
                bp--;
            }
        }
        else
        {
            if (!name)
                continue;
        }
        p++;
    }

    //---- clear buffer
    OSi_ClearThreadFunctionCostBuffer(thread);
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OS_CheckStatisticsBuffer

  Description:  check if statistics buffer overflows the limit.

  Arguments:    None

  Returns:      FALSE if overflowed. TRUE if not
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_FUNCTIONCOST
#pragma profile off
BOOL OS_CheckStatisticsBuffer(void *statBuf)
{
    OSFunctionCostStatisticsInfo *infoPtr = statBuf;

    if (*(u32 *)(infoPtr->limit - 1) != OSi_FUNCTIONCOST_CHECKNUM_BUFFER)
    {
        //OS_Printf("OVER!\n");
        return FALSE;
    }

    return TRUE;
}

#pragma profile reset
#endif

//================================================================================
//              DUMP
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_DumpStatistics

  Description:  dump statistics of functionCost

  Arguments:    statBuf  buffer for statistics

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_DumpStatistics(void *statBuf)
{
    OSFunctionCostStatisticsInfo *infoPtr = statBuf;
    OSFunctionCostStatistics *p = infoPtr->array;
    BOOL    displayed = FALSE;

#ifdef SDK_NO_THREAD
    return;
#endif

    OS_Printf("---- functionCost statistics\n");

    while ((u32)p < (u32)infoPtr->limit)
    {
        // not output buffer check number
        if (p->name && (p->name != OSi_FUNCTIONCOST_CHECKNUM_BUFFER))
        {
            displayed = TRUE;
            OS_Printf("%s: count %d, cost %lld\n", p->name, p->count, p->time);
        }

        p++;
    }

    if (!displayed)
    {
        OS_Printf("no data\n");
    }
}

#pragma profile reset


/*---------------------------------------------------------------------------*
  Name:         OSi_CalcThreadInterval

  Description:  calculate interval from save context to load context
                for functionCost profiler.
                destroy r0-r3 registers

  Arguments:    saveThread     pointer to saving thread
                loadThread     pointer to loading thread

  Returns:      None
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
#pragma profile off
static asm void OSi_CalcThreadInterval( register OSThread* saveThread, register OSThread* loadThread )
{
#pragma unused( saveThread, loadThread )
#ifndef SDK_NO_THREAD
#ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL

    stmfd     sp!, {lr}

    //==== about saving thread
    //---- check if functionCost not available
    cmp     r0, #0                                    // r0 = saving thread
    beq     _skip_saveThread
    ldr     r2, [ r0, #OSThread.profiler ]            // r2 = functionCostInfo
    cmp     r2, #0
    beq     _skip_saveThread

    //---- check if functionCost master enable
    ldrh    r3, [ r2, #OSFunctionCostInfo.enable ]    // r3 = master enable
    cmp     r3, #0
    beq     _skip_saveThread

    //---- store current time as break time
    stmfd  sp!, {r1-r3}  
    bl     OSi_GetTick_noProfile                      // r0 = currentTime
    ldmfd  sp!, {r1-r3}
    str     r0, [ r2, #OSFunctionCostInfo.breakTime ]

_skip_saveThread:

    //==== about loading thread
    //---- check if functionCost not available
    cmp     r1, #0                                    // r1 = loadingThread
    beq     _skip_loadThread
    ldr     r2, [ r1, #OSThread.profiler ]            // r2 = functionCostInfo
    cmp     r2, #0
    beq     _skip_loadThread

    //---- check if functionCost master enable
    ldrh    r3, [ r2, #OSFunctionCostInfo.enable ]    // r3 = master enable
    cmp     r3, #0
    beq     _skip_loadThread

    //---- get currentPtr of functionCost
    ldr     r3, [ r2, #OSFunctionCostInfo.current ]   // r3 = currentPtr of trace
    cmp     r3, #0
    beq     _skip_loadThread

    sub     r1, r3, #OSi_SIZEOF_FUNCTIONCOST          // r1 = previous Ptr

    //---- check if buffer is below array
    add     r0, r2, #OSFunctionCostInfo.array[0]      // r0 = array Ptr
    cmp     r1, r0
    bmi     _skip_loadThread

    //---- store current time as break time
    stmfd  sp!, {r1-r3}  
    bl     OSi_GetTick_noProfile                      // r0 = currentTime
    ldmfd  sp!, {r1-r3}
    ldr     r3, [ r2, #OSFunctionCostInfo.breakTime ] // r3 = breakTime
    sub     r3, r0, r3                                // r3 = currentTime - breakTime = interval

    ldr     r0, [ r1, #OSFunctionCost.entry.interval ]      // add interval
    add     r0, r0, r3
    str     r0, [ r1, #OSFunctionCost.entry.interval ]

_skip_loadThread:

    ldmfd   sp!, {lr}
#endif // ifdef OSi_FUNCTIONCOST_THREAD_INTERVAL

    bx      lr

#endif // ifndef SDK_NO_THREAD
}
#pragma profile reset
#include <nitro/codereset.h>


#endif // ifdef OS_NO_FUNCTIONCOST
#endif // defined(OS_PROFILE_AVAILABLE) && defined(OS_PROFILE_FUNCTION_COST)
