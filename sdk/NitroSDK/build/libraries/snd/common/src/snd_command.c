/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_command.c

  Copyright 2004-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_command.c,v $
  Revision 1.8  2007/02/20 00:28:09  kitase_hirotake
  indent source

  Revision 1.7  2006/11/14 06:33:29  ida
  1.6の修正ミスを修正

  Revision 1.5  2006/03/20 01:57:52  ida
  SND_SetMuteTrackEx関数追加

  Revision 1.4  2006/01/18 02:12:29  kitase_hirotake
  do-indent

  Revision 1.3  2005/06/28 02:10:28  ida
  IsCommandAvailableチェック追加

  Revision 1.2  2005/04/14 06:52:23  ida
  SND_COMMAND_PREPARE_SEQコマンドの追加

  Revision 1.1  2005/04/12 06:16:12  ida
  SNDをNITRO-SDKへ移設

  Revision 1.27  2005/03/17 02:11:01  ida
  SND_COMMAND_SKIP_SEQコマンドの追加

  Revision 1.26  2005/03/01 02:14:22  ida
  SND_COMMAND_IMMEDIATEフラグ追加

  Revision 1.25  2005/02/21 02:37:17  ida
  警告対策

  Revision 1.24  2005/02/16 07:47:40  ida
  ドライバ情報取得関数追加

  Revision 1.23  2005/01/06 02:52:13  ida
  著作権年次表記を更新

  Revision 1.22  2005/01/05 06:59:10  ida
  ARM9からのサウンドレジスタ参照機能実装

  Revision 1.21  2004/12/27 01:17:32  ida
  待ちループ中では、割り込み禁止を解除するようにした

  Revision 1.20  2004/11/22 04:30:17  ida
  workaround thread safe

  Revision 1.19  2004/10/05 06:21:16  ida
  add SND_SetMasterPan

  Revision 1.18  2004/09/15 10:23:44  ida
  update status every time

  Revision 1.17  2004/08/19 06:50:01  ida
  work around alarm

  Revision 1.16  2004/08/10 02:29:41  ida
  add SND_StopUnlockedChannel

  Revision 1.15  2004/08/05 03:42:23  ida
  for ensata

  Revision 1.14  2004/08/02 05:31:28  ida
  unuse interrupt for reply command

  Revision 1.13  2004/07/20 01:21:20  ida
  disable interrupts in PxiFifoCallback

  Revision 1.12  2004/07/16 10:11:43  ida
  remove EnableIrq

  Revision 1.11  2004/07/16 06:42:49  ida
  fix Project name

  Revision 1.10  2004/07/16 04:18:52  ida
  invalidate range in every time

  Revision 1.9  2004/07/13 05:45:22  ida
  change function interface

  Revision 1.8  2004/07/09 06:51:39  ida
  fix shared work pointer send miss

  Revision 1.7  2004/07/06 07:45:59  ida
  change function argument

  Revision 1.6  2004/07/05 06:30:43  ida
  workaround shared work

  Revision 1.5  2004/06/29 06:44:42  ida
  remove SND_GetCurrentCommandList

  Revision 1.4  2004/06/28 07:15:14  ida
  change function interface

  Revision 1.3  2004/06/23 06:29:13  ida
  hwchannel -> channel
  hwcapture -> capture

  Revision 1.2  2004/06/21 01:16:12  ida
  change function interface

  Revision 1.1  2004/06/11 01:53:30  ida
  snd_port.c -> snd_command.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/command.h>

#include <nitro/misc.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/mi.h>

#include <nitro/snd/common/seq.h>
#include <nitro/snd/common/capture.h>
#include <nitro/snd/common/work.h>
#include <nitro/snd/common/global.h>
#include <nitro/snd/common/util.h>
#include <nitro/snd/common/alarm.h>
#include <nitro/snd/common/main.h>

/******************************************************************************
    macro definition
 ******************************************************************************/

#define SND_COMMAND_NUM 256

#define SND_PXI_FIFO_MESSAGE_BUFSIZE   8

#define SND_MSG_REQUEST_COMMAND_PROC   0

#define UNPACK_COMMAND( arg, shift, bit ) ( ( (arg) >> (shift) ) & ( ( 1 << (bit) ) - 1 ) )

/******************************************************************************
    static variable
 ******************************************************************************/

#ifdef SDK_ARM9

static SNDCommand sCommandArray[SND_COMMAND_NUM] ATTRIBUTE_ALIGN(32);   /* コマンド配列 */
static SNDCommand *sFreeList;          /* 空きコマンドリスト */
static SNDCommand *sFreeListEnd;       /* 空きコマンドリストの終端 */

static SNDCommand *sReserveList;       /* 予約済みコマンドリスト */
static SNDCommand *sReserveListEnd;    /* 予約済みコマンドリストの終端 */

static SNDCommand *sWaitingCommandListQueue[SND_PXI_FIFO_MESSAGE_BUFSIZE + 1];
static int sWaitingCommandListQueueRead;
static int sWaitingCommandListQueueWrite;

static int sWaitingCommandListCount;   /* 発行済みコマンドリストの数 */

static u32 sCurrentTag;                /* カレントタグ */
static u32 sFinishedTag;               /* 完了済みタグ */

static SNDSharedWork sSharedWork ATTRIBUTE_ALIGN(32);

#else  /* SDK_ARM7 */

static OSMessage sCommandMesgBuffer[SND_PXI_FIFO_MESSAGE_BUFSIZE];
static OSMessageQueue sCommandMesgQueue;

#endif /* SDK_ARM9 */


/******************************************************************************
    static function declaration
 ******************************************************************************/

static void PxiFifoCallback(PXIFifoTag tag, u32 data, BOOL err);
static void InitPXI(void);

#ifdef SDK_ARM9
static void RequestCommandProc(void);
static SNDCommand *AllocCommand(void);
static BOOL IsCommandAvailable(void);
#else
static void SetChannelTimer(u32 chBitMask, int timer);
static void SetChannelVolume(u32 chBitMask, int volume, SNDChannelDataShift shift);
static void SetChannelPan(u32 chBitMask, int pan);
static void StartTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
static void StopTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags);
static void ReadDriverInfo(SNDDriverInfo * info);
#endif

/******************************************************************************
    public function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SND_CommandInit

  Description:  コマンドライブラリの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_CommandInit(void)
{
#ifdef SDK_ARM9
    SNDCommand *command;
    int     i;
#endif /* SDK_ARM9 */

#ifdef SDK_ARM7
    OS_InitMessageQueue(&sCommandMesgQueue, sCommandMesgBuffer, SND_PXI_FIFO_MESSAGE_BUFSIZE);
#endif

    InitPXI();

#ifdef SDK_ARM9
    /* フリーリストの作成 */
    sFreeList = &sCommandArray[0];
    for (i = 0; i < SND_COMMAND_NUM - 1; i++)
    {
        sCommandArray[i].next = &sCommandArray[i + 1];
    }
    sCommandArray[SND_COMMAND_NUM - 1].next = NULL;
    sFreeListEnd = &sCommandArray[SND_COMMAND_NUM - 1];

    /* リザーブリストの初期化 */
    sReserveList = NULL;
    sReserveListEnd = NULL;

    /* その他変数初期化 */
    sWaitingCommandListCount = 0;

    sWaitingCommandListQueueRead = 0;
    sWaitingCommandListQueueWrite = 0;

    sCurrentTag = 1;
    sFinishedTag = 0;

    /* 共有ワークの初期化 */
    SNDi_SharedWork = &sSharedWork;
    SNDi_InitSharedWork(SNDi_SharedWork);

    command = SND_AllocCommand(SND_COMMAND_BLOCK);
    if (command != NULL)
    {
        command->id = SND_COMMAND_SHARED_WORK;
        command->arg[0] = (u32)SNDi_SharedWork;

        SND_PushCommand(command);
        (void)SND_FlushCommand(SND_COMMAND_BLOCK);
    }

#else  /* SDK_ARM7 */

    SNDi_SharedWork = NULL;

#endif /* SDK_ARM9 */
}


#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         SND_RecvCommandReply
  
  Description:  返答の受信(ThreadSafe)
  
  Arguments:    flags - BLOCK or NOBLOCK
  
  Returns:      処理済みコマンドリストまたは、NULL
  *---------------------------------------------------------------------------*/
const SNDCommand *SND_RecvCommandReply(u32 flags)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    SNDCommand *commandList;
    SNDCommand *commandListEnd;

    if (flags & SND_COMMAND_BLOCK)
    {
        while (sFinishedTag == SNDi_GetFinishedCommandTag())
        {
            // retry
            (void)OS_RestoreInterrupts(bak_psr);
            OS_SpinWait(100);
            bak_psr = OS_DisableInterrupts();
        }
    }
    else
    {
        if (sFinishedTag == SNDi_GetFinishedCommandTag())
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return NULL;
        }
    }

    /* 待ちコマンドリストからのＰＯＰ */
    commandList = sWaitingCommandListQueue[sWaitingCommandListQueueRead];
    sWaitingCommandListQueueRead++;
    if (sWaitingCommandListQueueRead > SND_PXI_FIFO_MESSAGE_BUFSIZE)
        sWaitingCommandListQueueRead = 0;

    /* コマンドリスト末尾の取得 */
    commandListEnd = commandList;
    while (commandListEnd->next != NULL)
    {
        commandListEnd = commandListEnd->next;
    }

    /* フリーリストの末尾へ結合 */
    if (sFreeListEnd != NULL)
    {
        sFreeListEnd->next = commandList;
    }
    else
    {
        sFreeList = commandList;
    }
    sFreeListEnd = commandListEnd;

    /* カウンタ更新 */
    sWaitingCommandListCount--;
    sFinishedTag++;

    (void)OS_RestoreInterrupts(bak_psr);
    return commandList;
}

/*---------------------------------------------------------------------------*
  Name:         SND_AllocCommand

  Description:  コマンドの確保(ThreadSafe)

  Arguments:    flags - BLOCK or NOBLOCK

  Returns:      コマンド
 *---------------------------------------------------------------------------*/
SNDCommand *SND_AllocCommand(u32 flags)
{
    SNDCommand *command;

    if (!IsCommandAvailable())
        return NULL;

    command = AllocCommand();
    if (command != NULL)
        return command;

    if ((flags & SND_COMMAND_BLOCK) == 0)
        return NULL;

    if (SND_CountWaitingCommand() > 0)
    {
        /* 処理待ちコマンドが存在する */

        /* 完了コマンドリストを受信してみる */
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        /* 空きコマンドができたか？ */
        command = AllocCommand();
        if (command != NULL)
            return command;
    }
    else
    {
        /* 処理待ちコマンドはない */

        /* 現在のコマンドリストを発行 */
        (void)SND_FlushCommand(SND_COMMAND_BLOCK);
    }

    /* 即時処理要求 */
    RequestCommandProc();

    /* コマンド処理完了待ち */
    do
    {
        (void)SND_RecvCommandReply(SND_COMMAND_BLOCK);
        command = AllocCommand();
    } while (command == NULL);

    return command;
}

/*---------------------------------------------------------------------------*
  Name:         SND_PushCommand

  Description:  コマンドリストへコマンドの追加(ThreadSafe)

  Arguments:    追加するコマンド

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_PushCommand(struct SNDCommand *command)
{
    OSIntrMode bak_psr;

    SDK_NULL_ASSERT(command);

    bak_psr = OS_DisableInterrupts();

    // add to end of sReserveList

    if (sReserveListEnd == NULL)
    {
        sReserveList = command;
        sReserveListEnd = command;
    }
    else
    {
        sReserveListEnd->next = command;
        sReserveListEnd = command;
    }

    command->next = NULL;

    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         SND_FlushCommand

  Description:  コマンドリストを送信(ThreadSafe)

  Arguments:    flags - BLOCK or NOBLOCK

  Returns:      成功したかどうか
 *---------------------------------------------------------------------------*/
BOOL SND_FlushCommand(u32 flags)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (sReserveList == NULL)
    {
        /* 予約済みコマンドが無いので何もしない */
        (void)OS_RestoreInterrupts(bak_psr);
        return TRUE;
    }

    if (sWaitingCommandListCount >= SND_PXI_FIFO_MESSAGE_BUFSIZE)
    {
        /* ARM7でのコマンド処理が滞っている */
        if ((flags & SND_COMMAND_BLOCK) == 0)
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return FALSE;
        }

        do
        {
            (void)SND_RecvCommandReply(SND_COMMAND_BLOCK);
        } while (sWaitingCommandListCount >= SND_PXI_FIFO_MESSAGE_BUFSIZE);

        /* SND_RecvCommandReplyで、割り込み処理発生のため、再確認 */
        if (sReserveList == NULL)
        {
            /* 予約済みコマンドが無いので何もしない */
            (void)OS_RestoreInterrupts(bak_psr);
            return TRUE;
        }
    }

    /* コマンドリスト発行 */
    DC_FlushRange(sCommandArray, sizeof(sCommandArray));
    if (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, (u32)sReserveList, FALSE) < 0)
    {
        if ((flags & SND_COMMAND_BLOCK) == 0)
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return FALSE;
        }

        while (sWaitingCommandListCount >= SND_PXI_FIFO_MESSAGE_BUFSIZE ||
               PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, (u32)sReserveList, FALSE) < 0)
        {
            /* 成功するまで待つ */
            (void)OS_RestoreInterrupts(bak_psr);
            (void)SND_RecvCommandReply(SND_COMMAND_NOBLOCK);
            bak_psr = OS_DisableInterrupts();

            DC_FlushRange(sCommandArray, sizeof(sCommandArray));
            /* 割り込み処理発生のため、再確認 */
            if (sReserveList == NULL)
            {
                /* 予約済みコマンドが無いので何もしない */
                (void)OS_RestoreInterrupts(bak_psr);
                return TRUE;
            }
        }
    }

    /* 待ちコマンドキューへＰＵＳＨ */
    sWaitingCommandListQueue[sWaitingCommandListQueueWrite] = sReserveList;
    sWaitingCommandListQueueWrite++;
    if (sWaitingCommandListQueueWrite > SND_PXI_FIFO_MESSAGE_BUFSIZE)
        sWaitingCommandListQueueWrite = 0;

    /* 変数の更新 */
    sReserveList = NULL;
    sReserveListEnd = NULL;

    sWaitingCommandListCount++;
    sCurrentTag++;

    (void)OS_RestoreInterrupts(bak_psr);

    if (flags & SND_COMMAND_IMMEDIATE)
    {
        /* 即時コマンド処理を要求 */
        RequestCommandProc();
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SND_WaitForCommandProc

  Description:  コマンド処理完了同期(ThreadSafe)

  Arguments:    tag - コマンドタグ

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_WaitForCommandProc(u32 tag)
{
    if (SND_IsFinishedCommandTag(tag))
    {
        /* 既に完了済み */
        return;
    }

    /* 完了コマンドリストを受信してみる */
    while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
    {
    }
    if (SND_IsFinishedCommandTag(tag))
    {
        /* 完了していた */
        return;
    }

    /* 即時コマンド処理を要求 */
    RequestCommandProc();

    /* 完了待ち */
    while (!SND_IsFinishedCommandTag(tag))
    {
        (void)SND_RecvCommandReply(SND_COMMAND_BLOCK);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SND_WaitForFreeCommand

  Description:  空きコマンド待ち(ThreadSafe)

  Arguments:    count - 必要な空きコマンドの数

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_WaitForFreeCommand(int count)
{
    int     freeCount = SND_CountFreeCommand();

    SDK_MAX_ASSERT(count, SND_COMMAND_NUM);

    if (freeCount >= count)
        return;

    if (freeCount + SND_CountWaitingCommand() >= count)
    {
        /* 処理待ちコマンドの完了を待てばＯＫ */

        /* 完了コマンドリストを受信してみる */
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
        {
        }

        /* 空きコマンドができたか？ */
        if (SND_CountFreeCommand() >= count)
            return;
    }
    else
    {
        /* 予約済みコマンドを処理する必要がある */

        /* 現在のコマンドリストを発行 */
        (void)SND_FlushCommand(SND_COMMAND_BLOCK);
    }

    /* 即時処理要求 */
    RequestCommandProc();

    /* コマンド処理完了待ち */
    do
    {
        (void)SND_RecvCommandReply(SND_COMMAND_BLOCK);
    } while (SND_CountFreeCommand() < count);
}

/*---------------------------------------------------------------------------*
  Name:         SND_GetCurrentCommandTag

  Description:  コマンドタグの取得(ThreadSafe)

  Arguments:    None.

  Returns:      コマンドタグ
 *---------------------------------------------------------------------------*/
u32 SND_GetCurrentCommandTag(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    u32     tag;

    if (sReserveList == NULL)
    {
        tag = sFinishedTag;
    }
    else
    {
        tag = sCurrentTag;
    }

    (void)OS_RestoreInterrupts(bak_psr);
    return tag;
}

/*---------------------------------------------------------------------------*
  Name:         SND_IsFinishedCommandTag

  Description:  コマンドタグが処理済みかどうかのチェック(ThreadSafe)

  Arguments:    tag - コマンドタグ

  Returns:      処理済みかどうか
 *---------------------------------------------------------------------------*/
BOOL SND_IsFinishedCommandTag(u32 tag)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    BOOL    result;

    if (tag > sFinishedTag)
    {
        if (tag - sFinishedTag < 0x80000000)
        {
            result = FALSE;
        }
        else
        {
            result = TRUE;
        }
    }
    else
    {
        if (sFinishedTag - tag < 0x80000000)
        {
            result = TRUE;
        }
        else
        {
            result = FALSE;
        }
    }

    (void)OS_RestoreInterrupts(bak_psr);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SND_CountFreeCommand

  Description:  空きコマンド数の取得(ThreadSafe)

  Arguments:    None.

  Returns:      空きコマンド数
 *---------------------------------------------------------------------------*/
int SND_CountFreeCommand(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    SNDCommand *command;
    int     count = 0;

    command = sFreeList;
    while (command != NULL)
    {
        ++count;
        command = command->next;
    }

    (void)OS_RestoreInterrupts(bak_psr);
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         SND_CountReservedCommand

  Description:  予約済みコマンド数の取得(ThreadSafe)

  Arguments:    None.

  Returns:      予約済みコマンド数
 *---------------------------------------------------------------------------*/
int SND_CountReservedCommand(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    SNDCommand *command;
    int     count = 0;

    command = sReserveList;
    while (command != NULL)
    {
        ++count;
        command = command->next;
    }

    (void)OS_RestoreInterrupts(bak_psr);
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         SND_CountWaitingCommand

  Description:  処理待ちコマンド数の取得(ThreadSafe)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
int SND_CountWaitingCommand(void)
{
    int     freeCount = SND_CountFreeCommand();
    int     reservedCount = SND_CountReservedCommand();

    return SND_COMMAND_NUM - freeCount - reservedCount;
}

#else  /* SDK_ARM7 */

/*---------------------------------------------------------------------------*
  Name:         SND_CommandProc

  Description:  コマンドリストの処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_CommandProc(void)
{
    OSMessage message;
    const SNDCommand *command_p;
    SNDCommand command;

    while (OS_ReceiveMessage(&sCommandMesgQueue, &message, OS_MESSAGE_NOBLOCK))
    {
        command_p = (const SNDCommand *)message;

        while (command_p != NULL)
        {
            // Note: MainMemory Access
            command = *command_p;

            switch (command.id)
            {
            case SND_COMMAND_START_SEQ:
                SND_StartSeq((int)command.arg[0],
                             (const void *)command.arg[1],
                             command.arg[2], (struct SNDBankData *)command.arg[3]);
                break;

            case SND_COMMAND_STOP_SEQ:
                SND_StopSeq((int)command.arg[0]);
                break;

            case SND_COMMAND_PREPARE_SEQ:
                SND_PrepareSeq((int)command.arg[0],
                               (const void *)command.arg[1],
                               command.arg[2], (struct SNDBankData *)command.arg[3]);
                break;

            case SND_COMMAND_START_PREPARED_SEQ:
                SND_StartPreparedSeq((int)command.arg[0]);
                break;

            case SND_COMMAND_PAUSE_SEQ:
                SND_PauseSeq((int)command.arg[0], (BOOL)command.arg[1]);
                break;

            case SND_COMMAND_SKIP_SEQ:
                SND_SkipSeq((int)command.arg[0], (u32)command.arg[1]);
                break;

            case SND_COMMAND_PLAYER_PARAM:
                SNDi_SetPlayerParam((int)command.arg[0],
                                    command.arg[1], command.arg[2], (int)command.arg[3]);
                break;

            case SND_COMMAND_TRACK_PARAM:
                SNDi_SetTrackParam((int)UNPACK_COMMAND(command.arg[0], 0, 24),
                                   command.arg[1],
                                   command.arg[2],
                                   command.arg[3], (int)UNPACK_COMMAND(command.arg[0], 24, 8));
                break;

            case SND_COMMAND_MUTE_TRACK:
                SND_SetTrackMute((int)command.arg[0], command.arg[1], (SNDSeqMute) command.arg[2]);
                break;

            case SND_COMMAND_ALLOCATABLE_CHANNEL:
                SND_SetTrackAllocatableChannel((int)command.arg[0], command.arg[1], command.arg[2]);
                break;

            case SND_COMMAND_PLAYER_LOCAL_VAR:
                SND_SetPlayerLocalVariable((int)command.arg[0],
                                           (int)command.arg[1], (s16)command.arg[2]);
                break;

            case SND_COMMAND_PLAYER_GLOBAL_VAR:
                SND_SetPlayerGlobalVariable((int)command.arg[0], (s16)command.arg[1]);
                break;

            case SND_COMMAND_START_TIMER:
                StartTimer(command.arg[0], command.arg[1], command.arg[2], command.arg[3]);
                break;

            case SND_COMMAND_STOP_TIMER:
                StopTimer(command.arg[0], command.arg[1], command.arg[2], command.arg[3]);
                break;

            case SND_COMMAND_SETUP_CAPTURE:
                SND_SetupCapture((SNDCapture)UNPACK_COMMAND(command.arg[2], 31, 1),
                                 (SNDCaptureFormat)UNPACK_COMMAND(command.arg[2], 30, 1),
                                 (void *)command.arg[0],
                                 (int)command.arg[1],
                                 (BOOL)UNPACK_COMMAND(command.arg[2], 29, 1),
                                 (SNDCaptureIn)UNPACK_COMMAND(command.arg[2], 28, 1),
                                 (SNDCaptureOut)UNPACK_COMMAND(command.arg[2], 27, 1));
                break;

            case SND_COMMAND_SETUP_ALARM:
                SND_SetupAlarm((int)command.arg[0],
                               (OSTick)command.arg[1], (OSTick)command.arg[2], (int)command.arg[3]);
                break;

            case SND_COMMAND_CHANNEL_TIMER:
                SetChannelTimer((u32)command.arg[0], (int)command.arg[1]);
                break;

            case SND_COMMAND_CHANNEL_VOLUME:
                SetChannelVolume((u32)command.arg[0],
                                 (int)command.arg[1], (SNDChannelDataShift)command.arg[2]);
                break;

            case SND_COMMAND_CHANNEL_PAN:
                SetChannelPan((u32)command.arg[0], (int)command.arg[1]);
                break;

            case SND_COMMAND_SETUP_CHANNEL_PCM:
                SND_SetupChannelPcm((int)UNPACK_COMMAND(command.arg[0], 0, 16), /* chNo */
                                    (const void *)UNPACK_COMMAND(command.arg[1], 0, 27),        /* dataaddr */
                                    (SNDWaveFormat)UNPACK_COMMAND(command.arg[3], 24, 2),       /* format */
                                    (SNDChannelLoop)UNPACK_COMMAND(command.arg[3], 26, 2),      /* loop */
                                    (int)UNPACK_COMMAND(command.arg[3], 0, 16), /* loopStart */
                                    (int)UNPACK_COMMAND(command.arg[2], 0, 22), /* loopLen */
                                    (int)UNPACK_COMMAND(command.arg[2], 24, 7), /* volume */
                                    (SNDChannelDataShift)UNPACK_COMMAND(command.arg[2], 22, 2), /* shift */
                                    (int)UNPACK_COMMAND(command.arg[0], 16, 16),        /* timer */
                                    (int)UNPACK_COMMAND(command.arg[3], 16, 7)  /* pan */
                    );
                break;

            case SND_COMMAND_SETUP_CHANNEL_PSG:
                SND_SetupChannelPsg((int)command.arg[0],        /* chNo */
                                    (SNDDuty)command.arg[3],    /* duty */
                                    (int)UNPACK_COMMAND(command.arg[1], 0, 7),  /* volume */
                                    (SNDChannelDataShift)UNPACK_COMMAND(command.arg[1], 8, 2),  /* shift */
                                    (int)UNPACK_COMMAND(command.arg[2], 8, 16), /* timer */
                                    (int)UNPACK_COMMAND(command.arg[2], 0, 7)   /* pan */
                    );
                break;

            case SND_COMMAND_SETUP_CHANNEL_NOISE:
                SND_SetupChannelNoise((int)command.arg[0],      /* chNo */
                                      (int)UNPACK_COMMAND(command.arg[1], 0, 7),        /* volume */
                                      (SNDChannelDataShift)UNPACK_COMMAND(command.arg[1], 8, 2),        /* shift */
                                      (int)UNPACK_COMMAND(command.arg[2], 8, 16),       /* timer */
                                      (int)UNPACK_COMMAND(command.arg[2], 0, 7) /* pan */
                    );
                break;

            case SND_COMMAND_SURROUND_DECAY:
                SNDi_SetSurroundDecay((int)command.arg[0]);
                break;

            case SND_COMMAND_MASTER_VOLUME:
                SND_SetMasterVolume((int)command.arg[0]);
                break;

            case SND_COMMAND_MASTER_PAN:
                SND_SetMasterPan((int)command.arg[0]);
                break;

            case SND_COMMAND_OUTPUT_SELECTOR:
                SND_SetOutputSelector((SNDOutput)command.arg[0],
                                      (SNDOutput)command.arg[1],
                                      (SNDChannelOut)command.arg[2], (SNDChannelOut)command.arg[3]);
                break;

            case SND_COMMAND_LOCK_CHANNEL:
                SND_LockChannel(command.arg[0], command.arg[1]);
                break;

            case SND_COMMAND_UNLOCK_CHANNEL:
                SND_UnlockChannel(command.arg[0], command.arg[1]);
                break;

            case SND_COMMAND_STOP_UNLOCKED_CHANNEL:
                SND_StopUnlockedChannel(command.arg[0], command.arg[1]);
                break;

            case SND_COMMAND_INVALIDATE_SEQ:
                SND_InvalidateSeq((const void *)command.arg[0], (const void *)command.arg[1]);
                break;

            case SND_COMMAND_INVALIDATE_BANK:
                SND_InvalidateBank((const void *)command.arg[0], (const void *)command.arg[1]);
                break;

            case SND_COMMAND_INVALIDATE_WAVE:
                SND_InvalidateWave((const void *)command.arg[0], (const void *)command.arg[1]);
                break;

            case SND_COMMAND_SHARED_WORK:
                SNDi_SharedWork = (SNDSharedWork *)command.arg[0];
                break;

            case SND_COMMAND_READ_DRIVER_INFO:
                ReadDriverInfo((SNDDriverInfo *) command.arg[0]);
                break;
            }

            command_p = command.next;

        }                              // end of while ( command_p != NULL )

        /* コマンドリスト処理完了 */
        SDK_NULL_ASSERT(SNDi_SharedWork);
        SNDi_SharedWork->finishCommandTag++;
    }
}

#endif /* SDK_ARM9 */

/******************************************************************************
    static function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         PxiFifoCallback

  Description:  PXIコールバック

  Arguments:    tag  - PXIタグ
                data - ユーザーデータ（アドレスまたはメッセージ番号）
                err  - エラーフラグ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PxiFifoCallback(PXIFifoTag tag, u32 data, BOOL /*err */ )
{
    OSIntrMode enabled;
    BOOL    result;

#ifdef SDK_DEBUG
    SDK_ASSERT(tag == PXI_FIFO_TAG_SOUND);
#else
#pragma unused( tag )
#endif

    enabled = OS_DisableInterrupts();

#ifdef SDK_ARM9

#pragma unused( result )
    SNDi_CallAlarmHandler((int)data);

#else  /* SDK_ARM7 */

    if (data >= HW_MAIN_MEM)
    {
        /* 送信側で抑制しているので、失敗しないはず */
        result = OS_SendMessage(&sCommandMesgQueue, (OSMessage)data, OS_MESSAGE_NOBLOCK);
        SDK_ASSERTMSG(result, "Failed OS_SendMessage");
    }
    else
    {
        switch (data)
        {
        case SND_MSG_REQUEST_COMMAND_PROC:
            {
                (void)SND_SendWakeupMessage();
                break;
            }
        }
    }

#endif /* SDK_ARM9 */

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         InitPXI

  Description:  PXIの初期化

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitPXI(void)
{
    // コールバックを登録し, 相手と初期同期をはかる.
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_SOUND, PxiFifoCallback);

#ifdef SDK_ARM9
    if (IsCommandAvailable())
    {
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_SOUND, PXI_PROC_ARM7))
        {
            OS_SpinWait(100);
        }
    }
#endif
}

#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         RequestCommandProc

  Description:  コマンド処理の要求(ThreadSafe)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void RequestCommandProc(void)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_SOUND, SND_MSG_REQUEST_COMMAND_PROC, FALSE) < 0)
    {
        // retry
    }
}

/*---------------------------------------------------------------------------*
  Name:         AllocCommand

  Description:  コマンドの確保(ThreadSafe)

  Arguments:    None.

  Returns:      コマンドまたは、NULL
 *---------------------------------------------------------------------------*/
static SNDCommand *AllocCommand(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    SNDCommand *command;

    if (sFreeList == NULL)
    {
        (void)OS_RestoreInterrupts(bak_psr);
        return NULL;
    }

    command = sFreeList;
    sFreeList = sFreeList->next;

    if (sFreeList == NULL)
        sFreeListEnd = NULL;

    (void)OS_RestoreInterrupts(bak_psr);
    return command;
}

/*---------------------------------------------------------------------------*
  Name:         IsCommandAvailable

  Description:  コマンドルーチンが有効かどうかを調べる

  Arguments:    None.

  Returns:      コマンドルーチンが有効かどうか
 *---------------------------------------------------------------------------*/
static BOOL IsCommandAvailable(void)
{
    OSIntrMode prev_mode;
    u32     res;

    if (!OS_IsRunOnEmulator())
        return TRUE;

    prev_mode = OS_DisableInterrupts();

    /* サウンド有効確認 */
    *(vu32 *)0x4fff200 = 0x10;
    res = *(vu32 *)0x4fff200;

    (void)OS_RestoreInterrupts(prev_mode);

    return res ? TRUE : FALSE;
}


#else  /* SDK_ARM7 */


static void SetChannelTimer(u32 chBitMask, int timer)
{
    int     ch;

    for (ch = 0; ch < SND_CHANNEL_NUM && chBitMask != 0; ch++, chBitMask >>= 1)
    {
        if ((chBitMask & 0x01) == 0)
            continue;
        SND_SetChannelTimer(ch, timer);
    }
}

static void SetChannelVolume(u32 chBitMask, int volume, SNDChannelDataShift shift)
{
    int     ch;

    for (ch = 0; ch < SND_CHANNEL_NUM && chBitMask != 0; ch++, chBitMask >>= 1)
    {
        if ((chBitMask & 0x01) == 0)
            continue;
        SND_SetChannelVolume(ch, volume, shift);
    }
}

static void SetChannelPan(u32 chBitMask, int pan)
{
    int     ch;

    for (ch = 0; ch < SND_CHANNEL_NUM && chBitMask != 0; ch++, chBitMask >>= 1)
    {
        if ((chBitMask & 0x01) == 0)
            continue;
        SND_SetChannelPan(ch, pan);
    }
}


static void StartTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 /*flags */ )
{
    OSIntrMode enabled;
    int     i;

    enabled = OS_DisableInterrupts();

    for (i = 0; i < SND_CHANNEL_NUM && chBitMask != 0; i++, chBitMask >>= 1)
    {
        if ((chBitMask & 0x01) == 0)
            continue;
        SND_StartChannel(i);
    }

    if (capBitMask & (1 << SND_CAPTURE_0))
    {
        if (capBitMask & (1 << SND_CAPTURE_1))
        {
            SND_StartCaptureBoth();
        }
        else
        {
            SND_StartCapture(SND_CAPTURE_0);
        }
    }
    else if (capBitMask & (1 << SND_CAPTURE_1))
    {
        SND_StartCapture(SND_CAPTURE_1);
    }

    for (i = 0; i < SND_ALARM_NUM && alarmBitMask != 0; i++, alarmBitMask >>= 1)
    {
        if ((alarmBitMask & 0x01) == 0)
            continue;
        SND_StartAlarm(i);
    }

    (void)OS_RestoreInterrupts(enabled);

    SND_UpdateSharedWork();
}

static void StopTimer(u32 chBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags)
{
    OSIntrMode enabled;
    int     i;

    enabled = OS_DisableInterrupts();

    for (i = 0; i < SND_ALARM_NUM && alarmBitMask != 0; i++, alarmBitMask >>= 1)
    {
        if ((alarmBitMask & 0x01) == 0)
            continue;
        SND_StopAlarm(i);
    }

    for (i = 0; i < SND_CHANNEL_NUM && chBitMask != 0; i++, chBitMask >>= 1)
    {
        if ((chBitMask & 0x01) == 0)
            continue;
        SND_StopChannel(i, (s32)flags);
    }

    if (capBitMask & (1 << SND_CAPTURE_0))
    {
        SND_StopCapture(SND_CAPTURE_0);
    }
    if (capBitMask & (1 << SND_CAPTURE_1))
    {
        SND_StopCapture(SND_CAPTURE_1);
    }

    (void)OS_RestoreInterrupts(enabled);

    SND_UpdateSharedWork();
}

static void ReadDriverInfo(SNDDriverInfo * info)
{
    int     ch;

    MI_CpuCopy32(&SNDi_Work, &info->work, sizeof(SNDi_Work));

    info->workAddress = &SNDi_Work;

    for (ch = 0; ch < SND_CHANNEL_NUM; ch++)
    {
        info->chCtrl[ch] = SND_GetChannelControl(ch);
    }

    info->lockedChannels = SND_GetLockedChannel(0);
}

#endif /* SDK_ARM9 */

/*====== End of snd_command.c ======*/
