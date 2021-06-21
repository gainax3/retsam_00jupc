/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - include
  File:     context.h

  Copyright 2006,2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	NITRO_WBT_CONTEXT_H_
#define	NITRO_WBT_CONTEXT_H_


/*===========================================================================*/

#include <nitro/platform.h>
#include <nitro/misc.h>
#include <nitro/math/math.h>
#include <nitro/wbt.h>


#ifdef	__cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* declarations */


/*****************************************************************************
 * 電波に乗る通信フォーマット.
 *****************************************************************************/

/* REQ_SYNC コマンド引数フォーマット構造体 */
typedef struct WBTPacketRequestSyncFormat
{
    PLATFORM_LE16 peer_packet;
    PLATFORM_LE16 own_packet;
}
PLATFORM_STRUCT_PADDING_FOOTER WBTPacketRequestSyncFormat;

/* RES_SYNC コマンド引数フォーマット構造体 */
typedef struct WBTPacketResponseSyncFormat
{
    PLATFORM_LE16 block_total;
    PLATFORM_LE16 peer_packet;
    PLATFORM_LE16 own_packet;
}
PLATFORM_STRUCT_PADDING_FOOTER WBTPacketResponseSyncFormat;

/* REQ_USERDATA コマンド引数フォーマット構造体 */
typedef struct WBTPacketRequestUserDataFormat
{
    PLATFORM_LE8 length;
    PLATFORM_LE8 buffer[WBT_SIZE_USER_DATA];
}
WBTPacketRequestUserDataFormat;

/* REQ_GETBLOCK_DONE コマンド引数フォーマット構造体 */
typedef struct WBTPacketRequestGetBlockDoneFormat
{
    PLATFORM_LE32 id;
}
WBTPacketRequestGetBlockDoneFormat;

/* RES_GETBLOCK_DONE コマンド引数フォーマット構造体 */
typedef struct WBTPacketResponseGetBlockDoneFormat
{
    PLATFORM_LE32 id;
}
WBTPacketResponseGetBlockDoneFormat;

/* REQ_GETBLOCK コマンド引数フォーマット構造体 */
typedef struct WBTPacketRequestGetBlockFormat
{
    PLATFORM_LE32 id;
    PLATFORM_LE32 index;
}
WBTPacketRequestGetBlockFormat;

/* RES_GETBLOCK コマンド引数フォーマット構造体 */
typedef struct WBTPacketResponseGetBlockFormat
{
    PLATFORM_LE32 id;
    PLATFORM_LE32 index;
}
WBTPacketResponseGetBlockFormat;

/* パケットヘッダフォーマット構造体 */
typedef struct WBTPacketHeaderFormat
{
    PLATFORM_LE8 command;
    PLATFORM_LE16 bitmap;
    PLATFORM_LE8 counter;
}
PLATFORM_STRUCT_PADDING_FOOTER WBTPacketHeaderFormat;

/* パケットフォーマット構造体 */
typedef struct WBTPacketFormat
{
    /* パケットヘッダ */
    WBTPacketHeaderFormat header;
    /* コマンドによっては後続の引数が存在する */
    union
    {
        u8      argument[10];
        WBTPacketRequestSyncFormat req_sync;
        WBTPacketResponseSyncFormat res_sync;
        WBTPacketRequestUserDataFormat req_userdata;
        WBTPacketRequestGetBlockDoneFormat req_getblock_done;
        WBTPacketResponseGetBlockDoneFormat res_getblock_done;
        WBTPacketRequestGetBlockFormat req_getblock;
        WBTPacketResponseGetBlockFormat res_getblock;
        u8      for_compiler[10];
    } PLATFORM_STRUCT_PADDING_FOOTER /* unnamed */ ;
    /* コマンドによってはさらに可変長の後続引数が存在する */
}
PLATFORM_STRUCT_PADDING_FOOTER WBTPacketFormat;

PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketHeaderFormat) == 4);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketRequestSyncFormat) == 4);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketResponseSyncFormat) == 6);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketRequestUserDataFormat) == 10);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketRequestGetBlockDoneFormat) == 4);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketResponseGetBlockDoneFormat) == 4);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketRequestGetBlockFormat) == 8);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketResponseGetBlockFormat) == 8);
PLATFORM_COMPILER_ASSERT(sizeof(WBTPacketFormat) == 14);


/*****************************************************************************
 * ローカル構造体.
 *****************************************************************************/

struct WBTContext;
struct WBTCommandList;

/* コマンドコールバックプロトタイプ */
typedef void (*WBTEventCallback)(void*, WBTCommand*);

/* コマンドリスト構造体 */
typedef struct WBTCommandList
{
    struct WBTCommandList  *next;
    WBTCommand              command;
    WBTEventCallback        callback;
}
WBTCommandList;


/* 親機が各AIDごとに保持しておく最新の受信状況 */
typedef struct WBTRecvToken
{
    u8      token_command;
    u8      token_peer_cmd_counter;
    u8      last_peer_cmd_counter;

    u8      dummy[1];

    /*
     * これらは WBT_CMD_REQ_GET_BLOCK* 系でしか代入されていない.
     * 応答時に参照する.
     */
    u32     token_block_id;
    s32     token_block_seq_no;
}
WBTRecvToken;

/* 親機が各AIDごとに保持しておく各種ワーク変数 */
typedef struct WBTPacketBitmap
{
    s32     length;
    void   *buffer;
    s32     count;
    s32     total;
    u32    *bitmap;
    s32     current;
}
WBTPacketBitmap;

/* wbt_data.c で使用するコマンド管理構造体 */
typedef struct WBTContext
{
    /* 処理中のコマンドリストと空きのコマンドプール */
    WBTCommandList *command;
    WBTCommandList *command_pool;

    /* 任意のユーザ定義値 */
    void               *userdata;
    WBTEventCallback    callback;

    /*
     * システムコールバック用バッファ.
     * 一時的にしか使用しないが, 156 BYTE もあるのでコンテキストに保持.
     */
    WBTCommand system_cmd;

    /* 各 AID の状態 */
    struct
    {
        WBTRecvToken recv_token;
        WBTPacketBitmap pkt_bmp;
    }
    peer_param[16];

    /* 自身の通信状態 */
    int     my_aid;                    /* 自局のAID */
    s16     peer_data_packet_size;     /* 相手局のパケットデータ部のサイズ */
    s16     my_data_packet_size;       /* 自局のパケットデータ部のサイズ */
    WBTBlockInfoList *list;            /* 登録済みデータブロックリスト */
    u8      my_command_counter;        /* ユーザーコマンド発行カウンタ */
    u8      padding[3];
    int     last_target_aid;

    /* ブロック番号の送信履歴 */
    u32     last_block_id;
    s32     last_seq_no_1;
    s32     last_seq_no_2;

    /* 現在要求を受信しているビットマップ */
    int     req_bitmap;

    /* GetBlockInfo 用のビットマップ */
    u32     binfo_bitmap[16][MATH_ROUNDUP(sizeof(WBTBlockInfo), sizeof(u32)) / sizeof(u32)];
}
WBTContext;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WBT_InitContext

  Description:  WBT構造体を初期化.

  Arguments:    work              WBTContext構造体.
                userdata          任意のユーザ定義値.
                callback          システムコールバック.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_InitContext(WBTContext *work, void *userdata, WBTEventCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_StartParent

  Description:  WBTを親機として開始.

  Arguments:    work              WBTContext構造体.
                own               自身のMP送信パケットサイズ.
                peer              相手のMP送信パケットサイズ.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void    WBT_StartParent(WBTContext *work, int own, int peer)
{
    SDK_MIN_ASSERT(own, WBT_PACKET_SIZE_MIN);
    SDK_MIN_ASSERT(peer, WBT_PACKET_SIZE_MIN);
    work->my_aid = 0;
    work->my_data_packet_size = (s16)(own - WBT_PACKET_SIZE_MIN);
    work->peer_data_packet_size = (s16)(peer - WBT_PACKET_SIZE_MIN);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_StartChild

  Description:  WBTを子機として開始.

  Arguments:    work              WBTContext構造体.
                aid               自身のAID.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void    WBT_StartChild(WBTContext *work, int aid)
{
    work->my_data_packet_size = 0;
    work->peer_data_packet_size = 0;
    work->my_aid = aid;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_ResetContext

  Description:  WBTを再初期化.

  Arguments:    work              WBTContext構造体.
                callback          システムコールバック.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_ResetContext(WBTContext *work, WBTEventCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_CallPacketSendHook

  Description:  送信パケットデータを生成するためのフック関数.

  Arguments:    work              WBTContext構造体.
                buffer            データ格納バッファ.
                length            バッファサイズ.

  Returns:      生成されたパケットサイズ.
 *---------------------------------------------------------------------------*/
int     WBT_CallPacketSendHook(WBTContext *work, void *buffer, int length, BOOL is_parent);

/*---------------------------------------------------------------------------*
  Name:         WBT_CallPacketRecvHook

  Description:  受信パケットデータを解析.

  Arguments:    work              WBTContext構造体.
                aid               データ送信元のAID
                buffer            受信データバッファ.
                length            受信データ長.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_CallPacketRecvHook(WBTContext *work, int aid, const void *buffer, int length);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetUserData

  Description:  コンテキストに関連付けられたユーザ定義値を取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在処理中のコマンドかまたは NULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void   *WBT_GetUserData(const WBTContext *work)
{
    return work->userdata;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetAid

  Description:  設定されたAID値を取得.

  Arguments:    work              WBTContext構造体.

  Returns:      設定されたAID値.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBT_GetAid(const WBTContext *work)
{
    return work->my_aid;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetOwnPacketLength

  Description:  現在の自身の送信パケットサイズを取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在の自身の送信パケットサイズ.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBT_GetOwnPacketLength(const WBTContext *work)
{
    return work->my_data_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetPeerPacketLength

  Description:  現在の相手の送信パケットサイズを取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在の相手の送信パケットサイズ.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBT_GetPeerPacketLength(const WBTContext *work)
{
    return work->peer_data_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetParentPacketLength

  Description:  現在の親機送信パケットサイズを取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在の親機送信パケットサイズ.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
int WBT_GetParentPacketLength(const WBTContext *work)
{
    return (work->my_aid == 0) ? work->my_data_packet_size : work->peer_data_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetRegisteredCount

  Description:  登録済みのデータブロック総数を取得.

  Arguments:    work              WBTContext構造体.

  Returns:      登録済みのデータブロック総数.
 *---------------------------------------------------------------------------*/
int     WBT_GetRegisteredCount(const WBTContext * work);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetCurrentCommandList

  Description:  現在処理中のコマンドを取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在処理中のコマンドかまたは NULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
WBTCommandList *WBT_GetCurrentCommandList(const WBTContext *work)
{
    return work->command;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetCurrentCommand

  Description:  現在処理中のコマンドを取得.

  Arguments:    work              WBTContext構造体.

  Returns:      現在処理中のコマンドかまたは NULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
WBTCommand *WBT_GetCurrentCommand(const WBTContext *work)
{
    WBTCommandList *list = work->command;
    return list ? &list->command : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetBitmapLength

  Description:  ブロック転送の制御に必要なビットマップバッファサイズを取得.

  Arguments:    work              WBTContext構造体.
                length            転送するブロックの最大サイズ.

  Returns:      必要なビットマップバッファのサイズ.
 *---------------------------------------------------------------------------*/
int WBT_GetBitmapLength(const WBTContext *work, int length);

/*---------------------------------------------------------------------------*
  Name:         WBT_AddCommandPool

  Description:  コマンドプールに新規のリストを追加.

  Arguments:    work              WBTContext構造体.
                list              コマンドリスト構造体の配列.
                count             配列の要素数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void    WBT_AddCommandPool(WBTContext *work, WBTCommandList *list, int count)
{
    while (--count >= 0)
    {
        list->next = work->command_pool;
        work->command_pool = list++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WBT_AllocCommandList

  Description:  コマンドプールから1個のリストを新規確保.

  Arguments:    work              WBTContext構造体.

  Returns:      確保された新規のコマンドリストかまたは NULL.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
WBTCommandList *WBT_AllocCommandList(WBTContext *work)
{
    WBTCommandList *list = work->command_pool;
    if (list)
    {
        work->command_pool = list->next;
        list->next = NULL;
    }
    return list;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_SetPacketLength

  Description:  パケットサイズを変更.
                親機のみ使用可能.

  Arguments:    work              WBTContext構造体.
                own               自身のMP送信パケットサイズ.
                peer              相手のMP送信パケットサイズ.

  Returns:      設定に成功すれば TRUE.
 *---------------------------------------------------------------------------*/
BOOL    WBT_SetPacketLength(WBTContext *work, int own, int peer);

/*---------------------------------------------------------------------------*
  Name:         WBT_CreateCommandSYNC

  Description:  "SYNC" コマンド情報を生成する.

  Arguments:    work              WBTContext構造体.
                list              コマンド情報を格納するために用意されたリスト.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void WBT_CreateCommandSYNC(WBTContext *work, WBTCommandList *list)
{
    (void)work;
    list->command.command = WBT_CMD_REQ_SYNC;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CreateCommandINFO

  Description:  "INFO" コマンド情報をリストに設定する.

  Arguments:    work              WBTContext構造体.
                list              コマンド情報を格納するために用意されたリスト.
                index             取得するブロック情報を示す
                                  登録リスト先頭からのインデックス.
                buffer_table      取得したブロック情報を格納する
                                  WBTBlockInfoTable ポインタのテーブル.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void WBT_CreateCommandINFO(WBTContext *work, WBTCommandList *list,
                           int index, const WBTBlockInfoTable *buffer_table)
{
    WBTGetBlockCallback *arg = &list->command.get;
    arg->block_id = (u32)index;
    arg->recv_data_size = sizeof(WBTBlockInfo);
    {
        int     i;
        for (i = 0; i < 16; ++i)
        {
            arg->pkt_bmp_table.packet_bitmap[i] = work->binfo_bitmap[i];
            arg->recv_buf_table.recv_buf[i] = (u8 *)buffer_table->block_info[i];
        }
    }
    list->command.command = WBT_CMD_REQ_GET_BLOCKINFO;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CreateCommandGET

  Description:  "GET" コマンド情報をリストに設定する.

  Arguments:    work              WBTContext構造体.
                list              コマンド情報を格納するために用意されたリスト.
                id                取得するブロックのID.
                length            取得するブロックデータ長.
                buffer_table      取得したブロックデータを格納する
                                  WBTRecvBufTable ポインタのテーブル.
                bitmap_table      内部でのブロック転送制御のために必要な
                                  受信状況管理バッファのテーブル.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void WBT_CreateCommandGET(WBTContext *work, WBTCommandList * list,
                          u32 id, u32 length,
                          const WBTRecvBufTable *buffer_table,
                          WBTPacketBitmapTable *bitmap_table)
{
    WBTGetBlockCallback *arg = &list->command.get;
    (void)work;
    arg->block_id = id;
    arg->recv_data_size = length;
    arg->recv_buf_table = *buffer_table;
    arg->pkt_bmp_table = *bitmap_table;
    list->command.command = WBT_CMD_REQ_GET_BLOCK;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CreateCommandMSG

  Description:  "MSG" コマンド情報をリストに設定する.

  Arguments:    work              WBTContext構造体.
                list              コマンド情報を格納するために用意されたリスト.
                buffer            送信データが格納されたバッファ.
                                  バッファの内容はこの関数内でのみ参照される.
                length            送信データ長.
                                  WBT_SIZE_USER_DATA 以下である必要がある.

  Returns:      None.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
void    WBT_CreateCommandMSG(WBTContext *work, WBTCommandList *list,
                             const void *buffer, u32 length)
{
    WBTRecvUserDataCallback *arg = &list->command.user_data;
    (void)work;
    SDK_MINMAX_ASSERT(length, 0, WBT_SIZE_USER_DATA);
    arg->size = (u8)length;
    MI_CpuCopy8(buffer, arg->data, length);
    list->command.command = WBT_CMD_REQ_USER_DATA;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommand

  Description:  コマンドを発行してコマンドキューに追加.

  Arguments:    work              WBTContext構造体.
                list              コマンド情報が格納された構造体.
                                  コマンド完了までライブラリ内部で管理される.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_PostCommand(WBTContext *work, WBTCommandList *list, u16 bitmap,
                        WBTEventCallback callback);

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommandSYNC

  Description:  "SYNC" コマンド情報を発行する.

  Arguments:    context           WBTContext構造体.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.

  Returns:      空きのコマンドリストが存在してコマンド発行に成功したらTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL    WBT_PostCommandSYNC(WBTContext *context, int bitmap, WBTEventCallback callback)
{
    WBTCommandList *list = WBT_AllocCommandList(context);
    if (list)
    {
        WBT_CreateCommandSYNC(context, list);
        WBT_PostCommand(context, list, (u16)bitmap, callback);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommandINFO

  Description:  "INFO" コマンド情報を発行する.

  Arguments:    context           WBTContext構造体.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.
                index             取得するブロック情報を示す
                                  登録リスト先頭からのインデックス.
                buffer_table      取得したブロック情報を格納する
                                  WBTBlockInfoTable ポインタのテーブル.

  Returns:      空きのコマンドリストが存在してコマンド発行に成功したらTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL    WBT_PostCommandINFO(WBTContext *context, int bitmap, WBTEventCallback callback,
                            int index, const WBTBlockInfoTable *buffer_table)
{
    WBTCommandList *list = WBT_AllocCommandList(context);
    if (list)
    {
        WBT_CreateCommandINFO(context, list, index, buffer_table);
        WBT_PostCommand(context, list, (u16)bitmap, callback);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommandGET

  Description:  "GET" コマンド情報を発行する.

  Arguments:    context           WBTContext構造体.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.
                id                取得するブロックのID.
                length            取得するブロックデータ長.
                buffer_table      取得したブロックデータを格納する
                                  WBTRecvBufTable ポインタのテーブル.
                bitmap_table      内部でのブロック転送制御のために必要な
                                  受信状況管理バッファのテーブル.

  Returns:      空きのコマンドリストが存在してコマンド発行に成功したらTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL    WBT_PostCommandGET(WBTContext *context, int bitmap, WBTEventCallback callback,
                           u32 id, u32 length, const WBTRecvBufTable *buffer_table,
                           WBTPacketBitmapTable *bitmap_table)
{
    WBTCommandList *list = WBT_AllocCommandList(context);
    if (list)
    {
        WBT_CreateCommandGET(context, list, id, length, buffer_table, bitmap_table);
        WBT_PostCommand(context, list, (u16)bitmap, callback);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_PostCommandMSG

  Description:  "MSG" コマンド情報を発行する.

  Arguments:    context           WBTContext構造体.
                bitmap            コマンド発行対象のAIDビットマップ.
                callback          コマンド完了コールバック. 不要なら NULL.
                buffer            送信データが格納されたバッファ.
                                  バッファの内容はこの関数内でのみ参照される.
                length            送信データ長.
                                  WBT_SIZE_USER_DATA 以下である必要がある.

  Returns:      空きのコマンドリストが存在してコマンド発行に成功したらTRUE.
 *---------------------------------------------------------------------------*/
PLATFORM_ATTRIBUTE_INLINE
BOOL    WBT_PostCommandMSG(WBTContext *context, int bitmap, WBTEventCallback callback,
                           const void *buffer, u32 length)
{
    WBTCommandList *list = WBT_AllocCommandList(context);
    if (list)
    {
        WBT_CreateCommandMSG(context, list, buffer, length);
        WBT_PostCommand(context, list, (u16)bitmap, callback);
    }
    return (list != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CancelCommand

  Description:  現在処理中のコマンドを中止.

  Arguments:    work              WBTContext構造体.
                bitmap            コマンドを中止する相手.

  Returns:      実際に中止された相手を示すビットマップ.
 *---------------------------------------------------------------------------*/
int     WBT_CancelCommand(WBTContext * work, int bitmap);

/*---------------------------------------------------------------------------*
  Name:         WBT_GetDownloadProgress

  Description:  ブロック転送の進捗状況を取得.

  Arguments:    work              WBTContext構造体.
                id                受信ブロックID.
                aid               受信先AID.
                current           受信済みパケット数の格納先.
                total             パケット総数の格納先.

  Returns:      None.
                ブロック転送状態になければ current, total とも 0 を返す.
 *---------------------------------------------------------------------------*/
void    WBT_GetDownloadProgress(const WBTContext * work, u32 id, int aid, int *current, int *total);

/*---------------------------------------------------------------------------*
  Name:         WBT_RegisterBlockInfo

  Description:  新規にデータブロックを登録する.

  Arguments:    work              WBTContext構造体.
                list              登録に使用するリスト構造体.
                                  Unregister で解放するまでライブラリが使用する.
                id                データブロックに関連付ける一意なID.
                userinfo          データブロックに関連付けるユーザ定義情報.
                                  このポインタが指す先はこの関数内でのみ参照する.
                                  不要な場合は NULL を指定することもできる.
                buffer            ブロックデータを格納したバッファ.
                                  NULL を指定すると, 必要に応じてライブラリから
                                  WBT_CMD_PREPARE_SEND_DATA コールバックが通知される.
                length            ブロックデータのサイズ.
                                  buffer に NULL を指定する場合であっても
                                  この値は正しく指定しておく必要がある.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WBT_RegisterBlockInfo(WBTContext * work, WBTBlockInfoList *list, u32 id,
                              const void *userinfo, const void *buffer, int length);

/*---------------------------------------------------------------------------*
  Name:         WBT_UnregisterBlockInfo

  Description:  登録済みのデータブロックを解放する.

  Arguments:    work              WBTContext構造体.
                id                解放するデータブロックに関連付けられた一意なID.

  Returns:      解放したリスト構造体または NULL.
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBT_UnregisterBlockInfo(WBTContext * work, u32 id);


/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif /* NITRO_WBT_CONTEXT_H_ */

/*---------------------------------------------------------------------------*

  $Log: context.h,v $
  Revision 1.3  2007/10/04 03:52:18  yosizaki
  fix about extern "C"

  Revision 1.2  2007/08/22 05:22:32  yosizaki
  fix dependency.

  Revision 1.1  2007/04/10 08:20:14  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
