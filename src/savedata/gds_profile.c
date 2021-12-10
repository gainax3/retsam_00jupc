//==============================================================================
/**
 * @file	gds_profile.c
 * @brief	GDSプロフィール(GDSのWifiデータに添付される)
 * @author	matsuda
 * @date	2007.07.23(月)
 */
//==============================================================================
#include "common.h"
#include "system/buflen.h"
#include "savedata/savedata.h"
#include "savedata/mystatus.h"
#include "savedata/wifihistory.h"
#include "system/pms_data.h"
#include "system/pm_str.h"
#include "poketool/monsno.h"
#include "gflib/strbuf_family.h"
#include "savedata/gds_profile.h"
#include "poketool/boxdata.h"
#include "savedata/system_data.h"
#include "savedata/misc.h"

#include "gds_profile_types.h"
#include "gflib/strcode.h"

#include "system/msgdata.h"
#include "system/msgdata_util.h"
#include "field/union_beacon_tool.h"

#include "msgdata/msg.naix"
#include "system/pms_data.h"
#include "system/pms_word.h"
#include "msgdata/msg_pmss_union.h"
#include "msgdata/msg_pmss_ready.h"
#include "msgdata/msg_pmss_won.h"
#include "msgdata/msg_pmss_lost.h"
#include "msgdata/msg_pms_word06.h"
#include "msgdata/msg_pms_word07.h"
#include "msgdata/msg_pms_word08.h"
#include "msgdata/msg_pms_word09.h"
#include "msgdata/msg_pms_word10.h"
#include "msgdata/msg_pms_word11.h"
#include "msgdata/msg_pms_word12.h"

#include "application/wifi_country.h"
#include "application/battle_recorder/gds_min_max.h"


//--------------------------------------------------------------
/**
 * @brief   GDSプロフィールワークをAllocする
 *
 * @param   heap_id		ヒープID
 *
 * @retval  GDSプロフィールワークへのポインタ
 */
//--------------------------------------------------------------
GDS_PROFILE_PTR GDS_Profile_AllocMemory(int heap_id)
{
	GDS_PROFILE_PTR gpp;
	
	gpp = sys_AllocMemory(heap_id, sizeof(GDS_PROFILE));
	MI_CpuClear8(gpp, sizeof(GDS_PROFILE));
	
	return gpp;
}

//--------------------------------------------------------------
/**
 * @brief   GDSプロフィールワークを解放
 *
 * @param   gpp		GDSプロフィールへのポインタ
 */
//--------------------------------------------------------------
void GDS_Profile_FreeMemory(GDS_PROFILE_PTR gpp)
{
	sys_FreeMemoryEz(gpp);
}

//--------------------------------------------------------------
/**
 * @brief   自分のGDSプロフィールデータを作成する
 *
 * @param   gpp			データ代入先
 * @param   sv			セーブデータへのポインタ
 * @param   monsno		GDSプロフィールに添付するポケモン番号
 *
 * @retval  
 */
//--------------------------------------------------------------
void GDS_Profile_MyDataSet(GDS_PROFILE_PTR gpp, SAVEDATA *sv)
{
	MYSTATUS *my = SaveData_GetMyStatus(sv);
	WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
	SYSTEMDATA *systemdata = SaveData_GetSystemData(sv);
	const MISC * misc = SaveData_GetMiscReadOnly(sv);
	int monsno, form_no, egg_flag;
	int i;
	OSOwnerInfo info;

	OS_GetOwnerInfo(&info);
	
	MISC_GetFavoriteMonsno(misc, &monsno, &form_no, &egg_flag);

	MI_CpuClear8(gpp, sizeof(GDS_PROFILE));

	PM_strcpy(gpp->name, MyStatus_GetMyName(my));
	
	gpp->player_id = MyStatus_GetID(my);
	gpp->player_sex = MyStatus_GetMySex(my);
	gpp->monsno = monsno;
	gpp->form_no = form_no;
	gpp->egg_flag = egg_flag;
	gpp->country_code = WIFIHISTORY_GetMyNation(wh);
	gpp->local_code = WIFIHISTORY_GetMyArea(wh);
	
	for(i = 0; i < EVENT_SELF_INTRO; i++){
		gpp->event_self_introduction[i] = EOM_;
	}
	MISC_GetGdsSelfIntroduction(misc, &gpp->self_introduction);

	gpp->birthday_month = info.birthday.month;
	gpp->trainer_view = UnionView_GetTrainerInfo(
		MyStatus_GetMySex(my), MyStatus_GetTrainerView(my), UNIONVIEW_ICONINDEX);
	gpp->version_code = PM_VERSION;
	gpp->language = PM_LANG;

	//CRC作成
	gpp->crc.crc16ccitt_hash = SaveData_CalcCRC(sv, gpp, sizeof(GDS_PROFILE) - GDS_CRC_SIZE);
}

//----------------------------------------------------------
/**
 * @brief	名前取得（STRBUFを生成）
 * @param	gpp		GDSプロフィールへのポインタ
 * @param	heapID	STRBUFを生成するヒープのID
 * @return	STRBUF	名前を格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
STRBUF * GDS_Profile_CreateNameString(const GDS_PROFILE_PTR gpp, int heapID)
{
	STRBUF * tmpBuf = STRBUF_Create( ( PERSON_NAME_SIZE * 2 ) + EOM_SIZE, heapID);
	STRBUF_SetStringCodeOrderLength(tmpBuf, gpp->name, ( PERSON_NAME_SIZE * 2 ) + EOM_SIZE);
	return tmpBuf;
}

u32 GDS_Profile_GetID(const GDS_PROFILE_PTR gpp)
{
	return gpp->player_id;
}

u32 GDS_Profile_GetSex(const GDS_PROFILE_PTR gpp)
{
	if(gpp->player_sex != PM_MALE && gpp->player_sex != PM_FEMALE){
		return PM_MALE;	//エラー処理
	}
	return gpp->player_sex;
}

int GDS_Profile_GetMonsNo(const GDS_PROFILE_PTR gpp)
{
	if(gpp->monsno >= MONSNO_MAX){
		return 0;	//エラー処理
	}
	return gpp->monsno;
}

int GDS_Profile_GetFormNo(const GDS_PROFILE_PTR gpp)
{
	if(gpp->monsno >= MONSNO_MAX){
		return 0;
	}
	return PokeFuseiFormNoCheck(gpp->monsno, gpp->form_no);
}

int GDS_Profile_GetEggFlag(const GDS_PROFILE_PTR gpp)
{
	if(gpp->egg_flag > 1){
		return 1;	//エラー処理
	}
	return gpp->egg_flag;
}

//--------------------------------------------------------------
/**
 * @brief   国コードを取得
 *
 * @param   gpp		
 *
 * @retval  国コード(msg_wifi_place_msg_world.h)
 */
//--------------------------------------------------------------
int GDS_Profile_GetNation(const GDS_PROFILE_PTR gpp)
{
	if(gpp->country_code >= WIFI_COUNTRY_MAX){
		return 0;
	}
	return gpp->country_code;
}

//--------------------------------------------------------------
/**
 * @brief   地域コードを取得
 *
 * @param   gpp		
 *
 * @retval  地域コード(msg_wifi_place_msg_ARG.h, JPN, BRA等)
 */
//--------------------------------------------------------------
int GDS_Profile_GetArea(const GDS_PROFILE_PTR gpp)
{
	if(gpp->country_code >= WIFI_COUNTRY_MAX){
		return 0;
	}
	if(WIFI_COUNTRY_CountryCodeToPlaceIndexMax(gpp->country_code) < gpp->local_code){
		return 0;
	}
	return gpp->local_code;
}

//--------------------------------------------------------------
/**
 * @brief   自己紹介メッセージ取得
 *
 * @param   gpp				
 * @param   pms_dest		簡易会話代入先
 * @param   heap_id			ヒープID(イベント用フリーワード時、中でSTRBUFを生成)
 *
 * @retval  NULLの場合はpms_destに簡易会話文が入っている
 * 			非NULLの場合はイベント用フリーワードメッセージ。
 *			内部でSTRBUFを作成し返している
 */
//--------------------------------------------------------------
STRBUF * GDS_Profile_GetSelfIntroduction(const GDS_PROFILE_PTR gpp, PMS_DATA *pms_dest, int heap_id)
{
	int error_flg = 0;
	
	if(gpp->message_flag == MESSAGE_FLAG_NORMAL){
		*pms_dest = gpp->self_introduction;
		if(pms_dest->sentence_type >= PMS_TYPE_MAX){
			error_flg++;
		}
		else if(pms_dest->sentence_id > pmss_union_20){
			error_flg++;
		}
		else{
			u32 file_id, word_id;
			if((pms_dest->word[0] != PMS_WORD_NULL 
					&& GetWordSorceID(pms_dest->word[0], &file_id, &word_id) == FALSE)
					|| (pms_dest->word[1] != PMS_WORD_NULL 
					&& GetWordSorceID(pms_dest->word[1], &file_id, &word_id) == FALSE)){
				error_flg++;
			}
		}
		
		if(error_flg > 0){
			PMSDAT_Init( pms_dest, PMS_TYPE_UNION );
			pms_dest->sentence_id = pmss_union_01;
			pms_dest->word[0] 
				= PMSW_GetWordNumberByGmmID( NARC_msg_pms_word08_dat, pms_word08_100 );
			pms_dest->word[1] = PMS_WORD_NULL;
		}
		return NULL;
	}
	else{
		//※check 最大チェック入れる
		STRBUF * tmpBuf = STRBUF_Create(EVENT_SELF_INTRO, heap_id);
		STRBUF_SetStringCodeOrderLength(tmpBuf, gpp->event_self_introduction, EVENT_SELF_INTRO);
		return tmpBuf;
	}
}

int GDS_Profile_GetMonthBirthday(const GDS_PROFILE_PTR gpp)
{
	if(gpp->birthday_month >= 1 && gpp->birthday_month <= 12){
		return gpp->birthday_month;
	}
	return 1;
}

int GDS_Profile_GetTrainerView(const GDS_PROFILE_PTR gpp)
{
	if(gpp->trainer_view > GT_RANKING_PROFILE_TRAINER_VIEW_MAX){	//ランキングのと一緒なので
		return 0;
	}
	return gpp->trainer_view;
}

int GDS_Profile_GetRomCode(const GDS_PROFILE_PTR gpp)
{
	switch(gpp->version_code){
	case VERSION_SAPPHIRE:
	case VERSION_RUBY:
	case VERSION_EMERALD:
	case VERSION_RED:
	case VERSION_GREEN:
	case VERSION_GOLD:
	case VERSION_SILVER:
	case VERSION_DIAMOND:
	case VERSION_PEARL:
	case VERSION_PLATINUM:
	case VERSION_COLOSSEUM:
		return gpp->version_code;
	}
	return PM_VERSION;
}

int GDS_Profile_GetLanguage(const GDS_PROFILE_PTR gpp)
{
	switch(gpp->language){
	case LANG_JAPAN:
	case LANG_ENGLISH:
	case LANG_FRANCE:
	case LANG_ITALY:
	case LANG_GERMANY:
	case LANG_SPAIN:
	case LANG_KOREA:
		return gpp->language;
	}
	return PM_LANG;
}

