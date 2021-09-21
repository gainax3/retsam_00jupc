/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_child.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_child.h,v $
  Revision 1.5  2005/09/02 08:29:14  yosizaki
  add compatible symbol 'MB_COMM_CSTATE_CANCELED'

  Revision 1.4  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.3  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.2  2004/11/24 23:46:03  takano_makoto
  MB_COMM_CSTATE_FAKE_ENDを追加

  Revision 1.1  2004/11/22 12:57:34  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#ifndef NITRO_MB_MB_CHILD_H_
#define NITRO_MB_MB_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb/mb.h>

/* ---------------------------------------------------------------------

        型定義

   ---------------------------------------------------------------------*/

//---------------------------------------------------------
// 子機ダウンロードの状態
//---------------------------------------------------------

typedef enum
{
    MB_COMM_CSTATE_NONE,               // 初期化前状態
    MB_COMM_CSTATE_INIT_COMPLETE,      // MB子機初期化完了状態
    MB_COMM_CSTATE_CONNECT,            // 親機への接続が完了した状態
    MB_COMM_CSTATE_CONNECT_FAILED,     // 親機への接続が失敗した状態
    MB_COMM_CSTATE_DISCONNECTED_BY_PARENT,      // 親機から切断された状態
    MB_COMM_CSTATE_REQ_ENABLE,         // MPが確立してデータリクエストが可能になった状態
    MB_COMM_CSTATE_REQ_REFUSED,        // リクエストに対して親機からKICKされた状態
    MB_COMM_CSTATE_DLINFO_ACCEPTED,    // 親機にリクエストが受理された状態
    MB_COMM_CSTATE_RECV_PROCEED,       // ダウンロードデータの受信開始状態
    MB_COMM_CSTATE_RECV_COMPLETE,      // ダウンロードデータの受信完了状態
    MB_COMM_CSTATE_BOOTREQ_ACCEPTED,   // 親機からのブート要求受信状態
    MB_COMM_CSTATE_BOOT_READY,         // 親機との通信を切断してブート完了した状態
    MB_COMM_CSTATE_CANCELED,           // 途中でキャンセルされ、親機との切断が完了した状態
    MB_COMM_CSTATE_CANCELLED = MB_COMM_CSTATE_CANCELED,
    MB_COMM_CSTATE_AUTHENTICATION_FAILED,       // 
    MB_COMM_CSTATE_MEMBER_FULL,        // 親機へのエントリー数が定員を超えていた場合の状態
    MB_COMM_CSTATE_GAMEINFO_VALIDATED, // 親機のビーコン受信状態
    MB_COMM_CSTATE_GAMEINFO_INVALIDATED,        // 既に取得していた親機のビーコンの状態が不完全な状態になった場合
    MB_COMM_CSTATE_GAMEINFO_LOST,      // 親機のビーコンを見失った状態の通知
    MB_COMM_CSTATE_GAMEINFO_LIST_FULL, // これ以上親機を取得できなくなった状態の通知
    MB_COMM_CSTATE_ERROR,              // 途中でエラーが発生した状態

    MB_COMM_CSTATE_FAKE_END,           // フェイク子機の完了通知(mb_fake_child使用時のみ遷移する状態)

    /*  内部使用の列挙値です。
       この状態には遷移しません。 */
    MB_COMM_CSTATE_WM_EVENT = 0x80000000
}
MBCommCState;

#define MBCommCStateCallback    MBCommCStateCallbackFunc
/*  子機イベント通知コールバック型 */
typedef void (*MBCommCStateCallbackFunc) (u32 status, void *arg);


#ifdef __cplusplus
}
#endif


#endif // NITRO_MB_MB_CHILD_H_
