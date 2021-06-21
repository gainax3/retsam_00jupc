/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_mp.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_mp.c,v $
  Revision 1.26  2007/11/12 09:49:57  seiki_masashi
  子機送信容量の変更に関する制限を緩和

  Revision 1.25  2007/05/10 01:01:29  yasu
  著作年度修正

  Revision 1.24  2007/05/02 08:44:06  seiki_masashi
  WM_StartMPEx 関数での追加設定パラメータの影響が一時的になるよう変更

  Revision 1.23  2006/07/06 11:20:52  okubata_ryoma
  undo

  Revision 1.20  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.19  2005/12/09 11:09:42  seiki_masashi
  ignoreSizePrecheckMode の判定を修正

  Revision 1.18  2005/12/01 13:58:14  seiki_masashi
  ignoreSizePrecheckMode の追加
  maxFrequency の追加
  送受信サイズ関連の処理の整理

  Revision 1.17  2005/11/25 00:45:19  seiki_masashi
  mask を WMMPParam 構造体内に移動

  Revision 1.16  2005/11/24 03:05:00  seiki_masashi
  WM_SetMPParameter 関数と関連する関数を追加
  WM_StartMP(Ex) 関数を WMMPParam 構造体を使用するように変更

  Revision 1.15  2005/11/22 09:02:06  seiki_masashi
  MATH_CountPopulation 関数を利用するように変更

  Revision 1.14  2005/04/11 02:06:54  seiki_masashi
  small fix

  Revision 1.13  2005/04/11 01:25:25  seiki_masashi
  WM_StartMP 時に子機のときだけ省電力モードの確認をするように変更

  Revision 1.12  2005/03/08 06:45:26  terui
  パワーセーブモードでないとMP開始を制限するチェックを追加。

  Revision 1.11  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.10  2005/02/14 06:40:04  terui
  minPollBmpModeの状態により、データ転送所要時間をチェックするタイミングを変更。

  Revision 1.9  2005/01/11 07:46:10  takano_makoto
  fix copyright header.

  Revision 1.8  2005/01/06 02:04:44  seiki_masashi
  WM_StartMP, WM_SetMPData, WM_SetMPDataToPort の inline 化
  若干のコードの最適化

  Revision 1.7  2004/11/29 04:55:35  seiki_masashi
  fix typo

  Revision 1.6  2004/11/29 04:40:57  seiki_masashi
  SetMPDataToPort の sendData が StartMP の sendBuf と等しい場合はエラーに

  Revision 1.5  2004/10/22 04:37:28  terui
  WMErrCodeとしてint型を返していた関数について、WMErrCode列挙型を返すように変更。

  Revision 1.4  2004/10/05 09:25:45  seiki_masashi
  WM_SetMPData* の引数チェックコードを追加

  Revision 1.3  2004/10/03 07:49:37  ooe
  ipl_branchとマージ。 ignoreFatalErrorフラグに対応。

  Revision 1.2  2004/09/13 04:10:20  seiki_masashi
  WM_SetMPFrequency の追加

  Revision 1.1  2004/09/10 11:03:30  terui
  wm.cの分割に伴い、新規upload。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    内部関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WMi_StartMP

  Description:  MP通信を開始する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf     -   受信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize -   受信データ格納バッファのサイズ。
                                WM_GetReceiveBufferSize() の返り値よりも大きい必要がある。
                sendBuf     -   送信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                sendBufSize -   送信データ格納バッファのサイズ。
                                WM_GetSendBufferSize() の返り値よりも大きい必要がある。
                tmpParam    -   MP の開始前に設定する MP 通信の一時パラメータの構造体。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartMP(WMCallbackFunc callback,
                      u16 *recvBuf,
                      u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, WMMPTmpParam* tmpParam)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // ステートチェック
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    WM_CHECK_RESULT(result);

    // 子機ではパワーセーブモードでない場合は MP開始禁止
    DC_InvalidateRange(&(status->aid), 2);
    DC_InvalidateRange(&(status->pwrMgtMode), 2);
    if (status->aid != 0 && status->pwrMgtMode != 1)
    {
        WM_WARNING("Could not start MP. Power save mode is illegal.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // MP状態確認
    DC_InvalidateRange(&(status->mp_flag), 4);  // ARM7ステータス領域のキャッシュを無効化
    if (status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // パラメータチェック
    if ((recvBufSize & 0x3f) != 0)     // recvBufSize/2 が 32Byte の倍数
    {
        WM_WARNING("Parameter \"recvBufSize\" is not a multiple of 64.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((sendBufSize & 0x1f) != 0)     // sendBufSize が 32Byte の倍数
    {
        WM_WARNING("Parameter \"sendBufSize\" is not a multiple of 32.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)recvBuf & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"recvBuf\" is not 32-byte aligned.\n");
    }
    if ((u32)sendBuf & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"sendBuf\" is not 32-byte aligned.\n");
    }

    DC_InvalidateRange(&(status->mp_ignoreSizePrecheckMode),
                       sizeof(status->mp_ignoreSizePrecheckMode));
    if (status->mp_ignoreSizePrecheckMode == FALSE)
    {
        // 送受信サイズの事前チェックを行う
        if (recvBufSize < WM_GetMPReceiveBufferSize())
        {
            WM_WARNING("Parameter \"recvBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if (sendBufSize < WM_GetMPSendBufferSize())
        {
            WM_WARNING("Parameter \"sendBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
#ifndef SDK_FINALROM
        // データ転送所要時間を確認
        DC_InvalidateRange(&(status->state), 2);
        if (status->state == WM_STATE_PARENT)
        {
            DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
            (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                                status->pparam.childMaxSize,
                                                (u8)(status->pparam.maxEntry));
        }
#endif
    }

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_START_MP, callback);

    // ARM7にFIFOで通知
    {
        WMStartMPReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_START_MP;
        Req.recvBuf = (u32 *)recvBuf;
        Req.recvBufSize = (u32)(recvBufSize / 2);       // バッファ１つ分のサイズ
        Req.sendBuf = (u32 *)sendBuf;
        Req.sendBufSize = (u32)sendBufSize;

        MI_CpuClear32(&Req.param, sizeof(Req.param)); // unused
        MI_CpuCopy32(tmpParam, &Req.tmpParam, sizeof(Req.tmpParam));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartMPEx

  Description:  MP通信を開始する。詳細な動作モードが指定可能。親機・子機共通関数。

  Arguments:    callback        -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf         -   受信データ格納バッファ。
                                    ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize     -   受信データ格納バッファのサイズ。
                                    WM_GetReceiveBufferSize() の返り値よりも大きい必要がある。
                sendBuf         -   送信データ格納バッファ。
                                    ARM7が直接データを書き出すので、キャッシュに注意。
                sendBufSize     -   送信データ格納バッファのサイズ。
                                    WM_GetSendBufferSize() の返り値よりも大きい必要がある。
                mpFreq          -   1フレームに何回 MP 通信をするか。
                                    0 で連続送信モード。親機のみ意味を持つ。
                defaultRetryCount - 0～7番ポートの通信の際に、送信に失敗した場合に
                                    リトライを行う標準の回数。0 でリトライを行わない。
                minPollBmpMode  -   MP 通信時の pollBitmap を送信する packet の宛先に応じて
                                    最低限の集合に抑える動作モード。
                singlePacketMode -  1 回の MP について一つの packet しか送らなくなる
                                    特殊な動作モード。
                fixFreqMode     -   リトライによる MP 通信の増加を禁止する特殊な動作モード。
                                    1フレーム中の MP 通信の回数をちょうど mpFreq 回に固定。

                ignoreFatalError -  FatalError発生時にAutoDisconnectを行なわなくなる。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartMPEx(WMCallbackFunc callback,
             u16 *recvBuf,
             u16 recvBufSize,
             u16 *sendBuf,
             u16 sendBufSize,
             u16 mpFreq,
             u16 defaultRetryCount,
             BOOL minPollBmpMode, BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError)
{
    WMMPTmpParam tmpParam;

    MI_CpuClear32(&tmpParam, sizeof(tmpParam));

    tmpParam.mask = WM_MP_TMP_PARAM_MIN_FREQUENCY | WM_MP_TMP_PARAM_FREQUENCY | WM_MP_TMP_PARAM_DEFAULT_RETRY_COUNT
        | WM_MP_TMP_PARAM_MIN_POLL_BMP_MODE | WM_MP_TMP_PARAM_SINGLE_PACKET_MODE |
        WM_MP_TMP_PARAM_IGNORE_FATAL_ERROR_MODE;
    tmpParam.minFrequency = mpFreq;
    tmpParam.frequency = mpFreq;
    tmpParam.defaultRetryCount = defaultRetryCount;
    tmpParam.minPollBmpMode = (u8)minPollBmpMode;
    tmpParam.singlePacketMode = (u8)singlePacketMode;
    tmpParam.ignoreFatalErrorMode = (u8)ignoreFatalError;

    if (fixFreqMode != FALSE && mpFreq != 0)
    {
        tmpParam.mask |= WM_MP_TMP_PARAM_MAX_FREQUENCY;
        tmpParam.maxFrequency = mpFreq;
    }

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &tmpParam);
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartMP

  Description:  MP通信を開始する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                recvBuf     -   受信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                recvBufSize -   受信データ格納バッファのサイズ。
                                WM_GetReceiveBufferSize() の返り値よりも大きい必要がある。
                sendBuf     -   送信データ格納バッファ。
                                ARM7が直接データを書き出すので、キャッシュに注意。
                sendBufSize -   送信データ格納バッファのサイズ。
                                WM_GetSendBufferSize() の返り値よりも大きい必要がある。
                mpFreq      -   1フレームに何回 MP 通信をするか。
                                0 で連続送信モード。親機のみ意味を持つ。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMP(WMCallbackFunc callback,
                     u16 *recvBuf, u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, u16 mpFreq)
{
    WMMPTmpParam tmpParam;

    MI_CpuClear32(&tmpParam, sizeof(tmpParam));

    tmpParam.mask = WM_MP_TMP_PARAM_FREQUENCY | WM_MP_TMP_PARAM_MIN_FREQUENCY;
    tmpParam.minFrequency = mpFreq;
    tmpParam.frequency = mpFreq;

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &tmpParam);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParameter

  Description:  MP 通信用の各種パラメータを設定する

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                param       -   MP 通信用のパラメータを格納した構造体へのポインタ

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParameter(WMCallbackFunc callback, const WMMPParam * param)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // ステートチェック
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_SET_MP_PARAMETER, callback);

    // ARM7にFIFOで通知
    {
        WMSetMPParameterReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_SET_MP_PARAMETER;
        MI_CpuCopy32(param, &Req.param, sizeof(Req.param));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPChildSize

  Description:  1回のMP通信での子機の送信バイト数を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                childSize   -   子機の送信バイト数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPChildSize(WMCallbackFunc callback, u16 childSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_CHILD_SIZE;
    param.childSize = childSize;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParentSize

  Description:  1回のMP通信での親機の送信バイト数を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                childSize   -   親機の送信バイト数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParentSize(WMCallbackFunc callback, u16 parentSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_SIZE;
    param.parentSize = parentSize;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPFrequency

  Description:  1フレームに何回 MP 通信するかを切り替える。親機用関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                mpFreq      -   1フレームに何回 MP 通信をするか。
                                0 で連続送信モード。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPFrequency(WMCallbackFunc callback, u16 mpFreq)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_FREQUENCY | WM_MP_PARAM_MIN_FREQUENCY;
    param.minFrequency = mpFreq;
    param.frequency = mpFreq;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPInterval

  Description:  1フレーム中に連続して MP 通信を行う場合の間隔を設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                parentInterval - 1フレーム中に連続して行う場合の親機の MP 通信間隔 (ms)
                childInterval  - 1フレーム中に連続して行う場合の子機が MP 通信を準備する間隔 (ms)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPInterval(WMCallbackFunc callback, u16 parentInterval, u16 childInterval)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_INTERVAL | WM_MP_PARAM_CHILD_INTERVAL;
    param.parentInterval = parentInterval;
    param.childInterval = childInterval;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPTiming

  Description:  フレーム同期通信モード時に MP 通信の準備を開始するタイミングを設定する。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                parentVCount -  フレーム同期通信時の親機処理の開始 V Count
                childVCount  -  フレーム同期通信時の子機処理の開始 V Count

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPTiming(WMCallbackFunc callback, u16 parentVCount, u16 childVCount)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_VCOUNT | WM_MP_PARAM_CHILD_VCOUNT;
    param.parentVCount = parentVCount;
    param.childVCount = childVCount;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPortEx

  Description:  MP通信にて送信するデータを予約する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。
                arg         -   callback に渡される引数
                sendData    -   送信予約するデータへのポインタ。
                                このデータの実体は強制的にキャッシュストアされる点に注意。
                sendDataSize -  送信予約するデータのサイズ。
                destBitmap  -   送信先子機を示す aid の bitmap を指定。
                port        -   送信するポート番号。
                prio        -   優先度 (0:最高～3:最低)

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode
WM_SetMPDataToPortEx(WMCallbackFunc callback, void *arg, const u16 *sendData, u16 sendDataSize,
                     u16 destBitmap, u16 port, u16 prio)
{
    WMErrCode result;
    BOOL    isParent;
    u16     mpReadyBitmap = 0x0001;
    u16     childBitmap = 0x0001;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // ステートチェック
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // 必要な情報をWMStatus構造体から取得
    DC_InvalidateRange(&(status->aid), 2);
    isParent = (status->aid == 0) ? TRUE : FALSE;
    if (isParent == TRUE)
    {
        DC_InvalidateRange(&(status->child_bitmap), 2);
        childBitmap = status->child_bitmap;
        DC_InvalidateRange(&(status->mp_readyBitmap), 2);
        mpReadyBitmap = status->mp_readyBitmap;
    }

    // パラメータチェック
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (childBitmap == 0)
    {
        WM_WARNING("There is no child connected.\n");
        return WM_ERRCODE_NO_CHILD;
    }
    if ((u32)sendData & 0x01f)
    {
        // アラインチェックは警告のみでエラーにはしない
        WM_WARNING("Parameter \"sendData\" is not 32-byte aligned.\n");
    }
    DC_InvalidateRange(&(status->mp_sendBuf), 2);
    if ((void *)sendData == (void *)status->mp_sendBuf)
    {
        WM_WARNING
            ("Parameter \"sendData\" must not be equal to the WM_StartMP argument \"sendBuf\".\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // 送信サイズチェック
    if (sendDataSize > WM_SIZE_MP_DATA_MAX)
    {
        WM_WARNING("Parameter \"sendDataSize\" is over limit.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    if (sendDataSize == 0)
    {
        WM_WARNING("Parameter \"sendDataSize\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

#ifndef SDK_FINALROM
    // データ転送所要時間を確認
    DC_InvalidateRange(&(status->mp_current_minPollBmpMode), 2);
    if (isParent && status->mp_current_minPollBmpMode)
    {
        DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
        (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                            status->pparam.childMaxSize,
                                            (u8)MATH_CountPopulation((u32)destBitmap));
    }
#endif

    // 指定バッファのキャッシュを書き出し
    DC_StoreRange((void *)sendData, sendDataSize);

    // コールバック関数を登録する必要なし

    // ARM7にFIFOで通知
    result = WMi_SendCommand(WM_APIID_SET_MP_DATA, 7,
                             (u32)sendData,
                             (u32)sendDataSize,
                             (u32)destBitmap, (u32)port, (u32)prio, (u32)callback, (u32)arg);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndMP

  Description:  MP通信を停止する。親機・子機共通関数。

  Arguments:    callback    -   非同期処理が完了した時に呼び出されるコールバック関数。

  Returns:      WMErrCode   -   処理結果を返す。非同期処理が正常に開始された場合に
                                WM_ERRCODE_OPERATINGが返され、その後コールバックに
                                非同期処理の結果が改めて渡される。
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndMP(WMCallbackFunc callback)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // ステートチェック
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // MP状態確認
    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == FALSE)
    {
        WM_WARNING("It is not MP mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // コールバック関数を登録
    WMi_SetCallbackTable(WM_APIID_END_MP, callback);

    // ARM7にFIFOで通知
    result = WMi_SendCommand(WM_APIID_END_MP, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
