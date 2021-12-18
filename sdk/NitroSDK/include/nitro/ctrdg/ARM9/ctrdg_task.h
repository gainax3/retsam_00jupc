/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - include
  File:     ctrdg_task.h

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_task.h,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:28:00  okubata_ryoma
  CTRDG_SetTaskThreadPriority関数を公開、スレッドの優先度の変更

  Revision 1.1  2006/04/05 10:34:15  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_CTRDG_TASK_H_
#define	NITRO_CTRDG_TASK_H_

#include <nitro.h>

/* タスクスレッド優先レベル */
#define CTRDG_TASK_PRIORITY_DEFAULT 20 // mainスレッドよりも優先度が低くて、タスクスレッドとmainスレッドの間にいくつかのスレッドを作れるように

/* タスクスレッドのスタックサイズ */
#define CTRDG_TASK_STACK_SIZE 1024

struct CTRDGTaskInfo_tag;

// 引数はCTRDGTaskInfo_tagの関数ポインタを宣言
typedef u32 (*CTRDG_TASK_FUNC) (struct CTRDGTaskInfo_tag *);

/*
 * タスクスレッドに要求するタスク情報構造体.
 */
typedef struct CTRDGTaskInfo_tag
{
    // スレッドで待ちタスクリストにタスクがくるのを待ってるループを抜けてから、タスクの実行が終わるまでの間TRUE
    CTRDG_TASK_FUNC task;              /* task function */
    CTRDG_TASK_FUNC callback;          /* callback */
    u32     result;                    /* task 関数の返り値 */
    u8     *data;                      /* 書き込みたいデータ programコマンド以外は使用しません */
    u8     *adr;                       /* 読み書きしたいデータのアドレス */
    u32     offset;                    /* セクタ内のバイト単位のオフセット */
    u32     size;                      /* サイズ */
    u8     *dst;                       /* リードしたデータを格納するワーク領域のアドレス */
    u16     sec_num;                   /* セクタNo */
    u8      busy;                      /* if now processing */
    u8      param[1];                  /* user-defined argument and return-value */
}
CTRDGTaskInfo;

typedef struct
{
    OSThread th[1];                    /* thread context */
    CTRDGTaskInfo *volatile list;      /* waiting task list */
    CTRDGTaskInfo end_task;            /* task structure for end-command */
}
CTRDGiTaskWork;

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskThread

  Description:  タスクスレッドを起動する.
                
  Arguments:    p_work     内部ワーク用のバッファ.
                           CTRDGi_EndTaskThread() 完了時まで内部で使用される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDGi_InitTaskThread(void *p_work);

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskAvailable

  Description:  タスクスレッドが現在使用可能か判定.
                
  Arguments:    None.

  Returns:      現在使用可能なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL    CTRDGi_IsTaskAvailable(void);

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskInfo

  Description:  タスク情報構造体を初期化する.
                使用する前に 1 回だけ呼び出す必要がある.

  Arguments:    pt         未初期化のタスク情報構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDGi_InitTaskInfo(CTRDGTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskBusy

  Description:  タスク情報が現在使用中か判定.
                
  Arguments:    pt         タスク情報

  Returns:      現在使用中なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL    CTRDGi_IsTaskBusy(volatile const CTRDGTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_SetTask

  Description:  タスクを内部スレッドに追加する.
                
  Arguments:    pt         現在使用中でないタスク情報
                task       タスク関数
                callback   タスク完了時のコールバック (NULL なら無視)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDGi_SetTask(CTRDGTaskInfo * pt, CTRDG_TASK_FUNC task, CTRDG_TASK_FUNC callback);

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_EndTaskThread

  Description:  タスクスレッドを終了する.
                
  Arguments:    callback   タスクスレッド終了時のコールバック (NULL なら無視)
                           このコールバックはタスクスレッド終了寸前の状態で
                           割り込みを禁止したまま呼び出される.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDGi_EndTaskThread(CTRDG_TASK_FUNC callback);

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetTaskThreadPriority

  Description:  タスクスレッドの優先度を変更する。
                
  Arguments:    priority   タスクスレッドの優先度
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CTRDG_SetTaskThreadPriority(u32 priority);

#endif /* NITRO_CTRDG_TASK_H_ */
