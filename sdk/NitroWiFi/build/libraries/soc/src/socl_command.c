/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_command.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_command.c,v $
  Revision 1.10  2006/05/17 08:37:11  yasu
  SOCLi_CommandPacketHandler 内でにおけるスレッドの再スケジュール処理を削除。
  スレッド切り替えは OS_ReadMessage 内で一体化。

  Revision 1.9  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.8  2005/12/06 11:53:13  yasu
  TCP BLOCK Write における不具合の修正
  BLOCK モード判定用のマクロの導入

  Revision 1.7  2005/10/14 06:51:35  yasu
  コマンドパケット作成時に response メンバに初期値を設定していなかったのを修正

  Revision 1.6  2005/10/14 01:56:53  yasu
  コマンドキューが一杯だったときに Close 処理が不完全な状態になることを修正

  Revision 1.5  2005/09/27 14:18:09  yasu
  SOC_Close の非同期動作サポート

  Revision 1.4  2005/08/11 07:02:00  yasu
  前回の修正を取り消し

  Revision 1.3  2005/08/11 05:08:21  yasu
  返値が不定となる関数の修正

  Revision 1.2  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.1  2005/07/30 15:29:33  yasu
  コマンドパイプに関する変数を構造体に分離

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>

//---------------------------------------------------------------------------*
// コマンドパケットキュー
//---------------------------------------------------------------------------*
static OSMessage*       SOCLiCommandPackets;
static OSMessageQueue   SOCLiCommandPacketQueue;

/*---------------------------------------------------------------------------*
  Name:         SOCLi_StartupCommandPacketQueue

  Description:  コマンドパケットキュー初期化

  Arguments:    cp_max_count パケットの数
  
  Returns:       0 成功
                -1 メモリ確保失敗
 *---------------------------------------------------------------------------*/
int SOCLi_StartupCommandPacketQueue(s32 cp_max_count)
{
    SOCLiCommandPacket*     cp_array;
    u32         cp_array_size;
    OSMessage*  cp_message_array;
    u32         cp_message_size;

    SDK_ASSERT(cp_max_count > 0);

    cp_message_size = MATH_ROUNDUP(cp_max_count * sizeof(OSMessage), 4);
    cp_array_size = MATH_ROUNDUP(cp_max_count * sizeof(SOCLiCommandPacket), 4);
    cp_message_array = (OSMessage*)SOCLi_Alloc(cp_array_size + cp_message_size);

    if (NULL == cp_message_array)
    {
        return -1;
    }

    // MessageQueue 作成および
    // Message としてコマンドパケットのアドレスを Queue に登録
    OS_InitMessageQueue(&SOCLiCommandPacketQueue, cp_message_array, cp_max_count);

    cp_array = (SOCLiCommandPacket *) ((u32) cp_message_array + cp_message_size);

    while (cp_max_count > 0)
    {
        SOCLi_FreeCommandPacket(cp_array);

        cp_array++;
        cp_max_count--;
    }

    SOCLiCommandPackets = cp_message_array;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CleanupCommandPacketQueue

  Description:  コマンドパケットキュー開放

  Arguments:    なし

  Returns:       0 正常終了
                -1 全てのコマンドパケット領域が回収されていない
 *---------------------------------------------------------------------------*/
int SOCLi_CleanupCommandPacketQueue(void)
{
    // キューがフルになっていないなら未回収のパケットがある
    if (!OS_IsMessageQueueFull(&SOCLiCommandPacketQueue))
    {
        return -1;
    }

    SOCLi_Free(SOCLiCommandPackets);
    SOCLiCommandPackets = NULL;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_AllocCommandPacket

  Description:  コマンドパケットの確保

  Arguments:    flag  コマンド動作のブロックモード
                      OS_MESSAGE_NOBLOCK or OS_MESSAGE_BLOCK

  Returns:      コマンドパケットへのポインタ
                ブロックモードが NOBLOCK でコマンドパケット領域がないなら NULL
                ブロックモードが BLOCK   でコマンドパケット領域がないなら
                空き領域ができるまでブロックします．
 *---------------------------------------------------------------------------*/
SOCLiCommandPacket* SOCLi_AllocCommandPacket(s32 flag)
{
    SOCLiCommandPacket*     cp;

    return(OS_ReceiveMessage(&SOCLiCommandPacketQueue, (void*) &cp, flag) ? cp : NULL);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CreateCommandPacket

  Description:  コマンドパケットの作成

  Arguments:    handler   コマンドのハンドラ
                socket    対象ソケット
                flag      コマンド動作のブロックモード
                          OS_MESSAGE_NOBLOCK or OS_MESSAGE_BLOCK
                          ソケットに設定されてるモードがそのまま使われるとは
                          限らない．あくまで今回のコマンドの動作モード．
   
  Returns:      コマンドパケットへのポインタ
                ブロックモードが NOBLOCK でコマンドパケット領域がないなら NULL
                ブロックモードが BLOCK   でコマンドパケット領域がないなら
                空き領域ができるまでブロックします．
 *---------------------------------------------------------------------------*/
SOCLiCommandPacket* SOCLi_CreateCommandPacket(SOCLiCommandHandler handler, SOCLSocket* socket, s32 flag)
{
    SOCLiCommandPacket*     cp;

    cp = SOCLi_AllocCommandPacket(flag);

    if (cp)
    {
        cp->h.handler = handler;
        cp->h.socket = socket;
        cp->h.response = NULL;
        cp->h.flag_mode = socket->flag_mode;
        cp->h.flag_block = (s8) flag;
    }

    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_FreeCommandPacket

  Description:  コマンドパケットの開放

  Arguments:    command   コマンドパケットへのポインタ

  Returns:      なし
 *---------------------------------------------------------------------------*/
void SOCLi_FreeCommandPacket(SOCLiCommandPacket* command)
{
    if (command)
    {
        (void)OS_SendMessage(&SOCLiCommandPacketQueue, command, OS_MESSAGE_NOBLOCK);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetCtrlPipe
  
  Description:  コマンド制御パイプの取得

  Arguments:    socket		ソケット
  
  Returns:      ソケット制御用のコマンドパイプへのポインタ
 *---------------------------------------------------------------------------*/
SOCLiSocketCommandPipe* SOCLi_GetCtrlPipe(SOCLSocket* socket)
{
    // 通常は Recv Pipe 側をマスターとする
    // UDPSend のときは例外で Send Pipe がマスターとなりデータを受ける.
    return(socket->recv_pipe) ? &socket->recv_pipe->h : &socket->send_pipe->h;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SendCommandPacket
  
  Description:  コマンドパケットの送信

  Arguments:    pipe		送信先のコマンドパイプ
                command         コマンドのパケット
  
  Returns:      command->flag_block が NOBLOCK のとき
                     送信成功なら SOCL_ESUCCESS=0
                         失敗なら SOCL_ENOBUFS<0
                         失敗したらコマンドを回収する
                command->flag_block が BLOCK のとき
                     常に 0
 *---------------------------------------------------------------------------*/
int SOCLi_SendCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command)
{
    s32 result;
    s32 flag_block;

    SDK_ASSERT(pipe);

    flag_block = (command && !SOCL_FLAGISBLOCK(command->h.flag_block)) ? OS_MESSAGE_NOBLOCK : OS_MESSAGE_BLOCK;
    result = OS_SendMessage(&pipe->queue, command, flag_block);

    if (!result)
    {
        SOCLi_FreeCommandPacket(command);
    }

    return result ? SOCL_ESUCCESS : SOCL_ENOBUFS;
}

int SOCLi_SendCommandPacketToCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketCommandPipe*     pipe = SOCLi_GetCtrlPipe(socket);

    SDK_ASSERT(pipe);
    return SOCLi_SendCommandPacket(pipe, command);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecCommandPacket
  
  Description:  コマンドパケットの実行(送信＆処理待ち)

  Arguments:    pipe		送信先のコマンドパイプ
                command         コマンドのパケット
  
  Returns:      command->flag_block が NOBLOCK のとき
                     送信成功なら SOCL_ESUCCESS=0
                         失敗なら SOCL_ENOBUFS<0
                         失敗したらコマンドを回収する
                command->flag_block が BLOCK のとき
                     コマンドのリターンコードが返る
 *---------------------------------------------------------------------------*/
int SOCLi_ExecCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command)
{
    s32 result;

    SDK_ASSERT(pipe);
    SDK_ASSERT(command);

    if (command->h.flag_block == OS_MESSAGE_BLOCK)
    {
        OSMessage       response;
        OSMessageQueue  response_queue;

        OS_InitMessageQueue(&response_queue, &response, 1);
        command->h.response = &response_queue;

        (void)SOCLi_SendCommandPacket(pipe, command);
        (void)OS_ReceiveMessage(&response_queue, (void**) &result, OS_MESSAGE_BLOCK);
    }
    else
    {
        // command->h.flag_block == OS_MESSAGE_NOBLOCK/NORESULT
        command->h.response = NULL;
        result = SOCLi_SendCommandPacket(pipe, command);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecCommandPacketInRecvPipe
                SOCLi_ExecCommandPacketInSendPipe
                SOCLi_ExecCommandPacketInCtrlPipe
  
  Description:  コマンドパケットを RecvPipe/SendPipe で実行(送信＆処理待ち)

  Arguments:    socket          ソケット
                command         コマンドのパケット
  
  Returns:      command->flag_block が NOBLOCK のとき
                     送信成功なら SOCL_ESUCCESS=0
                         失敗なら SOCL_ENOBUFS<0
                         失敗したらコマンドを回収する
                command->flag_block が BLOCK のとき
                     常に 0
 *---------------------------------------------------------------------------*/
int SOCLi_ExecCommandPacketInRecvPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(&pipe->h, command);
}

int SOCLi_ExecCommandPacketInSendPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketSendCommandPipe*     pipe = socket->send_pipe;

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(&pipe->h, command);
}

int SOCLi_ExecCommandPacketInCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketCommandPipe*     pipe = SOCLi_GetCtrlPipe(socket);

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(pipe, command);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CommandPacketHandler
  
  Description:  コマンドパケットを処理するためのスレッドのエントリ
                ソケット構造体のメンバーは既に次の SOCL_* 関数によって変更され
                ている可能性があるため、このスレッドからソケット構造体内部の
                値を原則的には参照してはならない．必要な値はコマンドメッセージ
                に積んで渡すこと．ただし、この動作を考慮して作られた処理の場合
                この限りではない．
                またこのスレッドの優先順位は SOCL 関数を呼び出す側よりも高く
                なければならない．
  
  Arguments:    arg             待ち受けキュー
  
  Returns:      なし
 *---------------------------------------------------------------------------*/
void SOCLi_CommandPacketHandler(void* arg)
{
    SOCLiSocketCommandPipe*     cpipe = (SOCLiSocketCommandPipe*)arg;
    SOCLiCommandPacket*         command;
    OSIntrMode  enable;
    s32         result;

    //
    //  コマンドをメッセージとして取得し、指定されたコマンドを実行する
    //  メッセージとして NULL が送られると終了
    //
    while (1)
    {
        // 処理の途中で OS_KillThread で処理スレッドが停止する可能性があるため
        // 完了メッセージを送るまでコマンドをメッセージキューから外さない．
        (void)OS_ReadMessage(&cpipe->queue, (void*) &command, OS_MESSAGE_BLOCK);

        if (command)
        {
            // コールバック呼び出し．
            result = command->h.handler(command);

            // 処理終了にともなうメッセージの分配．
            //   - キューからコマンドを一つ削除
            //   - 結果をソケット構造体へ書き戻す
            //   - 必要なら完了メッセージを投げる
            //   - コマンドパケットキューへコマンドを返却する
            // この関連処理全てをまとめてアトミックにしている．
            // 内部のメッセージ関連処理全てが NOBLOCK でないとハングする可能性
            // があるので注意．
            enable = OS_DisableInterrupts();
            (void)OS_DisableScheduler();
            (void)OS_ReceiveMessage(&cpipe->queue, (void*)NULL, OS_MESSAGE_NOBLOCK);

            if (command->h.socket)
            {
                command->h.socket->result = result;
            }

            if (command->h.response)
            {
                (void)OS_SendMessage(command->h.response, (void*)result, OS_MESSAGE_NOBLOCK);
            }

            SOCLi_FreeCommandPacket(command);   // NOBLOCK
            (void)OS_EnableScheduler();
            (void)OS_RestoreInterrupts(enable);

            // 次回の OS_ReadMessage() 内で上記のメッセージ送信処理によって
            // 起動した他の優先度の高いスレッドに実行権が委譲される
        }
        else
        {
            break;
        }
    }

    return;
}
