/*---------------------------------------------------------------------------*
  Project:  NitroSDK - build - libraries - ctrdg
  File:     ctrdg_vib.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*-----------------------------------------------------------------------*
                    Type, Constant
 *-----------------------------------------------------------------------*/
#ifdef SDK_FINALROM
#define VIBi_FatalError(...) (void)0;
#else
#define VIBi_FatalError(...) OS_Panic(__VA_ARGS__)
#endif

#define VIBi_INTR_DELAY_TICK    (19)

#define VIBi_BITID              (2)

typedef struct
{
    u32     current_pos;                /* パルスセット内の現在位置 */
    u32     rest_pos;                   /* 休止時間になる位置 */
    u32     rest_tick;                  /* 休止時間の長さ。1 = 1 Tick */
    u32     on_tick[VIB_PULSE_NUM_MAX]; /* 起動時間の長さ。1 = 1 Tick */
    u32     off_tick[VIB_PULSE_NUM_MAX];/* 停止時間の長さ。1 = 1 Tick */
    BOOL    is_enable;                  /* 振動しているときTRUEになる */
    u32     repeat_num;                 /* パルスセットを繰り返す数。0のときは、終わりなく繰り返します。 */
    u32     current_count;              /* 何回パルスセットを繰り返したかを示します。 */
    VIBCartridgePulloutCallback cartridge_pullout_callback;     /* カートリッジ抜けのコールバック */
}
VIBiPulseInfo;

/*-----------------------------------------------------------------------*
                    Function prototype
 *-----------------------------------------------------------------------*/

//--- Auto Function Prototype --- Don't comment here.
BOOL    VIB_Init(void);
void    VIB_End(void);
void    VIB_StartPulse(const VIBPulseState * state);
void    VIB_StopPulse(void);
BOOL    VIB_IsExecuting(void);
void    VIB_SetCartridgePulloutCallback(VIBCartridgePulloutCallback func);
BOOL    VIB_IsCartridgeEnabled(void);
static inline u32 VIBi_PulseTimeToTicks(u32 pulse_time);
static BOOL VIBi_PulledOutCallbackCartridge(void);
static void VIBi_MotorOnOff(VIBiPulseInfo * pulse_vib);
static void VIBi_SleepCallback(void *);
//--- End of Auto Function Prototype

/*-----------------------------------------------------------------------*
                    Variable
 *-----------------------------------------------------------------------*/

/* キャッシュは32バイト単位でアクセスするので先頭を揃える */
static VIBiPulseInfo pulse_vib ATTRIBUTE_ALIGN(32);
static PMSleepCallbackInfo sc_info;

/*-----------------------------------------------------------------------*
                    Global Function Definition
 *-----------------------------------------------------------------------*/

/*!
    パルス振動を初期化します。\n
    二重呼びを行った場合は、VIB_IsCartridgeEnabled 関数と同等になります。
    
    この関数内で、NitroSDKの PM_AppendPreSleepCallback 関数を用いて、スリープに入る前に
    振動を止めるコールバックが登録されます。
    
    @retval TRUE    初期化が成功しました。
    @retval FALSE   初期化が失敗しました。
*/
BOOL VIB_Init(void)
{

    static BOOL is_initialized;

    if (is_initialized)
    {
        return VIB_IsCartridgeEnabled();
    }
    is_initialized = TRUE;

    if (CTRDGi_IsBitIDAtInit(VIBi_BITID))
    {
        MI_CpuClearFast(&pulse_vib, sizeof(pulse_vib));
        CTRDG_SetPulledOutCallback(VIBi_PulledOutCallbackCartridge);

        /* スリープに入る前のコールバックを登録する */
        PM_SetSleepCallbackInfo(&sc_info, VIBi_SleepCallback, NULL);
        PM_AppendPreSleepCallback(&sc_info);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*!
    振動カートリッジライブラリの使用を止めます。
    
    パルス振動の停止、
    VIB_Init 関数で登録したスリープ前のコールバックの削除、
    カートリッジ抜けコールバックの削除、
    を行っています。
*/
void VIB_End(void)
{

    VIB_StopPulse();
    PM_DeletePreSleepCallback(&sc_info);
    CTRDG_SetPulledOutCallback(NULL);
}

/*!
    パルス振動を開始します。\n
    もし以前のパルス振動が終了していない場合は、いったん終了してから始めます。\n
    ステータスはライブラリ側でコピーしますので、メモリを確保しておく必要はありません。
    
    振動開始前にハードウェア制限のチェックを行います。
    制限に反していた場合、DEBUG又はRELEASEビルドでは、OS_Panic 関数でメッセージを表示し、
    プログラムを停止します。FINALROMビルドでは、パルス振動が開始されません。
    
    @sa VIBPulseState
    
    @param state    パルス振動のステータス
*/
void VIB_StartPulse(const VIBPulseState * state)
{

    if (!VIB_IsCartridgeEnabled())
        return;
    
    VIB_StopPulse();
    {
        int     i;

        /* ON時間のチェック */
        for (i = 0; i < state->pulse_num; i++)
        {
            /* ON時間が0でないかどうか */
            if (state->on_time[i] == 0)
            {
                VIBi_FatalError("pulse_vib: on_time[%d] must not be 0.\n", i);
                return;
            }
            /* ON時間が VIB_ON_TIME_MAX を超えていないかどうか */
            if (state->on_time[i] > VIB_ON_TIME_MAX)
            {
                VIBi_FatalError("pulse_vib: on_time[%d] is over VIB_ON_TIME_MAX.\n", i);
                return;
            }
        }
        
        /* OFF時間のチェック */
        for (i = 0; i < state->pulse_num - 1; i++)
        {
            /* OFF時間が0でないかどうか */
            if (state->off_time[i] == 0)
            {
                VIBi_FatalError("pulse_vib: off_time[%d] must not be 0.\n", i);
                return;
            }
            /* OFF時間が直前のON時間を超えていないか */
            if (state->on_time[i] > state->off_time[i])
            {
                VIBi_FatalError("pulse_vib: on_time[%d] is over off_time[%d].\n", i, i);
                return;
            }
        }
        /* REST時間が VIB_REST_TIME_MIN 未満になっていないか */
        if (state->rest_time < VIB_REST_TIME_MIN)
        {
            VIBi_FatalError("pulse_vib: rest_time is less than VIB_REST_TIME_MIN.\n", i);
            return;
        }
    }

    pulse_vib.rest_tick = VIBi_PulseTimeToTicks(state->rest_time) - VIBi_INTR_DELAY_TICK;
    pulse_vib.repeat_num = state->repeat_num;
    pulse_vib.current_count = 0;

    pulse_vib.current_pos = 0;

    {
        u32     i;

        for (i = 0; i < VIB_PULSE_NUM_MAX; i++)
        {
            pulse_vib.on_tick[i] = VIBi_PulseTimeToTicks(state->on_time[i]) - VIBi_INTR_DELAY_TICK;
            pulse_vib.off_tick[i] =
                VIBi_PulseTimeToTicks(state->off_time[i]) - VIBi_INTR_DELAY_TICK;
        }
    }
    pulse_vib.rest_pos = state->pulse_num * 2 - 1;

    pulse_vib.is_enable = TRUE;
    /* pulse_vib構造体のポインタを送る */
    VIBi_MotorOnOff(&pulse_vib);
}

/*!
    パルス振動を停止します。
*/
void VIB_StopPulse(void)
{

    if (pulse_vib.is_enable)
    {

        pulse_vib.is_enable = FALSE;
        /* pulse_vib構造体のポインタを送る */
        VIBi_MotorOnOff(&pulse_vib);
    }
}

/*!
    パルス振動が終了したか否かを返します。最後のrest_timeが終わった時点で終了したと見なされます。
    
    @retval TRUE    パルス振動は終了していません。
    @retval FALSE   パルス振動は終了しています。
*/
BOOL VIB_IsExecuting(void)
{

    return pulse_vib.is_enable;
}

/*!
    カートリッジ抜けコールバックを登録します。
    
    カートリッジ抜けが起こった場合、ライブラリは直ちにパルス振動を停止します。@n
    この関数を用いてコールバックが登録されていた場合は、その後にコールバックが
    呼ばれます。
    
    @param func カートリッジ抜けコールバック
*/
void VIB_SetCartridgePulloutCallback(VIBCartridgePulloutCallback func)
{

    pulse_vib.cartridge_pullout_callback = func;
}

/*!
    振動カートリッジが有効な状態かどうかを返します。
    （一度抜き差しを行った場合は、TRUEを返しません。）
    
    @retval TRUE    振動カートリッジが有効な状態である
    @retval FALSE   振動カートリッジが有効な状態ではない。
*/
BOOL VIB_IsCartridgeEnabled(void)
{

    return CTRDG_IsBitID(VIBi_BITID);
}

/*-----------------------------------------------------------------------*
                    Local Function Definition
 *-----------------------------------------------------------------------*/

static inline u32 VIBi_PulseTimeToTicks(u32 pulse_time)
{

    return ((OS_SYSTEM_CLOCK / 1000) * (pulse_time)) / 64 / 10;
}

static BOOL VIBi_PulledOutCallbackCartridge(void)
{

    VIB_StopPulse();
    if (pulse_vib.cartridge_pullout_callback)
    {
        pulse_vib.cartridge_pullout_callback();
    }

    return FALSE;                      /* すぐにソフトを停止しない */
}

/*!
    Turn on and off vibration.
    
    pulse_vib->is_enable
        @li TRUE     Turn on vibration.
        @li FALSE    Turn off vibration.
*/
static void VIBi_MotorOnOff(VIBiPulseInfo * pulse_vib)
{
    /* 設定した共有メモリをフラッシュ */
    DC_FlushRange(pulse_vib, sizeof(VIBiPulseInfo));
    if (pulse_vib->is_enable == TRUE)
    {
        CTRDG_SendToARM7(pulse_vib);
    }
    else
    {
        CTRDG_SendToARM7(NULL);
    }
}

static void VIBi_SleepCallback(void *)
{

    VIB_StopPulse();
}
