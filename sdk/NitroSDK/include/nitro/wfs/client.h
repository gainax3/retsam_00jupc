/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - include
  File:     client.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_WFS_CLIENT_H_
#define	NITRO_WFS_CLIENT_H_


#include <nitro/wfs/format.h>
#include <nitro/fs.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

/* 子機が受信しうる最大のファイルサイズ (4Mバイトとしておく ) */
#define	WFS_FILE_SIZE_MAX	(4 * 1024 * 1024)


/*---------------------------------------------------------------------------*/
/* declarations */

/* ROM読み込みリクエスト完了コールバック */
struct WFSClientContext;
typedef void (*WFSRequestClientReadDoneCallback)(struct WFSClientContext *context, BOOL succeeded, void *arg);

/* WBTクライアントコンテキスト構造体 */
typedef struct WFSClientContext
{
    /* 基本設定 */
    void                   *userdata;
    WFSEventCallback        callback;
    MIAllocator            *allocator;
    u32                     fat_ready:1;
    u32                     flags:31;
    /* WBT管理 */
    WBTContext              wbt[1];
    WBTCommandList          wbt_list[2];
    WBTRecvBufTable         recv_buf_table;
    WBTPacketBitmapTable    recv_buf_packet_bmp_table;
    WBTBlockInfoTable       block_info_table;
    WBTBlockInfo            block_info[16];
    /* 受信ビットセット管理 */
    u32                    *recv_pkt_bmp_buf;
    u32                     max_file_size;
    /* FAT管理 */
    WFSTableFormat          table[1];
    /* リクエスト管理 */
    u32                     block_id;
    CARDRomRegion           request_region;
    void                   *request_buffer;
    WFSRequestClientReadDoneCallback request_callback;
    void                   *request_argument;
    u8                      padding[12];
}
WFSClientContext;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFS_InitClient

  Description:  WFSクライアントコンテキストを初期化.

  Arguments:    context          WFSClientContext構造体.
                userdata         コンテキストに関連付ける任意のユーザ定義値.
                callback         システムイベント通知コールバック.
                                 不要ならNULLを指定する.
                allocator        内部で使用するアロケータ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_InitClient(WFSClientContext *context,
                    void *userdata, WFSEventCallback callback,
                    MIAllocator *allocator);

/*---------------------------------------------------------------------------*
  Name:         WFS_StartClient

  Description:  WFSクライアントコンテキストの通信を開始.

  Arguments:    context          WFSClientContext構造体.
                peer             自身の接続情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_StartClient(WFSClientContext *context, const WFSPeerInfo *peer);

/*---------------------------------------------------------------------------*
  Name:         WFS_EndClient

  Description:  WFSクライアントコンテキストを解放.

  Arguments:    context          WFSClientContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_EndClient(WFSClientContext *context);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientConnectHook

  Description:  クライアント側の接続通知.

  Arguments:    context          WFSClientContext構造体.
                peer             接続した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallClientConnectHook(WFSClientContext *context, const WFSPeerInfo *peer);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientDisconnectHook

  Description:  クライアント側の切断通知.

  Arguments:    context          WFSClientContext構造体.
                peer             切断した通信先の情報.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_CallClientDisconnectHook(WFSClientContext *context, const WFSPeerInfo *peer);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientPacketSendHook

  Description:  クライアント側のパケット送信可能タイミング通知.

  Arguments:    context          WFSClientContext構造体.
                packet           送信パケット設定.

  Returns:      実際のパケットサイズ.
 *---------------------------------------------------------------------------*/
void WFS_CallClientPacketSendHook(WFSClientContext *context, WFSPacketBuffer *packet);

/*---------------------------------------------------------------------------*
  Name:         WFS_CallClientPacketRecvHook

  Description:  クライアント側のパケット受信タイミング通知.

  Arguments:    context          WFSClientContext構造体.
                packet           送信元パケット情報.

  Returns:      実際のパケットサイズ.
 *---------------------------------------------------------------------------*/
void WFS_CallClientPacketRecvHook(WFSClientContext *context, const WFSPacketBuffer *packet);

/*---------------------------------------------------------------------------*
  Name:         WFS_IsClientReady

  Description:  クライアント側の準備が完了しているか判定.
                WFS_EVENT_CLIENT_READY通知以降はTRUEを返す.

  Arguments:    context          WFSClientContext構造体.

  Returns:      クライアント側の準備が完了していればTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL WFS_IsClientReady(const WFSClientContext *context)
{
    return context->fat_ready ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_GetTableFormat

  Description:  サーバから受信したROMファイルテーブルを取得.
                WFS_EVENT_CLIENT_READY通知以降でのみ有効な値を得る.

  Arguments:    context          WFSClientContext構造体.

  Returns:      ROMファイルテーブルまたはNULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
const WFSTableFormat *WFS_GetTableFormat(const WFSClientContext *context)
{
    return WFS_IsClientReady(context) ? context->table : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_RequestClientRead

  Description:  サーバへROMイメージ読み込み要求を開始.

  Arguments:    context          WFSClientContext構造体.
                buffer           読み込みデータの格納先メモリ.
                offset           デバイスの読み込み開始位置.
                length           読み込みサイズ.
                callback         読み込み完了コールバック.
                                 不要ならNULL.
                arg              読み込み完了コールバックに与える引数.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_RequestClientRead(WFSClientContext *context, void *buffer, u32 offset,
                           u32 length, WFSRequestClientReadDoneCallback callback,
                           void *arg);

/*---------------------------------------------------------------------------*
  Name:         WFS_GetClientReadProgress

  Description:  ROMイメージ読み込み要求の進捗状況を取得.

  Arguments:    context          WFSClientContext構造体.
                current          受信済みパケット数を取得する変数.
                total            受信予定のパケット総数を取得する変数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_GetClientReadProgress(WFSClientContext *context, int *current, int *total);

/*---------------------------------------------------------------------------*
  Name:         WFS_ReplaceRomArchive

  Description:  WFSのROMファイルテーブルを"rom"のアーカイブにマウント.

  Arguments:    context          WFSClientContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WFS_ReplaceRomArchive(WFSClientContext *context);


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_WFS_CLIENT_H_ */
/*---------------------------------------------------------------------------*
  $Log: client.h,v $
  Revision 1.3  2007/08/22 05:22:32  yosizaki
  fix dependency.

  Revision 1.2  2007/06/11 09:02:30  yosizaki
  add some functions.

  Revision 1.1  2007/04/13 04:14:18  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
