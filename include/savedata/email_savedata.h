//==============================================================================
/**
 * @file	email_savedata.h
 * @brief	Eメールセーブデータ操作関連
 * @author	matsuda
 * @date	2007.10.23(火)
 */
//==============================================================================
#ifndef	__EMAIL_SAVEDATA_H__
#define	__EMAIL_SAVEDATA_H__

#include "gflib/strbuf.h"
#include "libdpw/dpw_common.h"


//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	コンテストデータの不完全型定義
 */
//---------------------------------------------------------------------------
typedef struct _EMAIL_SAVEDATA EMAIL_SAVEDATA;


//==============================================================================
//	定数定義
//==============================================================================
///パラメータセットモード
enum{
	EMAIL_PARAM_RECV_FLAG,			///<受信設定フラグ(DPW_PROFILE_MAILRECVFLAG_EXCHANGE or 0)
	EMAIL_PARAM_AUTH_CODE_HIGH,		///<認証コード(上3桁)
	EMAIL_PARAM_AUTH_CODE_LOW,		///<認証コード(下4桁)
	EMAIL_PARAM_PASSWORD,			///<ユーザーが決定するパスワード
};


//==============================================================================
//	外部関数宣言
//==============================================================================
extern int	EMAILSAVE_GetWorkSize(void);
extern void EMAILSAVE_Init(EMAIL_SAVEDATA * emaildata);
extern void EMAILSAVE_DataInitialize(SAVEDATA *sv);
extern BOOL EMAILSAVE_UseCheck(SAVEDATA *sv);
extern void EMAILSAVE_AddressSet(SAVEDATA *sv, const char *address);
extern char * EMAILSAVE_AddressGet(SAVEDATA *sv);
extern void EMAILSAVE_ParamSet(SAVEDATA *sv, int param_id, u32 data);
extern u32 EMAILSAVE_ParamGet(SAVEDATA *sv, int param_id);
extern u32 EMAILSAVE_DCProfileCreate_AuthStart(SAVEDATA *sv, Dpw_Common_Profile *dc_profile);
extern void EMAILSAVE_DCProfileCreate_Update(SAVEDATA *sv, Dpw_Common_Profile *dc_profile);


#endif	__EMAIL_SAVEDATA_H__
