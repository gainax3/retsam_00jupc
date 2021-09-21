/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     backup.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: backup.h,v $
  Revision 1.17  2007/11/13 04:20:24  yosizaki
  support erase-SUBSECTOR.

  Revision 1.16  2007/10/04 13:08:51  yosizaki
  add test commands.

  Revision 1.15  2007/02/20 00:28:12  kitase_hirotake
  indent source

  Revision 1.14  2006/05/25 02:51:15  yosizaki
  add erase-chip, write-sector.

  Revision 1.13  2006/04/10 04:12:42  yosizaki
  beautify sourcecode indent.

  Revision 1.12  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  2005/09/02 07:35:02  yosizaki
  small fix.

  Revision 1.10  2005/09/02 07:14:55  yosizaki
  add CARD_CancelBackupAsync().

  Revision 1.9  2005/05/12 11:21:18  yosizaki
  add some internal parameters.

  Revision 1.8  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.7  2005/02/28 05:26:01  yosizaki
  do-indent.

  Revision 1.6  2004/12/08 06:46:28  yosizaki
  add comments.

  Revision 1.5  2004/09/06 13:44:31  yosizaki
  move CARD_(Lock/Unlock)Backup

  Revision 1.4  2004/08/23 10:39:07  yosizaki
  add write-command.
  delete erase-command. (FLASH)

  Revision 1.3  2004/08/18 13:40:49  yosizaki
  add FLASH 2Mbit.

  Revision 1.2  2004/07/28 13:18:29  yosizaki
  asynchronous mode support.

  Revision 1.1  2004/07/23 08:32:18  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_BACKUP_H_)
#define NITRO_CARD_BACKUP_H_


#include <nitro/types.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*
  Name:         CARD_LockBackup

  Description:  lock the CARD-bus before backup operation.

  Arguments:    lock id.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_LockBackup(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockBackup

  Description:  unlock the CARD-bus after backup operation.

  Arguments:    lock id which is used by CARD_LockBackup().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_UnlockBackup(u16 lock_id);


/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  check if asynchronous backup operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARD_TryWaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  wait for completion of current asynchronous backup operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARD_WaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  request to cancel current asynchronous backup operation.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_CancelBackupAsync(void);

/* internal chip command as Serial-Bus-Interface */


/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommand

  Description:  データを転送するコマンドのリクエストを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE
                req_type   コマンドリクエストタイプ
                req_retry  コマンドリクエストの失敗時リトライ最大回数
                req_mode   コマンドリクエストの動作モード

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async,
                                   CARDRequest req_type, int req_retry, CARDRequestMode req_mode);

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestWriteSectorCommand

  Description:  セクタ消去 + プログラムのリクエストを発行.

  Arguments:    src        転送元メモリアドレス
                dst        転送先オフセット
                len        転送サイズ
                verify     ベリファイを行う場合は TRUE
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_RequestWriteSectorCommand(u32 src, u32 dst, u32 len, BOOL verify,
                                        MIDmaCallback callback, void *arg, BOOL is_async);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackup

  Description:  チップコマンド "read" を発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ReadBackup(u32 src, void *dst, u32 len,
                                 MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len,
                                      callback, arg, is_async,
                                      CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackup

  Description:  チップコマンド "program" を使用する読み込みコマンドを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramBackup(u32 dst, const void *src, u32 len,
                                    MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackup

  Description:  チップコマンド "write" をを使用する書き込みコマンドを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteBackup(u32 dst, const void *src, u32 len,
                                  MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackup

  Description:  チップコマンド "read" を使用するベリファイコマンドを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_VerifyBackup(u32 dst, const void *src, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_VERIFY_BACKUP, 1, CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramAndVerifyBackup

  Description:  チップコマンド "program" を使用するプログラム + ベリファイコマンドを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len,
                                             MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteAndVerifyBackup

  Description:  チップコマンド "write" を使用するプログラム + ベリファイコマンドを発行.

  Arguments:    src        転送元オフセットまたはメモリアドレス
                dst        転送先オフセットまたはメモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteAndVerifyBackup(u32 dst, const void *src, u32 len,
                                           MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSector

  Description:  チップコマンド "erase sector" を使用するセクタ消去コマンドを発行.

  Arguments:    dst        消去先オフセット
                len        消去サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupSector(u32 dst, u32 len,
                                        MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSector

  Description:  チップコマンド "erase subsector" を使用するセクタ消去コマンドを発行.

  Arguments:    dst        消去先オフセット
                len        消去サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupSubSector(u32 dst, u32 len,
                                           MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SUBSECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupChip

  Description:  チップコマンド "erase chip" を使用するセクタ消去コマンドを発行.

  Arguments:    callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)
                is_async   非同期操作を指定されていれば TRUE

  Returns:      処理が成功したら TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupChip(MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, 0, 0, callback, arg, is_async,
                                      CARD_REQ_ERASE_CHIP_BACKUP, 1, CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackupAsync

  Description:  非同期バックアップリード (チップコマンド "read" に相当)

  Arguments:    src        転送元オフセット
                dst        転送先メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadBackupAsync(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_ReadBackup(src, dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackup

  Description:  同期バックアップリード (チップコマンド "read" に相当)

  Arguments:    src        転送元オフセット
                dst        転送先メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ReadBackup(u32 src, void *dst, u32 len)
{
    return CARDi_ReadBackup(src, dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackupAsync

  Description:  非同期バックアッププログラム (チップコマンド "program" に相当)

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramBackupAsync(u32 dst, const void *src, u32 len,
                                        MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackup

  Description:  同期バックアッププログラム (チップコマンド "program" に相当)

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupAsync

  Description:  非同期バックアップライト (チップコマンド "write" に相当)

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteBackupAsync(u32 dst, const void *src, u32 len,
                                      MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackup

  Description:  同期バックアップライト (チップコマンド "write" に相当)

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackupAsync

  Description:  非同期バックアップベリファイ (チップコマンド "read" に相当)

  Arguments:    src        比較元オフセット
                dst        比較先メモリアドレス
                len        比較サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_VerifyBackupAsync(u32 dst, const void *src, u32 len,
                                       MIDmaCallback callback, void *arg)
{
    (void)CARDi_VerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackup

  Description:  同期バックアップベリファイ (チップコマンド "read" に相当)

  Arguments:    src        比較元オフセット
                dst        比較先メモリアドレス
                len        比較サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_VerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_VerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackupAsync

  Description:  非同期バックアッププログラム + ベリファイ

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                                 MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackup

  Description:  同期バックアッププログラム + ベリファイ

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupAsync

  Description:  非同期バックアップライト + ベリファイ

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                               MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackup

  Description:  同期バックアップライト + ベリファイ

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSectorAsync

  Description:  非同期セクタ消去.

  Arguments:    dst        消去先オフセット
                len        消去サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupSectorAsync(u32 dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupSector(dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSector

  Description:  同期セクタ消去.

  Arguments:    dst        消去先オフセット
                len        消去サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupSector(u32 dst, u32 len)
{
    return CARDi_EraseBackupSector(dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSubSectorAsync

  Description:  非同期サブセクタ消去.

  Arguments:    dst        消去先オフセット
                len        消去サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupSubSectorAsync(u32 dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupSubSector(dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSubSector

  Description:  同期サブセクタ消去.

  Arguments:    dst        消去先オフセット
                len        消去サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupSubSector(u32 dst, u32 len)
{
    return CARDi_EraseBackupSubSector(dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupChipAsync

  Description:  非同期チップ消去.

  Arguments:    callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupChipAsync(MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupChip(callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupChip

  Description:  同期セクタ消去.

  Arguments:    None.

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupChip(void)
{
    return CARDi_EraseBackupChip(NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupSectorAsync

  Description:  セクタ単位消去 + セクタ単位プログラム.

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteBackupSectorAsync(u32 dst, const void *src, u32 len,
                                            MIDmaCallback callback, void *arg)
{
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupSector

  Description:  セクタ単位消去 + セクタ単位プログラム.

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteBackupSector(u32 dst, const void *src, u32 len)
{
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupSectorAsync

  Description:  セクタ単位消去 + セクタ単位プログラム + ベリファイ.

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ
                callback   完了コールバック (不使用なら NULL)
                arg        完了コールバックの引数 (不使用なら無視される)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyBackupSectorAsync(u32 dst, const void *src, u32 len,
                                                     MIDmaCallback callback, void *arg)
{
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupSector

  Description:  セクタ単位消去 + セクタ単位プログラム + ベリファイ.

  Arguments:    dst        転送先オフセット
                src        転送元メモリアドレス
                len        転送サイズ

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyBackupSector(u32 dst, const void *src, u32 len)
{
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_AccessStatus

  Description:  ステータスリードまたはライト (テスト用)

  Arguments:    command    CARD_REQ_READ_STATUSまたはCARD_REQ_WRITE_STATUS
                value      CARD_REQ_WRITE_STATUSであれば書き込む値

  Returns:      成功すれば0以上の値、失敗すれば負の値
 *---------------------------------------------------------------------------*/
int CARDi_AccessStatus(CARDRequest command, u8 value);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadStatus

  Description:  ステータスリード (テスト用)

  Arguments:    None.

  Returns:      成功すれば0以上の値、失敗すれば負の値
 *---------------------------------------------------------------------------*/
SDK_INLINE int CARDi_ReadStatus(void)
{
    return CARDi_AccessStatus(CARD_REQ_READ_STATUS, 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteStatus

  Description:  ステータスライト (テスト用)

  Arguments:    value      書き込む値

  Returns:      CARD_RESULT_SUCCESS で完了すれば TRUE, それ以外は FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteStatus(u8 value)
{
    return (CARDi_AccessStatus(CARD_REQ_WRITE_STATUS, value) >= 0);
}


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_CARD_BACKUP_H_ */
