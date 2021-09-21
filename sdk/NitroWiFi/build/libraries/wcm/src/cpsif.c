/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - WCM - libraries
  File:     cpsif.c

  Copyright 2005-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cpsif.c,v $
  Revision 1.7.4.2  2007/09/18 09:23:11  okubata_ryoma
  Copyright fix

  Revision 1.7.4.1  2007/09/18 09:18:05  okubata_ryoma
  HEADとマージ

  Revision 1.7  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.6  2005/09/01 23:45:02  terui
  WcmCpsifKACallback関数にて引数未使用警告が発生しないように改造

  Revision 1.5  2005/09/01 13:05:06  terui
  Keep Alive 用 NULL パケット送信機能を追加
  データ送信時に Keep Alive パケット送信アラームをリセットする機能を追加

  Revision 1.4  2005/08/10 10:39:24  adachi_hiroaki
  エラーメッセージの引数を修正

  Revision 1.3  2005/08/08 11:15:49  terui
  WCM_GetApEssid 関数を追加

  Revision 1.2  2005/07/11 11:42:04  yasu
  パディング警告をなくすためにパディング追加

  Revision 1.1  2005/07/07 10:45:52  terui
  新規追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_cpsif.h"

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
#if WCM_DEBUG

// 警告文用テキスト雛形
static const char   cpsifWarningText_NotInit[] = { "WCM library is not initialized yet.\n" };
static const char   cpsifWarningText_IllegalParam[] = { "Illegal parameter ( %s )\n" };
static const char   cpsifWarningText_InIrqMode[] = { "Processor mode is IRQ mode now.\n" };

// 報告文用テキスト雛形
static const char   cpsifReportText_WmSyncError[] = { "%s failed syncronously. ( ERRCODE: 0x%02x )\n" };
static const char   cpsifReportText_WmAsyncError[] = { "%s failed asyncronously. ( ERRCODE: 0x%02x )\n" };
#endif

#define WCMCPSIF_DUMMY_IRQ_THREAD   ((OSThread*)OS_IrqHandler)

/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/

// 内部状態管理用構造体
typedef struct WCMCpsifWork
{
    u8  initialized;                // ワーク変数初期化確認用
    u8  reserved[3];                // パディング  (OSThreadQueue が4byte以上に)
    OSThreadQueue   sendQ;          // DCF 送信完了待ちブロック用 Thread Queue
    OSMutex         sendMutex;      // 複数スレッドによる送信の排他制御用 Mutex
    WMErrCode       sendResult;     // DCF 送信の非同期的な処理結果退避エリア
    WCMRecvInd      recvCallback;   // DCF 受信コールバック先退避エリア
} WCMCpsifWork;

/*---------------------------------------------------------------------------*
    内部変数定義
 *---------------------------------------------------------------------------*/

/* 
 * ThreadQueue や Mutex など、WCMライブラリが終了される際に解放されてしまうと
 * 困る変数群は static な変数とする。これらの変数は一度初期化されると２度と
 * 解放されたり再初期化されたりしない。
 */
static WCMCpsifWork wcmCpsifw;

/*---------------------------------------------------------------------------*
    内部関数プロトタイプ
 *---------------------------------------------------------------------------*/
static void         WcmCpsifWmCallback(void* arg);
static void         WcmCpsifKACallback(void* arg);
static BOOL         WcmCpsifTryLockMutexInIRQ(OSMutex* mutex);
static void         WcmCpsifUnlockMutexInIRQ(OSMutex* mutex);

/*---------------------------------------------------------------------------*
  Name:         WCMi_InitCpsif

  Description:  CPS ライブラリとのインターフェースとなる部分について、ワーク
                変数群を初期化する。一度初期化したら再初期化はしない。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_InitCpsif(void)
{
    if (wcmCpsifw.initialized == 0)
    {
        wcmCpsifw.initialized = 1;
        wcmCpsifw.sendResult = WM_ERRCODE_SUCCESS;

        /* wcmCpsifw.recvCallback は変更されている可能性があるので初期化しない */
        OS_InitThreadQueue(&(wcmCpsifw.sendQ));
        OS_InitMutex(&(wcmCpsifw.sendMutex));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifRecvCallback

  Description:  無線ドライバが DCF フレームを受信した際に呼び出される関数。
                割り込みサービス内で呼び出され、CPS の受信コールバック関数を
                呼び出す。

  Arguments:    recv    -   受信した DCF フレーム構造体へのポインタが渡される。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifRecvCallback(WMDcfRecvBuf* recv)
{
    if (wcmCpsifw.recvCallback != NULL)
    {
        wcmCpsifw.recvCallback((u8 *) (recv->srcAdrs), (u8 *) (recv->destAdrs), (u8 *) (recv->data), (s32) (recv->length));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCMi_CpsifSendNullPacket

  Description:  キープアライブ NULL パケット を送信する。
                他の DCF パケットを送信中の場合は何も行わない。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifSendNullPacket(void)
{
    volatile WCMWork*   w = WCMi_GetSystemWork();

    // WCM ライブラリの内部状態を確認
    if (w == NULL)
    {
        return;
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        return;
    }

    // 排他ロック
    if (FALSE == WcmCpsifTryLockMutexInIRQ(&(wcmCpsifw.sendMutex)))
    {
        return;
    }

    // 送信要求発行
    if (WM_ERRCODE_OPERATING != WM_SetDCFData(WcmCpsifKACallback, (const u8*)w->bssDesc.bssid, (const u16*)(w->sendBuf),
        0))
    {
        WcmCpsifUnlockMutexInIRQ(&(wcmCpsifw.sendMutex));
    }
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetApMacAddress

  Description:  無線ドライバが接続を確立している AP の MAC アドレスを取得する。
    NOTICE:     この関数によって取得したポインタが示すバッファの内容は、割り込み
                等による状態変化に伴って変更されたり、バッファ自身が解放されたり
                する可能性がある点に注意。割り込み禁止した状態で呼び出し、内容を
                別バッファに退避することを推奨する。

  Arguments:    None.

  Returns:      u8*     -   接続を確立している AP の MAC アドレスへのポインタを
                            返す。接続が確立されていない場合には NULL を返す。
 *---------------------------------------------------------------------------*/
u8* WCM_GetApMacAddress(void)
{
    u8*         mac = NULL;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化確認
    if (w != NULL)
    {
        // WCM ライブラリの状態確認
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* 無線ドライバにおいてコネクションが確立されている状態 */
            mac = w->bssDesc.bssid;
        }
    }

    (void)OS_RestoreInterrupts(e);
    return mac;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_GetApEssid

  Description:  無線ドライバが接続を確立している AP の ESS-ID を取得する。
    NOTICE:     この関数によって取得したポインタが示すバッファの内容は、割り込み
                等による状態変化に伴って変更されたり、バッファ自身が解放されたり
                する可能性がある点に注意。割り込み禁止した状態で呼び出し、内容を
                別バッファに退避することを推奨する。

  Arguments:    length  -   ESS-ID の長さを取得する u16 型変数へのポインタを指定
                            する。接続が確立されていない場合には 0 が格納される。
                            長さを取得する必要がない場合には、この引数に NULL を
                            指定してもよい。

  Returns:      u8*     -   接続を確立している AP の ESS-ID データ列へのポインタ
                            を返す。接続が確立されていない場合には NULL を返す。
 *---------------------------------------------------------------------------*/
u8* WCM_GetApEssid(u16* length)
{
    u8*         essid = NULL;
    u16         len = 0;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

    // 初期化確認
    if (w != NULL)
    {
        // WCM ライブラリの状態確認
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* 無線ドライバにおいてコネクションが確立されている状態 */
            essid = &(w->bssDesc.ssid[0]);
            len = w->bssDesc.ssidLength;
        }
    }

    (void)OS_RestoreInterrupts(e);
    if (length != NULL)
    {
        *length = len;
    }

    return essid;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SetRecvDCFCallback

  Description:  CPS の受信コールバック関数を登録する。
                無線ドライバが DCF フレームを受信する度にここで登録した受信
                コールバック関数が呼び出される。
    NOTICE:     ここで登録したコールバック関数がオーバーレイ内にある場合には、
                飛び先がメモリに存在しなくなる前に登録しているコールバック関数を
                NULL で上書きクリアする必要がある点に注意。

  Arguments:    callback    -   DCF 受信コールバック関数を指定する。
                                NULL を指定すると受信通知されなくなる。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WCM_SetRecvDCFCallback(WCMRecvInd callback)
{
    OSIntrMode  e = OS_DisableInterrupts();

    wcmCpsifw.recvCallback = callback;

    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SendDCFData

  Description:  無線ドライバに DCF フレーム送信を指示する。DCF フレームの送信
                完了、もしくは失敗するまで内部でブロックして待つ。また、無線の
                接続状態が DCF フレームを送信できない状態の場合には失敗する。
    NOTICE:     複数のスレッドから非同期に呼び出されることを想定しているが、
                割り込みサービス内で呼び出されることは想定していない点に注意。

  Arguments:    dstAddr -   DCF フレームのあて先となる MAC アドレスを指定する。
                buf     -   送信するデータへのポインタを指定する。
                            当関数から戻るまでは指定したバッファの内容は保持
                            しておく必要がある点に注意。
                len     -   送信するデータの長さをバイト単位で指定する。

  Returns:      s32     -   送信に成功した場合に送信されたデータ長を返す。
                            失敗した場合には負値を返す。
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFData(const u8* dstAddr, const u8* buf, s32 len)
{
    OSIntrMode          e = OS_DisableInterrupts();
    volatile WCMWork*   w = WCMi_GetSystemWork();

    // 初期化済みを確認
    if (w == NULL)
    {
        WCMi_Warning(cpsifWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // WCM ライブラリが初期化されていない
    }

#if WCM_DEBUG
    // パラメータ確認
    if ((dstAddr == NULL) || (buf == NULL))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "dstAddr or buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // ポインタが NULL
    }

    if (len <= 0)
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len が 0 以下
    }

    if (len > WCM_ROUNDUP32(WM_DCF_MAX_SIZE))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len が サイズ超過
    }

    if ((u32) buf & 0x01)
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // buf が奇数アドレス
    }

    // 割り込みサービス内かどうかを確認
    if (OS_GetProcMode() == HW_PSR_IRQ_MODE)
    {
        WCMi_Warning(cpsifWarningText_InIrqMode);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_IN_IRQ_MODE;        // IRQ モードでの呼び出しは禁止
    }
#endif

    // 別スレッドが排他送信ブロック中の場合は解除されるのを待つ
    OS_LockMutex(&(wcmCpsifw.sendMutex));

    /*
     * ブロック状態から復帰した時点で、WCMライブラリの状態は不明。
     * AP と接続されている状態でないと送信はできないため、ここで WCM ライブラリの
     * 状態を確認する必要がある。
     */
    w = WCMi_GetSystemWork();
    if (w == NULL)
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // 寝ている間に WCM ライブラリが終了してしまった場合
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_NO_CONNECTION;      // 送信しようとした時に AP との接続が確立していない場合
    }

    /*
     * 送信データをキャッシュセーフなバッファに一旦コピーする。
     * 複数のスレッドからの送信要求は排他的に処理されるので、コピー先のキャッシュセーフな
     * バッファは一回の送信分だけあれば良いはず。
     */
    MI_CpuCopy8(buf, (void*)(w->sendBuf), (u32) len);

    // 送信要求発行
    {
        WMErrCode   wmResult;

        wmResult = WM_SetDCFData(WcmCpsifWmCallback, (const u8*)dstAddr, (const u16*)(w->sendBuf), (u16) len);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FIFO_ERROR:
        default:
            // 送信要求に失敗した場合
            WCMi_Printf(cpsifReportText_WmSyncError, "WM_SetDCFData", wmResult);
            OS_UnlockMutex(&(wcmCpsifw.sendMutex));
            (void)OS_RestoreInterrupts(e);
            return WCM_CPSIF_RESULT_SEND_FAILURE;   // DCF 送信に失敗
        }
    }

    // 送信完了待ちブロック
    OS_SleepThread(&(wcmCpsifw.sendQ));

    /*
     * ブロック状態から復帰した時点で、WCMライブラリの状態は不明。
     * WCM ライブラリが終了されていても、送信結果は保持されているので
     * WCM ライブラリの状態を確認する必要はない。
     */
    switch (wcmCpsifw.sendResult)
    {
    case WM_ERRCODE_SUCCESS:
        break;

    case WM_ERRCODE_FAILED:
    default:
        // 送信に失敗した場合
        WCMi_Printf(cpsifReportText_WmAsyncError, "WCM_SendDCFData", wcmCpsifw.sendResult);
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_SEND_FAILURE;       // DCF 送信に失敗
    }

    // 正常終了
    OS_UnlockMutex(&(wcmCpsifw.sendMutex));
    (void)OS_RestoreInterrupts(e);
    return len;
}

/*---------------------------------------------------------------------------*
  Name:         WCM_SendDCFDataEx

  Description:  無線ドライバに DCF フレーム送信を指示する。DCF フレームの送信
                完了、もしくは失敗するまで内部でブロックして待つ。また、無線の
                接続状態が DCF フレームを送信できない状態の場合には失敗する。
    NOTICE:     複数のスレッドから非同期に呼び出されることを想定しているが、
                割り込みサービス内で呼び出されることは想定していない点に注意。

  Arguments:    dstAddr     -   DCF フレームのあて先となる MAC アドレスを指定する。
                header      -   送信するデータのヘッダ部分へのポインタを指定する。
                headerLen   -   送信するデータのヘッダの長さをバイト単位で指定する。
                body        -   送信するデータのボディ部分へのポインタを指定する。
                bodyLen     -   送信するデータのボディ部分の長さをバイト単位で指定する。
                
                * header、body に関しては、当関数から戻るまでは指定したバッファの内容は
                  保持しておく必要がある点に注意。

  Returns:      s32     -   送信に成功した場合に送信されたデータ長を返す。
                            失敗した場合には負値を返す。
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFDataEx(const u8* dstAddr, const u8* header, s32 headerLen,
                      const u8* body, s32 bodyLen)
{
    OSIntrMode          e = OS_DisableInterrupts();
    volatile WCMWork*   w = WCMi_GetSystemWork();

    // 初期化済みを確認
    if (w == NULL)
    {
        WCMi_Warning(cpsifWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // WCM ライブラリが初期化されていない
    }

#if WCM_DEBUG
    // パラメータ確認
    if ((dstAddr == NULL) || (header == NULL))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "dstAddr or header");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // header のポインタが NULL
    }

    if (headerLen <= 0 || (body != NULL && bodyLen < 0) || (body == NULL && bodyLen != 0))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "headerLen or bodyLen");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // headerLen が 0 以下、または bodyLen が 0 未満
    }

    if (headerLen + bodyLen > WCM_ROUNDUP32(WM_DCF_MAX_SIZE))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "headerLen + bodyLen");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // (headerLen + bodyLen) が サイズ超過
    }

    if ((u32) header & 0x01 || ((body != NULL) && ((u32) body & 0x01)))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "header or body");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // header または body のバッファが奇数アドレス
    }

    // 割り込みサービス内かどうかを確認
    if (OS_GetProcMode() == HW_PSR_IRQ_MODE)
    {
        WCMi_Warning(cpsifWarningText_InIrqMode);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_IN_IRQ_MODE;        // IRQ モードでの呼び出しは禁止
    }
#endif

    // 別スレッドが排他送信ブロック中の場合は解除されるのを待つ
    OS_LockMutex(&(wcmCpsifw.sendMutex));

    /*
     * ブロック状態から復帰した時点で、WCMライブラリの状態は不明。
     * AP と接続されている状態でないと送信はできないため、ここで WCM ライブラリの
     * 状態を確認する必要がある。
     */
    w = WCMi_GetSystemWork();
    if (w == NULL)
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // 寝ている間に WCM ライブラリが終了してしまった場合
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_NO_CONNECTION;      // 送信しようとした時に AP との接続が確立していない場合
    }

    /*
     * 送信データをキャッシュセーフなバッファに一旦コピーする。
     * 複数のスレッドからの送信要求は排他的に処理されるので、コピー先のキャッシュセーフな
     * バッファは一回の送信分だけあれば良いはず。
     */

    /*
     * ヘッダ部分とボディ部分に分かれていたデータを、WCMWork の SendBuf にまとめてコピーする
     */
    MI_CpuCopy8(header, (void*)(w->sendBuf), (u32) headerLen);
    if (bodyLen > 0)
    {
        MI_CpuCopy8(body, (void*)(((u8*)w->sendBuf) + headerLen), (u32) bodyLen);
    }

    // 送信要求発行
    {
        WMErrCode   wmResult;

        wmResult = WM_SetDCFData(WcmCpsifWmCallback, (const u8*)dstAddr, (const u16*)(w->sendBuf), (u16) (headerLen + bodyLen));
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FIFO_ERROR:
        default:
            // 送信要求に失敗した場合
            WCMi_Printf(cpsifReportText_WmSyncError, "WM_SetDCFData", wmResult);
            OS_UnlockMutex(&(wcmCpsifw.sendMutex));
            (void)OS_RestoreInterrupts(e);
            return WCM_CPSIF_RESULT_SEND_FAILURE;   // DCF 送信に失敗
        }
    }

    // 送信完了待ちブロック
    OS_SleepThread(&(wcmCpsifw.sendQ));

    /*
     * ブロック状態から復帰した時点で、WCMライブラリの状態は不明。
     * WCM ライブラリが終了されていても、送信結果は保持されているので
     * WCM ライブラリの状態を確認する必要はない。
     */
    switch (wcmCpsifw.sendResult)
    {
    case WM_ERRCODE_SUCCESS:
        break;

    case WM_ERRCODE_FAILED:
    default:
        // 送信に失敗した場合
        WCMi_Printf(cpsifReportText_WmAsyncError, "WCM_SendDCFData", wcmCpsifw.sendResult);
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_SEND_FAILURE;       // DCF 送信に失敗
    }

    // 正常終了
    OS_UnlockMutex(&(wcmCpsifw.sendMutex));
    (void)OS_RestoreInterrupts(e);
    return headerLen + bodyLen;
}

/*---------------------------------------------------------------------------*
  Name:         WcmCpsifWmCallback

  Description:  DCF フレーム送信要求に対する無線ドライバからの非同期的な応答を
                受け取るコールバック関数。

  Arguments:    arg -   無線ドライバからの応答パラメータが格納されているバッファ
                        のアドレスが渡される。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifWmCallback(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->apiid)
    {
    case WM_APIID_SET_DCF_DATA:
        wcmCpsifw.sendResult = (WMErrCode) (cb->errcode);
        if (cb->errcode == WM_ERRCODE_SUCCESS)
        {
            WCMi_ResetKeepAliveAlarm();
        }

        OS_WakeupThread(&(wcmCpsifw.sendQ));
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WcmCpsifKACallback

  Description:  DCF フレーム送信要求に対する無線ドライバからの非同期的な応答を
                受け取るコールバック関数。キープアライブ NULL パケット送信時
                専用。通常のパケット送信とは排他的に送信制御を行うことで、
                一つしか登録できないコールバック関数を切り替えて使うことが可能。

  Arguments:    arg -   無線ドライバからの応答パラメータが格納されているバッファ
                        のアドレスが渡される。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifKACallback(void* arg)
{
#if WCM_DEBUG
    if (((WMCallback*)arg)->errcode == WM_ERRCODE_SUCCESS)
    {
        WCMi_Printf("Send NULL packet for KeepAlive.\n");
    }

#else
#pragma unused(arg)
#endif

    // 処理の成否に関わらず排他ロックを解除する
    WcmCpsifUnlockMutexInIRQ(&(wcmCpsifw.sendMutex));
}

/*---------------------------------------------------------------------------*
  Name:         WcmCpsifTryLockMutexInIRQ

  Description:  IRQ 割り込みサービス内において排他制御用 Mutex のロックを試みる。
                この関数でロックした Mutex は WcmCpsifUnlockMutexInIRQ 関数で
                のみアンロック可能。

  Arguments:    mutex   -   排他制御用 OSMutex 変数へのポインタを指定。

  Returns:      BOOL    -   ロックに成功した場合に TRUE を返す。既になんらかの
                            スレッドからロックされていた場合には FALSE を返す。
 *---------------------------------------------------------------------------*/
static BOOL WcmCpsifTryLockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if ((mutex == NULL) || (OS_GetProcMode() != HW_PSR_IRQ_MODE))
    {
        return FALSE;
    }
#endif
    if (mutex->thread == NULL)
    {
        mutex->thread = WCMCPSIF_DUMMY_IRQ_THREAD;
        mutex->count++;
        return TRUE;
    }
    else if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        mutex->count++;
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WcmCpsifUnlockMutexInIRQ

  Description:  IRQ 割り込みサービス内において排他制御用 Mutex のアンロックを
                試みる。WcmCpsifTryLockMutexInIRQ 関数でロックした Mutex に対
                してのみ有効。
                アンロックできなかった場合には、それを知る手段はない。

  Arguments:    mutex   -   排他制御用 OSMutex 変数へのポインタを指定。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifUnlockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if ((mutex == NULL) || (OS_GetProcMode() != HW_PSR_IRQ_MODE))
    {
        return;
    }
#endif
    if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        if (--(mutex->count) == 0)
        {
            mutex->thread = NULL;
            OS_WakeupThread(&(mutex->queue));
        }
    }
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
