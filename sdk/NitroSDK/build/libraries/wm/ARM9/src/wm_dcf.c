/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_dcf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_dcf.c,v $
  Revision 1.7  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.6  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.5  2004/12/03 06:35:20  ooe
  最大送信サイズのチェック部、1500をWM_DCF_MAX_SIZEに修正。

  Revision 1.4  2004/10/22 04:37:28  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.3  2004/10/08 08:37:37  ooe
  small fix

  Revision 1.2  2004/10/05 09:45:28  terui
  MACアドレスをu8型の配列に統一。

  Revision 1.1  2004/09/10 11:03:30  terui
  wm.cの分割に伴い、新規upload。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         WM_StartDCF

  Description:  インフラストラクチャモードでの通信を開始する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf     -   データ受信バッファへのポインタ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize -   データ受信バッファのサイズ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartDCF(WMCallbackFunc callback, WMDcfRecvBuf *recvBuf, u16 recvBufSize)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // ステートチェック
    result = WMi_CheckState(WM_STATE_CHILD);
    WM_CHECK_RESULT(result);

    // DCF状態確認
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == TRUE)
    {
        WM_WARNING("Already DCF mode. So can't start DCF again.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // パラメータチェック
    if (recvBufSize < 16)
    {
        WM_WARNING("Parameter \"recvBufSize\" must not be less than %d.\n", 16);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (recvBuf == NULL)
    {
        WM_WARNING("Parameter \"recvBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)recvBuf & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"recvBuf\" is not 32-byte aligned.\n");
    }

    // 指定バッファのキャッシュを書き出し
    DC_StoreRange(recvBuf, recvBufSize);

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_START_DCF, callback);

    // ARM7にFIFOで通知
    result = WMi_SendCommand(WM_APIID_START_DCF, 2, (u32)recvBuf, (u32)recvBufSize);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetDCFData

  Description:  インフラストラクチャモード通信にて送信するデータを予約する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                destAdr     -   通信相手のMACアドレスを示すバッファへのポインタ。
                sendData    -   送信予約するデータへのポインタ。
                                送信予約するデータの実体は強制的にキャッシュストア
                                される点に注意。
                sendDataSize -  送信予約するデータのサイズ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode
WM_SetDCFData(WMCallbackFunc callback, const u8 *destAdr, const u16 *sendData, u16 sendDataSize)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    u32     wMac[2];

    // ステートチェック
    result = WMi_CheckState(WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    // DCF状態確認
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == FALSE)
    {
        WM_WARNING("It is not DCF mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // パラメータチェック
    if (sendDataSize > WM_DCF_MAX_SIZE)
    {
        WM_WARNING("Parameter \"sendDataSize\" is over %d.\n", WM_DCF_MAX_SIZE);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)sendData & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"sendData\" is not 32-byte aligned.\n");
    }

    // 指定バッファのキャッシュを書き出し
    DC_StoreRange((void *)sendData, sendDataSize);

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_SET_DCF_DATA, callback);

    // MACアドレスをコピー
    MI_CpuCopy8(destAdr, wMac, 6);

    // ARM7にFIFOで通知
    result = WMi_SendCommand(WM_APIID_SET_DCF_DATA, 4,
                             (u32)wMac[0], (u32)wMac[1], (u32)sendData, (u32)sendDataSize);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndDCF

  Description:  インフラストラクチャモードでの通信を停止する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDCF(WMCallbackFunc callback)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // ステートチェック
    result = WMi_CheckState(WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    // DCF状態確認
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == FALSE)
    {
        WM_WARNING("It is not DCF mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_END_DCF, callback);

    // ARM7にFIFOで通知
    result = WMi_SendCommand(WM_APIID_END_DCF, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
