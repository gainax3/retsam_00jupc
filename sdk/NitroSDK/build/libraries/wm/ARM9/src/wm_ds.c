/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_ds.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_ds.c,v $
  Revision 1.26  2007/10/18 12:01:33  seiki_masashi
  WM_GetConnectedAIDs 関数を使用するように変更

  Revision 1.25  2007/10/16 02:00:38  okubata_ryoma
  不要な warning を削除

  Revision 1.24  2006/06/14 01:43:53  okubata_ryoma
  Reset時にWM_EndDataSharing関数を呼ぶ処理を追加に伴った変更

  Revision 1.23  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.22  2005/11/01 08:56:18  seiki_masashi
  不要な状態検査の除去

  Revision 1.21  2005/10/28 11:57:24  seiki_masashi
  small fix

  Revision 1.20  2005/10/28 11:17:33  seiki_masashi
  親機からの切断への対応
  port の仕様追加への対応
  WM の内部構造体にできるだけ触らないように修正

  Revision 1.19  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.18  2005/02/23 12:48:14  seiki_masashi
  デバッグ表示の整理

  Revision 1.17  2005/02/18 12:27:45  seiki_masashi
  デバッグ用スイッチの判定文を #if から #ifdef に変更

  Revision 1.16  2005/02/18 07:52:31  seiki_masashi
  warning 対策

  Revision 1.15  2005/01/14 07:46:55  seiki_masashi
  子機時に、自分と関係ない DataSharing のパケットを受信してしまう不具合を修正

  Revision 1.14  2005/01/14 07:45:12  seiki_masashi
  WmGetSharedDataAddress の整理

  Revision 1.13  2005/01/11 07:46:10  takano_makoto
  fix copyright header.

  Revision 1.12  2005/01/07 11:43:41  seiki_masashi
  デバッグ出力の OS_Printf を OS_TPrintf に変更

  Revision 1.11  2005/01/06 06:42:56  seiki_masashi
  MATH_CountPopulation を使用するように変更

  Revision 1.10  2005/01/06 01:54:17  seiki_masashi
  コメントの修正とコードの最適化

  Revision 1.9  2004/10/29 03:36:03  seiki_masashi
  デバッグメッセージの修正

  Revision 1.8  2004/10/29 01:41:11  seiki_masashi
  WM_EndDataSharing後の送信完了コールバックへの対応

  Revision 1.7  2004/10/22 04:37:28  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.6  2004/09/23 10:02:10  seiki_masashi
  small fix.

  Revision 1.5  2004/09/23 09:47:31  seiki_masashi
  DataSharing 内部のエラー処理ルーチンの追加

  Revision 1.4  2004/09/22 04:06:43  seiki_masashi
  DataSharing の排他に関する不具合を修正

  Revision 1.3  2004/09/21 10:50:38  takano_makoto
  remove warning in WM_GetSharedDataAddress()

  Revision 1.2  2004/09/10 12:02:46  terui
  Bug fix ( port number check ).

  Revision 1.1  2004/09/10 11:03:30  terui
  wm.cの分割に伴い、新規upload。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    内部関数定義
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback);
static void WmDataSharingReceiveCallback_Parent(void *callback);
static void WmDataSharingReceiveCallback_Child(void *callback);
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data);
static void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed);
static u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf,
                                   u32 aid);

//// data-sharing で使用中のポート番号一覧
//static u16 WmDataSharingPortBitmap = 0;

static inline u16 WmDataSharingGetNextIndex(u32 index)
{
    return (u16)((index + 1) % WM_DS_DATASET_NUM);
}

static inline u16 WmDataSharingGetPrevIndex(u32 index)
{
    return (u16)((index + WM_DS_DATASET_NUM - 1) % WM_DS_DATASET_NUM);
}


/*---------------------------------------------------------------------------*
  Name:         WM_StartDataSharing

  Description:  データシェアリング機能を有効にする。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体
                port        -   使用する port 番号
                aidBitmap   -   データシェアを行う相手の AID のビットマップ
                dataLength  -   共有メモリのデータ長 (偶数バイトを指定すること)
                doubleMode  -   毎フレーム WM_StepDataSharing する場合は TRUE を指定

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartDataSharing(WMDataSharingInfo *dsInfo, u16 port, u16 aidBitmap, u16 dataLength,
                    BOOL doubleMode)
{
    WMErrCode result;
    int     aid;
    u16     connectedAIDs = 0x0001;

    // ステートチェック
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // パラメータチェック
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (port >= WM_NUM_OF_PORT)
    {
        WM_WARNING("Parameter \"port\" must be less than %d.\n", WM_NUM_OF_PORT);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)dsInfo & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"dsInfo\" is not 32-byte aligned.\n");
    }
    if (aidBitmap == 0)
    {
        WM_WARNING("Parameter \"aidBitmap\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // 必要な情報を取得
    aid = WM_GetAID();
    if (aid == 0)
    {
        connectedAIDs = WM_GetConnectedAIDs();
    }

    // データシェアリング制御変数を初期化
    MI_CpuClearFast(dsInfo, sizeof(WMDataSharingInfo));
    dsInfo->writeIndex = 0;
    dsInfo->sendIndex = 0;
    dsInfo->readIndex = 0;
    dsInfo->dataLength = dataLength;
    dsInfo->port = port;
    dsInfo->aidBitmap = 0;
    dsInfo->doubleMode = (u16)((doubleMode) ? TRUE : FALSE);

    aidBitmap |= (1 << aid);           // 自分の分を足す
    dsInfo->aidBitmap = aidBitmap;

    {
        u16     count = MATH_CountPopulation(aidBitmap);
        dsInfo->stationNumber = count;
        dsInfo->dataSetLength = (u16)(dataLength * count);

        if (dsInfo->dataSetLength > WM_DS_DATA_SIZE)
        {
            // 合計のデータ量が許容値を超えている
            dsInfo->aidBitmap = 0;
            WM_WARNING("Total size of sharing data must be less than or equal to %d bytes.\n",
                       WM_DS_DATA_SIZE);
            return WM_ERRCODE_INVALID_PARAM;
        }
        dsInfo->dataSetLength += 4;    // aidBitmap, receivedBitmap
    }

    dsInfo->state = WM_DS_STATE_START;

    if (aid == 0)
    {
        // 親機の開始処理
        int     i;

        for (i = 0; i < WM_DS_DATASET_NUM; i++)
        {
            dsInfo->ds[i].aidBitmap = (u16)(dsInfo->aidBitmap & (connectedAIDs | 0x0001));
        }

        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Parent, (void *)dsInfo);

        // 起動用の空データを送信
        for (i = 0; i < ((dsInfo->doubleMode == TRUE) ? 2 : 1); i++)
        {
            int     res;

            dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[i],
                                     dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & connectedAIDs), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                dsInfo->seqNum[i] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }
    }
    else
    {
        // 子機の開始処理
        dsInfo->sendIndex = (u16)(WM_DS_DATASET_NUM - 1);
        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Child, (void *)dsInfo);
    }

    return WM_ERRCODE_SUCCESS;         // 成功終了
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndDataSharing

  Description:  データシェアリング機能を無効にする。

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体

  Returns:      WMErrCode   -   処理結果を返す。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDataSharing(WMDataSharingInfo *dsInfo)
{
    // パラメータチェック
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // データシェアリング状態チェック
    if (dsInfo->aidBitmap == 0)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    (void)WM_SetPortCallback(dsInfo->port, NULL, NULL);
    dsInfo->aidBitmap = 0;
    dsInfo->state = WM_DS_STATE_READY;

    return WM_ERRCODE_SUCCESS;         // 成功終了
}

/*---------------------------------------------------------------------------*
  Name:         WM_StepDataSharing

  Description:  データシェアリングの同期を一つ進める

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体
                sendData    -   共有したい送信データ
                receiveData -   受信した共有データ

  Returns:      WMErrCode   -   処理結果を返す。
                    *_SUCCESS    : 共有データ送受信成功
                    *_NO_DATASET : まだ次の共有データが来ていない
 *---------------------------------------------------------------------------*/
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo, const u16 *sendData, WMDataSet *receiveData)
{
    WMErrCode result;
    u16     aid;
    u16     connectedAIDs;
    u16     state;

    // ステートチェック
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // パラメータチェック
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // 必要な情報を取得
    aid = WM_GetAID();
    if (aid == 0)
    {
        connectedAIDs = WM_GetConnectedAIDs();
    }

    // データシェアリング状態チェック
    state = dsInfo->state;
    if (state == WM_DS_STATE_ERROR)
    {
        WM_WARNING("An error occurred during data-sharing.\n");
        return WM_ERRCODE_FAILED;
    }
    if (state != WM_DS_STATE_START && state != WM_DS_STATE_RETRY_SEND)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    result = WM_ERRCODE_NO_DATASET;

    if (aid == 0)
    {
        // 親機なら
        BOOL    sendFlag = FALSE;
        BOOL    delayed = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // 前回 SEND_QUEUE_FULL につき、再送信
            int     res;
            int     oldWI;
            dsInfo->state = WM_DS_STATE_START;

            WM_DLOG_DATASHARING("send queue was full. do retry.");

            oldWI = WmDataSharingGetPrevIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo,
                                     (u16 *)&dsInfo->ds[oldWI], dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & connectedAIDs), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d\n",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);
                dsInfo->seqNum[oldWI] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }

        // 親機は DataSet を送り終わったら使用可能になるので sendIndex が読み出せる限界を決める
        if (dsInfo->readIndex != dsInfo->sendIndex)
        {
            // 読み出し処理
            WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                 dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                 dsInfo->seqNum[dsInfo->readIndex]);

            dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;  // 送信データ中の aidBitmap の最下位ビットは遅延フラグ
            MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

            sendFlag = TRUE;
            result = WM_ERRCODE_SUCCESS;
            if (dsInfo->doubleMode == FALSE && connectedAIDs != 0
                && dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0x0001)
            {
                // 親機がデータをセットすればすぐに送信可能 ＝ 親機のみがフレームずれ
                delayed = TRUE;
            }
            else
            {
                delayed = FALSE;
            }
        }

        // 送信バッファに全機分揃っていたら DataSet を送信
        WmDataSharingSendDataSet(dsInfo, FALSE);

        if (sendFlag)
        {
            // 親機は無線で送信する代わりに自分のバッファに入れる
            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            WmDataSharingReceiveData(dsInfo, 0, (u16 *)sendData);       // 先での連鎖的な書き換えが面倒なので const を外す

            if (dsInfo->doubleMode == FALSE)
            {
                // 毎フレームの Step でない場合のみ、
                // このタイミングで今 GF での送信用の DataSet を用意しておく必要がある。
                // 送信バッファに全機分揃っていたら DataSet を送信
                WmDataSharingSendDataSet(dsInfo, delayed);
            }
        }
    }
    else
    {
        // 子機なら
        BOOL    sendFlag = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // 前回 SEND_QUEUE_FULL につき、再送信
            sendFlag = TRUE;
            dsInfo->state = WM_DS_STATE_START;
            WM_DLOG_DATASHARING("send queue was full. do retry.");
        }
        else
        {
            // 子機は DataSet を受信したらすぐに使用可能なので writeIndex が読み出せる限界を決める
            if (dsInfo->readIndex != dsInfo->writeIndex)
            {
                // 遅延フラグが寝ていたら、1フレーム強制的に遅らせる
                // 送信データ中の aidBitmap の最下位ビットは遅延フラグ
                if (!(dsInfo->ds[dsInfo->readIndex].aidBitmap & 0x0001))
                {
                    dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;
                }
                else
                {
                    // 読み出し処理
                    WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                         dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                         dsInfo->seqNum[dsInfo->readIndex]);
                    MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

                    sendFlag = TRUE;
                    result = WM_ERRCODE_SUCCESS;
                }
            }
        }

        if (sendFlag)
        {
            // 子機はそのまま送信
            int     res;
            // dsInfo->ds の一部を送信バッファとして使わせてもらう
            u16    *buf = (u16 *)(((u8 *)&dsInfo->ds[dsInfo->sendIndex]) + 32); // 32-byte align されている必要がある

            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            MI_CpuCopy16(sendData, buf, dsInfo->dataLength);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, buf, dsInfo->dataLength,
                                     dsInfo->aidBitmap, dsInfo->port, WM_PRIORITY_HIGH);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            if (res != WM_ERRCODE_OPERATING && res != WM_ERRCODE_SUCCESS)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
                result = WM_ERRCODE_FAILED;
            }
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSetDataCallback

  Description:  送信完了コールバック。

  Arguments:    callback - コールバック構造体へのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback)
{
    WMArm9Buf *p = WMi_GetSystemWork();
    WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback;
    WMDataSharingInfo *dsInfo;
    u16     aid;

    // WM_EndDataSharing() を呼ばれ、DSInfo が無効な状態で呼び出されていないかを確認
    dsInfo = (WMDataSharingInfo *)(p->portCallbackArgument[cb_Port->port]);
    if ((p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Parent
         && p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Child)
        || dsInfo == NULL || dsInfo != (WMDataSharingInfo *)(cb_Port->arg))
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    aid = WM_GetAID();

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        // 送信完了
        if (aid == 0)
        {
            // 親機側処理
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[dsInfo->sendIndex] = (u16)(cb_Port->seqNo >> 1);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            // 子機側処理
            WM_DLOGF_DATASHARING("sent data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);
        }
    }
    else
    {
        if (cb_Port->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
        {
            // 送信キューがいっぱいだったので再送する
            if (aid != 0)
            {
                // 子機の場合、sendIndex を一つ戻す
                dsInfo->sendIndex = WmDataSharingGetPrevIndex(dsInfo->sendIndex);
            }
            dsInfo->state = WM_DS_STATE_RETRY_SEND;
            WM_DLOG_DATASHARING("send queue is full. will retry.");
        }
        else
        {
            WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n",
                       cb_Port->errcode);
            dsInfo->state = WM_DS_STATE_ERROR;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Parent

  Description:  親機の port 受信コールバック。

  Arguments:    callback - コールバック構造体へのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Parent(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            // 受信した子機のデータをバッファに格納する
            WmDataSharingReceiveData(dsInfo, cb_Port->aid, cb_Port->data);
            WmDataSharingSendDataSet(dsInfo, FALSE);
            break;

        case WM_STATECODE_CONNECTED:
            WmDataSharingSendDataSet(dsInfo, FALSE);    // 必要はないはず
            break;

        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            {
                u32     aidBit;
                u32     writeIndex;
                OSIntrMode enabled;
                aidBit = 1U << cb_Port->aid;
                enabled = OS_DisableInterrupts();
                writeIndex = dsInfo->writeIndex;
                dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
                if (dsInfo->doubleMode == TRUE)
                {
                    dsInfo->ds[WmDataSharingGetNextIndex(writeIndex)].aidBitmap &= ~aidBit;
                }
                (void)OS_RestoreInterrupts(enabled);
                WmDataSharingSendDataSet(dsInfo, FALSE);
                if (dsInfo->doubleMode == TRUE)
                {
                    WmDataSharingSendDataSet(dsInfo, FALSE);
                }
            }
            break;

        case WM_STATECODE_PORT_INIT:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Child

  Description:  子機の port 受信コールバック。

  Arguments:    callback - コールバック構造体へのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Child(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            {
                u32     length;
                u32     aid;
                u32     aidBitmap;
                WMDataSet *dataSet;

                WM_DLOGF_DATASHARING("recv ds   : write: %d, read: %d, send: %d",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);

                // DataSet を保存する
                dataSet = (WMDataSet *)(cb_Port->data);
                aidBitmap = dataSet->aidBitmap;
                length = cb_Port->length;
                aid = WM_GetAID();

                if (length != dsInfo->dataSetLength)
                {
                    WM_WARNING("received DataSharing data size(%d) != dsInfo->dataSetLength(%d)\n",
                               length, dsInfo->dataSetLength);
                    if (length > sizeof(WMDataSet))
                    {
                        // バッファを超えるほど長すぎるデータを受信したら、補正する。
                        WM_WARNING("received DataSharing data exceeds sizeof(WMDataSet)\n");
                        length = sizeof(WMDataSet);
                    }
                }
#ifdef SDK_DEBUG
                if (aidBitmap & ~(dsInfo->aidBitmap))
                {
                    // 子機側で想定していた aidBitmap より多い
                    WM_WARNING("received aidBitmap(%x) has too many members.\n", aidBitmap);
                }
#endif
                if (length >= 4 && (aidBitmap & (1 << aid)))
                {
                    MI_CpuCopy16(dataSet, &dsInfo->ds[dsInfo->writeIndex], length);
                    dsInfo->seqNum[dsInfo->writeIndex] = (u16)(cb_Port->seqNo >> 1);
                    dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                }
            }
            break;
        case WM_STATECODE_PORT_INIT:
        case WM_STATECODE_CONNECTED:
        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveData

  Description:  親機が各機データを受信したので格納する。

  Arguments:    dsInfo - WMDataSharingInfo 構造体。
                aid    - データを受信した端末のAID。
                data   - 受信したデータへのポインタ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data)
{
    u16     aidBit = (u16)(1 << aid);

    WM_DLOGF_DATASHARING("recv data%d: write: %d, read: %d, send: %d", aid, dsInfo->writeIndex,
                         dsInfo->readIndex, dsInfo->sendIndex);

    // 処理対象かを確認
    if (dsInfo->aidBitmap & aidBit)
    {
        u16    *buf;
        u16     writeIndex;
        OSIntrMode enabled;

        if (!(dsInfo->ds[dsInfo->writeIndex].aidBitmap & aidBit))
        {
            if (dsInfo->doubleMode == TRUE)
            {
                WM_DLOGF_DATASHARING("[DS] received two DS packets from aid %d", aid);

                writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                if (!(dsInfo->ds[writeIndex].aidBitmap & aidBit))
                {
                    // 2つまでバッファに貯めるが、それ以上は捨てる
                    OS_Warning("received too many DataSharing packets from aid %d. discarded.\n",
                               aid);
                    return;
                }
            }
            else
            {
                // doubleMode ではなければ、バッファに貯めるのは1つまで
                OS_Warning("received too many DataSharing packets from aid %d. discarded.\n", aid);
                return;
            }
        }
        else
        {
            writeIndex = dsInfo->writeIndex;
        }

        buf = WmGetSharedDataAddress(dsInfo, dsInfo->aidBitmap, dsInfo->ds[writeIndex].data, aid);
        if (data != NULL)
        {
            MI_CpuCopy16(data, buf, dsInfo->dataLength);
        }
        else
        {
            MI_CpuClear16(buf, dsInfo->dataLength);
        }

        enabled = OS_DisableInterrupts();
        // 未受信フラグを寝かせる
        dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
        // 受信済みフラグを立たせる
        dsInfo->ds[writeIndex].receivedBitmap |= aidBit;
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSendDataSet

  Description:  親機が各機データの受信を確認したのち、データセットを送信する。

  Arguments:    dsInfo - WMDataSharingInfo 構造体。
                delayed - フレーム遅れ状態になっていたら TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    // もしも全機分受信が終わったら
    // （受信バッファ時の ds[].aidBitmap は未受信フラグの役目を果たす）
    if (dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0)
    {
        u16     newWI, oldWI, resetWI;
        WMErrCode res;
        u16     connectedAIDs;

        WM_DLOGF_DATASHARING("send ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                             dsInfo->readIndex, dsInfo->sendIndex);

        connectedAIDs = WM_GetConnectedAIDs();
        oldWI = dsInfo->writeIndex;
        newWI = WmDataSharingGetNextIndex(oldWI);
        if (dsInfo->doubleMode == TRUE)
        {
            resetWI = WmDataSharingGetNextIndex(newWI);
        }
        else
        {
            resetWI = newWI;
        }
        SDK_ASSERT(newWI != dsInfo->readIndex && resetWI != dsInfo->readIndex);
//        SDK_ASSERT( dsInfo->sendIndex == dsInfo->writeIndex);
        MI_CpuClear16(&dsInfo->ds[resetWI], sizeof(WMDataSet));
        dsInfo->ds[resetWI].aidBitmap = (u16)(dsInfo->aidBitmap & (connectedAIDs | 0x0001));
        dsInfo->writeIndex = newWI;
        dsInfo->ds[oldWI].aidBitmap = dsInfo->aidBitmap;        // 送信するにあたって aidBitmap に本来の値を入れる
        if (delayed == TRUE)
        {
            // aidBitmap の最下位ビットは遅延フラグ
            dsInfo->ds[oldWI].aidBitmap &= ~0x0001;
        }
        (void)OS_RestoreInterrupts(enabled);
        res =
            WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[oldWI],
                                 dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & connectedAIDs),
                                 dsInfo->port, WM_PRIORITY_HIGH);
        if (res == WM_ERRCODE_NO_CHILD)
        {
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[oldWI] = 0xffff;
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
            }
        }
    }
    else
    {
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetSharedDataAddress

  Description:  データシェアリングの受信データ中の特定の AID のアドレスを得る

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体
                receiveData -   受信した共有データ
                aid         -   AID。

  Returns:      u16*        -   受信データのアドレス。存在しなければ NULL を返す。
 *---------------------------------------------------------------------------*/
u16    *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, u16 aid)
{
    u32     aidBitmap = receiveData->aidBitmap;
    u32     receivedBitmap = receiveData->receivedBitmap;
    u32     aidBit = (1U << aid);

    // パラメータチェック
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return NULL;
    }

    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return NULL;
    }

    if (!(aidBitmap & aidBit))
    {
//        WM_WARNING("Parameter \"aid\" must be a member of \"receiveData->aidBitmap\".\n");
        return NULL;
    }

    if (!(receivedBitmap & aidBit))
    {
        // データを受信できていない
        return NULL;
    }

    return WmGetSharedDataAddress(dsInfo, aidBitmap, receiveData->data, aid);
}

/*---------------------------------------------------------------------------*
  Name:         WmGetSharedDataAddress

  Description:  受信バッファ中の特定の AID のアドレスを得る

  Arguments:    dsInfo      -   WMDataSharingInfo 構造体
                aidBitmap   -   データに含まれている通信相手
                receiveBuf  -   受信バッファ
                aid         -   AID。
                                aidBitmap & (1<<aid) は呼び出し前に確認すること。

  Returns:      u16*        -   受信データのアドレス。
 *---------------------------------------------------------------------------*/
u16    *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf, u32 aid)
{
    u32     mask;
    u32     count;
    u32     offset;

    // aidBitmap の下から aid ビット中の 1 の個数をカウントする
    mask = (0x0001U << aid) - 1U;
    aidBitmap &= mask;
    count = MATH_CountPopulation(aidBitmap);
    offset = dsInfo->dataLength * count;

    return (u16 *)(((u8 *)receiveBuf) + offset);
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
