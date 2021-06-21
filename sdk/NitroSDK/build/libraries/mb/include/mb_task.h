/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_task.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_task.h,v $
  Revision 1.3  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.2  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.1  2004/11/11 11:51:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if	!defined(NITRO_MB_TASK_H_)
#define NITRO_MB_TASK_H_


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* constant ---------------------------------------------------------------- */

/* タスクスレッド優先レベルの拡張定義 */
    /* 呼び出し元より 1 だけ高い優先レベルを指定 */
#define MB_TASK_PRIORITY_ABOVE  (OS_THREAD_PRIORITY_MAX + 1)
    /* 呼び出し元より 1 だけ低い優先レベルを指定 */
#define MB_TASK_PRIORITY_BELOW  (OS_THREAD_PRIORITY_MAX + 2)
    /* 呼び出し元と同じ優先レベルを指定 */
#define MB_TASK_PRIORITY_NORMAL (OS_THREAD_PRIORITY_MAX + 3)

#define MB_TASK_WORK_MIN	(sizeof(OSThread) + 256)


/* struct ------------------------------------------------------------------ */

    struct MBiTaskInfo_tag;

    typedef void (*MB_TASK_FUNC) (struct MBiTaskInfo_tag *);

/*
 * タスクスレッドに要求するタスク情報構造体.
 */
    typedef struct MBiTaskInfo_tag
    {
/* private: */
        struct MBiTaskInfo_tag *next;  /* next element as list */
        u32     busy:1;                /* if now processing, set 1 */
        u32     priority:31;           /* thread priority */
        MB_TASK_FUNC task;             /* task function */
        MB_TASK_FUNC callback;         /* callback */
/* public: */
        u32     param[4];              /* user-defined argument and return-value */
    }
    MBiTaskInfo;



/* function ---------------------------------------------------------------- */


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
    void    MBi_InitTaskThread(void *p_work, u32 size);

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskAvailable

  Description:  タスクスレッドが現在使用可能か判定.
                
  Arguments:    None.

  Returns:      現在使用可能なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_IsTaskAvailable(void);

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskInfo

  Description:  タスク情報構造体を初期化する.
                使用する前に 1 回だけ呼び出す必要がある.

  Arguments:    pt         未初期化のタスク情報構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_InitTaskInfo(MBiTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskBusy

  Description:  タスク情報が現在使用中か判定.
                
  Arguments:    pt         タスク情報

  Returns:      現在使用中なら TRUE, そうでないなら FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_IsTaskBusy(volatile const MBiTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         MBi_SetTask

  Description:  タスクを内部スレッドに追加する.
                
  Arguments:    pt         現在使用中でないタスク情報
                task       タスク関数
                callback   タスク完了時のコールバック (NULL なら無視)
                priority   タスク実行中のスレッド優先度

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_SetTask(MBiTaskInfo * pt, MB_TASK_FUNC task, MB_TASK_FUNC callback, u32 priority);

/*---------------------------------------------------------------------------*
  Name:         MBi_EndTaskThread

  Description:  タスクスレッドを終了する.
                
  Arguments:    callback   タスクスレッド終了時のコールバック (NULL なら無視)
                           このコールバックはタスクスレッド終了寸前の状態で
                           割り込みを禁止したまま呼び出される.
  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_EndTaskThread(MB_TASK_FUNC callback);


#if	defined(__cplusplus)
}
#endif


#endif                                 /* NITRO_MB_TASK_H_ */
