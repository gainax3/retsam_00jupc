/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_wm.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_wm.c,v $
  Revision 1.8  2005/10/28 11:23:41  seiki_masashi
  WM_STATECODE_DISCONNECTED_FROM_MYSELF の追加に対応
  WM_STATECODE_PORT_INIT の追加に対応

  Revision 1.7  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.6  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.5  2005/01/11 07:41:13  takano_makoto
  fix copyright header.

  Revision 1.4  2005/01/07 02:56:25  takano_makoto
  WM_StartMPのコールバックで問題のないエラー通知を無視するように修正。

  Revision 1.3  2004/11/24 13:00:02  takano_makoto
  エラー処理を追加

  Revision 1.2  2004/11/23 23:50:54  takano_makoto
  Warningを除去

  Revision 1.1  2004/11/22 12:38:30  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_wm.h"
#include "mb_child.h"
#include "mb_wm_base.h"
#include "mb_block.h"

//===========================================================================
// プロトタイプ宣言
//===========================================================================

static BOOL IsSendEnabled(void);
static void MBi_WMStateOutStartConnect(void *arg);
static void ChildStateOutStartMP(void *arg);
static void ChildPortCallback(void *arg);
static void StateOutMPSendToParent(void *arg);

static void MBi_WMStateOutStartConnect(void *arg);

static void MBi_WMStateOutEndMP(void *arg);

static void MBi_WMStateOutDisconnect(void *arg);
static void MBi_WMStateInDisconnect(void);

static void MBi_WMStateOutReset(void *arg);

static void MBi_WMSendCallback(u16 type, void *arg);
static void MBi_WMErrorCallback(u16 apiid, u16 error_code);
static void MBi_WMApiErrorCallback(u16 apiid, u16 error_code);


//===========================================================================
// 変数宣言
//===========================================================================

static MBWMWork *wmWork = NULL;


//===========================================================================
// 関数定義
//===========================================================================

/*---------------------------------------------------------------------------*
  Name:         MBi_WMSetBuffer

  Description:  MB_WMがMP通信のために使用するバッファを設定します。
                MBWMWorkのサイズだけ与える必要があります。

  Arguments:    buf     ワークバッファとして使用する領域へのポインタ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMSetBuffer(void *buf)
{
    SDK_NULL_ASSERT(buf);
    SDK_ASSERT(((u32)buf & 0x1f) == 0); // 32バイトアラインされているかどうか

    wmWork = (MBWMWork *) buf;
    wmWork->start_mp_busy = 0;         // 親機のStartMP多重呼び出し防止
    wmWork->mpStarted = 0;
    wmWork->child_bitmap = 0;
    wmWork->mpBusy = 0;
    wmWork->endReq = 0;
    wmWork->sendBuf = NULL;
    wmWork->recvBuf = NULL;
    wmWork->mpCallback = NULL;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMSetCallback

  Description:  コールバックを設定します。

  Arguments:    callback    コールバック関数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMSetCallback(MBWMCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    wmWork->mpCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStartConnect

  Description:  親機への接続開始

  Arguments:    bssDesc  接続する親機のBssDesc

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMStartConnect(WMBssDesc *bssDesc)
{
    WMErrCode result;

    wmWork->mpStarted = 0;
    wmWork->endReq = 0;

    wmWork->sendBufSize = (u16)WM_SIZE_MP_CHILD_SEND_BUFFER(bssDesc->gameInfo.childMaxSize, FALSE);
    wmWork->recvBufSize =
        (u16)WM_SIZE_MP_CHILD_RECEIVE_BUFFER(bssDesc->gameInfo.parentMaxSize, FALSE);
    wmWork->pSendLen = bssDesc->gameInfo.parentMaxSize;
    wmWork->pRecvLen = bssDesc->gameInfo.childMaxSize;
    wmWork->blockSizeMax = (u16)MB_COMM_CALC_BLOCK_SIZE(wmWork->pSendLen);
    MBi_SetChildMPMaxSize(wmWork->pRecvLen);

    result = WM_StartConnect(MBi_WMStateOutStartConnect, bssDesc, NULL);

    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMSendCallback(MB_CALLBACK_CONNECT_FAILED, NULL);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutStartConnect

  Description:  親機への接続コールバック

  Arguments:    arg     WM_StartConnectのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutStartConnect(void *arg)
{
    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMSendCallback(MB_CALLBACK_CONNECT_FAILED, arg);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_BEACON_LOST:
        break;
    case WM_STATECODE_CONNECT_START:
        break;
    case WM_STATECODE_DISCONNECTED:
        MBi_WMSendCallback(MB_CALLBACK_DISCONNECTED_FROM_PARENT, NULL);
        break;
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        // 自ら切断した場合は処理しない
        break;
    case WM_STATECODE_CONNECTED:
        // 認証終了時。
        MBi_WMSendCallback(MB_CALLBACK_CONNECTED_TO_PARENT, arg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ChildStartMP

  Description:  MP通信開始

  Arguments:    sendBuf     送信バッファとして設定する領域へのポインタ
                recvBuf     受信バッファとして設定する領域へのポインタ

  Returns:      エラーコード、正常時はWM_ERRCODE_OPERATING.
 *---------------------------------------------------------------------------*/
void MBi_ChildStartMP(u16 *sendBuf, u16 *recvBuf)
{
    WMErrCode result;

    wmWork->sendBuf = (u32 *)sendBuf;
    wmWork->recvBuf = (u32 *)recvBuf;

    result = WM_SetPortCallback(WM_PORT_BT, ChildPortCallback, NULL);
    if (result != WM_ERRCODE_SUCCESS)
    {
        MBi_WMApiErrorCallback(WM_APIID_START_MP, result);
        return;
    }

    result = WM_StartMPEx(ChildStateOutStartMP, recvBuf, wmWork->recvBufSize, sendBuf, wmWork->sendBufSize, 1,  // mpFreq
                          0,           // defaultRetryCount
                          FALSE,       // minPollBmpMode
                          FALSE,       // singlePacketMode
                          TRUE,        // fixFrameMode
                          TRUE);       // ignoreFatalError

    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMApiErrorCallback(WM_APIID_START_MP, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ChildStateOutStartMP

  Description:  子機 WM_StartMPExコールバック関数

  Arguments:    arg     WM_StartMPのコールバック引数

  Returns:      エラーコード、正常時はWM_ERRCODE_OPERATING.
 *---------------------------------------------------------------------------*/
static void ChildStateOutStartMP(void *arg)
{
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        // エラー処理の必要の無いエラー通知の場合は終了
        if (cb->errcode == WM_ERRCODE_SEND_FAILED)
        {
            return;
        }
        else if (cb->errcode == WM_ERRCODE_TIMEOUT)
        {
            return;
        }
        else if (cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
        {
            return;
        }

        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_MP_START:
        wmWork->mpStarted = 1;         // MP開始済みフラグをセット
        wmWork->mpBusy = 0;
        wmWork->child_bitmap = 0;
        MBi_WMSendCallback(MB_CALLBACK_MP_STARTED, NULL);
        {
            // MP送信許可コールバック
            MBi_WMSendCallback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
        }
        break;

    case WM_STATECODE_MP_IND:
        // None.
        break;

    case WM_STATECODE_MPACK_IND:
        // None.
        break;

    case WM_STATECODE_MPEND_IND:      // 親機のみに発生
    default:
        MBi_WMErrorCallback(cb->apiid, WM_ERRCODE_FAILED);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMDisconnect

  Description:  子機のMP切断処理。WM_EndMP完了後に親機からDisconnectします。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMDisconnect(void)
{
    WMErrCode result;

    wmWork->endReq = 1;

    result = WM_EndMP(MBi_WMStateOutEndMP);

    if (result != WM_ERRCODE_OPERATING)
    {
        wmWork->endReq = 0;
        MBi_WMApiErrorCallback(WM_APIID_END_MP, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutEndMP

  Description:  WM_EndMPのコールバック関数

  Arguments:    arg     WM_EndMPのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutEndMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        wmWork->endReq = 0;
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }

    wmWork->mpStarted = 0;
    MBi_WMStateInDisconnect();
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateInDisconnect

  Description:  子機を親機から切断し、IDLE状態へ遷移させます。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateInDisconnect(void)
{
    WMErrCode result;

    result = WM_Disconnect(MBi_WMStateOutDisconnect, 0);

    if (result != WM_ERRCODE_OPERATING)
    {
        wmWork->endReq = 0;
        MBi_WMApiErrorCallback(WM_APIID_DISCONNECT, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateInDisconnect

  Description:  WM_Disconnectのコールバック引数

  Arguments:    arg  WM_Disconnectのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutDisconnect(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    wmWork->endReq = 0;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }
    MBi_WMSendCallback(MB_CALLBACK_DISCONNECT_COMPLETE, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMReset

  Description:  子機のをリセットし、IDLEステートに遷移させます。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMReset(void)
{
    WMErrCode result;

    result = WM_Reset(MBi_WMStateOutReset);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMApiErrorCallback(WM_APIID_RESET, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutReset

  Description:  子機のをリセットし、IDLEステートに遷移させます。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutReset(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }
    // Reset は次の状態を開始せず、アイドリング（待機中）状態にします。
    MBi_WMSendCallback(MB_CALLBACK_DISCONNECT_COMPLETE, NULL);
}


/*
 * MP送信許可チェック
   
   SetMP後、コールバックが返る前に、再びMPをセットしてしまわないように、
   SetMP実行時に立てる、mpBusyのフラグを判定要素に追加。
 
 */
/*---------------------------------------------------------------------------*
  Name:         IsSendEnabled

  Description:  現在、新しいMPデータをセットしても良いかどうかを判定する関数です。
                SetMP後、コールバックが返る前に、再びMPをセットしてしまわないように、
                SetMP実行時に立てる、mpBusyのフラグを判定要素に追加しました。

  Arguments:    None.

  Returns:      新しいデータをセットしても構わないならば TRUE
                そうでない場合は FALSE を返します。
 *---------------------------------------------------------------------------*/
static BOOL IsSendEnabled(void)
{
    return (wmWork->mpStarted == 1) && (wmWork->mpBusy == 0) && (wmWork->endReq == 0);
}


/*---------------------------------------------------------------------------*
  Name:         ChildPortCallback

  Description:  子機MPポートコールバック関数

  Arguments:    arg     MP通信のポートコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ChildPortCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_PORT_RECV:
        // データ受信を通知
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_RECV, cb);
        break;
    case WM_STATECODE_CONNECTED:
        // 接続通知
        break;
    case WM_STATECODE_PORT_INIT:
    case WM_STATECODE_DISCONNECTED:
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_MPSendToParent

  Description:  バッファ内容を親機へ送信します。

  Arguments:    body_len データサイズ
                pollbmp  送信相手のポールビットマップ(子機の場合には関係ありません)
                sendbuf  送信データへのポインタ
  Returns:      送信処理の開始に成功したならば、WM_ERRCODE_OPERATING、
                失敗したらならば、それ以外のコードが返ってきます。
 *---------------------------------------------------------------------------*/
WMErrCode MBi_MPSendToParent(u32 body_len, u16 pollbmp, u32 *sendbuf)
{
    WMErrCode result;

    // 32バイトアラインチェック
    SDK_ASSERT(((u32)sendbuf & 0x1F) == 0);

    DC_FlushRange(sendbuf, sizeof(body_len));
    DC_WaitWriteBufferEmpty();

    if (!IsSendEnabled())
    {
        return WM_ERRCODE_FAILED;
    }

    result = WM_SetMPDataToPort(StateOutMPSendToParent,
                                (u16 *)sendbuf,
                                (u16)body_len, pollbmp, WM_PORT_BT, WM_PRIORITY_LOW);
    if (result != WM_ERRCODE_OPERATING)
    {
        return result;
    }

    wmWork->mpBusy = 1;
    return WM_ERRCODE_OPERATING;
}


/*---------------------------------------------------------------------------*
  Name:         StateOutMPSendToParent

  Description:  MP送信完了通知コールバック

  Arguments:    arg     WM_SetMPDataToPortのコールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StateOutMPSendToParent(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    wmWork->mpBusy = 0;
    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT, arg);
    }
    else if (cb->errcode == WM_ERRCODE_TIMEOUT)
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT_TIMEOUT, arg);
    }
    else
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT_ERR, arg);
    }
    // 次の送信を許可
    MBi_WMSendCallback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMSendCallback

  Description:  WMレイヤーでのコールバック通知を行ないます。

  Arguments:    type        コールバックタイプ
                arg         コールバック引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMSendCallback(u16 type, void *arg)
{
    if (wmWork->mpCallback == NULL)
    {
        return;
    }
    wmWork->mpCallback(type, arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMErrorCallback

  Description:  WMレイヤーでのエラー通知を行ないます。

  Arguments:    apiid       原因となったWM_APIID
                error_code  エラーコード

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMErrorCallback(u16 apiid, u16 error_code)
{
    MBErrorCallback arg;

    if (wmWork->mpCallback == NULL)
    {
        return;
    }

    arg.apiid = apiid;
    arg.errcode = error_code;

    wmWork->mpCallback(MB_CALLBACK_ERROR, &arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMApiErrorCallback

  Description:  WMのAPIコールの戻り値でエラーが発生した場合の通知を行ないます。

  Arguments:    apiid       原因となったWM_APIID
                error_code  エラーコード

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMApiErrorCallback(u16 apiid, u16 error_code)
{
    MBErrorCallback arg;

    if (wmWork->mpCallback == NULL)
    {
        return;
    }

    arg.apiid = apiid;
    arg.errcode = error_code;

    wmWork->mpCallback(MB_CALLBACK_API_ERROR, &arg);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMApiErrorCallback

  Description:  WMのAPIコールの戻り値でエラーが発生した場合の通知を行ないます。

  Arguments:    apiid       原因となったWM_APIID
                error_code  エラーコード

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMClearCallback(void)
{
    (void)WM_SetPortCallback(WM_PORT_BT, NULL, NULL);
}
