/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_consts.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_const.c,v $
  Revision 1.9  2006/03/10 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.8  2005/08/25 08:27:38  yasu
  UDP 受信におけるバッファリング処理追加

  Revision 1.7  2005/08/22 04:33:02  yasu
  UDP の受信バッファサイズを固定にした

  Revision 1.6  2005/08/19 05:05:23  yasu
  mtu/rwin 対応

  Revision 1.5  2005/08/13 08:01:23  yasu
  consumed_min の追加

  Revision 1.4  2005/07/30 22:30:14  yasu
  デモが動くように修正

  Revision 1.3  2005/07/30 15:28:29  yasu
  UDP 送信受信の2重化

  Revision 1.2  2005/07/22 12:44:56  yasu
  非同期処理仮実装

  Revision 1.1  2005/07/18 13:16:28  yasu
  送信受信スレッドを作成する

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

/*---------------------------------------------------------------------------*
  Name:         SOCLSocketParamTCP/SOCLSocketParamUDP

  Description:  ソケットにおけるデフォルトのメモリ利用パラメータ．
                弱いシンボルとして定義してあるのでアプリケーション側で
                メモリ利用に関する細かな設定のためにオーバーライド可能．
                TCP 版と UDP 版がある
 *---------------------------------------------------------------------------*/

// TCP
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamTCP =
{
    SOCL_FLAGMODE_TCP,                      // TCP
    SOCL_FLAGBLOCK_BLOCK,                   // BLOCK
    {                                       // cps
        0,                                  //SOCL_TCP_SOCKET_CPS_RCVBUF_SIZE,		// rcvbuf_size

        //****Set in SOCL_Startup()
        0,                                  //SOCL_TCP_SOCKET_CPS_RCVBUF_CONSUME_MIN,	// rcvbuf_consume_min

        //****Set in SOCL_Startup()
        SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE,    // sndbuf_size
        SOCL_TCP_SOCKET_CPS_LINBUF_SIZE,    // linbuf_size
        SOCL_TCP_SOCKET_CPS_OUTBUF_SIZE,    // outbuf_size
        SOCL_TCP_SOCKET_SEND_WRTBUF_SIZE,   // wrtbuf_size
        0,  // udpbuf_size
    },

    {       // recv_thread
        SOCL_TCP_SOCKET_RECV_THREAD_STACK_SIZE, // stack_size
        SOCL_TCP_SOCKET_RECV_THREAD_PRIORITY,   // priority
        SOCL_TCP_SOCKET_RECV_THREAD_QUEUE_MAX,  // queue_max
    },

    {   // send_thread
        SOCL_TCP_SOCKET_SEND_THREAD_STACK_SIZE, // stack_size
        SOCL_TCP_SOCKET_SEND_THREAD_PRIORITY,   // priority
        SOCL_TCP_SOCKET_SEND_THREAD_QUEUE_MAX,  // queue_max
    },
};

//----------------------------------------------------------------------------
// UDP
//
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamUDP =
{
    SOCL_FLAGMODE_UDP,                      // UDP
    SOCL_FLAGBLOCK_BLOCK,                   // BLOCK
    {                                       // cps
        SOCL_UDP_SOCKET_CPS_RCVBUF_SIZE,    // rcvbuf_size
        1,  // rcvbuf_consume_min
        0,  // sndbuf_size
        SOCL_UDP_SOCKET_CPS_LINBUF_SIZE,    // linbuf_size
        0,  // outbuf_size
        0,  // wrtbuf_size
        SOCL_UDP_SOCKET_RECV_UDPBUF_SIZE,   // udpbuf_size
    },

    {   // recv_thread
        SOCL_UDP_SOCKET_RECV_THREAD_STACK_SIZE, // stack_size
        SOCL_UDP_SOCKET_RECV_THREAD_PRIORITY,   // priority
        SOCL_UDP_SOCKET_RECV_THREAD_QUEUE_MAX,  // queue_max
    },

    {                       // send_thread
        0,                  // stack_size
        0,                  // priority
        0,                  // queue_max
    },
};

//----------------------------------------------------------------------------
// UDP for Send
//
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamUDPSend =
{
    SOCL_FLAGMODE_UDPSEND,  // UDPSEND
    SOCL_FLAGBLOCK_NOBLOCK, // BLOCK
    {                       // cps
        0,                  // rcvbuf_size
        0,                  // rcvbuf_consume_min
        SOCL_UDP_SOCKET_CPS_SNDBUF_SIZE,    // sndbuf_size
        0,  // linbuf_size
        SOCL_UDP_SOCKET_CPS_OUTBUF_SIZE,    // outbuf_size
        SOCL_UDP_SOCKET_SEND_WRTBUF_SIZE,   // wrtbuf_size
        0,  // udpbuf_size
    },

    {       // recv_thread
        0,  // stack_size
        0,  // priority
        0,  // queue_max
    },

    {       // send_thread
        SOCL_UDP_SOCKET_SEND_THREAD_STACK_SIZE, // stack_size
        SOCL_UDP_SOCKET_SEND_THREAD_PRIORITY,   // priority
        SOCL_UDP_SOCKET_SEND_THREAD_QUEUE_MAX,  // queue_max
    },
};
