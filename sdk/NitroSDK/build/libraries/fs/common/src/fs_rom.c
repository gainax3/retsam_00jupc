/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_rom.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fs_rom.c,v $
  Revision 1.39  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.38  2006/04/27 09:36:53  yosizaki
  fix about FSi_OnRomReadDone().

  Revision 1.37  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.36  2005/11/11 05:47:28  yosizaki
  disable all the implementations of FS on ARM7.

  Revision 1.35  2005/06/28 02:12:03  yosizaki
  add FS_CreateFileFromRom().

  Revision 1.34  2005/04/28 08:12:25  yosizaki
  add assertions about DMA-0 limitation.

  Revision 1.33  2005/04/18 02:48:32  yosizaki
  change FS_CreateReadServerThread() to inline-function.

  Revision 1.32  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.31  2005/02/18 07:53:19  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.30  2005/02/18 07:26:49  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.29  2004/11/12 07:03:27  yosizaki
  change wireless-downloaded program mounts empty archive.

  Revision 1.28  2004/09/04 13:21:32  yosizaki
  support multiboot-overlay.

  Revision 1.27  2004/09/02 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.26  2004/07/23 08:29:48  yosizaki
  add cast to OS_LockCard.

  Revision 1.25  2004/07/19 13:19:34  yosizaki
  change routine (for card_rom.c)

  Revision 1.24  2004/07/08 13:42:42  yosizaki
  move CARD-access routine to card/card_rom.c

  Revision 1.23  2004/06/30 04:33:00  yosizaki
  optimize CARD and cartridge access.

  Revision 1.22  2004/06/28 02:06:55  yosizaki
  add some comment.

  Revision 1.21  2004/06/22 01:48:14  yosizaki
  change to use OS_GetConsoleType().
  avoid cartridge access on SDK_TS.

  Revision 1.20  2004/06/11 05:42:20  yosizaki
  extern FSOverlayInfo.

  Revision 1.19  2004/06/03 13:33:07  yosizaki
  fix around archive asynchronous operation.

  Revision 1.18  2004/06/01 10:17:42  yosizaki
  add FS_OpenFileDirect(). (private)
  change around assertion code.

  Revision 1.17  2004/05/31 02:56:13  yosizaki
  change FSi_ASSERT_ARG() to FS_ASSERT_ARG().

  Revision 1.16  2004/05/25 08:52:55  yosizaki
  change around overlay. (prepare for multi-boot)

  Revision 1.15  2004/05/21 10:55:27  yosizaki
  change OS_IsThreadAvailable() to SDK_NO_THREAD.

  Revision 1.14  2004/05/21 10:08:36  yosizaki
  fix PXI_SendWordByFifo() check.

  Revision 1.13  2004/05/21 06:05:21  yosizaki
  fix PXI polling.

  Revision 1.12  2004/05/20 06:31:28  yosizaki
  small fix.

  Revision 1.11  2004/05/19 02:29:41  yosizaki
  fix thread stack bug.

  Revision 1.10  2004/05/18 04:38:53  yosizaki
  fix FSi_InitRom on ARM7. (avoid PXI blocking)

  Revision 1.9  2004/05/17 08:32:26  yosizaki
  add comment.
  add assertion of each interface function.

  Revision 1.8  2004/05/14 01:02:49  yosizaki
  change FSi_LoadOverlayDirect() to FS_LoadOverlay().

  Revision 1.7  2004/05/14 00:47:59  yosizaki
  add BOOL  FSi_LoadOverlayDirect().

  Revision 1.6  2004/05/13 05:05:35  yosizaki
  fix PXI initialization in ARM7.

  Revision 1.5  2004/05/13 04:05:57  yosizaki
  fix thread procedure.

  Revision 1.4  2004/05/12 05:22:55  yosizaki
  prepare a option "SDK_ARM7FS".

  Revision 1.3  2004/05/11 09:36:17  yosizaki
  add FS_CreateReadServerThread().

  Revision 1.2  2004/05/11 04:31:49  yosizaki
  move FS_LoadOverlay() from fs_file.

  Revision 1.1  2004/05/10 06:27:56  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/util.h"
#include "../include/rom.h"


/*****************************************************************************/
/* variable */

#if defined(FS_IMPLEMENT)

/* 使用する DMA チャンネル */
static u32 fsi_default_dma_no;

/* カード / カートリッジのロック ID */
s32     fsi_card_lock_id;

/* アタッチされたオーバーレイテーブル */
CARDRomRegion fsi_ovt9;
CARDRomRegion fsi_ovt7;


/* ROM アーカイブ */
FSArchive fsi_arc_rom;

#endif /* FS_IMPLEMENT */


/*****************************************************************************/
/* function */

#if defined(FS_IMPLEMENT)

/* 非同期のページ単位 CARD 処理が完了したときの処理 */
static void FSi_OnRomReadDone(void *p_arc)
{
    FS_NotifyArchiveAsyncEnd((FSArchive *)p_arc,
                             CARD_IsPulledOut()? FS_RESULT_ERROR : FS_RESULT_SUCCESS);
}

static FSResult FSi_ReadRomCallback(FSArchive *p_arc, void *dst, u32 src, u32 len)
{
    CARD_ReadRomAsync(fsi_default_dma_no, (const void *)src, dst, len, FSi_OnRomReadDone, p_arc);

    return FS_RESULT_PROC_ASYNC;
}

static FSResult FSi_WriteDummyCallback(FSArchive *p_arc, const void *src, u32 dst, u32 len)
{
    (void)p_arc;
    (void)src;
    (void)dst;
    (void)len;
    return FS_RESULT_FAILURE;
}

static FSResult FSi_RomArchiveProc(FSFile *p_file, FSCommandType cmd)
{
    (void)p_file;

    switch (cmd)
    {

    case FS_COMMAND_ACTIVATE:
        CARD_LockRom((u16)fsi_card_lock_id);
        return FS_RESULT_SUCCESS;

    case FS_COMMAND_IDLE:
        CARD_UnlockRom((u16)fsi_card_lock_id);
        return FS_RESULT_SUCCESS;

    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;

    default:
        return FS_RESULT_PROC_UNKNOWN;

    }

}

/* for wireless-downloaded child */
static FSResult FSi_ReadDummyCallback(FSArchive *p_arc, void *dst, u32 src, u32 len)
{
    (void)p_arc;
    (void)dst;
    (void)src;
    (void)len;
    return FS_RESULT_FAILURE;
}

static FSResult FSi_EmptyArchiveProc(FSFile *p_file, FSCommandType cmd)
{
    (void)p_file;
    (void)cmd;
    return FS_RESULT_UNSUPPORTED;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_InitRom

  Description:  Initialize "rom" archive

  Arguments:    default_dma_no   default DMA channel for ROM
                                 if out of range(0-3),
                                 use CpuCopy instead of DMA.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_InitRom(u32 default_dma_no)
{
    fsi_default_dma_no = default_dma_no;
    fsi_card_lock_id = OS_GetLockID();
    fsi_ovt9.offset = 0;
    fsi_ovt9.length = 0;
    fsi_ovt7.offset = 0;
    fsi_ovt7.length = 0;

    CARD_Init();

    /* default archive "rom" is always loaded on file system. */
    FS_InitArchive(&fsi_arc_rom);
    (void)FS_RegisterArchiveName(&fsi_arc_rom, "rom", 3);

    /* if wireless-downloaded program, mount empty directory and disable OVT */
    if (MB_IsMultiBootChild())
    {
        /* FS judges attached OVT is empty */
        fsi_ovt9.offset = (u32)~0;
        fsi_ovt9.length = 0;
        fsi_ovt7.offset = (u32)~0;
        fsi_ovt7.length = 0;
        /* any FS command will return FS_RESULT_UNSUPPORTED */
        FS_SetArchiveProc(&fsi_arc_rom, FSi_EmptyArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);
        (void)FS_LoadArchive(&fsi_arc_rom, 0x00000000,
                             0, 0, 0, 0, FSi_ReadDummyCallback, FSi_WriteDummyCallback);
    }
    /* if normal program, mount own directory */
    else
    {
        const CARDRomRegion *const fnt = CARD_GetRomRegionFNT();
        const CARDRomRegion *const fat = CARD_GetRomRegionFAT();

        FS_SetArchiveProc(&fsi_arc_rom, FSi_RomArchiveProc,
                          FS_ARCHIVE_PROC_WRITEFILE |
                          FS_ARCHIVE_PROC_ACTIVATE | FS_ARCHIVE_PROC_IDLE);

        if ((fnt->offset == 0xFFFFFFFF) || (fnt->offset == 0x00000000) ||
            (fat->offset == 0xFFFFFFFF) || (fat->offset == 0x00000000))
        {
            OS_Warning("file-system : no MAKEROM-information in rom header.");
        }
        else
        {
            (void)FS_LoadArchive(&fsi_arc_rom, 0x00000000,
                                 fat->offset, fat->length,
                                 fnt->offset, fnt->length,
                                 FSi_ReadRomCallback, FSi_WriteDummyCallback);
        }
    }

}

/*---------------------------------------------------------------------------*
  Name:         FS_GetDefaultDMA

  Description:  get default DMA channel

  Arguments:    None.

  Returns:      current DMA channel.
 *---------------------------------------------------------------------------*/
u32 FS_GetDefaultDMA(void)
{
    return fsi_default_dma_no;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetDefaultDMA

  Description:  set default DMA channel

  Arguments:    dma_no           default dma channel for FS
                                 if out of range(0-3),
                                 use CpuCopy instead of DMA.

  Returns:      previous DMA channel.
 *---------------------------------------------------------------------------*/
u32 FS_SetDefaultDMA(u32 dma_no)
{
    FS_ASSERT_DMA_CHANNEL(dma_no, fsi_default_dma_no);
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        u32     bak_dma_no = fsi_default_dma_no;
#if defined(FS_IMPLEMENT)
        BOOL    bak_stat = FS_SuspendArchive(&fsi_arc_rom);
#endif
        fsi_default_dma_no = dma_no;
#if defined(FS_IMPLEMENT)
        if (bak_stat)
            (void)FS_ResumeArchive(&fsi_arc_rom);
#endif
        (void)OS_RestoreInterrupts(bak_psr);
        return bak_dma_no;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_TryLoadTable

  Description:  preload FAT and FNT tables of file-system.
                if specified size is enough to load tables, load it to p_mem.
                then, all the operation using FAT and FNT is with p_mem.

  Arguments:    p_mem       buffer to preload
                size        byte length of p_mem

  Returns:      true size of memory for FAT and FNT.
 *---------------------------------------------------------------------------*/
u32 FS_TryLoadTable(void *p_mem, u32 size)
{
    return FS_LoadArchiveTables(&fsi_arc_rom, p_mem, size);
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileFromRom

  Description:  指定の CARD-ROM 領域をマップしたファイルを一時的に生成.

  Arguments:    p_file           ファイルハンドルを格納する FSFile 構造体
                offset           READ の対象となる CARD-ROM 領域先頭オフセット.
                size             対象領域の offset からのバイトサイズ

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFileFromRom(FSFile *p_file, u32 offset, u32 size)
{
    return FS_OpenFileDirect(p_file, &fsi_arc_rom, offset, offset + size, 0xFFFF);
}

#endif /* FS_IMPLEMENT */
