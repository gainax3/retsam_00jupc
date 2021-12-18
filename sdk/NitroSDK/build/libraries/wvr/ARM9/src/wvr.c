/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WVR - libraries
  File:     wvr.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wvr.c,v $
  Revision 1.9  2006/01/18 02:12:40  kitase_hirotake
  do-indent

  Revision 1.8  2005/05/09 08:30:20  terui
  Fix comment.
  強制終了成功時にPXIコールバック先をNULLクリアするよう修正

  Revision 1.7  2005/02/28 05:26:35  yosizaki
  do-indent.

  Revision 1.6  2005/02/17 00:01:51  terui
  PXI受信コールバックの設定は初回のみに限定するよう修正。

  Revision 1.5  2005/02/16 10:36:18  terui
  インデント修正のみ。

  Revision 1.4  2005/02/09 09:21:47  terui
  VRAMバンクの排他機能を追加。

  Revision 1.3  2005/01/11 07:46:42  takano_makoto
  fix copyright header.

  Revision 1.2  2005/01/05 07:30:37  terui
  wvrライブラリのincludeパスを明記するよう修正。

  Revision 1.1  2004/12/20 00:52:43  terui
  暫定版ではあるが、動作はする状態で新規登録。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wvr/common/wvr_common.h>
#include    <nitro/wvr/ARM9/wvr.h>
#include    <nitro/pxi.h>
#include    <nitro/os/ARM9/vramExclusive.h>
#include    <nitro/gx/gx_vramcnt.h>
#include    <nitro/wm.h>


/*---------------------------------------------------------------------------*
    内部関数定義
 *---------------------------------------------------------------------------*/
static void WvrReceiveCallback(PXIFifoTag tag, u32 data, BOOL err);
static void WvrDummyAsyncCallback(void *arg, WVRResult result);


/*---------------------------------------------------------------------------*
    内部変数定義
 *---------------------------------------------------------------------------*/
static WVRCallbackFunc wvrCallback = NULL;
static void *wvrArg = NULL;
static vu16 wvrVRam = 0;
static vu16 wvrLockId = 0;


/*---------------------------------------------------------------------------*
  Name:         WVR_StartUpAsync

  Description:  無線ライブラリを駆動開始する。
                強制停止するまで、指定したVRAM( C or D )へのアクセスは禁止になる。

  Arguments:    vram        -   ARM7に割り当てるVRAMバンクを指定。
                callback    -   処理完了時のコールバック関数を指定。
                arg         -   コールバック関数に渡す引数を指定。

  Returns:      処理結果を返す。
 *---------------------------------------------------------------------------*/
WVRResult WVR_StartUpAsync(GXVRamARM7 vram, WVRCallbackFunc callback, void *arg)
{
    OSIntrMode e;

    // コンポーネントがWVRに対応しているか確認
    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7))
    {
        return WVR_RESULT_DISABLE;
    }

    // 排他用IDを取得していなければ取得
    while (wvrLockId == 0)
    {
        s32     result = OS_GetLockID();

        if (result == OS_LOCK_ID_ERROR)
        {
            return WVR_RESULT_FATAL_ERROR;
        }
        wvrLockId = (u16)result;
    }

    // 割込み禁止
    e = OS_DisableInterrupts();

    // WVR非同期関数が動作中でないか確認
    if (wvrCallback != NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }

    // VRAM排他状態をチェック
    if (wvrVRam)
    {
        // すでに排他済み
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }
    switch (vram)
    {
    case GX_VRAM_ARM7_128_C:
        // VRAM-C を排他ロック
        if (!OSi_TryLockVram(OS_VRAM_BANK_ID_C, wvrLockId))
        {
            // VRAM-C のロックに失敗
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_C;
        // GX_SetBankForARM7( GX_VRAM_ARM7_128_C ) に相当するレジスタ設定処理
        reg_GX_VRAMCNT_C = ((2 << REG_GX_VRAMCNT_C_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
        break;
    case GX_VRAM_ARM7_128_D:
        // VRAM-D を排他ロック
        if (!OSi_TryLockVram(OS_VRAM_BANK_ID_D, wvrLockId))
        {
            // VRAM-D のロックに失敗
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_D;
        // GX_SetBankForARM7( GX_VRAM_ARM7_128_D ) に相当するレジスタ設定処理
        reg_GX_VRAMCNT_D = ((2 << REG_GX_VRAMCNT_D_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
        break;
    case GX_VRAM_ARM7_256_CD:
        // VRAM-C/D を排他ロック
        if (!OSi_TryLockVram((OS_VRAM_BANK_ID_C | OS_VRAM_BANK_ID_D), wvrLockId))
        {
            // VRAM-C/D のロックに失敗
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_C | OS_VRAM_BANK_ID_D;
        // GX_SetBankForARM7( GX_VRAM_ARM7_256_CD ) に相当するレジスタ設定処理
        reg_GX_VRAMCNT_C = ((2 << REG_GX_VRAMCNT_C_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
        reg_GX_VRAMCNT_D = ((2 << REG_GX_VRAMCNT_D_MST_SHIFT) |
                            (1 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
        break;
    default:
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_INVALID_PARAM;
    }

    // PXIコールバック関数を設定
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM9))
    {
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, WvrReceiveCallback);
    }

    // コールバック関数を退避
    if (callback == NULL)
    {
        wvrCallback = WvrDummyAsyncCallback;
    }
    else
    {
        wvrCallback = callback;
    }
    wvrArg = arg;

    // PXI経由でStartUpを通知
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_WVR, WVR_PXI_COMMAND_STARTUP, FALSE))
    {
        // 一旦排他ロックした VRAM を排他解除
        OSi_UnlockVram(wvrVRam, wvrLockId);
        wvrVRam = 0;
        wvrCallback = NULL;
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_FIFO_ERROR;
    }

    // 非同期処理を正常に開始完了
    (void)OS_RestoreInterrupts(e);
    return WVR_RESULT_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WVR_TerminateAsync

  Description:  無線ライブラリを強制停止する。
                非同期処理完了後、VRAM( C or D )へのアクセスは許可される。

  Arguments:    callback    -   処理完了時のコールバック関数を指定。
                arg         -   コールバック関数に渡す引数を指定。

  Returns:      処理結果を返す。
 *---------------------------------------------------------------------------*/
WVRResult WVR_TerminateAsync(WVRCallbackFunc callback, void *arg)
{
    OSIntrMode e;

    // コンポーネントの対応を確認
    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7))
    {
        return WVR_RESULT_DISABLE;
    }

    // 割込み禁止
    e = OS_DisableInterrupts();

    // WVR非同期関数が動作中でないか確認
    if (wvrCallback != NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }

    // PXIコールバック関数を設定
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM9))
    {
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, WvrReceiveCallback);
    }

    // コールバック関数を退避
    if (callback == NULL)
    {
        wvrCallback = WvrDummyAsyncCallback;
    }
    else
    {
        wvrCallback = callback;
    }
    wvrArg = arg;

    // PXI経由でTerminateを通知
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_WVR, WVR_PXI_COMMAND_TERMINATE, FALSE))
    {
        wvrCallback = NULL;
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_FIFO_ERROR;
    }

    // 非同期処理を正常に開始完了
    (void)OS_RestoreInterrupts(e);
    return WVR_RESULT_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WvrReceiveCallback

  Description:  FIFOを介したARM7からのコールバックを受信する。

  Arguments:    tag          - 未使用。
                data         - ARM7から受信したデータ。
                err          - 未使用。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WvrReceiveCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag , err )

    WVRCallbackFunc cb = wvrCallback;
    void   *cbArg = wvrArg;
    WVRResult result = (WVRResult)(data & 0x000000ff);

    // コマンド種別により処理を振り分け
    switch (data & 0xffff0000)
    {
    case WVR_PXI_COMMAND_STARTUP:
        switch (result)
        {
        case WVR_RESULT_FATAL_ERROR:
            // VRAM を排他解除
            if ((wvrVRam != 0) && (wvrLockId != 0))
            {
                OSi_UnlockVram(wvrVRam, wvrLockId);
                wvrVRam = 0;
            }
            break;
        }
        break;
    case WVR_PXI_COMMAND_TERMINATE:
        switch (result)
        {
        case WVR_RESULT_SUCCESS:
            // VRAM を排他解除
            if ((wvrVRam != 0) && (wvrLockId != 0))
            {
                OSi_UnlockVram(wvrVRam, wvrLockId);
                wvrVRam = 0;
            }
            break;
        }
        // PXI コールバック先設定を NULL クリア
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, NULL);
        break;
    }

    // ユーザーコールバック呼び出し
    if (cb != NULL)
    {
        wvrCallback = NULL;
        wvrArg = NULL;
        cb(cbArg, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WvrDummyAsyncCallback

  Description:  非同期関数に設定するダミーのコールバック関数。

  Arguments:    arg     -   未使用。
                result  -   非同期処理の処理結果。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WvrDummyAsyncCallback(void *arg, WVRResult result)
{
#pragma unused( arg , result )
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
