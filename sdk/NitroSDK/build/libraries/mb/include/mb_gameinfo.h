/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     MbGameInfo.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_gameinfo.h,v $
  Revision 1.12  2006/03/13 06:33:33  yosizaki
  add MB_FAKESCAN_PARENT_BEACON.

  Revision 1.11  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.9  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.8  2004/11/22 12:50:31  takano_makoto
  MBGameInfoRecvList, MBGameInfoRecvStatusをmb_child.hから移動

  Revision 1.7  2004/11/10 07:42:51  takano_makoto
  mb_gameinfo.cの中の子機側の処理からMBw関数を排除

  Revision 1.5  2004/09/14 23:55:48  yosizaki
  disable MB_CHANGE_TO_UNICODE
      (change MBGameregistry's member to Unicode)

  Revision 1.4  2004/09/13 09:24:39  yosiokat
  ・スキャンロック時にロック対象が全く見つからない状態の対策のため、MB_CountGameInfoLifetimeの引数にwmTypeEventを追加。

  Revision 1.3  2004/09/09 14:02:23  sato_masaki
  mb_gameinfo.cにおける、PlayerFlagのフォーマット変更。（作業中）

  Revision 1.2  2004/09/08 04:34:47  sato_masaki
  small fix

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  ファイルを機能別に分割。

  Revision 1.6  2004/09/01 09:40:51  sato_masaki
  MB_AUTHCODE_SIZEの値変更。

  Revision 1.5  2004/08/30 06:31:38  sato_masaki
  MBDownloadFileInfoに認証用コードauth_codeを追加。

  Revision 1.4  2004/08/19 07:43:11  sato_masaki
  GameInfoのフラグに関するマクロ定義を追加

  Revision 1.3  2004/08/12 09:15:52  yosiokat
  ・MbBeaconRecvStatusにスキャンロック期限カウント用nowLockTimeCountとlockTimeMaxCountを追加。

  Revision 1.2  2004/08/09 14:55:22  yosizaki
  format all the symbol for API

  Revision 1.1  2004/08/07 07:03:27  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


/*
 * このヘッダファイルは内部実装及び子機側でのみ使用します.
 * 通常のマルチブート親機を作成する上では必要ありません.
 */


#ifndef MB_GAME_INFO_H_
#define MB_GAME_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb/mb_gameinfo.h>


//=============================================================================
// 
// データ型　定義
//
//=============================================================================

#define MB_GAMEINFO_PARENT_FLAG                     (0x0001)
#define MB_GAMEINFO_CHILD_FLAG( __child_aid__ )     ( 1 << ( __child_aid__ ) )

//---------------------------------------------------------
// 親機から子機にビーコンで配信するゲーム情報
//---------------------------------------------------------

/*
 * 子機側ゲーム情報受信リスト構造体.
 */
typedef struct MBGameInfoRecvList
{
    MBGameInfo gameInfo;               // 親機ゲーム情報
    WMBssDesc bssDesc;                 // 親機接続用情報
    u32     getFlagmentFlag;           // 現在受信した断片ビーコンをビットで示す。
    u32     allFlagmentFlag;           // flagmentMaxNumをビットに換算した値
    u16     getPlayerFlag;             // 現在受信済みのプレイヤーフラグをビットで示す。
    s16     lifetimeCount;             // この情報に対する寿命カウンタ（この親機のビーコンを受信したら、寿命が延びる）
    u16     linkLevel;                 /* 親機からのBeacon受信強度を示す、4段階の値. */
    u8      beaconNo;                  // 最後に受信したビーコン番号
    u8      sameBeaconRecvCount;       // 同じビーコン番号を連続して受信した回数
}
MBGameInfoRecvList, MbGameInfoRecvList;


/*
 * 子機側ビーコン受信ステータス構造体
 */
typedef struct MbBeaconRecvStatus
{
    u16     usingGameInfoFlag;         // ゲーム情報の受信に使用中のgameInfo配列要素をビットで示す。
    u16     usefulGameInfoFlag;        // 一旦は全てのビーコンを受信して、validGameInfoFlagが立ったゲーム情報を示す。
    // （通信メンバーの更新等で、validGameInfoFlagは一時的に落ちる場合があるので、表示や接続にはこちらのフラグを使用して判断する。）
    u16     validGameInfoFlag;         // ゲーム情報を完全に受信しているgameInfo配列要素をビットで示す。
    u16     nowScanTargetFlag;         // 現在のScanターゲットをビットで示す。
    s16     nowLockTimeCount;          // 現在のScanターゲットの残りロック時間。
    s16     notFoundLockTargetCount;   // 現在のScanTargetが連続で見つからなかった回数
    u16     scanCountUnit;             // 現在のスキャン時間をカウント数に換算した値
    u8      pad[2];
    MBGameInfoRecvList list[MB_GAME_INFO_RECV_LIST_NUM];        // ゲーム情報受信リスト
}
MbBeaconRecvStatus;
/*
 * ※子機側は、
 * getFlagmentFlag == allFlagmentFlag　かつ　
 * getPlayerFlag   == gameInfo.volat.nowPlayerFlag
 * で、親機情報取得完了となる。
 */


/*
 * MB_RecvGameInfoBeacon, MB_CountGameInfoLifetime の
 * コールバック関数で返されるmsg.
 */
typedef enum MbBeaconMsg
{
    MB_BC_MSG_GINFO_VALIDATED = 1,
    MB_BC_MSG_GINFO_INVALIDATED,
    MB_BC_MSG_GINFO_LOST,
    MB_BC_MSG_GINFO_LIST_FULL,
    MB_BC_MSG_GINFO_BEACON
}
MbBeaconMsg;

typedef void (*MBBeaconMsgCallback) (MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

typedef void (*MbScanLockFunc) (u8 *macAddress);
typedef void (*MbScanUnlockFunc) (void);

/******************************************************************************/
/* 以下は内部使用 */


//------------------
// 親機側関数
//------------------

    // 送信ステータスの初期化。
void    MB_InitSendGameInfoStatus(void);

    // MbGameRegistryからMbGameInfoを作成する。
void    MBi_MakeGameInfo(MBGameInfo *gameInfop,
                         const MBGameRegistry *mbGameRegp, const MBUserInfo *parent);

    // MBGameInfoの子機メンバー情報を更新する。
void    MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                                const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag);

    // 生成したMBGameInfoをビーコンで発信するよう送信リストに追加する。
void    MB_AddGameInfo(MBGameInfo *newGameInfop);

    // 送信リストに追加しているMBGameInfoを削除する。
BOOL    MB_DeleteGameInfo(MBGameInfo *gameInfop);

    // 送信リストに登録されているMBGameInfoをビーコンに乗せて発信する。
void    MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute);


//------------------
// 子機側関数
//------------------
    // ビーコン受信ステータスバッファをstaticに確保
void    MBi_SetBeaconRecvStatusBufferDefault(void);
    // ビーコン受信ステータスバッファを設定
void    MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf);

    // 受信ステータスの初期化
void    MB_InitRecvGameInfoStatus(void);

    // 受信したビーコンからMBGameInfoを取り出す。
BOOL    MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp);

    // 親機情報リストの寿命カウント
void    MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent);

    // スキャンロック関数を設定
void    MBi_SetScanLockFunc(MbScanLockFunc lockFunc, MbScanUnlockFunc unlockFunc);

    // 受信した親機情報構造体へのポインタを取得する
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index);

/* ビーコン受信状態を取得 */
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void);

/* 指定されたゲーム情報の削除 */
void    MB_DeleteRecvGameInfo(int index);
void    MB_DeleteRecvGameInfoWithoutBssdesc(int index);

//------------------
// Unicode 移行用シンボル.
//------------------
//#define   MB_CHANGE_TO_UNICODE

#if defined(MB_CHANGE_TO_UNICODE)

void    MBi_ExSJIS_BEtoUTF16_LE(u8 *sjisp, u16 *unip, u16 length);
void    MBi_ExUTF16_LEtoSJIS_BE(u8 *sjisp, u16 *unip, u16 length);
void    MBi_CheckSJIS_BEtoUTF16_LE(void);

#endif /* defined(MB_CHANGE_TO_UNICODE) */

#ifdef __cplusplus
}
#endif

#endif // MB_GAME_INFO_H_
