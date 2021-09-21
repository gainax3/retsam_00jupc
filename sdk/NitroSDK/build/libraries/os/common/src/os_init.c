/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_init.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_init.c,v $
  Revision 1.52  2006/11/14 04:33:19  okubata_ryoma
  small fix

  Revision 1.51  2006/11/14 01:36:26  okubata_ryoma
  ARM9のOS_Init終了時にvcountを0に揃える処理を追加

  Revision 1.50  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.49  2005/11/22 09:11:28  yosizaki
  support SDK_ENABLE_ARM7_PRINT.

  Revision 1.48  2005/07/19 06:06:11  yada
  add irq stack check

  Revision 1.47  2005/04/26 06:54:22  terui
  Fix comment

  Revision 1.46  2005/04/18 12:26:59  terui
  OSi_CheckOwnerInfo関数を削除

  Revision 1.45  2005/04/12 10:49:15  terui
  中国語対応の為に改造

  Revision 1.44  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.43  2005/02/10 07:46:57  yada
  let call MI_Init()

  Revision 1.42  2005/02/09 09:17:48  terui
  ARM9における初期化時にVRAMの排他制御を初期化する機能を追加。

  Revision 1.41  2004/10/12 08:01:04  yada
  delete OS_InitPrintServer()

  Revision 1.40  2004/10/08 07:49:13  yada
  consider for the situation of not calling OS_InitPrintServer()

  Revision 1.39  2004/09/29 06:06:42  yada
  OS_InitReset() is not called if SDK_SMALL_BUILD defined

  Revision 1.38  2004/09/29 06:02:34  yada
  CARD_Init() is not called if SDK_SMALL_BUILD defined

  Revision 1.37  2004/09/28 00:03:36  yada
  add CARD_Init() in OS_Init() on ARM9

  Revision 1.36  2004/09/15 06:15:20  yada
  in SDK_TEG release, PM_Init() not called.

  Revision 1.35  2004/09/15 01:01:08  yada
  in case SDK_TEG, CTRDG_Init() not called

  Revision 1.34  2004/09/13 10:48:24  yada
  add CTRDG_Init()

  Revision 1.33  2004/09/02 06:18:22  yada
  only change comment

  Revision 1.32  2004/09/01 04:20:03  yada
  add OS_InitReset()

  Revision 1.31  2004/08/26 12:17:23  yada
  always call PM_Init()

  Revision 1.30  2004/08/16 04:22:04  yada
  in ensata, not call PM_Init()

  Revision 1.29  2004/08/09 02:12:10  yada
  add PM_Init in case of ARM9

  Revision 1.28  2004/07/27 06:39:55  yada
  add checker for system shared validity

  Revision 1.27  2004/07/10 04:07:46  yasu
  Add short comment

  Revision 1.26  2004/07/08 02:30:35  yada
  add calling OS_InitArenaEx()

  Revision 1.25  2004/05/24 04:50:08  yada
  fix profile pragma

  Revision 1.24  2004/05/20 02:56:46  yada
  add profile off

  Revision 1.23  2004/05/20 02:51:26  yada
  add OS_InitThread()

  Revision 1.22  2004/03/18 12:59:15  yada
  ARM7ではException初期化を呼ばないようにした(暫定)

  Revision 1.21  2004/03/09 07:26:18  yada
  PXI_Init() 呼び出し位置を OS_InitLock() の上に移動

  Revision 1.20  2004/03/08 01:55:49  yasu
  add PXI_Init()

  Revision 1.19  2004/03/01 11:57:31  yada
  英語の関数comment作成

  Revision 1.18  2004/02/25 11:31:18  yada
  systemClock→Tick に伴う変更

  Revision 1.17  2004/02/24 11:50:34  yada
  OS_Init() の A7側から PAD_InitXYButton() 削除

  Revision 1.16  2004/02/24 04:50:59  yada
  A9側に OS_InitValarm をいれた。

  Revision 1.15  2004/02/20 12:24:21  yada
  A7のOS_Init() コメントアウトはずし

  Revision 1.14  2004/02/20 08:29:18  yasu
  Temporary fix

  Revision 1.13  2004/02/19 08:42:11  yada
  A7側アリーナの初期化追加

  Revision 1.12  2004/02/18 08:56:56  yada
  A7 からは OS_InitArea() を呼ばないようにした

  Revision 1.11  2004/02/18 01:20:04  yada
  ARM7にsystemClock,Alarm,XYButtonのinitを入れた

  Revision 1.10  2004/02/13 02:29:59  yada
  ARM9とAMR7 の場合わけ考慮

  Revision 1.9  2004/02/05 10:51:53  yada
  OS_InitException() を呼ぶようにした

  Revision 1.8  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.7  2004/01/30 11:45:06  yada
  一旦OS_InitException() を外す

  Revision 1.6  2004/01/30 08:06:14  yada
  OS_Init() で例外表示システム初期化を呼ぶようにした。

  Revision 1.5  2004/01/18 02:25:37  yada
  インデント等の整形

  Revision 1.4  2004/01/07 01:43:51  yada
  OS_InitArena() を呼ぶように変更

  Revision 1.3  2003/12/26 05:52:45  yada
  型ルール統一による変更

  Revision 1.2  2003/12/24 07:54:20  yada
  wram 初期化を行うようにした

  Revision 1.1  2003/12/22 01:35:52  yada
  OS_Init() 設置につき init.h 作成


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>
#include <nitro/mi/wram.h>
#include <nitro/pxi.h>
#include <nitro/spi.h>

#ifndef SDK_TEG
#include <nitro/ctrdg.h>
#endif

#include        <nitro/code32.h>
static asm void OSi_WaitVCount0( void )
{
        //---- set IME = 0
        //     ( use that LSB of HW_REG_BASE equal to 0 )
        mov             r12, #HW_REG_BASE
        ldr             r1,  [r12, #REG_IME_OFFSET]
        str             r12, [r12, #REG_IME_OFFSET]

        //---- adjust VCOUNT.
@wait_vcount_0:
        ldrh            r0, [r12, #REG_VCOUNT_OFFSET]
        cmp             r0, #0
        bne             @wait_vcount_0
        str             r1, [r12, #REG_IME_OFFSET]
        bx              lr
}
#include         <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OS_Init

  Description:  initialize sdk os

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#pragma profile off
void OS_Init(void)
{
#ifdef SDK_ARM9
    //---- system shared area check
    SDK_ASSERT((u32)&(OS_GetSystemWork()->command_area) == HW_CMD_AREA);

    //----------------------------------------------------------------
    // for ARM9

#ifdef SDK_ENABLE_ARM7_PRINT
    // Init PrintServer for ARM7 (if specified)
    OS_InitPrintServer();
#endif

    //---- Init Arena (arenas except SUBPRIV-WRAM)
    OS_InitArena();

    //---- Init interProcessor I/F
    //  Sync with ARM7 to enable OS_GetConsoleType()
    //  PXI_Init() must be called before OS_InitArenaEx()
    PXI_Init();

    //---- Init Spinlock
    OS_InitLock();

    //---- Init Arena (extended main)
    OS_InitArenaEx();

    //---- Init IRQ Table
    OS_InitIrqTable();

    //---- Init IRQ Stack checker
    OS_SetIrqStackChecker();

    //---- Init Exception System
    OS_InitException();

    //---- Init MI (Wram bank and DMA0 arranged)
    MI_Init();

    //---- Init VCountAlarm
    OS_InitVAlarm();

    //---- Init VRAM exclusive System
    OSi_InitVramExclusive();

    //---- Init Thread System
#ifndef SDK_NO_THREAD
    OS_InitThread();
#endif
    //---- Init Reset System
#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

    //---- Init Cartridge
#ifndef SDK_TEG
    CTRDG_Init();
#endif

    //---- Init Card
#ifndef SDK_SMALL_BUILD
    CARD_Init();
#endif

    //---- Init Power Manager
#ifndef SDK_TEG
    PM_Init();
#endif

    //---- adjust VCOUNT
    OSi_WaitVCount0();

#else  // SDK_ARM9
    //----------------------------------------------------------------
    // for ARM7

    //---- Init Arena (SUBPRIV-WRAM arena)
    OS_InitArena();

    //---- Init interProcessor I/F
    PXI_Init();

    //---- Init Spinlock
    OS_InitLock();

    //---- Init IRQ Table
    OS_InitIrqTable();

#define SDK_EXCEPTION_BUG
#ifndef SDK_EXCEPTION_BUG
    //---- Init Exception System
    OS_InitException();
#endif

    //---- Init Tick
    OS_InitTick();

    //---- Init Alarm System
    OS_InitAlarm();

    //---- Init Thread System
    OS_InitThread();

    //---- Init Reset System
#ifndef SDK_SMALL_BUILD
    OS_InitReset();
#endif

    //---- Init Cartridge
#ifndef SDK_TEG
    CTRDG_Init();
#endif

#endif // SDK_ARM9
}

#pragma profile reset
