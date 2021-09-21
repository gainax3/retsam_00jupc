/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_gameinfo.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

    $Log: mb_gameinfo.c,v $
    Revision 1.47  2007/08/09 08:27:44  takano_makoto
    MB_BC_MSG_GINFO_LOSTのイベントコールバックでも、bssDescとgameInfoの情報を有効とする

    Revision 1.46  2007/02/20 00:28:10  kitase_hirotake
    indent source

    Revision 1.45  2006/03/13 06:33:33  yosizaki
    add MB_FAKESCAN_PARENT_BEACON.

    Revision 1.44  2006/02/20 02:34:17  seiki_masashi
    WMGameInfo.version を廃止し、magicNumber と ver を追加
    WMGameInfo.gameNameCount_attribute を attribute に名前変更
    WM_GAMEINFO_TYPE_OLD のサポートを廃止

    Revision 1.43  2006/01/18 02:11:30  kitase_hirotake
    do-indent

    Revision 1.42  2005/11/18 05:31:12  yosizaki
    fix MBi_ReadyBeaconSendStatus().

    Revision 1.41  2005/11/14 05:37:28  yosizaki
    fix about WM_ATTR_FLAG_ENTRY flag.

    Revision 1.40  2005/03/04 12:35:05  yosiokat
    MB_DeleteGameInfoで、現在ビーコンでゲーム情報配信中のゲームが削除された場合、
    その後のゲーム配信が正常に動作しなくなる不具合を修正。

    Revision 1.39  2005/02/28 05:26:24  yosizaki
    do-indent.

    Revision 1.38  2005/02/21 00:39:34  yosizaki
    replace prefix MBw to MBi.

    Revision 1.37  2005/02/18 11:56:03  yosizaki
    fix around hidden warnings.

    Revision 1.36  2005/01/24 23:48:25  takano_makoto
    ASSERTの修正

    Revision 1.35  2005/01/18 05:31:03  takano_makoto
    gameIntroductionのNUL文字以降のデータも送信するように修正

    Revision 1.33  2005/01/17 09:29:06  takano_makoto
    MB_XXXUserBeaconData を MB_XXXUserVolatDataへ変更。MB_SetSendVolatCallback関数を追加。

    Revision 1.32  2005/01/17 06:26:34  takano_makoto
    MB_SetUserBeaconDataにおいて、データセット時の割り込みを禁止

    Revision 1.31  2005/01/17 05:54:29  takano_makoto
    MB_SetUserBeaconData, MB_GetUserBeaconDataを追加

    Revision 1.30  2005/01/11 07:41:13  takano_makoto
    fix copyright header.

    Revision 1.29  2004/11/22 12:43:44  takano_makoto
    MBw関数を使用しないように変更。MB_GetGameInfoRecvListを追加。

    Revision 1.28  2004/11/10 07:40:03  takano_makoto
    mb_gameinfo.cの中からMBw関数を排除し、スキャンパラメータの設定を切り出す

    Revision 1.27  2004/11/10 03:17:52  takano_makoto
    mbrsで確保していたバッファを外から渡してもらえるように修正

    Revision 1.26  2004/11/02 18:17:58  takano_makoto
    fix DEBUG Print.

    Revision 1.24  2004/10/28 11:01:42  yada
    just fix comment

    Revision 1.23  2004/10/05 09:45:29  terui
    MACアドレスをu8型の配列に統一。

    Revision 1.22  2004/10/04 13:40:25  terui
    ゲームグループIDをu32型に統一。

    Revision 1.21  2004/09/29 05:12:28  sato_masaki
    BssDescに、各ゲームのGGIDを渡せていなかった不具合修正。
    
    Revision 1.20  2004/09/28 12:49:48  yosiokat
    IPLブランチ1.14.2.7とマージ。
    
    Revision 1.14.2.7  2004/09/28 10:53:13  yosiokat
    ・MBi_SendVolatBeacon内でbeaconNoのインクリメントを行っていなかった不具合を修正。
    ・MB_SAME_BEACON_RECV_MAX_COUNTを"3"に変更。
    
    Revision 1.14.2.6  2004/09/28 02:42:11  yosiokat
    MBi_UnlockScanTargetで、アンロック実行時に次のロックターゲットを調べてロックする処理を削除。
    
    Revision 1.14.2.5  2004/09/28 02:40:25  yosiokat
    ・親機のビーコンに送信ごとにインクリメントするbeaconNoを追加する。
    ・子機でbeaconNoが動いていない親機のビーコンを連続で受信した場合、その親機情報は無効とする処理を追加。
    
    Revision 1.14.2.4  2004/09/21 08:31:05  yosiokat
    親機で、ゲーム情報を追加→削除→追加した時におかしくなる不具合を修正。
    
    Revision 1.19  2004/09/22 09:41:42  sato_masaki
    IPL_branch 1.14.2.4とマージ
    (親機で、ゲーム情報を追加→削除→追加した時におかしくなる不具合を修正。)
    
    Revision 1.18  2004/09/20 10:45:41  yosiokat
    IPLブランチRev.1.14.2.3とのマージ。
    
    Revision 1.14.2.3  2004/09/20 10:15:49  yosiokat
    ・__UGI_IN_GGIDを削除。ビーコン内には、常にggidが入っている形式にする。
    ・MBi_ClearFixedInfoを削除。
    ・MB_RecvGameInfoBeacon内でのSSIDの設定をMBi_SetSSIDToBssDesc関数に分離。
    ・MBi_GetStoreElement内で行っていたTGIDチェックをMBi_CheckTGIDとして、MBi_AnalyzeBeacon内で行うようにする。
    ・MBi_CheckSeqNoFixed内のシーケンス番号チェックを修正。（一部をMBi_RecvFixedBeaconに移動）
    ・MBi_CheckSeqNoFixed、MBi_CheckSeqNoVolatの返り値をvoidに。
    ・MBGameInfoRecvListからseqNoFixed, seqNoVolatを削除して、同gameInfo内にあるseqNoFixed, seqNoVolatを使用するよう修正。
    
    Revision 1.14.2.2  2004/09/18 08:41:04  miya
    HEADとマージ
    
    Revision 1.17  2004/09/17 04:00:38  miya
    ２つ目以降のゲーム配信情報が受け取れないバグを修正
    
    Revision 1.16  2004/09/16 12:20:42  miya
    ファイル更新用の改造
    
    Revision 1.15  2004/09/16 11:22:22  miya
    ファイル更新用メンバ追加
    
    Revision 1.14  2004/09/15 12:42:42  yosiokat
    プレイメンバーが削除された際のビーコン受信がうまくいっていなかったのを修正。
    
    Revision 1.13  2004/09/15 09:22:27  yosiokat
    mbss.seqNoVolatが、MBi_ReadyBeaconSendStatus内でゲームを切り替える時にセットされていなかった不具合を修正。
    
    Revision 1.12  2004/09/15 07:03:10  yosiokat
    ・usefulGameInfoFlagを設けて、一旦親機データ一式を受信したら、親機データの寿命が来るまで立ちつづけるようにする。
    ・毎ビーコンにseqNoFixedとseqNoVolatの両方を入れて、スキャンロックの判定を行うよう変更。
    
    Revision 1.11  2004/09/14 14:40:14  yosiokat
    small fix.
    
    Revision 1.10  2004/09/14 14:22:19  yosiokat
    ・MbBeacon内にチェックサムを挿入し、ビーコン受信時にチェックサム確認に通らないビーコンは捨てるよう変更。
    ・mb_gameinfo.c内でしか使わない定数をmb.hから移動。
    ・MB_SEND_MEMBER_MAX_NUMの定義を修正。
    
    Revision 1.9  2004/09/13 09:28:16  yosiokat
    ・スキャンロック時にロック対象が全く見つからない状態の対策のため、MB_CountGameInfoLifetimeの引数にwmTypeEventを追加。
    　スキャンロック動作時にロック対象が連続で４回見つからなかったら、ロック解除するようにする。
    
    Revision 1.8  2004/09/11 12:47:47  yosiokat
    ・スキャンロック時にスキャン時間を変更する処理に、ロック時間、親機情報寿命カウント処理が対応できていなかったのを修正。
    
    Revision 1.7  2004/09/10 01:31:06  sato_masaki
    PlayerFlagのフォーマット変更。（作業完了）
    
    Revision 1.6  2004/09/09 14:01:47  sato_masaki
    mb_gameinfo.cにおける、PlayerFlagのフォーマット変更。（作業中）
    
    Revision 1.5  2004/09/09 10:04:17  sato_masaki
    small fix
    
    Revision 1.4  2004/09/09 09:22:40  sato_masaki
    GGIDとTGIDのSSIDにセットする方法を変更。
    
    Revision 1.3  2004/09/08 13:01:30  sato_masaki
    スキャン対象のロック時、ロック解除時に、MBi_SetMaxScanTime()関数にて、
    Scan時間を指定する処理を追加。
    
    Revision 1.2  2004/09/06 12:00:38  sato_masaki
    -GameInfoへのfileNoの設定を、MB_RegisterFile()(mb_fileinfo.c)内で行うように変更したため、
     MB_AddGameInfo()から削除。
    -親機プログラムのGGIDはアプリケーションによって異なる(初期化時に設定される)ため、
     子機の方では、親機GGID情報をもつことができなくなるので、
     MBi_CheckMBParent()内の親機GGIDチェックを削除。
    -SetGameInfoを行うときのGGIDをWM初期化時に設定した親機固有のGGIDに変更。
    -MB_RecvGameInfoBeacon()にて、ダウンロードさせるゲームのGGIDと、親機のTGIDを用いて
     子機に渡すSSIDを生成するようにした。
    
    Revision 1.1  2004/09/03 07:04:36  sato_masaki
    ファイルを機能別に分割。
    
    Revision 1.15  2004/08/26 09:53:00  sato_masaki
    tgidを1に固定（SSIDチェックの対策）
    
    Revision 1.14  2004/08/26 04:54:08  sato_masaki
    OS_Printf is changed into MB_DEBUG_OUTPUT
    
    Revision 1.13  2004/08/19 22:55:25  yosiokat
    MB_Initでのマルチブートフラグ、GGIDのセットを止めて、ビーコン送信時のWM_SetGameInfoでセットするようにする。
    
    Revision 1.12  2004/08/19 16:00:53  miya
    modified WM_SetGameInfo function
    
    Revision 1.11  2004/08/19 14:32:12  yosiokat
    ・__UGI_IN_GGIDをオフに。
    ・MbBeacon構造体のfixed要素のflagmentNo, flagmentMaxNum, sizeをu8指定に変更。
    ・MBi_ExUTF16_LEtoSJIS_BEに改行コード0x0d,0x0aの変換を追加。
    
    Revision 1.10  2004/08/18 23:57:14  yosizaki
    fix warning (p_game on SDK_FINALROM)
    
    Revision 1.9  2004/08/18 13:52:04  yosizaki
    add conversion routine for Unicode<->SJIS.
    
    Revision 1.8  2004/08/18 09:54:23  miya

    
    Revision 1.7  2004/08/18 09:30:47  yosiokat
    ・bsendBuffのアラインメントを32byteに。
    
    Revision 1.6  2004/08/18 09:08:32  miya
    modified WMGameInfo format
    
    Revision 1.5  2004/08/12 09:54:50  yosiokat
    MBi_MakeGameInfoのアイコンデータ取得をファイルから読み出すよう変更。
    
    Revision 1.4  2004/08/12 09:17:53  yosiokat
    ・スキャンロックに期限を設けるよう変更。
    ・MB_DeleteRecvGameInfoの追加。
    
    Revision 1.3  2004/08/09 14:56:43  yosizaki
    format all the symbol for API
    
    Revision 1.2  2004/08/07 15:02:37  yosizaki
    merge all the diff. (-2004/08/07)
    
    Revision 1.1  2004/08/07 07:14:51  yosizaki
    (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include "mb_gameinfo.h"
#include "mb_common.h"
#include "mb_private.h"                // for MB_SCAN_TIME_NORMAL & MB_SCAN_TIME_LOKING & MB_OUTPUT, MB_DEBUG_OUTPUT


/*

　＊＊＊＊＊＊注意事項＊＊＊＊＊＊＊
　・スキャンロック中は、スキャン時間を親機のビーコン間隔以上にした方がいいとのこと。（200ms以上）
　　但し、これをする場合、動的にスキャン時間を変えることになるので、LifetimeCountやLockTimeCountの最大値
    や既にカウントされている値をどうするか？

*/


/*
 * 使用している wmDEMOlib 関数:
 *  ・無し
 *
 * 使用している WM API:
 *  ・WM_SetGameInfo (MBi_SendFixedBeacon, MBi_SendVolatBeacon)
 *
 *
 *
 *
 */

// define data---------------------------------------------

#define MB_GAME_INFO_LIFETIME_SEC       60      // 受信したゲーム情報の寿命（このsec値の間ビーコン受信がなければ削除される）
#define MB_SCAN_LOCK_SEC                8       // 特定親機をロックしてスキャンする最大時間
#define MB_SAME_BEACON_RECV_MAX_COUNT   3       // 親機が同じビーコンを出し続けていた場合、その親機のビーコンを受信し続ける最大カウント数。

#define MB_BEACON_DATA_SIZE         (WM_SIZE_USER_GAMEINFO - 8)
#define MB_BEACON_FIXED_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 6) // ビーコン内の親機ゲーム情報固定領域のデータサイズ
#define MB_BEACON_VOLAT_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 8) // ビーコン内の親機ゲーム情報流動領域のデータサイズ
#define MB_SEND_MEMBER_MAX_NUM      (MB_BEACON_VOLAT_DATA_SIZE / sizeof(MBUserInfo))    // １回のビーコンで送信できるメンバー情報の最大数

#define FIXED_NORMAL_SIZE            sizeof(MBGameInfoFixed)    // ゲーム情報固定領域のノーマルサイズ
#define FIXED_NO_ICON_SIZE          (sizeof(MBGameInfoFixed) - sizeof(MBIconInfo))      // ゲーム情報固定領域のアイコンなしサイズ
#define FIXED_FLAGMENT_MAX(size)    ( ( size / MB_BEACON_FIXED_DATA_SIZE) + ( size % MB_BEACON_FIXED_DATA_SIZE ? 1 : 0 ) )
                                                                                    // ゲーム情報固定領域をビーコン分割数

#define MB_LIFETIME_MAX_COUNT       ( MB_GAME_INFO_LIFETIME_SEC * 1000 / MB_SCAN_TIME_NORMAL + 1 )
#define MB_LOCKTIME_MAX_COUNT       ( MB_SCAN_LOCK_SEC          * 1000 / MB_SCAN_TIME_NORMAL + 1 )
                                                                                    // 親機ゲーム情報生存期間をScanTime単位に換算したもの
                                                                                    // スキャンロック時間　　をScanTime単位に換算したもの
#define MB_SCAN_COUNT_UNIT_NORMAL   ( 1 )
#define MB_SCAN_COUNT_UNIT_LOCKING  ( MB_SCAN_TIME_LOCKING / MB_SCAN_TIME_NORMAL )



typedef enum MbBeaconState
{
    MB_BEACON_STATE_STOP = 0,
    MB_BEACON_STATE_READY,
    MB_BEACON_STATE_FIXED_START,
    MB_BEACON_STATE_FIXED,
    MB_BEACON_STATE_VOLAT_START,
    MB_BEACON_STATE_VOLAT,
    MB_BEACON_STATE_NEXT_GAME
}
MbBeaconState;


/* ビーコンフォーマット構造体 */
typedef struct MbBeacon
{
    u32     ggid;                      // GGID
    u8      dataAttr:2;                // データ属性（MbBeaconDataAttrで指定）
    u8      fileNo:6;                  // ファイルNo.
    u8      seqNoFixed;                // Fixedデータシーケンス番号（データ内容に更新があった場合に加算される。）
    u8      seqNoVolat;                // Volatデータシーケンス番号（　〃　）
    u8      beaconNo;                  // ビーコンナンバー（ビーコン送信ごとにインクリメント）
    /* 元 MbBeaconData */
    union
    {
        struct
        {                              // MBGameInfoFixed送信時
            u16     sum;               // 16bit checksum
            u8      flagmentNo;        // 断片化データの現在番号
            u8      flagmentMaxNum;    // 断片化データの最大数
            u8      size;              // データサイズ
            u8      rsv;
            u8      data[MB_BEACON_FIXED_DATA_SIZE];    // データ実体
        }
        fixed;
        struct
        {                              // MBGameInfoVolatile送信時
            u16     sum;               // 16bit checksum
            u8      nowPlayerNum;      // 現在のプレイヤー数
            u8      pad[1];
            u16     nowPlayerFlag;     // 現在の全プレイヤーのプレイヤー番号をビットで示す。
            u16     changePlayerFlag;  // 今回の更新で変更されたプレイヤー情報の番号をビットで示す。（シーケンス番号が変わった瞬間にのみ判定に使用）
            MBUserInfo member[MB_SEND_MEMBER_MAX_NUM];  // 各子機のユーザー情報をMB_SEND_MEMBER_MAX_NUMつずつ送信（PlayerNo == 15（親機） なら終端。）
            u8      userVolatData[MB_USER_VOLAT_DATA_SIZE];     // ユーザがセットできるデータ
        }
        volat;
    }
    data;

}
MbBeacon;

/* 親機側ビーコン送信ステータス構造体 */
typedef struct MbBeaconSendStatus
{
    MBGameInfo *gameInfoListTop;       // ゲーム情報リスト先頭へのポインタ（片方向リスト）
    MBGameInfo *nowGameInfop;          // 現在送信中のゲーム情報へのポインタ
    u8     *srcp;                      // 現在の送信中のゲーム情報ソースへのポインタ
    u8      state;                     // ビーコン送信ステート（固定データ部、流動データ部送信。各データを全送信したらステート変更。）
    u8      seqNoFixed;                // 固定領域のシーケンス番号
    u8      seqNoVolat;                // 流動領域のシーケンス番号
    u8      flagmentNo;                // 　　〃　　　　　　のフラグメント番号（固定領域の場合）
    u8      flagmentMaxNum;            // 　　〃　　　　　　のフラグメント数　（〃）
    u8      beaconNo;
    u8      pad[2];
}
MbBeaconSendStatus;



// function's prototype------------------------------------
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag);
static void MBi_ClearSendStatus(void);
static BOOL MBi_ReadyBeaconSendStatus(void);
static void MBi_InitSendFixedBeacon(void);
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute);
static void MBi_InitSendVolatBeacon(void);
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute);

static BOOL MBi_CheckMBParent(WMBssDesc *bssDescp);
static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid);
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp);
static void MBi_CheckCompleteGameInfoFlagments(int index, MBBeaconMsgCallback Callbackp);
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel);
static void MBi_CheckTGID(WMBssDesc *bssDescp, int inex);
static void MBi_CheckSeqNoFixed(int index);
static void MBi_CheckSeqNoVolat(int index);
static void MBi_InvalidateGameInfoBssID(u8 *bssidp);
static void MBi_RecvFixedBeacon(int index);
static void MBi_RecvVolatBeacon(int index);

static void MBi_LockScanTarget(int index);
static void MBi_UnlockScanTarget(void);
static int mystrlen(u16 *str);


// const data----------------------------------------------

// global variables----------------------------------------

static MbBeaconSendStatus mbss;        // ビーコン送信ステータス

static MbBeaconRecvStatus mbrs;        // ビーコン受信ステータス
static MbBeaconRecvStatus *mbrsp = NULL;        // ビーコン受信ステータス

// static variables----------------------------------------
static MbScanLockFunc sLockFunc = NULL; // スキャンロック設定関数ポインタ
static MbScanUnlockFunc sUnlockFunc = NULL;     // スキャンロック解除用関数ポインタ

static MbBeacon bsendBuff ATTRIBUTE_ALIGN(32);  // ビーコン送信バッファ
static MbBeacon *brecvBuffp;           // ビーコン受信バッファ
static WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);        // BssDescのテンポラリバッファ

static MBSendVolatCallbackFunc sSendVolatCallback = NULL;       // ユーザデータ送信コールバック
static u32 sSendVolatCallbackTimming;  // 送信コールバック発生タイミング

// function's description-----------------------------------------------


//=========================================================
// ビーコン受信ワークバッファ操作
//=========================================================
// 設定されているワークバッファを取得
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void)
{
    return mbrsp;
}

// static変数からワークバッファを設定(旧互換用)
void MBi_SetBeaconRecvStatusBufferDefault(void)
{
    mbrsp = &mbrs;
}

// ビーコン受信ワークバッファの設定
void MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf)
{
    mbrsp = buf;
}


// スキャンロック用関数を設定
void MBi_SetScanLockFunc(MbScanLockFunc lock, MbScanUnlockFunc unlock)
{
    sLockFunc = lock;
    sUnlockFunc = unlock;
}



//=========================================================
// 親機によるゲーム情報のビーコン送信
//=========================================================

// ビーコンで送信する親機ゲーム情報の生成
void MBi_MakeGameInfo(MBGameInfo *gameInfop,
                      const MBGameRegistry *mbGameRegp, const MBUserInfo *parent)
{
    BOOL    icon_disable;

    // とりあえず全クリア
    MI_CpuClear16(gameInfop, sizeof(MBGameInfo));

    // アイコンデータの登録
    gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NORMAL;
    icon_disable = !MBi_ReadIconInfo(mbGameRegp->iconCharPathp, &gameInfop->fixed.icon, TRUE);
    icon_disable |= !MBi_ReadIconInfo(mbGameRegp->iconPalettePathp, &gameInfop->fixed.icon, FALSE);

    if (icon_disable)
    {
        gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NO_ICON;
        MI_CpuClearFast(&gameInfop->fixed.icon, sizeof(MBIconInfo));
    }

    // GGID
    gameInfop->ggid = mbGameRegp->ggid;

    // 親のユーザー情報登録
    if (parent != NULL)
    {
        MI_CpuCopy16(parent, &gameInfop->fixed.parent, sizeof(MBUserInfo));
    }

    // 最大プレイ人数登録
    gameInfop->fixed.maxPlayerNum = mbGameRegp->maxPlayerNum;

    // ゲーム名＆ゲーム内容説明登録
    {
        int     len;

#if defined(MB_CHANGE_TO_UNICODE)
#define COPY_GAME_INFO_STRING   MBi_ExSJIS_BEtoUTF16_LE
#else  /* defined(MB_CHANGE_TO_UNICODE) */
#define COPY_GAME_INFO_STRING   MI_CpuCopy16
#endif /* defined(MB_CHANGE_TO_UNICODE) */

        len = mystrlen(mbGameRegp->gameNamep) << 1;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameNamep, gameInfop->fixed.gameName, (u16)len);
        // ゲーム内容説明の後にデータを埋め込めるように、NUL文字以降のデータも送信する。
        len = MB_GAME_INTRO_LENGTH * 2;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameIntroductionp,
                              gameInfop->fixed.gameIntroduction, (u16)len);
    }

    // 親機分のプレイヤー情報を登録
    gameInfop->volat.nowPlayerNum = 1;
    gameInfop->volat.nowPlayerFlag = 0x0001;    // 親機のプレイヤー番号は0
    gameInfop->broadcastedPlayerFlag = 0x0001;

    // ※マルチブート開始時にはプレイメンバーはいないので、登録はなし。
}


// アイコンデータのリード
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag)
{
    FSFile  file;
    s32     size = char_flag ? MB_ICON_DATA_SIZE : MB_ICON_PALETTE_SIZE;
    u16    *dstp = char_flag ? iconp->data : iconp->palette;

    if (filePathp == NULL)
    {                                  // ファイル指定がなければFALSEリターン
        return FALSE;
    }

    FS_InitFile(&file);

    if (FS_OpenFile(&file, filePathp) == FALSE)
    {                                  // ファイルが開けなかったらFALSEリターン
        MB_DEBUG_OUTPUT("\t%s : file open error.\n", filePathp);
        return FALSE;
    }
    else if ((u32)size != FS_GetLength(&file))
    {
        MB_DEBUG_OUTPUT("\t%s : different file size.\n", filePathp);
        (void)FS_CloseFile(&file);
        return FALSE;
    }

    (void)FS_ReadFile(&file, dstp, size);
    (void)FS_CloseFile(&file);
    return TRUE;
}


// ゲーム情報の流動部分を更新する。
void MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                             const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag)
{
    int     i;
    u8      playerNum = 1;

    MI_CpuCopy16(member, &gameInfop->volat.member[0], sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
    /* 子機数をカウント */
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if (nowPlayerFlag & (0x0002 << i))
        {
            playerNum++;
        }
    }
    gameInfop->volat.nowPlayerNum = playerNum;
    gameInfop->volat.nowPlayerFlag = (u16)(nowPlayerFlag | 0x0001);
    gameInfop->volat.changePlayerFlag = changePlayerFlag;
    gameInfop->seqNoVolat++;
}


// 文字列長の算出
static int mystrlen(u16 *str)
{
    int     len = 0;
    while (*str++)
        len++;
    return len;
}


// ゲーム情報を送信リストに追加
void MB_AddGameInfo(MBGameInfo *newGameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;

    if (!gInfop)
    {                                  // mbssの先頭がNULLなら先頭に登録。
        mbss.gameInfoListTop = newGameInfop;
    }
    else
    {                                  // さもなくば、リストを辿って行き、最後に追加。
        while (gInfop->nextp != NULL)
        {
            gInfop = gInfop->nextp;
        }
        gInfop->nextp = newGameInfop;
    }
    newGameInfop->nextp = NULL;        // 追加したゲーム情報のnextを終端とする。
}


// ゲーム情報を送信リストから削除
BOOL MB_DeleteGameInfo(MBGameInfo *gameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;
    MBGameInfo *before;

    while (gInfop != NULL)
    {
        if (gInfop != gameInfop)
        {
            before = gInfop;
            gInfop = gInfop->nextp;
            continue;
        }

        // 一致したのでリストから削除
        if ((u32)gInfop == (u32)mbss.gameInfoListTop)
        {                              // リスト先頭を削除する場合
            mbss.gameInfoListTop = mbss.gameInfoListTop->nextp;
        }
        else
        {
            before->nextp = gInfop->nextp;
        }

        if ((u32)gameInfop == (u32)mbss.nowGameInfop)   // 削除するゲーム情報が現在送信中の場合、現在送信中の
        {                              // ゲーム情報を再構築されたリストから再設定する。
            mbss.nowGameInfop = NULL;
            if (!MBi_ReadyBeaconSendStatus())
            {
                mbss.state = MB_BEACON_STATE_READY;     // ゲーム情報が全て削除されていたならば、ステータスをREADYに。
            }
        }
        return TRUE;
    }

    // ゲーム情報がない場合
    return FALSE;
}


// ゲーム情報送信設定の初期化
void MB_InitSendGameInfoStatus(void)
{
    mbss.gameInfoListTop = NULL;       // ゲーム情報リストを全削除。
    mbss.nowGameInfop = NULL;          // 現送信ゲームも削除。
    mbss.state = MB_BEACON_STATE_READY;
    sSendVolatCallback = NULL;
    MBi_ClearSendStatus();             // 送信ステータスもクリア。
}


// 送信ステータスのクリア
static void MBi_ClearSendStatus(void)
{
    mbss.seqNoFixed = 0;
    mbss.seqNoVolat = 0;
    mbss.flagmentNo = 0;
    mbss.flagmentMaxNum = 0;
    mbss.beaconNo = 0;
}


// ビーコン送信
void MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    while (1)
    {
        switch (mbss.state)
        {
        case MB_BEACON_STATE_STOP:
        case MB_BEACON_STATE_READY:
            if (!MBi_ReadyBeaconSendStatus())
            {                          // ゲーム情報の送信準備
                return;
            }
            break;
        case MB_BEACON_STATE_FIXED_START:
            MBi_InitSendFixedBeacon();
            break;
        case MB_BEACON_STATE_FIXED:
            MBi_SendFixedBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_VOLAT_START:
            MBi_InitSendVolatBeacon();
            break;
        case MB_BEACON_STATE_VOLAT:
            MBi_SendVolatBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_NEXT_GAME:
            break;
        }
    }
}


// ビーコン送信ステータスを送信可能に準備する。
static BOOL MBi_ReadyBeaconSendStatus(void)
{
    MBGameInfo *nextGameInfop;

    // ゲーム情報が登録されていないなら、エラーリターン。
    if (!mbss.gameInfoListTop)
    {
        /* MB フラグと ENTRY フラグはここで落とす */
        (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, MBi_GetGgid(),
                             MBi_GetTgid(), WM_ATTR_FLAG_CS);
        return FALSE;
    }

    // 次に送信するGameInfoの選択
    if (!mbss.nowGameInfop || !mbss.nowGameInfop->nextp)
    {
        // まだ送信中のゲーム情報がないか、リストの最後の場合は、リストの先頭を送信するよう準備。
        nextGameInfop = mbss.gameInfoListTop;
    }
    else
    {
        // さもなくば、リストの次のゲームを送信するよう準備。
        nextGameInfop = mbss.nowGameInfop->nextp;
    }

    mbss.nowGameInfop = nextGameInfop;

    MBi_ClearSendStatus();
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;

    mbss.state = MB_BEACON_STATE_FIXED_START;

    return TRUE;
}


// ゲーム情報の固定データ部の送信初期化
static void MBi_InitSendFixedBeacon(void)
{
    if (mbss.state != MB_BEACON_STATE_FIXED_START)
    {
        return;
    }

    if (mbss.nowGameInfop->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        mbss.flagmentMaxNum = FIXED_FLAGMENT_MAX(FIXED_NORMAL_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed;
    }
    else
    {
        mbss.flagmentMaxNum = FIXED_FLAGMENT_MAX(FIXED_NO_ICON_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed.parent;
    }
    mbss.state = MB_BEACON_STATE_FIXED;
}


// ゲーム情報の固定データ部を分割してビーコン送信
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    u32     lastAddr = (u32)mbss.nowGameInfop + sizeof(MBGameInfoFixed);

    if ((u32)mbss.srcp + MB_BEACON_FIXED_DATA_SIZE <= lastAddr)
    {
        bsendBuff.data.fixed.size = MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        bsendBuff.data.fixed.size = (u8)(lastAddr - (u32)mbss.srcp);
        MI_CpuClear16((void *)((u8 *)bsendBuff.data.fixed.data + bsendBuff.data.fixed.size),
                      (u32)(MB_BEACON_FIXED_DATA_SIZE - bsendBuff.data.fixed.size));
    }

    // ビーコン送信バッファにセット
    MB_DEBUG_OUTPUT("send flagment= %2d  adr = 0x%x\n", mbss.flagmentNo, mbss.srcp);
    MI_CpuCopy16(mbss.srcp, bsendBuff.data.fixed.data, bsendBuff.data.fixed.size);
    bsendBuff.data.fixed.flagmentNo = mbss.flagmentNo;
    bsendBuff.data.fixed.flagmentMaxNum = mbss.flagmentMaxNum;
    bsendBuff.dataAttr = mbss.nowGameInfop->dataAttr;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;
    bsendBuff.data.fixed.sum = 0;
    bsendBuff.data.fixed.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);

    // 送信ステータスの更新
    mbss.flagmentNo++;
    if (mbss.flagmentNo < mbss.flagmentMaxNum)
    {
        mbss.srcp += MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        mbss.state = MB_BEACON_STATE_VOLAT_START;
    }

    /* 親機プログラム自身ののGGIDを登録する */
    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // マルチブートフラグはここで立てる。
}


// ゲーム情報の流動データ部の送信初期化
static void MBi_InitSendVolatBeacon(void)
{
    mbss.nowGameInfop->broadcastedPlayerFlag = 0x0001;
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;
    mbss.state = MB_BEACON_STATE_VOLAT;
}


// ゲーム情報の流動データ部を分割してビーコン送信
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    int     i;
    int     setPlayerNum;
    u16     remainPlayerFlag;

    if (mbss.seqNoVolat != mbss.nowGameInfop->seqNoVolat)
    {                                  // 送信中にデータ更新があったら送り直し。
        MBi_InitSendVolatBeacon();
    }

    // ビーコン情報をセット
    bsendBuff.dataAttr = MB_BEACON_DATA_ATTR_VOLAT;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;

    // 現在のプレイヤー情報の状態をセット
    bsendBuff.data.volat.nowPlayerNum = mbss.nowGameInfop->volat.nowPlayerNum;
    bsendBuff.data.volat.nowPlayerFlag = mbss.nowGameInfop->volat.nowPlayerFlag;
    bsendBuff.data.volat.changePlayerFlag = mbss.nowGameInfop->volat.changePlayerFlag;

    // アプリ設定データをセット
    if (sSendVolatCallbackTimming == MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        bsendBuff.data.volat.userVolatData[i] = mbss.nowGameInfop->volat.userVolatData[i];
    }

    MB_DEBUG_OUTPUT("send PlayerFlag = %x\n", mbss.nowGameInfop->volat.nowPlayerFlag);

    // 今回送信するプレイヤー情報のセット
    MI_CpuClear16(&bsendBuff.data.volat.member[0], sizeof(MBUserInfo) * MB_SEND_MEMBER_MAX_NUM);
    setPlayerNum = 0;
    remainPlayerFlag =
        (u16)(mbss.nowGameInfop->broadcastedPlayerFlag ^ mbss.nowGameInfop->volat.nowPlayerFlag);
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if ((remainPlayerFlag & (0x0002 << i)) == 0)
        {
            continue;
        }

        MB_DEBUG_OUTPUT("  member %d set.\n", i);

        MI_CpuCopy16(&mbss.nowGameInfop->volat.member[i],
                     &bsendBuff.data.volat.member[setPlayerNum], sizeof(MBUserInfo));
        mbss.nowGameInfop->broadcastedPlayerFlag |= 0x0002 << i;
        if (++setPlayerNum == MB_SEND_MEMBER_MAX_NUM)
        {
            break;
        }
    }
    if (setPlayerNum < MB_SEND_MEMBER_MAX_NUM)
    {                                  // 最大送信数に達しなかった時には終端する。
        bsendBuff.data.volat.member[setPlayerNum].playerNo = 0;
    }

    // チェックサムのセット
    bsendBuff.data.volat.sum = 0;
    bsendBuff.data.volat.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);


    // 送信終了チェック
    if (mbss.nowGameInfop->broadcastedPlayerFlag == mbss.nowGameInfop->volat.nowPlayerFlag)
    {
        mbss.state = MB_BEACON_STATE_READY;     // 全情報を送信し終わったら、次のゲーム情報を送信するための準備ステートへ。
    }

    /* 親機プログラム自身ののGGIDを登録する */

    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // マルチブートフラグはここで立てる。

    if (sSendVolatCallbackTimming == MB_SEND_VOLAT_CALLBACK_TIMMING_AFTER
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

}


//=========================================================
// 子機によるゲーム情報のビーコン受信
//=========================================================

// ゲーム情報受信ステータスの初期化
void MB_InitRecvGameInfoStatus(void)
{
    MI_CpuClearFast(mbrsp, sizeof(MbBeaconRecvStatus));

    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
}


// ビーコン受信
BOOL MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp)
{
    int     index;

    // 今回取得したビーコンが、マルチブート親機かどうかを判定。
    if (!MBi_CheckMBParent(bssDescp))
    {
        return FALSE;
    }

    /* 取得したbssDescをテンポラリバッファへコピー */
    MI_CpuCopy16(bssDescp, &bssDescbuf, sizeof(WMBssDesc));

    /* bssDescpをローカルバッファに切り替える。 */
    bssDescp = &bssDescbuf;

    brecvBuffp = (MbBeacon *) bssDescp->gameInfo.userGameInfo;

    // チェックサム確認。
    if (MBi_calc_cksum((u16 *)&brecvBuffp->data, MB_BEACON_DATA_SIZE))
    {
        MB_DEBUG_OUTPUT("Beacon checksum error!\n");
        return FALSE;
    }

    // 親機ゲーム情報の格納場所を判定（既に同一親機のデータを受信しているなら、そこに続けて格納する）
    index = MBi_GetStoreElement(bssDescp, Callbackp);
    if (index < 0)
    {
        return FALSE;                  // 格納場所がないのでエラーリターン
    }
    MB_DEBUG_OUTPUT("GameInfo Index:%6d\n", index);

    // ビーコン解析
    MBi_AnalyzeBeacon(bssDescp, index, linkLevel);

    // 親機ゲーム情報の全ての断片が揃ったか判定し、コールバックで通知
    MBi_CheckCompleteGameInfoFlagments(index, Callbackp);

    return TRUE;
}


// 今回取得したビーコンがマルチブート親機かどうかを判定する。
static BOOL MBi_CheckMBParent(WMBssDesc *bssDescp)
{
    // マルチブート親機かどうかを判定する。
    if ((bssDescp->gameInfo.magicNumber != WM_GAMEINFO_MAGIC_NUMBER)
        || !(bssDescp->gameInfo.attribute & WM_ATTR_FLAG_MB))
    {
        MB_DEBUG_OUTPUT("not MB parent : %x%x\n",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return FALSE;
    }
    else
    {

        MB_DEBUG_OUTPUT("MB parent     : %x%x",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return TRUE;
    }
}


// bssDescにSSIDをセットする。
static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid)
{
    /* 
       SSIDの設定 

       ダウンロードアプリ固有のGGIDと、マルチブート親機のTGIDから
       SSIDを生成する。
       子機は、このSSIDを用いて親機アプリケーションと再接続を行う。
     */
    bssDescp->ssidLength = 32;
    ((u16 *)bssDescp->ssid)[0] = (u16)(ggid & 0x0000ffff);
    ((u16 *)bssDescp->ssid)[1] = (u16)((ggid & 0xffff0000) >> 16);
    ((u16 *)bssDescp->ssid)[2] = bssDescp->gameInfo.tgid;
}


// bssDescをもとに受信リスト要素のどこに格納するかを取得する。
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp)
{
    int     i;

    // 既にこの親機の同一ゲーム情報を受信しているかどうかを判定（ "GGID", "BSSID", "fileNo"の３つが一致するなら、同一ゲーム情報と判断する）
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];

        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }
        // GGIDが一致するか？
        if (info->gameInfo.ggid != brecvBuffp->ggid)
        {
            continue;
        }
        // MACアドレスが一致するか？
        if (!WM_IsBssidEqual(info->bssDesc.bssid, bssDescp->bssid))
        {
            continue;
        }
        // ファイルNo.が一致するか？
        if (mbrsp->list[i].gameInfo.fileNo != brecvBuffp->fileNo)
        {
            continue;
        }

        // =========================================
        // この親機情報に対して、既に受信場所が確保されていると判断。
        // =========================================
        if (!(mbrsp->validGameInfoFlag & (0x01 << i)))
        {
            MBi_LockScanTarget(i);     // まだ該当親機の情報が揃っていなければ、スキャンロックする。
        }
        return i;
    }

    // まだ受信していないので、リストのNULL位置を探して、そこを格納場所にする。
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if (mbrsp->usingGameInfoFlag & (0x01 << i))
        {
            continue;
        }

        MI_CpuCopy16(bssDescp, &mbrsp->list[i].bssDesc, sizeof(WMBssDesc));
        // BssDescをコピー
        mbrsp->list[i].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
        mbrsp->usingGameInfoFlag |= (u16)(0x01 << i);

        MB_DEBUG_OUTPUT("\n");
        // この親機のみをScan対象にロックする。
        MBi_LockScanTarget(i);
        return i;
    }

    // 格納場所が全て埋まっていた場合はコールバックで通知してエラーリターン
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_LIST_FULL, NULL, 0);
    }
    return -1;
}


// 親機ゲーム情報の全ての断片が揃ったか判定し、コールバックで通知
static void MBi_CheckCompleteGameInfoFlagments(int index, MBBeaconMsgCallback Callbackp)
{
    MBGameInfoRecvList *info = &mbrsp->list[index];

    /* いずれにせよアプリケーションへ親機発見を通知する */
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_BEACON, info, index);
    }
    if ((info->getFlagmentFlag == info->allFlagmentFlag) && (info->getFlagmentFlag)     // 新しく親機ゲーム情報が揃った場合
        && (info->gameInfo.volat.nowPlayerFlag)
        && (info->getPlayerFlag == info->gameInfo.volat.nowPlayerFlag))
    {
        if (mbrsp->validGameInfoFlag & (0x01 << index))
        {
            return;
        }
        mbrsp->validGameInfoFlag |= 0x01 << index;
        mbrsp->usefulGameInfoFlag |= 0x01 << index;
        MBi_UnlockScanTarget();        // Scan対象のロックを解除する。
        MB_DEBUG_OUTPUT("validated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_VALIDATED, info, index);
        }
    }
    else
    {                                  // 既に取得していた親機ゲーム情報が更新されて、不完全な状態になった場合
        if ((mbrsp->validGameInfoFlag & (0x01 << index)) == 0)
        {
            return;
        }
        mbrsp->validGameInfoFlag ^= (0x01 << index);
        MB_DEBUG_OUTPUT("Invaldated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_INVALIDATED, info, index);
        }
    }
}


// 親機のゲーム情報の寿命カウント（ついでにスキャンロック時間のカウントも行う）
void MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent)
{
    int     i;
    BOOL    unlock = FALSE;

    // ゲーム情報受信リストの寿命判定　＆　新規ロック対象ターゲットの有無を確認
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];
        u16     mask = (u16)(0x0001 << i);
        if ((mbrsp->usingGameInfoFlag & mask) == 0)
        {
            continue;
        }
        // 寿命判定
        info->lifetimeCount -= mbrsp->scanCountUnit;
        if (info->lifetimeCount >= 0)
        {
            continue;
        }
        info->lifetimeCount = 0;
        if (mbrsp->validGameInfoFlag & mask)
        {
            if (Callbackp != NULL)
            {
                Callbackp(MB_BC_MSG_GINFO_LOST, info, i);
            }
        }
        if (mbrsp->nowScanTargetFlag & mask)
        {
            unlock = TRUE;
        }
        mbrsp->usingGameInfoFlag &= ~mask;
        MB_DeleteRecvGameInfo(i);      // コールバック通知後にゲーム情報を削除する。
        MB_DEBUG_OUTPUT("gameInfo %2d : lifetime end.\n", i);
    }

    // スキャンロック時間のカウント
    if (mbrsp->nowScanTargetFlag && mbrsp->nowLockTimeCount > 0)
    {
        mbrsp->nowLockTimeCount -= mbrsp->scanCountUnit;        // スキャンロック時間がタイムアウトしたら、ロックを解除して次のロック対象を探す。
        if (mbrsp->nowLockTimeCount < 0)
        {
            MB_DEBUG_OUTPUT("scan lock time up!\n");
            unlock = TRUE;
        }
        else if (!found_parent)
        {
            if (++mbrsp->notFoundLockTargetCount > 4)
            {
                MB_DEBUG_OUTPUT("scan lock target not found!\n");
                unlock = TRUE;
            }
        }
        else
        {
            mbrsp->notFoundLockTargetCount = 0;
        }
    }

    // スキャンアンロック処理。
    if (unlock)
    {
        mbrsp->nowLockTimeCount = 0;
        MBi_UnlockScanTarget();
    }
}


// ビーコン解析
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel)
{
    MBi_CheckTGID(bssDescp, index);    // TGIDのチェック
    MBi_CheckSeqNoFixed(index);        // seqNoFixedのチェック
    MBi_CheckSeqNoVolat(index);        // seqNoVolatのチェック

    // 受信ビーコンの共通部分のデータ取得
    {
        MBGameInfoRecvList *info = &mbrsp->list[index];

        // 同じビーコンを出し続ける異常な状態の親機を除外する。
        if (info->beaconNo == brecvBuffp->beaconNo)
        {
            if (++info->sameBeaconRecvCount > MB_SAME_BEACON_RECV_MAX_COUNT)
            {
                info->lifetimeCount = 0;        // 親機情報の寿命をゼロにして、削除させる。
                MB_OUTPUT("The parent broadcast same beacon.: %d\n", index);
                MBi_InvalidateGameInfoBssID(&info->bssDesc.bssid[0]);
                return;                // この親機が他にもゲーム情報を配信していたら、それも削除させる。
            }
        }
        else
        {
            info->sameBeaconRecvCount = 0;
        }
        // 正常な状態の親機のデータ受信
        info->beaconNo = brecvBuffp->beaconNo;
        info->lifetimeCount = MB_LIFETIME_MAX_COUNT;    // 親機情報の延命。
        info->gameInfo.ggid = brecvBuffp->ggid; // ggid    の取得。
        info->gameInfo.fileNo = brecvBuffp->fileNo;     // fileNo  の取得。
        info->linkLevel = linkLevel;   // 電波強度の取得。
        // bssDescにSSIDをセットする。
        MBi_SetSSIDToBssDesc(&info->bssDesc, info->gameInfo.ggid);
    }

    // 受信ビーコンのデータ種類毎のデータ取得
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_VOLAT)
    {
        MBi_RecvVolatBeacon(index);
    }
    else
    {
        MBi_RecvFixedBeacon(index);
    }
}


// tgidのチェック
static void MBi_CheckTGID(WMBssDesc *bssDescp, int index)
{
    if (mbrsp->list[index].bssDesc.gameInfo.tgid == bssDescp->gameInfo.tgid)
    {
        return;
    }

    // tgidがアップしていたら、その親機は立ち上げ直されているとみなして、全データをクリアして取り直す。
    MB_DEBUG_OUTPUT("\ntgid updated! : %x%x", *(u16 *)(&bssDescp->bssid[4]),
                    *(u32 *)bssDescp->bssid);
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MI_CpuCopy16(bssDescp, &mbrsp->list[index].bssDesc, sizeof(WMBssDesc));
    // tgid更新の場合は、新しいbssDescをコピーする。
    MBi_LockScanTarget(index);         // 該当親機をスキャンロックする。
}


// FixedデータのSeqNoチェック
static void MBi_CheckSeqNoFixed(int index)
{
    // シーケンス番号が更新されていた場合の対応
    if (mbrsp->list[index].gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        return;
    }
    // シーケンス番号が更新されていたら、これまでに受信したデータをクリアする。
    MB_DEBUG_OUTPUT("\n seqNoFixed updated!");
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MBi_LockScanTarget(index);         // 該当親機をスキャンロックする。
    mbrsp->list[index].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
}


// VolatデータのSeqNoチェック
static void MBi_CheckSeqNoVolat(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    // シーケンス番号が更新されていた場合の対応
    if (mbrsp->list[index].gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        MB_DEBUG_OUTPUT("\n seqNoVolat updated!");
        MBi_LockScanTarget(index);     // スキャンロック可能か調べてロックする。
    }
}


// 対象BSSIDの親機ゲーム情報を全て無効にする。
static void MBi_InvalidateGameInfoBssID(u8 *bssidp)
{
    int     i;
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }

        if (!WM_IsBssidEqual(bssidp, mbrsp->list[i].bssDesc.bssid))
        {
            continue;
        }

        // 削除対象のデータと判定。
        mbrsp->list[i].lifetimeCount = 0;       // 親機情報の寿命をゼロにして、削除させる。
        MB_OUTPUT("The parent broadcast same beacon.: %d\n", i);
    }
}


// ゲーム情報の固定データ部を分割してビーコン受信
static void MBi_RecvFixedBeacon(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];
    u32     lastAddr = (u32)&grecvp->gameInfo + sizeof(MBGameInfoFixed);
    u8     *dstp;

    // シーケンス番号が更新されておらず、かつ既に取得済みのビーコンなら受信しない。
    if (grecvp->gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        if (grecvp->getFlagmentFlag & (0x01 << brecvBuffp->data.fixed.flagmentNo))
        {
            return;
        }
    }

    // 受信ビーコンが受信バッファをオーバーしないかチェック。
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed;
    }
    else
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed.parent;
    }
    dstp += MB_BEACON_FIXED_DATA_SIZE * brecvBuffp->data.fixed.flagmentNo;
    // ゲーム情報バッファの受信アドレスを算出。

    if ((u32)dstp + brecvBuffp->data.fixed.size > lastAddr)
    {
        MB_DEBUG_OUTPUT("recv beacon gInfoFixed Buffer over!\n");
        // バッファをオーバーするビーコンデータは無視する。
        return;
    }

    // 受信ビーコンを対象ゲーム情報バッファにセット
    MB_DEBUG_OUTPUT("recv flagment= %2d  adr = 0x%x", brecvBuffp->data.fixed.flagmentNo, dstp);
    MI_CpuCopy16(brecvBuffp->data.fixed.data, dstp, brecvBuffp->data.fixed.size);
    grecvp->gameInfo.dataAttr = brecvBuffp->dataAttr;
    grecvp->getFlagmentFlag |= 0x01 << brecvBuffp->data.fixed.flagmentNo;
    grecvp->allFlagmentFlag = (u32)((0x01 << brecvBuffp->data.fixed.flagmentMaxNum) - 1);
    MB_DEBUG_OUTPUT("\t now flagment = 0x%x \t all flagment = 0x%x\n",
                    grecvp->getFlagmentFlag, grecvp->allFlagmentFlag);
}


// ゲーム情報の流動データ部を分割してビーコン受信
static void MBi_RecvVolatBeacon(int index)
{
    int     i;
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    /* ユーザ定義データは常に受信する */
    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        grecvp->gameInfo.volat.userVolatData[i] = brecvBuffp->data.volat.userVolatData[i];
    }
    MI_CpuCopy16(brecvBuffp, &grecvp->bssDesc.gameInfo.userGameInfo, WM_SIZE_USER_GAMEINFO);

    // メンバー情報更新を検出した際の処理
    if (grecvp->gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        if ((u8)(grecvp->gameInfo.seqNoVolat + 1) == brecvBuffp->seqNoVolat)
        {                              // seqNoVolatが１つずれなら、変更されていないメンバー情報の引継ぎ
            for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
            {
                if (brecvBuffp->data.volat.changePlayerFlag & (0x02 << i))
                {
                    MI_CpuClear16(&grecvp->gameInfo.volat.member[i], sizeof(MBUserInfo));
                }
            }
            grecvp->getPlayerFlag &= ~brecvBuffp->data.volat.changePlayerFlag;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        else
        {                              // seqNoVolatがもっとずれていたら、これまで受信した全メンバー情報をクリア
            MI_CpuClear16(&grecvp->gameInfo.volat.member[0],
                          sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
            grecvp->getPlayerFlag = 0;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        grecvp->gameInfo.seqNoVolat = brecvBuffp->seqNoVolat;
    }
    else if (grecvp->getPlayerFlag == brecvBuffp->data.volat.nowPlayerFlag)
    {
        return;                        // シーケンス番号が更新されておらず、かつ既に取得済みのビーコンなら受信しない。
    }

    // プレイヤー情報の読み出し
    grecvp->gameInfo.volat.nowPlayerNum = brecvBuffp->data.volat.nowPlayerNum;
    grecvp->gameInfo.volat.nowPlayerFlag = brecvBuffp->data.volat.nowPlayerFlag;
    grecvp->gameInfo.volat.changePlayerFlag = brecvBuffp->data.volat.changePlayerFlag;
    grecvp->getPlayerFlag |= 0x0001;

    // 各メンバーのユーザー情報の読み出し
    for (i = 0; i < MB_SEND_MEMBER_MAX_NUM; i++)
    {
        int     playerNo = (int)brecvBuffp->data.volat.member[i].playerNo;
        if (playerNo == 0)
        {
            continue;
        }
        MB_DEBUG_OUTPUT("member %d recv.\n", playerNo);
        MI_CpuCopy16(&brecvBuffp->data.volat.member[i],
                     &grecvp->gameInfo.volat.member[playerNo - 1], sizeof(MBUserInfo));
        grecvp->getPlayerFlag |= 0x01 << playerNo;
    }
}


// スキャン対象を単一親機にロックする
static void MBi_LockScanTarget(int index)
{
    /* 既にロック中のターゲットがいるなら無視 */
    if (mbrsp->nowScanTargetFlag)
    {
        return;
    }

    if (sLockFunc != NULL)
    {
        sLockFunc(mbrsp->list[index].bssDesc.bssid);
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_LOCKING;

    mbrsp->nowScanTargetFlag = (u16)(0x01 << index);    // 新しいスキャンロック対象をビットで示す。
    mbrsp->nowLockTimeCount = MB_LOCKTIME_MAX_COUNT;    // スキャンロック時間を初期化
    MB_DEBUG_OUTPUT("scan target locked. : %x %x %x %x %x %x\n",
                    mbrsp->list[index].bssDesc.bssid[0],
                    mbrsp->list[index].bssDesc.bssid[1],
                    mbrsp->list[index].bssDesc.bssid[2],
                    mbrsp->list[index].bssDesc.bssid[3],
                    mbrsp->list[index].bssDesc.bssid[4], mbrsp->list[index].bssDesc.bssid[5]);
}


// スキャン対象のロックを解除する。
static void MBi_UnlockScanTarget(void)
{
    if (mbrsp->nowScanTargetFlag == 0)
    {
        return;
    }

    if (sUnlockFunc != NULL)
    {
        sUnlockFunc();
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
    mbrsp->nowScanTargetFlag = 0;
    mbrsp->notFoundLockTargetCount = 0;

    MB_DEBUG_OUTPUT(" unlock target\n");
}


// 受信したゲーム情報を完全に削除（ゲーム情報有効フラグも削除）
void MB_DeleteRecvGameInfo(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    MI_CpuClear16(&mbrsp->list[index], sizeof(MBGameInfoRecvList));
}


// 受信したゲーム情報をbssDescを除いて削除する。
void MB_DeleteRecvGameInfoWithoutBssdesc(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    mbrsp->list[index].getFlagmentFlag = 0;
    mbrsp->list[index].allFlagmentFlag = 0;
    mbrsp->list[index].getPlayerFlag = 0;
    mbrsp->list[index].linkLevel = 0;
    MI_CpuClear16(&(mbrsp->list[index].gameInfo), sizeof(MBGameInfo));
}

// 受信した親機情報構造体へのポインタを取得する
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index)
{
    // 有効なデータがなければNULLを返す
    if ((mbrsp->usefulGameInfoFlag & (0x01 << index)) == 0)
    {
        return NULL;
    }

    return &mbrsp->list[index];
}


//=========================================================
// ユーザVolatileデータの設定
//=========================================================

/*---------------------------------------------------------------------------*
  Name:         MB_SetSendVolatileCallback

  Description:  マルチブートのビーコン送信コールバックを設定します。
  
  Arguments:    callback    送信完了のコールバック関数です。
                            データが送信される度にコールバックが呼び出されます。
                timming     コールバック発生タイミング。
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetSendVolatCallback(MBSendVolatCallbackFunc callback, u32 timming)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    sSendVolatCallback = callback;
    sSendVolatCallbackTimming = timming;

    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         MB_SetUserVolatData

  Description:  マルチブートのビーコン中の空き領域にユーザデータを設定します。
  
  Arguments:    ggid        MB_RegisterFileで設定したプログラムのggidを指定して、
                            このファイルのビーコンにユーザデータを付加します。
                userData    設定するユーザデータへのポインタ。
                size        設定するユーザデータのサイズ (最大8バイトまで)
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetUserVolatData(u32 ggid, const u8 *userData, u32 size)
{
    MBGameInfo *gameInfo;

    SDK_ASSERT(size <= MB_USER_VOLAT_DATA_SIZE);
    SDK_NULL_ASSERT(userData);

    gameInfo = mbss.gameInfoListTop;
    if (gameInfo == NULL)
    {
        return;
    }

    while (gameInfo->ggid != ggid)
    {
        if (gameInfo == NULL)
        {
            return;
        }
        gameInfo = gameInfo->nextp;
    }

    {
        u32     i;

        OSIntrMode enabled = OS_DisableInterrupts();

        for (i = 0; i < size; i++)
        {
            gameInfo->volat.userVolatData[i] = userData[i];
        }

        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetUserVolatData

  Description:  ビーコン中の空き領域にあるユーザデータを取得します。
  
  Arguments:    gameInfo    Scan時に取得するgameInfoパラメータへのポインタ。
  
  Returns:      ユーザデータへのポインタ.
 *---------------------------------------------------------------------------*/
void   *MB_GetUserVolatData(const WMGameInfo *gameInfo)
{
    MbBeacon *beacon;

    SDK_NULL_ASSERT(gameInfo);

    if (!(gameInfo->attribute & WM_ATTR_FLAG_MB))
    {
        return NULL;
    }

    beacon = (MbBeacon *) (gameInfo->userGameInfo);

    if (beacon->dataAttr != MB_BEACON_DATA_ATTR_VOLAT)
    {
        return NULL;
    }

    return beacon->data.volat.userVolatData;
}








#if defined(MB_CHANGE_TO_UNICODE)

/******************************************************************************/
/* Unicode - SJIS */

/* 定義が無かったので応急で用意 */
#define SJIS_HIGHER_CODE1_MIN   0x81
#define SJIS_HIGHER_CODE1_MAX   0x9F
#define SJIS_HIGHER_CODE2_MIN   0xE0
#define SJIS_HIGHER_CODE2_MAX   0xFC

// define data------------------------------------------
#define TBL8140_ELEM_NUM                0xbd    // 8140tblの要素数
#define TBL849f_ELEM_NUM                0x20    // 849ftblの要素数

typedef struct SjisUtf16Pare
{
    u16     sjis;
    u16     unicode;
}
SjisUtf16Pare;

// extern data------------------------------------------

// function's prototype declaration---------------------
static u16 SearchUnicodeTable(u16 unicode, SjisUtf16Pare * tblp, int elem_num);

// global variable -------------------------------------

// static variable -------------------------------------

// const data  -----------------------------------------
static const SjisUtf16Pare tblSJIS_UTF16_8140[TBL8140_ELEM_NUM];
static const SjisUtf16Pare tblSJIS_UTF16_849f[TBL849f_ELEM_NUM];


//======================================================
// SJIS-BEからUTF16-LEへの変換
//======================================================
void MBi_ExSJIS_BEtoUTF16_LE(u8 *sjisp, u16 *unip, u16 length)
{
    u16     code;

    while ((*sjisp) && (length-- > 0))
    {
        if (((*sjisp >= SJIS_HIGHER_CODE1_MIN) && (*sjisp <= SJIS_HIGHER_CODE1_MAX))
            || ((*sjisp >= SJIS_HIGHER_CODE2_MIN) && (*sjisp <= SJIS_HIGHER_CODE2_MAX)))
        {                              // SJISか？
            code = (u16)((u16)*sjisp++ << 8);
            code |= (u16)*sjisp++;
        }
        else
        {                              // ASCII
            code = (u16)*sjisp++;
        }

        if (code == 0x005c)
        {
            *unip = 0x00a5;            // \

        }
        else if (code == 0x007e)
        {
            *unip = 0x203e;            // ~

        }
        else if ((code == 0x000d) || (code == 0x000a))
        {
            *unip = code;              // 改行コード

        }
        else if ((code >= 0x0020) && (code < 0x007e))   // ' ' ～ }
        {
            *unip = code;

        }
        else if ((code >= 0x00a1) && (code <= 0x00df))  // ｡ ～ ﾟ
        {
            *unip = (u16)((code - 0x00a1) + 0xff61);

        }
        else if ((code >= 0x8140) && (code <= 0x81fc))  // '　' ～ ○
        {
            // バラバラなので、テーブル引き
            *unip = tblSJIS_UTF16_8140[code - 0x8140].unicode;

        }
        else if ((code >= 0x824f) && (code <= 0x8258))  // ０ ～ ９
        {
            *unip = (u16)((code - 0x824f) + 0xff10);

        }
        else if ((code >= 0x8260) && (code <= 0x8279))  // Ａ ～ Ｚ
        {
            *unip = (u16)((code - 0x8260) + 0xff21);

        }
        else if ((code >= 0x8281) && (code <= 0x829a))  // ａ ～ ｚ
        {
            *unip = (u16)((code - 0x8281) + 0xff41);

        }
        else if ((code >= 0x829f) && (code <= 0x82f1))  // ぁ ～ ん
        {
            *unip = (u16)((code - 0x829f) + 0x3041);

        }
        else if ((code >= 0x8340) && (code <= 0x8396))  // ァ ～ ヶ  ※0x837fは抜け
        {
            *unip = (u16)((code - 0x8340) + 0x30a1);
            if (code == 0x837f)
            {
                *unip = 0x3000;
            }
            else if (code > 0x837f)
            {
                (*unip)--;
            }

        }
        else if ((code >= 0x839f) && (code <= 0x83b6))  // Α ～ Ω
        {
            *unip = (u16)((code - 0x839f) + 0x0391);
            if (code >= 0x83b0)
                (*unip)++;

        }
        else if ((code >= 0x83bf) && (code <= 0x83d6))  // α ～ ω
        {
            *unip = (u16)((code - 0x83bf) + 0x03b1);
            if (code >= 0x83d0)
                (*unip)++;

        }
        else if ((code >= 0x8440) && (code <= 0x8460))  // А ～ Я
        {
            *unip = (u16)((code - 0x8440) + 0x0410);
            if (code == 0x8446)
            {
                *unip = 0x0401;
            }
            else if (code > 0x8446)
            {
                (*unip)--;
            }

        }
        else if ((code >= 0x8470) && (code <= 0x8491))  // а ～ я ※0x847fは抜け
        {
            *unip = (u16)((code - 0x8470) + 0x0430);
            if (code == 0x8476)
            {
                *unip = 0x0451;
            }
            else if (code == 0x847f)
            {
                *unip = 0x3000;
            }
            else if (code > 0x8476)
            {
                (*unip)--;
                if (code > 0x847f)
                {
                    (*unip)--;
                }
            }

        }
        else if ((code >= 0x849f) && (code <= 0x84be))  // ─ ～ ╂
        {
            // バラバラなので、テーブル引き
            *unip = tblSJIS_UTF16_849f[code - 0x849f].unicode;
        }
        else
        {
            *unip = 0x3000;
        }

        unip++;
    }
}


//======================================================
// UTF16-LEからSJIS-BEへの変換
//======================================================
void MBi_ExUTF16_LEtoSJIS_BE(u8 *sjisp, u16 *unip, u16 length)
{
    u16     code, sjis_le;

    while ((*unip) && (length-- > 0))
    {
        code = *unip++;
        sjis_le = 0;

        if (code == 0x00a5)
        {
            sjis_le = 0x005c;          // \

        }
        else if (code == 0x005c)
        {
            sjis_le = 0x815f;          // ＼

        }
        else if (code == 0x203e)
        {
            sjis_le = 0x007e;          // ~

        }
        else if ((code == 0x000d) || (code == 0x000a))
        {
            sjis_le = code;            // 改行コード

        }
        else if ((code >= 0x0020) && (code < 0x007e))   // ' ' ～ }
        {
            sjis_le = code;

        }
        else if ((code >= 0x00a2) && (code <= 0x00f7))
        {                              // Unicode = 0x00a2 - 0x00f7 は、 SJIS = 0x814c - 0x81f7に配置
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0xc],
                                   0x81f7 - 0x814c);

        }
        else if ((code >= 0xff61) && (code <= 0xff9f))  // ｡ ～ ﾟ
        {
            sjis_le = (u16)((code - 0xff61) + 0x00a1);

        }
        else if (code == 0x4edd)
        {
            sjis_le = 0x8157;

        }
        else if ((code >= 0xff01) && (code <= 0xffe5))
        {
            if ((code >= 0xff10) && (code <= 0xff19))   // ０ ～ ９
            {
                sjis_le = (u16)((code - 0xff10) + 0x824f);

            }
            else if ((code >= 0xff21) && (code <= 0xff3a))      // Ａ ～ Ｚ
            {
                sjis_le = (u16)((code - 0xff21) + 0x8260);

            }
            else if ((code >= 0xff41) && (code <= 0xff5a))      // ａ ～ ｚ
            {
                sjis_le = (u16)((code - 0xff41) + 0x8281);
            }
            else
            {                          // Unicode = 0xff01 - 0xffe5 は、 SJIS = 0x8143 - 0x8197に配置
                sjis_le =
                    SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[3],
                                       0x8197 - 0x8143);
            }

        }
        else if ((code >= 0x3000) && (code <= 0x30fe))
        {
            if ((code >= 0x3041) && (code <= 0x3093))   // ぁ ～ ん
            {
                sjis_le = (u16)((code - 0x3041) + 0x829f);

            }
            else if ((code >= 0x30a1) && (code <= 0x30f6))      // ァ ～ ヶ  ※0x837fは抜け
            {
                sjis_le = (u16)((code - 0x30a1) + 0x8340);
                if (code >= 0x30e0)
                {
                    (sjis_le)++;
                }
            }
            else
            {                          // Unicode = 0x3000 - 0x30fe は、 SJIS = 0x8140 - 0x81acに配置
                sjis_le =
                    SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0],
                                       0x81ac - 0x8140);
            }

        }
        else if ((code >= 0x0391) && (code <= 0x03a9))  // Α ～ Ω
        {
            sjis_le = (u16)((code - 0x0391) + 0x839f);
            if (code >= 0x03a3)
                (sjis_le)--;

        }
        else if ((code >= 0x03b1) && (code <= 0x03c9))  // α ～ ω
        {
            sjis_le = (u16)((code - 0x03b1) + 0x83bf);
            if (code >= 0x03c3)
                (sjis_le)--;

        }
        else if (code == 0x0401)
        {
            sjis_le = 0x8446;

        }
        else if ((code >= 0x0410) && (code <= 0x042f))  // А ～ Я
        {
            sjis_le = (u16)((code - 0x0410) + 0x8440);
            if (code >= 0x0416)
            {
                (sjis_le)++;
            }

        }
        else if ((code >= 0x0430) && (code <= 0x044f))  // а ～ я ※0x847fは抜け
        {
            sjis_le = (u16)((code - 0x0430) + 0x8470);
            if (code >= 0x0436)
            {
                (sjis_le)++;
                if (code >= 0x043e)
                {
                    (sjis_le)++;
                }
            }

        }
        else if (code == 0x0451)
        {
            sjis_le = 0x8476;

        }
        else if ((code >= 0x2500) && (code <= 0x254b))  // ─ ～ ╂
        {
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_849f, TBL849f_ELEM_NUM);

        }
        else if (((code >= 0x2010) && (code <= 0x2312)) || ((code >= 0x25a0) && (code <= 0x266f)))
        {                              // 上記コードは、 SJIS = 0x815c - 0x81fcに配置
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0x815c - 0x8140],
                                   0x81fc - 0x815c);

        }
        else
        {
            sjis_le = 0x8140;
        }

        // 変換したSJISコードをバッファに格納
        if (sjis_le & 0xff00)
        {                              // ASCIIコードでなければ、ビッグエンディアン形式で格納。
            *sjisp++ = (u8)(sjis_le >> 8);
        }
        *sjisp++ = (u8)(sjis_le);
    }
}


// Unicode -> SJISへのテーブル引き
static u16 SearchUnicodeTable(u16 unicode, SjisUtf16Pare * tblp, int elem_num)
{
    elem_num++;
    while (elem_num--)
    {
        if (tblp->unicode == unicode)
        {
            return tblp->sjis;
        }
        tblp++;
    }
    return 0x8140;
}


//======================================================
// SJIS-BE <-> UTF16-LE変換のチェック
//======================================================
void MBi_CheckSJIS_BEtoUTF16_LE(void)
{
    u16     sjis, sjis_be, rev_sjis, sjis_le;
    u16     unicode;

    // ASCIIコードのチェック
    for (sjis = 0; sjis < 0x00ff; sjis++)
    {
        unicode = 0;
        rev_sjis = 0;
        MBi_ExSJIS_BEtoUTF16_LE((u8 *)&sjis, &unicode, 1);
        MBi_ExUTF16_LEtoSJIS_BE((u8 *)&rev_sjis, &unicode, 1);
        sjis_le = (u16)((rev_sjis >> 8) | (rev_sjis << 8));
        MB_DEBUG_OUTPUT("0x%x\t-> 0x%x\t-> 0x%x\n", sjis, unicode, sjis_le);
    }

    // SJISコードのチェック
    for (sjis = 0x8140; sjis < 0x84ff; sjis++)
    {
        unicode = 0;
        rev_sjis = 0;
        sjis_be = (u16)((sjis >> 8) | (sjis << 8));
        MBi_ExSJIS_BEtoUTF16_LE((u8 *)&sjis_be, &unicode, 1);
        MBi_ExUTF16_LEtoSJIS_BE((u8 *)&rev_sjis, &unicode, 1);
        sjis_le = (u16)((rev_sjis >> 8) | (rev_sjis << 8));
        MB_DEBUG_OUTPUT("0x%x\t-> 0x%x\t-> 0x%x\n", sjis, unicode, sjis_le);
    }
}


//======================================================
// SJISコード->Unicodeテーブル
//======================================================

// 0x8140～
static const SjisUtf16Pare tblSJIS_UTF16_8140[TBL8140_ELEM_NUM] = {
    {0x8140, 0x3000},                  // 　 
    {0x8141, 0x3001},                  // 、 
    {0x8142, 0x3002},                  // 。 
    {0x8143, 0xFF0C},                  // ， 
    {0x8144, 0xFF0E},                  // ． 
    {0x8145, 0x30FB},                  // ・ 
    {0x8146, 0xFF1A},                  // ： 
    {0x8147, 0xFF1B},                  // ； 
    {0x8148, 0xFF1F},                  // ？ 
    {0x8149, 0xFF01},                  // ！ 
    {0x814A, 0x309B},                  // ゛ 
    {0x814B, 0x309C},                  // ゜ 
    {0x814C, 0x00B4},                  // ´ 
    {0x814D, 0xFF40},                  // ｀ 
    {0x814E, 0x00A8},                  // ¨ 
    {0x814F, 0xFF3E},                  // ＾ 
    {0x8150, 0xFFE3},                  // ￣ 
    {0x8151, 0xFF3F},                  // ＿ 
    {0x8152, 0x30FD},                  // ヽ 
    {0x8153, 0x30FE},                  // ヾ 
    {0x8154, 0x309D},                  // ゝ 
    {0x8155, 0x309E},                  // ゞ 
    {0x8156, 0x3003},                  // 〃 
    {0x8157, 0x4EDD},                  // 仝 
    {0x8158, 0x3005},                  // 々 
    {0x8159, 0x3006},                  // 〆 
    {0x815A, 0x3007},                  // 〇 
    {0x815B, 0x30FC},                  // ー 
    {0x815C, 0x2015},                  // ― 
    {0x815D, 0x2010},                  // ‐ 
    {0x815E, 0xFF0F},                  // ／ 
    {0x815F, 0x005C},                  // ＼ 
    {0x8160, 0x301C},                  // ～ 
    {0x8161, 0x2016},                  // ∥ 
    {0x8162, 0xFF5C},                  // ｜ 
    {0x8163, 0x2026},                  // … 
    {0x8164, 0x2025},                  // ‥ 
    {0x8165, 0x2018},                  // ‘ 
    {0x8166, 0x2019},                  // ’ 
    {0x8167, 0x201C},                  // “ 
    {0x8168, 0x201D},                  // ” 
    {0x8169, 0xFF08},                  // （ 
    {0x816A, 0xFF09},                  // ） 
    {0x816B, 0x3014},                  // 〔 
    {0x816C, 0x3015},                  // 〕 
    {0x816D, 0xFF3B},                  // ［ 
    {0x816E, 0xFF3D},                  // ］ 
    {0x816F, 0xFF5B},                  // ｛ 
    {0x8170, 0xFF5D},                  // ｝ 
    {0x8171, 0x3008},                  // 〈 
    {0x8172, 0x3009},                  // 〉 
    {0x8173, 0x300A},                  // 《 
    {0x8174, 0x300B},                  // 》 
    {0x8175, 0x300C},                  // 「 
    {0x8176, 0x300D},                  // 」 
    {0x8177, 0x300E},                  // 『 
    {0x8178, 0x300F},                  // 』 
    {0x8179, 0x3010},                  // 【 
    {0x817A, 0x3011},                  // 】 
    {0x817B, 0xFF0B},                  // ＋ 
    {0x817C, 0x2212},                  // － 
    {0x817D, 0x00B1},                  // ± 
    {0x817E, 0x00D7},                  // × 
    {0x817F, 0x3000},                  //  
    {0x8180, 0x00F7},                  // ÷ 
    {0x8181, 0xFF1D},                  // ＝ 
    {0x8182, 0x2260},                  // ≠ 
    {0x8183, 0xFF1C},                  // ＜ 
    {0x8184, 0xFF1E},                  // ＞ 
    {0x8185, 0x2266},                  // ≦ 
    {0x8186, 0x2267},                  // ≧ 
    {0x8187, 0x221E},                  // ∞ 
    {0x8188, 0x2234},                  // ∴ 
    {0x8189, 0x2642},                  // ♂ 
    {0x818A, 0x2640},                  // ♀ 
    {0x818B, 0x00B0},                  // ° 
    {0x818C, 0x2032},                  // ′ 
    {0x818D, 0x2033},                  // ″ 
    {0x818E, 0x2103},                  // ℃ 
    {0x818F, 0xFFE5},                  // ￥ 
    {0x8190, 0xFF04},                  // ＄ 
    {0x8191, 0x00A2},                  // ￠ 
    {0x8192, 0x00A3},                  // ￡ 
    {0x8193, 0xFF05},                  // ％ 
    {0x8194, 0xFF03},                  // ＃ 
    {0x8195, 0xFF06},                  // ＆ 
    {0x8196, 0xFF0A},                  // ＊ 
    {0x8197, 0xFF20},                  // ＠ 
    {0x8198, 0x00A7},                  // § 
    {0x8199, 0x2606},                  // ☆ 
    {0x819A, 0x2605},                  // ★ 
    {0x819B, 0x25CB},                  // ○ 
    {0x819C, 0x25CF},                  // ● 
    {0x819D, 0x25CE},                  // ◎ 
    {0x819E, 0x25C7},                  // ◇ 
    {0x819F, 0x25C6},                  // ◆ 
    {0x81A0, 0x25A1},                  // □ 
    {0x81A1, 0x25A0},                  // ■ 
    {0x81A2, 0x25B3},                  // △ 
    {0x81A3, 0x25B2},                  // ▲ 
    {0x81A4, 0x25BD},                  // ▽ 
    {0x81A5, 0x25BC},                  // ▼ 
    {0x81A6, 0x203B},                  // ※ 
    {0x81A7, 0x3012},                  // 〒 
    {0x81A8, 0x2192},                  // → 
    {0x81A9, 0x2190},                  // ← 
    {0x81AA, 0x2191},                  // ↑ 
    {0x81AB, 0x2193},                  // ↓ 
    {0x81AC, 0x3013},                  // 〓 
    {0x81AD, 0x3000},                  //  
    {0x81AE, 0x3000},                  //  
    {0x81AF, 0x3000},                  //  
    {0x81B0, 0x3000},                  //  
    {0x81B1, 0x3000},                  //  
    {0x81B2, 0x3000},                  //  
    {0x81B3, 0x3000},                  //  
    {0x81B4, 0x3000},                  //  
    {0x81B5, 0x3000},                  //  
    {0x81B6, 0x3000},                  //  
    {0x81B7, 0x3000},                  //  
    {0x81B8, 0x2208},                  // ∈ 
    {0x81B9, 0x220B},                  // ∋ 
    {0x81BA, 0x2286},                  // ⊆ 
    {0x81BB, 0x2287},                  // ⊇ 
    {0x81BC, 0x2282},                  // ⊂ 
    {0x81BD, 0x2283},                  // ⊃ 
    {0x81BE, 0x222A},                  // ∪ 
    {0x81BF, 0x2229},                  // ∩ 
    {0x81C0, 0x3000},                  //  
    {0x81C1, 0x3000},                  //  
    {0x81C2, 0x3000},                  //  
    {0x81C3, 0x3000},                  //  
    {0x81C4, 0x3000},                  //  
    {0x81C5, 0x3000},                  //  
    {0x81C6, 0x3000},                  //  
    {0x81C7, 0x3000},                  //  
    {0x81C8, 0x2227},                  // ∧ 
    {0x81C9, 0x2228},                  // ∨ 
    {0x81CA, 0x00AC},                  // ￢ 
    {0x81CB, 0x21D2},                  // ⇒ 
    {0x81CC, 0x21D4},                  // ⇔ 
    {0x81CD, 0x2200},                  // ∀ 
    {0x81CE, 0x2203},                  // ∃ 
    {0x81CF, 0x3000},                  //  
    {0x81D0, 0x3000},                  //  
    {0x81D1, 0x3000},                  //  
    {0x81D2, 0x3000},                  //  
    {0x81D3, 0x3000},                  //  
    {0x81D4, 0x3000},                  //  
    {0x81D5, 0x3000},                  //  
    {0x81D6, 0x3000},                  //  
    {0x81D7, 0x3000},                  //  
    {0x81D8, 0x3000},                  //  
    {0x81D9, 0x3000},                  //  
    {0x81DA, 0x2220},                  // ∠ 
    {0x81DB, 0x22A5},                  // ⊥ 
    {0x81DC, 0x2312},                  // ⌒ 
    {0x81DD, 0x2202},                  // ∂ 
    {0x81DE, 0x2207},                  // ∇ 
    {0x81DF, 0x2261},                  // ≡ 
    {0x81E0, 0x2252},                  // ≒ 
    {0x81E1, 0x226A},                  // ≪ 
    {0x81E2, 0x226B},                  // ≫ 
    {0x81E3, 0x221A},                  // √ 
    {0x81E4, 0x223D},                  // ∽ 
    {0x81E5, 0x221D},                  // ∝ 
    {0x81E6, 0x2235},                  // ∵ 
    {0x81E7, 0x222B},                  // ∫ 
    {0x81E8, 0x222C},                  // ∬ 
    {0x81E9, 0x3000},                  //  
    {0x81EA, 0x3000},                  //  
    {0x81EB, 0x3000},                  //  
    {0x81EC, 0x3000},                  //  
    {0x81EE, 0x3000},                  //  
    {0x81EE, 0x3000},                  //  
    {0x81EF, 0x3000},                  //  
    {0x81F0, 0x212B},                  // Å 
    {0x81F1, 0x2030},                  // ‰ 
    {0x81F2, 0x266F},                  // ♯ 
    {0x81F3, 0x266D},                  // ♭ 
    {0x81F4, 0x266A},                  // ♪ 
    {0x81F5, 0x2020},                  // † 
    {0x81F6, 0x2021},                  // ‡ 
    {0x81F7, 0x00B6},                  // ¶ 
    {0x81F8, 0x3000},                  //  
    {0x81F9, 0x3000},                  //  
    {0x81FA, 0x3000},                  //  
    {0x81FB, 0x3000},                  //  
    {0x81FC, 0x25EF},                  // ◯ 
};


// 0x849f～
// Unicodeでは、0x2500-0x254bの間
static const SjisUtf16Pare tblSJIS_UTF16_849f[TBL849f_ELEM_NUM] = {
    {0x849F, 0x2500},                  // ─ 
    {0x84A0, 0x2502},                  // │ 
    {0x84A1, 0x250C},                  // ┌ 
    {0x84A2, 0x2510},                  // ┐ 
    {0x84A3, 0x2518},                  // ┘ 
    {0x84A4, 0x2514},                  // └ 
    {0x84A5, 0x251C},                  // ├ 
    {0x84A6, 0x252C},                  // ┬ 
    {0x84A7, 0x2524},                  // ┤ 
    {0x84A8, 0x2534},                  // ┴ 
    {0x84A9, 0x253C},                  // ┼ 
    {0x84AA, 0x2501},                  // ━ 
    {0x84AB, 0x2503},                  // ┃ 
    {0x84AC, 0x250F},                  // ┏ 
    {0x84AD, 0x2513},                  // ┓ 
    {0x84AE, 0x251B},                  // ┛ 
    {0x84AF, 0x2517},                  // ┗ 
    {0x84B0, 0x2523},                  // ┣ 
    {0x84B1, 0x2533},                  // ┳ 
    {0x84B2, 0x252B},                  // ┫ 
    {0x84B3, 0x253B},                  // ┻ 
    {0x84B4, 0x254B},                  // ╋ 
    {0x84B5, 0x2520},                  // ┠ 
    {0x84B6, 0x252F},                  // ┯ 
    {0x84B7, 0x2528},                  // ┨ 
    {0x84B8, 0x2537},                  // ┷ 
    {0x84B9, 0x253F},                  // ┿ 
    {0x84BA, 0x251D},                  // ┝ 
    {0x84BB, 0x2530},                  // ┰ 
    {0x84BC, 0x2525},                  // ┥ 
    {0x84BD, 0x2538},                  // ┸ 
    {0x84BE, 0x2542},                  // ╂ 
};

#endif /* defined(MB_CHANGE_TO_UNICODE) */
