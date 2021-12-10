/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_read.c

  Copyright 2005-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_read.c,v $
  Revision 1.41  2007/06/20 09:04:26  yasu
  コネクションの不意の断絶の対応

  Revision 1.40  2006/07/24 01:35:37  adachi_hiroaki
  UDP バッファがあふれていても、次のパケットの受信準備を正しく行うように修正 [nitro-ip:0170]

  Revision 1.39  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.38  2005/12/06 11:53:13  yasu
  TCP BLOCK Write における不具合の修正
  BLOCK モード判定用のマクロの導入

  Revision 1.37  2005/09/19 14:08:29  yasu
  Closing 状態を追加した

  Revision 1.36  2005/09/16 12:13:50  yasu
  イベントカウンタを FinalRom でも有効にする

  Revision 1.35  2005/09/14 06:00:19  yasu
  SOCLi_CheckCount のサポート

  Revision 1.34  2005/09/09 07:38:57  yasu
  Block 状況の判定を修正

  Revision 1.33  2005/09/08 05:35:34  yasu
  ソケットデスクリプタが有効値であるかどうかの判定を SOCL 側で行なうことにする

  Revision 1.32  2005/09/01 06:42:55  yasu
  ソケットが生きているかどうかの判定を一元化

  Revision 1.31  2005/08/25 08:27:38  yasu
  UDP 受信におけるバッファリング処理追加

  Revision 1.30  2005/08/24 09:25:13  yasu
  SOCL_SocketIsInvalid 追加

  Revision 1.29  2005/08/14 10:59:34  yasu
  consumed_min 制御の追加

  Revision 1.28  2005/08/13 08:20:27  yasu
  consumed_min 制御の追加

  Revision 1.27  2005/08/12 09:50:23  yasu
  EOF に達したときの接続ステータスの設定を追加

  Revision 1.26  2005/08/11 12:02:50  yasu
  consume 後の UDP ソケットの接続リセット処理の甘い部分があったのを修正

  Revision 1.25  2005/08/11 10:07:15  yasu
  read 時のローカルポートの UDP バインド結果を保存する

  Revision 1.24  2005/08/11 07:02:34  yasu
  デバッグ出力の削除

  Revision 1.23  2005/08/10 12:34:53  yasu
  バッファの操作変更

  Revision 1.22  2005/08/10 09:31:44  seiki_masashi
  SSL のサポートを追加

  Revision 1.21  2005/08/10 04:49:13  yasu
  受信時の返り値を変更

  Revision 1.20  2005/08/09 12:26:33  yasu
  consume 処理の修正

  Revision 1.19  2005/08/09 03:20:14  yasu
  TCP/UDP の consume 扱いの変更

  Revision 1.18  2005/08/08 14:15:18  yasu
  read の返値を変更

  Revision 1.17  2005/08/07 13:04:49  yasu
  Read の返値の再修正

  Revision 1.16  2005/08/04 04:15:56  yasu
  初期化バグ修正

  Revision 1.15  2005/08/04 04:08:28  yasu
  UDP read の返り値の修正

  Revision 1.14  2005/08/02 13:12:32  seiki_masashi
  EOF 判定用定数を enum 値に置き換え

  Revision 1.13  2005/08/02 10:50:09  yasu
  NoBlock の EOF 判定処理の暫定対応

  Revision 1.12  2005/08/02 09:40:11  yasu
  BLCOK READ モード時の処理修正追加

  Revision 1.11  2005/08/02 08:13:12  yasu
  READ の EOF の暫定実装．
  BLCOK READ モード時の処理修正

  Revision 1.10  2005/08/02 07:00:58  yasu
  NOBLOCK モードにおける EOF 対応

  Revision 1.9  2005/08/01 07:04:27  seiki_masashi
  non-blocking 時にデータがない場合に EWOULDBLOCK を返すように修正
  blocking 時の SOCLi_ExecReadCommand に与える引数を修正

  Revision 1.8  2005/08/01 06:26:36  yasu
  SOCLi_ReadCallBack　におけるデータがないときの返値の変更

  Revision 1.7  2005/08/01 05:55:26  yasu
  recv_pipe->consumed の減算タイミングを修正

  Revision 1.6  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.5  2005/07/30 15:30:29  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.4  2005/07/25 10:15:01  yasu
  SOC ライブラリの追加

  Revision 1.3  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.2  2005/07/19 14:06:33  yasu
  SOCL_Read/Write 追加

  Revision 1.1  2005/07/15 13:33:11  yasu
  soc ライブラリのソース調整

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_ReadAndConsumeBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32, s32);
static int  SOCLi_ReadBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32);
static int  SOCLi_ReadUdpBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32);
static int  SOCLi_CopyCPSBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* );
static u8*  SOCLi_ReadCPSBuffer(SOCLSocket* , s32* , u16* , u16* , SOCLInAddr* );
static int  SOCLi_ExecReadCommand(SOCLSocket* , void* , int, u16* , SOCLInAddr* );
static int  SOCLi_ReadCallBack(void* );
static int  SOCLi_ConsumeBuffer(SOCLSocket* );
static int  SOCLi_ConsumeCallBack(void* );
static int  SOCLi_ConsumeCPSBuffer(SOCLSocket* );

u32         SOCLi_CheckCount[SOCL_CHECKCOUNT_MAX] = { 0};

/*---------------------------------------------------------------------------*
  Name:         SOCL_Read

  Description:  ソケットからデータを受け取ります

  Arguments:    socket          ソケット
                buffer          受信データ
                buffer_len      受信データを格納バッファの長さ
                remote_port     送信元ホスト port
                remote_ip       送信元ホスト ip

                remote_port/remote_ip は Connect を行なっていない UDP の場合
                のみ有効. TCP や Connect を行なった UDP の場合は値は無視される

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_Read(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_ReadNoBlock

  Description:  ソケットから NOBLOCK モードでデータを受け取ります．

  Arguments:    socket          ソケット
                buffer          受信データ
                buffer_len      受信データの長さ
                remote_port     送信元ホスト port (UDP のみ有効)
                remote_ip       送信元ホスト ip   (UDP のみ有効)

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_ReadNoBlock(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, SOCL_MSG_DONTWAIT);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_ReadBlock

  Description:  ソケットから BLOCK モードでデータを受け取ります．

  Arguments:    socket          ソケット
                buffer          受信データ
                buffer_len      受信データの長さ
                remote_port     送信元ホスト port (UDP のみ有効)
                remote_ip       送信元ホスト ip   (UDP のみ有効)

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_ReadBlock(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_ReadFrom

  Description:  ソケットからデータを受け取ります

  Arguments:    socket          ソケット
                buffer          受信データ
                buffer_len      受信データを格納バッファの長さ
                remote_port     送信元ホスト port
                remote_ip       送信元ホスト ip
                flag            受信フラグ

                remote_port/remote_ip は Connect を行なっていない UDP の場合
                のみ有効. TCP や Connect を行なった UDP の場合は値は無視される

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_ReadFrom(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip, int flags)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    int flag_block;
    int result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // 規定外の値
    }

    if ((flags & SOCL_MSG_DONTWAIT) || SOCL_SocketIsNoBlock(socket))
    {
        if (SOCL_SocketIsSSL(socket))
        {
            return SOCL_EINVAL; // SSL はブロッキング呼び出ししかサポートしない
        }

        flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }
    else
    {
        if (OS_GetProcMode() == OS_PROCMODE_IRQ)
        {
            return SOCL_EINVAL; // ブロッキング呼び出しは割り込みハンドラ内で呼び出せない
        }

        flag_block = SOCL_FLAGBLOCK_BLOCK;
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // 初期化されていない
    }

    if (SOCL_SocketIsTCP(socket) && (!SOCL_SocketIsConnected(socket) || SOCL_SocketIsClosing(socket)))
    {
        return SOCL_ENOTCONN;   // 接続していない
    }

    SDK_ASSERT(buffer && buffer_len);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);      // 初期化のされ方がおかしい
    if (!SOCL_FLAGISBLOCK(flag_block))
    {
        if (!OS_TryLockMutex(&recv_pipe->h.in_use))
        {
            return SOCL_EWOULDBLOCK;
        }
    }
    else
    {
        OS_LockMutex(&recv_pipe->h.in_use);
    }

    result = SOCLi_ReadAndConsumeBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block, flags);

    OS_UnlockMutex(&recv_pipe->h.in_use);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ReadAndConsumeBuffer

  Description:  READ および Consume 処理を開始します．
                ただし排他制御は行なわれていません．

  Arguments:    socket          ソケット
                buffer          コピーデータ
                buffer_len      コピーデータの長さ
                remote_port     送信元ホスト port (UDP のみ有効)
                remote_ip       送信元ホスト ip   (UDP のみ有効)
                flag_block      BLOCK するかどうかのフラグ

  Returns:      コピーできたデータ長 (0 なら失敗)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadAndConsumeBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                                      SOCLInAddr*  remote_ip, s32 flag_block, s32 flags)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    BOOL    is_noconsumed = ((flags & SOCL_MSG_PEEK) && recv_pipe);
    int     result;
    s8      flag_noconsume;

    if (is_noconsumed)
    {
        // 一時的に noconsume フラグを立てる
        flag_noconsume = recv_pipe->flag_noconsume;
        recv_pipe->flag_noconsume = TRUE;
    }

    if (SOCL_SocketIsUDP(socket))
    {
        result = SOCLi_ReadUdpBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);
    }
    else
    {
        result = SOCLi_ReadBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);

        if (result >= 0)
        {
            (void)SOCLi_ConsumeBuffer(socket);
        }
    }

    if (is_noconsumed)
    {
        // noconsume フラグを元に戻す
        recv_pipe->flag_noconsume = flag_noconsume;
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ReadBuffer

  Description:  READ 処理を開始します．ただし排他制御は行なわれていません．

  Arguments:    socket          ソケット
                buffer          コピーデータ
                buffer_len      コピーデータの長さ
                remote_port     送信元ホスト port (UDP のみ有効)
                remote_ip       送信元ホスト ip   (UDP のみ有効)
                flag_block      BLOCK するかどうかのフラグ
                                BLOCK ならコマンドを作成して実行します．

  Returns:      コピーできたデータ長 (0 なら失敗)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port, SOCLInAddr*  remote_ip,
                            s32 flag_block)
{
    int result;

#ifdef SDK_MY_DEBUG
    OS_TPrintf("\n===\nReadBuffer.buffer                 =%d-%d %d\n", buffer, (u32) buffer + buffer_len, buffer_len);
#endif

    // SSL の場合は CPS の受信バッファを覗き見ることができないため、
    // 必ず受信スレッドにコマンドを発行する
    if (SOCL_SocketIsSSL(socket))
    {
        result = SOCLi_ExecReadCommand(socket, buffer, buffer_len, remote_port, remote_ip);
        return result;
    }

    //
    // まず、BLOCK/NOBLOCK に寄らずに NOBLOCK リードを発行する．
    // もし BLOCK モードの場合 EWOULDBLOCK が返ってきた場合、そのうちデータ
    // が到着することを期待して recv_pipe へ READ コマンドを発行する
    //
    result = SOCLi_CopyCPSBuffer(socket, buffer, buffer_len, remote_port, remote_ip);

    if (result == SOCL_EWOULDBLOCK && SOCL_FLAGISBLOCK(flag_block))
    {
        result = SOCLi_ExecReadCommand(socket, buffer, buffer_len, remote_port, remote_ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CopyCPSBuffer

  Description:  CPS ライブラリの受信バッファからデータをコピーする
                NOBLOCK リードを試みるという形式になっている

  Arguments:    socket          ソケット
                buffer          コピー先受信バッファタ
                buffer_len      コピー先受信バッファの長さ
                remote_port     受信相手のポートへのポインタ
                remote_ip       受信相手の IP へのポインタ

  Returns:      正の値: TCP バッファへコピーしたサイズ
                        UDP バッファへ送られてきていたサイズ
                0     : EOF
                負の値: エラー
                        SOCL_EWOULDBLOCK: データが送られてきていない
                        SOCL_EINVAL     : 内部状態がおかしくなっている
 *---------------------------------------------------------------------------*/
static int SOCLi_CopyCPSBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                               SOCLInAddr*  remote_ip)
{
    OSIntrMode  enable;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    u8*         cpsbuffer;
    s32         cpsbuffer_len;
    s32         copy_len;
    s32         consume_len;
    u16         my_port;
    u16         his_port;
    SOCLInAddr  his_ip;
    int         result = 0;

    SDK_ASSERT(buffer && buffer_len > 0);

    //
    // データのコピー中に Consume が掛かることを防止する
    // Mutex にする方法もあるがここでは割り込みで処理を行なう．
    //
    enable = OS_DisableInterrupts();
    {
        //
        // 受信バッファに着いているデータを確認し、データがあるならバッファに
        // コピーする．そうでなく、BLOCK モードであるなら READ コマンドを発行し
        // recv_pipe で転送を行なう
        //
        cpsbuffer = SOCLi_ReadCPSBuffer(socket, &cpsbuffer_len, &my_port, &his_port, &his_ip);

        if (cpsbuffer)
        {
            if (cpsbuffer_len == 0)
            {
                // ポインタはセットされていてサイズが 0 のときは
                // データが届いていない
                result = SOCL_EWOULDBLOCK;
            }
            else
            {
                // データのコピーサイズ、消費サイズを計算する
                //   到着しているデータサイズをバッファサイズまでコピーする
                //   TCP はコピーした分を UDP は到着しているデータ全てを
                //   捨てる
                copy_len = MATH_MIN(buffer_len, cpsbuffer_len);
                consume_len = SOCL_SocketIsTCP(socket) ? copy_len : cpsbuffer_len;

                // コピー＆廃棄
                // PEEK 読みの場合は廃棄しない
                MI_CpuCopy8(cpsbuffer, buffer, (u32) copy_len); // コピー
#ifdef SDK_MY_DEBUG
                OS_TPrintf("MI_CPUCOPY    .buffer             =%d-%d %d\n", buffer, (u32) buffer + copy_len, copy_len);
                OS_TPrintf("              .copy_len           =%d\n", copy_len);
#endif
                recv_pipe = socket->recv_pipe;
                SDK_ASSERT(recv_pipe);
                if (!recv_pipe->flag_noconsume)
                {
                    recv_pipe->consumed += consume_len;         // 廃棄
                }

#ifdef SDK_MY_DEBUG
                OS_TPrintf("              .recv_pipe->consumed=%d\n", recv_pipe->consumed);
#endif
                result = (int)consume_len;
            }
        }
        else
        {
            // cpsbuffer のポインタが返らずにサイズが 0 なら EOF
            // 0 を返す
            if (cpsbuffer_len == 0)
            {
                result = 0;
            }
            else
            {
                result = SOCL_EINVAL;   // System error
            }

            socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED);
        }

        if (result >= 0)
        {
            //
            // 相手 IP/Port のコピー
            // ローカルポートの確定
            //
            if (remote_port && remote_ip)
            {
                *remote_port = his_port;
                *remote_ip = his_ip;
            }

            if (socket->local_port == 0)
            {
                socket->local_port = my_port;
            }
        }
    }
    (void)OS_RestoreInterrupts(enable);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ReadCPSBuffer

  Description:  CPS ライブラリの受信バッファの状態を調べる

  Arguments:    socket          ソケット
                buffer_len      受信バッファの長さへのポインタ
                local_port      自ホストのポートへのポインタ
                remote_port     受信相手のポートへのポインタ
                remote_ip       受信相手の IP へのポインタ

  Returns:      非 0: 受信バッファの先頭アドレス
                        buffer_len にサイズが返る
                        0 ならデータが着くのを待っている状態
                   0: エラー or EOF
                        このとき buffer_len にエラーコードが返る
                        0 なら EOF  負の値ならシステムエラー
 *---------------------------------------------------------------------------*/
static u8* SOCLi_ReadCPSBuffer(SOCLSocket*  socket, s32*  buffer_len, u16*  local_port, u16*  remote_port,
                               SOCLInAddr*  remote_ip)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    CPSSoc*     cpsoc;
    s32         consumed;
    s32         len;

    recv_pipe = socket->recv_pipe;
    cpsoc = (CPSSoc*)OSi_GetSpecificData(&recv_pipe->h.thread, OSi_SPECIFIC_CPS);

    consumed = recv_pipe->consumed;
    len = (s32) cpsoc->rcvbufp - consumed;

    if (len >= 0)
    {
        (*local_port) = cpsoc->local_port;
        (*remote_port) = cpsoc->remote_port;
        (*remote_ip) = (SOCLInAddr) cpsoc->remote_ip;
        (*buffer_len) = len;

        // EOF 判定
        if (len == 0 && cpsoc->state != CPS_STT_ESTABLISHED)
        {
            return NULL;
        }
    }
    else
    {
        (*buffer_len) = -1;
        return NULL;    // Error
    }

    return((u8*)cpsoc->rcvbuf.data) + consumed;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecReadCommand

  Description:  BLOCK READ 発行コマンド

  Arguments:    s                 ソケット
                user_buffer       READ 先
                user_buffer_len   READ バッファサイズ

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecReadCommand(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                                 SOCLInAddr*  remote_ip)
{
    SOCLiCommandPacket*     command;
    s32 result;

    // recv_pipe にコマンドを投げ、コールバック関数内部で
    // CPS_Read/CPS_SocConsume を起動する．
    // 強制的に BLOCK モードなので成功するまで待つ
    command = SOCLi_CreateCommandPacket(SOCLi_ReadCallBack, socket, SOCL_FLAGBLOCK_BLOCK);
    SDK_ASSERT(command);

    command->read.buffer = buffer;
    command->read.buffer_len = buffer_len;
    command->read.remote_port = remote_port;
    command->read.remote_ip = remote_ip;

    // コマンド実行
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ReadCallBack

  Description:  READ 処理のコールバック．
                BLOCK モードのときのみ

  Arguments:    arg   コマンドパケットへのポインタ

  Returns:      正の値: 受信したメッセージのバイト数
                     0: EOF
                負の値: エラー
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadCallBack(void* arg)
{
    SOCLiCommandRead*   cpacket = (SOCLiCommandRead*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    u8*         buffer = cpacket->buffer;
    s32         buffer_len = cpacket->buffer_len;
    u16*        remote_port = cpacket->remote_port;
    SOCLInAddr*     remote_ip = cpacket->remote_ip;
    u8*         cpsbuffer;
    u32 cpsbuffer_len;
    s32 consumed;
    int result;

    // 新しいデータが追加されるまで BLOCK モードで待つ.
    // 現状では SOCL レイヤーにおいて本機能を実装するために OS_Sleep を
    // 使ったポーリングを行なっているが、CPS レイヤーの内部の実装を修正
    // して新規データ到着時に BLOCK 解除を行なうべき．
    consumed = recv_pipe->consumed;
    for (;;)
    {
        cpsbuffer = CPS_SocRead(&cpsbuffer_len);
        if (cpsbuffer == NULL || (s32) cpsbuffer_len - consumed > 0)
        {
            break;
        }

        if (SOCL_SocketIsTCP(socket) && socket->cps_socket.state != CPS_STT_ESTABLISHED)
        {
            cpsbuffer = NULL;   // EOF
            break;
        }

        OS_Sleep(10);           // 10msec の待ち
    }

    // 切断されたならエラー
    if (SOCL_SocketIsReset(socket))
    {
        return SOCL_ECONNRESET;
    }
    
    // SSL の場合は受信バッファを覗き見れないために
    // 単純に cpsbuffer から受信データをコピーする
    // SSL は必ずブロッキングで呼び出されるため、処理は単純となる
    if (SOCL_SocketIsSSL(socket))
    {
        u32 len;

        if (cpsbuffer == NULL)
        {
            return 0;
        }

        len = MATH_MIN(buffer_len, cpsbuffer_len);
        MI_CpuCopy8(cpsbuffer, buffer, len);
        CPS_SocConsume(len);
        return (int)len;
    }

    // CPS_SocRead で既にバッファにデータが追加されていることが期待できる
    // ため、この呼び出しは直ぐに終了する．cpsbuffer が 0 の時は EOF となる
    if (cpsbuffer)
    {
        result = SOCLi_CopyCPSBuffer(socket, buffer, buffer_len, remote_port, remote_ip);
    }
    else
    {
        result = 0 /*EOF*/ ;
    }

    // エラーまたは EOF ならその返値で終了
    if (result <= 0)
    {
        return result;
    }

    //
    // consumed が consumed の最小長以上なら Consume 処理を行なう
    //
    SDK_ASSERT(recv_pipe->consumed_min > 0);
    if (recv_pipe->consumed >= (s32) recv_pipe->consumed_min)
    {
        (void)SOCLi_ConsumeCPSBuffer(socket);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ConsumeBuffer

  Description:  Consume 処理を行なう

  Arguments:    s                 ソケット

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeBuffer(SOCLSocket* socket)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    SOCLiCommandPacket*         command;
    s32 result;

    //
    // consume が consume の最小長未満なら何もせずに終了する
    //
    SDK_ASSERT(recv_pipe->consumed_min > 0);
    if (recv_pipe->consumed < (s32) recv_pipe->consumed_min)
    {
        return SOCL_ESUCCESS;
    }

    // recv_pipe にコマンドを投げ、コールバック関数内部で
    // CPS_SocConsume を起動する．NOBLOCK で処理する
    command = SOCLi_CreateCommandPacket(SOCLi_ConsumeCallBack, socket, SOCL_FLAGBLOCK_NOBLOCK);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

    // コマンド実行
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ConsumeCallBack

  Description:  Consume 処理のコールバック．

  Arguments:    arg   コマンドパケットへのポインタ

  Returns:      consume したデータのバイト数
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeCallBack(void* arg)
{
    SOCLiCommandConsume*    cpacket = (SOCLiCommandConsume*)arg;

    return SOCLi_ConsumeCPSBuffer(cpacket->h.socket);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ConsumeCPSBuffer

  Description:  Consume 処理を行なう

  Arguments:    recv_pipe 受信パイプ

  Returns:      consume したデータのバイト数
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeCPSBuffer(SOCLSocket* socket)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    OSIntrMode  enable;
    s32         consumed;

    SDK_ASSERT(recv_pipe);

    // consumed パラメータを操作するので割り込みを停止させ、
    // consumed と CPS_SocConsume 内でのパラメータとの制御を同調させる
    enable = OS_DisableInterrupts();
    consumed = recv_pipe->consumed;

    if (consumed)
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("\nCPS_SocConsume(%d)\n", consumed);
#endif

        //
        // CPS_SocConsume の内部で、内部バッファが開放されるタイミングが
        // BLOCKing される前か後かで recv_pipe->consumed を減じるタイミングを
        // 変更する必要がある．この部分は CPS_SocConsume の実装依存なので注意
        //
#if 1   // 内部バッファ開放後 BLOCKing する場合は先に consumed を減じる(現状)
        recv_pipe->consumed = 0;
        CPS_SocConsume((u32) consumed); // BLOCKED
#else // 内部バッファ開放前 BLOCKing する場合は後で consumed を減じる
        CPS_SocConsume((u32) consumed); // BLOCKED
        recv_pipe->consumed -= consumed;
#endif

#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocConsume.cpssoc.state      =%d\n", socket->cps_socket.state);
        OS_TPrintf("              .cpssoc.rcvbufp    =%d\n", socket->cps_socket.rcvbufp);
        OS_TPrintf("              .recv_pipe.consumed=%d\n", consumed);
#endif
    }
    (void)OS_RestoreInterrupts(enable);

    return (int)consumed;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ReadUdpBuffer

  Description:  READ 処理を開始します．ただし排他制御は行なわれていません．

  Arguments:    socket          ソケット
                buffer          コピーデータ
                buffer_len      コピーデータの長さ
                remote_port     送信元ホスト port (UDP のみ有効)
                remote_ip       送信元ホスト ip   (UDP のみ有効)
                flag_block      BLOCK するかどうかのフラグ
                                BLOCK ならコマンドを作成して実行します．

  Returns:      コピーできたデータ長 (0 なら失敗)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadUdpBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                               SOCLInAddr*  remote_ip, s32 flag_block)
{
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;
    SOCLiSocketUdpData*         udpdata = pipe->udpdata.out;
    OSIntrMode  enable;
    int         result;

    enable = OS_DisableInterrupts();
    while (NULL == (udpdata = pipe->udpdata.out))
    {
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            result = SOCL_EWOULDBLOCK;
            break;
        }

        OS_SleepThread(&pipe->udpdata.waiting);

        if (SOCL_SocketIsInvalid(socket) || !SOCL_SocketIsCreated(socket))
        {
            result = SOCL_ENOTCONN;
            break;  // Sleep 中にソケットが close された
        }
    }

    if (udpdata)
    {
        MI_CpuCopy8((u8*) &udpdata[1], buffer, (u32) MATH_MIN(buffer_len, udpdata->size));

        if (remote_port)
        {
            (*remote_port) = udpdata->remote_port;
        }

        if (remote_ip)
        {
            (*remote_ip) = udpdata->remote_ip;
        }

        result = udpdata->size;

        if (!pipe->flag_noconsume)
        {
            pipe->udpdata.out = udpdata->next;
            if (udpdata->next == NULL)
            {
                pipe->udpdata.in = NULL;
            }

            SOCLi_Free(udpdata);
            pipe->udpdata.size -= result;
        }
    }
    (void)OS_RestoreInterrupts(enable);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_UdpRecvCallback

  Description:  UDP パケット着信時のコールバック

  Arguments:    data  データへのポインタ
                len   データ長
                soc   CPSSoc構造体へのポインタ

  Returns:      1 : バッファを破棄する
 *---------------------------------------------------------------------------*/
int SOCLi_UdpRecvCallback(u8* data, u32 len, CPSSoc* soc)
{
    SOCLSocket*     socket = (SOCLSocket*)soc;
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;
    SOCLiSocketUdpData*         udpheader;
    OSIntrMode  enable = OS_DisableInterrupts();

    if (pipe->udpdata.size_max >= pipe->udpdata.size + len)
    {
        // 受信バッファへのコピー
        udpheader = SOCLi_Alloc(sizeof(SOCLiSocketUdpData) + len);

        if (udpheader)
        {
            pipe->udpdata.size += len;
            udpheader->next = NULL;
            udpheader->size = (u16) len;
            udpheader->remote_port = soc->remote_port;
            udpheader->remote_ip = soc->remote_ip;
            MI_CpuCopy8(data, (void*) &udpheader[1], len);

            if (pipe->udpdata.in)
            {
                pipe->udpdata.in->next = udpheader;
            }

            pipe->udpdata.in = udpheader;

            if (pipe->udpdata.out == NULL)
            {
                pipe->udpdata.out = udpheader;
            }
        }
        else
        {
            SOCLi_CheckCount[SOCL_CHECKCOUNT_UDPRCVCB_1]++;
        }
    }
    else
    {
        SOCLi_CheckCount[SOCL_CHECKCOUNT_UDPRCVCB_2]++;
    }
    
    // ローカルポート確定
    if (socket->local_port == 0)
    {
        socket->local_port = soc->local_port;
    }

    // 次のデータの受信準備
    soc->remote_port = soc->remote_port_bound;
    soc->remote_ip = soc->remote_ip_bound;

    // 待っているスレッドがあるなら起こす
    OS_WakeupThread(&pipe->udpdata.waiting);

    (void)OS_RestoreInterrupts(enable);

    return 1;   // データ破棄
}
