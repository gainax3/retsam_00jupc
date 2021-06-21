//==============================================================================
/**
 *
 * GDSドレスアップ
 *
 */
//==============================================================================
#ifndef __GT_GDS_DRESSUP_H__
#define __GT_GDS_DRESSUP_H__


#include "gds_define.h"
#include "gds_profile_local.h"
#include "gds_dressup_sub.h"


//--------------------------------------------------------------
/**
 *	ドレスアップ		92byte
 */
//--------------------------------------------------------------
typedef struct {
	u32 init_flag;					///<初期化データかのフラグ
	GT_IMC_SAVEDATA_POKEMON	pokemon;///<ポケモンデータ
	u32 accessory_set_msk;			///<格納アクセサリーマスク
	GT_PMS_DATA title;					///<クリップタイトル
	GT_IMC_SAVEDATA_ACCESSORIE acce[ GT_IMC_SAVEDATA_TELEVISION_ACCE_NUM ];	///<アクセサリ
	u8 bg_id;								///<bgナンバー	
	u8 country;	// 国コード
	
	u8 padding[2];					// パディング
	
	GT_GDS_CRC crc;							///< CRC						4
}GT_GDS_DRESS;

//--------------------------------------------------------------
/**
 *	ドレスアップ送信データ：128 + 92 = 220byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;
	GT_GDS_DRESS dressup;
}GT_GDS_DRESS_SEND;

//--------------------------------------------------------------
/**
 *	ドレスアップ受信データ：128 + 92 = 220byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;
	GT_GDS_DRESS dressup;
}GT_GDS_DRESS_RECV;


#endif	//__GT_GDS_DRESSUP_H__

