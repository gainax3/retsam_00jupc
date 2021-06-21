/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - 
  File:     os_spinLock.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_spinLock.c,v $
  Revision 1.40  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.39  2005/11/21 05:42:31  yasu
  OSi_WaitByLoop を inline 化

  Revision 1.38  2005/11/17 10:11:36  yasu
  OS_SpinWait を SVC_WaitByLoop で行なうように修正

  Revision 1.37  2005/03/04 10:06:01  yasu
  過剰な volatile を削除

  Revision 1.36  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.35  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.34  2004/07/23 04:27:47  yada
  OS_UnLock* restored for compatibility

  Revision 1.33  2004/07/23 00:57:17  yada
  rename OS_UnLock* -> OS_Unlock*

  Revision 1.32  2004/07/22 08:21:17  yada
  change some lockID's type u32->u16

  Revision 1.31  2004/07/08 07:56:14  yosizaki
  change around OS_LockCard in (TEG && ARM9)

  Revision 1.30  2004/07/06 04:45:28  yosizaki
  add assertion (TEG && ARM9)

  Revision 1.29  2004/06/21 11:03:25  yada
  fix comment. incorrect about return value.

  Revision 1.28  2004/05/20 06:37:03  yasu
  fix in Wait

  Revision 1.27  2004/04/30 07:37:44  yada
  HW_LOCKIDFLAG_MAIN/SUB -> HW_LOCK_ID_FLAG_MAIN/SUB

  Revision 1.26  2004/04/30 00:18:00  yada
  put HW_LOCKID_FLAG_MAIN/SUB

  Revision 1.25  2004/04/20 06:56:11  yada
  OS_TryLockCartridge() が、無条件に _ISDbgLib_AllocateEmualtor() を
  呼んでいたのを修正。

  Revision 1.24  2004/03/30 06:09:29  yada
  ldconst や lda を ldr = を使用する記述にした。

  Revision 1.23  2004/03/26 10:37:23  yada
  OS_GetLockID(), OS_ReleaseLockID() 追加

  Revision 1.22  2004/03/25 09:44:24  yada
  カードのunlock, tryLock 関数の定数修正。ウェイトでARM7はSVC呼ぶように。

  Revision 1.21  2004/03/25 09:22:39  yada
  内部ロック関数等で引数のFIQフラグの順序を変え効率をよくした。
  Cardのロック・アンロック修正

  Revision 1.20  2004/03/25 07:25:05  yada
  OS_LockCard() 等カードのロックについて追加

  Revision 1.19  2004/03/25 02:38:30  yada
  MI_SetMainMemoryPriority(), MI_SetCardProcessor(),MI_SetCartridgeProcessor()
  を、MI_～からMIi_～ に変更し内部関数にするようにした。

  Revision 1.18  2004/03/04 09:47:50  yada
  カートリッジのロックに限りIRQ/FIQ禁止にした。

  Revision 1.17  2004/02/13 08:58:52  yasu
  nitro_sp.h -> nitro.h

  Revision 1.16  2004/02/13 01:33:58  yada
  ARM9とAMR7 でインクルードファイル場合わけ

  Revision 1.15  2004/02/10 02:04:23  yada
  subp 用にinclude修正

  Revision 1.14  2004/02/10 01:25:06  yada
  UTL_* を MI_ に取り込んだことによる修正

  Revision 1.13  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.12  2004/02/05 02:00:37  yada
  UTL_SwapWord を os_utility.c に移行

  Revision 1.11  2004/01/18 02:25:37  yada
  インデント等の整形

  Revision 1.10  2004/01/14 01:44:01  yada
  lockやunlockの途中実行関数を設定出来るようにしたことへの対応

  Revision 1.9  2003/12/25 07:31:26  yada
  型ルール統一による変更

  Revision 1.8  2003/12/22 13:40:59  yasu
  memorymap.h の明示的読み込み

  Revision 1.7  2003/12/22 01:37:27  yada
  WRAM, VRAM-c,d の設定をコメントアウト

  Revision 1.6  2003/12/18 07:17:09  yada
  red-sdk 仕様から変更

  Revision 1.5  2003/12/17 08:21:56  yasu
  ARM7 との共用コード化

  Revision 1.4  2003/12/12 05:04:22  yasu
  _ISDbgLib_* 関数の追加

  Revision 1.3  2003/12/10 10:56:18  yasu
  ISDPrint で必要な SpinLock 関数のみの移植
  OS_LockWord() -> OS_LockByWord()
  volatile LockWord -> OS_LockWord などの変更

  Revision 1.2  2003/12/08 12:20:43  yada
  12/3　REDSDK への変更対応

  Revision 1.1  2003/11/29 01:25:46  yada
  ファイル名称変更

  Revision 1.2  2003/11/28 01:56:22  yada
  REDSDKの03-11-27反映
  但し殆どをコメントアウト

  Revision 1.1  2003/11/06 09:30:39  yada
  暫定版

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

void    _ISDbgLib_Initialize(void);
void    _ISDbgLib_AllocateEmualtor(void);
void    _ISDbgLib_FreeEmulator(void);

s32     OS_LockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnlockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_TryLockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*crtlFuncp) (void));

static s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                            BOOL disableFiq);
static s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                              BOOL disableFIQ);
static s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                               BOOL disableFIQ);

static void OSi_AllocateCartridgeBus(void);
static void OSi_FreeCartridgeBus(void);

static void OSi_AllocateCardBus(void);
static void OSi_FreeCardBus(void);

static void OSi_WaitByLoop(void);


#ifdef  SDK_ARM9
#define OSi_ASSERT_ID( id )       SDK_ASSERTMSG( id >= OS_MAINP_LOCK_ID_START && id <= OS_MAINP_SYSTEM_LOCK_ID, \
                             "lock ID %d is out of bounds", id )
#else
#define OSi_ASSERT_ID( id )       SDK_ASSERTMSG( id >= OS_SUBP_LOCK_ID_START && id <= OS_SUBP_SYSTEM_LOCK_ID, \
                             "lock ID %d is out of bounds", id )
#endif


#define OSi_LOCKID_INITIAL_FLAG_0     0xffffffff
#define OSi_LOCKID_INITIAL_FLAG_1     0xffff0000


#ifdef SDK_ARM9
#define OSi_ANYP_LOCK_ID_FLAG  HW_LOCK_ID_FLAG_MAIN
#define OSi_ANYP_LOCK_ID_START OS_MAINP_LOCK_ID_START
#else
#define OSi_ANYP_LOCK_ID_FLAG  HW_LOCK_ID_FLAG_SUB
#define OSi_ANYP_LOCK_ID_START OS_SUBP_LOCK_ID_START
#endif

//======================================================================
//                      DUMMY LOOP
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_WaitByLoop

  Description:  wait by for() loop

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void OSi_WaitByLoop(void)
{
    SVC_WaitByLoop(0x1000 / 4);
    // OS_SpinWait(0x1000);
}

//======================================================================
//                      INITIALIZE
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitLock

  Description:  initialize system lock variable 
                and privilege to access shared resources

                * cartridge exclusive control area is not cleared 
                  because debugger uses.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitLock(void)
{
    static BOOL isInitialized = FALSE;
    OSLockWord *lockp;

    if (isInitialized)
        return;                        // do it only once
    isInitialized = TRUE;

    lockp = (OSLockWord *)HW_INIT_LOCK_BUF;

#ifdef  SDK_ARM9
    {
        //
        // Code for MAIN PROCESSOR
        //

        lockp->lockFlag = 0;
        (void)OS_LockByWord(OS_MAINP_SYSTEM_LOCK_ID - 1, lockp, NULL);

        // サブプロセッサによる共有リソース未使用チェック
        while (lockp->extension != 0)
        {
            OSi_WaitByLoop();
        }

        // ロックIDカウンタ用フラグ 初期化
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

        // ロックバッファ クリア（カートリッジ領域以外）
        MI_CpuClear32((void *)HW_SHARED_LOCK_BUF, HW_CTRDG_LOCK_BUF - HW_SHARED_LOCK_BUF);

        // NITRO カードアクセス権 → サブプロセッサ
        MIi_SetCardProcessor(MI_PROCESSOR_ARM7);

        // カートリッジアクセス権   → サブプロセッサ
        MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM7);

#ifndef SDK_FINALROM
        _ISDbgLib_Initialize();
#endif

        (void)OS_UnlockByWord(OS_MAINP_SYSTEM_LOCK_ID - 1, lockp, NULL);
        (void)OS_LockByWord(OS_MAINP_SYSTEM_LOCK_ID, lockp, NULL);
    }

#else  // SDK_ARM7
    {
        //
        // Code for SUB PROCESSOR
        //

        lockp->extension = 0;
        while (lockp->ownerID != OS_MAINP_SYSTEM_LOCK_ID)
        {
            OSi_WaitByLoop();
        }

#ifndef SDK_FINALROM
        _ISDbgLib_Initialize();
#endif

        // ロックIDカウンタ用フラグ 初期化
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

        // メインプロセッサとの初期化終了の同期
        lockp->extension = OS_SUBP_SYSTEM_LOCK_ID;
    }
#endif
}


//======================================================================
//                      LOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoLockByWord

  Description:  do spinlock. keep to try till success

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function 
                disableFiq       whether do disable fiq

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                            BOOL disableFiq)
{
    s32     lastLockFlag;

    while ((lastLockFlag =
            OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, disableFiq)) > OS_LOCK_SUCCESS)
    {
        OSi_WaitByLoop();
    }

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_LockByWord

  Description:  do spinlock. keep to try till success.

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function 

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_LockByWord_IrqAndFiq

  Description:  do spinlock. keep to try till success.
                disable irq and fiq

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function 

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      UNLOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoUnlockByWord

  Description:  do unlock lock.

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function 
                disableFiq       whether do disable fiq
                
  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                              BOOL disableFIQ)
{
    OSIntrMode lastInterrupts;

    OSi_ASSERT_ID(lockID);

    if (lockID != lockp->ownerID)
    {
        return OS_UNLOCK_ERROR;
    }

    //---- Disable irq/fiq or irq
    lastInterrupts = (disableFIQ) ? OS_DisableInterrupts_IrqAndFiq() : OS_DisableInterrupts();

    lockp->ownerID = 0;
    if (ctrlFuncp)
    {
        ctrlFuncp();
    }
    lockp->lockFlag = 0;

    //---- Restore irq/fiq or irq
    if (disableFIQ)
    {
        (void)OS_RestoreInterrupts_IrqAndFiq(lastInterrupts);
    }
    else
    {
        (void)OS_RestoreInterrupts(lastInterrupts);
    }

    return OS_UNLOCK_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockByWord

  Description:  do unlock lock.
                disable irq and fiq

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function 

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
s32 OS_UnLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockByWord_IrqAndFiq

  Description:  do unlock lock.

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function 

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      TRY LOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoTryLockByWord

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function 
                disableFiq       whether do disable fiq

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                               BOOL disableFIQ)
{
    s32     lastLockFlag;
    OSIntrMode lastInterrupts;

    OSi_ASSERT_ID(lockID);

    //---- Disable irq/fiq or irq
    lastInterrupts = (disableFIQ) ? OS_DisableInterrupts_IrqAndFiq() : OS_DisableInterrupts();

    lastLockFlag = (s32)MI_SwapWord(lockID, &lockp->lockFlag);

    if (lastLockFlag == OS_LOCK_SUCCESS)
    {
        if (ctrlFuncp)
        {
            ctrlFuncp();
        }
        lockp->ownerID = lockID;
    }

    //---- Restore irq/fiq or irq
    if (disableFIQ)
    {
        (void)OS_RestoreInterrupts_IrqAndFiq(lastInterrupts);
    }
    else
    {
        (void)OS_RestoreInterrupts(lastInterrupts);
    }

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockByWord

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function 

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockByWord_IrqAndFiq

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function 

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      CARTRIDGE
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_LockCartridge

  Description:  lock cartridge

  Arguments:    lockID           lock ID

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    OSi_ASSERT_ID(lockID);

    lastLockFlag =
        OSi_DoLockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_AllocateCartridgeBus, TRUE);

#ifndef SDK_FINALROM
    _ISDbgLib_AllocateEmualtor();
#endif

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockCartridge

  Description:  unlock cartridge

  Arguments:    lockID           lock ID

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    OSi_ASSERT_ID(lockID);

#ifndef SDK_FINALROM
    _ISDbgLib_FreeEmulator();
#endif

    lastLockFlag =
        OSi_DoUnlockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_FreeCartridgeBus, TRUE);

    return lastLockFlag;
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
asm s32 OS_UnLockCartridge( u16 lockID )
{
  ldr  r1, =OS_UnlockCartridge
  bx   r1
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockCartridge

  Description:  try to lock cartridge

  Arguments:    lockID           lock ID

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    lastLockFlag =
        OSi_DoTryLockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_AllocateCartridgeBus,
                            TRUE);

#ifndef SDK_FINALROM
    if (lastLockFlag == OS_LOCK_SUCCESS)
    {
        _ISDbgLib_AllocateEmualtor();
    }
#endif

    return lastLockFlag;
}

//---------------- 
static void OSi_AllocateCartridgeBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM9);       // Cartridge for MAIN
#endif
}

//---------------- 
static void OSi_FreeCartridgeBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM7);       // Cartridge for SUB
#endif
}

//======================================================================
//                      CARD
//======================================================================

/*
 * 20040708:yosizaki
 * TEG では ARM9 がロックした後 ARM7 へ転送要求する必要がある.
 * このときロックが共有だと ARM9 のロック中で ARM 7 がロックできない.
 * ので, 単に個々のロック ID によってプロセッサ内での排他制御を行う.
 */
#if	defined(SDK_TEG)

static u32 osi_card_lock = OS_LOCK_ID_ERROR;

static s32 OSi_LockCardID(u16 lockID, BOOL blocking)
{
    s32     ret = OS_LOCK_ID_ERROR;
    for (;;)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        ret = osi_card_lock;
        if (ret == OS_LOCK_ID_ERROR)
        {
            osi_card_lock = lockID;
            ret = OS_LOCK_SUCCESS;
        }
        (void)OS_RestoreInterrupts(bak_psr);
        if ((ret != OS_LOCK_SUCCESS) && blocking)
        {
            OSi_WaitByLoop();
            continue;
        }
        break;
    }
    return ret;
}

static s32 OSi_UnlockCardID(u16 lockID)
{
    if (osi_card_lock != lockID)
        return OS_UNLOCK_ERROR;
    osi_card_lock = OS_LOCK_ID_ERROR;
    return OS_UNLOCK_SUCCESS;
}

#endif


/*---------------------------------------------------------------------------*
  Name:         OS_LockCard

  Description:  lock card

  Arguments:    lockID           lock ID

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockCard(u16 lockID)
{
    OSi_ASSERT_ID(lockID);

#if	defined(SDK_TEG)
    return OSi_LockCardID(lockID, TRUE);
#else
    return OS_LockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_AllocateCardBus);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockCard

  Description:  unlock card

  Arguments:    lockID           lock ID

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockCard(u16 lockID)
{
    OSi_ASSERT_ID(lockID);

#if	defined(SDK_TEG)
    return OSi_UnlockCardID(lockID);
#else
    return OS_UnlockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_FreeCardBus);
#endif
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
asm s32 OS_UnLockCard( u16 lockID )
{
  ldr  r1, =OS_UnlockCard
  bx   r1
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockCard

  Description:  try to lock card

  Arguments:    lockID           lock ID

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockCard(u16 lockID)
{
#if	defined(SDK_TEG)
    return OSi_LockCardID(lockID, FALSE);
#else
    return OS_TryLockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_AllocateCardBus);
#endif
}

//---------------- 
static void OSi_AllocateCardBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCardProcessor(MI_PROCESSOR_ARM9);    // Card for MAIN
#endif
}

//---------------- 
static void OSi_FreeCardBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCardProcessor(MI_PROCESSOR_ARM7);    // Card for SUB
#endif
}


//======================================================================
//                      READ OWNER
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_ReadOwnerOfLockWord

  Description:  read owner id of lock

       ・モジュールID が非0 の場合はその時点でどちらのプロセッサ側が
         所有権を持っているのかを確認できます。
       ・共有リソースの場合は「メインプロセッサ側が所有権を持っている状態」
         のみを割り込みを禁止することによって維持することができます。
         その他の状態はサブプロセッサが変化させてしまう可能性があります。
       ・所有モジュールID が 0 であってもロック変数が解除されているとは限りません。

  Arguments:    lockp            pointer to lock

  Returns:      owner id
 *---------------------------------------------------------------------------*/
u16 OS_ReadOwnerOfLockWord(OSLockWord *lockp)
{
    return lockp->ownerID;
}

//======================================================================
//                     LOCK ID
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetLockID

  Description:  get lock ID

  Arguments:    None.

  Returns:      OS_LOCK_ID_ERROR, if fail to get ID

                if ARM9
                   0x40～0x6f       lockID
                else if ARM7
                   0x80～0xaf       lockID
                endif

                *Notice:  ID is allocated only 48 pattern at a highest.

                IDは48種類までしか割り当てることができません。
               モジュール内にて複数のロック変数を制御する場合は
               できるだけ1つのIDを使用するようにして下さい。
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm s32 OS_GetLockID( void )
{
    //---- フラグの前32ビットに立っているフラグ(空きID)があるか
    ldr    r3, =OSi_ANYP_LOCK_ID_FLAG
    ldr    r1, [r3, #0]

#ifdef SDK_ARM9
    clz    r2, r1
#else
    mov    r2, #0
    mov    r0, #0x80000000
_lp1:
    tst    r1, r0
    bne    _ex1
    add    r2, r2, #1
    cmp    r2, #32
    beq    _ex1

    mov    r0, r0, lsr #1
    b      _lp1
 _ex1:
#endif
    cmp    r2, #32

    //---- 空きIDがあるとき
    movne  r0, #OSi_ANYP_LOCK_ID_START
    bne    _1

    //---- 後ろ323ビットに立っているフラグ(空きID)があるか
    add    r3, r3, #4
    ldr    r1, [r3, #0]
#ifdef SDK_ARM9
    clz    r2, r1
#else
    mov    r2, #0
    mov    r0, #0x80000000
_lp2:
    tst    r1, r0
    bne    _ex2
    add    r2, r2, #1
    cmp    r2, #32
    beq    _ex2

    mov    r0, r0, lsr #1
    b      _lp2
 _ex2:
#endif
    cmp    r2, #32

    //---- 空きIDがない
    ldr    r0, =OS_LOCK_ID_ERROR
    bxeq   lr

    //---- 空きIDがあるとき
    mov    r0, #OSi_ANYP_LOCK_ID_START+32

_1:
    add    r0, r0, r2
    mov    r1, #0x80000000
    mov    r1, r1, lsr r2

    ldr    r2, [r3, #0]
    bic    r2, r2, r1
    str    r2, [r3, #0]

    bx     lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_ReleaseLockID

  Description:  release lock ID

  Arguments:    id to tend to release

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void OS_ReleaseLockID( register u16 lockID )
{
#pragma unused( lockID )

    ldr    r3, =OSi_ANYP_LOCK_ID_FLAG

    cmp    r0, #OSi_ANYP_LOCK_ID_START+32
    addpl  r3, r3, #4

    subpl  r0, r0, #OSi_ANYP_LOCK_ID_START+32
    submi  r0, r0, #OSi_ANYP_LOCK_ID_START

    mov r1, #0x80000000
    mov r1, r1, lsr r0

    ldr    r2, [r3, #0]
    orr    r2, r2, r1
    str    r2, [r3, #0]

    bx     lr
}
#include <nitro/codereset.h>
