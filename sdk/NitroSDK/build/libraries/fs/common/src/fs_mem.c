/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_mem.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/util.h"


#if defined(FS_IMPLEMENT)

/*****************************************************************************/
/* variable */

/* メモリマップドファイルアーカイブ */
static FSArchive fsi_arc_mem;
static BOOL fsi_mem_init;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FSi_MemArchiveProc

  Description:  メモリマップドファイルアーカイブのプロシージャ

  Arguments:    p_file           コマンド情報を格納した FSFile 構造体
                cmd              コマンドタイプ

  Returns:      コマンド処理結果.
 *---------------------------------------------------------------------------*/
static FSResult FSi_MemArchiveProc(FSFile *p_file, FSCommandType cmd)
{
#pragma unused(p_file)

    switch (cmd)
    {

    case FS_COMMAND_SEEKDIR:
    case FS_COMMAND_READDIR:
    case FS_COMMAND_FINDPATH:
    case FS_COMMAND_GETPATH:
    case FS_COMMAND_OPENFILEFAST:
        return FS_RESULT_UNSUPPORTED;

    default:
        return FS_RESULT_PROC_UNKNOWN;

    }

}

/*---------------------------------------------------------------------------*
  Name:         FSi_InitMem

  Description:  メモリマップドファイルアーカイブの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_InitMem(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    if (!fsi_mem_init || !FS_IsArchiveLoaded(&fsi_arc_mem))
    {
        FS_InitArchive(&fsi_arc_mem);
        FS_SetArchiveProc(&fsi_arc_mem, FSi_MemArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);
        /* FAT は FNT は無い */
        if (!FS_LoadArchive(&fsi_arc_mem, 0, 0, 0, 0, 0, NULL, NULL))
        {
            OS_TPanic("failed to load memory-mapping archive!");
        }
        fsi_mem_init = TRUE;
    }

    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileFromMemory

  Description:  メモリ領域をマップしたファイルを一時的に生成.

  Arguments:    p_file           ファイルハンドルを格納する FSFile 構造体
                buf              READ および WRITE の対象となるメモリ
                size             buf のバイトサイズ

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFileFromMemory(FSFile *p_file, void *buf, u32 size)
{
    FSi_InitMem();
    return FS_OpenFileDirect(p_file, &fsi_arc_mem, (u32)buf, (u32)buf + size, 0);
}


#endif /* FS_IMPLEMENT */

/*---------------------------------------------------------------------------*
  $Log: fs_mem.c,v $
  Revision 1.2  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/28 02:11:11  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
