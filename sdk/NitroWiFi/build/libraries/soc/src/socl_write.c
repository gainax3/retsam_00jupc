/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_write.c

  Copyright 2005-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_write.c,v $
  Revision 1.35  2007/06/20 09:04:26  yasu
  コネクションの不意の断絶の対応

  Revision 1.34  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.33  2006/01/17 07:26:22  yasu
  著作年度 2005->2006 変更

  Revision 1.32  2006/01/17 07:24:36  yasu
  TCP BLOCK Write における不具合の修正の再追加
  BLOCK モード判定用のマクロの導入

  Revision 1.31  2005/10/20 09:08:14  yasu
  送信失敗時において Write バッファを開放していない不具合修正

  Revision 1.30  2005/10/18 06:12:44  terui
  TCP送信スループット向上に関連する変更

  Revision 1.29  2005/09/19 15:11:37  yasu
  Closing 状態を追加した部分の微調整

  Revision 1.28  2005/09/19 14:08:29  yasu
  Closing 状態を追加した

  Revision 1.27  2005/09/19 04:51:49  seiki_masashi
  small fix

  Revision 1.26  2005/09/17 11:54:52  yasu
  typo 修正

  Revision 1.25  2005/09/17 10:14:00  yasu
  SocWrite のエラー時のハンドリンクを厳しくした

  Revision 1.24  2005/09/13 07:27:06  yada
  applied for connect() on UDP

  Revision 1.23  2005/09/08 05:35:34  yasu
  ソケットデスクリプタが有効値であるかどうかの判定を SOCL 側で行なうことにする

  Revision 1.22  2005/08/24 09:25:13  yasu
  SOCL_SocketIsInvalid 追加

  Revision 1.21  2005/08/11 10:06:09  yasu
  write での UDP バインド結果を CPS 層に反映させる

  Revision 1.20  2005/08/11 07:56:32  yasu
  UDP の Write 時に明示的に Eポートを取得する

  Revision 1.19  2005/08/11 07:30:03  yasu
  Write コマンドのパイプ送信時のエラーハンドリングの修正

  Revision 1.18  2005/08/11 05:08:51  yasu
  リングバッファ管理の修正

  Revision 1.17  2005/08/10 12:34:04  yasu
  バッファの操作変更

  Revision 1.16  2005/08/10 04:51:12  yasu
  リングバッファ full 時の処理修正

  Revision 1.15  2005/08/09 12:27:12  yasu
  リングバッファからのデータコピー処理の修正

  Revision 1.14  2005/08/09 07:43:29  yasu
  LANG=en でのコンパイル対応

  Revision 1.13  2005/08/09 04:31:10  yasu
  ASSERT 処理の正偽判定の修正

  Revision 1.12  2005/08/09 03:20:39  yasu
  混入していたデバックメッセージ削除

  Revision 1.11  2005/08/09 01:21:02  yasu
  sndbuf サイズの制限処理の修正

  Revision 1.10  2005/08/08 14:16:36  yasu
  TCP の send 処理の対応

  Revision 1.9  2005/08/01 07:02:50  seiki_masashi
  non-blocking 時にバッファの空きがない場合に EWOULDBLOCK を返すように修正

  Revision 1.8  2005/08/01 04:55:25  yasu
  SOCL_Write の返値の修正

  Revision 1.7  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.6  2005/07/30 15:30:52  yasu
  コマンドパイプ分離にともなう修正

  Revision 1.5  2005/07/28 11:16:12  yasu
  共有可能なコードを切り出して関数へ

  Revision 1.4  2005/07/27 12:19:00  yasu
  sendto の追加

  Revision 1.3  2005/07/25 10:15:01  yasu
  SOC ライブラリの追加

  Revision 1.2  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.1  2005/07/19 14:06:33  yasu
  SOCL_Read/Write 追加

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_WriteBuffer(SOCLSocket* , const u8* , s32, u16, SOCLInAddr, s32);
static int  SOCLi_ExecWriteCommand(SOCLSocket* , const u8* , s32, s32, u16, SOCLInAddr, s32);
static int  SOCLi_WriteCallBack(void* );
static s32  SOCLi_AllocWriteBuffer(SOCLSocket* , s32, s32, s32* , s32);
static int  SOCLi_MemCpy(u8* , int, SOCLiCommandWrite* );

#if SOCL_OPTIMIZE_TCP_SEND
static int  SOCLi_GetOptimumSendBufLen(int);
#endif

/*---------------------------------------------------------------------------*
  Name:         SOCL_Write

  Description:  ソケットからデータを送ります．

  Arguments:    socket          ソケット
                buffer          送信データ
                buffer_len      送信データの長さ
                remote_port     送信先ホスト port
                remote_ip       送信先ホスト ip

                remote_port/remote_ip は Connect を行なっていない UDP の場合
                のみ有効. TCP や Connect を行なった UDP の場合は値は無視される

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_Write(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_WriteNoBlock

  Description:  ソケットから NOBLOCK モードでデータを送ります．

  Arguments:    socket          ソケット
                buffer          送信データ
                buffer_len      送信データの長さ
                remote_port     送信先ホスト port (UDP のみ有効)
                remote_ip       送信先ホスト ip   (UDP のみ有効)

  Returns:      正の値: 送信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_WriteNoBlock(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, SOCL_MSG_DONTWAIT);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_WriteBlock

  Description:  ソケットから BLOCK モードでデータを送ります．

  Arguments:    socket          ソケット
                buffer          送信データ
                buffer_len      送信データの長さ
                remote_port     送信先ホスト port (UDP のみ有効)
                remote_ip       送信先ホスト ip   (UDP のみ有効)

  Returns:      正の値: 送信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_WriteBlock(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_WriteTo

  Description:  ソケットからデータを送ります．

  Arguments:    socket          ソケット
                buffer          送信データ
                buffer_len      送信データの長さ
                remote_port     送信先ホスト port
                remote_ip       送信先ホスト ip
                flag            送信フラグ

                remote_port/remote_ip は Connect を行なっていない UDP の場合
                のみ有効. TCP や Connect を行なった UDP の場合は値は無視される

  Returns:      正の値: 受信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
int SOCL_WriteTo(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip, int flags)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    SOCLiSocketSendCommandPipe*     send_pipe;
    int flag_block;
    int result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // 規定外の値
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
    send_pipe = socket->send_pipe;
    SDK_ASSERT(send_pipe);      // 初期化のされ方がおかしい

    //
    // Lock をかけて他のスレッドからのアクセスを排除する．
    // 特にバッファの内部にアクセスする処理は注意深く排他をかける必要あり
    //
    if ((flags & SOCL_MSG_DONTWAIT) || SOCL_SocketIsNoBlock(socket))
    {
        if (!OS_TryLockMutex(&send_pipe->h.in_use))
        {
            return SOCL_EWOULDBLOCK;
        }

        flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }
    else
    {
        OS_LockMutex(&send_pipe->h.in_use);
        flag_block = SOCL_FLAGBLOCK_BLOCK;
    }

    result = SOCLi_WriteBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);
    OS_UnlockMutex(&send_pipe->h.in_use);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_WriteBuffer

  Description:  WRITE 処理を開始します．ただし排他制御は行なわれていません．

  Arguments:    socket          ソケット
                buffer          コピーデータ
                buffer_len      コピーデータの長さ
                remote_port     送信先ホスト port (UDP のみ有効)
                remote_ip       送信先ホスト ip   (UDP のみ有効)
                flag_block      BLOCK するかどうかのフラグ

  Returns:      コピーできたデータ長 (0 なら失敗)
 *---------------------------------------------------------------------------*/
static int SOCLi_WriteBuffer(SOCLSocket*  socket, const u8*  buffer, s32 buffer_len, u16 remote_port,
                             SOCLInAddr remote_ip, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    s32 sndbuf_len;
    s32 wrtbuf_in;
    s32 write_len;
    s32 buffer_min;
    s32 copied_len = 0;
#ifdef SDK_DEBUG
    s32 wrtbuf_size = (s32) send_pipe->buffer.area.size - 1;
#endif
    int result;

    SDK_ASSERT(send_pipe);              // 初期化のされ方がおかしい
    SDK_ASSERT(send_pipe->exe_socket);  // 初期化のされ方がおかしい

    // 送信スロット(sndbuf)のバッファサイズを取得する
    sndbuf_len = (s32) send_pipe->exe_socket->cps_socket.sndbuf.size;

    if (SOCL_SocketIsUDP(socket))
    {
        sndbuf_len -= SOCL_UDP_SNDBUF_NOCOPY_OFFSET;

        // UDP の場合、sndbuf/wrtbuf のサイズよりも転送サイズが大きい場合は
        // 転送不可
        // SEND PIPE に接続されている CPS ソケットの sndbuf サイズを確認する
        SDK_ASSERT(sndbuf_len >= 0);

        if (buffer_len > sndbuf_len)
        {
            return SOCL_EMSGSIZE;
        }

        // UDP の転送は一度で行なわれるためデータを確保できる十分な大きさの
        // バッファが必要
        buffer_min = buffer_len;
    }
    else
    {
        sndbuf_len -= SOCL_TCP_SNDBUF_NOCOPY_OFFSET;

        // TCP の転送の場合はバッファが少なければ分割する
        // 2つのポリシーがある
#ifdef SDK_TCPSEND_DOIT_SOON
        // NOBLOCK 時に少しでもバッファが空けば詰め込む
        buffer_min = (!SOCL_FLAGISBLOCK(flag_block)) ? 1 : MATH_MIN(buffer_len, sndbuf_len);
#else
        // NOBLOCK 時でも詰め込まない
        buffer_min = MATH_MIN(buffer_len, sndbuf_len);
#endif
    }

    SDK_ASSERT(wrtbuf_size >= sndbuf_len);

    while (buffer_len > 0)
    {
        //
        // バッファを確保する
        //   BLOCK   時は最低でも buffer_min だけの大きさが返るまで BLOCK する
        //   NOBLOCK 時は buffer_min だけの大きさを取得できなければ 0 で返る
        write_len = SOCLi_AllocWriteBuffer(socket, buffer_len, buffer_min, &wrtbuf_in, flag_block);

        //
        // 確保できたらコマンド処理開始
        //
        if (write_len > 0)
        {
            result = SOCLi_ExecWriteCommand(socket, buffer, write_len, wrtbuf_in, remote_port, remote_ip, flag_block);

            if (result <= 0)
            {
                // SOCLi_ExecWriteCommand が ERROR になるのは NOBLOCK 時のみ
                return SOCL_EWOULDBLOCK;
            }

            buffer += write_len;
            buffer_len -= write_len;
            copied_len += write_len;
        }

        //
        // NoBlock ならループ一回で終了
        //
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            if (write_len <= 0)
            {
                // バッファに空きがないことを通知
                return SOCL_EWOULDBLOCK;
            }
            break;
        }
    }

    return (int)copied_len;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_AllocWriteBuffer

  Description:  転送用バッファとして確保可能なサイズを取得します．

                要求した長さのバッファが確保可能になるまで BLOCK します．
                0 を指定した場合は NOBLOCK の扱いになります．

  Arguments:    socket               ソケット
                max_size             取得するバッファの最大
                min_size             取得するバッファの最小
                wrtbuf_in_ptr        バッファの入力先頭
                flag_block           取得できるまで BLOCK する？

  Returns:      確保可能なバッファ長
 *---------------------------------------------------------------------------*/
static s32 SOCLi_AllocWriteBuffer(SOCLSocket* socket, s32 max_size, s32 min_size, s32* wrtbuf_in_ptr, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
#ifdef SDK_DEBUG
    s32         wrtbuf_size = (s32) send_pipe->buffer.area.size - 1;
#endif
    s32         free_size;
    OSIntrMode  enable;

    SDK_ASSERT(send_pipe);
    SDK_ASSERT(wrtbuf_size >= min_size);

    //
    // もし最大サイズと最小サイズが矛盾していたら最大サイズを優先
    // 最大サイズ以上のデータは絶対に必要でない
    // 最小サイズは転送効率を向上するための設定であるため
    //
    if (min_size > max_size)
    {
        min_size = max_size;
    }

    //
    // リングバッファのポインタを取得
    //   wrtbuf_in が入力側/wrtbuf_out が出力側
    //
    enable = OS_DisableInterrupts();
    while (1)
    {
        free_size = SOCLi_GetWriteBufferFreeSize(socket);

        //
        // バッファ長が十分にある場合は終了
        // max_size を超えないように上限を設定する
        //
        if (free_size >= min_size)
        {
            if (free_size >= max_size)
            {
                free_size = max_size;
            }

            *wrtbuf_in_ptr = (s32) send_pipe->buffer.in;
            break;
        }

        //
        // NOBLOCK なら BLOCK せずに取得サイズ 0 で終了
        //
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            free_size = 0;
            break;
        }

        //
        // バッファが十分に得られないときはバッファが空くまで待つ
        //   サイズが足りないことの判断の開始から SleepThread 内で
        //   バッファ空き待ちに入るまでは WriteCallBack が走らない
        //   ようにしなければならない
        //
        OS_SleepThread(&send_pipe->buffer.waiting);

    }
    (void)OS_RestoreInterrupts(enable);

    // リングバッファの空き領域を返す
    return free_size;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetWriteBufferFreeSize

  Description:  空きバッファ量を返します.
                この関数は割り込み禁止状態で呼ばれなければいけません．

  Arguments:    socket                ソケット

  Returns:      空きバッファサイズ
 *---------------------------------------------------------------------------*/
s32 SOCLi_GetWriteBufferFreeSize(SOCLSocket* socket)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    s32 wrtbuf_size;
    s32 wrtbuf_in;
    s32 wrtbuf_out;
    s32 available_len;

    SDK_ASSERT(send_pipe);

    wrtbuf_size = (s32) send_pipe->buffer.area.size;
    wrtbuf_in = (s32) send_pipe->buffer.in;
    wrtbuf_out = (s32) send_pipe->buffer.out;
    available_len = wrtbuf_out - wrtbuf_in - 1;

    if (available_len < 0)
    {
        available_len += wrtbuf_size;
    }

    return available_len;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecWriteCommand

  Description:  WRITE 処理を開始します．ただし排他制御は行なわれていません．
                また事前にリングバッファへデータをコピーできることを確認して
                おく必要があります．

  Arguments:    socket                ソケット
                buffer                コピーデータ
                buffer_len            コピーデータの長さ
                wrtbuf_in             リングバッファへの投入位置
                remote_port           送信先ホスト port (UDP のみ有効)
                remote_ip             送信先ホスト ip   (UDP のみ有効)

  Returns:      コピーできたデータ長 (0 なら失敗)
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecWriteCommand(SOCLSocket*  socket, const u8*  buffer, s32 buffer_len, s32 wrtbuf_in, u16 remote_port,
                                  SOCLInAddr remote_ip, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    SOCLiCommandPacket*         command;
    u8*     wrtbuf;
    s32     wrtbuf_size;
    int     result;
    u16     saved_send_buffer_in;

#ifdef SDK_MY_DEBUG
    OS_TPrintf("socket=%08x exe_socket=%08x\n", socket, send_pipe->exe_socket);
#endif

    // 送信スレッドにコマンドを投げ、コールバック関数内部で CPS_Write を
    // 起動する．
    //    コマンドパケットを取得するのは flag_block に基づいたモードで
    //    処理するが、write の処理に関しては NORESULT で行なう．
    //
    command = SOCLi_CreateCommandPacket(SOCLi_WriteCallBack, send_pipe->exe_socket, flag_block);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

    if (SOCL_FLAGISBLOCK(flag_block) && !SOCL_SocketIsUDP(socket))
    {
        command->h.flag_block = SOCL_FLAGBLOCK_NORESULT;
    }
    else
    {
        command->h.flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }

    wrtbuf = (u8*)send_pipe->buffer.area.data;
    wrtbuf_size = (s32) send_pipe->buffer.area.size;

    // データを順に WriteBuffer へ転送する
    //    １チャンクで送れる場合と２チャンクに分けないといけない場合に
    //    対応する．
    //
    if (wrtbuf_in + buffer_len < wrtbuf_size)
    {
        command->write.buffer1 = wrtbuf + wrtbuf_in;
        command->write.buffer1_len = buffer_len;
        command->write.buffer2 = NULL;
        command->write.buffer2_len = 0;
        wrtbuf_in += buffer_len;
    }
    else
    {
        const u8*   buffer2;

        command->write.buffer1 = wrtbuf + wrtbuf_in;
        command->write.buffer1_len = wrtbuf_size - wrtbuf_in;
        command->write.buffer2 = wrtbuf;
        command->write.buffer2_len = buffer_len - command->write.buffer1_len;
        wrtbuf_in = command->write.buffer2_len;
        buffer2 = buffer + command->write.buffer1_len;
        MI_CpuCopy8(buffer2, command->write.buffer2, (u32) command->write.buffer2_len);
    }

    MI_CpuCopy8(buffer, command->write.buffer1, (u32) command->write.buffer1_len);

    // リングバッファのポインタを移動させる
    saved_send_buffer_in = send_pipe->buffer.in;
    send_pipe->buffer.in = command->write.wrtbuf_after = (u16) wrtbuf_in;

    //
    // Connect されていない UDP 通信であり、送り先が write の引数で
    // 指定されている場合は、送り相手先をコマンドパケットに指定する
    //
    // UDP か
    if (SOCL_SocketIsUDP(socket))
    {
        if (socket->local_port == 0)
        {
            socket->local_port = CPS_SocGetEport(); // bind されてないならバインドする
            socket->cps_socket.local_port = socket->local_port;
        }

        command->write.local_port = socket->local_port;

        // connect されていない UDP
        if (socket->remote_ip == 0 || remote_ip != 0)
        {
            command->write.remote_ip = remote_ip;
            command->write.remote_port = remote_port;
        }

        // connect されている UDP
        else
        {
            command->write.remote_ip = socket->remote_ip;
            command->write.remote_port = socket->remote_port;
        }
    }
    else
    {
        command->write.remote_ip = 0;               // Bind なし
    }

    result = SOCLi_ExecCommandPacketInSendPipe(send_pipe->exe_socket, command);

    if (result != SOCL_ESUCCESS)
    {
        // 失敗するのは NOBLOCK のとき
        // 返値を 0 にし、バッファを開放する
        send_pipe->buffer.in = saved_send_buffer_in;
        buffer_len = 0;
    }

    return (int)buffer_len;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_WriteCallBack

  Description:  WRITE 処理のコールバック．

  Arguments:    arg   コマンドパケットへのポインタ

  Returns:      正の値: 送信したメッセージのバイト数
                負の値: エラー
 *---------------------------------------------------------------------------*/
static int SOCLi_WriteCallBack(void* arg)
{
    SOCLiCommandWrite*  cpacket = (SOCLiCommandWrite*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    u8*     sndbuf;
    int     sndbuf_len;
    int     sndbuf_offset;
    int     copied = 0;
    int     result;

    SDK_ASSERT(send_pipe);

    if ((!SOCL_SocketIsTCP(socket)) || SOCL_SocketIsConnected(socket))  // Closing の場合は処理続行する
    {
        // remote_ip に指定があったときは、Bind 処理を行なう
        //   これは UDP 接続の場合に使用する
        if (cpacket->remote_ip)
        {
#ifdef SDK_MY_DEBUG
            OS_TPrintf("WriteCallBack-BIND:%d %d %08x\n", cpacket->local_port, cpacket->remote_port, cpacket->remote_ip);
#endif
            CPS_SocBind(cpacket->local_port, cpacket->remote_port, cpacket->remote_ip);
        }

        //
        // リングバッファからのデータの取り出し．
        // 自力で sndbuf にコピーしてから SocWrite 関数を呼び出す．
        // message queue によってシリアライズされているため、mutex なしでも sndbuf の
        // 排他がなされている
        //
#ifdef SDK_MY_DEBUG
        OS_TPrintf("WriteCallBack-WRITE 1: %08x %d\n", cpacket->buffer1, cpacket->buffer1_len);
        OS_TPrintf("                    2: %08x %d\n", cpacket->buffer2, cpacket->buffer2_len);
        OS_DumpThreadList();
        OS_TPrintf("  CPS->rcvbuf %08x%5d\n", socket->cps_socket.rcvbuf.data, socket->cps_socket.rcvbuf.size);
        OS_TPrintf("  CPS->sndbuf %08x%5d\n", socket->cps_socket.sndbuf.data, socket->cps_socket.sndbuf.size);
        OS_TPrintf("  CPS->linbuf %08x%5d\n", socket->cps_socket.linbuf.data, socket->cps_socket.linbuf.size);
        OS_TPrintf("  CPS->outbuf %08x%5d\n", socket->cps_socket.outbuf.data, socket->cps_socket.outbuf.size);
#endif

        sndbuf_offset = SOCL_SocketIsTCP(socket) ? SOCL_TCP_SNDBUF_NOCOPY_OFFSET : SOCL_UDP_SNDBUF_NOCOPY_OFFSET;
        sndbuf = socket->cps_socket.sndbuf.data + sndbuf_offset;
#if SOCL_OPTIMIZE_TCP_SEND
        if (SOCL_SocketIsTCP(socket))
        {
            /* 送信スループットを向上させるため、一度に送信を試みるデータサイズを最適化 */
            sndbuf_len = SOCLi_GetOptimumSendBufLen((int)socket->cps_socket.sndbuf.size - sndbuf_offset);
        }
        else
        {
            sndbuf_len = (int)socket->cps_socket.sndbuf.size - sndbuf_offset;
        }

#else
        sndbuf_len = (int)socket->cps_socket.sndbuf.size - sndbuf_offset;
#endif

        for (;;)
        {
            result = SOCLi_MemCpy(sndbuf, sndbuf_len, cpacket);
            if (result <= 0)
            {
                break;
            }

#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_SocWrite.sendbuf      =%d\n", sndbuf);
            OS_TPrintf("            .sendlen      =%d\n", result);
#endif
            result = (int)CPS_SocWrite(sndbuf, (u32) result);

#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_SocWrite.result       =%d\n", result);
#endif

            if (result <= 0)
            {
                // 接続が切れた場合
                if (SOCL_SocketIsTCP(socket))
                {
                    // もう一度コネクトから処理をする必要がある．
                    socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED | SOCL_STATUS_CLOSING);
                    socket->state |=  SOCL_STATUS_RESET;  // 接続切れを READ 側に知らせるためのフラグ
                    
                    // ブロック解除
                    CPS_TcpClose();
                }

                copied = SOCL_ETIMEDOUT;
                break;
            }

            copied += result;
        }
    }
    else
    {
        copied = SOCL_ETIMEDOUT;    // 途中で接続が切れている
    }

#ifdef SDK_MY_DEBUG
    OS_TPrintf("WriteCallBack-WRITE.copied=%d\n", copied);
#endif

    // リングバッファを開放し、開放待ちになっているスレッドがいたら起こす
    // このメッセージを受け取るためには、排他処理に気をつけること
    // エラーが起きてもバッファを空ける
    send_pipe->buffer.out = cpacket->wrtbuf_after;
    OS_WakeupThread(&send_pipe->buffer.waiting);

    return copied;
}

#if SOCL_OPTIMIZE_TCP_SEND

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetOptimumSendBufLen

  Description:  TCP送信時、最適なスループットを得るためにCPS_SocWriteに与える
                送信データサイズを取得する。

  Arguments:    len - 上限サイズを指定する。

  Returns:      int - 最適な送信データサイズを返す。
 *---------------------------------------------------------------------------*/
static int SOCLi_GetOptimumSendBufLen(int len)
{
    CPSSoc*     soc = CPS_CURSOC;
    int         temp = 0;
    OSIntrMode  enabled = OS_DisableInterrupts();

    if (soc != NULL)
    {
        if ((soc->remote_mss > 0) && (soc->remote_win > 0))
        {
            temp = 2 * MATH_IMin(MATH_IMin(soc->remote_mss, soc->remote_win), (int)SOCLiCPSConfig.mymss);
        }
        else
        {
            temp = 2 * (int)SOCLiCPSConfig.mymss;
        }
    }
    else
    {
        temp = 2 * (int)SOCLiCPSConfig.mymss;
    }
    (void)OS_RestoreInterrupts(enabled);

    if (temp > 0)
    {
        int count = len / temp;

        if (count > 0)
        {
            return temp * count;
        }
    }

    return len;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         SOCLi_MemCpy

  Description:  2 つのバッファにまたがっているデータの COPY 処理

  Arguments:    socket  ソケット
                cpacket バッファデータのパラメータ

  Returns:      正 or 0: 送信したデータのバイト数
 *---------------------------------------------------------------------------*/
static int SOCLi_MemCpy(u8* sndbuf_ptr, int sndbuf_len, SOCLiCommandWrite* cpacket)
{
    s32 buffer1_len = cpacket->buffer1_len;
    s32 buffer2_len = cpacket->buffer2_len;

    if (buffer1_len > sndbuf_len)
    {
        // buffer1 の一部で足りる場合
        buffer1_len = sndbuf_len;
        buffer2_len = 0;
    }
    else if (buffer2_len > sndbuf_len - buffer1_len)
    {
        // buffer1 全部と buffer2 の一部で足りる場合
        buffer2_len = sndbuf_len - buffer1_len;
    }
    else
    {
        // buffer1 全部と buffer2 全部でも足りない場合
        // そのまま buffer1_len, buffer2_len でＯＫ
    }

    if (buffer1_len > 0)
    {
        MI_CpuCopy8(cpacket->buffer1, sndbuf_ptr, (u32) buffer1_len);
        cpacket->buffer1 += buffer1_len;
        cpacket->buffer1_len -= buffer1_len;
    }

    if (buffer2_len > 0)
    {
        MI_CpuCopy8(cpacket->buffer2, sndbuf_ptr + buffer1_len, (u32) buffer2_len);
        cpacket->buffer2 += buffer2_len;
        cpacket->buffer2_len -= buffer2_len;
    }

    return buffer1_len + buffer2_len;
}
