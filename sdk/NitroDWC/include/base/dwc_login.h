/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_login
  File:     dwc_login.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_login.h,v $
  Revision 1.18  2007/04/04 08:46:56  nakata
  ログイン処理で新規アカウント作成時にログインログアウトを行わないように変更

  Revision 1.17  2006/03/07 01:18:16  nakata
  NASに関連する機能追加をdwc_nasfunc.c/dwc_nasfunc.hに集約する際に必要な変更を行った。

  Revision 1.16  2006/01/23 04:50:43  arakit
  統一用語に合わせてコメントを修正

  Revision 1.15  2005/12/26 06:57:44  nakata
  Changed DWC_AUTH_INGAMESN_NOTCHECKED to DWC_AUTH_INGAMESN_NOT_CHECKED.
  Changed DWC_INGAMESN_NOTCHECKED to DWC_INGAMESN_NOT_CHECKED.

  Revision 1.14  2005/12/26 06:43:20  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.13  2005/12/21 03:08:54  arakit
  ログインの際にサーバにゲーム内スクリーンネームを渡せるようにした。

  Revision 1.12  2005/11/04 05:04:49  arakit
  認証用データを取得する内部関数DWCi_GetAuthInfo()を追加した。

  Revision 1.11  2005/11/02 02:49:33  arakit
  ファイルのインクルード関係を調整した。

  Revision 1.10  2005/10/07 06:48:45  sasakit
  gpConnectのタイムアウトを入れた。

  Revision 1.9  2005/10/06 00:15:11  sasakit
  接続中状態を取得できるようにした。

  Revision 1.8  2005/10/01 10:30:49  sasakit
  ログイン時に、GSのAvailableCheckに失敗すると、フリーズする問題を修正。

  Revision 1.7  2005/09/05 13:42:15  sasakit
  Login時の認証エラーの処理を追加。

  Revision 1.6  2005/08/26 08:15:22  arakit
  ・ASSERTとモジュール制御変数のチェックを整理した。
  ・外部関数DWC_ShutdownFriendsMatch()の引数をとらないように変更した。
  また、同関数から各モジュールの制御変数をNULLクリアするようにした。
  ・エラー処理関数を変更し、エラーセット、エラーチェックを必要な箇所に入れた。

  Revision 1.5  2005/08/20 07:01:19  sasakit
  ヘッダインクルードガードの書式を統一した。
  bm/dwc_init.h -> bm/dwc_bm_init.hに変更
  ヘッダにCopyrightをつけた。
  構造体の名前空間をできるだけルールにちかづけた。
  util_wifiidtool.hとdwc_backup.hの関係を若干修正。


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_LOGIN_H_
#define DWC_LOGIN_H_

#include "auth/dwc_auth.h"

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define DWC_LOGIN_AUTH_TIMEOUT       ( 10 * 1000 ) // [ms]
#define DWC_LOGIN_GPCONNECT_TIMEOUT  ( 60 * 1000 ) // [ms]


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// ログイン進行状況列挙子
typedef enum {
    DWC_LOGIN_STATE_INIT = 0,         // 初期状態
    DWC_LOGIN_STATE_REMOTE_AUTH,      // リモート認証中
    DWC_LOGIN_STATE_CONNECTING,       // GPサーバ接続中
    DWC_LOGIN_STATE_GPGETINFO,        // GPにloginしたときのlastname取得時
    DWC_LOGIN_STATE_GPSETINFO,        // GPに初めてloginしたときのlastname設定
    DWC_LOGIN_STATE_CONNECTED,        // コネクト完了状態
    DWC_LOGIN_STATE_NUM
} DWCLoginState;

//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  ログイン完了コールバック型
  引数　：error     DWCエラー種別
          profileID 取得できた自分のプロファイルID
          param     コールバック用パラメータ
  戻り値：なし
 *---------------------------------------------------------------------------*/
typedef void (*DWCLoginCallback)(DWCError error, int profileID, void *param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// ログイン制御構造体
typedef struct DWCstLoginControl
{
    GPConnection* pGpObj;       // gpコネクション構造体ポインタへのポインタ
    DWCLoginState state;        // ログイン進行状況
    int  productID;             // GameSpyが配布するゲームのプロダクトID
    u32  gamecode;              // 任天堂がゲームに割り当てるgamecode
    //const char* userID;         // 認証用ユーザID
    //const char* password;       // 認証用パスワード
    const u16* playerName;      // プレイヤー名（ゲーム内スクリーンネーム）へのポインタ
    DWCLoginCallback callback;  // ログイン完了コールバック
    void *param;                // 上記コールバック用パラメータ

    DWCUserData* userdata;      // ユーザデータへのポインタ

    // working
    void* bmwork;
    void* http;
    OSTick startTick;
    u32    connectFlag;
    OSTick connectTick;

    DWCAccLoginId tempLoginId;

    char authToken[GP_AUTHTOKEN_LEN];  // 認証用トークン
    char partnerChallenge[GP_PARTNERCHALLENGE_LEN];  // 認証用チャレンジ値

    char username[DWC_ACC_USERNAME_STRING_BUFSIZE];
    
    // [nakata] GPログイン時情報の退避場所
    GPConnectResponseArg    gpconnectresponsearg;

} DWCLoginControl;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern void DWCi_LoginInit(DWCLoginControl* logcnt, DWCUserData* userdata, GPConnection* pGpObj, int productID, u32 gamecode, const u16* playerName, DWCLoginCallback callback, void* param);


extern void DWCi_LoginAsync( void );


extern void DWCi_LoginProcess(void);


extern DWCAccUserData* DWCi_GetUserData(void);


extern void DWCi_StopLogin(DWCError error, int errorCode);


extern void DWCi_ShutdownLogin(void);


extern BOOL DWCi_CheckLogin( void );


extern BOOL DWCi_GetAuthInfo( char** authToken, char** partnerChallenge );


#ifdef __cplusplus
}
#endif


#endif // DWC_LOGIN_H_
