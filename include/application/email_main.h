//==============================================================================
/**
 * @file	email_main.h
 * @brief	Eメール画面のヘッダ
 * @author	matsuda
 * @date	2007.10.18(木)
 */
//==============================================================================
#ifndef __EMAIL_MAIN_H__
#define __EMAIL_MAIN_H__

#include "gflib/strbuf.h"


//==============================================================================
//	定数定義
//==============================================================================
///サブプロセス指定
enum{
	EMAIL_SUBPROC_MENU,					///<メニュー
	EMAIL_SUBPROC_ADDRESS_INPUT,		///<Eメール入力
	EMAIL_SUBPROC_AUTHENTICATE_INPUT,	///<認証番号入力
	EMAIL_SUBPROC_PASSWORD_INPUT,		///<パスワード入力
	EMAIL_SUBPROC_ADDRESS_CHECK,		///<Eメール確認
	EMAIL_SUBPROC_GSPROFILE_GET,		///<自分のプロファイルID取得モード
	
	EMAIL_SUBPROC_END,
};


///入力画面の起動モード
enum{
	EMAIL_MODE_INPUT_EMAIL,			///<Eメール入力モード
	EMAIL_MODE_INPUT_AUTHENTICATE,	///<認証番号入力モード
	EMAIL_MODE_INPUT_PASSWORD,		///<パスワード入力モード
	EMAIL_MODE_INPUT_EMAIL_CHECK,	///<Eメール確認モード
};

///認証コード入力をキャンセル終了した場合の返事
#define EMAIL_AUTHENTICATE_CODE_CANCEL		(0xffffffff)

///Eメールアドレスの文字数サイズ(セーブではなくシーンで使用するヒープサイズ用)
#define EMAIL_ADDRESS_LEN_SIZE		(100)	//STRBUFのサイズなので余裕をもって100文字分

///アドレス入力画面を終了した時の戻り値
enum{
	EMAIL_ADDRESS_RET_SET,			///<アドレスをセットして終了
	EMAIL_ADDRESS_RET_CANCEL,		///<アドレスをセットせずにキャンセルして終了
};


//==============================================================================
//	構造体定義
//==============================================================================
///Eメール画面制御用システムワーク構造体
typedef struct{
	PROC *sub_proc;					///<実行中のサブプロセス
	SAVEDATA *savedata;				///<セーブデータへのポインタ
	CONFIG *config;					///<コンフィグ

	void 			*heapPtr;		///<NitroDWCに渡すヒープワークの解放用ポインタ
	NNSFndHeapHandle heapHandle;	///<heapPtrを32バイトアライメントに合わせたポインタ

	DWCInetControl   stConnCtrl;							// DWC接続ワーク
	
	u16 authenticate_rand_code;		///<ローカルで決定される認証コードの上3桁
	
	u8 sub_menu_recovery_mode;		///<メニュー起動時のモード

	int comm_initialize_ok;

	int now_process;						// 現在実行中のサブプロセス
	int	sub_process;						// 世界交換サブプログラム制御ナンバー
	int	sub_nextprocess;					// 世界交換サブNEXT制御ナンバー
	int sub_process_mode;					// 
	int	sub_returnprocess;					// nextプロセスを呼んで終了した時の戻りプロセス

	void *sub_proc_parent_work;		///<サブPROC呼び出し時に作成するparent_work保持ポインタ

	Dpw_Common_Profile dc_profile;					///<送信プロフィールデータ
	Dpw_Common_ProfileResult dc_profile_result;		///<送信結果受信ワーク
	
	//各サブPROC間のデータのやり取りで使用
	u32 ret_password;		///<パスワード入力の結果値
	u32 ret_authenticate_code;	///<認証コード入力の結果値(下4桁)
	u8 ret_address_flag;		///<Eメールアドレス入力画面を終了した時の状態
	u8 subproc_padding[3];	//パディング
	STRBUF *email_address;			///<Eメールアドレスの文字列セット場所
	STRBUF *now_email_address;		///<現在設定されているEメールアドレス

	BOOL dpw_tr_init;		///<TRUE:Dpw_Tr_Init実行済み
}EMAIL_SYSWORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
//-- email_main.c --//
extern PROC_RESULT EmailProc_Init( PROC * proc, int * seq );
extern PROC_RESULT EmailProc_Main( PROC * proc, int * seq );
extern PROC_RESULT EmailProc_End(PROC *proc, int *seq);
extern void Email_SubProcessChange( EMAIL_SYSWORK *esys, int subprccess, int mode );
extern void Email_SubProcessEndSet(EMAIL_SYSWORK *esys);
extern void Email_RecoveryMenuModeSet(EMAIL_SYSWORK *esys, int mode);
extern int Email_RecoveryMenuModeGet(EMAIL_SYSWORK *esys);
extern void Email_AddressSaveWorkSet(EMAIL_SYSWORK *esys);
extern void Email_AuthenticateCodeSaveWorkSet(EMAIL_SYSWORK *esys);
extern void Email_PasswordSaveWorkSet(EMAIL_SYSWORK *esys);
extern void Email_DCProfileCreate(EMAIL_SYSWORK *esys);
extern void Email_DCProfileSet_Address(EMAIL_SYSWORK *esys);
extern void Email_DCProfileSet_Authenticate(EMAIL_SYSWORK *esys);
extern void Email_AuthenticateRandCodeSet(EMAIL_SYSWORK *esys, u32 code);
extern u32 Email_AuthenticateRandCodeGet(EMAIL_SYSWORK *esys);
extern void Email_AuthenticateCodeSet(EMAIL_SYSWORK *esys, u32 code);
extern u32 Email_AuthenticateCodeGet(EMAIL_SYSWORK *esys);
extern void Email_PasswordNumberSet(EMAIL_SYSWORK *esys, u32 password);
extern u32 Email_PasswordNumberGet(EMAIL_SYSWORK *esys);
extern void Email_AddressReturnFlagSet(EMAIL_SYSWORK *esys, int flag);
extern int Email_AddressReturnFlagGet(EMAIL_SYSWORK *esys);
extern STRBUF * Email_AddressStrbufGet(EMAIL_SYSWORK *esys);

//-- email_enter.c --//
extern PROC_RESULT EmailMenu_Enter_Init( PROC * proc, int * seq );
extern PROC_RESULT EmailMenu_Enter_Main( PROC * proc, int * seq );
extern PROC_RESULT EmailMenu_Enter_End(PROC *proc, int *seq);

//-- email_input.c --//
extern PROC_RESULT EmailInput_Init( PROC * proc, int * seq );
extern PROC_RESULT EmailInput_Main( PROC * proc, int * seq );
extern PROC_RESULT EmailInput_End(PROC *proc, int *seq);

//-- email_tool.c --//
extern void Email_Strcode_to_Ascii(STRBUF *src, char *dest, int heap_id);
extern void Email_Ascii_to_Strcode(char *src, STRBUF *dest, int heap_id);

//-- email_test.c ※後で消す --//
extern PROC_RESULT EmailAddress_Init( PROC * proc, int * seq );
extern PROC_RESULT EmailAddress_Main( PROC * proc, int * seq );
extern PROC_RESULT EmailAddress_End(PROC *proc, int *seq);


#endif	//__EMAIL_MAIN_H__
