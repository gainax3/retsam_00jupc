/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb.h,v $
  Revision 1.13  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.12  2005/08/17 01:13:34  yosizaki
  add MB_GetMultiBootDownloadParameter().

  Revision 1.11  2005/05/20 03:11:07  yosizaki
  add MBGameRegistry::userParam.

  Revision 1.10  2005/04/11 03:37:16  yosizaki
  fix comment.

  Revision 1.9  2005/04/11 01:46:10  yosizaki
  add MB_SetLifeTime().
  add MB_SetPowerSaveMode().

  Revision 1.8  2005/03/14 07:09:33  sato_masaki
  MB_GetGameEntryBitmap()関数を追加。

  Revision 1.7  2005/03/01 01:57:00  yosizaki
  copyright の年を修正.

  Revision 1.6  2005/02/28 05:26:24  yosizaki
  do-indent.

  Revision 1.5  2004/11/24 05:14:43  yosizaki
  rename MB_StartParentEx/MB_EndEx..

  Revision 1.4  2004/11/22 12:58:17  takano_makoto
  コメント追加

  Revision 1.3  2004/11/18 07:34:05  takano_makoto
  MB_SetParentCommSize, MB_SetParentCommParamで子機送信サイズの設定引数を廃止

  Revision 1.2  2004/11/11 11:54:44  yosizaki
  add MB_SEGMENT_BUFFER_MIN.
  increase MB_SYSTEM_BUF_SIZE for task-system. (+0x1000)

  Revision 1.1  2004/11/10 13:12:47  takano_makoto
  ファイルの位置を移動

  Revision 1.54  2004/10/26 08:10:34  takano_makoto
  MB_DisconnectChildを追加

  Revision 1.53  2004/10/18 11:46:12  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.52  2004/10/05 14:42:50  yosizaki
  export some MB_COMM_P_* range consts.

  Revision 1.51  2004/10/04 13:40:25  terui
  ゲームグループIDをu32型に統一。

  Revision 1.50  2004/10/01 07:39:11  sato_masaki
  MB_COMM_PSTATE_WAIT_TO_SEND ステートを追加。

  Revision 1.49  2004/09/22 12:17:56  sato_masaki
  add function MB_SetParentCommParam()

  Revision 1.48  2004/09/21 01:39:31  sato_masaki
  WMに起因するエラーを、MB_ERRCODE_WM_FAILUREに統一。MB_ERRCODE_MP_SENT_FAILUREを廃止。

  Revision 1.47  2004/09/20 13:16:07  sato_masaki
  - MB_MAX_FILEを16に変更
  - MB_ERROR定義を削除
  - MBErrorStatus構造体のlevelメンバを削除(エラーレベルの廃止)
  - エラーの定義をMBError列挙子から、MBErrCode列挙子に移行。

  Revision 1.46  2004/09/17 05:08:10  sato_masaki
  small fix

  Revision 1.45  2004/09/17 04:43:09  sato_masaki
  add function MB_ReadMultiBootParentBssDesc()

  Revision 1.44  2004/09/16 12:46:56  sato_masaki
  MBi_***で定義していた関数を、MB_***に変更。

  Revision 1.43  2004/09/15 06:27:34  sato_masaki
  add function MBi_SetParentCommSize, MBi_GetTgid

  Revision 1.42  2004/09/15 05:08:20  miya
  エラーコード追加

  Revision 1.41  2004/09/14 14:23:19  yosiokat
  ・mb_gameinfo.c内でしか使わない定数定義をmb_gameinfo.cに移動。

  Revision 1.40  2004/09/14 13:20:13  sato_masaki
  リクエストデータを断片化して送るように変更。

  Revision 1.39  2004/09/14 06:06:24  sato_masaki
  MB_CommBootRequest**, MB_CommStartSending**をインライン化。

  Revision 1.38  2004/09/13 13:03:32  yosiokat
  ・MB_GAME_NAME_LENGTHを"48"に変更。
  ・MB_GAME_INTRO_LENGTHを"96"に変更。

  Revision 1.37  2004/09/13 08:18:54  sato_masaki
  - add MB_COMM_PSTATE_ERROR to MBCommPState
  - add MBiErrorStatus, MBiError

  Revision 1.36  2004/09/13 03:01:14  sato_masaki
  MBi_Init()を追加。(旧仕様との互換のため)
  MB_GetParentSystemBufSize を MBi_GetParentSystemBufSize に変更。

  Revision 1.35  2004/09/11 11:23:57  sato_masaki
  - MB_SYSTEM_BUF_SIZE 値変更
  - MBCommResponseRequestType型にメンバ追加。
  - MB_GetParentSystemBufSize()追加。

  Revision 1.34  2004/09/10 07:40:51  yosizaki
  add MBCommRequestData.

  Revision 1.33  2004/09/10 06:30:29  sato_masaki
  comment追加。

  Revision 1.32  2004/09/10 02:42:10  sato_masaki
  MBUserInfo構造体のplayerNoに関する番号定義を変更。
  (旧:親15、子0-14 → 新:親0、子1-15)

  Revision 1.31  2004/09/08 04:39:54  sato_masaki
  comment修正。

  Revision 1.30  2004/09/08 04:39:13  sato_masaki
  MB_CommBootRequestAll()関数を追加。変更前のMB_CommBootRequest()と互換の機能を持つ関数です。

  Revision 1.29  2004/09/07 11:56:05  sato_masaki
  - MB_CommBootRequest()をAID指定の関数に変更。
  - MB_COMM_PSTATE_CANCELをMB_COMM_PSTATE_ENDに名称変更。
    MBライブラリの終了コールバックは、MB_COMM_PSTATE_ENDのみとする。

  Revision 1.28  2004/09/07 04:41:14  sato_masaki
  MBCommPState列挙子にMB_COMM_PSTATE_WM_EVENTを追加。

  Revision 1.27  2004/09/06 12:26:43  sato_masaki
  comment修正

  Revision 1.26  2004/09/06 12:02:33  sato_masaki
  MB_Init()関数の引数に、tgidを追加。

  Revision 1.25  2004/09/04 09:51:35  sato_masaki
  記述修正。子機APIおよび型に関する定義が残っていたため、削除する。

  Revision 1.24  2004/09/04 06:46:49  sato_masaki
  定員超過時のコールバックステート(MB_COMM_*STATE_MEMBER_FULL)を設ける。

  Revision 1.23  2004/09/03 07:11:15  sato_masaki
  子機制御用API記述を削除。

  Revision 1.22  2004/09/02 11:55:58  sato_masaki
  MB_CommStartSending()に子機AIDを引数として追加。
  エントリーしてきた子機に一括して送信する場合は、MB_CommStartSendingAll()
  を使用するように変更。

  Revision 1.21  2004/09/02 09:59:44  sato_masaki
  MB_DeleteFile()をMB_UnregisterFile()に名前変更。

  Revision 1.20  2004/09/02 09:45:56  sato_masaki
  ファイル登録削除の関数、MB_DeleteFile()を追加。

  Revision 1.19  2004/08/27 04:04:43  sato_masaki
  MBCommCStateにMB_COMM_CSTATE_AUTHENTICATION_FAILED追加。

  Revision 1.18  2004/08/26 09:59:54  sato_masaki
  MB_COMM_PSTATE_REQUESTEDを追加。
  MB_CommResponseRequest関数を追加。

  Revision 1.17  2004/08/25 03:15:18  sato_masaki
  MB_CommStartSending関数を追加。

  Revision 1.16  2004/08/19 14:54:44  yosiokat
  MB_BEACON_FIXED_DATA_SIZEを構造体の変更に合わせて修正。

  Revision 1.15  2004/08/19 07:42:26  sato_masaki
  MB_ROM_HEADER_ADDRESSの定義をHW_ROM_HEADER_BUFに変更

  Revision 1.14  2004/08/19 02:42:14  yosizaki
  change type of MBUserInfo.

  Revision 1.13  2004/08/16 13:08:02  sato_masaki
  RomHeader格納アドレス、MB_ROM_HEADER_ADDRESSを追加

  Revision 1.12  2004/08/14 12:19:56  sato_masaki
  MBParentBssDescから、gameInfoLengthメンバを削除

  Revision 1.11  2004/08/13 13:16:10  sato_masaki
  MB_COMM_PSTATE_DISCONNECTEDを追加

  Revision 1.10  2004/08/12 12:05:18  sato_masaki
  MBCommCStateの並び順変更

  Revision 1.9  2004/08/12 09:56:28  yosiokat
  MBGameRegistryのアイコンデータ指定をファイルパスへのポインタに変更。

  Revision 1.8  2004/08/12 09:14:11  yosiokat
  ・MB_SCAN_LOCK_SECの追加。
  ・MBCommCStateにMB_COMM_CSTATE_CONNECT_FAILEDとMB_COMM_CSTATE_DISCONNECTED_BY_PARENTを追加。
  ・内部使用関数MB_DeleteRecvGameInfoの追加。

  Revision 1.7  2004/08/12 09:11:11  yosiokat
  ・MB_BSSDESC_SIZE定義をsizeof(MBParentBssDesc)に変更。
  ・MB_BSSDESC_ADDRESSの定義を0x023fe800に変更。

  Revision 1.6  2004/08/10 12:07:55  sato_masaki
  親機、子機ステート定義の変更、および、MB_CommBootRequest, MB_CommIsBootable関数の追加。

  Revision 1.5  2004/08/09 14:56:09  yosizaki
  format all the symbol for API

  Revision 1.4  2004/08/09 07:08:49  sato_masaki
  定義 MB_BSSDESC_ADDRESS の値を変更

  Revision 1.3  2004/08/08 23:42:14  yosizaki
  change some names.

  Revision 1.2  2004/08/07 15:03:12  yosizaki
  merge all the diff. (-2004/08/07)

  Revision 1.1  2004/08/07 07:03:27  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#ifndef NITRO_MB_MB_H_
#define NITRO_MB_MB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/misc.h>
#include <nitro/fs/file.h>
#include <nitro/wm.h>


/* ---------------------------------------------------------------------

        定数

   ---------------------------------------------------------------------*/

#define MB_MAX_CHILD                    WM_NUM_MAX_CHILD

/* ファイル名の文字数 */
#define MB_FILENAME_SIZE_MAX            (10)

/* 親機のファイル最大数 */
#define MB_MAX_FILE                     (16)

/* マルチブートワークのサイズ */
#define MB_SYSTEM_BUF_SIZE              (0xC000)

/* 親機の通信サイズの設定可能範囲 */
#define MB_COMM_PARENT_SEND_MIN         256
#define MB_COMM_PARENT_SEND_MAX         510

#define MB_COMM_PARENT_RECV_MIN         8
#define MB_COMM_PARENT_RECV_MAX         16

/* MB_ReadSegment() に必要なバッファサイズの最小値 */
#define MB_SEGMENT_BUFFER_MIN           0x10000

/* MB API 結果型 */
#define MB_SUCCESS                      MB_ERRCODE_SUCCESS      // 旧互換

/* MB_Init() に対して TGID を自動で設定するよう指定する定義値 */
#define	MB_TGID_AUTO	0x10000

/*
 * マルチブート用親機ゲーム情報
 */

#define MB_ICON_COLOR_NUM               16      // アイコン色数
#define MB_ICON_PALETTE_SIZE            (MB_ICON_COLOR_NUM * 2) // アイコンデータサイズ (16bitカラー×色数)
#define MB_ICON_DATA_SIZE               512     // アイコンデータサイズ (32×32dot 16色)
#define MB_GAME_NAME_LENGTH             48      // ゲームネーム長       (2byte単位)         ※最大で左記文字数でかつ、横幅185ドット以内に収まる範囲で指定。
#define MB_GAME_INTRO_LENGTH            96      // ゲーム説明長         (2byte単位)         ※最大で左記文字数でかつ、横幅199ドット×2に収まる範囲で指定。
#define MB_USER_NAME_LENGTH             10      // ユーザーネーム長     (2byte単位)
#define MB_MEMBER_MAX_NUM               15      // 通信メンバー最大数
#define MB_FILE_NO_MAX_NUM              64      // MbGameInfo.fileNoはビーコンに乗せる時に6bitになるので、最大は64個
#define MB_GAME_INFO_RECV_LIST_NUM      16      // 子機側でのゲーム情報受信リスト個数

/* ブートタイプ */
#define MB_TYPE_ILLEGAL                 0       /* 不正な状態 */
#define MB_TYPE_NORMAL                  1       /* ROM から起動 */
#define MB_TYPE_MULTIBOOT               2       /* マルチブート子機 */

#define MB_BSSDESC_SIZE                 (sizeof(MBParentBssDesc))
#define MB_DOWNLOAD_PARAMETER_SIZE      HW_DOWNLOAD_PARAMETER_SIZE

/* 親機ダウンロードの状態 */
typedef enum
{
    MB_COMM_PSTATE_NONE,
    MB_COMM_PSTATE_INIT_COMPLETE,
    MB_COMM_PSTATE_CONNECTED,
    MB_COMM_PSTATE_DISCONNECTED,
    MB_COMM_PSTATE_KICKED,
    MB_COMM_PSTATE_REQ_ACCEPTED,
    MB_COMM_PSTATE_SEND_PROCEED,
    MB_COMM_PSTATE_SEND_COMPLETE,
    MB_COMM_PSTATE_BOOT_REQUEST,
    MB_COMM_PSTATE_BOOT_STARTABLE,
    MB_COMM_PSTATE_REQUESTED,
    MB_COMM_PSTATE_MEMBER_FULL,
    MB_COMM_PSTATE_END,
    MB_COMM_PSTATE_ERROR,
    MB_COMM_PSTATE_WAIT_TO_SEND,

    /*  内部使用の列挙値です。
       この値の状態には遷移しません。 */
    MB_COMM_PSTATE_WM_EVENT = 0x80000000
}
MBCommPState;


/* 子機からの接続リクエストに対する応答 */
typedef enum
{
    MB_COMM_RESPONSE_REQUEST_KICK,     /* 子機からのエントリー要求を拒否する． */
    MB_COMM_RESPONSE_REQUEST_ACCEPT,   /* 子機からのエントリー要求を受け入れる． */
    MB_COMM_RESPONSE_REQUEST_DOWNLOAD, /* 子機へダウンロード開始を通知する． */
    MB_COMM_RESPONSE_REQUEST_BOOT      /* 子機へブート開始を通知する． */
}
MBCommResponseRequestType;


/*  親機イベント通知コールバック型 */
typedef void (*MBCommPStateCallback) (u16 child_aid, u32 status, void *arg);


typedef struct
{
    u16     errcode;
}
MBErrorStatus;

typedef enum
{
    /* 成功を示す値 */
    MB_ERRCODE_SUCCESS = 0,

    MB_ERRCODE_INVALID_PARAM,          /* 引数エラー */
    MB_ERRCODE_INVALID_STATE,          /* 呼び出し状態の条件不整合 */

    /* 以下のエラーは子機専用． */
    MB_ERRCODE_INVALID_DLFILEINFO,     /* ダウンロード情報が不正 */
    MB_ERRCODE_INVALID_BLOCK_NO,       /* 受信したブロック番号が不正 */
    MB_ERRCODE_INVALID_BLOCK_NUM,      /* 受信ファイルのブロック数が不正 */
    MB_ERRCODE_INVALID_FILE,           /* 要求していないファイルのブロックを受信した */
    MB_ERRCODE_INVALID_RECV_ADDR,      /* 受信アドレスが不正 */

    /* 以下のエラーはWMのエラーに起因． */
    MB_ERRCODE_WM_FAILURE,             /* WM のコールバックにおけるエラー */

    /** 以降は通信継続不可なもの (WM の再初期化が必要) **/
    MB_ERRCODE_FATAL,

    MB_ERRCODE_MAX
}
MBErrCode;

/* ---------------------------------------------------------------------

        構造体

   ---------------------------------------------------------------------*/

/*
 * マルチブートゲーム登録情報構造体.
 */
typedef struct
{
    /* プログラムバイナリのファイルパス */
    const char *romFilePathp;
    /* ゲーム名の文字列を指すポインタ */
    u16    *gameNamep;
    /* ゲーム内容の説明文文字列を指すポインタ */
    u16    *gameIntroductionp;
    /* アイコンキャラデータのファイルパス */
    const char *iconCharPathp;
    /* アイコンパレットデータのファイルパス */
    const char *iconPalettePathp;
    /* ゲームグループID */
    u32     ggid;
    /* 最大プレイ人数 */
    u8      maxPlayerNum;
    u8      pad[3];
    /* ユーザ定義の拡張パラメータ (32BYTE) */
    u8      userParam[MB_DOWNLOAD_PARAMETER_SIZE];
}
MBGameRegistry;


/*
 * アイコンデータ構造体.
 * アイコンは 16色パレット + 32dot * 32dot.
 */
typedef struct
{
    /* パレットデータ (32 BYTE) */
    u16     palette[MB_ICON_COLOR_NUM];
    u16     data[MB_ICON_DATA_SIZE / 2];
    /* キャラクタデータ (512 BUYTE) */
}
MBIconInfo;                            /* 544byte */


/*
 * ユーザー情報構造体.
 */
typedef struct
{
    /* 好きな色データ (色セット番号) */
    u8      favoriteColor:4;
    /* プレイヤー番号（親機はNo.0、子機はNo.1-15） */
    u8      playerNo:4;
    /* ニックネーム長 */
    u8      nameLength;
    /* ニックネーム */
    u16     name[MB_USER_NAME_LENGTH];
}
MBUserInfo;                            /* 22byte */


/*
 * マルチブート親機情報.
 * (WMbssDescからgameInfo[128]を除いたもの)
 */
typedef struct
{
    u16     length;                    // 2
    u16     rssi;                      // 4
    u16     bssid[3];                  // 10
    u16     ssidLength;                // 12
    u8      ssid[32];                  // 44
    u16     capaInfo;                  // 46
    struct
    {
        u16     basic;                 // 48
        u16     support;               // 50
    }
    rateSet;
    u16     beaconPeriod;              // 52
    u16     dtimPeriod;                // 54
    u16     channel;                   // 56
    u16     cfpPeriod;                 // 58
    u16     cfpMaxDuration;            // 60
}
MBParentBssDesc;


/* マルチブート子機にとっての親機情報を保持する構造体 */
typedef struct
{
    u16     boot_type;
    MBParentBssDesc parent_bss_desc;
}
MBParam;


/* 子機からのエントリーリクエスト時に受信するデータ型 */
typedef struct
{
    u32     ggid;                      //  4B
    MBUserInfo userinfo;               // 22B
    u16     version;                   //  2B
    u8      fileid;                    //  1B
    u8      pad[3];                    // 計 29B
}
MBCommRequestData;


/* ---------------------------------------------------------------------

        マルチブートライブラリ(MB)API - 親機用

   ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MB_Init

  Description:  MB ライブラリ初期化

  Arguments:    work   - MBの内部状態管理のために割り当てるメモリへのポインタ
                         このサイズは MB_SYSTEM_BUF_SIZE バイト以上である必要
                         があります。また、動的確保を行う場合、
                         MB_GetParentSystemBufSize関数にて、サイズを取得することも
                         できます。
                         割り当てたメモリはMB_End関数を呼ぶまで内部で使用され
                         ます。

                user   - ユーザデータが格納された構造体のポインタ
                         このポインタが指す内容はMB_Init関数の中でのみ参照され
                         ます。

                ggid   - GGIDを指定します。

                tgid   - TGIDの値 (値はWMのガイドラインに則った決め方で生成した
                         ものを使用してください。)
                         MB_TGID_AUTO を指定すると、内部で自動的に適切な値が
                         設定されます。

                dma    - MBの内部処理のために割り当てるDMAチャンネル
                         このDMAチャンネルはMB_End関数を呼ぶまで内部で使用され
                         ます。 
  
  Returns:      初期化が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_Init(void *work, const MBUserInfo *user, u32 ggid, u32 tgid, u32 dma);


/*---------------------------------------------------------------------------*
  Name:         MB_GetParentSystemBufSize

  Description:  MBで使用するワークメモリのサイズを取得

  Arguments:    None.
  
  Returns:      MBで使用するワークメモリのサイズ
 *---------------------------------------------------------------------------*/
int     MB_GetParentSystemBufSize(void);


/*---------------------------------------------------------------------------*
  Name:         MB_SetParentCommSize

  Description:  MB 通信に使用する親機の通信データサイズをセット。
                MB_Init を実行し、 MB_StartParent をコールする前のタイミングで
                使用してください。通信開始後は、バッファサイズの変更はできません。

  Arguments:    sendSize - 親機から各子機に送る、送信データサイズ
  
  Returns:      送受信データサイズの変更が成功すればTRUE、そうでない場合は
                FALSEを返します．
 *---------------------------------------------------------------------------*/

BOOL    MB_SetParentCommSize(u16 sendSize);

/*---------------------------------------------------------------------------*
  Name:         MB_SetParentCommParam

  Description:  MB 通信に使用する親機の通信データサイズ、最大子機接続数をセット。
                MB_Init を実行し、 MB_StartParent をコールする前のタイミングで
                使用してください。通信開始後は、バッファサイズの変更はできません。

  Arguments:    sendSize    - 親機から各子機に送る、送信データサイズ
                maxChildren - 最大子機接続数
  
  Returns:      送受信データサイズの変更が成功すればTRUE、そうでない場合は
                FALSEを返します．
 *---------------------------------------------------------------------------*/

BOOL    MB_SetParentCommParam(u16 sendSize, u16 maxChildren);

/*---------------------------------------------------------------------------*
  Name:         MB_GetTgid

  Description:  MB ライブラリにおいて、WMで使用しているTGIDを取得

  Arguments:    None.
  
  Returns:      WMで使用しているTGIDを返します．
 *---------------------------------------------------------------------------*/

u16     MB_GetTgid(void);


/*---------------------------------------------------------------------------*
  Name:         MB_End

  Description:  MBライブラリを終了

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_End(void);


/*---------------------------------------------------------------------------*
  Name:         MB_EndToIdle

  Description:  MBライブラリを終了.
                ただしWMライブラリは IDLE ステートに維持されます.

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_EndToIdle(void);

/*---------------------------------------------------------------------------*
  Name:         MB_DisconnectChild

  Description:  子機の切断

  Arguments:    aid - 切断する子機のaid 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

void    MB_DisconnectChild(u16 aid);


/*---------------------------------------------------------------------------*
  Name:         MB_StartParent

  Description:  親機パラメータ設定 & スタート

  Arguments:    channel - 親機として通信を行うチャンネル 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_StartParent(int channel);


/*---------------------------------------------------------------------------*
  Name:         MB_StartParentFromIdle

  Description:  親機パラメータ設定 & スタート.
                ただしWMライブラリが IDLE ステートであることが必要です.

  Arguments:    channel - 親機として通信を行うチャンネル 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

int     MB_StartParentFromIdle(int channel);

/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildrenNumber

  Description:  現在エントリーしている子の総数を取得

  Arguments:    None.

  Returns:      現在親機が認識している子機の総数を返します。
 *---------------------------------------------------------------------------*/

u8      MB_CommGetChildrenNumber(void);


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetChildUser

  Description:  指定 aid の子の情報を取得

  Arguments:    child_aid - 状態を取得する子機を指す aid 
  
  Returns:      開始処理が正しく完了すれば MB_SUCCESS を返します。
 *---------------------------------------------------------------------------*/

const MBUserInfo *MB_CommGetChildUser(u16 child_aid);


/*---------------------------------------------------------------------------*
  Name:         MB_CommGetParentState

  Description:  指定 aid の子機に対する親機状態を取得

  Arguments:    child_aid - 状態を取得する子機を指す aid 
  
  Returns:      MBCommPStateで示されるいずれかの状態を返します。
 *---------------------------------------------------------------------------*/

int     MB_CommGetParentState(u16 child);


/*---------------------------------------------------------------------------*
  Name:         MB_CommSetParentStateCallback

  Description:  親機状態通知コールバックを設定

  Arguments:    callback - 親機状態を通知させるコールバック関数へのポインタ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/

void    MB_CommSetParentStateCallback(MBCommPStateCallback callback);


/*---------------------------------------------------------------------------*
  Name:         MB_GetSegmentLength

  Description:  指定されたバイナリファイルに必要なセグメントバッファ長を取得

  Arguments:    file - バイナリファイルを指す FSFile 構造体
  
  Returns:      正常にサイズを取得できれば正の値を, そうでなければ 0
 *---------------------------------------------------------------------------*/

u32     MB_GetSegmentLength(FSFile *file);


/*---------------------------------------------------------------------------*
  Name:         MB_ReadSegment

  Description:  指定されたバイナリファイルから必要なセグメントデータを読み出し.

  Arguments:    file - バイナリファイルを指す FSFile 構造体
                buf  - セグメントデータを読み出すバッファ
                len  - buf のサイズ
  
  Returns:      正常にセグメントデータを読み出せれば TRUE, そうでなければ FALSE
 *---------------------------------------------------------------------------*/

BOOL    MB_ReadSegment(FSFile *file, void *buf, u32 len);


/*---------------------------------------------------------------------------*
  Name:         MB_RegisterFile

  Description:  指定したファイルをダウンロードリストへ登録

  Arguments:    game_reg - 登録するプログラムの情報が格納された
                           MBGameRegistry構造体へのポインタ 
                buf      - 抽出したセグメント情報が格納されたメモリへのポインタ
                           このセグメント情報はMB_ReadSegment関数で取得します。
  
  Returns:      ファイルを正しく登録できた場合はTRUE、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

BOOL    MB_RegisterFile(const MBGameRegistry *game_reg, const void *buf);


/*---------------------------------------------------------------------------*
  Name:         MB_UnregisterFile

  Description:  指定したファイルをダウンロードリストから削除

  Arguments:    game_reg - 削除するプログラムの情報が格納された
                           MBGameRegistry構造体へのポインタ 

  Returns:      ファイルを正しく削除できた場合はTRUE、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

void   *MB_UnregisterFile(const MBGameRegistry *game_reg);


/*---------------------------------------------------------------------------*
  Name:         MB_CommResponseRequest

  Description:  子機からの接続リクエストに対する応答を指定

  Arguments:    child_aid - 応答を送信する子機を指す aid
                ack       - 子機への応答結果を指定する
                            MBCommResponseRequestType型の列挙値

  Returns:      指定した子機が接続応答待ち状態であり
                指定した応答種類が正当なものであればTRUEを、
                そうでない場合はFALSEを返します。
 *---------------------------------------------------------------------------*/

BOOL    MB_CommResponseRequest(u16 child_aid, MBCommResponseRequestType ack);


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartSending

  Description:  エントリー済みの指定 aid の子機へ、ブートイメージ送信を開始する

  Arguments:    child_aid - ブートイメージ送信を開始する子機を指す aid

  Returns:      指定子機がエントリーしていればTRUEを、
                そうでないならFALSEを返します。
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommStartSending(u16 child_aid)
{
    return MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_DOWNLOAD);
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommStartSendingAll

  Description:  エントリーしてきた子機へ、ブートイメージ送信を開始する

  Arguments:    None.

  Returns:      エントリーしている子機が存在していればTRUEを、
                そうでないならFALSEを返します。
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommStartSendingAll(void)
{
    u8      child, num;

    for (num = 0, child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (TRUE == MB_CommStartSending(child))
        {
            num++;
        }
    }

    if (num == 0)
        return FALSE;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommIsBootable

  Description:  ブート可能かを判定

  Arguments:    child_aid - 対象子機のAID

  Returns:      yes - TRUE  no - FALSE
 *---------------------------------------------------------------------------*/

BOOL    MB_CommIsBootable(u16 child_aid);


/*---------------------------------------------------------------------------*
  Name:         MB_CommBootRequest

  Description:  ダウンロード済みの指定した子機に対してブート要求を送信する

  Arguments:    child_aid - 対象子機のAID

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommBootRequest(u16 child_aid)
{
    return MB_CommResponseRequest(child_aid, MB_COMM_RESPONSE_REQUEST_BOOT);
}


/*---------------------------------------------------------------------------*
  Name:         MB_CommBootRequestAll

  Description:  ダウンロード済み子機に対してブート要求を送信する

  Arguments:    None.

  Returns:      success - TRUE  failed - FALSE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_CommBootRequestAll(void)
{
    u8      child, num;

    for (num = 0, child = 1; child <= WM_NUM_MAX_CHILD; child++)
    {
        if (TRUE == MB_CommBootRequest(child))
        {
            num++;
        }
    }

    if (num == 0)
        return FALSE;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_GetGameEntryBitmap

  Description:  配信中のGameRegistryに対してエントリーしている
                AIDビットマップを取得する。

  Arguments:    game_req      - 対象となるGameRegistryへのポインタ

  Returns:      指定したGameRegistryにエントリーしているAIDビットマップ
                (親機AID:0、子機AID:1-15)
                ゲームが配信中ではない場合、返り値は0となる。
                （ゲームが配信中の場合、必ず親機AID:0がエントリーメンバーに
                  含まれている。）
 *---------------------------------------------------------------------------*/

u16     MB_GetGameEntryBitmap(const MBGameRegistry *game_reg);


/* ---------------------------------------------------------------------

        マルチブートライブラリ(MB)API - ブート後子機用

   ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootParam

  Description:  マルチブート子機にとっての親機情報が保持されたポインタを取得

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static inline const MBParam *MB_GetMultiBootParam(void)
{
    return (const MBParam *)HW_WM_BOOT_BUF;
}


/*---------------------------------------------------------------------------*
  Name:         MB_IsMultiBootChild

  Description:  マルチブート子機かどうか判定
                TRUE であれば MB_GetMultiBootParentBssDesc() が有効

  Arguments:    None.

  Returns:      マルチブート子機であれば TRUE
 *---------------------------------------------------------------------------*/

static inline BOOL MB_IsMultiBootChild(void)
{
    return MB_GetMultiBootParam()->boot_type == MB_TYPE_MULTIBOOT;
}


/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootParentBssDesc

  Description:  マルチブート子機であれば、親機情報へのポインタを返す

  Arguments:    None.

  Returns:      マルチブート子機であれば有効なポインタ、そうでなければ NULL
 *---------------------------------------------------------------------------*/

static inline const MBParentBssDesc *MB_GetMultiBootParentBssDesc(void)
{
    return MB_IsMultiBootChild()? &MB_GetMultiBootParam()->parent_bss_desc : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetMultiBootDownloadParameter

  Description:  マルチブート子機であれば、ダウンロード時に指定された
                ユーザ定義の拡張パラメータを返す

  Arguments:    None.

  Returns:      マルチブート子機であれば有効なポインタ、そうでなければ NULL
 *---------------------------------------------------------------------------*/

static inline const u8 *MB_GetMultiBootDownloadParameter(void)
{
    return MB_IsMultiBootChild()? (const u8 *)HW_DOWNLOAD_PARAMETER : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         MB_ReadMultiBootParentBssDesc

  Description:  マルチブート親機から引き継いだ情報をもとに、
                WM_StartConnect 関数に用いるための、WMBssDesc 構造体の情報を
                セットする

  Arguments:    p_desc            pointer to destination WMBssDesc
                                    (must be aligned 32-bytes)
                parent_max_size   max packet length of parent in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                child_max_size    max packet length of child in MP-protocol.
                                    (must be equal to parent's WMParentParam!)
                ks_flag           if use key-sharing mode, TRUE.
                                    (must be equal to parent's WMParentParam!)
                cs_flag           if use continuous mode, TRUE.
                                    (must be equal to parent's WMParentParam!)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_ReadMultiBootParentBssDesc(WMBssDesc *p_desc,
                                      u16 parent_max_size, u16 child_max_size, BOOL ks_flag,
                                      BOOL cs_flag);

/*---------------------------------------------------------------------------*
  Name:         MB_SetLifeTime

  Description:  MB のワイヤレス駆動に対してライフタイムを明示的に指定します.
                デフォルトでは ( 0xFFFF, 40, 0xFFFF, 40 ) になっています.

  Arguments:    tableNumber     ライフタイムを設定するCAMテーブル番号
                camLifeTime     CAMテーブルのライフタイム
                frameLifeTime   フレーム単位でのライフタイム
                mpLifeTime      MP通信のライフタイム

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_SetLifeTime(u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime);

/*---------------------------------------------------------------------------*
  Name:         MB_SetPowerSaveMode

  Description:  省電力モードを設定します.
                省電力モードは, デフォルトで有効となっています.
                この関数は電力消費を考慮しなくてよい場面で安定に駆動する
                ためのオプションであり, 電源につながれていることを保証
                された動作環境でない限り, 通常のゲームアプリケーションが
                これを使用すべきではありません.

  Arguments:    enable           有効にするならば TRUE, 無効にするならば FALSE.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MB_SetPowerSaveMode(BOOL enable);



/*****************************************************************************/
/* obsolete interfaces */

#define MB_StartParentEx(channel)    SDK_ERR("MB_StartParentEx() is discontinued. please use MB_StartParentFromIdle()\n")
#define MB_EndEx()    SDK_ERR("MB_EndEx() is discontinued. please use MB_EndToIdle()\n")



#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // NITRO_MB_MB_H_
