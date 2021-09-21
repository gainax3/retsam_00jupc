/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_task.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_task.c,v $
  Revision 1.8  2007/07/31 00:58:42  yosizaki
  update copyright

  Revision 1.7  2007/07/30 03:43:19  yosizaki
  fix about MB_End

  Revision 1.6  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.4  2005/02/24 07:34:25  yosizaki
  不要な #ifdef __cplusplus を除去.

  Revision 1.3  2005/01/27 11:27:32  yosizaki
  remove debug-output...

  Revision 1.2  2004/12/08 09:32:46  yosizaki
  fix around end of task-thread.

  Revision 1.1  2004/11/11 11:45:57  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "mb_task.h"


/* struct ------------------------------------------------------------------ */

typedef struct
{
    OSThread th[1];                    /* thread context */
    MBiTaskInfo *volatile list;        /* waiting task list */
    MBiTaskInfo end_task;              /* task structure for end-command */
}
MBiTaskWork;

/* variable ---------------------------------------------------------------- */

static MBiTaskWork *mbi_task_work = NULL;


/* function ---------------------------------------------------------------- */

static void MBi_TaskThread(void *arg)
{
    MBiTaskWork *const p = (MBiTaskWork *) arg;
    for (;;)
    {
        MBiTaskInfo *trg = NULL;
        /* 次のタスクを取得 */
        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            /* アイドル状態ならスリープ */
            while (!p->list)
            {
                (void)OS_SetThreadPriority(p->th, OS_THREAD_PRIORITY_MIN);
                OS_SleepThread(NULL);
            }
            trg = p->list;
            p->list = p->list->next;
            (void)OS_SetThreadPriority(p->th, trg->priority);
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        /* タスクを実行 */
        if (trg->task)
            (*trg->task) (trg);
        /* タスク完了コールバック実行 */
        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            MB_TASK_FUNC callback = trg->callback;
            /*
             * ここで, スレッド優先レベルについては慎重に操作する.
             * 1. 次のタスクが無いのであれば最高に指定. (待ちsleep)
             * 2. 次のタスクがあり現在より高ければそれに変更.
             * 3. 次のタスクがあり現在より低ければそのまま.
             * 優先レベルが現状より低くなることは無い.
             */
            const u32 cur_priority = OS_GetThreadPriority(p->th);
            u32     new_priority;
            if (!p->list)
                new_priority = OS_THREAD_PRIORITY_MIN;
            else if (cur_priority < p->list->priority)
                new_priority = p->list->priority;
            else
                new_priority = cur_priority;
            if (new_priority != cur_priority)
                (void)OS_SetThreadPriority(p->th, new_priority);
            trg->next = NULL;
            trg->busy = FALSE;
            if (callback)
                (*callback) (trg);
            /*
             * 終了要求なら割り込み禁止のままスレッド終了.
             * (この禁止設定はコンテキスト切り替えの瞬間まで有効)
             */
            if (trg == &p->end_task)
                break;
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
    }
    OS_TPrintf("task-thread end.\n");
    OS_ExitThread();
    return;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskThread

  Description:  タスクスレッドを起動する.
                
  Arguments:    p_work     内部ワーク用のバッファ.
                           MBi_EndTaskThread() 完了時まで内部で使用される.
                size       p_work のバイトサイズ.
                           MB_TASK_WORK_MIN 以上である必要があり,
                           size - MB_TASK_WORK_MIN がスタックに使用される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitTaskThread(void *p_work, u32 size)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (!mbi_task_work)
    {
        MBiTaskWork *const p = (MBiTaskWork *) p_work;

        SDK_ASSERT(size >= MB_TASK_WORK_MIN);
        SDK_ASSERT(OS_IsThreadAvailable());

        /* ワーク構造体, スタックバッファ, タスクスレッドの準備 */
        mbi_task_work = p;
        MBi_InitTaskInfo(&p->end_task);
        p->list = NULL;
        size = (u32)((size - sizeof(MBiTaskWork)) & ~3);
        OS_CreateThread(p->th, MBi_TaskThread, p,
                        (u8 *)(p + 1) + size, size, OS_THREAD_PRIORITY_MIN);
        OS_WakeupThreadDirect(p->th);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskAvailable

  Description:  タスクスレッドが現在使用可能か判定.
                
  Arguments:    None.

  Returns:      現在使用可能なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL MBi_IsTaskAvailable(void)
{
    return (mbi_task_work != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskInfo

  Description:  タスク情報構造体を初期化する.
                使用する前に 1 回だけ呼び出す必要がある.

  Arguments:    pt         未初期化のタスク情報構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitTaskInfo(MBiTaskInfo * pt)
{
    SDK_ASSERT(pt != NULL);
    MI_CpuClear8(pt, sizeof(*pt));
}

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskBusy

  Description:  タスク情報が現在使用中か判定.
                
  Arguments:    pt         タスク情報

  Returns:      現在使用中なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
BOOL MBi_IsTaskBusy(volatile const MBiTaskInfo * pt)
{
    return pt->busy != FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_SetTask

  Description:  タスクを内部スレッドに追加する.
                
  Arguments:    pt         現在使用中でないタスク情報
                task       タスク関数
                callback   タスク完了時のコールバック (NULL なら無視)
                priority   タスク実行中のスレッド優先度

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetTask(MBiTaskInfo * pt, MB_TASK_FUNC task, MB_TASK_FUNC callback, u32 priority)
{
    MBiTaskWork *const p_work = mbi_task_work;

    SDK_ASSERT(pt != NULL);

    /* ライブラリ終了時またはカード抜け時、静かに処理を無視するよう変更 */
    if (!MBi_IsTaskAvailable())
    {
        OS_TWarning("MBi_SetTask() ignored... (task-thread is not available now)");
        return;
    }
    if (pt->busy)
    {
        OS_TWarning("MBi_SetTask() ignored... (specified structure is busy)");
        return;
    }

    /* 優先レベルの拡張定義への対応 */
    if (priority > OS_THREAD_PRIORITY_MAX)
    {
        const u32 cur_priority = OS_GetThreadPriority(p_work->th);
        if (priority == MB_TASK_PRIORITY_ABOVE)
        {
            /* 呼び出し元より 1 だけ高優先 */
            priority = (u32)((cur_priority > OS_THREAD_PRIORITY_MIN) ?
                             (cur_priority - 1) : OS_THREAD_PRIORITY_MIN);
        }
        else if (priority == MB_TASK_PRIORITY_BELOW)
        {
            /* 呼び出し元より 1 だけ低優先 */
            priority = (u32)((cur_priority < OS_THREAD_PRIORITY_MAX) ?
                             (cur_priority + 1) : OS_THREAD_PRIORITY_MAX);
        }
        else if (priority == MB_TASK_PRIORITY_NORMAL)
        {
            /* 呼び出し元と同じ優先レベル */
            priority = cur_priority;
        }
        else
        {
            /* 単なる不正指定 */
            priority = OS_THREAD_PRIORITY_MAX;
        }
    }
    /* タスク追加 */
    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        pt->busy = TRUE;
        pt->priority = priority;
        pt->task = task;
        pt->callback = callback;
        /* アイドル状態の新規タスクならスレッドを起動 */
        if (!p_work->list)
        {

            if (pt == &p_work->end_task)
            {
                /* ここからタスクスレッドの利用を禁止する */
                mbi_task_work = NULL;
            }

            p_work->list = pt;
            OS_WakeupThreadDirect(p_work->th);
        }
        else
        {
            /* リストが空でなければ挿入 */
            MBiTaskInfo *pos = p_work->list;
            /* 終了コマンドなら常に終端に追加 */
            if (pt == &p_work->end_task)
            {
                while (pos->next)
                    pos = pos->next;
                pos->next = pt;
                /* ここからタスクスレッドの利用を禁止する */
                mbi_task_work = NULL;
            }
            /* 通常コマンドなら優先レベル順に挿入 */
            else
            {
                if (pos->priority > priority)
                {
                    p_work->list = pt;
                    pt->next = pos;
                }
                else
                {
                    while (pos->next && (pos->next->priority <= priority))
                        pos = pos->next;
                    pt->next = pos->next;
                    pos->next = pt;
                }
            }
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_EndTaskThread

  Description:  タスクスレッドを終了する.
                
  Arguments:    callback   タスクスレッド終了時のコールバック (NULL なら無視)
                           このコールバックはタスクスレッド終了寸前の状態で
                           割り込みを禁止したまま呼び出される.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_EndTaskThread(MB_TASK_FUNC callback)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (MBi_IsTaskAvailable())
    {
        MBi_SetTask(&mbi_task_work->end_task, NULL, callback, OS_THREAD_PRIORITY_MIN);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}
