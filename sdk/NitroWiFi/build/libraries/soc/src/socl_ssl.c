/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_ssl.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_ssl.c,v $
  Revision 1.4  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.3  2005/08/24 09:25:13  yasu
  SOCL_SocketIsInvalid 追加

  Revision 1.2  2005/08/10 09:32:00  seiki_masashi
  SSL のサポートを追加

  Revision 1.1  2005/08/06 11:55:46  seiki_masashi
  SOCL_EnableSsl を追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_EnableSslCallBack(void* arg);
static int  SOCLi_ExecEnableSslCommand(SOCLSocket* socket, SOCLSslConnection* con);

/*---------------------------------------------------------------------------*
  Name:         SOCL_EnableSsl

  Description:  ソケットを SSL モードにする。
                bind 前に呼び出さねばならない。
  
  Arguments:    s    ソケット
                con  SOCLSslConnection 構造体へのポインタ
                     NULL で SSL モードから通常に戻す
  
  Returns:      正 or 0: 成功
                負     : エラー
                  現状サポートしているエラー値は以下
                    - :  
                  エラー値は追加される場合があるため、他の負の値も全て汎用
                  エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOCL_EnableSsl(int s, SOCLSslConnection* con)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // 規定外の値
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;     // SSL は TCP でしか使えない
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // 初期化されていない
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;     // 接続処理を開始している
    }

    return SOCLi_ExecEnableSslCommand(socket, con);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecEnableSslCommand

  Description:  ソケットを SSL モードにする
  
  Arguments:    s    ソケット
                con  SOCLSslConnection 構造体へのポインタ
                     NULL で SSL モードから通常に戻す
  
  Returns:      正or 0: 成功
                負    : エラー
                現状サポートしているエラー値は以下
                    - :  
                エラー値は追加される場合があるため、他の負の値も全て汎用
                エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecEnableSslCommand(SOCLSocket* socket, SOCLSslConnection* con)
{
    SOCLiCommandPacket*     command;
    s32 result;

    // recv_pipe にコマンドを投げ、コールバック関数内部で CPS_SetSsl を
    // 実行する．コマンドを確保できない場合にエラーとなる
    command = SOCLi_CreateCommandPacket(SOCLi_EnableSslCallBack, socket, OS_MESSAGE_BLOCK);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

    // enable ssl 用のパラメータ設定
    command->enable_ssl.connection = con;

    // コマンド実行
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_EnableSslCallBack

  Description:  EnableSsl のコールバック
  
  Arguments:    arg  コマンドブロックへのポインタ
  
  Returns:      SOCLi_ExecCommand* に渡される値(BLOCKモードのとき)
 *---------------------------------------------------------------------------*/
static int SOCLi_EnableSslCallBack(void* arg)
{
    SOCLiCommandEnableSsl*          cpacket = (SOCLiCommandEnableSsl*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    CPSSoc*     cpsoc;

    SDK_ASSERT(socket);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);
    cpsoc = (CPSSoc*)OSi_GetSpecificData(&recv_pipe->h.thread, OSi_SPECIFIC_CPS);
    SDK_ASSERT(cpsoc);

    OS_LockMutex(&recv_pipe->h.in_use); // BLOCKED
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SetSsl(%d)\n", (cpacket->connection != NULL) ? 1 : 0);
#endif

        if (cpacket->connection != NULL)
        {
            cpsoc->con = cpacket->connection;
            CPS_SetSsl(1);
            socket->flag_mode = SOCL_FLAGMODE_SSL;
        }
        else
        {
            socket->flag_mode = SOCL_FLAGMODE_TCP;
            CPS_SetSsl(0);
            cpsoc->con = NULL;
        }
    }

    OS_UnlockMutex(&recv_pipe->h.in_use);

    return SOCL_ESUCCESS;               // 成功
}
