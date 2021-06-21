/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     external.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: external.c,v $
  Revision 1.18  2007/09/24 23:49:43  yosizaki
  update copyright date.

  Revision 1.17  2007/08/06 00:49:44  okubata_ryoma
  small fix

  Revision 1.16  2007/07/31 02:15:14  tokunaga_eiji
  RTC_GetDateTimeExByTick 関数と付随する内部関数を追加。

  Revision 1.15  2006/07/05 09:12:56  yosizaki
  fix about include header.

  Revision 1.14  2006/01/18 02:11:20  kitase_hirotake
  do-indent

  Revision 1.13  2005/09/30 00:27:04  yada
  calculate day of week instead of reading rtc

  Revision 1.12  2005/02/28 05:26:07  yosizaki
  do-indent.

  Revision 1.11  2004/11/02 04:13:31  terui
  コメント修正。

  Revision 1.10  2004/08/19 13:43:01  terui
  HourFormat関連の定義を削除。

  Revision 1.9  2004/08/19 12:50:36  terui
  24時間表記固定化に伴う修正。

  Revision 1.8  2004/08/19 07:49:37  terui
  RTC_GetAlarmStatusAsync関数内の"Adjust"での上書き編集を元の状態に修正。

  Revision 1.7  2004/08/04 05:31:48  miya
  工場設定用に関数追加

  Revision 1.6  2004/07/01 03:16:52  yosizaki
  change around RTCHourFormat to internal use.

  Revision 1.5  2004/05/31 08:44:45  terui
  PXIコールバック関数のロジックを変更。

  Revision 1.4  2004/05/18 01:37:33  terui
  ARM7とのPXIの同期機構に対応

  Revision 1.3  2004/05/18 01:15:12  terui
  ARM7とのPXIの同期機構に対応

  Revision 1.2  2004/05/17 11:05:20  terui
  PXI処理結果をAPI専用処理結果に改造。
  非同期関数をBOOLから処理結果種別を返すように変更。

  Revision 1.1  2004/05/12 02:36:08  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/os.h>
#include <nitro/rtc.h>


/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
// 非同期関数排他処理用ロック定義
typedef enum RTCLock
{
    RTC_LOCK_OFF = 0,                  // ロック開錠状態
    RTC_LOCK_ON,                       // ロック施錠状態
    RTC_LOCK_MAX
}
RTCLock;

// 連続コマンド送信を含む処理用のシーケンス定義
typedef enum RTCSequence
{
    RTC_SEQ_GET_DATE = 0,              // 日付取得シーケンス
    RTC_SEQ_GET_TIME,                  // 時刻取得シーケンス
    RTC_SEQ_GET_DATETIME,              // 日付・時刻取得シーケンス
    RTC_SEQ_SET_DATE,                  // 日付設定シーケンス
    RTC_SEQ_SET_TIME,                  // 時刻設定シーケンス
    RTC_SEQ_SET_DATETIME,              // 日付・時刻設定シーケンス
    RTC_SEQ_GET_ALARM1_STATUS,         // アラーム１状態取得シーケンス
    RTC_SEQ_GET_ALARM2_STATUS,         // アラーム２状態取得シーケンス
    RTC_SEQ_GET_ALARM_PARAM,           // アラーム設定値取得シーケンス
    RTC_SEQ_SET_ALARM1_STATUS,         // アラーム１状態変更シーケンス
    RTC_SEQ_SET_ALARM2_STATUS,         // アラーム２状態変更シーケンス
    RTC_SEQ_SET_ALARM1_PARAM,          // アラーム１設定値変更シーケンス
    RTC_SEQ_SET_ALARM2_PARAM,          // アラーム２設定値変更シーケンス
    RTC_SEQ_SET_HOUR_FORMAT,           // 時間表記フォーマット変更シーケンス
    RTC_SEQ_SET_REG_STATUS2,           // ステータス２レジスタ書き込みシーケンス
    RTC_SEQ_SET_REG_ADJUST,            // アジャストレジスタ書き込みシーケンス
    RTC_SEQ_MAX
}
RTCSequence;

// ワーク用構造体
typedef struct RTCWork
{
    u32     lock;                      // 排他ロック
    RTCCallback callback;              // 非同期関数コールバック退避用
    void   *buffer[2];                 // 非同期関数パラメータ退避用
    void   *callbackArg;               // コールバック関数の引数保存用
    u32     sequence;                  // 連続処理モード管理用
    u32     index;                     // 連続処理の状況管理用
    RTCInterrupt interrupt;            // アラーム通知時の呼び出し関数退避用
    RTCResult commonResult;            // 非同期関数の処理結果退避用

}
RTCWork;

/*---------------------------------------------------------------------------*
    静的変数定義
 *---------------------------------------------------------------------------*/
static u16 rtcInitialized;             // 初期化確認フラグ
static RTCWork rtcWork;                // ワーク変数をまとめた構造体
static u16     rtcTickInitialized;     // チック初期化確認フラグ
static OSTick  rtcInitialTotalTicks;   // チック初期化時に保存される 2000/01/01
                                       // 00:00:00 からの総チック数。

/*---------------------------------------------------------------------------*
    内部関数定義
 *---------------------------------------------------------------------------*/
static void RtcCommonCallback(PXIFifoTag tag, u32 data, BOOL err);
static u32 RtcBCD2HEX(u32 bcd);
static u32 RtcHEX2BCD(u32 hex);
static BOOL RtcCheckAlarmParam(const RTCAlarmParam *param);
static RTCRawAlarm RtcMakeAlarmParam(const RTCAlarmParam *param);
static BOOL RtcCheckDate(const RTCDate *date, RTCRawDate *raw);
static BOOL RtcCheckTime(const RTCTime *time, RTCRawTime *raw);
static void RtcGetResultCallback(RTCResult result, void *arg);
static void RtcWaitBusy(void);
static RTCResult RtcTickInit(void);
static void RtcConvertTickToDateTimeEx(RTCDate * date, RTCTimeEx *time, OSTick tick);

/*---------------------------------------------------------------------------*
  Name:         RTC_Init

  Description:  RTCライブラリを初期化する。
       Notice:  コンポーネント側の初期化によって電源投入チェックが行われる。
                この際、RTCへの電源供給が一旦停止されていた場合はRTC内部状態は
                リセットされ、2000/01/01/00:00:00(12時間表記)0曜日(日曜日)になる。
                アラーム設定値も0クリアされる。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_Init(void)
{
    // 未初期化確認
    if (rtcInitialized)
    {
        return;
    }
    rtcInitialized = 1;

    // ワーク用変数初期化
    rtcWork.lock = RTC_LOCK_OFF;
    rtcWork.callback = NULL;
    rtcWork.interrupt = NULL;
    rtcWork.buffer[0] = NULL;
    rtcWork.buffer[1] = NULL;

    // ARM7のRTCライブラリが開始されるまで待つ
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_RTC, PXI_PROC_ARM7))
    {
    }

    // PXIコールバック関数を設定
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_RTC, RtcCommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateAsync

  Description:  RTCから日付データを非同期で読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateAsync(RTCDate *date, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_ASSERT(date != NULL);
    SDK_ASSERT(callback != NULL);

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 日付読み出しコマンドを送信
    rtcWork.sequence = RTC_SEQ_GET_DATE;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)date;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawDateAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDate

  Description:  RTCから日付データを読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDate(RTCDate *date)
{
    rtcWork.commonResult = RTC_GetDateAsync(date, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTimeAsync

  Description:  RTCから時刻データを非同期で読み出す。

  Arguments:    time      - 時刻データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTimeAsync(RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 時刻読み出しコマンドを送信
    rtcWork.sequence = RTC_SEQ_GET_TIME;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)time;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTime

  Description:  RTCから時刻データを読み出す。

  Arguments:    time      - 時刻データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTime(RTCTime *time)
{
    rtcWork.commonResult = RTC_GetTimeAsync(time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeAsync

  Description:  RTCから日付・時刻データを非同期で読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                time      - 時刻データを格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeAsync(RTCDate *date, RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 時刻読み出しコマンドを送信
    rtcWork.sequence = RTC_SEQ_GET_DATETIME;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)date;
    rtcWork.buffer[1] = (void *)time;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawDateTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTime

  Description:  RTCから日付・時刻データを読み出す。

  Arguments:    date      - 日付データを格納するバッファを指定。
                time      - 時刻データを格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTime(RTCDate *date, RTCTime *time)
{
    rtcWork.commonResult = RTC_GetDateTimeAsync(date, time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeExByTick

  Description:  Gets date and time by coverting total CPU ticks progressed
                since 2000/01/01/00:00:00. The total ticks are calculted by
                adding current ticks and initial total ticks. The initial
                ticks are calculated and saved at the first call. OS_InitTick
                must be called before calling this function.

  Arguments:    date      - buffer for storing RTCDate
                time      - buffer for storing RTCTimeEx

  Returns:      RTCResult - first call: result of initial RTC_GetDateTime
                            second call~: RTC_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeExByTick(RTCDate *date, RTCTimeEx *time)
{
    RTCResult result;
    OSTick totalTicks;
    
    if(!rtcTickInitialized){
        result = RtcTickInit();
    }else{
        result = RTC_RESULT_SUCCESS;
    }

    SDK_ASSERT(OS_IsTickAvailable());

    // culcurate total ticks progressed since 2000/01/01/00:00:00
    totalTicks = OS_GetTick() + rtcInitialTotalTicks;

    RtcConvertTickToDateTimeEx(date, time, totalTicks);
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateAsync

  Description:  RTCに日付データを非同期で書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateAsync(const RTCDate *date, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(callback);

    // セットする日付を確認、編集
    if (!RtcCheckDate(date, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 日付書き込みコマンドを送信
    rtcWork.sequence = RTC_SEQ_SET_DATE;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawDateAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDate

  Description:  RTCに日付データを書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDate(const RTCDate *date)
{
    rtcWork.commonResult = RTC_SetDateAsync(date, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTimeAsync

  Description:  RTCに時刻データを非同期で書き込む。

  Arguments:    time      - 時刻データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTimeAsync(const RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // セットする時刻を確認、編集
    if (!RtcCheckTime(time, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 時刻書き込みコマンドを送信
    rtcWork.sequence = RTC_SEQ_SET_TIME;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTime

  Description:  RTCに時刻データを書き込む。

  Arguments:    time      - 時刻データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTime(const RTCTime *time)
{
    rtcWork.commonResult = RTC_SetTimeAsync(time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTimeAsync

  Description:  RTCに日付・時刻データを非同期で書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                time      - 時刻データが格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetDateTimeAsync(const RTCDate *date, const RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // セットする日付・時刻を確認、編集
    if (!RtcCheckDate(date, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (!RtcCheckTime(time, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 日付・時刻書き込みコマンドを送信
    rtcWork.sequence = RTC_SEQ_SET_DATETIME;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawDateTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTime

  Description:  RTCに日付・時刻データを書き込む。

  Arguments:    date      - 日付データが格納されているバッファを指定。
                time      - 時刻データが格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTime(const RTCDate *date, const RTCTime *time)
{
    rtcWork.commonResult = RTC_SetDateTimeAsync(date, time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}




/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegStatus2Async

  Description:  RTCステータス２レジスタにデータを書き込む

  Arguments:    status2   - ステータス２レジスタ内容が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegStatus2Async(const RTCRawStatus2 *status2, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status2);
    SDK_NULL_ASSERT(callback);

    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.intr_mode = status2->intr_mode;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.intr2_mode =
        status2->intr2_mode;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.test = status2->test;

    /* パラメータチェック無し */
    // return RTC_RESULT_ILLEGAL_PARAMETER;


    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // ステータス２レジスタ書き込みコマンドを送信
    rtcWork.sequence = RTC_SEQ_SET_REG_STATUS2;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegStatus2

  Description:  RTCステータス２レジスタにデータを書き込む

  Arguments:    status2   - ステータス２レジスタ内容が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegStatus2(const RTCRawStatus2 *status2)
{
    rtcWork.commonResult = RTCi_SetRegStatus2Async(status2, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}


/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegAdjustAsync

  Description:  RTCアジャストレジスタにデータを書き込む

  Arguments:    adjust    - アジャストレジスタ内容が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegAdjustAsync(const RTCRawAdjust *adjust, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(adjust);
    SDK_NULL_ASSERT(callback);

    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.adjust.adjust = adjust->adjust;

    /* パラメータチェック無し */
    // return RTC_RESULT_ILLEGAL_PARAMETER;

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // ステータス２レジスタ書き込みコマンドを送信
    rtcWork.sequence = RTC_SEQ_SET_REG_ADJUST;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawAdjustAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}


/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegAdjust

  Description:  RTCアジャストレジスタにデータを書き込む

  Arguments:    status2   - アジャストレジスタ内容が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegAdjust(const RTCRawAdjust *Adjust)
{
    rtcWork.commonResult = RTCi_SetRegAdjustAsync(Adjust, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}



/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatusAsync

  Description:  RTCからアラームのON/OFF状態を非同期で読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態を格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult
RTC_GetAlarmStatusAsync(RTCAlarmChan chan, RTCAlarmStatus *status, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status);
    SDK_NULL_ASSERT(callback);

    // パラメータを確認
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // ステータス２レジスタ読み出しコマンドを送信
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        rtcWork.sequence = RTC_SEQ_GET_ALARM1_STATUS;
        break;
    case RTC_ALARM_CHAN_2:
        rtcWork.sequence = RTC_SEQ_GET_ALARM2_STATUS;
        break;
    }
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)status;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatus

  Description:  RTCからアラームのON/OFF状態を読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態を格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatus(RTCAlarmChan chan, RTCAlarmStatus *status)
{
    rtcWork.commonResult = RTC_GetAlarmStatusAsync(chan, status, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParamAsync

  Description:  RTCからアラームの設定値を非同期で読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値を格納するバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult
RTC_GetAlarmParamAsync(RTCAlarmChan chan, RTCAlarmParam *param, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(param);
    SDK_NULL_ASSERT(callback);

    // パラメータ確認
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // アラーム１or２設定値読み出しコマンドを送信
    rtcWork.sequence = RTC_SEQ_GET_ALARM_PARAM;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)param;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (chan == RTC_ALARM_CHAN_1)
    {
        if (RTCi_ReadRawAlarm1Async())
        {
            return RTC_RESULT_SUCCESS;
        }
        else
        {
            return RTC_RESULT_SEND_ERROR;
        }
    }
    if (RTCi_ReadRawAlarm2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParam

  Description:  RTCからアラームの設定値を読み出す。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値を格納するバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParam(RTCAlarmChan chan, RTCAlarmParam *param)
{
    rtcWork.commonResult = RTC_GetAlarmParamAsync(chan, param, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmInterrupt

  Description:  アラーム割込み発生時のコールバック関数を設定する。

  Arguments:    interrupt - コールバック関数を指定。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_SetAlarmInterrupt(RTCInterrupt interrupt)
{
    rtcWork.interrupt = interrupt;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatusAsync

  Description:  RTCにアラーム状態を非同期で書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetAlarmStatusAsync(RTCAlarmChan chan, const RTCAlarmStatus *status, RTCCallback callback,
                        void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status);
    SDK_NULL_ASSERT(callback);

    // パラメータ確認
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (*status > RTC_ALARM_STATUS_ON)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // ステータス２レジスタ読み出しコマンドを送信
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        rtcWork.sequence = RTC_SEQ_SET_ALARM1_STATUS;
        break;
    case RTC_ALARM_CHAN_2:
        rtcWork.sequence = RTC_SEQ_SET_ALARM2_STATUS;
        break;
    }
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)status;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatus

  Description:  RTCにアラーム状態を書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                status    - アラーム状態が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatus(RTCAlarmChan chan, const RTCAlarmStatus *status)
{
    rtcWork.commonResult = RTC_SetAlarmStatusAsync(chan, status, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParamAsync

  Description:  RTCにアラーム設定を非同期で書き込む。
       Notice:  RTCのアラーム状態がONになっていない場合、デバイス側でライトを
                受け付けてくれないので、書き込みは失敗する。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値が格納されているバッファを指定。
                callback  - 非同期処理が完了した際に呼び出す関数を指定。
                arg       - コールバック関数呼び出し時の引数を指定。

  Returns:      RTCResult - 非同期デバイス操作開始の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetAlarmParamAsync(RTCAlarmChan chan, const RTCAlarmParam *param, RTCCallback callback,
                       void *arg)
{
    OSIntrMode enabled;
    RTCRawAlarm *pAlarm = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm);
    BOOL    result = FALSE;

    SDK_NULL_ASSERT(param);
    SDK_NULL_ASSERT(callback);

    // パラメータを確認
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (!RtcCheckAlarmParam(param))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // ロック確認
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // 設定するデータを編集
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm = RtcMakeAlarmParam(param);
    // アラーム番号により送信コマンドを振り分け
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        // アラーム１レジスタ書き込みコマンドを送信
        rtcWork.sequence = RTC_SEQ_SET_ALARM1_PARAM;
        result = RTCi_WriteRawAlarm1Async();
        break;
    case RTC_ALARM_CHAN_2:
        // アラーム２レジスタ書き込みコマンドを送信
        rtcWork.sequence = RTC_SEQ_SET_ALARM2_PARAM;
        result = RTCi_WriteRawAlarm2Async();
        break;
    }
    if (result)
    {
        return RTC_RESULT_SUCCESS;
    }
    return RTC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParam

  Description:  RTCにアラーム設定値を書き込む。

  Arguments:    chan      - アラームのチャンネルを指定。
                param     - アラーム設定値が格納されているバッファを指定。

  Returns:      RTCResult - デバイス操作の処理結果を返す。
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParam(RTCAlarmChan chan, const RTCAlarmParam *param)
{
    rtcWork.commonResult = RTC_SetAlarmParamAsync(chan, param, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCommonCallback

  Description:  非同期RTC関数用の共通コールバック関数。

  Arguments:    tag -  PXI tag which show message type.
                data - message from ARM7.
                err -  PXI transfer error flag.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void RtcCommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag )

    RTCResult result;
    RTCPxiResult pxiresult;
    u8      command;
    RTCCallback cb;

    // PXI通信エラーを確認
    if (err)
    {
        // シーケンスを強制終了
        if (rtcWork.index)
        {
            rtcWork.index = 0;
        }
        if (rtcWork.lock != RTC_LOCK_OFF)
        {
            rtcWork.lock = RTC_LOCK_OFF;
        }
        if (rtcWork.callback)
        {
            cb = rtcWork.callback;
            rtcWork.callback = NULL;
            cb(RTC_RESULT_FATAL_ERROR, rtcWork.callbackArg);
        }
        return;
    }

    // 受信データを解析
    command = (u8)((data & RTC_PXI_COMMAND_MASK) >> RTC_PXI_COMMAND_SHIFT);
    pxiresult = (RTCPxiResult)((data & RTC_PXI_RESULT_MASK) >> RTC_PXI_RESULT_SHIFT);

    // アラーム割込みを確認
    if (command == RTC_PXI_COMMAND_INTERRUPT)
    {
        // pxiresultにてアラーム１or２を判断可能だが、
        // コールバックを統一しているのでどちらであるかは無視する。
        if (rtcWork.interrupt)
        {
            // アラーム割込み通知をコールバック
            rtcWork.interrupt();
        }
        return;
    }

    // 処理成功応答なら、内部状態に応じて各種事後処理
    if (pxiresult == RTC_PXI_RESULT_SUCCESS)
    {
        result = RTC_RESULT_SUCCESS;
        switch (rtcWork.sequence)
        {
            // 日付取得シーケンス
        case RTC_SEQ_GET_DATE:
            {
                RTCDate *pDst = (RTCDate *)(rtcWork.buffer[0]);
                RTCRawDate *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date);

                pDst->year = RtcBCD2HEX(pSrc->year);
                pDst->month = RtcBCD2HEX(pSrc->month);
                pDst->day = RtcBCD2HEX(pSrc->day);
                pDst->week = RTC_GetDayOfWeek(pDst);
            }
            break;
            // 時刻取得シーケンス
        case RTC_SEQ_GET_TIME:
            {
                RTCTime *pDst = (RTCTime *)(rtcWork.buffer[0]);
                RTCRawTime *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time);

                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->second = RtcBCD2HEX(pSrc->second);
            }
            break;
            // 日付・時刻取得シーケンス
        case RTC_SEQ_GET_DATETIME:
            {
                RTCDate *pDst = (RTCDate *)(rtcWork.buffer[0]);
                RTCRawDate *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date);

                //pDst->year =  RtcBCD2HEX( pSrc->year );   // なぜか値が渡されないので下のコードに変更
                pDst->year = RtcBCD2HEX(*(u32 *)pSrc & 0x000000ff);
                pDst->month = RtcBCD2HEX(pSrc->month);
                pDst->day = RtcBCD2HEX(pSrc->day);
                pDst->week = RTC_GetDayOfWeek(pDst);
            }
            {
                RTCTime *pDst = (RTCTime *)(rtcWork.buffer[1]);
                RTCRawTime *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time);

                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->second = RtcBCD2HEX(pSrc->second);
            }
            break;
            // 日付変更シーケンス
        case RTC_SEQ_SET_DATE:
        case RTC_SEQ_SET_TIME:
        case RTC_SEQ_SET_DATETIME:
            // 特に処理なし
            break;
            // アラーム１状態取得シーケンス
        case RTC_SEQ_GET_ALARM1_STATUS:
            {
                RTCAlarmStatus *pDst = (RTCAlarmStatus *)(rtcWork.buffer[0]);
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                switch (pSrc->intr_mode)
                {
                case RTC_INTERRUPT_MODE_ALARM:
                    *pDst = RTC_ALARM_STATUS_ON;
                    break;
                default:
                    *pDst = RTC_ALARM_STATUS_OFF;
                }
            }
            break;
            // アラーム２状態取得シーケンス
        case RTC_SEQ_GET_ALARM2_STATUS:
            {
                RTCAlarmStatus *pDst = (RTCAlarmStatus *)(rtcWork.buffer[0]);
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                if (pSrc->intr2_mode)
                {
                    *pDst = RTC_ALARM_STATUS_ON;
                }
                else
                {
                    *pDst = RTC_ALARM_STATUS_OFF;
                }
            }
            break;
            // アラーム１or２設定値取得シーケンス
        case RTC_SEQ_GET_ALARM_PARAM:
            {
                RTCAlarmParam *pDst = (RTCAlarmParam *)(rtcWork.buffer[0]);
                RTCRawAlarm *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm);

                pDst->week = (RTCWeek)(pSrc->week);
                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->enable = RTC_ALARM_ENABLE_NONE;
                if (pSrc->we)
                    pDst->enable += RTC_ALARM_ENABLE_WEEK;
                if (pSrc->he)
                    pDst->enable += RTC_ALARM_ENABLE_HOUR;
                if (pSrc->me)
                    pDst->enable += RTC_ALARM_ENABLE_MINUTE;
            }
            break;
            // アラーム１状態設定シーケンス
        case RTC_SEQ_SET_ALARM1_STATUS:
            if (rtcWork.index == 0)
            {
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                // ステータス２レジスタ読み出し結果
                if (*(RTCAlarmStatus *)(rtcWork.buffer[0]) == RTC_ALARM_STATUS_ON)
                {
                    // 割込み許可する場合
                    if (pSrc->intr_mode != RTC_INTERRUPT_MODE_ALARM)
                    {
                        // ステータス２レジスタ書き込み
                        rtcWork.index++;        // 次のシーケンスへ移行
                        pSrc->intr_mode = RTC_INTERRUPT_MODE_ALARM;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // シーケンスを中断
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
                else
                {
                    // 割込み禁止する場合
                    if (pSrc->intr_mode != RTC_INTERRUPT_MODE_NONE)
                    {
                        // ステータス２レジスタ書き込み
                        rtcWork.index++;        // 次のシーケンスへ移行
                        pSrc->intr_mode = RTC_INTERRUPT_MODE_NONE;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // シーケンスを中断
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
            }
            else
            {
                // ステータス２レジスタ書き込み結果
                rtcWork.index = 0;     // シーケンス終了
            }
            break;
            // アラーム２状態設定シーケンス
        case RTC_SEQ_SET_ALARM2_STATUS:
            if (rtcWork.index == 0)
            {
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                // ステータス２レジスタ読み出し結果
                if (*(RTCAlarmStatus *)(rtcWork.buffer[0]) == RTC_ALARM_STATUS_ON)
                {
                    // 割込み許可する場合
                    if (!pSrc->intr2_mode)
                    {
                        // ステータス２レジスタ書き込み
                        rtcWork.index++;        // 次のシーケンスへ移行
                        pSrc->intr2_mode = 1;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // シーケンスを中断
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
                else
                {
                    // 割込み禁止する場合
                    if (pSrc->intr2_mode)
                    {
                        // ステータス２レジスタ書き込み
                        rtcWork.index++;        // 次のシーケンスへ移行
                        pSrc->intr2_mode = 0;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // シーケンスを中断
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
            }
            else
            {
                // ステータス２レジスタ書き込み結果
                rtcWork.index = 0;     // シーケンス終了
            }
            break;
            // アラーム１パラメータ設定シーケンス
        case RTC_SEQ_SET_ALARM1_PARAM:
            // アラーム２パラメータ設定シーケンス
        case RTC_SEQ_SET_ALARM2_PARAM:
            // 時間表記変更シーケンス
        case RTC_SEQ_SET_HOUR_FORMAT:
            // ステータス２レジスタ書き込みシーケンス
        case RTC_SEQ_SET_REG_STATUS2:
            // アジャストレジスタ書き込みシーケンス
        case RTC_SEQ_SET_REG_ADJUST:
            // 特に処理なし
            break;

            // その他謎のシーケンス
        default:
            result = RTC_RESULT_INVALID_COMMAND;
            rtcWork.index = 0;
        }
    }
    else
    {
        // コマンドに失敗応答が返されたのでシーケンス中断
        rtcWork.index = 0;
        // PXI通信応答種別から処理結果を決定
        switch (pxiresult)
        {
        case RTC_PXI_RESULT_INVALID_COMMAND:
            result = RTC_RESULT_INVALID_COMMAND;
            break;
        case RTC_PXI_RESULT_ILLEGAL_STATUS:
            result = RTC_RESULT_ILLEGAL_STATUS;
            break;
        case RTC_PXI_RESULT_BUSY:
            result = RTC_RESULT_BUSY;
            break;
        case RTC_PXI_RESULT_FATAL_ERROR:
        default:
            result = RTC_RESULT_FATAL_ERROR;
        }
    }

    // 連続したシーケンスが完了ならば、シーケンスの終了処理
    if (rtcWork.index == 0)
    {
        // 排他ロック開錠
        if (rtcWork.lock != RTC_LOCK_OFF)
        {
            rtcWork.lock = RTC_LOCK_OFF;
        }
        // コールバック関数を呼び出し
        if (rtcWork.callback)
        {
            cb = rtcWork.callback;
            rtcWork.callback = NULL;
            cb(result, rtcWork.callbackArg);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         RtcBCD2HEX

  Description:  BCD型で表現された数値を一般的なu32で表現した数値に変換する。

  Arguments:    bcd  - BCD型で表現された数値。

  Returns:      u32  - 一般的なu32で表現し直した数値。
                       入力パラメータがBCD型でない場合、0を返す。
 *---------------------------------------------------------------------------*/
static u32 RtcBCD2HEX(u32 bcd)
{
    u32     hex = 0;
    s32     i;
    s32     w;

    // 0xA ~ 0xF がどこの桁にも含まれていないことを確認
    for (i = 0; i < 8; i++)
    {
        if (((bcd >> (i * 4)) & 0x0000000f) >= 0x0a)
        {
            return hex;                // 変換を中断して強制的に"0"を返す
        }
    }

    // 変換ループ
    for (i = 0, w = 1; i < 8; i++, w *= 10)
    {
        hex += (((bcd >> (i * 4)) & 0x0000000f) * w);
    }
    return hex;
}

/*---------------------------------------------------------------------------*
  Name:         RtcHEX2BCD

  Description:  一般的なu32で表現した数値をBCD型で表現された数値に変換する。

  Arguments:    hex  - 一般的なu32で表現した数値。

  Returns:      u32  - BCD型で表現し直した数値。
                       入力パラメータがBCD型で表現できない場合、0を返す。
 *---------------------------------------------------------------------------*/
static u32 RtcHEX2BCD(u32 hex)
{
    u32     bcd = 0;
    s32     i;
    u32     w;

    // 99999999を越えていないことを確認
    if (hex > 99999999)
    {
        return 0;
    }

    // 変換ループ
    for (i = 0, w = hex; i < 8; i++)
    {
        bcd += ((w % 10) << (i * 4));
        w /= 10;
    }
    return bcd;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckAlarmParam

  Description:  アラーム設定値がRTCにセットして問題ない値かどうかをチェックする。

  Arguments:    param  - チェックするアラーム設定値。

  Returns:      BOOL   - アラーム設定値として問題ない場合はTRUEを、何らかの
                         問題がある場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckAlarmParam(const RTCAlarmParam *param)
{
    if (param->week >= RTC_WEEK_MAX)
        return FALSE;
    if (param->hour >= 24)
        return FALSE;
    if (param->minute >= 60)
        return FALSE;
    if (param->enable & ~RTC_ALARM_ENABLE_ALL)
        return FALSE;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcMakeAlarmParam

  Description:  アラーム設定値をRTCにセットできる形に変換する。

  Arguments:    param  - 変換する対象となるアラーム設定値。

  Returns:      RTCRawAlarm - RTCにセットできる形に変換されたデータ。
 *---------------------------------------------------------------------------*/
static RTCRawAlarm RtcMakeAlarmParam(const RTCAlarmParam *param)
{
    RTCRawAlarm dst;

    // 戻り値を0クリア
    *((u32 *)(&dst)) = 0;

    // 念のため設定値の整合性を確認
    if (!RtcCheckAlarmParam(param))
    {
        return dst;
    }

    // 曜日データ
    dst.week = (u32)(param->week);
    // 時間データ、及び午前・午後フラグ
    if (param->hour >= 12)
    {
        dst.afternoon = 1;
    }
    dst.hour = RtcHEX2BCD(param->hour);
    // 分データ
    dst.minute = RtcHEX2BCD(param->minute);
    // 有効フラグ
    if (param->enable & RTC_ALARM_ENABLE_WEEK)
    {
        dst.we = 1;
    }
    if (param->enable & RTC_ALARM_ENABLE_HOUR)
    {
        dst.he = 1;
    }
    if (param->enable & RTC_ALARM_ENABLE_MINUTE)
    {
        dst.me = 1;
    }

    return dst;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckDate

  Description:  日付がRTCにセットして問題ない値かどうかをチェックする。
                問題なければRTCにセットできる形に編集する。

  Arguments:    date - チェックする日付を入力。
                raw  - RTCにセットできる形に編集されたデータを出力。

  Returns:      BOOL - チェックして問題がある場合にFALSEを返す。
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckDate(const RTCDate *date, RTCRawDate *raw)
{
    // 各メンバが許容範囲内かチェック
    if (date->year >= 100)
        return FALSE;
    if ((date->month < 1) || (date->month > 12))
        return FALSE;
    if ((date->day < 1) || (date->day > 31))
        return FALSE;
    if (date->week >= RTC_WEEK_MAX)
        return FALSE;

    // 生データの型に編集
    //raw->year  = RtcHEX2BCD( date->year );    // なぜか値が格納されないので下のコードに変更
    *(u32 *)raw = RtcHEX2BCD(date->year);
    raw->month = RtcHEX2BCD(date->month);
    raw->day = RtcHEX2BCD(date->day);
    raw->week = (u32)(date->week);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckTime

  Description:  時刻がRTCにセットして問題ない値かどうかをチェックする。
                問題なければRTCにセットできる形に編集する。

  Arguments:    date - チェックする時刻を入力。
                raw  - RTCにセットできる形に編集されたデータを出力。

  Returns:      BOOL - チェックして問題がある場合にFALSEを返す。
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckTime(const RTCTime *time, RTCRawTime *raw)
{
    // 各メンバが許容範囲内かチェック
    if (time->hour >= 24)
        return FALSE;
    if (time->minute >= 60)
        return FALSE;
    if (time->second >= 60)
        return FALSE;

    // 生データの型に編集
    if (time->hour >= 12)
    {
        raw->afternoon = 1;
    }
    else
    {
        raw->afternoon = 0;
    }
    raw->hour = RtcHEX2BCD(time->hour);
    raw->minute = RtcHEX2BCD(time->minute);
    raw->second = RtcHEX2BCD(time->second);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcGetResultCallback

  Description:  非同期処理の完了時に呼び出され、内部変数の処理結果を更新する。

  Arguments:    result - 非同期関数の処理結果。
                arg    - 使用しない。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void RtcGetResultCallback(RTCResult result, void *arg)
{
#pragma unused( arg )

    rtcWork.commonResult = result;
}

/*---------------------------------------------------------------------------*
  Name:         RtcConvertTickToDateTimeEx

  Description:  Converts CPU ticks to RTCDate and RTCTimeEx. The number of ticks
                are treated as total ticks progressed since 2000/01/01/00:00:00.

  Arguments:    date      - buffer for storing RTCDate
                time      - buffer for storing RTCTimeEx

  Returns:      None
 *---------------------------------------------------------------------------*/
static void RtcConvertTickToDateTimeEx(RTCDate * date, RTCTimeEx *time, OSTick tick)
{
    // store date and time converted from the seconds converted from ticks
    RTC_ConvertSecondToDateTime(date, (RTCTime*)time, (s64) OS_TicksToSeconds(tick));
    // store milliseconds independently
    time->millisecond = (s32) (OS_TicksToMilliSeconds(tick) % 1000);
}

/*---------------------------------------------------------------------------*
  Name:         RtcTickInit

  Description:  Saves initial total CPU ticks progressed since
                2000/01/01/00:00:00. The total ticks are calculated using
                current RTCDate, RTCTime and OS_GetTick().

  Arguments:    None

  Returns:      RTCResult - result of initial RTC_GetDateTime
 *---------------------------------------------------------------------------*/
static RTCResult RtcTickInit(void)
{   
    RTCDate currentDate;
    RTCTime currentTime;
    s64     totalSeconds;
    RTCResult result;
    
    SDK_ASSERT(OS_IsTickAvailable());

    // save current seconds converted from current date and time
    result = RTC_GetDateTime(&currentDate, &currentTime);
    totalSeconds = RTC_ConvertDateTimeToSecond(&currentDate, &currentTime);

    rtcInitialTotalTicks = OS_SecondsToTicks(totalSeconds) - OS_GetTick();
    rtcTickInitialized = TRUE;

    return result;
}

#include    <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name:         RtcWaitBusy

  Description:  RTCの非同期処理がロックされている間待つ。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void
RtcWaitBusy( void )
{
    ldr     r12,    =rtcWork.lock
loop:
    ldr     r0,     [ r12,  #0 ]
    cmp     r0,     #RTC_LOCK_ON
    beq     loop
    bx      lr
}
#include    <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
