/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_command.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "../include/util.h"
#include "../include/command.h"


/* アーカイブは基本的に ARM7 に含まない */
#if defined(FS_IMPLEMENT)


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseCommand

  Description:  内部関数.
                コマンドを完了し待機スレッドがあれば復帰する.

  Arguments:    p_file           完了したコマンドを格納する FSFile 構造体.
                ret              コマンドの結果値.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ReleaseCommand(FSFile *p_file, FSResult ret)
{
    SDK_ASSERT(FS_IsBusy(p_file));
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        FSi_CutFromList(p_file);
        p_file->stat &= ~(FS_FILE_STATUS_CANCEL | FS_FILE_STATUS_BUSY |
                          FS_FILE_STATUS_SYNC | FS_FILE_STATUS_ASYNC | FS_FILE_STATUS_OPERATING);
        p_file->error = ret;
        OS_WakeupThread(p_file->queue);
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_TranslateCommand

  Description:  内部関数.
                ユーザプロシージャまたはデフォルト処理を呼び出し, 結果を返す.
                同期系コマンドが非同期的応答を返した場合は内部で完了を待つ.
                非同期系コマンドが非同期的応答を返した場合はそのまま返す.

  Arguments:    p_file           実行するコマンドを格納した FSFile 構造体.
                command          コマンド ID.

  Returns:      コマンドの処理結果.
 *---------------------------------------------------------------------------*/
FSResult FSi_TranslateCommand(FSFile *p_file, FSCommandType command)
{
    FSResult ret;

    FSArchive *const p_arc = p_file->arc;
    const int bit = (1 << command);

    /* 同期 / 非同期処理のフラグを立てる */
    if (FS_IsFileSyncMode(p_file))
        p_arc->flag |= FS_ARCHIVE_FLAG_IS_SYNC;
    else
        p_arc->flag |= FS_ARCHIVE_FLAG_IS_ASYNC;

    /* プロシージャが対応していれば呼び出して応答を確認する */
    if ((p_arc->proc_flag & bit) != 0)
    {
        switch (ret = (*p_arc->proc) (p_file, command))
        {
        case FS_RESULT_SUCCESS:
        case FS_RESULT_FAILURE:
        case FS_RESULT_UNSUPPORTED:
            /* プロシージャ側でコマンドを処理完了 */
            p_file->error = ret;
            break;
        case FS_RESULT_PROC_ASYNC:
            /* 非同期処理の扱いは後で細かく制御 */
            break;
        case FS_RESULT_PROC_UNKNOWN:
            /* 未知のコマンドなので今回以降デフォルトに切り替える */
            ret = FS_RESULT_PROC_DEFAULT;
            p_arc->proc_flag &= ~bit;
            break;
        }
    }
    else
    {
        ret = FS_RESULT_PROC_DEFAULT;
    }
    /* 必要ならデフォルトの処理を呼び出す */
    if (ret == FS_RESULT_PROC_DEFAULT)
    {
        ret = (*fsi_default_command[command]) (p_file);
    }
    /* 非同期処理に入った場合, 同期モードならブロッキング */
    if (ret == FS_RESULT_PROC_ASYNC)
    {
        if (FS_IsFileSyncMode(p_file))
        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while (FSi_IsArchiveSync(p_arc))
                OS_SleepThread(&p_arc->sync_q);
            ret = p_file->error;
            (void)OS_RestoreInterrupts(bak_psr);
        }
    }
    /* 非同期モードで同期完了した場合はここで解放 */
    else if (!FS_IsFileSyncMode(p_file))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
        FSi_ReleaseCommand(p_file, ret);
    }
    /* 同期系コマンドが同期完了した場合はそのまま結果を格納 */
    else
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
        p_file->error = ret;
    }
    /* 結果を返す. (非同期系の非同期処理のみ FS_RESULT_PROC_ASYNC) */
    return ret;
}


#endif /* FS_IMPLEMENT */

/*---------------------------------------------------------------------------*
  $Log: fs_command.c,v $
  Revision 1.14  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.13  2005/06/17 13:02:55  yosizaki
  fix FS_WaitAsync. (add FS_FILE_STATUS_OPERATING)

  Revision 1.12  2005/05/30 04:18:26  yosizaki
  move some functions to fs_command_default.c.
  add comments.

  Revision 1.11  2005/02/28 05:26:13  yosizaki
  do-indent.

  Revision 1.10  2005/02/18 07:26:49  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.9  2004/10/22 13:06:52  yosizaki

  Revision 1.8  2004/09/22 08:55:59  yosizaki
  fix FSi_SeekDirDirect().

  Revision 1.7  2004/09/03 06:54:20  yosizaki
  small fix.

  Revision 1.6  2004/09/02 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.5  2004/08/05 03:26:28  yosizaki
  fix FS_COMMAND_READDIR (numbering of file-ID)

  Revision 1.4  2004/07/23 08:30:06  yosizaki
  fix command operation.

  Revision 1.3  2004/07/19 13:18:03  yosizaki
  change system (sync & async)

  Revision 1.2  2004/07/12 13:04:00  yosizaki
  small fix (FS_COMMAND_GETPATH)

  Revision 1.1  2004/07/08 13:41:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
