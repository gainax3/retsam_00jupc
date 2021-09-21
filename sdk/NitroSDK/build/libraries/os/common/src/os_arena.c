/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_arena.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_arena.c,v $
  Revision 1.44  2006/01/18 05:46:04  okubata_ryoma
  do-indentの修正

  Revision 1.43  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.42  2005/10/18 10:42:36  yada
  modified about getting arena HI of PREV_WRAM in ARM7

  Revision 1.41  2005/09/06 04:57:41  yada
  fix arena default hi address on DTCM

  Revision 1.40  2005/08/26 11:49:31  yada
  ARENALO is considered for DTCM, ITCM, MAINEX autoload

  Revision 1.39  2005/05/25 04:03:05  terui
  MB 子機の場合でもメインメモリを8MBに設定できるように修正

  Revision 1.38  2005/04/14 06:46:13  yasu
  Multiboot 子機の場合、使用可能メモリを 4MB に制限するようにした

  Revision 1.37  2005/04/13 13:00:10  terui
  OS_InitArenaEx関数内で、4M設定時にARM7専用リージョンの位置を0x027e0000 から 0x023e0000に変更するように修正。

  Revision 1.36  2005/02/28 09:57:43  seiki_masashi
  fix a debug message.

  Revision 1.35  2005/02/28 05:26:28  yosizaki
  do-indent.

  Revision 1.34  2004/12/21 11:24:27  yada
  add OS_EnableMainExArena() / OS_DisableMainExArena()

  Revision 1.33  2004/11/02 11:58:22  yada
  fix a little

  Revision 1.32  2004/10/04 06:16:53  yada
  protection region for MainRAM  is set to 4MB if specified

  Revision 1.31  2004/08/31 01:02:51  yada
  add functions to get/set initial setting in specified arena

  Revision 1.30  2004/08/18 02:45:44  yada
  change sub-processor private arena of mainRAM

  Revision 1.29  2004/07/29 07:25:56  yada
  remove OS_Printf() in OS_GetArenaLo()

  Revision 1.28  2004/07/20 11:41:23  yada
  in case sysStack=0 (means maximum size), DTCM arena lo
  was set to HW_DTCM. fix to SDK_AUTOLOAD_DTCM_BSS_END.

  Revision 1.27  2004/07/12 12:25:21  yasu
  Fix arena setting

  Revision 1.26  2004/07/10 04:07:56  yasu
  Add short comment

  Revision 1.25  2004/07/08 02:22:21  yada
  add OS_InitArenaEx(),  setting for extended mainRam arena

  Revision 1.24  2004/07/01 09:40:31  yasu
  add TARGET_ROM

  Revision 1.23  2004/05/27 04:40:25  yada
  change setting arena rule for ARM7

  Revision 1.22  2004/04/14 00:07:33  yada
  add checkcode in allocation memory from arena

  Revision 1.21  2004/02/20 12:23:47  yada
  初期化バグ修正

  Revision 1.20  2004/02/19 08:41:41  yada
  ARM7 と ARM9 の初期化を分けた

  Revision 1.19  2004/02/16 09:39:46  yada
  Sysスタック修正

  Revision 1.18  2004/02/14 08:23:12  yasu
  comment out TCM arena setting code on ARM7 due to pass nightly build

  Revision 1.17  2004/02/14 07:05:47  yasu
  fix indent level

  Revision 1.16  2004/02/13 10:38:26  yada
  ARENAを６個から９個に

  Revision 1.15  2004/02/13 04:05:36  yada
  ARM9とAMR7 の場合わけ考慮

  Revision 1.14  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  2004/01/18 01:25:40  yada
  インデント等の整形のみ

  Revision 1.12  2004/01/16 11:47:20  yada
  arena情報の位置を動かしたことによる修正

  Revision 1.11  2004/01/16 08:10:03  yada
  DTCMアリーナ設定のASSERT修正

  Revision 1.10  2004/01/16 07:25:26  yada
  arena の initialized フラグ位置修正

  Revision 1.9  2004/01/16 04:07:39  yada
  idle Thread のスタックを lcf定義から取るように変更

  Revision 1.8  2004/01/15 12:13:55  yada
  SystemWork にアリーナ構造体を取るようにした

  Revision 1.7  2004/01/15 10:07:25  yasu
  Use SDK_MAIN_ARENA_LO from LCF to resolve arena region

  Revision 1.6  2004/01/07 02:52:04  yada
  小変更

  Revision 1.5  2004/01/07 01:42:40  yada
  arena初期化部分の変更

  Revision 1.4  2004/01/05 04:28:24  yada
  各アリーナの仮初期化を入れた

  Revision 1.3  2003/12/26 08:21:13  yada
  Assert 中の不具合修正

  Revision 1.2  2003/12/26 08:11:35  yada
  IDの型を int -> OSArenaId に

  Revision 1.1  2003/12/26 06:42:33  yada
  初版


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#define OSi_TRUNC(n, a)     (((u32) (n)) & ~((a) - 1))
#define OSi_ROUND(n, a)     (((u32) (n) + (a) - 1) & ~((a) - 1))

#define OS_ERR_GETARENAHI_INIT     "OS_GetArenaHi: init in advance"
#define OS_ERR_GETARENAHI_INVALID  "OS_GetArenaHi: invalid arena (hi<lo)"
#define OS_ERR_GETARENALO_INIT     "OS_GetArenaLo: init in advance"
#define OS_ERR_GETARENALO_INVALID  "OS_GetArenaLo: invalid arena (hi<lo)"

#define SDK_ARENAID_ASSERT( id )        SDK_ASSERT( (u32)id < OS_ARENA_MAX )


//---- Arena infomation Address ( allocated in Shared Memory )
#define OSi_GetArenaInfo()              (*(OSArenaInfo*)HW_ARENA_INFO_BUF)


//---- default Main Arena Address (for arm9)
extern void SDK_MAIN_ARENA_LO(void);
#define OSi_MAIN_ARENA_LO_DEFAULT               ((u32)SDK_MAIN_ARENA_LO)        // Defined by LCF
#define OSi_MAIN_ARENA_HI_DEFAULT               HW_MAIN_MEM_MAIN_END

//---- default Main Arena Address (for arm7)
#ifdef SDK_ARM7
extern void SDK_SUBPRIV_ARENA_LO(void);
#define OSi_MAIN_SUBPRIV_ARENA_LO_DEFAULT       ((u32)SDK_SUBPRIV_ARENA_LO)     // Defined by LCF
#define OSi_MAIN_SUBPRIV_ARENA_HI_DEFAULT       HW_MAIN_MEM_SUB_END
#endif

//---- default Extended Main Arena Address (for arm9)
#ifdef SDK_ARM9
extern void SDK_SECTION_ARENA_EX_START(void);
#define OSi_MAINEX_ARENA_LO_DEFAULT             ((u32)SDK_SECTION_ARENA_EX_START)       // Defined by LCF
#define OSi_MAINEX_ARENA_HI_DEFAULT             HW_MAIN_MEM_DEBUGGER
#endif

#ifdef SDK_ARM9
//---- default DTCM Arena Address (for arm9)
extern void SDK_SECTION_ARENA_DTCM_START(void);
#define OSi_DTCM_ARENA_LO_DEFAULT               ((u32)SDK_SECTION_ARENA_DTCM_START)     // Defined by LCF

//---- default ITCM Arena Address (for arm9)
extern void SDK_SECTION_ARENA_ITCM_START(void);
#define OSi_ITCM_ARENA_LO_DEFAULT               ((u32)SDK_SECTION_ARENA_ITCM_START)     // Defined by LCF
#endif

//---- default common WorkRAM (for Main-Proc) Arena Address
#define OSi_WRAM_MAIN_ARENA_LO_DEFAULT          HW_WRAM
#define OSi_WRAM_MAIN_ARENA_HI_DEFAULT          HW_WRAM

//---- default common WorkRAM (for Sub-Proc) Arena Address
#ifdef SDK_ARM7
extern void SDK_WRAM_ARENA_LO(void);
#define OSi_WRAM_SUB_ARENA_LO_DEFAULT           ((u32)SDK_WRAM_ARENA_LO)        // Defined by LCF
#define OSi_WRAM_SUB_ARENA_HI_DEFAULT           HW_WRAM_END
#endif

//---- default Sub-Proc Private WorkRAM Arena Address (for from ARM9)
#ifdef SDK_ARM7
#define OSi_WRAM_SUBPRIV_ARENA_LO_DEFAULT       HW_PRV_WRAM
#define OSi_WRAM_SUBPRIV_ARENA_HI_DEFAULT       HW_PRV_WRAM_END
#endif

//---- stack size indication from LCF
extern void SDK_SYS_STACKSIZE(void);
#define OSi_SYS_STACKSIZE               ((int)SDK_SYS_STACKSIZE)
extern void SDK_IRQ_STACKSIZE(void);
#define OSi_IRQ_STACKSIZE               ((int)SDK_IRQ_STACKSIZE)

static BOOL OSi_Initialized = FALSE;

#ifdef SDK_ARM9
#ifdef SDK_4M
BOOL    OSi_MainExArenaEnabled = FALSE;
#else
BOOL    OSi_MainExArenaEnabled = TRUE;
#endif
#endif


//================================================================================
//        INIT
//================================================================================
#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         OS_InitArena

  Description:  Initialize Arena

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitArena(void)
{
    //---- check if arena initialized
    if (OSi_Initialized)
    {
        return;
    }
    OSi_Initialized = TRUE;

    //---------------- set Arena Lo/Hi addresses
    //---- MainRam, Main-Processor private
    OS_InitArenaHiAndLo(OS_ARENA_MAIN);

    //---- Extended MainRam (temporary. set in OS_InitArenaEx later.)
    OS_SetArenaLo(OS_ARENA_MAINEX, (void *)0);
    OS_SetArenaHi(OS_ARENA_MAINEX, (void *)0);

    //---- ITCM
    OS_InitArenaHiAndLo(OS_ARENA_ITCM);

    //---- DTCM
    OS_InitArenaHiAndLo(OS_ARENA_DTCM);

    //---- set Shared Memory Arena
    OS_InitArenaHiAndLo(OS_ARENA_SHARED);

    //---- Work RAM, for Main-Processor
    OS_InitArenaHiAndLo(OS_ARENA_WRAM_MAIN);
}
#endif

#ifdef SDK_ARM7
/*---------------------------------------------------------------------------*
  Name:         OS_InitArena

  Description:  Initialize WRAM_SUBPRIV_ARENA

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitArena(void)
{
    //---- check if arena initialized
    if (OSi_Initialized)
    {
        return;
    }
    OSi_Initialized = TRUE;

    //---------------- set Arena Lo/Hi addresses
    //---- MainRam, Sub-Processor private
    OS_InitArenaHiAndLo(OS_ARENA_MAIN_SUBPRIV);

    //---- Work RAM, for Sub-Processor
    OS_InitArenaHiAndLo(OS_ARENA_WRAM_SUB);

    //---- Sub-Processor private Work RAM
    OS_InitArenaHiAndLo(OS_ARENA_WRAM_SUBPRIV);
}
#endif

#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         OS_InitArenaEx

  Description:  Initialize extended MainRam Arena 
                OS_InitArena() must be called before
  
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitArenaEx(void)
{
    OS_InitArenaHiAndLo(OS_ARENA_MAINEX);

    /*
     * MB 子機の場合でも、デバッグ目的でメインメモリを 8M に拡張して
     * メインメモリ上の拡張アリーナを使用できるように修正。
     * MB 子機において、オートロードが実行される前の段階で
     * 重要な情報が格納されている 0x023fe000周辺のメモリ領域から本当に必要な
     * 情報を抽出しつつ 0x027ffa80 周辺に退避するという対策が入ったため
     * 0x023fe000 周辺を保護する必要がなくなった事を受けての修正です。
     */
    if (!OSi_MainExArenaEnabled ||
        ((OS_GetConsoleType() & OS_CONSOLE_SIZE_MASK) == OS_CONSOLE_SIZE_4MB))
    {
        //---- no Extended MainRam arena
        OS_SetProtectionRegion(1, HW_MAIN_MEM_MAIN, 4MB);
        //---- Move exclusive area for ARM7
#if     HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x1000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 4KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x2000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 8KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x4000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 16KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x8000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 32KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x10000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 64KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x20000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 128KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x40000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 256KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x80000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 512KB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x100000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 1MB);
#elif   HW_MAIN_MEM_SUB_SIZE+HW_MAIN_MEM_SHARED_SIZE == 0x200000
        OS_SetProtectionRegion(2, HW_MAIN_MEM_MAIN_END, 2MB);
#else
#pragma message(ERROR: Size unmatch HW_MAIN_MEM_MAIN_END)
#endif
    }
}
#endif // ifdef SDK_ARM9

//================================================================================
//        GET ARENA INFO
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaInfo

  Description:  Get the pointer of Arena structure

  Arguments:    None.

  Returns:      the pointer of Arena structure
 *---------------------------------------------------------------------------*/
void   *OS_GetArenaInfo(void)
{
    return (void *)&OSi_GetArenaInfo();
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaHi

  Description:  Get the high boundary of the arena

  Arguments:    id : arena ID

  Returns:      the high boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetArenaHi(OSArenaId id)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);
    SDK_ASSERTMSG((u32)OSi_GetArenaInfo().lo[id] != 0xffffffff, OS_ERR_GETARENALO_INIT);
    SDK_ASSERTMSG((u32)OSi_GetArenaInfo().lo[id] <= (u32)OSi_GetArenaInfo().hi[id],
                  OS_ERR_GETARENALO_INVALID);

    return OSi_GetArenaInfo().hi[id];
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaLo

  Description:  Get the low boundary of the arena

  Arguments:    id : arena ID

  Returns:      the low boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetArenaLo(OSArenaId id)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);
    SDK_ASSERTMSG((u32)OSi_GetArenaInfo().lo[id] != 0xffffffff, OS_ERR_GETARENALO_INIT);
    SDK_ASSERTMSG((u32)OSi_GetArenaInfo().lo[id] <= (u32)OSi_GetArenaInfo().hi[id],
                  OS_ERR_GETARENALO_INVALID);

    return OSi_GetArenaInfo().lo[id];
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetInitArenaHi

  Description:  Get the initial high boundary of the arena

  Arguments:    id : arena ID

  Returns:      the initial high boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetInitArenaHi(OSArenaId id)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);

    switch (id)
    {
#ifdef SDK_ARM9
    case OS_ARENA_MAIN:
        return (void *)OSi_MAIN_ARENA_HI_DEFAULT;
    case OS_ARENA_MAINEX:
        if (!OSi_MainExArenaEnabled ||
            ((OS_GetConsoleType() & OS_CONSOLE_SIZE_MASK) == OS_CONSOLE_SIZE_4MB))
        {
            return (void *)0;
        }
        else
        {
            return (void *)OSi_MAINEX_ARENA_HI_DEFAULT;
        }
    case OS_ARENA_ITCM:
        return (void *)HW_ITCM_ARENA_HI_DEFAULT;
    case OS_ARENA_DTCM:
        {
            u32     irqStackLo;
            u32     sysStackLo;

            irqStackLo = (u32)HW_DTCM_IRQ_STACK_END - OSi_IRQ_STACKSIZE;

            if (OSi_SYS_STACKSIZE == 0) // maximum SysStack in DTCM
            {
                sysStackLo = HW_DTCM;
                if (sysStackLo < OSi_DTCM_ARENA_LO_DEFAULT)
                {
                    sysStackLo = OSi_DTCM_ARENA_LO_DEFAULT;
                }
            }
            else if (OSi_SYS_STACKSIZE < 0)
            {
                sysStackLo = OSi_DTCM_ARENA_LO_DEFAULT - OSi_SYS_STACKSIZE;
            }
            else
            {
                sysStackLo = irqStackLo - OSi_SYS_STACKSIZE;
            }
            SDK_ASSERT((u32)OSi_DTCM_ARENA_LO_DEFAULT <= sysStackLo && sysStackLo < irqStackLo);
            return (void *)sysStackLo;
        }
    case OS_ARENA_SHARED:
        return (void *)HW_SHARED_ARENA_HI_DEFAULT;
    case OS_ARENA_WRAM_MAIN:
        return (void *)(void *)OSi_WRAM_MAIN_ARENA_HI_DEFAULT;
#else
    case OS_ARENA_MAIN_SUBPRIV:
        return (void *)OSi_MAIN_SUBPRIV_ARENA_HI_DEFAULT;
    case OS_ARENA_WRAM_SUB:
        return (void *)OSi_WRAM_SUB_ARENA_HI_DEFAULT;
    case OS_ARENA_WRAM_SUBPRIV:
        {
            u32     irqStackLo;
            u32     sysStackLo;

            // ---- Sub-Processor private Work RAM
            // decide sysStackLo
            irqStackLo = (u32)HW_PRV_WRAM_IRQ_STACK_END - OSi_IRQ_STACKSIZE;
            sysStackLo = (u32)HW_PRV_WRAM;

            //---- if program area is larger than the size of common wram
            if ((u32)SDK_WRAM_ARENA_LO > (u32)HW_PRV_WRAM)
            {
                sysStackLo = (u32)SDK_WRAM_ARENA_LO;
            }

            if (OSi_SYS_STACKSIZE == 0) // maximum SysStack in SURPRIV-WRAM
            {
                // do nothing
            }
            else if (OSi_SYS_STACKSIZE < 0)
            {
                sysStackLo -= OSi_SYS_STACKSIZE;
            }
            else
            {
                sysStackLo = irqStackLo - OSi_SYS_STACKSIZE;
            }
            SDK_ASSERT((u32)HW_PRV_WRAM <= sysStackLo && sysStackLo < irqStackLo);
            return (void *)sysStackLo;
        }
#endif
    default:
        SDK_WARNING(0, "Bad arena id");
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetInitArenaLo

  Description:  Get the initial low boundary of the arena

  Arguments:    id : arena ID

  Returns:      the initial low boundary of the arena
 *---------------------------------------------------------------------------*/
void   *OS_GetInitArenaLo(OSArenaId id)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);

    switch (id)
    {
#ifdef SDK_ARM9
    case OS_ARENA_MAIN:
        return (void *)OSi_MAIN_ARENA_LO_DEFAULT;
    case OS_ARENA_MAINEX:
        if (!OSi_MainExArenaEnabled ||
            ((OS_GetConsoleType() & OS_CONSOLE_SIZE_MASK) == OS_CONSOLE_SIZE_4MB))
        {
            return (void *)0;
        }
        else
        {
            return (void *)OSi_MAINEX_ARENA_LO_DEFAULT;
        }
    case OS_ARENA_ITCM:
        return (void *)OSi_ITCM_ARENA_LO_DEFAULT;
    case OS_ARENA_DTCM:
        return (void *)OSi_DTCM_ARENA_LO_DEFAULT;
    case OS_ARENA_SHARED:
        return (void *)HW_SHARED_ARENA_LO_DEFAULT;
    case OS_ARENA_WRAM_MAIN:
        return (void *)OSi_WRAM_MAIN_ARENA_LO_DEFAULT;
#else
    case OS_ARENA_MAIN_SUBPRIV:
        return (void *)OSi_MAIN_SUBPRIV_ARENA_LO_DEFAULT;
    case OS_ARENA_WRAM_SUB:
        {
            u32     wramSubLo = OSi_WRAM_SUB_ARENA_LO_DEFAULT;
            if ((u32)HW_WRAM_END < (u32)wramSubLo)
            {
                wramSubLo = (u32)HW_WRAM_END;
            }
            return (void *)wramSubLo;
        }
    case OS_ARENA_WRAM_SUBPRIV:
        {
            u32     privWramLo = OSi_WRAM_SUBPRIV_ARENA_LO_DEFAULT;
            if ((u32)SDK_WRAM_ARENA_LO > (u32)privWramLo)
            {
                privWramLo = (u32)SDK_WRAM_ARENA_LO;
            }
            return (void *)privWramLo;
        }
#endif
    default:
        SDK_WARNING(0, "Bad arena id");
    }

    return NULL;
}

//================================================================================
//        SET ARENA INFO
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetArenaHi

  Description:  Set the high boundary of the arena

  Arguments:    id    : arena ID
                newHi : New high boundary of the arena.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetArenaHi(OSArenaId id, void *newHi)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);

    OSi_GetArenaInfo().hi[id] = newHi;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetArenaLo

  Description:  Set the low boundary of the arena

  Arguments:    id    : arena ID
                newLo : New low boundary of the arena.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetArenaLo(OSArenaId id, void *newLo)
{
    SDK_ASSERT(OSi_Initialized);
    SDK_ARENAID_ASSERT(id);

    OSi_GetArenaInfo().lo[id] = newLo;
}


//================================================================================
//        ALLOC
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_AllocFromArenaLo

  Description:  Allocates /size/ bytes from the low boundary of the arena.
                Some additional memory will also be consumed from arena for
                the alignment.

  Arguments:    id    : arena ID
                size  : Size of object to be allocated
                align : Alignment of object and new arena boundary to
                        be set (must be power of 2).

  Returns:      a pointer to the allocated space aligned with /align/ bytes
                boundaries. The new low boundary of the arena is also
                aligned with /align/ bytes boundaries.

                NULL if cannot allocate.
 *---------------------------------------------------------------------------*/
void   *OS_AllocFromArenaLo(OSArenaId id, u32 size, u32 align)
{
    void   *ptr;
    u8     *arenaLo;

    ptr = OS_GetArenaLo(id);
    if (!ptr)
    {
        return NULL;
    }

    arenaLo = ptr = (void *)OSi_ROUND(ptr, align);
    arenaLo += size;
    arenaLo = (u8 *)OSi_ROUND(arenaLo, align);

    if (arenaLo > OS_GetArenaHi(id))
    {
        return NULL;
    }

    OS_SetArenaLo(id, arenaLo);

    return ptr;
}

/*---------------------------------------------------------------------------*
  Name:         OS_AllocFromArenaHi

  Description:  Allocates /size/ bytes from the high boundary of the arena.
                Some additional memory will also be consumed from arena for
                the alignment.

  Arguments:    id    : arena ID
                size  : Size of object to be allocated
                align : Alignment of object and new arena boundary to
                        be set (must be power of 2).

  Returns:      a pointer to the allocated space aligned with /align/ bytes
                boundaries. The new high boundary of the arena is also
                aligned with /align/ bytes boundaries.

                NULL if cannot allocate.
 *---------------------------------------------------------------------------*/
void   *OS_AllocFromArenaHi(OSArenaId id, u32 size, u32 align)
{
    void   *ptr;
    u8     *arenaHi;

    arenaHi = OS_GetArenaHi(id);
    if (!arenaHi)
    {
        return NULL;
    }

    arenaHi = (u8 *)OSi_TRUNC(arenaHi, align);
    arenaHi -= size;
    arenaHi = ptr = (void *)OSi_TRUNC(arenaHi, align);

    if (arenaHi < OS_GetArenaLo(id))
    {
        return NULL;
    }

    OS_SetArenaHi(id, arenaHi);

    return ptr;
}

#ifdef SDK_ARM9
/*---------------------------------------------------------------------------*
  Name:         OS_EnableMainExArena

  Description:  Set extended main memory arena enable.
                This function should be called before OS_Init().

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_EnableMainExArena(void)
{
    SDK_ASSERT(!OSi_Initialized);

    OSi_MainExArenaEnabled = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_DisableMainExArena

  Description:  Set extended main memory arena disable.
                This function should be called before OS_Init().

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_DisableMainExArena(void)
{
    SDK_ASSERT(!OSi_Initialized);

    OSi_MainExArenaEnabled = FALSE;
}
#endif
