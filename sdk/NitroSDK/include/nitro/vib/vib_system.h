/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include - nitro - vib
  File:     vib_system.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: vib_system.h,v $
  Revision 1.3  2007/01/22 08:48:12  okubata_ryoma
  log追加

  Revision 1.2  2006/03/03 00:35:00  okubata_ryoma
  includeをextern"C"の外へ

  Revision 1.1  2006/03/02 10:22:14  okubata_ryoma
  vib_system.hの追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef PULSE_VIB_H
#define PULSE_VIB_H

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------*
                    Type, Constant
 *-----------------------------------------------------------------------*/

/*!
    パルスセット内のパルスの最大数。変更した場合はライブラリを再ビルドしてください。
*/
#define VIB_PULSE_NUM_MAX   6

/*!
    一回のon_timeの最大値（0.1ms単位）
*/
#define VIB_ON_TIME_MAX 15

/*!
    rest_timeの最小値（0.1ms単位）
*/
#define VIB_REST_TIME_MIN   15

/*!
    パルス振動のステータスを示します。
    
    1.5msのON、1.5msのOFF、1.5msのONというパルスが標準的な振動で、これによって
    最も強い振動を発生させることができます。
    
    VIBPulseState の値は、ハードウェア仕様上、以下のルールを守る必要があります。
    
    @li 一回の on_time は1.5msまでとする。
    @li off_time[n] は直前の on_time[n] 以上の値を設定する。
    @li rest_time は1.5ms以上とする。
    
    なお、 VIB_StartPulse 関数が呼ばれた際にこれらをチェックします。
    
    @image html pulse_vib.jpg "パルス振動の例(パルス数が3の場合)"
*/
typedef struct
{
    u32     pulse_num;                  /*! 一回のパルスセットで何回パルスを発生させるか。1以上 VIB_PULSE_NUM_MAX 以下である必要があります。 */
    u32     rest_time;                  /*! パルスセット間の休止時間の長さ。1=0.1ミリ秒となります。 */
    u32     on_time[VIB_PULSE_NUM_MAX]; /*! 起動時間の長さ。0より大きい値としてください。1=0.1ミリ秒となります。 */
    u32     off_time[VIB_PULSE_NUM_MAX];/*! 停止時間の長さ。0より大きい値としてください。1=0.1ミリ秒となります。 */
    u32     repeat_num;                 /*! パルスセットを繰り返す数。0の時は、終わりなく繰り返します。 */
}
VIBPulseState;

/*! カートリッジ抜けコールバックの型です。 */
typedef void (*VIBCartridgePulloutCallback) (void);

/*-----------------------------------------------------------------------*
                    Function external declaration
 *-----------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         VIB_Init

  Description:  振動カートリッジライブラリを初期化します。
                一度この関数を呼んだ状態で再度この関数を呼んだ場合は、
                VIB_IsCartridgeEnabled 関数と同等になります。
                
                この関数内で、PM_AppendPreSleepCallback 関数を呼んで、スリープに入る前に
                振動を止めるコールバックを登録しています。
                
                また、カートリッジの抜けを検出したら振動を止めるコールバックもこの関数内で登録していますので、
                この関数を呼んだ後で、CTRDG_SetPulledOutCallback 関数でカートリッジ抜けコールバックをセットすると、
                VIB_Init 関数でセットされたカートリッジ抜け検出コールバックが上書きされてしまいます。
                その場合はセットしたカートリッジ抜けコールバック内で振動を止める必要があります。
                カートリッジの抜けコールバック内で振動を止める以外にも処理を行いたい場合は 
                VIB_SetCartridgePulloutCallback 関数でコールバックを登録して
                そのコールバック内で処理を行うようにしてください。 

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL VIB_Init(void);

/*---------------------------------------------------------------------------*
  Name:         VIB_End

  Description:  振動カートリッジライブラリの使用を終了します。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void VIB_End(void);

/*---------------------------------------------------------------------------*
  Name:         VIB_StartPulse

  Description:  パルス振動を開始します。
                もし以前のパルス振動が終了していない場合は、いったん終了してから始めます。
                ステータスはライブラリ側でコピーしますので、メモリを確保しておく必要はありません。

  Arguments:    state : パルス振動のステータス

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void VIB_StartPulse(const VIBPulseState * state);

/*---------------------------------------------------------------------------*
  Name:         VIB_StopPulse

  Description:  パルス振動を停止します。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void VIB_StopPulse(void);

/*---------------------------------------------------------------------------*
  Name:         VIB_StopPulse

  Description:  パルス振動が実行中か否かを判定します。
                VIB_StartPulse 関数で振動ONにしてから VIB_StopPulse 関数で振動OFFにするまでの間 TRUE を返します。

  Arguments:    None.

  Returns:      TRUE  : パルス振動実行している状態
                FALSE : パルス振動実行していない状態
 *---------------------------------------------------------------------------*/
extern BOOL VIB_IsExecuting(void);

/*---------------------------------------------------------------------------*
  Name:         VIB_SetCartridgePulloutCallback

  Description:  カートリッジ抜けコールバックを登録します。
                カートリッジ抜けが起こった場合、ライブラリは直ちにパルス振動を停止します。
                この関数を用いてコールバックが登録されていた場合は、その後にコールバックが呼ばれます。 

  Arguments:    func : カートリッジ抜けコールバック。

  Returns:      None.
 *---------------------------------------------------------------------------*/
extern void VIB_SetCartridgePulloutCallback(VIBCartridgePulloutCallback func);

/*---------------------------------------------------------------------------*
  Name:         VIB_IsCartridgeEnabled

  Description:  振動カートリッジがささっているかどうかを判定します。

  Arguments:    None.

  Returns:      TRUE  : 起動時に振動カートリッジがささっている状態 
                FALSE : 起動時に振動カートリッジがささっていない状態
 *---------------------------------------------------------------------------*/
extern BOOL VIB_IsCartridgeEnabled(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* PULSE_VIB_H */
