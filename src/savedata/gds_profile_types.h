//==============================================================================
/**
 * @file	gds_profile_types.h
 * @brief	GDSプロフィールのローカルヘッダ
 * @author	matsuda
 * @date	2007.08.22(水)
 */
//==============================================================================
#ifndef __GDS_PROFILE_TYPES_H__
#define __GDS_PROFILE_TYPES_H__

#include "gds_local_common.h"
#include "system/pms_data.h"

//==============================================================================
//	定数定義
//==============================================================================
///名前のサイズ(EOM込み)
#define PLW_TRAINER_NAME_SIZE	(7+1)	//PERSON_NAME_SIZE + EOM_SIZE

///イベント録画の自己紹介メッセージ文字数
#define EVENT_SELF_INTRO			(40)

///自己紹介メッセージの表示形式
enum{
	MESSAGE_FLAG_NORMAL,		///<簡易会話
	MESSAGE_FLAG_EVENT,			///<フリーワード
};


//==============================================================================
//	構造体定義
//==============================================================================
//--------------------------------------------------------------
/**
 *	GDSプロフィール構造体
 *		88byte
 */
//--------------------------------------------------------------
typedef struct _GDS_PROFILE{
	u16 name[PLW_TRAINER_NAME_SIZE];		///< 名前(EOM込み)		16
	u32 player_id;							///< プレイヤーID		4
	u8 player_sex;							///< プレイヤー性別
	u8 birthday_month;						///< プレイヤーの誕生月
	u8 trainer_view;						///< ユニオンルーム内での見た目

	u8 country_code;						///< 住んでいる国コード
	u8 local_code;							///< 住んでいる地方コード

	u8 version_code;						///< バージョンコード
	u8 language;							///< 言語コード
	
	u8 egg_flag:1;							///< 1=タマゴ
	u8 form_no:7;							///< ポケモンフォルム番号
	u16 monsno;								///< ポケモン番号		2
	
	u8 message_flag;						///< 簡易会話、フリーワードどちらを使用するか
	u8 padding;
	//PMS_DATA self_introduction;			///< 自己紹介(簡易会話)	8
	union{
		PMS_DATA self_introduction;			///< 自己紹介(簡易会話)	8
		u16 event_self_introduction[EVENT_SELF_INTRO];	///< 自己紹介(イベント用フリーワード(EOM込み))
	};
	
	u8 work[12];							///< 予備				14
	
	//CRC(必ず最後尾にしておくこと)
	GDS_CRC				crc;
}GDS_PROFILE;


#endif	//__GDS_PROFILE_TYPES_H__
