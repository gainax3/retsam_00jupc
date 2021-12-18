/*---------------------------------------------------------------------------*
  Project:  DP WiFi Library
  File:     dpw_common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	DP WiFi Common ライブラリ
	
	@author	Yamaguchi Ryo(yamaguchi_ryo@nintendo.co.jp)
	
*/

#ifndef DPW_COMMON_H_
#define DPW_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NITRO
#include <dwc.h>
#else
#include <nitro.h>
#include <dwc.h>
#endif

// ----------------------------------------------------------------------------
// localize_spec_mark(LANG_ALL) imatake 2008/07/29
// PG5_WIFIRELEASE = yes なら、Wi-Fi関係がリリースサーバに接続
#ifdef PG5_WIFIRELEASE
// リリースサーバに接続するには下のコメントアウトを削除してください。
#define DPW_SERVER_PUBLIC
#endif
// ----------------------------------------------------------------------------

/*-----------------------------------------------------------------------*
					型・定数宣言
 *-----------------------------------------------------------------------*/

//! メールアドレスの長さ(NULL終端を除く)
#define DPW_MAIL_ADDR_LENGTH 55

//! メールアドレスの認証を開始するパスワード
#define DPW_MAIL_ADDR_AUTH_START_PASSWORD 0xffff

//! メールアドレスの認証を強制的に通すデバッグ用パスワード、常に認証結果がDPW_PROFILE_AUTHRESULT_SUCCESSになります。認証メールは送信されません。
#define DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD 9999

//! Dpw_Common_Profile構造体のmailRecvFlagメンバのビットフラグ
typedef enum {
    DPW_PROFILE_MAILRECVFLAG_EXCHANGE = 0x1 //!< ポケモンが交換されたときのメールを受信するか
}DPW_PROFILE_MAILRECVFLAG;

//! Dpw_Common_Profile構造体のflagメンバのビットフラグ
typedef enum {
    DPW_PROFILE_FLAG_HANGEUL_AVAILABLE = 0x1 //!< ハングル文字を表示できるか
}DPW_PROFILE_FLAG;

//! 自分の情報登録用構造体
typedef struct {
    u8      version;                        //!< バージョン
    u8      language;                       //!< 言語コード
	u8      countryCode;                    //!< 住んでいる国コード
	u8      localCode;                      //!< 住んでいる地方コード
	u32     playerId;			            //!< プレイヤーID
	u16     playerName[8];		            //!< プレイヤー名(ポケモンコード)
    u32     flag;                           //!< 各種フラグ、DPW_PROFILE_FLAG列挙体の値をセットしてください。
    u8      macAddr[6];                     //!< MACアドレス、ライブラリ内で格納するのでセットする必要はありません。
    u8      reserved[2];                    //!< パディング
    char    mailAddr[DPW_MAIL_ADDR_LENGTH+1]; //!< メールアドレス。ASCII文字列をNULL終端したものをセットしてください。メール送信機能を使用しない場合は空文字を入れてください。
    u32     mailRecvFlag;                   //!< メール受信フラグ、DPW_PROFILE_MAILRECVFLAG列挙体の値をセットしてください。
    u16     mailAddrAuthVerification;       //!< メールアドレス認証用確認コード。0～999の整数を入力してください。
    u16     mailAddrAuthPass;               //!< メールアドレス認証パスワード。認証を開始するときはDPW_MAIL_ADDR_AUTH_START_PASSWORDをセットしてください。DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORDを指定すると強制的に認証を通します。(デバッグ用)
} Dpw_Common_Profile;

//! Dpw_Common_ProfileResult構造体のcodeメンバに格納される結果
typedef enum {
    DPW_PROFILE_RESULTCODE_SUCCESS,             //!< 情報の登録に成功しました。mailAddrAuthResultメンバに認証結果が格納されます。
    DPW_PROFILE_RESULTCODE_ERROR_INVALIDPARAM,  //!< 送信したパラメータが不正です。
    DPW_PROFILE_RESULTCODE_ERROR_SERVERSTATE    //!< サーバの状態がメンテナンスもしくは一時停止中です。
} DPW_PROFILE_RESULTCODE;

//! Dpw_Common_ProfileResult構造体のmailAddrAuthResultメンバ(メールアドレス認証結果)に格納される結果。
typedef enum {
    DPW_PROFILE_AUTHRESULT_SUCCESS,     //!< 認証成功。メール機能が有効になり、メールの有効期限が31日に延長されました。メールアドレスに空文字を入れた場合は必ずこの値が返り、メール機能が無効になります。
    DPW_PROFILE_AUTHRESULT_SEND,        //!< 認証メールを送信しました。認証メールに書かれているパスワードをmailAddrAuthPassにセットし、再度Dpw_xx_SetProfileAsync関数を呼んでください。
    DPW_PROFILE_AUTHRESULT_SENDFAILURE, //!< 認証メールの送信に失敗しました。おそらくメールアドレスが間違っています。
    DPW_PROFILE_AUTHRESULT_FAILURE      //!< 認証に失敗しました。認証メールに記載されているパスワードと送信されたパスワードが違います。もしくは認証メールの送信先と異なるメールアドレスです。
} DPW_PROFILE_AUTHRESULT;

//! 自分の情報登録レスポンス用構造体
typedef struct {
    u32 code;                   //!< 結果コード。DPW_PROFILE_RESULTCODE列挙体の値が格納されます。
    u32 mailAddrAuthResult;     //!< メールアドレス認証結果。codeメンバにDPW_PROFILE_RESULTCODE_SUCCESSがセットされた場合のみ、ここにDPW_PROFILE_AUTHRESULT列挙体の値が格納されます。ただしDP韓国語版では必ず0になります。
} Dpw_Common_ProfileResult;

/*-----------------------------------------------------------------------*
					グローバル変数定義
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					関数外部宣言
 *-----------------------------------------------------------------------*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // DPW_COMMON_H_