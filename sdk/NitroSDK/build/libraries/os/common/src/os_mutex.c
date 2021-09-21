/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - 
  File:     os_mutex.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_mutex.c,v $
  Revision 1.17  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.16  2005/07/08 08:35:28  yada
  remove unnecessary line

  Revision 1.15  2005/06/28 09:49:19  yada
  applied for SDK_THREAD_INFINITY

  Revision 1.14  2005/06/22 12:15:49  yada
  fix OSi_EnqueueTail().

  Revision 1.13  2005/06/02 01:12:40  yada
  fixed that when locking mutex with OS_TryLockMutex(), it's not added to mutex list.

  Revision 1.12  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.11  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.10  2004/11/02 10:35:03  yada
  add some assert

  Revision 1.9  2004/09/02 06:24:03  yada
  only change comment

  Revision 1.8  2004/04/02 12:48:49  yada
  mutexにリストを追加し、スレッド指定で全てのmutexを解放できるようにした。

  Revision 1.7  2004/03/01 11:54:24  yada
  英語の関数comment作成

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  2004/01/18 02:25:37  yada
  インデント等の整形

  Revision 1.4  2003/12/25 07:31:26  yada
  型ルール統一による変更

  Revision 1.3  2003/12/11 11:53:36  yada
  OS_CreateThreadQueue() → OS_InitThreadQueue() の改名による修正

  Revision 1.2  2003/11/30 04:19:25  yasu
  OS_EnableInterrupt/DisableInterrupt の戻り値の型の変更

  Revision 1.1  2003/11/29 10:10:29  yada
  初版


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>
#include <nitro.h>

void    OSi_EnqueueTail(OSThread *thread, OSMutex *mutex);
void    OSi_DequeueItem(OSThread *thread, OSMutex *mutex);
OSMutex *OSi_DequeueHead(OSThread *thread);

/*---------------------------------------------------------------------------*
  Name:         OS_InitMutex

  Description:  initialize mutex

  Arguments:    mutex       pointer to mutex structure
                            to be initialized

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_InitMutex(OSMutex *mutex)
{
    SDK_ASSERT(mutex);

    OS_InitThreadQueue(&mutex->queue);
    mutex->thread = NULL;
    mutex->count = 0;
}

/*---------------------------------------------------------------------------*
  Name:         OS_LockMutex

  Description:  lock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_LockMutex(OSMutex *mutex)
{
    OSIntrMode saved;
    OSThread *currentThread;
    OSThread *ownerThread;

    SDK_ASSERT(mutex);

    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    for (;;)
    {
        ownerThread = ((volatile OSMutex *)mutex)->thread;

        // ---- able to lock mutex
        if (ownerThread == NULL)
        {
            mutex->thread = currentThread;
            mutex->count++;
            OSi_EnqueueTail(currentThread, mutex);
            break;
        }
        // ---- current thread is same with thread locking mutex
        else if (ownerThread == currentThread)
        {
            mutex->count++;
            break;
        }
        // ---- current thread is different from locking mutex
        else
        {
            currentThread->mutex = mutex;
            OS_SleepThread(&mutex->queue);
            currentThread->mutex = NULL;
        }
    }

    (void)OS_RestoreInterrupts(saved);
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockMutex

  Description:  unlock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_UnlockMutex(OSMutex *mutex)
{
    OSIntrMode saved;
    OSThread *currentThread;

    SDK_ASSERT(mutex);

    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    if (mutex->thread == currentThread && --mutex->count == 0)
    {
        OSi_DequeueItem(currentThread, mutex);
        mutex->thread = NULL;

        // ---- wakeup threads entered in mutex thread queue
        OS_WakeupThread(&mutex->queue);
    }

    (void)OS_RestoreInterrupts(saved);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_UnlockAllMutex

  Description:  unlocks all the mutexes locked by the thread

  Arguments:    mutex       pointer to mutex structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_UnlockAllMutex(OSThread *thread)
{
    OSMutex *mutex;

    SDK_ASSERT(thread);

#ifndef SDK_THREAD_INFINITY
    while (thread->mutexQueueHead)
    {
        mutex = OSi_DequeueHead(thread);
        SDK_ASSERT(mutex->thread == thread);

        mutex->count = 0;
        mutex->thread = NULL;
        OS_WakeupThread(&(mutex->queue));
    }
#else
    while (thread->mutexQueue.head)
    {
        mutex = OSi_RemoveMutexLinkFromQueue(&thread->mutexQueue);

        SDK_ASSERT(mutex->thread == thread);
        mutex->count = 0;
        mutex->thread = NULL;
        OS_WakeupThread(&mutex->queue);

    }
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockMutex

  Description:  try to lock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      True if lock
 *---------------------------------------------------------------------------*/
BOOL OS_TryLockMutex(OSMutex *mutex)
{
    OSIntrMode saved;
    OSThread *currentThread;
    BOOL    locked;

    SDK_ASSERT(mutex);

    saved = OS_DisableInterrupts();
    currentThread = OS_GetCurrentThread();

    // ---- able to lock mutex
    if (mutex->thread == NULL)
    {
        mutex->thread = currentThread;
        mutex->count++;
        OSi_EnqueueTail(currentThread, mutex);
        locked = TRUE;
    }
    // ---- current thread is same with thread locking mutex
    else if (mutex->thread == currentThread)
    {
        mutex->count++;
        locked = TRUE;
    }
    // ---- current thread is different from locking mutex
    else
    {
        locked = FALSE;
    }

    (void)OS_RestoreInterrupts(saved);
    return locked;
}


//===========================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_EnqueueTail

  Description:  internal function.
                add mutex to thread's mutex list

  Arguments:    thread      pointer to thread
                mutex       pointer to mutex to be add

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_EnqueueTail(OSThread *thread, OSMutex *mutex)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex *prev = thread->mutexQueueTail;

    SDK_ASSERT(thread && mutex);

    if (!prev)
    {
        thread->mutexQueueHead = mutex;
    }
    else
    {
        prev->next = mutex;
    }

    mutex->prev = prev;
    mutex->next = NULL;
    thread->mutexQueueTail = mutex;
#else
    OSMutex *prev = thread->mutexQueue.tail;

    SDK_ASSERT(thread && mutex);

    if (!prev)
    {
        thread->mutexQueue.head = mutex;
    }
    else
    {
        prev->link.next = mutex;
    }

    mutex->link.prev = prev;
    mutex->link.next = NULL;
    thread->mutexQueue.tail = mutex;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OSi_DequeueItem

  Description:  internal function.
                remove specified mutex from thread's mutex list

  Arguments:    thread      pointer to thread
                mutex       pointer to mutex to be remove

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_DequeueItem(OSThread *thread, OSMutex *mutex)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex *next = mutex->next;
    OSMutex *prev = mutex->prev;

    SDK_ASSERT(thread && mutex);

    if (!next)
    {
        thread->mutexQueueTail = prev;
    }
    else
    {
        next->prev = prev;
    }

    if (!prev)
    {
        thread->mutexQueueHead = next;
    }
    else
    {
        prev->next = next;
    }
#else
    OSMutex *next = mutex->link.next;
    OSMutex *prev = mutex->link.prev;

    SDK_ASSERT(thread && mutex);

    if (!next)
    {
        thread->mutexQueue.tail = prev;
    }
    else
    {
        next->link.prev = prev;
    }

    if (!prev)
    {
        thread->mutexQueue.head = next;
    }
    else
    {
        prev->link.next = next;
    }
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OSi_DequeueHead

  Description:  remove top mutex from thread's list, and return mutex

  Arguments:    thread      pointer to thread

  Returns:      mutex which listed at top of thread
 *---------------------------------------------------------------------------*/
OSMutex *OSi_DequeueHead(OSThread *thread)
{
#ifndef SDK_THREAD_INFINITY
    OSMutex *mutex = thread->mutexQueueHead;
    OSMutex *next = mutex->next;

    SDK_ASSERT(thread);

    if (!next)
    {
        thread->mutexQueueTail = NULL;
    }
    else
    {
        next->prev = NULL;
    }

    thread->mutexQueueHead = next;
#else
    OSMutex *mutex = thread->mutexQueue.head;
    OSMutex *next = mutex->link.next;

    SDK_ASSERT(thread);

    if (!next)
    {
        thread->mutexQueue.tail = NULL;
    }
    else
    {
        next->link.prev = NULL;
    }

    thread->mutexQueue.head = next;
#endif

    return mutex;
}
