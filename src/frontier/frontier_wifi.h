//============================================================================================
/**
 * @file	frontier_wifi.h
 * @bfief	フロンティアWiFi受付　呼び出し
 * @author	Satoshi Nohara
 * @date	07.07.27
 */
//============================================================================================
#ifndef	__FRONTIER_WIFI_H__
#define	__FRONTIER_WIFI_H__

#include "field/field.h"
#include "frontier/frontier_main.h"
#include "application/pokelist.h"
#include "application/p_status.h"

//============================================================================================
//
//	extern宣言
//
//============================================================================================
extern FRONTIER_EX_PARAM* EvCmdFrontierSystemCall( FIELDSYS_WORK* fsys, void* work );

#define FRWIFI_COMM_BUF_LEN		(40)
#define FR_WIFI_COUNTER_ENTRY_POKE_MAX		(4)


//============================================================================================
//
//	構造体
//
//============================================================================================
typedef struct{

	SAVEDATA* sv;
	void* p_work;

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//通信用：データバッファ
	u16	send_buf[FRWIFI_COMM_BUF_LEN];

	//通信用：基本情報
	u8	pair_sex;											//性別
	u8	pair_retire_flag;									//リタイヤを選んだか=1
	STRCODE pair_name[PERSON_NAME_SIZE + EOM_SIZE];			//

	//参加している手持ちポケモンの位置情報
	u8	mine_poke_pos[FR_WIFI_COUNTER_ENTRY_POKE_MAX];

	u8	pair_bf_no;
	u8	recieve_count;

	u8	dummy36;
	u8	pair_game_continue;									//パートナーが続けて遊ぶか

	u16 pair_rensyou;										//パートナーの連勝数チェック用
	u16	pair_stage_del;										//パートナーがステージ記録消してよいか

	u16 monsno[FR_WIFI_COUNTER_ENTRY_POKE_MAX];
	u16 itemno[FR_WIFI_COUNTER_ENTRY_POKE_MAX];
	u16 pair_monsno[FR_WIFI_COUNTER_ENTRY_POKE_MAX];
	u16 pair_itemno[FR_WIFI_COUNTER_ENTRY_POKE_MAX];

	//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
	//ポケモンリスト用
	FIELDSYS_WORK* fsys;									//ポケモンリスト呼び出しに必要
	u8	seq;
	u8  next_seq;
	u8	mode;												//ポケモンリストモード
	u8	pos;												//ポケモンリストの現在カーソル位置
	u8	bf_no;												//施設No
	u8	sel[FR_WIFI_COUNTER_ENTRY_POKE_MAX];
	PLIST_DATA* pld;
	PSTATUS_DATA* psd;
	u16* ret_wk;
}FRWIFI_SCRWORK;


#endif	//__FRONTIER_WIFI_H__


