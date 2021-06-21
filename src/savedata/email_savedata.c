//==============================================================================
/**
 * @file	email_savedata.c
 * @brief	Eメールセーブデータ操作
 * @author	matsuda
 * @date	2007.10.23(火)
 */
//==============================================================================
#include "common.h"
#include "savedata/savedata_def.h"
#include "savedata/savedata.h"
#include "savedata/email_savedata.h"
#include "gflib/strcode.h"
#include "gflib/strbuf_family.h"
#include "system/buflen.h"
#include "libdpw/dpw_common.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "system/pm_str.h"


//==============================================================================
//	定数定義
//==============================================================================
///Eメールアドレスの文字数(終端'\0'込み)
#define EMAIL_STR_LEN				(50 + 1)


//==============================================================================
//	構造体定義
//==============================================================================
///Eメールセーブデータ構造体
struct _EMAIL_SAVEDATA{
	char email_ascii[EMAIL_STR_LEN];	///<Eメールアドレス
	int recv_flag;
	u16 auth_code_high;					///<認証コード(上3桁)
	u16 auth_code_low;					///<認証コード(下4桁)
	u32 password;						///<パスワード
};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void EMAILSAVE_DCProfileCreateCommon(SAVEDATA *sv, Dpw_Common_Profile *dc_profile);



//--------------------------------------------------------------
/**
 * @brief	Eメールのセーブデータサイズを取得
 *
 * @param	none	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
int	EMAILSAVE_GetWorkSize(void)
{
	return sizeof(EMAIL_SAVEDATA);
}

//---------------------------------------------------------------------------
/**
 * @brief	Eメールセーブデータの初期化処理
 * @param	emaildata		Eメールセーブデータへのポインタ
 */
//---------------------------------------------------------------------------
void EMAILSAVE_Init(EMAIL_SAVEDATA * emaildata)
{
	MI_CpuClear8(emaildata, sizeof(EMAIL_SAVEDATA));
	
	memset(emaildata->email_ascii, '\0', EMAIL_STR_LEN);
	emaildata->recv_flag = DPW_PROFILE_MAILRECVFLAG_EXCHANGE;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 * @brief   Eメールセーブデータを初期化状態にする
 *
 * @param   sv		セーブデータへのポインタ
 *
 * セーブシステム以外の場所から初期化する場合、この関数を使用する
 */
//--------------------------------------------------------------
void EMAILSAVE_DataInitialize(SAVEDATA *sv)
{
	EMAILSAVE_Init(SaveData_Get(sv, GMDATA_ID_EMAIL));
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレスが設定されているか調べる
 *
 * @param   sv		セーブデータへのポインタ
 *
 * @retval  TRUE:設定されている。
 * @retval  FALSE:設定されていない
 */
//--------------------------------------------------------------
BOOL EMAILSAVE_UseCheck(SAVEDATA *sv)
{
	EMAIL_SAVEDATA *emaildata;
	
	emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);
	if(emaildata->email_ascii[0] == '\0'){
		return FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレスをセットする
 *
 * @param   sv			セーブデータへのポインタ
 * @param   address		セットするEメールアドレス(ASCIIコード)
 */
//--------------------------------------------------------------
void EMAILSAVE_AddressSet(SAVEDATA *sv, const char *address)
{
	EMAIL_SAVEDATA *emaildata;
	
	emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);
	strcpy(emaildata->email_ascii, address);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 * @brief   Eメールアドレスを取得する(ASCIIコード)
 *
 * @param   sv			セーブデータへのポインタ
 *
 * @retval  TRUE:Eメールアドレスがセットされている
 * @retval  FALSE:Eメールアドレスはセットされていない
 */
//--------------------------------------------------------------
char * EMAILSAVE_AddressGet(SAVEDATA *sv)
{
	EMAIL_SAVEDATA *emaildata;
	
	emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);
	return emaildata->email_ascii;
}

//--------------------------------------------------------------
/**
 * @brief   Eメールセーブデータ：パラメータセット
 *
 * @param   sv				セーブデータへのポインタ
 * @param   param_id		パラメータID
 * @param   data			データ
 */
//--------------------------------------------------------------
void EMAILSAVE_ParamSet(SAVEDATA *sv, int param_id, u32 data)
{
	EMAIL_SAVEDATA *emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);

	switch(param_id){
	case EMAIL_PARAM_RECV_FLAG:		// DPW_PROFILE_MAILRECVFLAG_EXCHANGE or 0
		emaildata->recv_flag = data;
		break;
	case EMAIL_PARAM_AUTH_CODE_HIGH:
		emaildata->auth_code_high = data;
		break;
	case EMAIL_PARAM_AUTH_CODE_LOW:
		emaildata->auth_code_low = data;
		break;
	case EMAIL_PARAM_PASSWORD:
		emaildata->password = data;
		break;
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 * @brief   Eメールセーブデータ：パラメータ取得
 *
 * @param   sv				セーブデータへのポインタ
 * @param   param_id		パラメータID
 * 
 * @retval	データ
 */
//--------------------------------------------------------------
u32 EMAILSAVE_ParamGet(SAVEDATA *sv, int param_id)
{
	EMAIL_SAVEDATA *emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);

	switch(param_id){
	case EMAIL_PARAM_RECV_FLAG:		// DPW_PROFILE_MAILRECVFLAG_EXCHANGE or 0
		return emaildata->recv_flag;
	case EMAIL_PARAM_AUTH_CODE_HIGH:
		return emaildata->auth_code_high;
	case EMAIL_PARAM_AUTH_CODE_LOW:
		return emaildata->auth_code_low;
	case EMAIL_PARAM_PASSWORD:
		return emaildata->password;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * @brief   プロフィールデータ共通部分作成
 *
 * @param   sv				セーブデータへのポインタ
 * @param   dc_profile		プロフィール代入先
 */
//--------------------------------------------------------------
static void EMAILSAVE_DCProfileCreateCommon(SAVEDATA *sv, Dpw_Common_Profile *dc_profile)
{
	WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
	MYSTATUS *my = SaveData_GetMyStatus(sv);
	char *email_address = EMAILSAVE_AddressGet(sv);
	
	MI_CpuClear8(dc_profile, sizeof(Dpw_Common_Profile));
	
	dc_profile->version = PM_VERSION;
	dc_profile->language = PM_LANG;
	dc_profile->countryCode = WIFIHISTORY_GetMyNation(wh);
	dc_profile->localCode = WIFIHISTORY_GetMyArea(wh);
	dc_profile->playerId = MyStatus_GetID(my);
	
	PM_strcpy(dc_profile->playerName, MyStatus_GetMyName(my));
	
	dc_profile->flag = 0;	//ハングル文字を表示できるか
//	dc_profile->macAddr		ライブラリ内で格納するのでセットの必要なし
	
	strcpy(dc_profile->mailAddr, email_address);

	dc_profile->mailRecvFlag = EMAILSAVE_ParamGet(sv, EMAIL_PARAM_RECV_FLAG);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 * @brief   認証開始用プロフィール作成
 *
 * @param   sv				セーブデータへのポインタ
 * @param   dc_profile		プロフィール代入先
 *
 * @retval  認証用にローカルで決定された登録コード上3桁の番号
 */
//--------------------------------------------------------------
u32 EMAILSAVE_DCProfileCreate_AuthStart(SAVEDATA *sv, Dpw_Common_Profile *dc_profile)
{
	u32 AuthVerification;
	
	EMAILSAVE_DCProfileCreateCommon(sv, dc_profile);
	
	AuthVerification = gf_rand() % 1000;	//0～999の値をランダムでセット
	
	dc_profile->mailAddrAuthVerification = AuthVerification;
	dc_profile->mailAddrAuthPass = DPW_MAIL_ADDR_AUTH_START_PASSWORD;
	
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
	return AuthVerification;
}

//--------------------------------------------------------------
/**
 * @brief   世界交換、バトルタワー等の接続の際に実行する期限更新用のプロフィール作成
 *
 * @param   sv				セーブデータへのポインタ
 * @param   dc_profile		プロフィール代入先
 */
//--------------------------------------------------------------
void EMAILSAVE_DCProfileCreate_Update(SAVEDATA *sv, Dpw_Common_Profile *dc_profile)
{
	EMAIL_SAVEDATA *emaildata = SaveData_Get(sv, GMDATA_ID_EMAIL);

	EMAILSAVE_DCProfileCreateCommon(sv, dc_profile);
	
	dc_profile->mailAddrAuthVerification = emaildata->auth_code_high;	//一応入れておく
	dc_profile->mailAddrAuthPass = emaildata->auth_code_low;
	OS_TPrintf("mailAddrAuthVerification = %d\n", dc_profile->mailAddrAuthVerification);
	OS_TPrintf("mailAddrAuthPass = %d\n", dc_profile->mailAddrAuthPass);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
}

