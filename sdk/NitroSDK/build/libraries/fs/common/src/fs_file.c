/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_file.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fs_file.c,v $
  Revision 1.50  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.49  2005/11/11 05:47:28  yosizaki
  disable all the implementations of FS on ARM7.

  Revision 1.48  2005/06/17 13:02:12  yosizaki
  fix FS_WaitAsync. (add FS_FILE_STATUS_OPERATING)

  Revision 1.47  2005/06/09 06:52:30  yosizaki
  fix FS_WaitAsync().

  Revision 1.46  2005/05/30 04:19:22  yosizaki
  add FS_End().
  add comments.

  Revision 1.45  2005/04/28 08:12:25  yosizaki
  add assertions about DMA-0 limitation.

  Revision 1.44  2005/04/18 02:47:56  yosizaki
  add macro 'FS_ASSERT_IRQ_ENABLED'.

  Revision 1.43  2005/04/15 08:42:58  yosizaki
  fix assertion macros format. (remove ';' )

  Revision 1.42  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.41  2005/02/18 07:26:49  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.40  2004/11/04 03:56:04  yosizaki
  fix comment typo.
  add assertion in FS_InitFile().

  Revision 1.39  2004/11/02 10:06:03  yosizaki
  fix comment.

  Revision 1.38  2004/09/02 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.37  2004/07/23 08:30:06  yosizaki
  fix command operation.

  Revision 1.36  2004/07/19 13:18:24  yosizaki
  change FS_ReadFile/Async

  Revision 1.35  2004/07/08 13:41:28  yosizaki
  move some functions to fs_archive.c

  Revision 1.34  2004/06/30 04:32:20  yosizaki
  small fix.

  Revision 1.33  2004/06/03 13:33:07  yosizaki
  fix around archive asynchronous operation.

  Revision 1.32  2004/06/01 10:17:56  yosizaki
  change around assertion code.

  Revision 1.31  2004/05/31 02:55:57  yosizaki
  fix FS_LoadOverlayImage(). (add DC_InvalidateRange)
  change FSi_ASSERT_ARG() to FS_ASSERT_ARG().

  Revision 1.30  2004/05/21 10:55:27  yosizaki
  change OS_IsThreadAvailable() to SDK_NO_THREAD.

  Revision 1.29  2004/05/20 06:31:28  yosizaki
  small fix.

  Revision 1.28  2004/05/19 02:29:58  yosizaki
  add comment.

  Revision 1.27  2004/05/17 08:32:26  yosizaki
  add comment.
  add assertion of each interface function.

  Revision 1.26  2004/05/12 05:22:55  yosizaki
  prepare a option "SDK_ARM7FS".

  Revision 1.25  2004/05/11 04:30:42  yosizaki
  change format of FSDirPos and FSFileID.
  change archive system.
  move FS_LoadOverlay() to fs_rom.
  move FS_OpenFile(), FS_SeekDir(), ... to fs_archive.

  Revision 1.24  2004/05/10 06:27:35  yosizaki
  divided to fs_archive and fs_rom.
  add FS_ReadFileAsync, FS_WriteFile, FS_WriteFileAsync.

  Revision 1.23  2004/05/01 05:11:57  yosizaki
  change OS_WaitAnyInterrupt() -> OS_WaitAnyIrq()

  Revision 1.22  2004/05/01 03:26:05  yosizaki
  u32 FSFile::arc_id -> void *FSFile::arc.
  change PXI protocol.
  fix cartridge-access.

  Revision 1.21  2004/04/28 04:22:54  yosizaki
  fix FS_Init().

  Revision 1.20  2004/04/27 04:11:53  yosizaki
  move FSi_(Copy|Clear)Memory to mi/mi_memory.c as MI_Cpu(Copy|Fill)8.

  Revision 1.19  2004/04/26 11:06:07  yosizaki
  move MI_ReadCard to mi_card.c

  Revision 1.18  2004/04/23 07:03:39  yada
  CW バグへの対策

  Revision 1.17  2004/04/22 02:31:02  yosizaki
  FS_LoadTables() -> FS_TryLoadTable().
  add FS_LoadTable().
  add FS_GetTableSize().

  Revision 1.16  2004/04/21 09:42:22  yosizaki
  fix OS_UnlockCard to OS_UnLockCard.
  add true MI_ReadCard().

  Revision 1.15  2004/04/19 06:46:33  yosizaki
  small fix around TEG-ARM7.

  Revision 1.14  2004/04/19 00:30:23  yosizaki
  add __declspec(weak) to MI_ReadCard().

  Revision 1.13  2004/04/15 11:40:20  yosizaki
  support CARD-on-TEG. (but CARD access routine is extern)
  add FSi_ReadServer().
  fix FS_Init().
  other small fix.

  Revision 1.12  2004/04/15 00:05:46  yosizaki
  add FSi_LoadArchiveTables().
  add FS_LoadTables().
  add FS_GetDefaultDMA().

  Revision 1.11  2004/04/13 12:14:10  yosizaki
  fix header comment.
  add FS_IsAvailable.
  change FS_SetDefaultDMA.
  add archive selector system (internal).

  Revision 1.10  2004/04/08 11:09:40  yosizaki
  fix (change FSDir to FSFile)

  Revision 1.9  2004/04/08 10:45:40  yosizaki
  change FSFile to FSFile

  Revision 1.8  2004/04/07 00:24:19  yosizaki
  fix STRB-code.

  Revision 1.7  2004/04/06 11:59:30  yosizaki
  fix FS_SeekDir()

  Revision 1.6  2004/04/06 06:59:35  yosizaki
  change FS_Init()
  change around FSOverlayID

  Revision 1.5  2004/04/05 02:49:56  yosizaki
  change argtype of FSLoadOverlay()

  Revision 1.4  2004/04/02 12:19:24  yosizaki
  fix FS_LoadOverlay/FS_UnloadOverlay

  Revision 1.3  2004/04/02 10:38:23  yosizaki
  (none)

  Revision 1.2  2004/04/02 00:46:12  yosizaki
  allow path-name to be any length.
  fix ASSERT of the MAKEROM-check.

  Revision 1.1  2004/04/01 11:34:18  yosizaki
  (none)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>

#include <nitro/fs.h>

#include "../include/rom.h"
#include "../include/util.h"
#include "../include/command.h"


/*****************************************************************************/
/* constant */

/* エラーメッセージ */
#if !defined(SDK_FINALROM)

const char *fsi_assert_fs_format = "[file-system] %s.\n";
const char *fsi_assert_is_init = "not initialized";
const char *fsi_assert_is_file = "invalid file-handle";
const char *fsi_assert_is_dir = "invalid directory-handle";
const char *fsi_assert_is_handle = "neither file-handle nor directory-handle";
const char *fsi_assert_is_idle = "specified file-handle is busy";
const char *fsi_assert_is_empty = "specified file-handle is already opened";
const char *fsi_assert_is_valid_arg = "invalid argument parameter";
const char *fsi_assert_is_valid_dma_channel = "cannot specify DMA channel 0";
const char *fsi_assert_irq_enabled = "not IRQ-enabled";

const char *fsi_assert_arc_free = "archive is still now registered";
const char *fsi_assert_arc_unloaded = "archive is still now loaded";
const char *fsi_assert_arc_not_rom = "cannot modify \"rom\" archive";

#endif


/*****************************************************************************/
/* variable */

/* if FS_Init() called, True */
static BOOL is_init = FALSE;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FS_Init

  Description:  Initialize file-system

  Arguments:    default_dma_no   default DMA channel for FS
                                 if out of range(0-3),
                                 use CpuCopy instead of DMA.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_Init(u32 default_dma_no)
{
#if defined(FS_IMPLEMENT)
    FS_ASSERT_DMA_CHANNEL(default_dma_no, void);
    if (!is_init)
    {
        is_init = TRUE;
        /* setup archive system */
        /* set "rom" archive */
        FSi_InitRom(default_dma_no);
    }

#else
#pragma unused(default_dma_no)
    CARD_Init();
#endif /* FS_IMPLEMENT */
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsAvailable

  Description:  check if file-system is ready

  Arguments:    None.

  Returns:      If file-system is ready, True.
 *---------------------------------------------------------------------------*/
BOOL FS_IsAvailable(void)
{
    return is_init;
}

/*---------------------------------------------------------------------------*
  Name:         FS_End

  Description:  Finalize file-system.
                unload and unregister all the archives.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_End(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (is_init)
    {
#if defined(FS_IMPLEMENT)
        FSi_EndArchive();
        OS_ReleaseLockID((u16)fsi_card_lock_id);
#endif /* FS_IMPLEMENT */
        is_init = FALSE;
    }

    (void)OS_RestoreInterrupts(bak_psr);
}

#if defined(FS_IMPLEMENT)

/*---------------------------------------------------------------------------*
  Name:         FS_InitFile

  Description:  initialize FSFile object

  Arguments:    p_file      pointer to FSFile to be initialized

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_InitFile(FSFile *p_file)
{
    FS_ASSERT_ARG(p_file, void);
    p_file->link.next = p_file->link.prev = NULL;
#if !defined(SDK_NO_THREAD)
    OS_InitThreadQueue(p_file->queue);
#endif
    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat = 0;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FindPath

  Description:  内部関数. 以下の処理を一括して行う.
                1. 指定のパス名を絶対パスか相対パスか判定し,
                   (FSArchive*) + (FSDirPos) + ("path") の形に正規化する.
                2. FS_COMMAND_FINDPATH コマンドを送り, 結果を返す.

  Arguments:    p_dir       結果のディレクトリ情報を格納するポインタ
  Arguments:    path        正規化されていないパス文字列
                p_file_id   ファイルIDを求める場合に結果を格納するポインタ
                p_dir_pos   ディレクトリ位置を求める場合に結果を格納するポインタ

  Returns:      FINDPATH コマンドの結果値.
 *---------------------------------------------------------------------------*/
static BOOL FSi_FindPath(FSFile *p_dir, const char *path, FSFileID *p_file_id, FSDirPos *p_dir_pos)
{
    FSDirPos pos;

    FS_ASSERT_ARG(p_dir && path, FALSE);
    FS_ASSERT_IRQ_ENABLED(-1);

    if (FSi_IsSlash(MI_ReadByte(path)))
    {
        /* "/(path)" */
        pos.arc = current_dir_pos.arc;
        pos.own_id = 0;
        pos.pos = 0;
        pos.index = 0;
        ++path;
    }
    else
    {
        /* "(path)/(path)" または "(arc):/(path)" */
        int     i;
        /* 基本的には現在の位置からの相対パス */
        pos = current_dir_pos;
        for (i = 0; i <= FS_ARCHIVE_NAME_LEN_MAX; ++i)
        {
            u32     c = MI_ReadByte(path + i);
            if (!c || FSi_IsSlash(c))
                break;
            else if (c == ':')
            {
                /* "(arc):/(path)" */
                FSArchive *const p_arc = FS_FindArchive(path, i);
                if (!p_arc)
                {
                    /* 指定アーカイブが未登録 */
                    OS_Warning("[file-system] " "archive \"%*s\" is not found.", i, path);
                    return FALSE;
                }
                else if (!FS_IsArchiveLoaded(p_arc))
                {
                    /* 指定アーカイブが未ロード */
                    OS_Warning("[file-system] "
                               "archive \"%*s\" is registered, but not loaded yet.", i, path);
                    return FALSE;
                }
                /* "(path)/(path)" */
                pos.arc = p_arc;
                pos.pos = 0;
                pos.index = 0;
                pos.own_id = 0;
                path += i + 1;
                if (FSi_IsSlash(MI_ReadByte(path)))
                    ++path;
                break;
            }
        }
    }

    p_dir->arc = pos.arc;
    p_dir->arg.findpath.path = path;
    p_dir->arg.findpath.pos = pos;
    if (p_dir_pos)
    {
        p_dir->arg.findpath.find_directory = TRUE;
        p_dir->arg.findpath.result.dir = p_dir_pos;
    }
    else
    {
        p_dir->arg.findpath.find_directory = FALSE;
        p_dir->arg.findpath.result.file = p_file_id;
    }
    return FSi_SendCommand(p_dir, FS_COMMAND_FINDPATH);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadFileCore

  Description:  内部関数. ReadFile の共通処理

  Arguments:    p_file      データを読み出すファイルハンドル
  Arguments:    dst         読み出したデータを格納するバッファ
                len         読み出しサイズ
                async       非同期処理を指定するなら TRUE

  Returns:      結果として予想される論理的な読み出しサイズ.
 *---------------------------------------------------------------------------*/
static s32 FSi_ReadFileCore(FSFile *p_file, void *dst, s32 len, BOOL async)
{
    FS_ASSERT_INIT(-1);
    FS_ASSERT_ARG(p_file && dst && (len >= 0), -1);
    FS_ASSERT_FILE(p_file, -1);
    FS_ASSERT_IDLE(p_file, -1);

    {
        const s32 pos = (s32)p_file->prop.file.pos;
        const s32 rest = (s32)p_file->prop.file.bottom - pos;
        const u32 org = (u32)len;
        if (len > rest)
            len = rest;
        if (len < 0)
            len = 0;
        p_file->arg.readfile.dst = dst;
        p_file->arg.readfile.len_org = org;
        p_file->arg.readfile.len = (u32)(len);
        if (!async)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        (void)FSi_SendCommand(p_file, FS_COMMAND_READFILE);
        if (!async)
        {
            if (FS_WaitAsync(p_file))
                len = (s32)p_file->prop.file.pos - pos;
            else
                len = -1;
        }
    }
    return len;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadFileCore

  Description:  内部関数. WriteFile の共通処理

  Arguments:    p_file      データを書き込むファイルハンドル
  Arguments:    src         書き込んたデータを格納するバッファ
                len         書き込みサイズ
                async       非同期処理を指定するなら TRUE

  Returns:      結果として予想される論理的な書き込みサイズ.
 *---------------------------------------------------------------------------*/
static s32 FSi_WriteFileCore(FSFile *p_file, const void *src, s32 len, BOOL async)
{
    FS_ASSERT_INIT(-1);
    FS_ASSERT_ARG(p_file && src && (len >= 0), -1);
    FS_ASSERT_FILE(p_file, -1);
    FS_ASSERT_IDLE(p_file, -1);

    {
        const s32 pos = (s32)p_file->prop.file.pos;
        const s32 rest = (s32)p_file->prop.file.bottom - pos;
        const u32 org = (u32)len;
        if (len > rest)
            len = rest;
        if (len < 0)
            len = 0;
        p_file->arg.writefile.src = src;
        p_file->arg.writefile.len_org = org;
        p_file->arg.writefile.len = (u32)(len);
        if (!async)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        (void)FSi_SendCommand(p_file, FS_COMMAND_WRITEFILE);
        if (!async)
        {
            if (FS_WaitAsync(p_file))
                len = (s32)p_file->prop.file.pos - pos;
            else
                len = -1;
        }
    }

    return len;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ConvertPathToFileID

  Description:  get file id with path-name

  Arguments:    p_file_id   destination pointer to FSFileID
                path        relative or absolute path-name

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_ConvertPathToFileID(FSFileID *p_file_id, const char *path)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file_id && path, FALSE);
    FS_ASSERT_IRQ_ENABLED(FALSE);

    {
        FSFile  dir;
        FS_InitFile(&dir);
        if (!FSi_FindPath(&dir, path, p_file_id, NULL))
            return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileDirect

  Description:  open file directly by mapping information, without FAT.

  Arguments:    p_file       pointer to FSFile
                p_arc        mapped archive
                image_top    top position of file image 
                image_bottom bottom position of file image (last + 1)
                file_index   file index

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileDirect(FSFile *p_file, FSArchive *p_arc,
                       u32 image_top, u32 image_bottom, u32 file_index)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file && p_arc, FALSE);
    FS_ASSERT_EMPTY(p_file, FALSE);
    FS_ASSERT_IRQ_ENABLED(FALSE);

    {
        p_file->arc = p_arc;
        p_file->arg.openfiledirect.index = file_index;
        p_file->arg.openfiledirect.top = image_top;
        p_file->arg.openfiledirect.bottom = image_bottom;
        if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEDIRECT))
            return FALSE;
        p_file->stat |= FS_FILE_STATUS_IS_FILE;
        p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileFast

  Description:  Open file with file id.

  Arguments:    p_file      FSFileID to be opened
                file_id     file id (by FS_ReadDir)

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileFast(FSFile *p_file, FSFileID file_id)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file, FALSE);
    FS_ASSERT_EMPTY(p_file, FALSE);
    FS_ASSERT_IRQ_ENABLED(FALSE);

    {
        if (!file_id.arc)
            return FALSE;
        p_file->arc = file_id.arc;
        p_file->arg.openfilefast.id = file_id;
        if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEFAST))
            return FALSE;
        p_file->stat |= FS_FILE_STATUS_IS_FILE;
        p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFile

  Description:  Open file with path-name

  Arguments:    p_file      pointer to FSFile to be opened
                path        relative or absolute path-name

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFile(FSFile *p_file, const char *path)
{
    FSFileID id;
    return (FS_ConvertPathToFileID(&id, path) && FS_OpenFileFast(p_file, id));
}

/*---------------------------------------------------------------------------*
  Name:         FS_CloseFile

  Description:  Close FSFile object

  Arguments:    p_file      pointer to FSFile to be closed

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_CloseFile(FSFile *p_file)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file, FALSE);
    FS_ASSERT_FILE(p_file, FALSE);
    FS_ASSERT_IRQ_ENABLED(FALSE);

    if (!FSi_SendCommand(p_file, FS_COMMAND_CLOSEFILE))
        return FALSE;
    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathName

  Description:  get full path name of file or diirectory.

  Arguments:    p_file      FSFileID of file or directory
                buf         destination buffer
                len         max length of buf

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_GetPathName(FSFile *p_file, char *buf, u32 len)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_HANDLE(p_file, FALSE);
    FS_ASSERT_IRQ_ENABLED(FALSE);

    /*
     * if previous command is not FS_COMMAND_GETPATH, we initialize fields.
     * else, we reuse fields instead of expensive and constant operation.
     */
    if (p_file->command != FS_COMMAND_GETPATH)
    {
        p_file->arg.getpath.total_len = 0;
        p_file->arg.getpath.dir_id = 0;
    }
    p_file->arg.getpath.buf = (u8 *)buf;
    p_file->arg.getpath.buf_len = len;
    return FSi_SendCommand(p_file, FS_COMMAND_GETPATH);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathLength

  Description:  get full path length of file or directory
                it includes '\0'

  Arguments:    p_file      FSFileID of file or directory

  Returns:      If succeeded, path length. If failed, -1.
 *---------------------------------------------------------------------------*/
s32 FS_GetPathLength(FSFile *p_file)
{
    return FS_GetPathName(p_file, NULL, 0) ? p_file->arg.getpath.total_len : -1;
}

/*---------------------------------------------------------------------------*
  Name:         FS_WaitAsync

  Description:  wait for end of asynchronous function and return result.

  Arguments:    file to wait

  Returns:      if succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_WaitAsync(FSFile *p_file)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file, FALSE);
    FS_ASSERT_IRQ_ENABLED(-1);

#if !defined(SDK_NO_THREAD)
    {
        BOOL    is_owner = FALSE;
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (FS_IsBusy(p_file))
        {
            /* いまだ未処理の非同期モードならここで処理を引き受ける */
            is_owner = !(p_file->stat & (FS_FILE_STATUS_SYNC | FS_FILE_STATUS_OPERATING));
            if (is_owner)
            {
                p_file->stat |= FS_FILE_STATUS_SYNC;
                do
                {
                    OS_SleepThread(p_file->queue);
                }
                while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
            }
            /* そうでないなら単に処理の完了を待つ */
            else
            {
                do
                {
                    OS_SleepThread(p_file->queue);
                }
                while (FS_IsBusy(p_file));
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
        /* 引き受けた処理をここで実行する */
        if (is_owner)
        {
            return FSi_ExecuteSyncCommand(p_file);
        }
    }
#else
    while (FS_IsBusy(p_file))
        ;
#endif

    return FS_IsSucceeded(p_file);
}

/*---------------------------------------------------------------------------*
  Name:         FS_CancelFile

  Description:  send request to cancel asynchronous function

  Arguments:    file to cancel

  Returns:      none.
 *---------------------------------------------------------------------------*/
void FS_CancelFile(FSFile *p_file)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file, FALSE);

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (FS_IsBusy(p_file))
        {
            p_file->stat |= FS_FILE_STATUS_CANCEL;
            p_file->arc->flag |= FS_ARCHIVE_FLAG_CANCELING;
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFileAsync

  Description:  Read data from FSFile and return without no blocking

  Arguments:    p_file      pointer to FSFile
                dst         pointer to destination buffer
                len         read size

  Returns:      If succeeded, read size. If failed, -1.
 *---------------------------------------------------------------------------*/
s32 FS_ReadFileAsync(FSFile *p_file, void *dst, s32 len)
{
    return FSi_ReadFileCore(p_file, dst, len, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFile

  Description:  Read data from FSFile

  Arguments:    p_file      pointer to FSFile
                dst         pointer to destination buffer
                len         read size

  Returns:      If succeeded, read size. If failed, -1.
 *---------------------------------------------------------------------------*/
s32 FS_ReadFile(FSFile *p_file, void *dst, s32 len)
{
    FS_ASSERT_IRQ_ENABLED(-1);
    return FSi_ReadFileCore(p_file, dst, len, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFileAsync

  Description:  Write data to FSFile and return without no blocking

  Arguments:    p_file      pointer to FSFile
                src         pointer to source buffer
                len         write size

  Returns:      If succeeded, written size. If failed, -1.
 *---------------------------------------------------------------------------*/
s32 FS_WriteFileAsync(FSFile *p_file, const void *src, s32 len)
{
    return FSi_WriteFileCore(p_file, src, len, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFile

  Description:  Write data to FSFile

  Arguments:    p_file      pointer to FSFile
                src         pointer to source buffer
                len         write size

  Returns:      If succeeded, written size. If failed, -1.
 *---------------------------------------------------------------------------*/
s32 FS_WriteFile(FSFile *p_file, const void *src, s32 len)
{
    FS_ASSERT_IRQ_ENABLED(-1);
    return FSi_WriteFileCore(p_file, src, len, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FS_SeekFile

  Description:  move seek-pointer of FSFile

  Arguments:    p_file      pointer to FSFile
                offset      move offset
                origin      base position for offset

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_SeekFile(FSFile *p_file, s32 offset, FSSeekFileMode origin)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_file, FALSE);
    FS_ASSERT_FILE(p_file, FALSE);

    {
        switch (origin)
        {
        case FS_SEEK_SET:
            offset += p_file->prop.file.top;
            break;
        case FS_SEEK_CUR:
            offset += p_file->prop.file.pos;
            break;
        case FS_SEEK_END:
            offset += p_file->prop.file.bottom;
            break;
        default:
            FS_ASSERT_ARG(FALSE, FALSE);
            return FALSE;
        }
        if (offset < (s32)p_file->prop.file.top)
            offset = (s32)p_file->prop.file.top;
        if (offset > (s32)p_file->prop.file.bottom)
            offset = (s32)p_file->prop.file.bottom;
        p_file->prop.file.pos = (u32)offset;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SeekDir

  Description:  set FSFile to specified directory position

  Arguments:    p_dir       pointer to FSFile
                p_pos       directory position(by FS_ReadDir, FS_TellDir)

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_SeekDir(FSFile *p_dir, const FSDirPos *p_pos)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_dir && p_pos->arc && p_pos, FALSE);
    FS_ASSERT_IRQ_ENABLED(-1);

    p_dir->arc = p_pos->arc;
    p_dir->arg.seekdir.pos = *p_pos;
    if (!FSi_SendCommand(p_dir, FS_COMMAND_SEEKDIR))
        return FALSE;
    p_dir->stat |= FS_FILE_STATUS_IS_DIR;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDir

  Description:  read 1 FSDirEntry information from directory and step to next

  Arguments:    p_dir       pointer to FSFile
                p_entry     pointer to destination FSDirEntry

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_ReadDir(FSFile *p_dir, FSDirEntry *p_entry)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_dir && p_entry, FALSE);
    FS_ASSERT_DIR(p_dir, FALSE);
    FS_ASSERT_IRQ_ENABLED(-1);

    p_dir->arg.readdir.p_entry = p_entry;
    p_dir->arg.readdir.skip_string = FALSE;
    return FSi_SendCommand(p_dir, FS_COMMAND_READDIR);
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindDir

  Description:  find directory and seek there

  Arguments:    p_dir       pointer to FSFile
                path        relative or absolute directory-name

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_FindDir(FSFile *p_dir, const char *path)
{
    FSDirPos pos;
    if (!FSi_FindPath(p_dir, path, NULL, &pos))
        return FALSE;
    return FS_SeekDir(p_dir, &pos);
}

/*---------------------------------------------------------------------------*
  Name:         FS_ChangeDir

  Description:  change current directory

  Arguments:    path        relative or absolute directory-name

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_ChangeDir(const char *path)
{
    FSDirPos pos;
    FSFile  dir;
    FS_InitFile(&dir);
    if (!FSi_FindPath(&dir, path, NULL, &pos))
        return FALSE;
    current_dir_pos = pos;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_TellDir

  Description:  get directory position

  Arguments:    p_dir       pointer to FSFile
                p_pos       pointer to destination FSDirPos

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_TellDir(const FSFile *p_dir, FSDirPos *p_pos)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_dir && p_pos, FALSE);
    FS_ASSERT_DIR(p_dir, FALSE);

    {
        *p_pos = p_dir->prop.dir.pos;
        return TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_RewindDir

  Description:  set FSFile to starting position

  Arguments:    p_dir       pointer to FSFile

  Returns:      If succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_RewindDir(FSFile *p_dir)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_dir, FALSE);
    FS_ASSERT_DIR(p_dir, FALSE);
    FS_ASSERT_IRQ_ENABLED(-1);

    {
        FSDirPos pos;
        pos.arc = p_dir->arc;
        pos.own_id = p_dir->prop.dir.pos.own_id;
        pos.pos = 0;
        pos.index = 0;
        return FS_SeekDir(p_dir, &pos);
    }
}


#endif /* FS_IMPLEMENT */
