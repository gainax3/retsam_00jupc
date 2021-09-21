/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_backup.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>


#include "../include/card_spi.h"
#include "../include/card_common.h"


/*****************************************************************************/
/* function */

#if defined(SDK_ARM9)

/*
 * バックアップ使用アプリケーションの検出用処理.
 * 意図の有無を問わずバックアップを使用した場合は必ずこの文字列を含む.
 */
#include <nitro/version_begin.h>
SDK_DEFINE_MIDDLEWARE(cardi_backup_assert, "NINTENDO", "BACKUP");
#include <nitro/version_end.h>
#define SDK_USING_BACKUP() SDK_USING_MIDDLEWARE(cardi_backup_assert)


/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommandCore

  Description:  データを転送するコマンドリクエストの処理本体.
                同期的あるいは非同期的に呼び出される.

  Arguments:    p          ライブラリのワークバッファ (効率のために引数渡し)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_RequestStreamCommandCore(CARDiCommon * p)
{
    const int req_type = p->req_type;
    const int req_mode = p->req_mode;
    const int retry_count = p->req_retry;
    u32     size = sizeof(p->backup_cache_page_buf);

    SDK_USING_BACKUP();

    /* ページまたはセクタ単位でリクエスト */
    if (req_type == CARD_REQ_ERASE_SECTOR_BACKUP)
    {
        size = CARD_GetBackupSectorSize();
    }
    else if (req_type == CARD_REQ_ERASE_SUBSECTOR_BACKUP)
    {
        size = cardi_common.cmd->spec.subsect_size;
    }
    do
    {
        const u32 len = (size < p->len) ? size : p->len;
        p->cmd->len = len;

        /* キャンセル要求があればここで中止 */
        if ((p->flag & CARD_STAT_CANCEL) != 0)
        {
            p->flag &= ~CARD_STAT_CANCEL;
            p->cmd->result = CARD_RESULT_CANCELED;
            break;
        }
        switch (req_mode)
        {
        case CARD_REQUEST_MODE_RECV:
            /* 受信系コマンドならバッファを無効化 */
            DC_InvalidateRange(p->backup_cache_page_buf, len);
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)p->backup_cache_page_buf;
            break;
        case CARD_REQUEST_MODE_SEND:
        case CARD_REQUEST_MODE_SEND_VERIFY:
            /* 送信系コマンドならデータをテンポラリバッファへコピー */
            MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, len);
            DC_FlushRange(p->backup_cache_page_buf, len);
            DC_WaitWriteBufferEmpty();
            p->cmd->src = (u32)p->backup_cache_page_buf;
            p->cmd->dst = (u32)p->dst;
            break;
        case CARD_REQUEST_MODE_SPECIAL:
            /* バッファ操作は不要 */
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)p->dst;
            break;
        }
        /* リクエスト送信 */
        if (!CARDi_Request(p, req_type, retry_count))
        {
            break;
        }
        /* 指定があればさらに同じ設定でベリファイ要求 */
        if (req_mode == CARD_REQUEST_MODE_SEND_VERIFY)
        {
            if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
                break;
        }
        else if (req_mode == CARD_REQUEST_MODE_RECV)
        {
            /* キャッシュからコピー */
            MI_CpuCopy8(p->backup_cache_page_buf, (void *)p->dst, len);
        }
        p->src += len;
        p->dst += len;
        p->len -= len;
    }
    while (p->len > 0);
    CARDi_EndTask(p, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestWriteSectorCommandCore

  Description:  セクタ消去 + プログラムのリクエストの処理本体.
                同期的あるいは非同期的に呼び出される.

  Arguments:    p          ライブラリのワークバッファ (効率のために引数渡し)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_RequestWriteSectorCommandCore(CARDiCommon * p)
{
    const u32 sector_size = CARD_GetBackupSectorSize();
    SDK_USING_BACKUP();

    /* 処理範囲がセクタ単位の整数倍でなければ失敗を返す */
    if ((((u32)p->dst | p->len) & (sector_size - 1)) != 0)
    {
        p->flag &= ~CARD_STAT_CANCEL;
        p->cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else
    {
        /* セクタ単位処理 */
        for (; p->len > 0; p->len -= sector_size)
        {
            u32     len = sector_size;
            /* キャンセル要求があればここで中止 */
            if ((p->flag & CARD_STAT_CANCEL) != 0)
            {
                p->flag &= ~CARD_STAT_CANCEL;
                p->cmd->result = CARD_RESULT_CANCELED;
                break;
            }
            /* セクタ消去 */
            p->cmd->dst = (u32)p->dst;
            p->cmd->len = len;
            if (!CARDi_Request(p, CARD_REQ_ERASE_SECTOR_BACKUP, 1))
            {
                break;
            }
            while (len > 0)
            {
                const u32 page = sizeof(p->backup_cache_page_buf);
                /* キャンセル要求があればここで中止 */
                if ((p->flag & CARD_STAT_CANCEL) != 0)
                {
                    p->flag &= ~CARD_STAT_CANCEL;
                    p->cmd->result = CARD_RESULT_CANCELED;
                    break;
                }
                /* プログラム */
                MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, page);
                DC_FlushRange(p->backup_cache_page_buf, page);
                DC_WaitWriteBufferEmpty();
                p->cmd->src = (u32)p->backup_cache_page_buf;
                p->cmd->dst = (u32)p->dst;
                p->cmd->len = page;
                if (!CARDi_Request(p, CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX))
                {
                    break;
                }
                /* 必要ならベリファイ */
                if (p->req_mode == CARD_REQUEST_MODE_SEND_VERIFY)
                {
                    if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
                        break;
                }
                p->src += page;
                p->dst += page;
                len -= page;
            }
        }
    }
    CARDi_EndTask(p, TRUE);
}

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
BOOL CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                MIDmaCallback callback, void *arg, BOOL is_async,
                                CARDRequest req_type, int req_retry, CARDRequestMode req_mode)
{
    CARDiCommon *const p = &cardi_common;

    SDK_USING_BACKUP();

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                  "[CARD] current locking target is not backup.");

    CARDi_WaitTask(p, callback, arg);
    p->src = src;
    p->dst = dst;
    p->len = len;
    p->req_type = req_type;
    p->req_retry = req_retry;
    p->req_mode = req_mode;
    if (is_async)
    {
        CARDi_SetTask(CARDi_RequestStreamCommandCore);
        return TRUE;
    }
    else
    {
        cardi_common.cur_th = OS_GetCurrentThread();
        CARDi_RequestStreamCommandCore(p);
        return (p->cmd->result == CARD_RESULT_SUCCESS);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_AccessStatus

  Description:  ステータスリードまたはライト (テスト用)

  Arguments:    command    CARD_REQ_READ_STATUSまたはCARD_REQ_WRITE_STATUS
                value      CARD_REQ_WRITE_STATUSであれば書き込む値

  Returns:      成功すれば0以上の値、失敗すれば負の値
 *---------------------------------------------------------------------------*/
int CARDi_AccessStatus(CARDRequest command, u8 value)
{
    CARDiCommon *const p = &cardi_common;

    SDK_USING_BACKUP();

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                  "[CARD] current locking target is not backup.");

    CARDi_WaitTask(p, NULL, NULL);
    cardi_common.cur_th = OS_GetCurrentThread();
    p->backup_cache_page_buf[0] = value;
    DC_FlushRange(p->backup_cache_page_buf, 1);
    p->cmd->src = (u32)p->backup_cache_page_buf;
    p->cmd->dst = (u32)p->backup_cache_page_buf;
    (void)CARDi_Request(p, command, 1);
    DC_InvalidateRange(p->backup_cache_page_buf, 1);
    CARDi_EndTask(p, TRUE);
    return (p->cmd->result == CARD_RESULT_SUCCESS) ? p->backup_cache_page_buf[0] : -1;
}

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
BOOL CARDi_RequestWriteSectorCommand(u32 src, u32 dst, u32 len, BOOL verify,
                                     MIDmaCallback callback, void *arg, BOOL is_async)
{
    CARDiCommon *const p = &cardi_common;

    SDK_USING_BACKUP();

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                  "[CARD] current locking target is not backup.");

    CARDi_WaitTask(p, callback, arg);
    p->src = src;
    p->dst = dst;
    p->len = len;
    p->req_mode = verify ? CARD_REQUEST_MODE_SEND_VERIFY : CARD_REQUEST_MODE_SEND;
    if (is_async)
    {
        CARDi_SetTask(CARDi_RequestWriteSectorCommandCore);
        return TRUE;
    }
    else
    {
        cardi_common.cur_th = OS_GetCurrentThread();
        CARDi_RequestWriteSectorCommandCore(p);
        return (p->cmd->result == CARD_RESULT_SUCCESS);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  get type of current specified backup device.

  Arguments:    None.

  Returns:      type of current backup device.
 *---------------------------------------------------------------------------*/
CARDBackupType CARD_GetCurrentBackupType(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->type;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  get total size of backup device.

  Arguments:    None.

  Returns:      total size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupTotalSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.total_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  get sector size of backup device.

  Arguments:    None.

  Returns:      sector size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupSectorSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.sect_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  get page size of backup device.

  Arguments:    None.

  Returns:      page size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupPageSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.page_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  identify the type of backup device on NITRO-CARD.

  Arguments:    type of backup device on NITRO-CARD.

  Returns:      if the reading test operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_IdentifyBackup(CARDBackupType type)
{
    CARDiCommon *const p = &cardi_common;

    /*
     * 2.2PR 以降で導入されたバックアップアクセス静的判定の妨げになるため,
     * 明示的に CARD_BACKUP_TYPE_NOT_USE を指定することは禁止された.
     */
    SDK_USING_BACKUP();
    if (type == CARD_BACKUP_TYPE_NOT_USE)
    {
        OS_TPanic("cannot specify CARD_BACKUP_TYPE_NOT_USE.");
    }

    SDK_ASSERT(CARD_IsAvailable());
    /* 2.1PR 以降, 正当性判定まで行うことになりバスのロックが必須になった */
    SDK_ASSERTMSG((CARDi_GetTargetMode() == CARD_TARGET_BACKUP),
                  "CARD_IdentifyBackup() must be called with CARD locked by CARD_LockBackup()!");

    /* CARD アクセスに対する正当性判定
       もしアクセス禁止なら関数内部で OS_Panic する */
    CARD_CheckEnabled();

    /*
     * タスクを待って ARM7 へデバイスタイプを送信.
     * これは直接デバイスを操作しないので事前のロックは特に不要だったが,
     * 2.1PR 以降, 正当性判定まで行うことになりバスのロックが必須になった.
     */
    CARDi_WaitTask(p, NULL, NULL);
    CARDi_IdentifyBackupCore(type);
    cardi_common.cur_th = OS_GetCurrentThread();
    (void)CARDi_Request(p, CARD_REQ_IDENTIFY, 1);
    /*
     * 先頭 1 BYTE のリードコマンドを発行して結果値を取得する.
     * 接触不良, 破損, 寿命のいずれかであれば値に関わらず TIMEOUT が返る.
     * (TIMEOUT は, デバイス種別に関わらず Read-Status コマンドで判断可能)
     */
    p->cmd->src = 0;
    p->cmd->dst = (u32)p->backup_cache_page_buf;
    p->cmd->len = 1;
    (void)CARDi_Request(p, CARD_REQ_READ_BACKUP, 1);
    CARDi_EndTask(p, TRUE);
    return (p->cmd->result == CARD_RESULT_SUCCESS);
}


/*---------------------------------------------------------------------------*
  Name:         CARD_WaitBackupAsync

  Description:  wait for completion of current asynchronous backup operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_WaitBackupAsync(void)
{
    return CARDi_WaitAsync();
}


/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  check if asynchronous backup operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_TryWaitBackupAsync(void)
{
    return CARDi_TryWaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  request to cancel current asynchronous backup operation.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_CancelBackupAsync(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    cardi_common.flag |= CARD_STAT_CANCEL;
    (void)OS_RestoreInterrupts(bak_cpsr);
}



#endif /* SDK_ARM9 */


/*---------------------------------------------------------------------------*

  $Log: card_backup.c,v $
  Revision 1.31  2007/11/13 04:22:01  yosizaki
  support erase-SUBSECTOR.

  Revision 1.30  2007/10/04 13:11:37  yosizaki
  add test commands.

  Revision 1.29  2007/02/20 00:28:12  kitase_hirotake
  indent source

  Revision 1.28  2006/05/25 02:49:51  yosizaki
  add CARDi_RequestWriteSectorCommand.

  Revision 1.27  2006/05/01 12:18:31  yosizaki
  fix about CARD_REQ_ERASE_SECTOR_BACKUP.

  Revision 1.26  2006/04/10 04:15:16  yosizaki
  small fix for CARD_REQUEST_MODE_SPECIAL.

  Revision 1.25  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.24  2005/09/02 07:35:24  yosizaki
  small fix.

  Revision 1.23  2005/09/02 07:04:38  yosizaki
  fix comment.

  Revision 1.22  2005/09/02 07:03:03  yosizaki
  add CARD_CancelBackupAsync()

  Revision 1.21  2005/06/20 04:47:08  yosizaki
  add assertion in CARD_IdentifyBackup().

  Revision 1.20  2005/06/10 10:16:39  yosizaki
  add a string which is embedded when using backup.

  Revision 1.19  2005/05/12 11:19:42  yosizaki
  optimize some functions and calling interfaces.
  add comment.

  Revision 1.18  2005/04/05 07:24:03  yosizaki
  add check in CARD_IdentifyBackup().

  Revision 1.17  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.16  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.15  2004/12/15 09:44:45  yasu
  CARD アクセスイネーブラ機構の追加

  Revision 1.14  2004/12/08 12:40:26  yosizaki
  add comments.
  remove cardi_device.

  Revision 1.13  2004/12/07 11:05:37  yosizaki
  add comment.

  Revision 1.12  2004/11/12 05:46:01  yosizaki
  add CARDi_CheckValidity().

  Revision 1.11  2004/11/02 10:04:12  yosizaki
  fix comment.

  Revision 1.10  2004/09/10 00:00:16  yosizaki
  add assertion checkings.

  Revision 1.9  2004/09/02 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.8  2004/08/25 23:57:49  yosizaki
  fix CARD_IdentifyBackup. (set callback NULL)

  Revision 1.7  2004/08/23 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.6  2004/08/18 13:42:05  yosizaki
  add FLASH 2Mbit.

  Revision 1.5  2004/08/18 12:11:37  yosizaki
  small fix (cardi_device->type)

  Revision 1.4  2004/08/18 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.3  2004/07/28 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.2  2004/07/23 08:28:46  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.1  2004/07/19 13:14:50  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
