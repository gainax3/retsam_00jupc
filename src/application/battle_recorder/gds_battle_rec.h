//==============================================================================
/**
 *
 * 戦闘録画
 *
 */
//==============================================================================
#ifndef __GT_GDS_BATTLE_REC_H__
#define __GT_GDS_BATTLE_REC_H__


#include "gds_define.h"
#include "gds_profile_local.h"
#include "gds_battle_rec_sub.h"


//----------------------------------------------------------
/**
 *	録画セーブデータ本体（7272byte）
 */
//----------------------------------------------------------
typedef struct {
	GT_REC_BATTLE_PARAM	bp;						//戦闘開始パラメータ	336byte
	GT_RECORD_PARAM		rp;						//録画バッファ			4096byte
	GT_REC_POKEPARTY	rec_party[GT_CLIENT_MAX];	//ポケモンパラメータ	628 * 4
	GT_MYSTATUS			my_status[GT_CLIENT_MAX];	//プレイヤー情報		32byte * 4
	GT_CONFIG			config;					//コンフィグ			2byte
	u16 magic_key;
	
	//u32					checksum;	//チェックサム	4byte
	GT_GDS_CRC crc;							///< CRC						4
}GT_BATTLE_REC_WORK;

//--------------------------------------------------------------
/**
 *	戦闘録画のヘッダ
 *		100byte
 */
//--------------------------------------------------------------
typedef struct {
	u16 monsno[GT_HEADER_MONSNO_MAX];	///<ポケモン番号(表示する必要がないのでタマゴの場合は0)	24
	u8 form_no[GT_HEADER_MONSNO_MAX];	///<ポケモンのフォルム番号								12

	u16 battle_counter;		///<連勝数												2
	u8 mode;				///<戦闘モード(ファクトリー50、ファクトリー100、通信対戦...)
	
	u8 secure;				///<TRUE:安全が保障されている。　FALSE：再生した事がない
	
	GT_REGULATION regulation;			///<レギュレーションデータ		32

	u16 magic_key;			///<マジックキー
	
	u8 work[14];			///< 予備										16
	
	GT_DATA_NUMBER data_number;		///<データナンバー(サーバー側でセットされる)	8
	GT_GDS_CRC crc;							///< CRC						4
}GT_BATTLE_REC_HEADER;

//--------------------------------------------------------------
/**
 *	送信データ：戦闘録画
 *	7500byte
 */
//--------------------------------------------------------------
typedef struct {
	GT_GDS_PROFILE profile;		///<GDSプロフィール		88
	GT_BATTLE_REC_HEADER head;		///<戦闘録画のヘッダ	100
	GT_BATTLE_REC_WORK rec;		///<録画データ本体			7272
}GT_BATTLE_REC_SEND;

//--------------------------------------------------------------
/**
 *	送信データ：検索リクエスト
 *	16byte
 */
//--------------------------------------------------------------
typedef struct{
	u16 monsno;					///< ポケモン番号
	u8 battle_mode;				///< 検索施設
	u8 country_code;			///< 住んでいる国コード
	u8 local_code;				///< 住んでいる地方コード
	
	u8 padding[3];				//パディング
	
	u32 server_version;			///< ROMのサーバーバージョン
	
	u8 work[4];					///<予備
}GT_BATTLE_REC_SEARCH_SEND;

//--------------------------------------------------------------
/**
 *	送信データ：ランキング取得
 */
//--------------------------------------------------------------
typedef struct{
	u32 server_version;			///< ROMのサーバーバージョン
}GT_BATTLE_REC_RANKING_SEARCH_SEND;


//--------------------------------------------------------------
/**
 *	受信データ：概要一覧取得時
 *	228byte
 */
//--------------------------------------------------------------
typedef struct {
	GT_GDS_PROFILE profile;		///<GDSプロフィール
	GT_BATTLE_REC_HEADER head;		///<戦闘録画のヘッダ
}GT_BATTLE_REC_OUTLINE_RECV;

//--------------------------------------------------------------
/**
 *	受信データ：一件の検索結果取得時
 *	7500byte
 */
//--------------------------------------------------------------
typedef struct{
	GT_GDS_PROFILE profile;		///<GDSプロフィール
	GT_BATTLE_REC_HEADER head;		///<戦闘録画のヘッダ
	GT_BATTLE_REC_WORK rec;		///<戦闘録画本体
}GT_BATTLE_REC_RECV;


#endif	//__GT_GDS_BATTLE_REC_H__

