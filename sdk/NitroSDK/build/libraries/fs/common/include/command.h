/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     command.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_COMMAND_H_)
#define NITRO_FS_COMMAND_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* constant */

extern FSResult (*const (fsi_default_command[])) (FSFile *);


/*****************************************************************************/
/* variable */

extern FSDirPos current_dir_pos;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         FSi_StrNICmp

  Description:  内部関数.
                大小文字を区別せずに指定バイト数だけ文字列比較.

  Arguments:    str1             比較元文字列.
                str2             比較先文字列.
                len              比較バイト数.

  Returns:      (str1 - str2) の比較結果.
 *---------------------------------------------------------------------------*/
int     FSi_StrNICmp(const char *str1, const char *str2, u32 len);

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  内部関数.
                アーカイブへコマンドを発行する.
                起動タイミングの調整とともに, 同期系ならここでブロッキング.

  Arguments:    p_file           コマンド引数を指定された FSFile 構造体.
                command          コマンド ID.

  Returns:      コマンドが成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_SendCommand(FSFile *p_file, FSCommandType command);

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
FSResult FSi_TranslateCommand(FSFile *p_file, FSCommandType command);

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseCommand

  Description:  内部関数.
                コマンドを完了し待機スレッドがあれば復帰する.

  Arguments:    p_file           完了したコマンドを格納する FSFile 構造体.
                ret              コマンドの結果値.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_ReleaseCommand(FSFile *p_file, FSResult ret);

/*---------------------------------------------------------------------------*
  Name:         FSi_NextCommand

  Description:  内部関数.
                次に処理すべきコマンドを選択する.
                非同期コマンドが選択されたらそのポインタを返す.
                NULL 以外が返された場合はその場で処理が必要.

  Arguments:    p_arc            次のコマンドを取得するアーカイブ.

  Returns:      この場で処理を必要とする次のコマンド.
 *---------------------------------------------------------------------------*/
FSFile *FSi_NextCommand(FSArchive *p_arc);

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteAsyncCommand

  Description:  内部関数.
                非同期系コマンドの実行.
                最初の 1 回はユーザスレッドから割り込み許可で呼ばれる.
                アーカイブが同期的に動作する限りここでコマンド処理を繰り返し,
                1 回でも非同期処理になれば続きは NotifyAsyncEnd() で行う.

                よって, アーカイブ処理が同期 / 非同期で切り替わる場合は
                NotifyAsyncEnd() の呼び出し環境に注意する必要がある.

  Arguments:    p_file           実行する非同期コマンドを格納した FSFile 構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_ExecuteAsyncCommand(FSFile *p_file);

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteSyncCommand

  Description:  内部関数.
                同期モードにおけるコマンド実行.

  Arguments:    p_file           実行する同期コマンドを格納した FSFile 構造体.

  Returns:      コマンドが成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_ExecuteSyncCommand(FSFile *p_file);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_COMMAND_H_ */

/*---------------------------------------------------------------------------*
  $Log: command.h,v $
  Revision 1.6  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/05/30 04:13:26  yosizaki
  add comments.

  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.3  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.2  2004/07/23 08:30:26  yosizaki
  fix command operation.

  Revision 1.1  2004/07/08 13:40:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
