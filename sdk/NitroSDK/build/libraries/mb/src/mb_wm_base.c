/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_wm_base.c

  Copyright 2003-2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_wm_base.c,v $
  Revision 1.18  2007/10/03 06:32:42  yosizaki
  fix to disable Beacon-indication on MB_EndToIdle().

  Revision 1.17  2007/07/30 03:32:58  yosizaki
  fix about MB_End

  Revision 1.16  2007/01/19 04:34:37  yosizaki
  copyrightを更新.

  Revision 1.15  2007/01/18 05:58:03  yosizaki
  fix about finalization.

  Revision 1.14  2006/07/19 04:41:30  yosizaki
  change default lifetime parameters.

  Revision 1.13  2006/02/20 02:35:20  seiki_masashi
  WM_GAMEINFO_TYPE_OLD のサポートを廃止

  Revision 1.12  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.11  2005/12/13 10:41:29  yosizaki
  remove temporary hook for SEND_FAILED

  Revision 1.10  2005/12/09 08:16:10  yosizaki
  add hook for SEND_FAILED (temporary)

  Revision 1.9  2005/11/14 05:37:28  yosizaki
  fix about WM_ATTR_FLAG_ENTRY flag.

  Revision 1.8  2005/11/07 01:21:50  okubata_ryoma
  SDK_STATIC_ASSERTからSDK_COMPILER_ASSERTに変更

  Revision 1.7  2005/10/28 11:23:41  seiki_masashi
  WM_STATECODE_DISCONNECTED_FROM_MYSELF の追加に対応
  WM_STATECODE_PORT_INIT の追加に対応

  Revision 1.6  2005/07/29 03:45:01  seiki_masashi
  WM_STATECODE_INFORMATION の追加

  Revision 1.5  2005/04/14 04:40:20  yosizaki
  fix timing of calling WM_GetNextTgid().

  Revision 1.4  2005/04/11 08:51:56  yosizaki
  move MBi_CreateTgid() to wm_sync.c as WM_GetNextTgid().

  Revision 1.3  2005/04/11 01:45:48  yosizaki
  add MB_SetLifeTime().
  add MB_SetPowerSaveMode().
  add MBi_CreateTgid().

  Revision 1.2  2005/02/28 05:26:25  yosizaki
  do-indent.

  Revision 1.1  2005/02/21 00:40:08  yosizaki
  replace filename from tmp_lib_stub.


  Revision 1.62  2005/02/18 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.61  2005/01/27 07:07:49  takano_makoto
  ichneumon使用時にウェイトを入れるように修正

  Revision 1.60  2004/12/28 09:32:28  pair2
  fix around MB_EndToIdle. (check endReq == 1 or pWork == NULL )

  Revision 1.59  2004/11/29 04:47:41  yosizaki
  allocate sendBuf. (avoid to share with SetMPData's buffer)

  Revision 1.58  2004/11/24 05:14:33  yosizaki
  rename MB_StartParentEx/MB_EndEx..

  Revision 1.57  2004/11/22 12:47:26  takano_makoto
  分割リクエスト送信の処理をmb_blockとして分離

  Revision 1.56  2004/11/18 07:28:46  takano_makoto
  MB_SetParentCommSize, MB_SetParentCommParamで子機送信サイズの設定引数を廃止

  Revision 1.55  2004/11/11 11:46:56  yosizaki
  change WM_End/Ex to finish task-thread before WM_Reset.

  Revision 1.54  2004/11/10 07:40:03  takano_makoto
  mb_gameinfo.cの中からMBw関数を排除し、スキャンパラメータの設定を切り出す

  Revision 1.53  2004/11/10 03:17:02  takano_makoto
  MBi_SetBeaconRecvStatusBufferDefaultを追加

  Revision 1.52  2004/11/09 12:02:23  yosizaki
  fix allocation of wmBuf.

  Revision 1.51  2004/11/09 01:05:39  seiki_masashi
  Null 応答によるタイムアウトを実装

  Revision 1.50  2004/10/26 08:09:54  takano_makoto
  MB_DisconnectChildを追加

  Revision 1.49  2004/10/19 00:55:31  sato_masaki
  WM_APIの返り値エラーがMB_CALLBACK_API_ERRORで返されていなかった箇所があるのを修正。

  Revision 1.48  2004/10/18 11:44:36  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.47  2004/10/15 08:50:44  yosizaki
  add start_mp_busy.

  Revision 1.46  2004/10/08 10:32:49  takano_makoto
  IPL起動画面でnameLengthの変更が反映されない不具合を修正

  Revision 1.45  2004/10/08 03:04:17  takano_makoto
  1.44の修正に追加してNUL文字をチェックするように修正

  Revision 1.44  2004/10/08 02:44:46  takano_makoto
  MB_InitでMBUserInfoのnameLengthを見るように修正

  Revision 1.43  2004/10/05 09:45:29  terui
  MACアドレスをu8型の配列に統一。

  Revision 1.42  2004/10/04 13:40:25  terui
  ゲームグループIDをu32型に統一。

  Revision 1.41  2004/10/03 08:17:31  ooe
  StartMPExの仕様変更に対応。ignoreFatalError。

  Revision 1.40  2004/09/28 08:55:30  sato_masaki
  - Indication callbackで、WM_STATECODE_FIFO_ERRORが出たときは、OS_Panicを出すように変更。
  - MBi_IsCommSizeValidにて、サイズ範囲の判定を追加。

  Revision 1.39  2004/09/25 05:51:47  sato_masaki
  NitroSDK_IPL_branch 1.27.2.7 とマージ

  Revision 1.38  2004/09/25 05:43:49  sato_masaki
  メインループにおいて呼ばれ得るAPI実行中に、割り込み禁止をする処理を追加。

  Revision 1.37  2004/09/22 09:41:18  sato_masaki
  IPL_branch 1.27.2.5とマージ

  Revision 1.36  2004/09/22 02:50:31  yosizaki
  optimize for code-size around MBi_ChildCallback().

  Revision 1.35  2004/09/22 02:31:57  yosizaki
  optimize for code-size around MB_Init().

  Revision 1.34  2004/09/21 01:39:15  sato_masaki
  WMに起因するエラーを、MB_ERRCODE_WM_FAILUREに統一。MB_ERRCODE_MP_SENT_FAILUREを廃止。

  Revision 1.33  2004/09/20 12:56:17  sato_masaki
  MB_Init()において、初期化済みであるか否かの判定を追加。

  Revision 1.32  2004/09/18 09:16:26  sato_masaki
  p_mbi_param->endReqフラグが立っている場合は、MB_End()は何もしないように変更。

  Revision 1.31  2004/09/18 03:11:25  sato_masaki
  small fix

  Revision 1.30  2004/09/18 02:58:50  sato_masaki
  bug fix

  Revision 1.29  2004/09/17 11:36:30  sato_masaki
  - add function MBi_IsStarted()
  - change MP size check (it always returns 'TRUE')

  Revision 1.28  2004/09/16 12:48:35  sato_masaki
  - mb.hにて、MBi_***で定義していた関数を、MB_***に変更。
  - 送受信データサイズの正当性評価をするようにした。

  Revision 1.27  2004/09/15 06:51:59  sato_masaki
  WBT定義に纏わる処理を削除。

  Revision 1.26  2004/09/15 06:17:47  sato_masaki
  add MBi_SetParentCommSize, MBi_GetTgid functions

  Revision 1.25  2004/09/14 12:07:15  terui
  rand関数を削除、MACアドレスによりビーコン間隔をばらつかせる機能に差替え。

  Revision 1.24  2004/09/14 02:55:29  sato_masaki
  comment 修正

  Revision 1.23  2004/09/14 02:41:38  sato_masaki
  MBiParam に mpBusy 追加。

  Revision 1.21  2004/09/13 13:08:26  sato_masaki
  - MBi_ChildPortCallback()を追加。
  - WM_STATECODE_CHILD_CONNECTEDのときは、MPセットをしないように変更。

  Revision 1.20  2004/09/13 02:56:23  sato_masaki
  旧仕様との互換のため、tgidを引数に持ったMBi_Init()を新設。

  Revision 1.19  2004/09/11 12:49:06  yosiokat
  ・MBi_StartCommon内でのスキャン時間設定を定数10からMB_SCAN_TIME_NORMALに変更。

  Revision 1.18  2004/09/11 11:16:10  sato_masaki
  - 親子それぞれについて、SDK_STATIC_ASSERTにてワークサイズの正当性を判定。
  - MB_CommWork構造体をMB_CommPWork, MB_CommCWork, MB_CommCommonWorkに分離

  Revision 1.17  2004/09/11 07:02:08  yosiokat
  ・SDK_SMALL_BUILD定義による親子コード分離を止め、普通にビルドして分離できるよう変更する。
  　MBi_StartCoreの処理の共通化できる部分をMBi_StartCommonとし、それ以外をMB_StartParentとMB_StartChildに移す。
  　MBi_SendMP内でのMODE_PARENT時のコールバック指定をMBi_ParentCallbackからp_mbi_param->callback_ptrに変更。

  Revision 1.16  2004/09/11 05:01:26  terui
  送受信バッファサイズ計算マクロの暫定対策を元に戻した。

  Revision 1.15  2004/09/11 04:00:21  sato_masaki
  WM_Calc*******()マクロ中の、キーシェアリング使用を示すBOOL値をTRUEにする。
  (WM_StartMPEx()のエラー回避のための一時的対応策。)

  Revision 1.14  2004/09/10 12:32:47  seiki_masashi
  キーシェアリングの送信バイト数をライブラリ内で隠蔽するように変更

  Revision 1.13  2004/09/10 07:50:15  sato_masaki
  changeScanChannel()関数にて、WM_GetAllowedChannel()から取得した
  使用可能channelの中から、ScanChannelを選定するように変更。
  また、スキャン可能なChannelが存在しない場合、MBi_CommEnd()を呼ぶように変更。

  Revision 1.12  2004/09/10 02:32:23  sato_masaki
  WM_StartScan()発行時のscanParam.channel値を変更。

  Revision 1.11  2004/09/09 06:49:08  sato_masaki
  bug fix about changeScanChannel()

  Revision 1.10  2004/09/09 06:29:49  yosizaki
  change WM_SetMPData to use ToPort(WM_PORT_BT, WM_PRIORITY_LOW).

  Revision 1.9  2004/09/09 02:46:53  sato_masaki
  CAM_LIFE_TIMEを40に戻す。

  Revision 1.8  2004/09/09 01:33:18  sato_masaki
  FRAME_LIFE_TIMEを5に戻し、CAM_LIFE_TIMEを2*40に設定。

  Revision 1.7  2004/09/09 00:50:03  sato_masaki
  FRAME_LIFE_TIMEを10に設定。

  Revision 1.6  2004/09/08 13:10:09  sato_masaki
  WM_ERRCODE_INVALID_POLLBITMAPの時もMB_CALLBACK_MP_CHILD_RECVコールバックを返すようにした。

  Revision 1.5  2004/09/08 10:58:54  sato_masaki
  - LIFETIMEを有限に設定。
  - Scan時のseekチャンネルを1,7,13に変更
  - WM_StartMPをExに変更し、再送無しモードに設定。

  Revision 1.4  2004/09/06 12:05:24  sato_masaki
  StartConnectに関するCommentを削除。

  Revision 1.3  2004/09/06 11:43:21  sato_masaki
  MB_Init()関数の引数に、tgidを追加。
  MBi_GetTgid()関数の追加。

  Revision 1.2  2004/09/04 09:29:03  yosiokat
  SDK_SMALL_BUILD定義時に親機モードのコードを削るよう変更。

  Revision 1.1  2004/09/03 07:04:36  sato_masaki
  ファイルを機能別に分割。

  Revision 1.25  2004/09/02 09:46:49  sato_masaki
  mbc->fileinfoの0クリアを追加。

  Revision 1.24  2004/08/30 06:32:46  sato_masaki
  定義WMBTをWBTに変更。

  Revision 1.23  2004/08/26 10:08:57  sato_masaki
  BeaconRecv時のステートチェックを追加

  Revision 1.22  2004/08/26 09:54:59  sato_masaki
  small fix

  Revision 1.21  2004/08/24 12:20:06  sato_masaki
  SSID照合対策(一時凌ぎ)

  Revision 1.20  2004/08/23 04:45:17  ooe
  WM_StartConnect()の仕様変更に対応。

  Revision 1.19  2004/08/19 22:55:17  yosiokat
  MB_Initでのマルチブートフラグ、GGIDのセットを止めて、ビーコン送信時のWM_SetGameInfoでセットするようにする。

  Revision 1.18  2004/08/19 16:00:09  miya
  modified WM_SetGameInfo function

  Revision 1.17  2004/08/19 15:11:47  yosiokat
  前回修正のコメント追加。MB_Initでp_mbi_param->parentParam->multiBootFlag= 1に変更。

  Revision 1.16  2004/08/19 15:07:37  yosiokat
  WM_GAMEINFO_TYPE_OLD定義のみp_mbi_param->parentParam->userName、gameNameをセットするよう修正。

  Revision 1.15  2004/08/19 12:27:20  sato_masaki
  Beacon Indicateでの、通信終了処理をコメントアウト

  Revision 1.14  2004/08/19 08:00:53  ikedae
  #include <string.h>　削除

  Revision 1.13  2004/08/19 07:51:40  sato_masaki
  WMの変更仕様に対応。

  Revision 1.12  2004/08/19 07:34:16  ooe
  beaconPeriodのランダム化(暫定 実験用)

  Revision 1.11  2004/08/19 02:42:36  yosizaki
  change type of MBUserInfo.

  Revision 1.10  2004/08/18 01:25:09  miya
  small fix

  Revision 1.9  2004/08/18 00:05:14  sato_masaki
  WM_StartMPの引数変更に伴う更新

  Revision 1.8  2004/08/16 13:02:40  sato_masaki
  MBi_GetSendBufSize関数を追加

  Revision 1.7  2004/08/12 09:20:57  yosiokat
  ・MBi_RestartScan、MBi_SetMaxScanTime、MBi_GetScanChannel関数の追加。
  ・スキャン時にPARENT_NOT_FOUNDだった場合にも、MB_Commレイヤにコールバックを返すよう変更。

  Revision 1.6  2004/08/10 12:11:55  sato_masaki
  MBi_GetAid関数の追加。

  Revision 1.5  2004/08/09 14:56:43  yosizaki
  format all the symbol for API

  $NoKeywords: $
 *---------------------------------------------------------------------------*/



#include "mb_private.h"


/*
 * 要求しているサイズで容量が正しいか判定.
 */
SDK_COMPILER_ASSERT(32 + sizeof(MBiParam) + 32 + sizeof(MB_CommPWork) + 32 + WM_SYSTEM_BUF_SIZE <=
                    MB_SYSTEM_BUF_SIZE);
SDK_COMPILER_ASSERT(32 + sizeof(MBiParam) + 32 + sizeof(MB_CommCWork) + 32 + WM_SYSTEM_BUF_SIZE <=
                    MB_CHILD_SYSTEM_BUF_SIZE);


/*
 * 親/子 送受信バッファサイズ計算マクロ
 * 受信バッファ  parentMaxSize + WLHeader(48B) + WMHeader(2B)
 * 送信バッファ (parentMaxSize + WMHeader(2B) + (KeySet(36B) KS時のみ) + 31) & 0xffffffe0 (アライメント修正)
 * 受信バッファ (childMaxSize + WLHeader(8B) + WMHeader(2B) + (KeyData(2B)(KeySharing時のみ)) を15台分 ＋ 全体のHeader(8B) + 31) & 0xffffffe0 (アライメント修正)
 */
#define WL_HEADER_LENGTH_P  48         /* 親機受信時のWLヘッダ長 */
#define WM_HEADER_LENGTH    2          /* WMHeader長 */
#define WL_HEADER_LENGTH_T  8          /* 全体のWLヘッダ長 */
#define WL_HEADER_LENGTH_C  8          /* それぞれの子機データのWLヘッダ長 */


// マクロ 子機受信バッファサイズ計算

#define WM_CalcChildSendBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_SEND_BUFFER(((WMGameInfo *)(&(((WMBssDesc*)(_pInfo_))->gameInfo)))->childMaxSize, FALSE))
#define WM_CalcChildRecvBufSize(_pInfo_)        (u16)(WM_SIZE_MP_CHILD_RECEIVE_BUFFER(((WMGameInfo *)(&(((WMBssDesc*)(_pInfo_))->gameInfo)))->parentMaxSize, FALSE))

// マクロ 親機送受信バッファサイズ計算
#define WM_CalcParentSendBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_SEND_BUFFER(((MBiParam*)(_libParam_))->parentParam.parentMaxSize, FALSE))
#define WM_CalcParentRecvBufSize(_libParam_)    (u16)(WM_SIZE_MP_PARENT_RECEIVE_BUFFER(((MBiParam*)(_libParam_))->parentParam.childMaxSize, WM_NUM_MAX_CHILD, FALSE))


/* Beacon間隔(ms)デフォルト値 */
#define MB_BEACON_PERIOD_DEFAULT    (200)

/*
 * LifeTime の切り替えデバッグスイッチ.
 */
#define  NO_LIFETIME    0
#if (NO_LIFETIME == 1)
#define FRAME_LIFE_TIME 0xFFFF
#define CAM_LIFE_TIME   0xFFFF
#define MP_LIFE_TIME    0xFFFF
#else
/* WMライブラリが不安定であった時期に行っていた対策 */
/*
#define FRAME_LIFE_TIME 0xFFFF
*/
#define FRAME_LIFE_TIME 5
#define CAM_LIFE_TIME   40
#define MP_LIFE_TIME    40
#endif

#define TABLE_NO    0xFFFF


static u16 mbi_life_table_no = TABLE_NO;
static u16 mbi_life_frame = FRAME_LIFE_TIME;
static u16 mbi_life_cam = CAM_LIFE_TIME;
static u16 mbi_life_mp = MP_LIFE_TIME;
static BOOL mbi_power_save_mode = TRUE;


//===========================================================
// 関数プロトタイプ宣言
//===========================================================
static void MBi_ScanLock(u8 *macAddr); // スキャンロック設定用関数
static void MBi_ScanUnlock(void);      // スキャンロック解除用関数

/*
 * WM 結果値をチェックしてエラーならコールバック
 */
static void MBi_CheckWmErrcode(u16 apiid, int errcode);
static void MBi_ParentCallback(void *arg);



/******************************************************************************/
/* 変数 */

static MBiParam *p_mbi_param;
static u16 WM_DMA_NO;

/*
 * WMシステムバッファ.
 * 元は MBiParam メンバだったが,
 * MB_StartParentFromIdle() のために独立.
 */
static u8 *wmBuf;


MB_CommCommonWork *mbc = NULL;


/* 子機用変数 */
static WMscanParam scanParam ATTRIBUTE_ALIGN(32);


/******************************************************************************/
/* 関数 */


/* ScanChannel を巡回変更していく */
static BOOL changeScanChannel(WMscanParam *p)
{
    u16     channel_bmp, channel, i;

    /* channel bitmap 取得 */
    channel_bmp = WM_GetAllowedChannel();

    /* 使用可能なchannelが無い場合は、FALSEを返す． */
    if (channel_bmp == 0)
    {
        OS_TWarning("No Available Scan channel\n");
        return FALSE;
    }

    /* 使用可能なchannelが存在した場合． */
    for (i = 0, channel = p->channel;
         i < 16; i++, channel = (u16)((channel == 16) ? 1 : channel + 1))
    {
        if ((channel_bmp & (1 << (channel - 1))) == 0)
        {
            continue;
        }

        /* 検出されたchannelが前のものと同じだった場合は別のchannelを検索する */
        if (p->channel != channel)
        {
            p->channel = channel;
            break;
        }
    }

    return TRUE;

}


/*---------------------------------------------------------------------------*
  Name:         MB_SetLifeTime

  Description:  MB のワイヤレス駆動に対してライフタイムを明示的に指定.
                デフォルトでは ( 0xFFFF, 40, 0xFFFF, 40 ) になっている.

  Arguments:    tgid            指定される TGID

  Returns:      引数をそのままか, または適切な TGID.
 *---------------------------------------------------------------------------*/
void MB_SetLifeTime(u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime)
{
    mbi_life_table_no = tableNumber;
    mbi_life_cam = camLifeTime;
    mbi_life_frame = frameLifeTime;
    mbi_life_mp = mpLifeTime;
}

/*---------------------------------------------------------------------------*
  Name:         MB_SetPowerSaveMode

  Description:  常時通電モードを設定する.
                これは電力消費を考慮しなくて良い場面で安定に駆動するための
                オプションでありデフォルトでは無効となっている.
                電源につながれていることを保証された動作環境でない限り,
                通常のゲームアプリケーションがこれを使用すべきではない.

  Arguments:    enable           有効にするならば TRUE, 無効にするならば FALSE.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetPowerSaveMode(BOOL enable)
{
    mbi_power_save_mode = enable;
}


/*
 * 処理中を成功とみなして変換した WM 結果値を返す
 */
static inline int conv_errcode(int errcode)
{
    return (errcode == WM_ERRCODE_OPERATING) ? WM_ERRCODE_SUCCESS : errcode;
}


/*
 * MP送信許可チェック
   
   SetMP後、コールバックが返る前に、再びMPをセットしてしまわないように、
   SetMP実行時に立てる、mpBusyのフラグを判定要素に追加。
 
 */
static BOOL MBi_IsSendEnabled(void)
{
    return (p_mbi_param->mpStarted == 1) &&
        (p_mbi_param->mpBusy == 0) &&
        (p_mbi_param->endReq == 0) && (p_mbi_param->child_bitmap != 0);
}

static void MBi_OnInitializeDone(void)
{
    int     errcode;
    /* APIの発行 */
    errcode = WM_SetIndCallback(MBi_ParentCallback);
    MBi_CheckWmErrcode(WM_APIID_INDICATION, errcode);
    errcode = WM_SetLifeTime(MBi_ParentCallback, mbi_life_table_no,
                             mbi_life_cam, mbi_life_frame, mbi_life_mp);
    MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME, errcode);
}


/* 
 * MB親機がichneumonを使用してVRAMで無線を動作させている場合、
 * 子機台数が少ないと親機のMPデータ設定が早くに処理され過ぎてしまい、
 * IPLが親機のMP通信に追いつけない。
 * この対策のため、一定時間のウェイトを入れる。
 */
static inline void MbWaitForWvr(u32 cycles)
{
    u32     child_cnt = 0;
    u32     i;

    for (i = 0; i < MB_MAX_CHILD; i++)
    {
        if (pPwork->p_comm_state[i] != MB_COMM_PSTATE_NONE)
        {
            if (++child_cnt >= 2)
                break;
        }
    }
    // 子機が一台だけの場合にウェイト処理を入れる。
    if (child_cnt == 1)
    {
        OS_SpinWait(cycles);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_EndCommon

  Description:  共通のMB終了処理.

  Arguments:    arg               ユーザへの終了コールバック引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_EndCommon(void *arg)
{
    p_mbi_param->mbIsStarted = 0;
    mbc->isMbInitialized = 0;
    if (p_mbi_param->callback)
    {
        p_mbi_param->callback(MB_CALLBACK_END_COMPLETE, arg);
    }
}

/*
 * 親機コールバック
 */
static void MBi_ParentCallback(void *arg)
{
    int     errcode;
    WMCallback *buf = (WMCallback *)arg;

    switch (buf->apiid)
    {
    case WM_APIID_INITIALIZE:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            MBi_OnInitializeDone();
        }
        break;

    case WM_APIID_SET_LIFETIME:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            // APIの発行
            errcode = WM_SetParentParameter(MBi_ParentCallback, &p_mbi_param->parentParam);
            MBi_CheckWmErrcode(WM_APIID_SET_P_PARAM, errcode);
        }
        break;

    case WM_APIID_SET_P_PARAM:
        {
            p_mbi_param->callback(MB_CALLBACK_INIT_COMPLETE, arg);
            // ここではエラーコードは返ってこない
            errcode = WM_SetBeaconIndication(MBi_ParentCallback, 1 /* 1:ON, 0:OFF */ );
            MBi_CheckWmErrcode(WM_APIID_SET_BEACON_IND, errcode);
        }
        break;

        // Beacon Send/Recv Indicationを発生させるかどうか
    case WM_APIID_SET_BEACON_IND:
        if (!p_mbi_param->endReq)
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            errcode = WMi_StartParentEx(MBi_ParentCallback, mbi_power_save_mode);
            MBi_CheckWmErrcode(WM_APIID_START_PARENT, errcode);
        }
        else
        {
            // ビーコン通知を無効化し、終了処理完了
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
            }
            else
            {
                MBi_EndCommon(arg);
            }
        }
        break;

    case WM_APIID_START_PARENT:
        {
            WMstartParentCallback *callback = (WMstartParentCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_PARENT_START:
                p_mbi_param->child_bitmap = 0;
                p_mbi_param->mpStarted = 0;
                break;
            case WM_STATECODE_CHILD_CONNECTED:

                /* MB_End()コール後は強制的にbreak */
                if (p_mbi_param->endReq == 1)
                    break;

                p_mbi_param->child_bitmap |= (0x1 << callback->aid);
                p_mbi_param->callback(MB_CALLBACK_CHILD_CONNECTED, arg);

                // MP開始前ならMP開始
                if ((p_mbi_param->mpStarted == 0) && (!mbc->start_mp_busy))
                {
                    mbc->start_mp_busy = TRUE;
                    errcode = WM_StartMPEx(MBi_ParentCallback, (u16 *)p_mbi_param->recvBuf, p_mbi_param->recvBufSize, (u16 *)p_mbi_param->sendBuf, p_mbi_param->sendBufSize, (u16)(p_mbi_param->contSend ? 0 : 1), 0,   /* defaultRetryCount */
                                           FALSE, FALSE, 1,     /* fixFreqMode */
                                           TRUE /* ignoreFatalError */
                        );
                    MBi_CheckWmErrcode(WM_APIID_START_MP, errcode);

                }
                else
                {
                    // MP送信許可コールバック
                    if (MBi_IsSendEnabled())
                    {
                        p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
                    }
                }
                break;

            case WM_STATECODE_DISCONNECTED:
                p_mbi_param->child_bitmap &= ~(0x1 << callback->aid);
                p_mbi_param->callback(MB_CALLBACK_CHILD_DISCONNECTED, arg);
                break;

            case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                // 自ら切断した場合は処理しない
                break;

            case WM_STATECODE_BEACON_SENT:
                /* MB_End()コール後は強制的にbreak */
                if (p_mbi_param->endReq == 1)
                    break;

                // ビーコン送信完了通知
                p_mbi_param->callback(MB_CALLBACK_BEACON_SENT, arg);
                break;

            default:
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_START_MP:
        {
            // ここではエラーコードは返ってこない
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;
            mbc->start_mp_busy = FALSE;
            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                // MP開始済みフラグをセット
                p_mbi_param->mpStarted = 1;
                {
                    // MP送信許可コールバック
                    if (p_mbi_param->endReq == 0)
                    {
                        p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
                    }
                }
                break;

            case WM_STATECODE_MPEND_IND:
                // MP受信コールバック
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_RECV, (void *)(callback->recvBuf));
                break;

            default:
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
            break;
        }
        break;

    case WM_APIID_SET_MP_DATA:
        {
            /* MB親機がichneumonを使用している場合にウェイトを入れる */
            if (pPwork->useWvrFlag)
            {
                MbWaitForWvr(13000);   // ARM9をIRQ割り込み中に約3ラインウェイトさせる
            }
            p_mbi_param->mpBusy = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_SENT, arg);
                if (p_mbi_param->endReq == 0)
                {
                    p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);        // 次の送信を許可
                }
            }
            else if (buf->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
            {
                p_mbi_param->callback(MB_CALLBACK_SEND_QUEUE_FULL_ERR, arg);
            }
            else
            {
                p_mbi_param->callback(MB_CALLBACK_MP_PARENT_SENT_ERR, arg);
                if (p_mbi_param->endReq == 0)
                {
                    p_mbi_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);        // 次の送信を許可
                }
            }
        }
        break;

    case WM_APIID_RESET:
        if (!mbc->is_started_ex)
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
            }
            else
            {
                p_mbi_param->child_bitmap = 0;
                p_mbi_param->mpStarted = 0;
                errcode = WM_End(MBi_ParentCallback);
                MBi_CheckWmErrcode(WM_APIID_END, errcode);
            }
        }
        else
        {
            (void)WM_SetPortCallback(WM_PORT_BT, NULL, NULL);
            (void)WM_SetIndCallback(NULL);
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
            }
            else
            {
                errcode = WM_SetBeaconIndication(MBi_ParentCallback, 0);
                MBi_CheckWmErrcode(WM_APIID_SET_BEACON_IND, errcode);
            }
        }
        break;
    case WM_APIID_END:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_mbi_param->endReq = 0;
                p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
            }
            else
            {
                MBi_EndCommon(arg);
            }
        }
        break;

    case WM_APIID_DISCONNECT:
        {
            WMDisconnectCallback *callback = (WMDisconnectCallback *)arg;

            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                return;
            }

            // 子機情報更新
            p_mbi_param->child_bitmap &= ~(callback->disconnectedBitmap);
        }
        break;

    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_BEACON_RECV:     // Beacon受信indicate
                p_mbi_param->callback(MB_CALLBACK_BEACON_RECV, arg);
                break;
            case WM_STATECODE_DISASSOCIATE:    // 接続切断indicate
                p_mbi_param->callback(MB_CALLBACK_DISASSOCIATE, arg);
                break;
            case WM_STATECODE_REASSOCIATE:     // 再接続indicate
                p_mbi_param->callback(MB_CALLBACK_REASSOCIATE, arg);
                break;
            case WM_STATECODE_AUTHENTICATE:    // 認証確認indicate
                p_mbi_param->callback(MB_CALLBACK_AUTHENTICATE, arg);
                break;

            case WM_STATECODE_FIFO_ERROR:
                OS_TPanic("FIFO Error\n");
                break;
            case WM_STATECODE_INFORMATION:
                // 何もしない
                break;
            }
        }
        break;

    default:
        p_mbi_param->callback(MB_CALLBACK_ERROR, arg);
        break;
    }
}

/*
 * 子機コールバック
 */
static void MBi_ChildPortCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_PORT_RECV:
        // データ受信を通知
        p_mbi_param->callback(MB_CALLBACK_MP_CHILD_RECV, (void *)(arg));
        break;
    case WM_STATECODE_CONNECTED:
        // 接続通知
        break;
    case WM_STATECODE_PORT_INIT:
    case WM_STATECODE_DISCONNECTED:
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        break;
    }
}


static void MBi_ChildCallback(void *arg)
{
    WMCallback *buf = (WMCallback *)arg;
    MBiParam *const p_param = p_mbi_param;
    int     errcode;

    switch (buf->apiid)
    {
        //---------------------------------------------------------------
    case WM_APIID_INITIALIZE:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            p_param->callback(MB_CALLBACK_INIT_COMPLETE, arg);

            // APIの発行
            errcode =
                WM_SetLifeTime(MBi_ChildCallback, mbi_life_table_no, mbi_life_cam, mbi_life_frame,
                               mbi_life_mp);
            MBi_CheckWmErrcode(WM_APIID_SET_LIFETIME, errcode);
        }
        break;

    case WM_APIID_SET_LIFETIME:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            // WM_StartScan()発行
            scanParam.scanBuf = &p_param->parentInfoBuf;
            if (scanParam.channel == 0)
                scanParam.channel = 1;
            if (scanParam.maxChannelTime == 0)
            {
                scanParam.maxChannelTime = 200;
            }
            scanParam.bssid[0] = 0xff;
            scanParam.bssid[1] = 0xff;
            scanParam.bssid[2] = 0xff;
            scanParam.bssid[3] = 0xff;
            scanParam.bssid[4] = 0xff;
            scanParam.bssid[5] = 0xff;
            p_param->scanning_flag = TRUE;
            p_param->scan_channel_flag = TRUE;
            errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
            MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
        }
        break;

    case WM_APIID_START_SCAN:
        {
            WMstartScanCallback *callback = (WMstartScanCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_SCAN_START:
                break;

            case WM_STATECODE_PARENT_FOUND:
                // Scanで取得した親機情報の格納
                {
                    WMstartScanCallback *buf = (WMstartScanCallback *)arg;
                    ParentInfo *p = &p_param->parent_info[0];

                    int     i;
                    for (i = 0; i < p_param->found_parent_count; ++i)
                    {

                        if (WM_IsBssidEqual(buf->macAddress, p[i].scan_data.macAddress))
                        {
                            p[i].scan_data.gameInfoLength = buf->gameInfoLength;
                            p[i].scan_data.gameInfo = buf->gameInfo;
                            DC_InvalidateRange(p_param->parent_info[i].parentInfo,
                                               WM_BSS_DESC_SIZE);
                            MI_DmaCopy16(WM_DMA_NO, &p_param->parentInfoBuf,
                                         p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                            p_param->last_found_parent_no = i;
                            goto scan_end;
                        }
                    }

                    if (i < MB_NUM_PARENT_INFORMATIONS)
                    {
                        p_param->found_parent_count = (u16)(i + 1);
                        // 新規に発見した親機なので、データを格納する
                        MI_CpuCopy16(buf, &p[i].scan_data, sizeof(p[i].scan_data));
                        DC_InvalidateRange(p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                        MI_DmaCopy16(WM_DMA_NO, &p_param->parentInfoBuf,
                                     p_param->parent_info[i].parentInfo, WM_BSS_DESC_SIZE);
                        p_param->last_found_parent_no = i;
                    }
                }
              scan_end:
                p_param->callback(MB_CALLBACK_PARENT_FOUND, arg);
                if (!p_param->scanning_flag)
                {
                    return;
                }

                if (p_param->scan_channel_flag)
                {
                    if (FALSE == changeScanChannel(&scanParam))
                    {
                        (void)MBi_CommEnd();
                    }
                }
                errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
                MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
                break;

            case WM_STATECODE_PARENT_NOT_FOUND:
                p_param->callback(MB_CALLBACK_PARENT_NOT_FOUND, arg);   // ここのコールバックはParentInfoLifeTimeCountに
                if (!p_param->scanning_flag)
                {
                    return;
                }

                if (p_param->scan_channel_flag)
                {
                    if (FALSE == changeScanChannel(&scanParam))
                    {
                        (void)MBi_CommEnd();
                    }
                }
                errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
                MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_END_SCAN:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }

            errcode = WM_StartConnect(MBi_ChildCallback, p_param->pInfo, NULL);
            MBi_CheckWmErrcode(WM_APIID_START_CONNECT, errcode);
        }
        break;

    case WM_APIID_START_CONNECT:
        {
            WMstartConnectCallback *callback = (WMstartConnectCallback *)arg;

            if (callback->errcode != WM_ERRCODE_SUCCESS)
            {
                /* 親の数をリセット */
                p_param->found_parent_count = 0;
                // コールバックでエラー通知
                p_param->callback(MB_CALLBACK_CONNECT_FAILED, arg);
                return;
            }

            switch (callback->state)
            {
            case WM_STATECODE_CONNECT_START:
                p_param->child_bitmap = 0;
                p_param->mpStarted = 1;
                break;

            case WM_STATECODE_CONNECTED:
                p_param->my_aid = (u16)callback->aid;
                p_param->callback(MB_CALLBACK_CONNECTED_TO_PARENT, arg);
                p_param->child_bitmap = 1;

                errcode = WM_SetPortCallback(WM_PORT_BT, MBi_ChildPortCallback, NULL);

                if (errcode != WM_ERRCODE_SUCCESS)
                {
                    break;
                }

                errcode = WM_StartMPEx(MBi_ChildCallback, (u16 *)p_param->recvBuf, p_param->recvBufSize, (u16 *)p_param->sendBuf, p_param->sendBufSize, (u16)(p_param->contSend ? 0 : 1), 0,    /* defaultRetryCount */
                                       FALSE, FALSE, 1, /* fixFreqMode */
                                       TRUE     /* ignoreFatalError */
                    );
                MBi_CheckWmErrcode(WM_APIID_START_MP, errcode);
                break;

            case WM_STATECODE_DISCONNECTED:
                p_param->callback(MB_CALLBACK_DISCONNECTED_FROM_PARENT, arg);
                p_param->child_bitmap = 0;
                p_param->mpStarted = 0;
                break;

            case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
                // 自ら切断した場合は処理しない
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_START_MP:
        {
            WMstartMPCallback *callback = (WMstartMPCallback *)arg;

            switch (callback->state)
            {
            case WM_STATECODE_MP_START:
                p_param->mpStarted = 1; // MP開始済みフラグをセット
                {
                    // MP送信許可コールバック
                    if (MBi_IsSendEnabled())
                    {
                        p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
                    }
                }
                break;

            case WM_STATECODE_MP_IND:
                if (callback->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
                {
//                  p_param->callback( MB_CALLBACK_MP_CHILD_RECV, (void*)(callback->recvBuf) );
                }
                else
                {
//                    p_param->callback( MB_CALLBACK_MP_CHILD_RECV, (void*)(callback->recvBuf) );
                }
                break;

            case WM_STATECODE_MPACK_IND:
                {
                    //p_param->callback( MB_CALLBACK_MPACK_IND, NULL );
                }
                break;

            default:
                p_param->callback(MB_CALLBACK_ERROR, arg);
                break;
            }
        }
        break;

    case WM_APIID_SET_MP_DATA:
        {
            p_param->mpBusy = 0;
            if (buf->errcode == WM_ERRCODE_SUCCESS)
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT, arg);
            }
            else if (buf->errcode == WM_ERRCODE_TIMEOUT)
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT_TIMEOUT, arg);
            }
            else
            {
                p_param->callback(MB_CALLBACK_MP_CHILD_SENT_ERR, arg);
            }
            // 次の送信を許可
            if (p_mbi_param->endReq == 0)
            {
                p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
            }
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_RESET:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->endReq = 0;
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            p_mbi_param->child_bitmap = 0;
            p_mbi_param->mpStarted = 0;

            errcode = WM_End(MBi_ChildCallback);
            MBi_CheckWmErrcode(WM_APIID_END, errcode);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_END:
        {
            if (buf->errcode != WM_ERRCODE_SUCCESS)
            {
                p_param->endReq = 0;
                p_param->callback(MB_CALLBACK_ERROR, arg);
                return;
            }
            MBi_EndCommon(arg);
        }
        break;

        //---------------------------------------------------------------
    case WM_APIID_START_KS:
        {
            // MP送信許可コールバック
            if (MBi_IsSendEnabled())
            {
                p_param->callback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
            }
        }
        break;

        //---------------------------------------------------------------        
    case WM_APIID_INDICATION:
        {
            WMindCallback *cb = (WMindCallback *)arg;
            switch (cb->state)
            {
            case WM_STATECODE_FIFO_ERROR:
                OS_TPanic("FIFO Error\n");
                break;
            case WM_STATECODE_INFORMATION:
                // 何もしない
                break;
            }
        }
        break;
    default:
        p_param->callback(MB_CALLBACK_ERROR, arg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_GetBeaconPeriodDispersion

  Description:  機器のMACアドレスごとに適度にばらついたビーコン間隔へ付加する
                値を取得する。

  Arguments:    None.

  Returns:      u32 -   機器ごとに適度にばらついた 0 ～ 19 の値を返す。
 *---------------------------------------------------------------------------*/
static u32 MBi_GetBeaconPeriodDispersion(void)
{
    u8      mac[6];
    u32     ret;
    s32     i;

    OS_GetMacAddress(mac);
    for (i = 0, ret = 0; i < 6; i++)
    {
        ret += mac[i];
    }
    ret += OS_GetVBlankCount();
    ret *= 7;
    return (ret % 20);
}


/******************************************************************************/
/* API */

/* 初期化 */
int MB_Init(void *work, const MBUserInfo *user, u32 ggid, u32 tgid, u32 dma)
{

    SDK_ASSERT(user != NULL);
    SDK_ASSERT(work != NULL);
    SDK_ASSERT((dma < 4));
    SDK_ASSERT(user->nameLength <= MB_USER_NAME_LENGTH);

    if (mbc && mbc->isMbInitialized)
    {
        return MB_ERRCODE_INVALID_STATE;
    }
    else
    {
        MBiParam *const p_parm = (MBiParam *) ((((u32)work) + 0x1F) & ~0x1F);
        MB_CommCommonWork *const p_com =
            (MB_CommCommonWork *) ((((u32)p_parm) + sizeof(*p_parm) + 0x1F) & ~0x1F);
        OSIntrMode enabled;

        /*
         * TGID 自動指定の場合は自動取得処理.
         * (内部で RTC を使用するので割り込み禁止の前に取得)
         */
        if (tgid == MB_TGID_AUTO)
        {
            tgid = WM_GetNextTgid();
        }

        enabled = OS_DisableInterrupts();

        /* ライフタイムと省電力モード設定値の初期化 */
        mbi_life_table_no = TABLE_NO;
        mbi_life_frame = FRAME_LIFE_TIME;
        mbi_life_cam = CAM_LIFE_TIME;
        mbi_life_mp = MP_LIFE_TIME;
        mbi_power_save_mode = TRUE;

        /* DMA チャンネル, ワーク領域の初期化 */
        WM_DMA_NO = (u16)dma;
        p_mbi_param = p_parm;
        mbc = p_com;
        MI_CpuClear32(p_parm, sizeof(*p_parm));
        MI_CpuClear16(p_com, sizeof(*p_com));

        {                              /* ユーザ名, ゲーム名を保存 */
            int     i;
            static const u16 *game = L"multiboot";
            u16    *c;
            c = (u16 *)p_parm->uname;
            for (i = 0; i < user->nameLength; ++i)
            {
                *c++ = user->name[i];
            }
            c = (u16 *)p_parm->gname;
            for (i = 0; i < WM_SIZE_GAMENAME; ++i)
            {
                if (*game == 0)
                {
                    break;
                }
                *c++ = *game++;
            }
            MI_CpuCopy8(user, &p_com->user, sizeof(MBUserInfo));
            if (user->nameLength < MB_USER_NAME_LENGTH)
            {
                p_com->user.name[user->nameLength] = 0;
            }
        }

        {
            p_parm->p_sendlen = MB_COMM_P_SENDLEN_DEFAULT;
            p_parm->p_recvlen = MB_COMM_P_RECVLEN_DEFAULT;

            /* 親機情報(子機がScan時に取得する情報)をセット */
            p_parm->sendBufSize = 0;
            p_parm->recvBufSize = 0;

            /* 不要? */
            p_parm->contSend = 1;

            p_parm->recvBuf = (WMmpRecvBuf *)p_com->recvbuf;

            {
                WMParentParam *const p_parent = &p_parm->parentParam;
                /*
                 * 初回ビーコンは entry/mb ともに無効.
                 * MbBeaconのSetGameInfoでONにする.
                 */
                p_parent->entryFlag = 0;
                p_parent->multiBootFlag = 0;
                p_parent->CS_Flag = 1;
                p_parent->KS_Flag = 0;
                /* 自機固有のGGID TGIDをセット */
                p_parent->ggid = ggid;
                p_parent->tgid = (u16)tgid;
                p_parent->beaconPeriod =
                    (u16)(MB_BEACON_PERIOD_DEFAULT + MBi_GetBeaconPeriodDispersion());
                p_parent->maxEntry = 15;
            }
        }

        p_parm->mpBusy = 0;
        p_parm->mbIsStarted = 0;
        p_com->isMbInitialized = 1;

        p_com->start_mp_busy = FALSE;

        (void)OS_RestoreInterrupts(enabled);
    }

    return MB_ERRCODE_SUCCESS;
}

#define MP_USEC_TIME_LIMIT  (5600)

static BOOL MBi_IsCommSizeValid(u16 sendSize, u16 recvSize, u16 entry_num)
{
    int     usec;

    SDK_ASSERT(entry_num > 0 && entry_num <= 15);

    /* sendSizeが規定範囲外の場合、無効なサイズ設定と判定 */
    if (sendSize > MB_COMM_P_SENDLEN_MAX || sendSize < MB_COMM_P_SENDLEN_MIN)
    {
        OS_TWarning("MB Parent send buffer size is out of the range.[%3d - %3d Bytes]\n",
                    MB_COMM_P_SENDLEN_MIN, MB_COMM_P_SENDLEN_MAX);
        return FALSE;
    }

    /* recvSizeが規定範囲外の場合、無効なサイズ設定と判定 */
    if (recvSize > MB_COMM_P_RECVLEN_MAX || recvSize < MB_COMM_P_RECVLEN_MIN)
    {
        OS_TWarning
            ("MB Parent receive buffer size per child is out of the range.[%3d - %3d Bytes]\n",
             MB_COMM_P_RECVLEN_MIN, MB_COMM_P_RECVLEN_MAX);
        return FALSE;
    }

    /* 1回のMP通信にかかる時間に対する評価 */
    usec = 330 + 4 * (sendSize + 38) + entry_num * (112 + 4 * (recvSize + 32));

    /* 所要時間が5600usを超過した場合、無効なサイズ設定と判定される。 */
    if (usec >= MP_USEC_TIME_LIMIT)
    {
        OS_TWarning("These send receive sizes require lower than %4dusec\n"
                    "it exceeds %4d usec\n", MP_USEC_TIME_LIMIT, usec);
        return FALSE;
    }

    return TRUE;
}


BOOL MB_SetParentCommParam(u16 sendSize, u16 maxChildren)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    /* WMが起動していたら、変更不可 */
    if (p_mbi_param->mbIsStarted)
    {
        OS_TWarning("MB has Already started\n");

        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    /* 送受信データサイズの正当性を評価 */
    if (FALSE == MBi_IsCommSizeValid(sendSize, MB_COMM_P_RECVLEN_DEFAULT, maxChildren))
    {
        OS_TWarning("MP data sizes have not changed\n");
        // 
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    /* 最大接続人数のセット */
    p_mbi_param->parentParam.maxEntry = maxChildren;

    /* 送受信データサイズのセット */
    p_mbi_param->p_sendlen = sendSize;
    p_mbi_param->p_recvlen = MB_COMM_P_RECVLEN_DEFAULT;

    (void)OS_RestoreInterrupts(enabled);

    return TRUE;
}


BOOL MB_SetParentCommSize(u16 sendSize)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    BOOL    ret;

    /* 最大接続人数は、現在の設定を引き継ぐ */
    ret = MB_SetParentCommParam(sendSize, p_mbi_param->parentParam.maxEntry);

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}

u16 MB_GetTgid(void)
{
    return p_mbi_param->parentParam.tgid;
}

/* 初期化（親子共通部分） */
static int MBi_StartCommon(void)
{
    int     errcode;

    p_mbi_param->mpStarted = 0;
    p_mbi_param->child_bitmap = 0;
    p_mbi_param->endReq = 0;
    p_mbi_param->currentTgid = 0;
    MBi_SetMaxScanTime(MB_SCAN_TIME_NORMAL);

    if (!mbc->is_started_ex)
    {
        do
        {
            errcode = WM_Initialize(wmBuf, p_mbi_param->callback_ptr, WM_DMA_NO);
        }
        while (errcode == WM_ERRCODE_WM_DISABLE);
        if (errcode != WM_ERRCODE_OPERATING)
        {
            OS_TWarning("WM_Initialize failed!\n");
            return MB_ERRCODE_WM_FAILURE;
        }
        else
        {
            (void)WM_SetIndCallback(p_mbi_param->callback_ptr);
            p_mbi_param->mbIsStarted = 1;
            return MB_ERRCODE_SUCCESS;
        }
    }
    else
    {
        (void)WM_SetIndCallback(p_mbi_param->callback_ptr);
        p_mbi_param->mbIsStarted = 1;
        MBi_OnInitializeDone();
        return MB_ERRCODE_SUCCESS;
    }
}


/* 親機パラメータ設定 & スタート 共通処理 */
static int MBi_StartParentCore(int channel)
{
    int     i, ret;
    MBCommPStateCallback cb_tmp;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    p_mbi_param->parentParam.channel = (u16)channel;
    wmBuf = (u8 *)((((u32)mbc) + sizeof(MB_CommPWork) + 31) & ~31);

    /* 既にCallbackがセットされていた場合のために、テンポラリに退避 */
    cb_tmp = pPwork->parent_callback;

    /* 親機固有のワーク領域をクリア */
    MI_CpuClear16((void *)((u32)pPwork + sizeof(MB_CommCommonWork)),
                  sizeof(MB_CommPWork) - sizeof(MB_CommCommonWork));
    MB_CommSetParentStateCallback(cb_tmp);

    /* 送受信サイズ依存のパラメータ算出 */
    mbc->block_size_max = MB_COMM_CALC_BLOCK_SIZE(p_mbi_param->p_sendlen);

    MBi_SetChildMPMaxSize(p_mbi_param->p_recvlen);
    MBi_SetParentPieceBuffer(&pPwork->req_data_buf);

    for (i = 0; i < MB_MAX_CHILD; i++)
    {
        pPwork->p_comm_state[i] = MB_COMM_PSTATE_NONE;
        pPwork->fileid_of_child[i] = -1;        /* 子機からのリクエストFileIDを初期化 */
    }
    pPwork->file_num = 0;

    MI_CpuClear16(&pPwork->fileinfo[0], sizeof(pPwork->fileinfo));
    MI_CpuClear8(&pPwork->req2child[0], sizeof(pPwork->req2child));

    p_mbi_param->mode = MB_MODE_PARENT;
    p_mbi_param->callback = MBi_CommParentCallback;
    p_mbi_param->callback_ptr = MBi_ParentCallback;

    /* 親機最大送信サイズ(Byte) */
    p_mbi_param->parentParam.parentMaxSize = p_mbi_param->p_sendlen;
    p_mbi_param->sendBufSize = WM_CalcParentSendBufSize(p_mbi_param);
    /* 子機最大送信サイズ(Byte) */
    p_mbi_param->parentParam.childMaxSize = p_mbi_param->p_recvlen;
    p_mbi_param->recvBufSize = WM_CalcParentRecvBufSize(p_mbi_param);

    OS_TPrintf("Parent sendSize : %4d\n", p_mbi_param->parentParam.parentMaxSize);
    OS_TPrintf("Parent recvSize : %4d\n", p_mbi_param->parentParam.childMaxSize);
    OS_TPrintf("Parent sendBufSize : %4d\n", p_mbi_param->sendBufSize);
    OS_TPrintf("Parent recvBufSize : %4d\n", p_mbi_param->recvBufSize);

    MB_InitSendGameInfoStatus();

    ret = MBi_StartCommon();

    (void)OS_RestoreInterrupts(enabled);

    pPwork->useWvrFlag = PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7);

    return ret;
}

/* 親機パラメータ設定 & スタート (WM 内部初期化) */
int MB_StartParent(int channel)
{
    mbc->is_started_ex = FALSE;
    return MBi_StartParentCore(channel);
}

/* 親機パラメータ設定 & スタート (WM 外部起動済) */
int MB_StartParentFromIdle(int channel)
{
    mbc->is_started_ex = TRUE;
    return MBi_StartParentCore(channel);
}

/* 子機パラメータ設定 & スタート */
int MB_StartChild(void)
{
    int     ret;
    MBCommCStateCallbackFunc cb_tmp;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    mbc->is_started_ex = FALSE;

    wmBuf = (u8 *)((((u32)mbc) + sizeof(MB_CommCWork) + 31) & ~31);

    /* 既にCallbackがセットされていた場合のために、テンポラリに退避 */
    cb_tmp = pCwork->child_callback;

    /* 子機固有のワーク領域をクリア */
    MI_CpuClear16((void *)((u32)pCwork + sizeof(MB_CommCommonWork)),
                  sizeof(MB_CommCWork) - sizeof(MB_CommCommonWork));
    MB_CommSetChildStateCallback(cb_tmp);

    pCwork->c_comm_state = MB_COMM_CSTATE_NONE;

    p_mbi_param->mode = MB_MODE_CHILD;
    p_mbi_param->callback = MBi_CommChildCallback;
    p_mbi_param->callback_ptr = MBi_ChildCallback;

    p_mbi_param->scanning_flag = FALSE;
    p_mbi_param->scan_channel_flag = TRUE;
    p_mbi_param->last_found_parent_no = -1;

    MBi_SetBeaconRecvStatusBufferDefault();
    MBi_SetScanLockFunc(MBi_ScanLock, MBi_ScanUnlock);
    MB_InitRecvGameInfoStatus();

    ret = MBi_StartCommon();

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}


/* タスクスレッドが終了したかまたは元々起動していなかった場合の無線の終了処理 */
static int MBi_CallReset(void)
{
    int     errcode;
    errcode = WM_Reset(p_mbi_param->callback_ptr);
    MBi_CheckWmErrcode(WM_APIID_RESET, errcode);
    return conv_errcode(errcode);
}

static void MBi_OnReset(MBiTaskInfo * p_task)
{
    (void)p_task;
    (void)MBi_CallReset();
}

/* 通信を終了 */
int MBi_CommEnd(void)
{
    int     ret = WM_ERRCODE_FAILED;
    OSIntrMode enabled = OS_DisableInterrupts();

    /* まだStartを呼び出していなければこの場でただちに終了 */
    if (!p_mbi_param->mbIsStarted)
    {
        MBi_EndCommon(NULL);
    }
    else if (p_mbi_param->endReq == 0)
    {
        p_mbi_param->scanning_flag = FALSE;
        p_mbi_param->endReq = 1;
        /* タスクスレッドが走っていればこれを停止した後にリセット */
        if (MBi_IsTaskAvailable())
        {
            MBi_EndTaskThread(MBi_OnReset);
            ret = WM_ERRCODE_SUCCESS;
        }
        /* そうでなければこの場でリセット */
        else
        {
            ret = MBi_CallReset();
        }
    }

    (void)OS_RestoreInterrupts(enabled);

    return ret;
}

void MB_End(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (mbc->is_started_ex)
    {
        OS_TPanic("MB_End called after MB_StartParentFromIdle! (please call MB_EndToIdle)");
    }

    (void)MBi_CommEnd();

    (void)OS_RestoreInterrupts(enabled);
}

void MB_EndToIdle(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!mbc->is_started_ex)
    {
        OS_TPanic("MB_EndToIdle called after MB_StartParent! (please call MB_End)");
    }

    (void)MBi_CommEnd();

    (void)OS_RestoreInterrupts(enabled);
}

//-------------------------------------
// 子機を切断
//-------------------------------------
void MB_DisconnectChild(u16 aid)
{
    SDK_NULL_ASSERT(pPwork);
    SDK_ASSERT(p_mbi_param->endReq != 1);

    if (WM_Disconnect(MBi_ParentCallback, aid) != WM_ERRCODE_OPERATING)
    {
        OS_TWarning("MB_DisconnectChild failed disconnect child %d\n", aid);
    }

    if (aid == 0 || aid >= 16)
    {
        OS_TWarning("Disconnected Illegal AID No. ---> %2d\n", aid);
        return;
    }

    /*  子機情報を消去
       (切断が起こった場合、そのAIDに関する子機情報は消去する) */
    pPwork->childversion[aid - 1] = 0;
    MI_CpuClear8(&pPwork->childggid[aid - 1], sizeof(u32));
    MI_CpuClear8(&pPwork->childUser[aid - 1], sizeof(MBUserInfo));

    /* 受信バッファをクリア */
    MBi_ClearParentPieceBuffer(aid);

    pPwork->req2child[aid - 1] = MB_COMM_USER_REQ_NONE;

    /* 要求FileIDがセットされていた場合。IDを-1にクリアする。 */
    if (pPwork->fileid_of_child[aid - 1] != -1)
    {
        u8      fileID = (u8)pPwork->fileid_of_child[aid - 1];
        u16     nowChildFlag = pPwork->fileinfo[fileID].gameinfo_child_bmp;
        u16     child = aid;

        pPwork->fileinfo[fileID].gameinfo_child_bmp &= ~(MB_GAMEINFO_CHILD_FLAG(child));
        pPwork->fileinfo[fileID].gameinfo_changed_bmp |= MB_GAMEINFO_CHILD_FLAG(child);
        pPwork->fileid_of_child[child - 1] = -1;
        pPwork->fileinfo[fileID].pollbmp &= ~(0x0001 << child);

        MB_DEBUG_OUTPUT("Update Member (AID:%2d)\n", child);
    }

    /* 接続情報をクリア */
    if (pPwork->child_entry_bmp & (0x0001 << aid))
    {
        pPwork->child_num--;
        pPwork->child_entry_bmp &= ~(0x0001 << aid);
    }
    pPwork->p_comm_state[aid - 1] = MB_COMM_PSTATE_NONE;
}


/* mb_child.c でのみ使用 ****************************************************/

int MBi_GetLastFountParent(void)
{
    return p_mbi_param->last_found_parent_no;
}

WMBssDesc *MBi_GetParentBssDesc(int id)
{
    return p_mbi_param->parent_info[id].parentInfo;
}

static void MBi_ScanLock(u8 *macAddr)
{
    scanParam.maxChannelTime = MB_SCAN_TIME_LOCKING;
    p_mbi_param->scan_channel_flag = FALSE;
    WM_CopyBssid(macAddr, scanParam.bssid);
}

static void MBi_ScanUnlock(void)
{
    static const u8 bss_fill[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    scanParam.maxChannelTime = MB_SCAN_TIME_NORMAL;
    p_mbi_param->scan_channel_flag = TRUE;
    WM_CopyBssid(bss_fill, scanParam.bssid);
}

/* mb_comm でのみ使用 *********************************************************/

// スキャン時間の設定
void MBi_SetMaxScanTime(u16 time)
{
    scanParam.maxChannelTime = time;
}

static int
MBi_SetMPData(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 tmptt,
              u16 pollbmp)
{
#pragma unused( tmptt )
    int     errcode;
    errcode =
        WM_SetMPDataToPort(callback, sendData, sendDataSize, pollbmp, WM_PORT_BT, WM_PRIORITY_LOW);
    MBi_CheckWmErrcode(WM_APIID_SET_MP_DATA, errcode);
    return errcode;
}


int MBi_SendMP(const void *buf, int len, int pollbmp)
{
    int     errcode;

    u16     size = (u16)len;
    u16     pollbitmap = (u16)pollbmp;

    // MP開始前及び、終了要求発行後はMPデータの送信はできない
    if ((p_mbi_param->mpStarted == 0) || (p_mbi_param->endReq == 1))
    {
        return WM_ERRCODE_FAILED;
    }

    switch (p_mbi_param->mode)
    {
    case MB_MODE_PARENT:
        errcode = MBi_SetMPData(p_mbi_param->callback_ptr, (u16 *)buf, size,    // ※MBi_ParentCallbackを使うとmb_childでもParentCallbackがリンクされてしまうので、callback_ptrで代用。
                                (u16)((!p_mbi_param->contSend) ? 1000 : 0), pollbitmap);

        if (errcode == WM_ERRCODE_OPERATING)
        {
            p_mbi_param->mpBusy = 1;
        }

        return conv_errcode(errcode);

    case MB_MODE_CHILD:
        errcode = MBi_SetMPData(MBi_ChildCallback, (u16 *)buf, size, 0, pollbitmap);

        if (errcode == WM_ERRCODE_OPERATING)
        {
            p_mbi_param->mpBusy = 1;
        }

        return conv_errcode(errcode);

    default:
        return WM_ERRCODE_FAILED;
    }
}

int MBi_GetSendBufSize(void)
{
    return (int)p_mbi_param->sendBufSize;
}

int MBi_GetRecvBufSize(void)
{
    return (int)p_mbi_param->recvBufSize;
}

int MBi_CommConnectToParent(const WMBssDesc *bssDescp)
{
    WMgameInfo *gameInfo;
    int     errcode;
    SDK_ASSERT(bssDescp != 0);

    gameInfo = (WMgameInfo *)(&(bssDescp->gameInfo));
    p_mbi_param->p_sendlen = gameInfo->parentMaxSize;
    p_mbi_param->p_recvlen = gameInfo->childMaxSize;
    /* 送受信サイズ依存のパラメータ算出 */
    mbc->block_size_max = MB_COMM_CALC_BLOCK_SIZE(p_mbi_param->p_sendlen);

    MBi_SetChildMPMaxSize(p_mbi_param->p_recvlen);

    OS_TPrintf("\trecv size : %d\n", p_mbi_param->p_sendlen);
    OS_TPrintf("\tsend size : %d\n", p_mbi_param->p_recvlen);
    OS_TPrintf("\tblock size: %d\n", mbc->block_size_max);

    p_mbi_param->recvBufSize = (u16)WM_CalcChildRecvBufSize(bssDescp);
    p_mbi_param->sendBufSize = (u16)WM_CalcChildSendBufSize(bssDescp);
    p_mbi_param->pInfo = bssDescp;
    p_mbi_param->currentTgid = ((WMGameInfo *)&(bssDescp->gameInfo))->tgid;
    p_mbi_param->scanning_flag = FALSE;

    errcode = WM_EndScan(p_mbi_param->callback_ptr);
    MBi_CheckWmErrcode(WM_APIID_END_SCAN, errcode);
    return conv_errcode(errcode);
}

u32 MBi_GetGgid(void)
{
    return p_mbi_param->parentParam.ggid;
}

u16 MBi_GetTgid(void)
{
    return (p_mbi_param->parentParam.tgid);
}

u8 MBi_GetAttribute(void)
{
    return ((u8)(((p_mbi_param->parentParam.entryFlag) ? WM_ATTR_FLAG_ENTRY : 0) |      // entryFlag 最下位ビット
                 ((p_mbi_param->parentParam.multiBootFlag) ? WM_ATTR_FLAG_MB : 0) |     // multiBootFlag 第2ビット
                 ((p_mbi_param->parentParam.KS_Flag) ? WM_ATTR_FLAG_KS : 0) |   // KS_Flag 第3ビット
                 ((p_mbi_param->parentParam.CS_Flag) ? WM_ATTR_FLAG_CS : 0)     // CS_Flag 第4ビット
            ));
}


// スキャンの再開
int MBi_RestartScan(void)
{
    int     errcode;

    p_mbi_param->scanning_flag = TRUE;

    if (p_mbi_param->scan_channel_flag)
    {
        if (FALSE == changeScanChannel(&scanParam))
        {
            (void)MBi_CommEnd();
        }
    }

    errcode = WM_StartScan(MBi_ChildCallback, &scanParam);
    MBi_CheckWmErrcode(WM_APIID_START_SCAN, errcode);
    return conv_errcode(errcode);
}


// スキャンチャンネルの取得
int MBi_GetScanChannel(void)
{
    return scanParam.channel;
}

// 自身のAID取得
u16 MBi_GetAid(void)
{
    return p_mbi_param->my_aid;
}

BOOL MBi_IsStarted(void)
{
    return (p_mbi_param->mbIsStarted == 1) ? TRUE : FALSE;
}

// WM_APIの戻り値チェック
static void MBi_CheckWmErrcode(u16 apiid, int errcode)
{
    u16     arg[2];

    if (errcode != WM_ERRCODE_OPERATING && errcode != WM_ERRCODE_SUCCESS)
    {
        arg[0] = apiid;
        arg[1] = (u16)errcode;
        p_mbi_param->callback(MB_CALLBACK_API_ERROR, arg);
    }
}
