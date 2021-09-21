/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_task.c

  Copyright 2003,2004,2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_task.c,v $
  Revision 1.3  2007/02/20 00:28:11  kitase_hirotake
  indent source

  Revision 1.2  2006/04/07 03:26:35  okubata_ryoma
  CTRDG_SetTaskThreadPriority関数を公開

  Revision 1.1  2006/04/05 10:48:30  okubata_ryoma
  AGBバックアップライブラリのSDK収録のための変更


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*******************************************************

    function's description

********************************************************/
static CTRDGiTaskWork *ctrdgi_task_work = NULL;
static CTRDGTaskInfo ctrdgi_task_list;

static void CTRDGi_TaskThread(void *arg);

u64     ctrdg_task_stack[CTRDG_TASK_STACK_SIZE / sizeof(u64)];
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskThread

  Description:  タスクスレッドを起動する.
                
  Arguments:    p_work     内部ワーク用のバッファ.
                           CTRDGi_EndTaskThread() 完了時まで内部で使用される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_InitTaskThread(void *p_work)
{
    // IRQ割り込みの禁止
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    // この構造体がNULLならスレッド作成
    if (!ctrdgi_task_work)
    {
        CTRDGiTaskWork *const p = (CTRDGiTaskWork *) p_work;

        // スレッドが初期化されていて使用可能か判断
        SDK_ASSERT(OS_IsThreadAvailable());

        /* ワーク構造体, スタックバッファ, タスクスレッドの準備 */
        // これでこの構造体がNULLでなくなるのでタスクスレッドは新しく作られない 
        ctrdgi_task_work = p;
        // end_taskの構造体を初期化
        CTRDGi_InitTaskInfo(&p->end_task);
        // ctrdgi_task_list構造体を初期化
        CTRDGi_InitTaskInfo(&ctrdgi_task_list);
        // 待っているタスクリストはこの時点ではないはずなのでNULLを入れる
        p->list = NULL;

        OS_CreateThread(p->th, CTRDGi_TaskThread, p,
                        ctrdg_task_stack + CTRDG_TASK_STACK_SIZE / sizeof(u64),
                        CTRDG_TASK_STACK_SIZE, CTRDG_TASK_PRIORITY_DEFAULT);
        OS_WakeupThreadDirect(p->th);
    }
    // IRQ割り込み許可を戻す
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskAvailable

  Description:  タスクスレッドが現在使用可能か判定.
                
  Arguments:    None.

  Returns:      現在使用可能なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsTaskAvailable(void)
{
    return (ctrdgi_task_work != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskInfo

  Description:  タスク情報構造体を初期化する.
                使用する前に 1 回だけ呼び出す必要がある.

  Arguments:    pt         未初期化のタスク情報構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_InitTaskInfo(CTRDGTaskInfo * pt)
{
    SDK_ASSERT(pt != NULL);
    MI_CpuClear8(pt, sizeof(*pt));
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskBusy

  Description:  タスク情報が現在使用中か判定.
                
  Arguments:    pt         タスク情報

  Returns:      現在使用中なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsTaskBusy(volatile const CTRDGTaskInfo * pt)
{
    return pt->busy != FALSE;
}

static void CTRDGi_TaskThread(void *arg)
{
    CTRDGiTaskWork *const p = (CTRDGiTaskWork *) arg;
    // スレッドを終了する命令がくるまでループ
    for (;;)
    {
        // 構造体を初期化
        CTRDGTaskInfo trg;
        MI_CpuClear8(&trg, sizeof(CTRDGTaskInfo));
        /* 次のタスクを取得 */
        {
            // IRQ割り込み禁止
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            /* アイドル状態ならスリープ */
            // 待っているタスクリストにタスクがくるまでループして待つ
            while (!p->list)
            {
                OS_SleepThread(NULL);
            }
            // 待ちのタスクリストにタスクがきたので、そのタスク情報構造体をtrgにコピー
            trg = *p->list;
            // IRQ割り込み許可を戻す
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        /* タスクを実行 */
        if (trg.task)
            // 関数ポインタでタスクを実行、引数はtrg
            trg.result = (u32)(*trg.task) (&trg);
        /* タスク完了コールバック実行 */
        // ここにきてるってことはタスクが終わっているのでタスクのコールバック
        {
            // IRQ割り込み禁止
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            // コールバック関数を設定
            CTRDG_TASK_FUNC callback = trg.callback;

            // タスクが実行中ではないはずなのでFALSE
            ctrdgi_task_list.busy = FALSE;
            // コールバック関数があれば
            if (callback)
                // 関数ポインタでコールバック関数を呼ぶ、引数はtrg
                (void)(*callback) (&trg);
            /*
             * 終了要求なら割り込み禁止のままスレッド終了.
             * (この禁止設定はコンテキスト切り替えの瞬間まで有効)
             */
            //if (p->list == &p->end_task)
            if (ctrdgi_task_work == NULL)
                break;

            // リスト構造体の初期化
            p->list = NULL;

            (void)OS_RestoreInterrupts(bak_cpsr);
        }
    }
    OS_TPrintf("task-thread end.\n");
    OS_ExitThread();
    return;
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_SetTask

  Description:  タスクを内部スレッドに追加する.
                
  Arguments:    pt         現在使用中でないタスク情報
                task       タスク関数
                callback   タスク完了時のコールバック (NULL なら無視)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_SetTask(CTRDGTaskInfo * pt, CTRDG_TASK_FUNC task, CTRDG_TASK_FUNC callback)
{
    // 現在のスレッドのポインタや、待ちタスクリストなどの入った構造体を入れる
    CTRDGiTaskWork *const p_work = ctrdgi_task_work;

    SDK_ASSERT(pt != NULL);
    SDK_ASSERT(CTRDGi_IsTaskAvailable());

    if (!CTRDGi_IsTaskAvailable())
    {
        OS_TPanic("CTRDGi_SetTask() failed! (task-thread is not available now)");
    }

    // もしスレッドの中でタスクが実行中ならダメ
    if (ctrdgi_task_list.busy)
    {
        OS_TPanic("CTRDGi_SetTask() failed! (specified structure is busy)");
    }

    /* タスク追加 */
    {
        // 構造体にパラメータを設定
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        pt->busy = TRUE;
        pt->task = task;
        pt->callback = callback;
        /* アイドル状態の新規タスクならスレッドを起動 */

        // そのタスクが終了コマンドなら
        if (pt == &p_work->end_task)
        {
            /* ここからタスクスレッドの利用を禁止する */
            ctrdgi_task_work = NULL;
        }
        // 待ちタスクリストにこのタスクの構造体を入れてタスクスレッドを起こす
        ctrdgi_task_list = *pt;
        // 実体のアドレスを格納する。
        p_work->list = &ctrdgi_task_list;
        OS_WakeupThreadDirect(p_work->th);

        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_EndTaskThread

  Description:  タスクスレッドを終了する.
                
  Arguments:    callback   タスクスレッド終了時のコールバック (NULL なら無視)
                           このコールバックはタスクスレッド終了寸前の状態で
                           割り込みを禁止したまま呼び出される.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_EndTaskThread(CTRDG_TASK_FUNC callback)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (CTRDGi_IsTaskAvailable())
    {
        (void)CTRDGi_SetTask(&ctrdgi_task_work->end_task, NULL, callback);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetTaskThreadPriority

  Description:  タスクスレッドの優先度を変更する。
                
  Arguments:    priority   タスクスレッドの優先度
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDG_SetTaskThreadPriority(u32 priority)
{
    if (ctrdgi_task_work)
    {
        CTRDGiTaskWork *const p = ctrdgi_task_work;
        (void)OS_SetThreadPriority(p->th, priority);
    }
}
