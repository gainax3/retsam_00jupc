/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_listen_accept.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_listen_accept.c,v $
  Revision 1.7  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.6  2006/01/13 07:26:50  yasu
  SOCL 関数のパラメータ調整

  Revision 1.5  2005/08/24 09:25:13  yasu
  SOCL_SocketIsInvalid 追加

  Revision 1.4  2005/08/01 13:23:00  yasu
  SOCL_Listen/Accept の追加

  Revision 1.3  2005/07/30 15:30:43  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.2  2005/07/27 12:20:04  yasu
  データ出力の際にネットワークバイトオーダーを選択可能にする

  Revision 1.1  2005/07/22 13:55:35  yasu
  listen accept の追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include <nitroWiFi/soc.h>

static int  SOCL_ExecListenAcceptCommand(int s, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr);
static int  SOCLi_ListenAcceptCallBack(void* arg);

/*---------------------------------------------------------------------------*
  Name:         SOCL_Listen

  Description:  TCP のパッシブコネクションを確立する数を指定し、
                待ちうけの準備をする. 現在は何もしない
  
  Arguments:    s               ソケット
                backlog         待ち受け可能な接続の数
  
  Returns:      正or 0: 成功 (accept された
 *---------------------------------------------------------------------------*/
int SOCL_Listen(int s, int backlog)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;         // 規定外の値
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;      // 初期化されていない
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;         // 接続処理を開始している
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;         // TCP ソケットでない
    }

    if (!SOCL_SocketIsBlock(socket))
    {
        return SOCL_EWOULDBLOCK;    // BLOCK の実装のみ
    }

    // 現状では何もしない
    // 非同期の Listen/Accept をサポートする場合、処理を追加する必要がある．
    (void)backlog;
    (void)socket;

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_Accept

  Description:  TCP のパッシブコネクションを確立する
                CPS の仕様にあわせて Listen と Accept が同時に行なわれる
                ソケットを新しく作成する
  
  Arguments:    s               ソケット
                remote_port     リモートのポート番号へのポインタ
                remote_ip       リモートの IP アドレスへのポインタ
  
  Returns:      正or 0: 成功 (accept された
                負    : エラー
                現状サポートしているエラー値は以下
  
                エラー値は追加される場合があるため、他の負の値も全て汎用
                エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOCL_Accept(int s_listen, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr)
{
    SOCLSocket*     socket_listen = (SOCLSocket*)s_listen;
    int s_accept;
    s32 result;

    if (SOCL_SocketIsInvalid(socket_listen))
    {
        return SOCL_EINVAL;         // 規定外の値
    }

    if (!SOCL_SocketIsCreated(socket_listen))
    {
        return SOCL_ENETRESET;      // 初期化されていない
    }

    if (SOCL_SocketIsConnecting(socket_listen))
    {
        return SOCL_EINVAL;         // 接続処理を開始している
    }

    if (!SOCL_SocketIsTCP(socket_listen))
    {
        return SOCL_EINVAL;         // TCP ソケットでない
    }

    if (!SOCL_SocketIsBlock(socket_listen))
    {
        return SOCL_EWOULDBLOCK;    // BLOCK の実装のみ
    }

    s_accept = SOCL_TcpSocket();

    if (s_accept < 0)
    {
        return s_accept;

        // SOCL_ENOMEM    : メモリー不足
    }

    result = SOCL_Bind(s_accept, socket_listen->local_port);

    if (result < 0)
    {
        return result;

        // SOCL_Bind の返り値
        //   SOCL_EINVAL;      規定外の値             -> ここではありえない
        //   SOCL_ENETRESET;   初期化されていない     -> ここではありえない
        //   SOCL_EALREADY;    接続処理を開始している -> ここではありえない
        //   SOCL_EMFILE;      ソケットコマンドパケットを確保できなかった
        //   SOCL_ETIMEDOUT;   接続タイムアウト
    }

    result = SOCL_ListenAccept(s_accept, remote_port_ptr, remote_ip_ptr);

    if (result < 0)
    {
        return result;

        // SOCL_ListenAccept の返り値
        //   SOCL_EINVAL;       規定外の値             -> ここではありえない
        //   SOCL_ENETRESET;    初期化されていない     -> ここではありえない
        //   SOCL_EINVAL;       接続処理を開始している -> ここではありえない
        //   SOCL_EINVAL;       TCP ソケットでない     -> ここではありえない
        //   SOCL_EWOULDBLOCK;  BLOCK モードでない     -> ここではありえない
        //   SOCL_EINVAL;       Listen 用のポートが設定されていない -> ここではありえない
    }

    return s_accept;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_ListenAccept

  Description:  TCP のパッシブコネクションを確立する
                CPS の仕様にあわせて Listen と Accept が同時に行なわれる
                引数で指定されたソケットを accept 用として使う
  
  Arguments:    s               ソケット
                remote_port     リモートのポート番号へのポインタ
                remote_ip       リモートの IP アドレスへのポインタ
                net_byte_order  バイトオーダー TRUE なら NetwrokByteOrder
  
  Returns:      正or 0: 成功
                負    : エラー
                現状サポートしているエラー値は以下
  
                エラー値は追加される場合があるため、他の負の値も全て汎用
                エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOCL_ListenAccept(int s, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr)
{
    SOCLSocket*         socket = (SOCLSocket*)s;
    SOCLiCommandPacket*     command;
    s32 result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;         // 規定外の値
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;      // 初期化されていない
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;         // 接続処理を開始している
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;         // TCP ソケットでない
    }

    if (!SOCL_SocketIsBlock(socket))
    {
        return SOCL_EWOULDBLOCK;    // BLOCK モードでない
    }

    if (socket->local_port == 0)
    {
        return SOCL_EINVAL;         // Listen 用のポートが設定されていない
    }

    // 受信スレッドにコマンドを投げ、コールバック関数内部で CPS_Listen
    // および CPS_SocWho を起動する．
    // 現在、ソケットタイプ NOBLOCK はサポートしていない
    command = SOCLi_CreateCommandPacket(SOCLi_ListenAcceptCallBack, socket, socket->flag_block);

    SDK_ASSERT(command);

    // listen_accept 用のパラメータ設定
    command->listen_accept.local_port = socket->local_port;
    command->listen_accept.remote_port_ptr = remote_port_ptr;
    command->listen_accept.remote_ip_ptr = remote_ip_ptr;
    socket->state |= SOCL_STATUS_CONNECTING;

    // コマンド実行
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ListenAcceptCallBack

  Description:  Listen & Accept 処理のコールバック
  
  Arguments:    arg  コマンドブロックへのポインタ
  
  Returns:      SOCLi_ExecCommand* に渡される値(BLOCKモードのとき)
 *---------------------------------------------------------------------------*/
static int SOCLi_ListenAcceptCallBack(void* arg)
{
    SOCLiCommandListenAccept*   cpacket = (SOCLiCommandListenAccept*)arg;
    SOCLSocket*     socket = (SOCLSocket*)cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    u16         remote_port;
    CPSInAddr   remote_ip;
    CPSInAddr   local_ip;

    SDK_ASSERT(socket);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);

    OS_LockMutex(&recv_pipe->h.in_use); // BLOCKED
    {
        // ローカルポート待ち受け設定  リモートは any 設定
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocBind(%d,0,0)\n", cpacket->local_port);
#endif
        CPS_SocBind(cpacket->local_port, 0, 0);

        // 他のソケットから接続処理が入るまで待つ
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_TcpListen()\n");
#endif
        CPS_TcpListen();

        recv_pipe->consumed = 0;

        // 接続先の取得
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocWho()\n");
#endif
        remote_ip = CPS_SocWho(&remote_port, &local_ip);

        *cpacket->remote_port_ptr = remote_port;
        *cpacket->remote_ip_ptr = remote_ip;

        socket->state |= SOCL_STATUS_CONNECTED;
    }

    OS_UnlockMutex(&recv_pipe->h.in_use);

    return SOCL_ESUCCESS;               // 成功
}
