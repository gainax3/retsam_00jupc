//============================================================================================
/**
 * @file	misc.c
 * @brief	未分類セーブデータのアクセス
 * @author	tamada	GAME FREAK Inc.
 * @date	2006.01.26
 *
 * 分類されていないセーブデータはとりあえずここに追加される。
 * アプリケーションレベル、セーブデータアクセス関数レベルからは
 * 直接MISC構造体を取り出すことはできない。
 * MISC構造体はメンバに持つ構造体へのアクセスを提供するだけの
 * インターフェイスとして機能している。
 *
 * セーブデータとしての性質がきちんと定義できるようになった時点で、
 * それらのセーブ構造体はきちんと別ソースに切り分けられることになる。
 *
 * 2006.06.02
 * 時間切れで分類がわからんものはここに追加することにしました。
 * 無念。
 */
//============================================================================================

#include "common.h"
#include "gflib/system.h"

#include "system/gamedata.h"

#include "savedata/savedata.h"

#include "savedata/misc.h"
#include "misc_local.h"

#include "seedbed_local.h"
#include "savedata/seedbed.h"
#include "gimmickwork_local.h"
#include "savedata/gimmickwork.h"


#include "gflib/strbuf_family.h"
#include "system/pm_str.h"

#include "msgdata/msg.naix"
#include "system/pms_data.h"
#include "system/pms_word.h"
#include "msgdata\msg_pmss_union.h"
#include "msgdata\msg_pmss_ready.h"
#include "msgdata\msg_pmss_won.h"
#include "msgdata\msg_pmss_lost.h"
#include "msgdata\msg_pms_word06.h"
#include "msgdata\msg_pms_word07.h"
#include "msgdata\msg_pms_word08.h"
#include "msgdata\msg_pms_word09.h"
#include "msgdata\msg_pms_word10.h"
#include "msgdata\msg_pms_word11.h"
#include "msgdata\msg_pms_word12.h"

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   外部セーブの参照先とランダムキーの制御構造体定義
 * 			殿堂入りデータのみ、DPからあるので除く
 */
//--------------------------------------------------------------
typedef struct{
	EX_CERTIFY_SAVE_KEY key[EXDATA_ID_MAX - EXDATA_ID_FRONTIER];		//認証キー
	EX_CERTIFY_SAVE_KEY old_key[EXDATA_ID_MAX - EXDATA_ID_FRONTIER];	//1つ前の認証キー
	u8 flag[EXDATA_ID_MAX - EXDATA_ID_FRONTIER];						//参照先フラグ
	u8 padding[3];
}EX_SAVE_KEY;

//---------------------------------------------------------------------------
/**
 * @brief	MISC構造体の定義
 *
 * 下記のようなセーブデータブロックは、MISCのメンバーとして確保する。
 * -どこに分類するべきか、判断に迷うもの。今は分類を保留しておきたいもの
 * -規模が小さく、そのためにセーブブロックを確保するのがもったいないもの
 */
//---------------------------------------------------------------------------
struct _MISC {
	SEEDBED seedbed[SEEDBED_MAX];
	GIMMICKWORK gimmick;
	STRCODE rivalname[PERSON_NAME_SIZE + EOM_SIZE];
	STRCODE monument_name[MONUMENT_NAME_SIZE + EOM_SIZE];
	
	//プラチナから追加
	u16 favorite_monsno;		//お気に入りポケモン
	u8  favorite_form_no:7;		//お気に入りポケモンのフォルム番号
	u8  favorite_egg_flag:1;	//お気に入りポケモンのタマゴフラグ

	u8 extra_init_flag:1;		// 外部セーブ初期化済みかどうかのフラグ(TRUE初期化済み)
	u8 battle_recoder_color:4;	// バトルレコーダーの色(5色)
	u8 				:3;			//余り
	
	//未使用になった 2008.06.21(土) matsuda(元々GDS系の送信済みフラグだったもの)
	u32 dummy;
	
	PMS_DATA gds_self_introduction;		// GDSプロフィールの自己紹介メッセージ
	
	EX_SAVE_KEY ex_save_key;	///<外部セーブの認証キー
};

#ifdef	PM_DEBUG
static const STRCODE RivalName[] = {PA_, bou_,RU_, EOM_ };
#endif
//============================================================================================
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	MISC構造体のサイズ取得
 * @return	int		MISC構造体のサイズ
 */
//---------------------------------------------------------------------------
int MISC_GetWorkSize(void)
{
	return sizeof(MISC);
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_Copy(const MISC * from, MISC * to)
{
	MI_CpuCopy8(from ,to, sizeof(MISC));
}

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	色々セーブデータ保持ワークの初期化
 * @param	misc	色々セーブデータ保持ワークへのポインタ
 */
//---------------------------------------------------------------------------
void MISC_Init(MISC * misc)
{
	MI_CpuClearFast(misc, sizeof(MISC));
	/* 以下に個別部分の初期化処理をおく */
	SEEDBED_Init(misc->seedbed);
	GIMMICKWORK_Init(&misc->gimmick);
	MI_CpuFill16(misc->rivalname, EOM_, PERSON_NAME_SIZE + EOM_SIZE);
	MI_CpuFill16(misc->monument_name, EOM_, MONUMENT_NAME_SIZE + EOM_SIZE);
#ifdef	PM_DEBUG
	PM_strcpy(misc->rivalname, RivalName);
#endif

	PMSDAT_Init( &misc->gds_self_introduction, PMS_TYPE_UNION );
	misc->gds_self_introduction.sentence_id = pmss_union_01;
	misc->gds_self_introduction.word[0] 
		= PMSW_GetWordNumberByGmmID( NARC_msg_pms_word08_dat, pms_word08_100 );
	misc->gds_self_introduction.word[1] = PMS_WORD_NULL;

	{
		int i;
		for(i = 0; i < EXDATA_ID_MAX - EXDATA_ID_FRONTIER; i++){
			misc->ex_save_key.key[i] = EX_CERTIFY_SAVE_KEY_NO_DATA;
			misc->ex_save_key.old_key[i] = EX_CERTIFY_SAVE_KEY_NO_DATA;
		}
	}
	
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}

//============================================================================================
//
//	セーブデータ取得のための関数
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	MISCセーブデータの取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	MISC構造体へのポインタ
 */
//---------------------------------------------------------------------------
MISC * SaveData_GetMisc(SAVEDATA * sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_CheckCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
	return SaveData_Get(sv, GMDATA_ID_MISC);
}

//---------------------------------------------------------------------------
/**
 * @brief	MISCセーブデータの取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	MISC構造体へのポインタ
 */
//---------------------------------------------------------------------------
const MISC * SaveData_GetMiscReadOnly(const SAVEDATA * sv)
{
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_CheckCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
	return SaveData_GetReadOnlyData(sv, GMDATA_ID_MISC);
}

//---------------------------------------------------------------------------
/**
 * @brief	きのみ状態データへのポインタ取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	CONFIG		設定情報保持ワークへのポインタ
 */
//---------------------------------------------------------------------------
SEEDBED * SaveData_GetSeedBed(SAVEDATA * sv)
{
	MISC * misc;
	misc = SaveData_Get(sv, GMDATA_ID_MISC);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_CheckCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
	return misc->seedbed;
}

//---------------------------------------------------------------------------
/**
 * @brief	マップ固有の仕掛けワークへのポインタ取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	GIMMICKWORK	仕掛け用ワークへのポインタ
 */
//---------------------------------------------------------------------------
GIMMICKWORK * SaveData_GetGimmickWork(SAVEDATA * sv)
{
	MISC * misc;
	misc = SaveData_Get(sv, GMDATA_ID_MISC);
	return &misc->gimmick;
}

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
const STRCODE * MISC_GetRivalName(const MISC * misc)
{
	return misc->rivalname;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_SetRivalName(MISC * misc, STRBUF * str)
{
	STRBUF_GetStringCode( str, misc->rivalname, PERSON_NAME_SIZE + EOM_SIZE );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
const STRCODE * MISC_GetMonumentName(const MISC * misc)
{
	return misc->monument_name;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void MISC_SetMonumentName(MISC * misc, STRBUF * str)
{
	STRBUF_GetStringCode( str, misc->monument_name, MONUMENT_NAME_SIZE + EOM_SIZE );
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}


//==============================================================================
//==============================================================================
//----------------------------------------------------------
/**
 * @brief	外部セーブデータの初期化済みフラグのセット
 * @param	my		自分状態保持ワークへのポインタ
 */
//----------------------------------------------------------
void MISC_SetExtraInitFlag(MISC * misc)
{
	misc->extra_init_flag = 1;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief	外部セーブデータの初期化済みフラグ取得
 * @param	misc		自分状態保持ワークへのポインタ
 * @retval	1:初期化済み、0:初期化されていない
 */
//----------------------------------------------------------
u32 MISC_GetExtraInitFlag(const MISC * misc)
{
	return misc->extra_init_flag;
}

#ifdef PM_DEBUG //==============
//--------------------
//	外部セーブデータの初期化済みフラグをリセットする
//		※デバッグ用
//--------------------
void MISC_ClearExtraInitFlag(MISC * misc)
{
	misc->extra_init_flag = 0;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}
#endif	//PM_DEBUG	===============

//==============================================================================
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   お気に入りポケモンのセット
 *
 * @param   misc		
 * @param   monsno		ポケモン番号
 * @param   form_no		フォルム番号
 * @param   egg_flag	タマゴフラグ
 */
//--------------------------------------------------------------
void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag)
{
	misc->favorite_monsno = monsno;
	misc->favorite_form_no = form_no;
	misc->favorite_egg_flag = egg_flag;
	
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_MISC)
	SVLD_SetCrc(GMDATA_ID_MISC);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------
/**
 * @brief   お気に入りポケモン取得
 *
 * @param   misc		
 * @param   monsno		ポケモン番号(お気に入りを設定していない場合は0)
 * @param   form_no		フォルム番号
 * @param   egg_flag	タマゴフラグ
 */
//--------------------------------------------------------------
void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag)
{
	*monsno = misc->favorite_monsno;
	*form_no = misc->favorite_form_no;
	*egg_flag = misc->favorite_egg_flag;
}

//--------------------------------------------------------------
/**
 * @brief   GDS自己紹介メッセージを取得
 *
 * @param   misc		
 * @param   pms			代入先
 */
//--------------------------------------------------------------
void MISC_GetGdsSelfIntroduction(const MISC *misc, PMS_DATA *pms)
{
	*pms = misc->gds_self_introduction;
}

//--------------------------------------------------------------
/**
 * @brief   GDS自己紹介メッセージをセットする
 *
 * @param   misc		
 * @param   pms			セットするメッセージ
 */
//--------------------------------------------------------------
void MISC_SetGdsSelfIntroduction(MISC *misc, const PMS_DATA *pms)
{
	misc->gds_self_introduction = *pms;
}

//--------------------------------------------------------------
/**
 * @brief	バトルレコーダーの色取得
 *
 * @param	misc	
 * @param	color	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void MISC_GetBattleRecoderColor( const MISC * misc, u8* color )
{
	*color = misc->battle_recoder_color;
}


//--------------------------------------------------------------
/**
 * @brief	バトルレコーダーの色設定
 *
 * @param	misc	
 * @param	color	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void MISC_SetBattleRecoderColor( MISC * misc, u8 color )
{
	misc->battle_recoder_color = color;
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータから外部データの認証キー、フラグを取得する
 *
 * @param   sv				セーブデータへのポインタ
 * @param   exdata_id		外部データ番号(EXDATA_ID_???)
 * @param   key				認証キー代入先
 * @param   old_key			1つ前の認証キー代入先
 * @param   flag			フラグ代入先
 */
//--------------------------------------------------------------
void MISC_ExtraSaveKeyGet(const MISC *misc, EXDATA_ID exdata_id, EX_CERTIFY_SAVE_KEY *key, EX_CERTIFY_SAVE_KEY *old_key, u8 *flag)
{
	*key = misc->ex_save_key.key[exdata_id - EXDATA_ID_FRONTIER];
	*old_key = misc->ex_save_key.old_key[exdata_id - EXDATA_ID_FRONTIER];
	*flag = misc->ex_save_key.flag[exdata_id - EXDATA_ID_FRONTIER];
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータから外部データの認証キー、フラグをセットする
 *
 * @param   misc		
 * @param   exdata_id	
 * @param   key			認証キー
 * @param   key			1つ前の認証キー
 * @param   flag		フラグ
 * @param   flag		1つ前のフラグ
 */
//--------------------------------------------------------------
void MISC_ExtraSaveKeySet(MISC *misc, EXDATA_ID exdata_id, EX_CERTIFY_SAVE_KEY key, EX_CERTIFY_SAVE_KEY old_key, u8 flag)
{
	misc->ex_save_key.key[exdata_id - EXDATA_ID_FRONTIER] = key;
	misc->ex_save_key.old_key[exdata_id - EXDATA_ID_FRONTIER] = old_key;
	misc->ex_save_key.flag[exdata_id - EXDATA_ID_FRONTIER] = flag;
}
