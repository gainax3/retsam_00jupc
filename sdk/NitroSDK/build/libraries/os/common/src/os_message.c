/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_message.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_message.c,v $
  Revision 1.14  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.13  2005/02/28 05:26:29  yosizaki
  do-indent.

  Revision 1.12  2004/11/12 11:01:19  yada
  just fix comment

  Revision 1.11  2004/03/12 12:10:22  yada
  only fix comment

  Revision 1.10  2004/03/12 08:41:14  yada
  OS_PeepMessage()→OS_ReadMessage() に改名

  Revision 1.9  2004/03/12 02:38:51  yada
  OS_PeepMessage() 追加

  Revision 1.8  2004/03/01 11:56:47  yada
  英語の関数comment作成

  Revision 1.7  2004/02/12 11:08:30  yada
  ARM9/ARM7 振り分け作業

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

  Revision 1.1  2003/11/29 01:25:46  yada
  ファイル名称変更

  Revision 1.3  2003/11/26 04:18:14  yada
  (none)

  Revision 1.2  2003/11/26 01:50:27  yada
  関数の中身を実装

  Revision 1.1  2003/11/25 12:36:36  yada
  初版。関数名のみ


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include  <nitro/os.h>
#include  <nitro/os/common/message.h>

/*---------------------------------------------------------------------------*
  Name:         OS_InitMessageQueue

  Description:  initialize message queue

  Arguments:    mq          message queue
                msgArray    buffer for message queue
                msgCount    max massage size for buffer

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_InitMessageQueue(OSMessageQueue *mq, OSMessage *msgArray, s32 msgCount)
{
    OS_InitThreadQueue(&mq->queueSend);
    OS_InitThreadQueue(&mq->queueReceive);
    mq->msgArray = msgArray;
    mq->msgCount = msgCount;
    mq->firstIndex = 0;
    mq->usedCount = 0;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SendMessage

  Description:  send message to message queue

  Arguments:    mq          message queue
                msg         massage which is sent
                flags       whether wait or not when queue is full

  Returns:      TRUE if send
                
 *---------------------------------------------------------------------------*/
BOOL OS_SendMessage(OSMessageQueue *mq, OSMessage msg, s32 flags)
{
    OSIntrMode enabled;
    s32     lastIndex;

    enabled = OS_DisableInterrupts();

    while (mq->msgCount <= mq->usedCount)       // check for full queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueSend);
        }
    }

    // Store message
    lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
    mq->msgArray[lastIndex] = msg;
    mq->usedCount++;

    // Wakeup receiving threads if any
    OS_WakeupThread(&mq->queueReceive);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_ReceiveMessage

  Description:  receive message from message queue

  Arguments:    mq          message queue
                msg         massage which is received
                flags       whether wait or not when queue is empty

  Returns:      TRUE if reveice
                
 *---------------------------------------------------------------------------*/
BOOL OS_ReceiveMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->usedCount == 0)         // check for empty queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueReceive);
        }
    }

    // Copy-out message
    if (msg != NULL)
    {
        *msg = mq->msgArray[mq->firstIndex];
    }
    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;

    // Wakeup sending threads if any
    OS_WakeupThread(&mq->queueSend);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         OS_JamMessage

  Description:  send message to message queue, cutting in the top of the queue

  Arguments:    mq          message queue
                msg         massage which is sent
                flags       whether wait or not when queue is full

  Returns:      TRUE if send
                
 *---------------------------------------------------------------------------*/
BOOL OS_JamMessage(OSMessageQueue *mq, OSMessage msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->msgCount <= mq->usedCount)       // check for full queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueSend);
        }
    }

    // Store message at the front
    mq->firstIndex = (mq->firstIndex + mq->msgCount - 1) % mq->msgCount;
    mq->msgArray[mq->firstIndex] = msg;
    mq->usedCount++;

    // Wakeup receiving threads if any
    OS_WakeupThread(&mq->queueReceive);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         OS_ReadMessage

  Description:  read message at message queue. no change in queue
                not occur to switch threads.

  Arguments:    mq          message queue
                msg         massage which is received
                flags       whether wait or not when queue is empty

  Returns:      TRUE if read
                
 *---------------------------------------------------------------------------*/
BOOL OS_ReadMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->usedCount == 0)         // check for empty queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueReceive);
        }
    }

    // Copy-out message
    if (msg != NULL)
    {
        *msg = mq->msgArray[mq->firstIndex];
    }

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}
