//============================================================================================
/**
 * @file	tower_scr_common.h
 * @bfief	バトルタワースクリプト共通ヘッダー
 * @author	Satoshi Nohara
 * @date	07.05.28
 */
//============================================================================================
#ifndef	__TOWER_SCR_COMMON_H__
#define	__TOWER_SCR_COMMON_H__

#include "battle/battle_common.h"
#include "battle/b_tower_data.h"


//============================================================================================
//
//	構造体宣言
//
//============================================================================================
///バトルタワー　スクリプトワーク
typedef struct _BTOWER_SCRWORK{
	int	magicNo;	///<マジックナンバー
	int	heapID;	///<ヒープ
	u32	play_rnd_seed;	///<プレイ用ランダムシード
	u8	now_round;	///<現在何人目？
	u8	now_win;	///<現在何人に勝った？
	u8	member_num;	///<メンバー数
	u8	play_mode;	///<プレイデータ

	u8	clear_f:1;	///<クリアフラグ
	u8	leader_f:2;	///<タイクーン2
	u8	retire_f:1;	///<リタイアフラグ
	u8	prize_f:1;	///<ご褒美フラグ
	u8	partner:3;	///<パートナーNo

	u8	my_sex;			///<自分の性別
	u8	pare_sex;		///<自分とパートナーの性別
	u16	pare_stage;		///<通信マルチパートナーの周回数
	u16	pare_poke[2];	///<通信マルチパートナーが持つポケモンNo
	
	u16	renshou;	///<現在の連勝数
	u16	stage;		///<現在の周回数
	u32	win_cnt;	///<現在までの総勝利数

	u16	rec_turn;	///<かかったターン数
	u16	rec_damage;	///<食らったダメージ
	u16	rec_down;	///<ポケモンのダウン数

	u8	member[BTOWER_STOCK_MEMBER_MAX];	///<選んだポケモンの手持ちNo
	u16	mem_poke[BTOWER_STOCK_MEMBER_MAX];	///<選んだポケモンのmonsno
	u16	mem_item[BTOWER_STOCK_MEMBER_MAX];	///<選んだポケモンのアイテムno
	u16	trainer[BTOWER_STOCK_TRAINER_MAX];	///<対戦トレーナーNo

	//現在のロケーションを記憶しておく
	LOCATION_WORK	now_location;

	BTLTOWER_PLAYWORK*	playSave;	///<セーブデータへのポインタ
	BTLTOWER_SCOREWORK*	scoreSave;	///<セーブデータへのポインタ

	//対戦相手データ格納場所
	B_TOWER_PARTNER_DATA	tr_data[2];
	
	//5人衆抽選データ格納場所
	B_TOWER_PARTNER_DATA	five_data[BTWR_FIVE_NUM];

	//5人衆抽選ポケモンパラメータ格納場所
	B_TOWER_PAREPOKE_PARAM	five_poke[BTWR_FIVE_NUM];

	//5人衆抽選ポケモンアイテムfixフラグ
	u8						five_item[BTWR_FIVE_NUM];

	//通信用データバッファ
	u16	send_buf[35];
	u16	recv_buf[35];

	/***********************************************************************************/
	/***********************************************************************************/
	/***********************************************************************************/
	//プラチナで追加
	int winlose_flag;		//(LOCATION_WORKを消して、そこに入れることも可能？)
	void* p_work;			//(LOCATION_WORKを消して、そこに入れることも可能？)
	u8	recieve_count;
	u8	mode;
	u8	first_btl_flag;		//初戦をおこなったかフラグ
	u8	dummy;
	u16	check_work;
	u16	ret_wkno;
	/***********************************************************************************/
	/***********************************************************************************/
	/***********************************************************************************/

}BTOWER_SCRWORK;


#endif	//__TOWER_SCR_COMMON_H__


