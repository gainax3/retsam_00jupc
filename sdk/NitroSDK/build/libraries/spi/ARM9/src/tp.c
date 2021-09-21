/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - tp
  File:     tp.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: tp.c,v $
  Revision 1.36.18.1  2008/01/18 01:06:18  seiki_masashi
  HEAD とマージ

  Revision 1.38  2007/12/20 01:06:08  terui
  Correct copyright year.

  Revision 1.37  2007/12/19 01:10:55  terui
  TP_Wait[All]Busy() において、待機からの復帰条件を既に満たしている場合は
  割込み許可状態に関わらず成功するように修正。

  Revision 1.36  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.35  2005/04/14 05:07:42  takano_makoto
  キャリブレーションパラメータが不正な場合に座標値を0として返すよう修正

  Revision 1.34  2005/04/14 04:05:10  takano_makoto
  TP_GetUserInfoが常にTRUEを返すように修正

  Revision 1.33  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.32  2005/02/18 08:54:02  yasu
  著作年度変更

  Revision 1.31  2005/02/18 07:29:41  yasu
  Signed/Unsigned 変換警告抑制

  Revision 1.30  2004/12/29 06:08:57  takano_makoto
  TP_GetRatestRawPointInAuto関数に座標飛び対策処理を追加

  Revision 1.29  2004/12/29 04:00:26  takano_makoto
  TP_RequestSetStability関数の引数に過去との互換のためretryパラメータを追加(内部では使用されない)

  Revision 1.28  2004/12/29 02:04:34  takano_makoto
  SetStability関数のretryパラメータを廃止

  Revision 1.27  2004/12/14 00:20:29  takano_makoto
  TP_GetLatestRawPointInAuto中のtpState.indexの値をあらかじめ退避しておくように修正

  Revision 1.26  2004/12/13 09:01:33  takano_makoto
  万一アプリケーションがvalidityを見ていない場合のため、TP_GetLatestRawPointInAuto関数でINVALIDの場合でも座標値を一応設定しておく。

  Revision 1.25  2004/12/08 06:16:37  takano_makoto
  TP_GetLatestRawPointInAutoで、invalidな座標値しか発見できなかった場合には invalidフラグを立てた値を返すように修正。

  Revision 1.24  2004/11/02 10:26:33  takano_makoto
  fix typo.

  Revision 1.23  2004/11/02 10:19:34  takano_makoto
  fix typo.

  Revision 1.22  2004/10/20 06:34:45  terui
  LCDのライン数定義名を変更

  Revision 1.21  2004/09/27 10:06:37  takano_makoto
  branchとのMerge

  Revision 1.20.2.1  2004/09/27 10:02:33  takano_makoto
  TP_CalcCalivrateParamで割り込み禁止のままエラー終了する場合があったバグを修正

  Revision 1.20  2004/09/13 05:49:56  yasu
  merged with NitroSDK_2_00pr6_branch

  Revision 1.19.2.1  2004/09/13 05:43:02  yasu
  Support SDK_NVRAM_FORMAT

  Revision 1.19  2004/09/03 04:29:15  terui
  NVRAM内ユーザー情報に関する構造体定義整理に伴う修正。

  Revision 1.18  2004/08/25 09:29:17  terui
  calibration情報読み出し部分をTS_VERSIONにより切り分けるように変更。

  Revision 1.17  2004/07/27 07:31:10  yasu
  Add #pragma  warn_padding off

  Revision 1.16  2004/06/10 06:52:15  yasu
  add comment @ TP_WaitBusy

  Revision 1.15  2004/06/10 05:27:03  yasu
  Add TP_GetCalibratedResult

  Revision 1.14  2004/06/10 05:00:47  takano_makoto
  TP_WaitBusy, TP_WaitAllBusyでのASSERT修正

  Revision 1.13  2004/06/10 04:41:37  yasu
  fix assert condition in TP_RequestAutoSamplingStartAsync

  Revision 1.12  2004/06/09 04:16:00  takano_makoto
  TP_GetCalibratePoint()でinvalidな値に対しても変換を実行するように修正

  Revision 1.11  2004/06/02 06:14:37  takano_makoto
  バグ修正、TP_RequestAutoSamplingStartAsync(), TP_SetCallback()の引数変更

  Revision 1.10  2004/05/31 07:54:35  takano_makoto
  PXI送信直後に割り込みが発生した場合のバグ修正

  Revision 1.9  2004/05/21 10:01:39  takano_makoto
  TP_GetUserInfo()を追加

  Revision 1.8  2004/05/18 01:38:48  terui
  ARM7とのPXI同期機構に対応

  Revision 1.7  2004/05/14 00:42:18  takano_makoto
  Modify Calibrate Parameter format.

  Revision 1.6  2004/04/21 12:13:38  takano_makoto
  Add Async function.

  Revision 1.5  2004/04/19 12:00:01  takano_makoto
  modify any functions interface.

  Revision 1.4  2004/04/16 02:13:37  takano_makoto
  Modify for STRB warning.

  Revision 1.3  2004/04/15 13:55:18  takano_makoto
  Chang any function name.

  Revision 1.2  2004/04/15 11:43:17  takano_makoto
  Change header file path.

  Revision 1.1  2004/04/15 10:56:17  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/spi/ARM9/tp.h>
#include <nitro/spi/common/config.h>
#include "spi.h"

#define TP_RAW_MAX  0x1000
#define TP_CALIBRATE_DOT_INV_SCALE_SHIFT    (28 - TP_CALIBRATE_DOT_SCALE_SHIFT)
#define TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT (TP_CALIBRATE_DOT_SCALE_SHIFT - TP_CALIBRATE_ORIGIN_SCALE_SHIFT)

/*===========================================================================*
    Static function prototype definition
 *===========================================================================*/

static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err);

/*---------------------------------------------------------------------------*
    Static variables definition
 *---------------------------------------------------------------------------*/
typedef struct
{
    s32     x0;                        // X座標の切片
    s32     xDotSize;                  // Xドット幅
    s32     xDotSizeInv;               // X分母

    s32     y0;                        // Y座標の切片
    s32     yDotSize;                  // Yドット幅
    s32     yDotSizeInv;               // Y分母

}
TPiCalibrateParam;

#pragma  warn_padding off
static struct
{
    TPRecvCallback callback;           // TP値を取得した時に呼び出すユーザコールバック関数
    TPData  buf;                       // TP値１回取得時のTP値受信バッファ
    u16     index;                     // オートサンプリング時の最新バッファインデックス
    u16     frequence;                 // オートサンプリング時の１フレームサンプリング回数
    TPData *samplingBufs;              // オートサンプリング時のTP値バッファポインタ
    u16     bufSize;                   // オートサンプリング時のTPバッファサイズ
    // PADDING 2 BYTE
    TPiCalibrateParam calibrate;       // キャリブレーションパラメータ
    u16     calibrate_flg;             // キャリブレーションフラグ

    vu16    state;                     // タッチパネルの状態
    vu16    err_flg;                   // エラーフラグ
    vu16    command_flg;               // リクエストコマンド実行中フラグ
}
tpState;
#pragma  warn_padding reset


/*---------------------------------------------------------------------------*
    Inline sub-routine definition
    
    これらの命令に対するARM7からの応答はPXIライブラリ経由で返されます。
    PXIにタグ"PXI_FIFO_TAG_TOUCHPANEL"のコールバックを指定することで
    ARM7からの応答を得ることができます。
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         TPi_SamplingNow

  Description:  タッチパネルを一回サンプリングする。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_SamplingNow(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_SAMPLING << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_AutoSamplingOn

  Description:  タッチパネルのオートサンプリングを開始する。

  Arguments:    vCount -    サンプリングを行うVカウント。
                            1フレームに複数回サンプリングする場合、ここを
                            起点に1フレームが時分割される。
                frequency - 1フレームに何回サンプリングするかの頻度。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_AutoSamplingOn(u16 vCount, u8 frequency)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_AUTO_ON << 8) | (u32)frequency, 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | (u32)vCount, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_AutoSamplingOff

  Description:  タッチパネルのオートサンプリングを停止する。

  Arguments:    None.

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_AutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_SetupStability

  Description:  サンプリングにおける安定判定パラメータを設定する。

  Arguments:    range - 連続したサンプリングにおいて、検出電圧が安定したとみなす誤差。
                        なお、検出値は12bitで 0 ～ 4095。

  Returns:      BOOL - 命令をPXI経由で正常に送信できた場合TRUE、
                       失敗した場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_SetupStability(u16 range)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_SETUP_STABILITY << 8) | (u32)range, 0))
    {
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         TPi_CopyTpFromSystemWork

  Description:  ARM7からshared memoryへ書き込まれたタッチパネル値のデータ
                を別領域に読み出す。

  Returns:      result - 読み出された最新のタッチパネル値を格納する。
 *---------------------------------------------------------------------------*/
static inline void TPi_CopyTpFromSystemWork(TPData *result)
{
    SPITpData spi_tp;

    spi_tp.halfs[0] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[0]));
    spi_tp.halfs[1] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[2]));

    // システム領域から読み出し( 2バイトアクセス )
    result->x = (u16)spi_tp.e.x;
    result->y = (u16)spi_tp.e.y;
    result->touch = (u8)spi_tp.e.touch;
    result->validity = (u8)spi_tp.e.validity;
}


/*---------------------------------------------------------------------------*
  Name:         TPi_ErrorAtPxi

  Description:  ARM7とのPXI通信においてエラーが発生した場合の処理。
                コールバックが設定されている場合には, TP_RESULT_PXI_BUSY
                コールバックを返します。
                
  Arguments:    command - リクエストの種類
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void TPi_ErrorAtPxi(TPRequestCommand command)
{

    tpState.err_flg |= (1 << command);

    if (tpState.callback)
    {
        (tpState.callback) (command, TP_RESULT_PXI_BUSY, 0);
    }
}


/*===========================================================================*
    Function definition
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         TPi_TpCallback

  Description:  PXIがARM7からタッチパネル関連のメッセージを受け取った場合に
                呼び出される関数。
                ARM7からのタッチパネル値の退避を行い、コールバック関数が
                設定されている場合には、更にユーザーコールバックを呼び出す。
                

  Arguments:    tag -  PXIが種別を見分けるためのタグ。
                data - ARM7からのメッセージデータ。
                err -  PXI転送のエラーフラグ。
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)

    u16     result;
    u16     command;

    result = (u16)(data & SPI_PXI_DATA_MASK);
    command = (u16)((result & 0x7f00) >> 8);

    // PXI転送エラー
    if (err)
    {
        TPi_ErrorAtPxi((TPRequestCommand)command);
        return;
    }

    if (command == SPI_PXI_COMMAND_TP_AUTO_SAMPLING)
    {
        // オートサンプリングの実行完了通知
//        tpState.index = (u16) (result & 0x00FF);
        tpState.index++;
        if (tpState.index >= tpState.bufSize)
        {
            tpState.index = 0;
        }

        // Shared領域からタッチパネル値を退避
        TPi_CopyTpFromSystemWork(&tpState.samplingBufs[tpState.index]);

        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, TP_RESULT_SUCCESS, (u8)tpState.index);
        }
        return;
    }

    if (!(data & SPI_PXI_END_BIT))
    {
        return;
    }

    SDK_ASSERT(result & 0x8000);

    switch ((u8)(result & 0x00ff))
    {
    case SPI_PXI_RESULT_SUCCESS:
        // 成功した場合の処理
        switch (command)
        {
        case SPI_PXI_COMMAND_TP_SAMPLING:
            // サンプリング結果を取得
            TPi_CopyTpFromSystemWork(&tpState.buf);
            tpState.state = TP_STATE_READY;
            break;

        case SPI_PXI_COMMAND_TP_AUTO_ON:
            tpState.state = TP_STATE_AUTO_SAMPLING;
            break;

        case SPI_PXI_COMMAND_TP_AUTO_OFF:
            tpState.state = TP_STATE_READY;
            break;
        }

        // ビジーフラグを下げる
        tpState.command_flg &= ~(1 << command);

        // コールバック呼び出し
        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, TP_RESULT_SUCCESS, 0);
        }
        break;

    case SPI_PXI_RESULT_EXCLUSIVE:
        result = TP_RESULT_EXCLUSIVE;
        goto common;

    case SPI_PXI_RESULT_INVALID_PARAMETER:
        result = TP_RESULT_INVALID_PARAMETER;
        goto common;

    case SPI_PXI_RESULT_ILLEGAL_STATUS:
        result = TP_RESULT_ILLEGAL_STATUS;

      common:
        // エラー処理
        // エラーフラグを立てる
        tpState.err_flg |= (1 << command);
        tpState.command_flg &= ~(1 << command);

        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, (TPRequestResult)(result & 0xff), 0);
        }
        break;

    case SPI_PXI_RESULT_INVALID_COMMAND:
    default:
        // 異常終了
//        OS_Printf("result=%x\n",result);
        OS_Panic("Get illegal TP command from ARM7!\n");
        return;
    }
}




/*---------------------------------------------------------------------------*
  Name:         TP_Init

  Description:  タッチパネルライブラリの初期化.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_Init(void)
{
    static u16 initial = FALSE;

    if (initial)
    {
        return;
    }
    initial = TRUE;

    //****************************************************************
    // Initialize TP.
    //****************************************************************
    PXI_Init();

    tpState.index = 0;
    tpState.callback = NULL;
    tpState.samplingBufs = NULL;
    tpState.state = TP_STATE_READY;
    tpState.calibrate_flg = FALSE;
    tpState.command_flg = 0;
    tpState.err_flg = 0;

    // 2003/05/18 Add by terui.
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_TOUCHPANEL, PXI_PROC_ARM7))
    {
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_TOUCHPANEL, TPi_TpCallback);
}


/*---------------------------------------------------------------------------*
  Name:         TP_GetUserInfo

  Description:  キャリブレーションのためのパラメータを本体内フラッシュメモリより
                取得する関数。

  Returns:      param - パラメータを取得する領域へのポインタ。
                BOOL  - 値の取得に成功した場合はTRUE
                        有効な値が見つからなかった場合はFALSEを返す。
 *---------------------------------------------------------------------------*/
BOOL TP_GetUserInfo(TPCalibrateParam *calibrate)
{
    NVRAMConfig *info = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
//    s16 x0, y0, xdot, ydot;
    u16     x1, y1, x2, y2, dx1, dy1, dx2, dy2;

    SDK_NULL_ASSERT(calibrate);

#if ( defined( SDK_TS ) && ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 ) )
    x1 = info->ncd.tp.raw_x1;
    y1 = info->ncd.tp.raw_y1;
    dx1 = (u16)(info->ncd.tp.dx1);
    dy1 = (u16)(info->ncd.tp.dy1);
    x2 = info->ncd.tp.raw_x2;
    y2 = info->ncd.tp.raw_y2;
    dx2 = (u16)(info->ncd.tp.dx2);
    dy2 = (u16)(info->ncd.tp.dy2);
#else
    x1 = *(u16 *)(&(info->ncd.tp.calib_data[0]));
    y1 = *(u16 *)(&(info->ncd.tp.calib_data[1]));
    dy1 = (u16)(*(u16 *)(&(info->ncd.tp.calib_data[2])) >> 8);
    dx1 = MI_ReadByte((u8 *)(&(info->ncd.tp.calib_data[2])));
    x2 = *(u16 *)(&(info->ncd.tp.calib_data[3]));
    y2 = *(u16 *)(&(info->ncd.tp.calib_data[4]));
    dy2 = (u16)(*(u16 *)(&(info->ncd.tp.calib_data[5])) >> 8);
    dx2 = MI_ReadByte((u8 *)(&(info->ncd.tp.calib_data[5])));
#endif

    /* キャリブレーション値の有効フラグがないのでとりあえず */
    if ((x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0) ||
        (TP_CalcCalibrateParam(calibrate, x1, y1, dx1, dy1, x2, y2, dx2, dy2) != 0))
    {
        // IPLでデータのCRCチェックをしていないので、異常値の場合はすべてのパラメータを
        // 0でクリアした上でTRUEを返します。 
        // この場合キャリブレート後のTP値は常に(0,0)になります。
        calibrate->x0 = 0;
        calibrate->y0 = 0;
        calibrate->xDotSize = 0;
        calibrate->yDotSize = 0;
        return TRUE;
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         TP_SetCalibrateParam

  Description:  キャリブレーションのためのパラメータをセットする。
                この関数でパラメータを設定すると、以降TP_WaitCalibratedResult()
                TP_GetLatestCalibratedPointInAuto(), TP_GetCalibratedPoint(),
                TP_GetUnCalibratedPoint()においてこの値を使用してキャリブレーションが
                行われる。ドットサイズの逆数はこの時点であらかじめ計算される。

  Arguments:    param - 設定するキャリブレーションパラメータへのポインタ。
                        NULLが設定された場合には、以降キャリブレーションによる座標変換は
                        行われない。 (デフォルト値: NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_SetCalibrateParam(const TPCalibrateParam *param)
{
    OSIntrMode enabled;

    if (param == NULL)
    {
        tpState.calibrate_flg = FALSE;
        return;
    }

    // 予めxDotSize, yDotSIzeの逆数を計算しておく
    enabled = OS_DisableInterrupts();

    if (param->xDotSize != 0)
    {
        CP_SetDiv32_32(0x10000000, (u32)param->xDotSize);

        tpState.calibrate.x0 = param->x0;
        tpState.calibrate.xDotSize = param->xDotSize;
        tpState.calibrate.xDotSizeInv = (s32)CP_GetDivResult32();
    }
    else
    {
        tpState.calibrate.x0 = 0;
        tpState.calibrate.xDotSize = 0;
        tpState.calibrate.xDotSizeInv = 0;
    }

    if (param->yDotSize != 0)
    {
        CP_SetDiv32_32(0x10000000, (u32)param->yDotSize);

        tpState.calibrate.y0 = param->y0;
        tpState.calibrate.yDotSize = param->yDotSize;
        tpState.calibrate.yDotSizeInv = (s32)CP_GetDivResult32();
    }
    else
    {
        tpState.calibrate.y0 = 0;
        tpState.calibrate.yDotSize = 0;
        tpState.calibrate.yDotSizeInv = 0;
    }

    (void)OS_RestoreInterrupts(enabled);

    tpState.calibrate_flg = TRUE;

}


/*---------------------------------------------------------------------------*
  Name:         TP_SetCallback

  Description:  タッチパネルから処理結果が返ってきた時に呼び出す
                ユーザコールバック関数を設定する。

  Arguments:    callback - ユーザコールバック関数ポインタ。
                           NULLの場合はコールバックを呼び出さない(デフォルト:NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_SetCallback(TPRecvCallback callback)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    tpState.callback = callback;
    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         TP_RequestSamplingAsync

  Description:  ARM7に対してタッチパネル値を要求する。
                この関数をコールした後TP_WaitRawResult() または、
                TP_WaitCalibratedResult()でタッチパネル値を読み出すことができる。
                オートサンプリング中は使用できない。
                
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestSamplingAsync(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING));

    enabled = OS_DisableInterrupts();

    if (TPi_SamplingNow() == FALSE)
    {
        // PXI送信失敗
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SAMPLING);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SAMPLING;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SAMPLING;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitRawResult

  Description:  ARM7からタッチパネル値がセットされるのを待ち、
                キャリブレートされていない生の値を読み出す。
                ここで得られる値はチャタリング対策済み。

  Arguments:    None.

  Returns:      result - タッチパネル値を取得するための変数へのポインタ。
                         x,y座標はキャリブレートされていない値(0～4095)をとる。
                u32    - データの取得に成功した場合には 0、
                         失敗した場合には 0以外を返す。
 *---------------------------------------------------------------------------*/
u32 TP_WaitRawResult(TPData *result)
{
    SDK_NULL_ASSERT(result);

    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SAMPLING);

    if (tpState.err_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING)
    {
        return 1;
    }

    *result = tpState.buf;
    return 0;
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetCalibratedResult

  Description:  ARM7からタッチパネル値がセットされているとみなし、
                キャリブレートされた画面座標に対応した値を読み出す。
                ここで得られる値はチャタリング対策済み。

  Arguments:    result - タッチパネル値を取得するための変数へのポインタ。
                         x,y座標は画面座標に対応した値をとる。
                         もしもキャリブレーションパラメータが設定されていない場合
                         には(0～4095)のタッチパネル値が得られる。
                         
  Returns:      u32    - データの取得に成功した場合には 0、
                         失敗した場合には 0以外を返す。
 *---------------------------------------------------------------------------*/
u32 TP_GetCalibratedResult(TPData *result)
{
    SDK_NULL_ASSERT(result);

    if (tpState.err_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING)
    {
        return 1;
    }

    *result = tpState.buf;
    TP_GetCalibratedPoint(result, result);
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitCalibratedResult

  Description:  ARM7からタッチパネル値がセットされるのを待ち、
                キャリブレートされた画面座標に対応した値を読み出す。
                ここで得られる値はチャタリング対策済み。

  Arguments:    result - タッチパネル値を取得するための変数へのポインタ。
                         x,y座標は画面座標に対応した値をとる。
                         もしもキャリブレーションパラメータが設定されていない場合
                         には(0～4095)のタッチパネル値が得られる。
                         
  Returns:      u32    - データの取得に成功した場合には 0、
                         失敗した場合には 0以外を返す。
 *---------------------------------------------------------------------------*/
u32 TP_WaitCalibratedResult(TPData *result)
{
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SAMPLING);
    return TP_GetCalibratedResult(result);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStartAsync

  Description:  ARM7に対してタッチパネル値のオートサンプリング開始要求を出す。
                1フレームにfrequence回のデータが均等な間隔でサンプリングされ、
                結果をsamplingBufsで指定した配列へ格納する。

  Arguments:    vcount       - オートサンプリングを行う基準となるVCOUNT値を設定。
                frequence    - １フレームに何回のサンプリングを行うかの設定。
                samplingBufs - オートサンプリングしたデータを格納する領域を設定。
                bufSize      - バッファサイズの設定。
                               samplingBufsの配列がbusSizeのリングバッファとして使用される。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[], u16 bufSize)
{
    u32     i;
    OSIntrMode enabled;

    SDK_NULL_ASSERT(samplingBufs);
    SDK_ASSERT(vcount < HW_LCD_LINES);
    SDK_ASSERT(frequence != 0 && frequence <= SPI_TP_SAMPLING_FREQUENCY_MAX);
    SDK_ASSERT(tpState.state == TP_STATE_READY);
    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_AUTO_ON));
    SDK_ASSERT(bufSize != 0);

    tpState.samplingBufs = samplingBufs;
    tpState.index = 0;
    tpState.frequence = frequence;
    tpState.bufSize = bufSize;

    for (i = 0; i < bufSize; i++)
    {
        tpState.samplingBufs[i].touch = TP_TOUCH_OFF;
    }

    enabled = OS_DisableInterrupts();

    if ((u8)TPi_AutoSamplingOn(vcount, (u8)frequence) == FALSE)
    {
        // PXI送信失敗
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_AUTO_ON);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_AUTO_ON;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_AUTO_ON;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStopAsync

  Description:  ARM7に対してタッチパネル値のオートサンプリング停止要求を出す。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestAutoSamplingStopAsync(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(tpState.state == TP_STATE_AUTO_SAMPLING);
    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_AUTO_OFF));

    enabled = OS_DisableInterrupts();

    if (TPi_AutoSamplingOff() == FALSE)
    {
        // PXI送信失敗
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_AUTO_OFF);
        return;
    }

    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_AUTO_OFF;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_AUTO_OFF;

    (void)OS_RestoreInterrupts(enabled);

}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestSetStabilityAsync

  Description:  タッチパネルのチャタリング対策パラメータを設定する。
                値が安定するまでのリトライサンプリング回数と、
                値が安定したと判定するためのレンジを設定。

  Arguments:    retry -  この引数は内部では使用されていません。
                range -  値が安定したかどうかを判定するためのレンジ.
                         座標値がこの範囲に収束しなかった場合にはinvalidとなります。
                         (範囲:0～4095, デフォルト値:20)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestSetStabilityAsync(u8 retry, u16 range)
{
#pragma unused( retry )
    OSIntrMode enabled;

    SDK_ASSERT(range != 0);
    SDK_ASSERT(range < 255);

    enabled = OS_DisableInterrupts();

    if (TPi_SetupStability(range) == FALSE)
    {
        // PXI送信失敗
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SET_STABILITY);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SET_STABILITY;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SET_STABILITY;

    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestRawPointInAuto

  Description:  オートサンプリング結果から、有効な最新のタッチパネル値を取得する。
                値のx,y座標はキャリブレーションされていない(0～4095)の範囲をとる。

  Arguments:    None.

  Returns:      result - 有効な最新のタッチパネル値を取得するためのポインタ。
 *---------------------------------------------------------------------------*/
void TP_GetLatestRawPointInAuto(TPData *result)
{
    s32     i, curr_index;
    TPData *tp;

    SDK_NULL_ASSERT(result);
    SDK_NULL_ASSERT(tpState.samplingBufs);

    result->validity = TP_VALIDITY_INVALID_XY;

    curr_index = tpState.index;

    // サンプリングバッファサイズが１の場合は、現在の座標値をそのまま返す
    if (tpState.frequence == 1 || tpState.bufSize == 1)
    {
        *result = tpState.samplingBufs[curr_index];
        return;
    }

    // 有効なサンプリング値の中で最新のものを検索
    for (i = 0; i < tpState.frequence && i < tpState.bufSize - 1; i++)
    {
        s32     index;

        index = curr_index - i;
        if (index < 0)
        {
            index += tpState.bufSize;
        }

        tp = &tpState.samplingBufs[index];

        if (!tp->touch)
        {
            *result = *tp;
            return;
        }

        // 無効なデータを含む場合は１つ古いサンプリング値を参照
        if (result->validity & TP_VALIDITY_INVALID_X)
        {
            /* X座標 */
            if (!(tp->validity & TP_VALIDITY_INVALID_X))
            {
                result->x = tp->x;
                if (i != 0)
                {
                    result->validity &= ~TP_VALIDITY_INVALID_X;
                }
            }
        }

        if (result->validity & TP_VALIDITY_INVALID_Y)
        {
            /* Y座標 */
            if (!(tp->validity & TP_VALIDITY_INVALID_Y))
            {
                result->y = tp->y;
                if (i != 0)
                {
                    result->validity &= ~TP_VALIDITY_INVALID_Y;
                }
            }
        }

        if (result->validity == TP_VALIDITY_VALID)
        {
            result->touch = TP_TOUCH_ON;
            return;
        }
    }

    // 無効な座標値が残っている場合は取得できた値を返す。
    result->touch = TP_TOUCH_ON;
    return;
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestCalibratedPointInAuto

  Description:  オートサンプリング結果から、有効な最新のタッチパネル値を取得する。
                値のx,y座標は画面座標の範囲をとる。

  Arguments:    None.

  Returns:      result - 有効な最新のタッチパネル値を取得するためのポインタ。
 *---------------------------------------------------------------------------*/
void TP_GetLatestCalibratedPointInAuto(TPData *result)
{
    SDK_NULL_ASSERT(result);

    TP_GetLatestRawPointInAuto(result);

    TP_GetCalibratedPoint(result, result);

}


/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestIndexInAuto

  Description:  オートサンプリングによって値が格納されるループバッファの中で、
                最後にサンプリングされたデータのインデックスを返す。

  Arguments:    None.

  Returns:      u16 - 最後にサンプリングされたデータのインデックス。
 *---------------------------------------------------------------------------*/
u16 TP_GetLatestIndexInAuto(void)
{
    return tpState.index;
}




/*---------------------------------------------------------------------------*
  Name:         TP_CalcCalibrateParam

  Description:  ２点のタッチパネル値と画面座標から、キャリブレーション用の
                パラメータを計算する。

  Arguments:    raw_x1, raw_y1 - ポイント１のタッチパネル値
                dx1, dy1       - ポイント１の画面座標
                raw_x2, raw_y2 - ポイント２のタッチパネル値
                dx2, dy2,      - ポイント２の画面座標
                
  Returns:      calibrate      - キャリブレーションパラメータ
                u32            - 有効なパラメータの場合は0
                                 有効でないパラメータの場合は0以外を返す
                                 
 *---------------------------------------------------------------------------*/
u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate,
                          u16 raw_x1, u16 raw_y1,
                          u16 dx1, u16 dy1, u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2)
{
    s32     rx_width, dx_width, ry_width, dy_width;
    s32     tmp32;
    OSIntrMode enabled;

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

    /*                                                                  */
    /* xDotSize = ((raw_x1 - raw_x2) << SCALE_SHIFT) / (dx1 - dx2)      */
    /* x0 = ((raw_x1 + raw_x2) - (dx1 + dx2) * xDotSize) / 2            */
    /* xDotSize = ((raw_x1 - raw_x2) << SCALE_SHIFT) / (dx1 - dx2)      */
    /* x0 = ((raw_x1 + raw_x2) - (dx1 + dx2) * xDotSize) / 2            */
    /*                                                                  */

    SDK_NULL_ASSERT(calibrate);
    // 座標の範囲チェック
    if (raw_x1 >= TP_RAW_MAX || raw_y1 >= TP_RAW_MAX || raw_x2 >= TP_RAW_MAX
        || raw_y2 >= TP_RAW_MAX)
    {
        return 1;
    }
    if (dx1 >= GX_LCD_SIZE_X || dx2 >= GX_LCD_SIZE_X || dy1 >= GX_LCD_SIZE_Y
        || dy2 >= GX_LCD_SIZE_Y)
    {
        return 1;
    }
    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2)
    {
        return 1;
    }

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = OS_DisableInterrupts();

    // xDotSizeを計算
    CP_SetDiv32_32(((u32)rx_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, (u32)dx_width);

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    tmp32 = CP_GetDivResult32();
    CP_SetDiv32_32(((u32)ry_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, (u32)dy_width);

    if (!IN_S16_RANGE(tmp32))
    {
        (void)OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->xDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_x1 + raw_x2) << TP_CALIBRATE_DOT_SCALE_SHIFT)
                   - ((s32)(dx1 + dx2) * calibrate->xDotSize)) >> (TP_CALIBRATE_DOT_SCALE_SHIFT -
                                                                   TP_CALIBRATE_ORIGIN_SCALE_SHIFT +
                                                                   1));
    if (!IN_S16_RANGE(tmp32))
    {
        (void)OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->x0 = (s16)tmp32;

    tmp32 = CP_GetDivResult32();
    (void)OS_RestoreInterrupts(enabled);

    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->yDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_y1 + raw_y2) << TP_CALIBRATE_DOT_SCALE_SHIFT)
                   - ((s32)(dy1 + dy2) * calibrate->yDotSize)) >> (TP_CALIBRATE_DOT_SCALE_SHIFT -
                                                                   TP_CALIBRATE_ORIGIN_SCALE_SHIFT +
                                                                   1));
    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->y0 = (s16)tmp32;

    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         TP_GetCalibratedPoint
  
  Description:  タッチパネル値から画面座標への変換をおこなう。
                キャリブレーションパラメータが設定されていない場合には
                タッチパネル値がそのまま出力される。
                引数dispとrawには同じポインタを渡しても構わない。

  Arguments:    raw      - 変換元のタッチパネル値へのポインタ。
                
  ReturnValue:  disp     - 画面座標へ変換された値が返される変数へのポインタ。
 *---------------------------------------------------------------------------*/
void TP_GetCalibratedPoint(TPData *disp, const TPData *raw)
{
    TPiCalibrateParam *cal;

    enum
    { MAX_X = GX_LCD_SIZE_X - 1, MAX_Y = GX_LCD_SIZE_Y - 1 };

    // ----------------------------------------
    // dx = (raw_x * x0) / xDotSize
    // dy = (raw_y * y0) / yDotSize
    // ----------------------------------------

    SDK_NULL_ASSERT(disp);
    SDK_NULL_ASSERT(raw);
    SDK_NULL_ASSERT(tpState.calibrate_flg);

    if (!tpState.calibrate_flg)
    {
        *disp = *raw;
        return;
    }

    cal = &tpState.calibrate;

    disp->touch = raw->touch;
    disp->validity = raw->validity;

    if (raw->touch == 0)
    {
        disp->x = 0;
        disp->y = 0;
        return;
    }

    // X座標変換
    // disp->x = (x - x0) / xDotSize;
    disp->x =
        (u16)((((u64)(raw->x << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) -
                cal->x0) * cal->xDotSizeInv) >> (TP_CALIBRATE_DOT_INV_SCALE_SHIFT +
                                                 TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->x < 0)
    {
        disp->x = 0;
    }
    else if ((s16)disp->x > MAX_X)
    {
        disp->x = MAX_X;
    }
    // Y座標変換
    // disp->y = (y - y0) / yDotSize;
    disp->y =
        (u16)((((u64)(raw->y << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) -
                cal->y0) * cal->yDotSizeInv) >> (TP_CALIBRATE_DOT_INV_SCALE_SHIFT +
                                                 TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->y < 0)
    {
        disp->y = 0;
    }
    else if ((s16)disp->y > MAX_Y)
    {
        disp->y = MAX_Y;
    }
}

/*---------------------------------------------------------------------------*
  Name:         TP_GetUnCalibratedPoint

  Description:  キャリブレーションの逆変換結果を取得する。
                画面座標からタッチパネル値への変換。
                キャリブレーションパラメータが設定されていない場合には
                画面座標がそのまま出力される。

  Arguments:    dx, dy       - 画面のXY座標。
                
  ReturnValue:  raw_x, raw_y - 対応するタッチパネル値を返すためのポインタ。
 *---------------------------------------------------------------------------*/
void TP_GetUnCalibratedPoint(u16 *raw_x, u16 *raw_y, u16 dx, u16 dy)
{
    TPiCalibrateParam *cal;

    // ----------------------------------------
    // raw_x = dx * xDotSize + x0;
    // raw_y = dy * yDotSize + y0;
    // ----------------------------------------

    SDK_NULL_ASSERT(raw_x);
    SDK_NULL_ASSERT(raw_y);
    SDK_ASSERT(tpState.calibrate_flg);

    if (!tpState.calibrate_flg)
    {
        *raw_x = dx;
        *raw_y = dy;
        return;
    }

    cal = &tpState.calibrate;

    *raw_x =
        (u16)((((s32)(dx * cal->xDotSize) >> TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT) +
               (s32)cal->x0) >> TP_CALIBRATE_ORIGIN_SCALE_SHIFT);
    *raw_y =
        (u16)((((s32)(dy * cal->yDotSize) >> TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT) +
               (s32)cal->y0) >> TP_CALIBRATE_ORIGIN_SCALE_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitBusy

  Description:  ARM7がタッチパネルの要求を終了するまでウェイトする。

  Arguments:    command_flgs - タッチパネルへの要求の種類。
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_WaitBusy(TPRequestCommandFlag command_flgs)
{
#ifdef  SDK_DEBUG
    if (!(tpState.command_flg & command_flgs))
    {
        return;
    }
#endif
    // 割り込みがOFFの場合は無限ループに陥る
    SDK_ASSERT(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_ENABLE);
    SDK_ASSERT(reg_OS_IME == 1);
    SDK_ASSERT(OS_GetIrqMask() & OS_IE_SPFIFO_RECV);

    // ループの開始前に ASSERT を入れる．割り込み禁止状態でこの関数が
    // 呼ばれることを許すべきではない．tpState のフラグが既にクリアされて
    // いるかどうかは状況によって左右される可能性があるため

    while (tpState.command_flg & command_flgs)
    {
        // Do nothing
    }

    return;
}

/*---------------------------------------------------------------------------*
  Name:         TP_WaitAllBusy

  Description:  ARM7がタッチパネルの要求をすべて終了するまでウェイトする。

  Arguments:    None.
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_WaitAllBusy(void)
{
#ifdef  SDK_DEBUG
    if (!tpState.command_flg)
    {
        return;
    }
#endif
    // 割り込みがOFFの場合は無限ループに陥る
    SDK_ASSERT(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_ENABLE);
    SDK_ASSERT(reg_OS_IME == 1);
    SDK_ASSERT(OS_GetIrqMask() & OS_IE_SPFIFO_RECV);

    while (tpState.command_flg)
    {
        // Do nothing
    }

    return;
}


/*---------------------------------------------------------------------------*
  Name:         TP_CheckBusy

  Description:  ARM7へのタッチパネル要求がビジー中であるかをチェックする。

  Arguments:    command_flgs - タッチパネルへの要求の種類。
                
  Returns:      u32          - ビジー中でなければ 0、
                               ビジー中ならば 0以外を返す
 *---------------------------------------------------------------------------*/
u32 TP_CheckBusy(TPRequestCommandFlag command_flgs)
{
    return (u32)(tpState.command_flg & command_flgs);
}



/*---------------------------------------------------------------------------*
  Name:         TP_CheckError

  Description:  ARM7へのタッチパネル要求がエラー終了していないかチェックする。

  Arguments:    command_flgs  - タッチパネルへの要求の種類。
                
  Returns:      u32           - エラーが発生していなければ 0、
                                エラーが発生していれば 0以外を返す
 *---------------------------------------------------------------------------*/
u32 TP_CheckError(TPRequestCommandFlag command_flgs)
{
    return (u32)(tpState.err_flg & command_flgs);
}
