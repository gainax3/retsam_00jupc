/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_common.h

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_common.h,v $
  Revision 1.30  2007/11/13 04:22:02  yosizaki
  support erase-SUBSECTOR.

  Revision 1.29  2007/10/04 13:11:37  yosizaki
  add test commands.

  Revision 1.28  2007/06/06 01:43:30  yasu
  著作年度修正

  Revision 1.27  2007/05/28 23:31:52  yosizaki
  add CARD_SetCacheFlushThreshold.

  Revision 1.26  2006/04/28 07:43:40  yosizaki
  add DeviceCaps.

  Revision 1.25  2006/04/10 04:14:26  yosizaki
  add cmd->spec (erase timeout)

  Revision 1.24  2006/01/18 02:11:19  kitase_hirotake
  do-indent

  Revision 1.23  2005/11/07 01:09:47  okubata_ryoma
  SDK_ASSERT_ON_COMPILEからSDK_COMPILER_ASSERTに変更

  Revision 1.22  2005/11/01 06:14:25  okubata_ryoma
  SDK_ASSERT_ON_COMPILEを共用に

  Revision 1.21  2005/10/13 01:14:40  yosizaki
  add initial_status.

  Revision 1.20  2005/09/02 07:03:35  yosizaki
  add CARD_STAT_CANCEL

  Revision 1.19  2005/07/04 01:27:22  yosizaki
  fix size of padding in SDK_THREAD_INFINITY.

  Revision 1.18  2005/06/30 00:00:58  yosizaki
  fix padding for alignment.

  Revision 1.17  2005/06/28 09:52:53  yada
  applied for SDK_THREAD_INFINITY

  Revision 1.16  2005/05/12 11:19:14  yosizaki
  optimize some functions and calling interfaces.
  add comment.

  Revision 1.15  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.14  2005/01/24 06:35:09  yosizaki
  copyright 年表記修正.

  Revision 1.13  2005/01/19 12:17:55  yosizaki
  delete pragma `pack'.

  Revision 1.12  2004/12/15 09:44:45  yasu
  CARD アクセスイネーブラ機構の追加

  Revision 1.11  2004/12/08 12:39:18  yosizaki
  add device-spec members.

  Revision 1.10  2004/11/12 05:44:53  yosizaki
  add CARDi_CheckValidity().

  Revision 1.9  2004/08/23 10:38:23  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.8  2004/08/18 03:24:03  yosizaki
  fix around PXI-INIT command.

  Revision 1.7  2004/07/28 13:18:14  yosizaki
  asynchronous mode support.

  Revision 1.6  2004/07/27 06:47:31  yosizaki
  small fix (warning "padding inserted")

  Revision 1.5  2004/07/24 08:23:55  yasu
  Change type of card owner ID (with error status) to s32.

  Revision 1.4  2004/07/23 08:28:23  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.3  2004/07/22 11:33:28  yosizaki
  fix CARD access of ARM7.

  Revision 1.2  2004/07/19 13:14:32  yosizaki
  add task thread.

  Revision 1.1  2004/07/10 10:20:05  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __NITRO_CARD_COMMON_H__
#define __NITRO_CARD_COMMON_H__


#include <nitro.h>
#include <nitro/pxi.h>


/*****************************************************************************/
/* macro */

/* 配列の要素数取得 */
#define	COUNT_OF_(array)	(sizeof(array) / sizeof(*array))

/* ビット演算マクロ */
#define	BIT_MASK(n)	((1 << (n)) - 1)
#define	ALIGN_MASK(a)	((a) - 1)
#define	ALIGN_BYTE(n, a)	(((u32)(n) + ALIGN_MASK(a)) & ~ALIGN_MASK(a))
#define CARD_ALIGN_HI_BIT(n)     (((u32)(n)) & ~ALIGN_MASK(CARD_ROM_PAGE_SIZE))
#define CARD_ALIGN_LO_BIT(n)     (((u32)(n)) & ALIGN_MASK(CARD_ROM_PAGE_SIZE))


/*****************************************************************************/
/* constant */

/* CARD 内部状態 */
enum
{
    CARD_STAT_INIT = (1 << 0),         /* 初期化済み */
    CARD_STAT_INIT_CMD = (1 << 1),     /* コマンドPXI初期化済み */
    CARD_STAT_BUSY = (1 << 2),         /* リード関数自体の処理中 */
    CARD_STAT_TASK = (1 << 3),         /* タスクスレッドへ移管中 */
    CARD_STAT_RECV = (1 << 4),         /* PXI 経由リクエストを受信中 */
    CARD_STAT_REQ = (1 << 5),          /* タスク処理内で PXI 経由の依頼中 */
    CARD_STAT_CANCEL = (1 << 6)        /* キャンセル要求中 */
};

typedef enum
{
    CARD_TARGET_NONE,
    CARD_TARGET_ROM,
    CARD_TARGET_BACKUP
}
CARDTargetMode;

/* バックアップデバイス機能 (内部使用) */
#define CARD_BACKUP_CAPS_AVAILABLE      (u32)(CARD_BACKUP_CAPS_READ - 1)
#define CARD_BACKUP_CAPS_READ           (u32)(1 << CARD_REQ_READ_BACKUP)
#define CARD_BACKUP_CAPS_WRITE          (u32)(1 << CARD_REQ_WRITE_BACKUP)
#define CARD_BACKUP_CAPS_PROGRAM        (u32)(1 << CARD_REQ_PROGRAM_BACKUP)
#define CARD_BACKUP_CAPS_VERIFY         (u32)(1 << CARD_REQ_VERIFY_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_PAGE     (u32)(1 << CARD_REQ_ERASE_PAGE_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_SECTOR   (u32)(1 << CARD_REQ_ERASE_SECTOR_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_CHIP     (u32)(1 << CARD_REQ_ERASE_CHIP_BACKUP)
#define CARD_BACKUP_CAPS_READ_STATUS    (u32)(1 << CARD_REQ_READ_STATUS)
#define CARD_BACKUP_CAPS_WRITE_STATUS   (u32)(1 << CARD_REQ_WRITE_STATUS)
#define CARD_BACKUP_CAPS_ERASE_SUBSECTOR    (u32)(1 << CARD_REQ_ERASE_SUBSECTOR_BACKUP)


/*****************************************************************************/
/* declaration */

typedef s32 CARDiOwner;                // s32 for error status


/*
 * PXI コマンドリクエスト送受信パラメータ.
 */
typedef struct CARDiCommandArg
{
    CARDResult result;                 /* 戻り値 */
    CARDBackupType type;               /* デバイスタイプ */
    u32     id;                        /* カード ID */
    u32     src;                       /* 転送元 */
    u32     dst;                       /* 転送先 */
    u32     len;                       /* 転送長 */

    struct
    {
        /* memory property */
        u32     total_size;            /* memory capacity (BYTE) */
        u32     sect_size;             /* write-sector (BYTE) */
        u32     subsect_size;          /* write-sector (BYTE) */
        u32     page_size;             /* write-page (BYTE) */
        u32     addr_width;            /* address width on command (BYTE) */
        /* maximun wait-time (as LCD-V-COUNT tick) */
        u32     program_page;
        u32     write_page;
        u32     write_page_total;
        u32     erase_chip;
        u32     erase_chip_total;
        u32     erase_sector;
        u32     erase_sector_total;
        u32     erase_subsector;
        u32     erase_subsector_total;
        u32     erase_page;
        /* initial parameter of status register */
        u8      initial_status;
        u8      padding1[3];
        u32     caps;
        u8      padding2[4];
    }
    spec;

}
CARDiCommandArg;


/* CARD 共通パラメータ */
typedef struct CARDiCommon
{
    /* コマンド引数用共有メモリ */
    CARDiCommandArg *cmd;

    int     command;                   /* 現在のコマンド */
#if	defined(SDK_ARM7)
    u32     recv_step;                 /* 受信ステップ数 */
#endif

    /*
     * カードアクセス権管理.
     * プロセッサ内のカード/バックアップ排他を取る.
     * これは, CARD アクセスを使用する複数の非同期関数(Rom&Backup)が
     * 同一スレッドから呼び出される場合があるために必要.
     *
     * OSMutex がスレッドに付いて回るのに対し, これは lock-ID に付いて回る.
     */
    volatile CARDiOwner lock_owner;    // ==s32 with Error status
    volatile int lock_ref;
#ifndef SDK_THREAD_INFINITY
    OSThreadQueue lock_queue[4 / sizeof(OSThreadQueue)];
#else
    OSThreadQueue lock_queue[1];
#endif
    CARDTargetMode lock_target;

    /* API タスクパラメータ */
    u32     src;                       /* 転送元 */
    u32     dst;                       /* 転送先 */
    u32     len;                       /* 転送サイズ */
    u32     dma;                       /* DMA チャンネル */
    CARDRequest req_type;              /* リクエストタイプ */
    int     req_retry;                 /* リクエストのリトライ回数 */
    CARDRequestMode req_mode;          /* リクエストの動作モード */
    MIDmaCallback callback;            /* コールバック */
    void   *callback_arg;              /* コールバック引数 */
    void    (*task_func) (struct CARDiCommon *);        /* タスク */

    /* タスクスレッド */
    OSThread thread[1];                /* スレッド本体 */
    OSThread *cur_th;                  /* タスク処理中のスレッド.
                                          ユーザスレッドのこともある */
    u32     priority;
#ifndef SDK_THREAD_INFINITY
    OSThreadQueue busy_q[4 / sizeof(OSThreadQueue)];    /* タスク完了待ちスレッド */
#else
    OSThreadQueue busy_q[1];           /* タスク完了待ちスレッド */
#endif

    /* 状態フラグ */
    volatile u32 flag;

#if	defined(SDK_ARM9)
    u32     flush_threshold_ic;        /* 命令キャッシュ全体無効化閾値 */
    u32     flush_threshold_dc;        /* データキャッシュ全体無効化閾値 */
#endif

    /* padding */
#ifndef SDK_THREAD_INFINITY
    u8      dummy[20];
#endif

    /* 最後に転送したバックアップページのキャッシュ */
    u8      backup_cache_page_buf[256] ATTRIBUTE_ALIGN(32);

}
CARDiCommon;


SDK_COMPILER_ASSERT(sizeof(CARDiCommandArg) % 32 == 0);

SDK_COMPILER_ASSERT(sizeof(CARDiCommon) % 32 == 0);


/*****************************************************************************/
/* variable */

extern CARDiCommon cardi_common;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendPxi

  Description:  PXI FIFO ワード送信.

  Arguments:    data       送信データ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_SendPxi(u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_FS, data, TRUE) < 0)
        ;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO ワード受信コールバック.

  Arguments:    tag        PXI タグ (常に PXI_FIFO_TAG_FS)
                data       受信データ
                err        エラービット (旧仕様によるもの)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err);

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetTargetMode

  Description:  CARD バスの現在のロックターゲットを取得.

  Arguments:    None.

  Returns:      CARDTargetMode で示される 3 つの状態のいずれか.
 *---------------------------------------------------------------------------*/
static inline CARDTargetMode CARDi_GetTargetMode(void)
{
    return cardi_common.lock_target;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitAsync

  Description:  非同期完了を待つ.

  Arguments:    None.

  Returns:      最新の処理結果が CARD_RESULT_SUCCESS であれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_WaitAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryWaitAsync

  Description:  非同期完了待機を試行し, 成否に関わらずただちに制御を返す.

  Arguments:    None.

  Returns:      最新の非同期処理が完了していれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_TryWaitAsync(void);


/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitTask

  Description:  タスクスレッドの使用権利を取得できるまで待つ.
                (所定のバスのロックは, この関数の呼び出し元で保証する)

  Arguments:    p            ライブラリのワークバッファ (効率のために引数渡し)
                callback     アクセス終了後のコールバック関数
                callback_arg コールバック関数の引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_WaitTask(CARDiCommon * p, MIDmaCallback callback, void *callback_arg)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    while ((p->flag & CARD_STAT_BUSY) != 0)
    {
        OS_SleepThread(p->busy_q);
    }
    p->flag |= CARD_STAT_BUSY;
    p->callback = callback;
    p->callback_arg = callback_arg;
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetTask

  Description:  タスクスレッドにタスクを設定.
                (すでにタスクスレッドが CARDi_WaitTask() によって排他制御
                 されていることは, この関数の呼び出し元で保証する)

  Arguments:    task       設定するタスク関数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_SetTask(void (*task) (CARDiCommon *));

/*---------------------------------------------------------------------------*
  Name:         CARDi_EndTask

  Description:  タスク終了を通知してタスクスレッドの使用権利を解放.

  Arguments:    p            ライブラリのワークバッファ (効率のために引数渡し)
                is_own_task  自身のプロセッサでのタスクなら TRUE
                             (ARM7 による ARM9 タスク処理の完了なら FALSE)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_EndTask(CARDiCommon * p, BOOL is_own_task)
{
    const MIDmaCallback func = p->callback;
    void   *const arg = p->callback_arg;

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();

        p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK | CARD_STAT_CANCEL);
        OS_WakeupThread(p->busy_q);
        if ((p->flag & CARD_STAT_RECV) != 0)
        {
            OS_WakeupThreadDirect(p->thread);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }

    if (is_own_task && func)
    {
        (*func) (arg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskThread

  Description:  タスクスレッドのメイン関数.

  Arguments:    arg          不使用

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_TaskThread(void *arg);

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitCommon

  Description:  CARD ライブラリ共通部分の初期化.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_InitCommon(void);


#if	defined(SDK_ARM9)

/*---------------------------------------------------------------------------*
  Name:         CARDi_Request

  Description:  ARM9 から ARM7 へリクエスト送信し, 完了をブロッキング.
                結果が CARD_RESULT_SUCCESS でないなら指定回数までは再試行する.
                (所定のバスのロックおよびタスクスレッドの排他制御は,
                 この関数の呼び出し元で保証する)

  Arguments:    p            ライブラリのワークバッファ (効率のために引数渡し)
                req_type     コマンドリクエストの種類
                retry_max    リトライ最大回数

  Returns:      結果が CARD_RESULT_SUCCESS であれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_Request(CARDiCommon * p, int req_type, int retry_max);


#endif /* SDK_ARM9 */


#endif  /*__NITRO_CARD_SPI_H__*/
