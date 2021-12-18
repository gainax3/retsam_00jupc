//==============================================================================
/**
 * @file	email.h
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.10.19(金)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __EMAIL_H__
#define __EMAIL_H__

#include "system/procsys.h"
#include "libdpw/dpw_common.h"
#include "email_main.h"
	
#define	EMAIL_INPUT_ADDRESS		( EMAIL_MODE_INPUT_EMAIL )
#define	EMAIL_INPUT_NUMBER		( EMAIL_MODE_INPUT_AUTHENTICATE )
#define	EMAIL_INPUT_PASSWORD	( EMAIL_MODE_INPUT_PASSWORD )
#define	EMAIL_INPUT_VIEW		( EMAIL_MODE_INPUT_EMAIL_CHECK )

#define PASS_CODE_MAX		( 4 + 1 )
#define EMAIL_CODE_MAX		( 50 + 1 )
#define EMAIL_NUM_MAX		( 4 + 4 + 1 )
#define EMAIL_PASS_MAX		( 4 + 1 )
#define EMAIL_CODE_1LINE	( 25 )

#define EMAIL_CUR_BACK_CPX	( 5 )
#define EMAIL_CUR_END_CPX	( 7 )

#define EMAIL_CUR_BACK_PX	( 2 )
#define EMAIL_CUR_BACK_PY	( 6 )
#define EMAIL_CUR_END_PX	( 9 )
#define EMAIL_CUR_END_PY	( EMAIL_CUR_BACK_PY )


///< OMA 座標
#define EMAIL_OAM_BTN_OX	( 0 )	///< ボタンのオフセット
#define EMAIL_OAM_BTN_OY	( 2 )
#define EMAIL_OAM_PNL_OX	( 8 )	///< パネルのオフセット
#define EMAIL_OAM_PNL_OY	( 8 )

///< アニメ定義
enum {	
	eEM_ANM_BACK_1	= 0,
	eEM_ANM_BACK_2,
	eEM_ANM_END_1,
	eEM_ANM_END_2,
	eEM_ANM_CUR_1,
	eEM_ANM_BAR_1,		///< 未使用
	eEM_ANM_BAR_2,
	eEM_ANM_CUR_2,
	eEM_ANM_CUR_BIG,
};

enum {
	
	eINFO_ADRS_INPUT = 0,	///< e-mail入力して
	eINFO_NUMBER_INPUT,		///< 認証コード入力して
	eINFO_PASS_INPUT,		///< パスワード入力して
	eINFO_ADRS_RE_INPUT,	///< e-mailもっかい
	eINFO_ADRS_ERR,			///< e-mailちがうー
	eINFO_PASS_ERR,			///< パスがちがうー
	
	eINFO_MAIL_ADDRESS = 0xFF,
};

enum {	
	EMAIL_CODE_0	= 0,
	EMAIL_CODE_ETC	= 10,
	EMAIL_CODE_A	= 15,
	EMAIL_CODE_a	= 41,
	EMAIL_BACK		= 67,	
	EMAIL_END,
	
	EMAIL_BTN_MAX	= 10 + 5 + 52 + 2,
};

typedef struct {
	
	int mode;		///< EMailMode参照
	STRBUF* now_str;	///< 現在設定されているメールアドレス
	STRBUF* str;		///< 格納先(呼び出し時は現在設定されているアドレスが入っている)

	u16 authenticate_rand_code;		///<ローカルで決定される認証コードの上3桁
	u16 password;					///<ユーザーが設定済みのパスワード
	
	//戻り値
	u8 ret_address_flag;			///<アドレス入力画面を終了した時の状態(EMAIL_ADDRESS_RET_???)
	u32 ret_password;				///<パスワード入力の結果値
	u32 ret_authenticate_code;		///<認証コード入力の結果値(7桁)orEMAIL_AUTHENTICATE_CODE_CANCEL
	
} EMAIL_PARAM;

extern const PROC_DATA EMail_ProcData;

extern PROC_RESULT EMail_Proc_Init( PROC* proc, int* seq );
extern PROC_RESULT EMail_Proc_Main( PROC* proc, int* seq );
extern PROC_RESULT EMail_Proc_Exit( PROC* proc, int* seq );


#endif

