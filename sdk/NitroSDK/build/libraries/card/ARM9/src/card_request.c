/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_request.c

  Copyright 2003-2005,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#if defined(SDK_ARM9)
/*****************************************************************************/
/* code for ARM9 only */


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"


/******************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO ワード受信コールバック.

  Arguments:    tag        PXI タグ (常に PXI_FIFO_TAG_FS)
                data       受信データ
                err        エラービット (旧仕様によるもの)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(data)
    if ((tag == PXI_FIFO_TAG_FS) && err)
    {
        CARDiCommon *const p = &cardi_common;
        /* ARM7 からの応答を受信して完了を通知する */
        SDK_ASSERT(data == CARD_REQ_ACK);
        p->flag &= ~CARD_STAT_REQ;
        OS_WakeupThreadDirect(p->cur_th);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskThread

  Description:  タスクスレッドのメイン関数.

  Arguments:    arg          不使用

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_TaskThread(void *arg)
{
    CARDiCommon *const p = &cardi_common;
    (void)arg;

    for (;;)
    {
        /* 次の処理を待つ */
        OSIntrMode bak_psr = OS_DisableInterrupts();
        while ((p->flag & CARD_STAT_TASK) == 0)
        {
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_psr);
        (*p->task_func) (p);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_Request

  Description:  ARM9 から ARM7 へリクエスト送信し, 完了をブロッキング.
                結果が CARD_RESULT_SUCCESS でないなら指定回数までは再試行する.
                (所定のバスのロックおよびタスクスレッドの排他制御は,
                 この関数の呼び出し元で保証する)

  Arguments:    p            ライブラリのワークバッファ (効率のために引数渡し)
                req_type     コマンドリクエストの種類
                retry_max    リトライ最大回数

  Returns:      結果が CARD_RESULT_SUCCESS であれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARDi_Request(CARDiCommon * p, int req_type, int retry_count)
{
    /* PXI 未初期化ならここで実行 */
    if ((p->flag & CARD_STAT_INIT_CMD) == 0)
    {
        p->flag |= CARD_STAT_INIT_CMD;
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7))
        {
            OS_SpinWait(100);
        }
        /* 最初のコマンド "INIT" を送信 (再帰) */
        (void)CARDi_Request(p, CARD_REQ_INIT, 1);
    }
    /* 設定した共有メモリをフラッシュ */
    DC_FlushRange(p->cmd, sizeof(*p->cmd));
    DC_WaitWriteBufferEmpty();

    do
    {
        /* コマンドリクエスト送信 */
        p->command = req_type;
        p->flag |= CARD_STAT_REQ;
        CARDi_SendPxi((u32)req_type);
        /* 後続引数があれば追加送信 */
        switch (req_type)
        {
        case CARD_REQ_INIT:
            CARDi_SendPxi((u32)p->cmd);
            break;
        }
        {
            /* コマンド完了を待機 */
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while ((p->flag & CARD_STAT_REQ) != 0)
            {
                OS_SleepThread(NULL);
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
        DC_InvalidateRange(p->cmd, sizeof(*p->cmd));
        /* タイムアウトなら指定回数まで再試行 */
    }
    while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

    /* 成否を返す */
    return (p->cmd->result == CARD_RESULT_SUCCESS);
}


/******************************************************************************/
#endif /* defined(SDK_ARM9) */


/*---------------------------------------------------------------------------*
  $Log: card_request.c,v $
  Revision 1.4  2007/07/31 00:53:00  yosizaki
  update copyright

  Revision 1.3  2007/07/30 01:59:16  yosizaki
  fix about asynchronous task thread.

  Revision 1.2  2005/11/28 01:17:26  yosizaki
  fix about cache operation.

  Revision 1.1  2005/05/12 11:15:23  yosizaki
  moved from ../common/src

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*

  [old log (build/libraries/card/common/src/card_request.c)]

  Revision 1.9  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.8  2005/02/18 11:20:41  yosizaki
  fix around hidden warnings.

  Revision 1.7  2004/11/15 00:54:38  yosizaki
  move ARM7 implementations to ../arm7/src/card_command.c

  Revision 1.6  2004/09/02 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.5  2004/08/23 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.4  2004/08/18 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.3  2004/08/18 03:24:14  yosizaki
  fix around PXI-INIT command.

  Revision 1.2  2004/07/28 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.1  2004/07/23 08:29:16  yosizaki
  (none)

 *---------------------------------------------------------------------------*/
