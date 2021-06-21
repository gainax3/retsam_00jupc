/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_emulator.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_emulator.c,v $
  Revision 1.22  2006/04/25 23:58:19  okubata_ryoma
  返り値がリファレンスの記述と異なる値を返すという不具合修正

  Revision 1.21  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.20  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.19  2005/02/18 08:51:35  yasu
  著作年度変更

  Revision 1.18  2005/02/18 06:20:18  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.17  2004/09/22 00:25:38  yada
  OS_GetConsole() Type returns constant value
  if  SDK_SMALL_BUILD and SDK_FINALROM are defined

  Revision 1.16  2004/08/17 04:10:32  yada
  detect debugger correctly

  Revision 1.15  2004/08/16 08:15:54  yada
  HW_CHECK_DEBUGGER_BUF definition is moved to mmap_shared.h header

  Revision 1.14  2004/08/11 13:18:58  yada
  change method to detect platform

  Revision 1.13  2004/07/23 01:03:03  yada
  OS_UnLockCartridge -> OS_UnlockCartridge

  Revision 1.12  2004/07/22 08:22:41  yada
  fix for changing some lockID's type u32->u16

  Revision 1.11  2004/06/08 10:43:26  yada
  add information about main memory size

  Revision 1.10  2004/06/08 04:34:45  yada
  only fix comment

  Revision 1.9  2004/06/07 04:29:39  yada
  add OS_GetConsoleType()

  Revision 1.8  2004/02/13 08:39:30  yasu
  OS_IsRunOnEmulator returns by FALSE if ARM7

  Revision 1.7  2004/02/13 02:26:56  yada
  ARM9とAMR7 でインクルードファイル場合わけ

  Revision 1.6  2004/02/12 11:08:30  yada
  ARM9/ARM7 振り分け作業

  Revision 1.5  2004/02/06 09:29:43  yasu
  OS_IsRunEmulator の認識処理を一度だけにした．

  Revision 1.4  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  2003/12/25 07:31:26  yada
  型ルール統一による変更

  Revision 1.2  2003/11/30 04:20:26  yasu
  OS_EnableInterrupt/DisableInterrupt の戻り値の型の変更

  Revision 1.1  2003/11/29 01:25:46  yada
  ファイル名称変更

  Revision 1.1  2003/11/25 11:17:37  yasu
  エミュレータの自動判別追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

//---- current processor lock flag
#ifdef SDK_ARM9
#define OSi_CURPROC_LOCKED_FLAG    OS_MAINP_LOCKED_FLAG
#else
#define OSi_CURPROC_LOCKED_FLAG    OS_SUBP_LOCKED_FLAG
#endif

static u32 OSi_ConsoleTypeCache = OSi_CONSOLE_NOT_DETECT;

u32     OSi_GetDeviceType(void);
BOOL    OSi_IsRunOnDebugger(void);

//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_IsRunOnEmulator

  Description:  Detect emulator

  Arguments:    None

  Returns:      non 0 if emulator
 *---------------------------------------------------------------------------*/
BOOL OS_IsRunOnEmulator(void)
{
#ifdef  SDK_ARM9
#ifndef SDK_FINALROM
    static int onEmu = -1;             // Not so good coding style...
    u32     val;
    OSIntrMode intr;

    if (onEmu == -1)
    {
        intr = OS_DisableInterrupts();
        {
            // PING magic number to ensata
            (*(REGType32v *)REG_CLIPMTX_RESULT_0_ADDR) = 0x2468ace0;

            // PONG magic number from ensata
            val = *(vu16 *)REG_VCOUNT_ADDR & 0x1ffU;

            if (val == 270)
            {
                // Recognized as ensata, send back ACK signal to ensata
                *(vu32 *)0x4fff010 = 0x13579bdf;        // ACK Signal 1
                *(vu32 *)0x4fff010 = 0xfdb97531;        // ACK Signal 2
                onEmu = TRUE;
            }
            else
            {
                // Unidentified ...
                onEmu = FALSE;
            }
        }
        (void)OS_RestoreInterrupts(intr);
    }
    return (BOOL)onEmu;
#else
    return FALSE;
#endif

#else  // SDK_ARM7
    return FALSE;                      // always FALSE if ARM7
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetConsoleType

  Description:  Detect console type

  Arguments:    None

  Returns:
 *---------------------------------------------------------------------------*/
u32 OS_GetConsoleType(void)
{
#if defined( SDK_FINALROM ) || defined( SDK_SMALL_BUILD )
    OSi_ConsoleTypeCache = OS_CONSOLE_NITRO | OS_CONSOLE_DEV_CARD | OS_CONSOLE_SIZE_4MB;

#else  // if defined( SDK_FINALROM ) && defined( SDK_SMALL_BUILD )

    //---- skip if detect already
    if (OSi_ConsoleTypeCache == OSi_CONSOLE_NOT_DETECT)
    {
        //---- whether device is cartridge?
        u32     type = OSi_GetDeviceType();

        //---- running on software emulator Ensata?
        if (OS_IsRunOnEmulator())
        {
            type |= OS_CONSOLE_ENSATA;
        }
        //---- running on debugger?
        else if (OSi_IsRunOnDebugger())
        {
            type |= OS_CONSOLE_ISDEBUGGER;
        }
        //---- is card device cartridge? so that running on IS-Emulator
        else if (type & OS_CONSOLE_DEV_CARTRIDGE)
        {
            type |= OS_CONSOLE_ISEMULATOR;
        }
        //---- else, maybe retail NITRO.
        else
        {
            type |= OS_CONSOLE_NITRO;
        }

        //---- main memory size (maybe ARM7 has already detected)
        type |= *(u16 *)HW_MMEMCHECKER_SUB;

        OSi_ConsoleTypeCache = type;
    }
#endif // if defined( SDK_FINALROM ) && defined( SDK_SMALL_BUILD )

    return OSi_ConsoleTypeCache;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetDeviceType

  Description:  get card device type
                (subroutine of OS_GetConsoleType)

  Arguments:    None

  Returns:      decided by bus card data loaded
                OS_CONSOLE_DEV_CARTRIDGE : via cartridge bus
                OS_CONSOLE_DEV_CARD      : via card bus
 *---------------------------------------------------------------------------*/
u32 OSi_GetDeviceType(void)
{
    BOOL    checked = FALSE;
    u16     lockId = (u16)OS_GetLockID();       // treat as success absolutely
    u32     result = 0;

    do
    {
        s32     ret = OS_LOCK_ERROR;
        OSIntrMode enabled = OS_DisableInterrupts();

        //---- cartridge already locked  or  lock now success
        if ((OS_ReadOwnerOfLockCartridge() & OSi_CURPROC_LOCKED_FLAG)
            || ((ret = (s32)OS_TryLockCartridge(lockId)) == OS_LOCK_SUCCESS))
        {
            //---- check cartridge header
            result =
                (u32)(((((vu32 *)HW_CTRDG_ROM)[0] == (u32)'TNIN') &&
                       (((vu32 *)HW_CTRDG_ROM)[1] == (u32)'ODNE')) ?
                      OS_CONSOLE_DEV_CARTRIDGE : OS_CONSOLE_DEV_CARD);

            //---- unlock cartridge
            if (ret == OS_LOCK_SUCCESS)
            {
                (void)OS_UnlockCartridge(lockId);
                checked = TRUE;
            }
        }

        (void)OS_RestoreInterrupts(enabled);
    }
    while (!checked);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IsRunOnDebugger

  Description:  Detect Debugger
                (subroutine of OS_GetConsoleType)

  Arguments:    None

  Returns:      TRUE  : debugger
                FALSE : not debugger
 *---------------------------------------------------------------------------*/
BOOL OSi_IsRunOnDebugger(void)
{
    u16    *checkAddress = (u16 *)((*(u16 *)HW_CHECK_DEBUGGER_SW ==
                                    0) ? HW_CHECK_DEBUGGER_BUF1 : HW_CHECK_DEBUGGER_BUF2);

    return (*checkAddress == 1) ? TRUE : FALSE;
}
