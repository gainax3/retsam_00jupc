/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - TP
  File:     tp.h
  
  Copyright 2003-2006 Nintendo.  All rights reserved.
  
  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
  
  $Log: tp.h,v $
  Revision 1.15  2006/01/18 02:12:27  kitase_hirotake
  do-indent

  Revision 1.14  2005/02/28 05:26:26  yosizaki
  do-indent.

  Revision 1.13  2004/12/29 03:59:49  takano_makoto
  TP_RequestSetStability関数の引数に過去との互換のためretryパラメータを追加(内部では使用されない)

  Revision 1.12  2004/12/29 02:09:11  takano_makoto
  SetStability関数のretryパラメータを廃止

  Revision 1.11  2004/06/10 05:27:03  yasu
  Add TP_GetCalibratedResult

  Revision 1.10  2004/06/02 06:15:27  takano_makoto
  TP_RequestAutoSamplingStart(), TP_SetCallback()の引数変更

  Revision 1.9  2004/05/21 09:52:51  takano_makoto
  TP_GetUserInfo()の追加

  Revision 1.8  2004/05/14 00:41:48  takano_makoto
  Modify Calibrate Parameter format.

  Revision 1.7  2004/04/22 09:02:27  takano_makoto
  Add include type.h and misc.h

  Revision 1.6  2004/04/21 12:08:56  takano_makoto
  Add Async function.

  Revision 1.5  2004/04/19 12:00:36  takano_makoto
  Add TPCommandFlag.

  Revision 1.4  2004/04/16 02:13:53  takano_makoto
  Modify for STRB warning.

  Revision 1.3  2004/04/15 13:55:07  takano_makoto
  Chang any function name.

  Revision 1.2  2004/04/15 11:54:46  takano_makoto
  Change define to NITRO_SPI_ARM9_TP_H

  Revision 1.1  2004/04/15 11:01:26  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SPI_ARM9_TP_H_
#define NITRO_SPI_ARM9_TP_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define     TP_SAMPLING_FREQUENCY_MAX   16      // サンプリング頻度の限度

// タッチパネルの接触正否
#define     TP_TOUCH_OFF                0       // 触れていない
#define     TP_TOUCH_ON                 1       // 触れている

// タッチパネルのデータ有効性正否
#define     TP_VALIDITY_VALID           0       // 有効
#define     TP_VALIDITY_INVALID_X       1       // X座標が無効なデータ
#define     TP_VALIDITY_INVALID_Y       2       // Y座標が無効なデータ
#define     TP_VALIDITY_INVALID_XY      (TP_VALIDITY_INVALID_X | TP_VALIDITY_INVALID_Y) // XY座標共に無効なデータ

// タッチパネルへ発行する命令種別
typedef enum
{
    TP_REQUEST_COMMAND_SAMPLING = 0x0, // サンプリングを１回実行
    TP_REQUEST_COMMAND_AUTO_ON = 0x1,  // オートサンプリング開始
    TP_REQUEST_COMMAND_AUTO_OFF = 0x2, // オートサンプリング停止
    TP_REQUEST_COMMAND_SET_STABILITY = 0x3,     // チャタリング対策値の設定
    TP_REQUEST_COMMAND_AUTO_SAMPLING = 0x10     // オートサンプリングの結果受信
}
TPRequestCommand;

// タッチパネルへのコマンドフラグ
typedef enum
{
    TP_REQUEST_COMMAND_FLAG_SAMPLING = 1 << TP_REQUEST_COMMAND_SAMPLING,        // サンプリングを１回実行
    TP_REQUEST_COMMAND_FLAG_AUTO_ON = 1 << TP_REQUEST_COMMAND_AUTO_ON,  // オートサンプリング開始
    TP_REQUEST_COMMAND_FLAG_AUTO_OFF = 1 << TP_REQUEST_COMMAND_AUTO_OFF,        // オートサンプリング停止
    TP_REQUEST_COMMAND_FLAG_SET_STABILITY = 1 << TP_REQUEST_COMMAND_SET_STABILITY       // チャタリング対策値の設定
}
TPRequestCommandFlag;

// タッチパネルからの結果種別
typedef enum
{
    TP_RESULT_SUCCESS = 0,             // 成功
    TP_RESULT_INVALID_PARAMETER,       // パラメータが異常
    TP_RESULT_ILLEGAL_STATUS,          // 命令を受け付けられない状態
    TP_RESULT_EXCLUSIVE,               // SPIデバイスがビジー中
    TP_RESULT_PXI_BUSY                 // ARM7とのPXI通信がビジー中
}
TPRequestResult;

// タッチパネルの状態
typedef enum
{
    TP_STATE_READY = 0,                // レディ状態
    TP_STATE_SAMPLING,                 // サンプリングリクエスト中
    TP_STATE_AUTO_SAMPLING,            // オートサンプリング中
    TP_STATE_AUTO_WAIT_END             // オートサンプリング終了待ち
}
TPState;

/*---------------------------------------------------------------------------*
    Structures definition
 *---------------------------------------------------------------------------*/

// タッチパネル入力構造体
typedef struct
{
    u16     x;                         // x座標( 0 ～ 4095 )
    u16     y;                         // y座標( 0 ～ 4095 )
    u16     touch;                     // 接触正否
    u16     validity;                  // 有効性正否
}
TPData;


#define TP_CALIBRATE_DOT_SCALE_SHIFT        8   // Xドットサイズの精度
#define TP_CALIBRATE_ORIGIN_SCALE_SHIFT     2   // 原点座標の精度

// タッチパネルキャリブレーションデータ
typedef struct NvTpData
{
    s16     x0;                        // X原点座標
    s16     y0;                        // Y原点座標
    s16     xDotSize;                  // Xドットサイズ
    s16     yDotSize;                  // Yドットサイズ
}
TPCalibrateParam;                      // 8byte


// ユーザコールバック関数
typedef void (*TPRecvCallback) (TPRequestCommand command, TPRequestResult result, u16 index);

/*===========================================================================*
    Function definition
 *===========================================================================*/
void    TP_Init(void);
void    TP_SetCallback(TPRecvCallback callback);
void    TP_SetCalibrateParam(const TPCalibrateParam *param);

void    TP_RequestSamplingAsync(void);
u32     TP_WaitRawResult(TPData *result);
u32     TP_WaitCalibratedResult(TPData *result);
u32     TP_GetCalibratedResult(TPData *result);
void    TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[],
                                         u16 bufSize);
void    TP_RequestAutoSamplingStopAsync(void);
void    TP_RequestSetStabilityAsync(u8 retry, u16 range);
void    TP_WaitBusy(TPRequestCommandFlag command_flgs);
void    TP_WaitAllBusy(void);
u32     TP_CheckBusy(TPRequestCommandFlag command_flgs);
u32     TP_CheckError(TPRequestCommandFlag command);

void    TP_GetLatestRawPointInAuto(TPData *result);
void    TP_GetLatestCalibratedPointInAuto(TPData *result);
u16     TP_GetLatestIndexInAuto(void);
u32     TP_CalcCalibrateParam(TPCalibrateParam *calibrate,
                              u16 raw_x1, u16 raw_y1,
                              u16 dx1, u16 dy1, u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2);
BOOL    TP_GetUserInfo(TPCalibrateParam *calibrate);
void    TP_GetCalibratedPoint(TPData *disp, const TPData *raw);
void    TP_GetUnCalibratedPoint(u16 *raw_x, u16 *raw_y, u16 dx, u16 dy);

/*---------------------------------------------------------------------------*
    Inline function definition
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         TP_RequestSetStability

  Description:  タッチパネルのチャタリング対策パラメータを設定する。
                値が安定するまでのリトライサンプリング回数と、
                値が安定したと判定するためのレンジを設定。

  Arguments:    retry -  この引数は内部では使用されていません。
                range -  値が安定したかどうかを判定するためのレンジ.
                         (範囲:0～255, デフォルト値:20)
                
  Returns:      u32  - リクエストが成功すれば 0
                       失敗していれば 0以外.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestSetStability(u8 retry, u16 range)
{
    TP_RequestSetStabilityAsync(retry, range);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStart

  Description:  ARM7に対してタッチパネル値のオートサンプリング開始要求を出す。
                1フレームにfrequence回のデータが均等な間隔でサンプリングされ、
                結果をsamplingBusで指定した配列へ格納する。

  Arguments:    vcount       - オートサンプリングを行う基準となるVCOUNT値を設定。
                frequence    - １フレームに何回のサンプリングを行うかの設定。
                samplingBufs - オートサンプリングしたデータを格納する領域を設定。
                               最低でもfrequenceの大きさ分の領域用意する必要が
                               ある。

  Returns:      u32  - リクエストが成功すれば 0
                       失敗していれば 0以外.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestAutoSamplingStart(u16 vcount, u16 frequence, TPData samplingBufs[],
                                              u16 bufSize)
{
    TP_RequestAutoSamplingStartAsync(vcount, frequence, samplingBufs, bufSize);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStop

  Description:  ARM7に対してタッチパネル値のオートサンプリング停止要求を出す。

  Arguments:    None.

  Returns:      u32  - リクエストが成功すれば 0
                       失敗していれば 0以外.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestAutoSamplingStop(void)
{
    TP_RequestAutoSamplingStopAsync();
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestRawSampling

  Description:  ARM7に対してタッチパネル値を要求し、キャリブレートされていない
                生の値を読み出す。
                ここで得られる値はチャタリング対策済み。
                オートサンプリング中は使用できない。
                
  Arguments:    None.

  Returns:      result - タッチパネル値を取得するための変数へのポインタ。
                         x,y座標はキャリブレートされていない値(0～4095)をとる。
                
                u32    - リクエストが成功すれば 0
                         失敗していれば 0以外.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestRawSampling(TPData *result)
{
    TP_RequestSamplingAsync();
    return TP_WaitRawResult(result);
}

/*---------------------------------------------------------------------------*
  Name:         TP_RequestCalibratedSampling

  Description:  ARM7に対してタッチパネル値を要求し、キャリブレートされた
                画面座標に対応した値を読み出す。
                ここで得られる値はチャタリング対策済み。
                オートサンプリング中は使用できない。
                
  Arguments:    None.

  Returns:      result - タッチパネル値を取得するための変数へのポインタ。
                         x,y座標は画面座標に対応した値をとる。
                         もしもキャリブレーションパラメータが設定されていない場合
                         には(0～4095)のタッチパネル値が得られる。
                
                u32    - リクエストが成功すれば 0
                         失敗していれば 0以外.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestCalibratedSampling(TPData *result)
{
    TP_RequestSamplingAsync();
    return TP_WaitCalibratedResult(result);
}



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/* NITRO_SPI_ARM9_TP_H_ */
