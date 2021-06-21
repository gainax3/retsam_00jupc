/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - include
  File:     server.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_WFS_SERVER_H_
#define	NITRO_WFS_SERVER_H_


#include <nitro/wfs/format.h>
#include <nitro/mi/device.h>
#include <nitro/fs.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

/* 転送ロックハンドル総数 (最大同時接続クライアント数だけあれば充分) */
#define	WFS_LOCK_HANDLE_MAX	    15


/*---------------------------------------------------------------------------*/
/* declarations */

/* WFS_EVENT_SERVER_SEGMENT_REQUESTイベントで通知される情報構造体 */
typedef struct WFSSegmentBuffer
{
    u32     offset;
    u32     length;
    void   *buffer;
}
WFSSegmentBuffer;

/* 転送ロック範囲情報構造体 */
typedef struct WFSLockInfo
{
    WBTBlockInfoList info;
    int     ref;        /* 参照カウント */
    u32     offset;     /* ROM 先頭アドレス */
    u32     length;     /* ROM リードサイズ */
    u32     ack_seq;    /* 次に返すべきシーケンス番号 */
}
WFSLockInfo;

/* WFSサーバ情報構造体 */
typedef struct WFSServerContext
{
    void               *userdata;
    WFSEventCallback    callback;

    /* 内部スレッド情報 */
    void               *thread_work;
    void (*thread_hook)(void *thread, void *argument);

    /* ロック領域を管理するリスト */
    int                 use_bitmap;
    WFSLockInfo         list[WFS_LOCK_HANDLE_MAX];

    /* ROMファイルテーブル */
    WFSTableFormat      table[1];
    WBTBlockInfoList    table_info[1];
    MIAllocator        *allocator;

    /* クライアント状態管理メンバ */
    int                 all_bitmap;     /* 接続中 */
    int                 busy_bitmap;    /* アクセス中 */
    int                 sync_bitmap;    /* 同期中 */
    int                 ack_bitmap;     /* サーバからの応答待ち */
    WFSMessageFormat    recv_msg[16];   /* 最新のメッセージ */

    /* パケットサイズ動的変更に関するメンバ */
    BOOL                is_changing;    /* 変更予定中フラグ */
    int                 new_packet;     /* 変更後のパケットサイズ */
    int                 deny_bitmap;	/* 変更に伴う拒否応答対象 */

    /* WBT関連メンバ */
    u8                  cache_hit_buf[512];
    WBTContext          wbt[1];
    WBTCommandList      wbt_list[2];
    BOOL                msg_busy;       /* 応答メッセージ送信中 */
}
WFSServerContext;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFS_InitServer

  Description:  WFSサーバコンテキストを初期化.

  Arguments:    context          WFSServerContext構造体.
                userdata         コンテキストに関連付ける任意のユーザ定義値.
                callback         システムイベント通知コールバック.
                                 不要ならNULLを指定する.
                allocator        内部で使用するアロケータ.
                packet           親機の初期パケットサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitServer(WFSServerContext *context,
                    void *userdata, WFSEventCallback callback,
                    MIAllocator *allocator, int packet);

/*---------------------------------------------------------------------------*
  Name:         WFS_EndServer

  Description:  WFSサーバコンテキストを解放.

  Arguments:    context          WFSServerContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_EndServer(WFSServerContext *context);

/*---------------------------------------------------------------------------*
  Name:         WFS_RegisterServerTable

  Description:  デバイスからROMファイルテーブルをロードしてサーバに登録.

  Arguments:    context          WFSServerContext構造体.
                device           NTRバイナリが格納されているデバイス.
                fatbase          NTRバイナリが配置されているデバイス内オフセット.
                overlay          マージしたいオーバーレイを含むNTRバイナリが
                                 配置されているデバイス内オフセット.
                                 (複数のROMファイルテーブルをマージしないなら
                                  この値はfatbaseと同じ)

  Returns:      ROMファイルテーブルを正しくロードし登録できればTRUE.
 *---------------------------------------------------------------------------*/
BOOL WFS_RegisterServerTable(WFSServerContext *context,
                             MIDevice *device, u32 fatbase, u32 overlay);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerConnectHook

  Description:  サーバ側の接続通知.

  Arguments:    context          WFSServerContext構造体.
                peer             接続した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerConnectHook(WFSServerContext *context, const WFSPeerInfo *peer);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerDisconnectHook

  Description:  サーバ側の切断通知.

  Arguments:    context          WFSServerContext構造体.
                peer             切断した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerDisconnectHook(WFSServerContext *context, const WFSPeerInfo *peer);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerPacketSendHook

  Description:  サーバ側のパケット送信可能タイミング通知.

  Arguments:    context          WFSServerContext構造体.
                packet           送信パケット設定.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerPacketSendHook(WFSServerContext *context, WFSPacketBuffer *packet);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallServerPacketRecvHook

  Description:  サーバ側のパケット受信通知.

  Arguments:    context          WFSServerContext構造体.
                packet           送信元パケット情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallServerPacketRecvHook(WFSServerContext *context, const WFSPacketBuffer *packet);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetServerConnectedBitmap

  Description:  現在サーバに接続中のクライアント群のビットマップを取得.

  Arguments:    context          WFSServerContext構造体.

  Returns:      現在サーバに接続中のクライアント群.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WFS_GetServerConnectedBitmap(const WFSServerContext *context)
{
    return context->all_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetServerBusyBitmap

  Description:  現在サーバにアクセス中のクライアント群のビットマップを取得.

  Arguments:    context          WFSServerContext構造体.

  Returns:      現在サーバにアクセス中のクライアント群.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WFS_GetServerBusyBitmap(const WFSServerContext *context)
{
    return context->busy_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetServerSyncBitmap

  Description:  現在サーバがアクセス同期しているクライアント群のビットマップを取得.

  Arguments:    context          WFSServerContext構造体.

  Returns:      現在サーバがアクセス同期しているクライアント群.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WFS_GetServerSyncBitmap(const WFSServerContext *context)
{
    return context->sync_bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetServerPacketLength

  Description:  サーバ送信パケットサイズを取得.

  Arguments:    context          WFSServerContext構造体.

  Returns:      現在設定されているパケットサイズ.
 *---------------------------------------------------------------------------*/
int WFS_GetServerPacketLength(const WFSServerContext *context);

/*---------------------------------------------------------------------------*
  Name:         WFS_SetServerPacketLength

  Description:  サーバ送信パケットサイズを設定.

  Arguments:    context          WFSServerContext構造体.
                length           設定するパケットサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_SetServerPacketLength(WFSServerContext *context, int length);

/*---------------------------------------------------------------------------*
  Name:         WFS_SetServerSync

  Description:  サーバ側でアクセス同期を取るクライアント群を設定.
                この関数は, 同一のファイル群を同じ順序でアクセスすることが
                明らかに保証されているクライアント群へ応答を同期するにより
                WBTライブラリの特性を活かした効率的な転送を実現する.
                ただし, 同期開始のタイミングが論理的に安全でない場合
                応答がずれてデッドロックする点に注意する必要がある.

  Arguments:    context          WFSServerContext構造体.
                bitmap           同期させるクライアントのAIDビットマップ.
                                 0を指定すると同期は行われない. (デフォルト)
                                 最下位ビット 1 は常に無視される.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_SetServerSync(WFSServerContext *context, int bitmap);

/*---------------------------------------------------------------------------*
  Name:         WFS_ExecuteRomServerThread

  Description:  指定のROMファイルを配信するようWFSライブラリへ登録し,
                内部でROMアクセス用のスレッドを自動的に起動する.
                このスレッドはWFS_EndServer関数の内部で自動的に破棄される.

  Arguments:    context          WFSServerContext構造体.
                file             配信するファイルシステムを持つSRLファイル.
                                 クローンブートならNULLを指定する.
                sharedFS         ファイルシステムを親子で共有させるならTRUE.
                                 その場合, 親機自体の持つファイルシステムに
                                 fileの持つオーバーレイだけを抽出して追加した
                                 混合ファイルシステムを配信する.
                                 fileにNULLを指定した場合はクローンブートなので
                                 この引数は無視される. (常にTRUEと解釈される)


  Returns:      ROMファイルの登録とスレッドの生成に成功すればTRUE.
 *---------------------------------------------------------------------------*/
BOOL WFS_ExecuteRomServerThread(WFSServerContext *context, FSFile *file, BOOL sharedFS);


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_WFS_SERVER_H_ */
/*---------------------------------------------------------------------------*
  $Log: server.h,v $
  Revision 1.5  2007/08/22 05:22:32  yosizaki
  fix dependency.

  Revision 1.4  2007/06/14 13:14:10  yosizaki
  add WFS_ExecuteRomServerThread.

  Revision 1.3  2007/06/11 09:03:37  yosizaki
  add WFS_GetServerPacketLength().

  Revision 1.2  2007/04/17 00:00:52  yosizaki
  rename some structures.

  Revision 1.1  2007/04/13 04:14:18  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

