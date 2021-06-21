/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_private.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_private.h,v $
  Revision 1.38  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.37  2005/11/07 01:22:05  okubata_ryoma
  SDK_STATIC_ASSERTからSDK_COMPILER_ASSERTに変更

  Revision 1.36  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.35  2005/02/21 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.34  2005/02/20 23:53:07  takano_makoto
  invalidate PRINT_DEBUG

  Revision 1.33  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.32  2005/01/27 07:05:32  takano_makoto
  fix copyright header.

  Revision 1.31  2005/01/27 07:03:56  takano_makoto
  MB_CommPWork構造体にuseWvrFlagメンバを追加

  Revision 1.30  2004/11/22 12:52:13  takano_makoto
  mb_block mb_gameinfo内の構造体に分割・隠蔽できる情報を整理

  Revision 1.29  2004/11/11 11:52:31  yosizaki
  add structures for task-thread.

  Revision 1.28  2004/11/10 13:14:20  yosizaki
  add fileinfo members for cache-system.

  Revision 1.27  2004/10/28 11:02:11  yada
  just fix comment

  Revision 1.26  2004/10/18 11:44:30  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.25  2004/10/15 08:50:34  yosizaki
  add start_mp_busy.

  Revision 1.24  2004/10/05 10:05:49  sato_masaki
  BOOT_REQを受けた時のMB終了処理を識別する処理の追加。

  Revision 1.23  2004/09/25 09:36:52  sato_masaki
  MB_CommGetChildUser()にて、子機情報をバッファにコピーするように変更。

  Revision 1.22  2004/09/25 04:40:34  sato_masaki
  NitroSDK_IPL_branch 1.14.2.5　とマージ

  Revision 1.21  2004/09/24 07:30:57  sato_masaki
  small fix

  Revision 1.20  2004/09/24 01:42:39  sato_masaki
  req_data_bufのサイズに関する不具合修正。

  Revision 1.19  2004/09/22 09:42:27  sato_masaki
  MB_MAX_BLOCKの値変更。

  Revision 1.18  2004/09/20 12:52:54  sato_masaki
  MB_CommPWork, MB_CommCWorkの並びを変更。

  Revision 1.17  2004/09/17 11:38:10  sato_masaki
  add 'isMbInitialized'　member to MB_CommCommonWork structure

  Revision 1.16  2004/09/17 04:03:05  miya
  不要な部分の削除

  Revision 1.15  2004/09/16 08:43:29  miya
  ファイル更新用メンバ追加

  Revision 1.14  2004/09/15 06:23:32  sato_masaki
  MB_CommCommonWorkにおいて、MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN の定義に依存していた値を格納する変数を追加。
    (block_size_max, req_data_piece_size, req_data_piece_num)

  Revision 1.13  2004/09/14 13:19:50  sato_masaki
  リクエストデータを断片化して送るように変更。

  Revision 1.12  2004/09/13 13:18:10  sato_masaki
  modify pad bytes

  Revision 1.11  2004/09/11 12:41:42  yosiokat
  ・ロック時間、親機情報寿命のカウント処理修正に伴い、MB_SCAN_TIME_NORMAL、MB_SCAN_TIME_LOCKINGを追加。

  Revision 1.10  2004/09/11 11:08:49  sato_masaki
  MB_CommWork構造体をMB_CommPWork, MB_CommCWork, MB_CommCommonWorkに分離。

  Revision 1.9  2004/09/10 07:22:47  sato_masaki
  debug swith off

  Revision 1.8  2004/09/10 02:59:10  sato_masaki
  PRINT_DEBUG, CALLBACK_WM_STATE定義に関する変更。

  Revision 1.7  2004/09/09 09:17:00  sato_masaki
  CALLBACK_WM_STATEを定義

  Revision 1.6  2004/09/07 04:48:21  sato_masaki
  padding調整。

  Revision 1.5  2004/09/07 04:26:17  sato_masaki
  rom_header.hをmb_rom_header.hにリネーム。

  Revision 1.4  2004/09/06 11:12:38  sato_masaki
  マルチブートRAMアドレス定義(内部使用)をmb_child.hへ移動。

  Revision 1.3  2004/09/04 11:03:50  sato_masaki
  small fix

  Revision 1.2  2004/09/04 09:59:03  sato_masaki
  記述整理。

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  ファイルを機能別に分割。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*
    MBライブラリの各ソースファイルでは、このヘッダファイルのみをインクルードしています。
*/

#ifndef _MB_PRIVATE_H_
#define _MB_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NITRO_FINALROM)
/*
#define PRINT_DEBUG
*/
#endif

#define CALLBACK_WM_STATE       0

#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb.h>
#include "mb_wm_base.h"
#include "mb_common.h"
#include "mb_block.h"
#include "mb_rom_header.h"
#include "mb_gameinfo.h"
#include "mb_fileinfo.h"
#include "mb_child.h"

/* キャッシュ方式のための新機構 */
#include "mb_cache.h"
#include "mb_task.h"

/* デバッグ出力 */
#ifdef  PRINT_DEBUG
#define MB_OUTPUT( ... )        OS_TPrintf( __VA_ARGS__ )
#define MB_DEBUG_OUTPUT( ... )  MBi_DebugPrint(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#else
#define MB_OUTPUT( ... )        ((void)0)
#define MB_DEBUG_OUTPUT( ... )  ((void)0)
#endif

// ----------------------------------------------------------------------------
// definition

/* ----------------------------------------------------------------------------

    ブロック転送関連定義

   ----------------------------------------------------------------------------*/

#define MB_IPL_VERSION                          (0x0001)
#define MB_MAX_BLOCK                            12288   // max 12288 * 256 = 3.0Mbyte

#define MB_NUM_PARENT_INFORMATIONS              16      /* 最大親機数 */

#define MB_SCAN_TIME_NORMAL                     10
#define MB_SCAN_TIME_LOCKING                    220

//SDK_COMPILER_ASSERT( sizeof(MBDownloadFileInfo) <= MB_COMM_BLOCK_SIZE );


/* ----------------------------------------------------------------------------

    内部使用型定義

   ----------------------------------------------------------------------------*/

/*
 * MB ライブラリで使用するワーク領域.
 */

typedef struct MB_CommCommonWork
{
    //  --- 共通    ---
    //  送受信バッファ
    u32     sendbuf[MB_MAX_SEND_BUFFER_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    u32     recvbuf[MB_MAX_RECV_BUFFER_SIZE * MB_MAX_CHILD * 2 / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    MBUserInfo user;                   // 自機のユーザー情報
    u16     isMbInitialized;
    int     block_size_max;
    BOOL    start_mp_busy;             /* WM_StartMP() is operating */
    BOOL    is_started_ex;             /* WM_StartParentEX() called */
    u8      padding[28];
}
MB_CommCommonWork;

typedef struct MB_CommPWork
{
    MB_CommCommonWork common;

    //  --- 親機用  ---
    MBUserInfo childUser[MB_MAX_CHILD] ATTRIBUTE_ALIGN(4);
    u16     childversion[MB_MAX_CHILD]; // 子機のバージョン情報(子機台数分)
    u32     childggid[MB_MAX_CHILD];   // 子機のGGID(子機台数分)
    MBCommPStateCallback parent_callback;       // 親機の状態コールバック関数ポインタ
    int     p_comm_state[MB_MAX_CHILD]; // それぞれの子機に対するstateを持つ
    u8      file_num;                  // 登録されているファイル数
    u8      cur_fileid;                // 現在送信するFile番号
    s8      fileid_of_child[MB_MAX_CHILD];      // 子機のリクエストしてきたFile番号(リクエスト無しの場合は-1)
    u8      child_num;                 // エントリー子機数
    u16     child_entry_bmp;           // 子機のエントリー管理bitmap
    MbRequestPieceBuf req_data_buf;    // 子機の分割リクエスト受信用のバッファ
    u16     req2child[MB_MAX_CHILD];   // 子機へ送信するリクエスト
    MBUserInfo childUserBuf;           // アプリケーションに渡すための、子機情報バッファ

    //  ファイル情報
    struct
    {
        // DownloadFileInfoのバッファ
        MBDownloadFileInfo dl_fileinfo;
        MBGameInfo game_info;
        MB_BlockInfoTable blockinfo_table;
        MBGameRegistry *game_reg;
        void   *src_adr;               //  親機上のブートイメージ先頭アドレス
        u16     currentb;              // 現在のブロック
        u16     nextb;                 // 次回送信するブロック
        u16     pollbmp;               // データ送信用PollBitmap
        u16     gameinfo_child_bmp;    // GameInfo更新のための現在のメンバbitmap
        u16     gameinfo_changed_bmp;  // GameInfo更新のための変更が発生したメンバbitmap
        u8      active;
        u8      update;

        /*
         * キャッシュ方式リードの対応のために追加.
         * これらが指す内容は src_adr の中.
         */
        MBiCacheList *cache_list;      /* キャッシュリスト */
        u32    *card_mapping;          /* 各セグメント先頭の CARD アドレス */

    }
    fileinfo[MB_MAX_FILE];

    BOOL    useWvrFlag;                // WVRを使用しているかどうかのフラグ
    u8      padding2[20];

    /* タスクスレッドのために追加 */
    u8      task_work[2 * 1024];
    MBiTaskInfo cur_task;

}
MB_CommPWork;


typedef struct MB_CommCWork
{
    MB_CommCommonWork common;

    //  --- 子機用  ---
    WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);   // WMBssDescのバックアップ
    MBDownloadFileInfo dl_fileinfo;    // 子機のDownloadFileInfoバッファ
    MBCommCStateCallbackFunc child_callback;    // 子機の状態コールバック関数ポインタ
    int     c_comm_state;              // 子機のstate
    int     connectTargetNo;           // 接続対象のMbBeaconRecvStatusリストNo.
    u8      fileid;                    // リクエストしたFile番号
    u8      _padding1[1];
    u16     user_req;
    u16     got_block;                 // Download済みブロック数
    u16     total_block;               // Downloadファイル総ブロック
    u8      recvflag[MB_MAX_BLOCK / 8]; // ブロック受信状態を表すフラグ
    MB_BlockInfoTable blockinfo_table;
    int     last_recv_seq_no;          // 前回受信したブロックシーケンス番号
    u8      boot_end_flag;             // BOOT_READYによるMB終了処理中か否かのフラグ
    u8      _padding2[15];
}
MB_CommCWork;


/* ----------------------------------------------------------------------------

    内部使用変数定義

   ----------------------------------------------------------------------------*/

extern MB_CommCommonWork *mbc;
/* 親機用ワークへのポインタマクロ */
#define pPwork                                  ((MB_CommPWork*)mbc)
/* 子機用ワークへのポインタマクロ */
#define pCwork                                  ((MB_CommCWork*)mbc)


#ifdef __cplusplus
}
#endif

#endif /*  _MB_PRIVATE_H_  */
