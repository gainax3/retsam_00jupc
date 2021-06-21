/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_create.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_create.c,v $
  Revision 1.14  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.13  2006/01/13 07:26:50  yasu
  SOCL 関数のパラメータ調整

  Revision 1.12  2005/09/27 14:18:09  yasu
  SOC_Close の非同期動作サポート

  Revision 1.11  2005/08/25 08:27:38  yasu
  UDP 受信におけるバッファリング処理追加

  Revision 1.10  2005/08/18 13:18:49  yasu
  ソケットリンクリストによる cleanup 処理の追加

  Revision 1.9  2005/08/13 08:02:39  yasu
  consumed_min 制御の追加

  Revision 1.8  2005/08/10 09:30:57  seiki_masashi
  SSL のサポートを追加

  Revision 1.7  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.6  2005/07/30 15:30:08  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.5  2005/07/27 12:20:33  yasu
  バッファサイズ 0 の時はポインタも NULL とする

  Revision 1.4  2005/07/23 14:16:49  yasu
  Debug メッセージを隠匿

  Revision 1.3  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.2  2005/07/19 14:06:33  yasu
  SOCL_Read/Write 追加

  Revision 1.1  2005/07/18 13:16:28  yasu
  送信受信スレッドを作成する

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int          SOCLi_CreateSocketCallBack(void* arg);

static SOCLSocket*  SOCLi_StartupSocket(const SOCLSocketParam* param);
static u32          SOCLi_GetSizeSocket(const SOCLSocketParam* param);
static u32          SOCLi_GetSizeCommandPipe(const SOCLSocketCommandPipeParam* param);
static u8*          SOCLi_InitSocket(SOCLSocket* socket, const SOCLSocketParam* param);
static u8*          SOCLi_InitSocketBuffer(u8* ptr, CPSSocBuf* buf, u32 size);
static u8*          SOCLi_InitCommandPipe(u8*  ptr, SOCLiSocketCommandPipe*  pipe, const SOCLSocketCommandPipeParam*  param);

/*---------------------------------------------------------------------------*
  Name:         SOCL_CreateSocket

  Description:  ソケットを作成する．
                独自のインターフェイスを使用している．
                メモリ利用に関する細かな設定が可能．

  Arguments:    param           詳細な設定パラメータ
                                SDK で用意しているデフォルトは 
                                TCP の場合: &SOCLSocketParamTCP
                                UDP の場合: &SOCLSocketParamUDP*
  
  Returns:      正の値: ソケットへのポインタ
                負    : エラー
                現状サポートしているエラー値は以下
                  SOCL_ENOMEM    : メモリー不足
                エラー値は追加される場合があるため、他の負の値も全て汎用
                エラーとして扱うこと．
 *---------------------------------------------------------------------------*/
int SOCL_CreateSocket(const SOCLSocketParam* param)
{
    SOCLSocket*         socket;
    SOCLiCommandPacket*     command;
    s32 result;

    SDK_ASSERT(param != NULL);

    // ソケットの領域を確保し、ソケット処理を行なうためのスレッドを走らせる
    // SOCLSocket 内の値を他のスレッドが書き換えるのは、ソケット作成と廃棄時
    // のみとする．
    socket = SOCLi_StartupSocket(param);
    if (socket == NULL)
    {
        return SOCL_ENOMEM;
    }

    // ソケット初期化コマンドを送りコールバックを起動させ、
    // 処理完了を待つ．command 領域は完了時には内部で開放されている．
    command = SOCLi_CreateCommandPacket(SOCLi_CreateSocketCallBack, socket, OS_MESSAGE_BLOCK);

    // 作成と破棄の場合は Ctrl Pipe へコマンドを送る
    // 通常は Recv Pipe が Ctrl Pipe に設定されている(UDP Send のときは例外)
    // SOCLi_CreateSocketCallBack は必ず SUCCESS で返る
    result = SOCLi_ExecCommandPacketInCtrlPipe(socket, command);
    SDK_ASSERT(result >= 0);

    return (int)socket;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CreateSocketCallBack

  Description:  Create 処理のコールバック
  
  Arguments:    arg  コマンドブロックへのポインタ
  
  Returns:      SOCLi_ExecCommand* に渡される値
 *---------------------------------------------------------------------------*/
static int SOCLi_CreateSocketCallBack(void* arg)
{
    SOCLiCommandCreateSocket*   cpacket = (SOCLiCommandCreateSocket*)arg;
    SOCLSocket*     socket = cpacket->h.socket;
    SOCLiSocketSendCommandPipe*     send_pipe;
    SOCLiSocketRecvCommandPipe*     recv_pipe;

    SDK_ASSERT(socket);
#ifdef SDK_MY_DEBUG
    OS_TPrintf("CPS_SocRegister\n");
#endif
    CPS_SocRegister(&socket->cps_socket);

    send_pipe = socket->send_pipe;
    recv_pipe = socket->recv_pipe;

    switch (socket->flag_mode)
    {
    case SOCL_FLAGMODE_TCP:
    case SOCL_FLAGMODE_SSL:
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocDup\n");
        OS_TPrintf("CPS_SocUse\n");
#endif
        CPS_SocDup(&send_pipe->h.thread);
        CPS_SocUse();
        break;

    case SOCL_FLAGMODE_UDP:
        CPS_SocUse();
        CPS_SocDatagramMode();
        CPS_SetUdpCallback(SOCLi_UdpRecvCallback);
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocUse\n");
        OS_TPrintf("CPS_SocDatagramMode\n");
        OS_TPrintf("CPS_SetUdpCallback\n");
#endif
        break;

    case SOCL_FLAGMODE_UDPSEND:
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocDatagramMode\n");
#endif
        CPS_SocDatagramMode();
        break;

    case SOCL_FLAGMODE_ICMP:
    default:
        SDK_ASSERT(0);
        break;
    }

    socket->state = SOCL_STATUS_CREATE;

    return SOCL_ESUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_StartupSocket

  Description:  ソケット用の領域を取得する
                以下の領域を連続したメモリマップ上に確保する
  
                領域
                --------------------------------------------------------
                SOCL ソケット領域
                CPS  ソケット用のバッファ
                受信用スレッドのバッファ
                送信用スレッドのバッファ

  Arguments:    param  パラメータ
  
  Returns:      確保したソケット情報領域のポインタ
 *---------------------------------------------------------------------------*/
static SOCLSocket* SOCLi_StartupSocket(const SOCLSocketParam* param)
{
    SOCLSocket*     socket;
    u32         size;
    OSIntrMode  enable;

    // 必要なメモリサイズを求める
    size = SOCLi_GetSizeSocket(param);

    enable = OS_DisableInterrupts();
    socket = SOCLi_Alloc(size);

    if (socket)
    {
        // 一旦 0 クリアする
        MI_CpuClear8(socket, size);

        // メモリを順に割り振る
        (void)SOCLi_InitSocket(socket, param);

        // リンクのリストへの登録
        SOCLi_SocketRegister(socket);
    }

    (void)OS_RestoreInterrupts(enable);

    return socket;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetSizeSocket
  
  Description:  SOCKET に必要なメモリサイズを返す
  
  Arguments:    param  ソケット生成のパラメータ
  
  Returns:      SOCKET に必要な領域のサイズ
 *---------------------------------------------------------------------------*/
static u32 SOCLi_GetSizeSocket(const SOCLSocketParam* param)
{
    u32 size;

    size = SOCLi_ROUNDUP4(sizeof(SOCLSocket));

    // 受信機能に関して必要なメモリ
    if (param->buffer.rcvbuf_size)
    {
        size += SOCLi_ROUNDUP4(sizeof(SOCLiSocketRecvCommandPipe));
        size += SOCLi_RoundUp4(param->buffer.rcvbuf_size);
        size += SOCLi_RoundUp4(param->buffer.linbuf_size);
        size += SOCLi_GetSizeCommandPipe(&param->recv_pipe);
    }

    // 送信機能に関して必要なメモリ
    if (param->buffer.sndbuf_size)
    {
        size += SOCLi_ROUNDUP4(sizeof(SOCLiSocketSendCommandPipe));
        size += SOCLi_RoundUp4(param->buffer.sndbuf_size);
        size += SOCLi_RoundUp4(param->buffer.outbuf_size);
        size += SOCLi_RoundUp4(param->buffer.wrtbuf_size);
        size += SOCLi_GetSizeCommandPipe(&param->send_pipe);
    }

    return size;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetSizeCommandPipe
  
  Description:  Command Pipe に必要なメモリサイズを返す
  
  Arguments:    param  Command Pipe生成のパラメータ
  
  Returns:      Command Pipe に必要な領域のサイズ
 *---------------------------------------------------------------------------*/
static u32 SOCLi_GetSizeCommandPipe(const SOCLSocketCommandPipeParam* param)
{
    u32 size;

    size = SOCLi_RoundUp4(param->queue_max * sizeof(OSMessage));
    size += SOCLi_RoundUp4(param->stack_size);

    return size;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_InitSocket
  
  Description:  SOCKET を初期化する

  Arguments:    socket  対象のソケットの領域(0 クリアされていること)
                param   初期化パラメータ
  
  Returns:      SOCKET の領域の次のポインタ
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitSocket(SOCLSocket* socket, const SOCLSocketParam* param)
{
    u8*         ptr;
    CPSSoc*     cps = &socket->cps_socket;

    socket->flag_mode = param->flag_mode;
    socket->flag_block = param->flag_block;

    ptr = (u8*)socket;
    ptr = ptr + SOCLi_ROUNDUP4(sizeof(SOCLSocket));

    // 受信機能に関する初期化
    if (param->buffer.rcvbuf_size)
    {
        SOCLiSocketRecvCommandPipe*     pipe = (SOCLiSocketRecvCommandPipe*)ptr;

        // Recv Command Pipe
        socket->recv_pipe = pipe;
        pipe->consumed_min = param->buffer.rcvbuf_consume_min;
        ptr += SOCLi_ROUNDUP4(sizeof(SOCLiSocketRecvCommandPipe));
        ptr = SOCLi_InitCommandPipe(ptr, &pipe->h, &param->recv_pipe);

        // Buffers
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->rcvbuf, param->buffer.rcvbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->linbuf, param->buffer.linbuf_size);

        pipe->udpdata.size_max = param->buffer.udpbuf_size;
        OS_InitThreadQueue(&pipe->udpdata.waiting);
    }

    // 送信機能に関する初期化
    if (param->buffer.sndbuf_size)
    {
        SOCLiSocketSendCommandPipe*     pipe = (SOCLiSocketSendCommandPipe*)ptr;

        // Send Command Pipe
        socket->send_pipe = pipe;   // ソケットとパイプの相互リンク
        pipe->exe_socket = socket;  //   Send Pipe 共有時に有効
        ptr += SOCLi_ROUNDUP4(sizeof(SOCLiSocketSendCommandPipe));
        ptr = SOCLi_InitCommandPipe(ptr, &pipe->h, &param->send_pipe);

        // Buffers
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->sndbuf, param->buffer.sndbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->outbuf, param->buffer.outbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &pipe->buffer.area, param->buffer.wrtbuf_size);
        OS_InitThreadQueue(&pipe->buffer.waiting);
    }
    else
    {
        // Send Pipe を共有する
        socket->send_pipe = ((SOCLSocket*)SOCLiUDPSendSocket)->send_pipe;
    }

    return ptr;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_InitSocketBuffer
  
  Description:  SOCKET 内のバッファを初期化する

  Arguments:    ptr     バッファ用に確保されたメモリ
                buf     バッファ構造体
                size    バッファサイズ
  
  Returns:      バッファの領域の次に利用可能なポインタ
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitSocketBuffer(u8* ptr, CPSSocBuf* buf, u32 size)
{
    buf->data = size ? ptr : NULL;  // 0 なら NULL を設定
    buf->size = size;
    return ptr + SOCLi_RoundUp4(size);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_InitCommandPipe
  
  Description:  コマンドパイプを初期化する

  Arguments:    ptr     コマンドパイプ内のバッファ用に確保されたメモリ
                buf     バッファ構造体
                size    バッファサイズ
  
  Returns:      ptr の次に利用可能なポインタ
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitCommandPipe(u8* ptr, SOCLiSocketCommandPipe* pipe, const SOCLSocketCommandPipeParam* param)
{
    OSMessage*  messages;
    u8*         stack_bottom;

    // メモリの割り振り
    messages = (OSMessage*)ptr;
    stack_bottom = ptr + SOCLi_GetSizeCommandPipe(param);

    // 各領域の初期化
    OS_InitMessageQueue(&pipe->queue, messages, param->queue_max);

    OS_InitMutex(&pipe->in_use);

    OS_CreateThread(&pipe->thread, SOCLi_CommandPacketHandler, pipe, stack_bottom, param->stack_size, param->priority);
    OS_WakeupThreadDirect(&pipe->thread);

    return stack_bottom;
}
