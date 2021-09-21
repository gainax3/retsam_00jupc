//============================================================================================
/**
 * @file	castle_def.h
 * @bfief	「バトルキャッスル」定義
 * @author	Satoshi Nohara
 * @date	07.07.04
 */
//============================================================================================
#ifndef	__CASTLE_DEF_H__
#define	__CASTLE_DEF_H__


//============================================================================================
//
//	定義(enum禁止)
//
//============================================================================================
#define CASTLE_LAP_ENEMY_MAX	(7)							//1週毎の敵トレーナーの最大数
#define CASTLE_LAP_MAX			(8)							///キャッスルの最大周回数
#define CASTLE_LAP_MULTI_ENEMY_MAX	(CASTLE_LAP_ENEMY_MAX*2)//マルチ1週毎の敵トレーナーの最大数

#define CASTLE_ENTRY_POKE_MAX	(3)							//プレイヤー1人の最大参加ポケモン数

//タイプ
#define CASTLE_TYPE_SINGLE		(0)
#define CASTLE_TYPE_DOUBLE		(1)
#define CASTLE_TYPE_MULTI		(2)
#define CASTLE_TYPE_WIFI_MULTI	(3)
#define CASTLE_TYPE_MAX			(4)							//最大数

//ポケモン数
#define CASTLE_MINE_POKE_MAX	(4)							//味方ポケモンデータの最大数
#define CASTLE_ENEMY_POKE_MAX	(4)							//敵ポケモンデータの最大数
#define CASTLE_TOTAL_POKE_MAX	(CASTLE_MINE_POKE_MAX+CASTLE_ENEMY_POKE_MAX)	//被りチェックに使用

//戻り値に必要なワークの数
#define	CASTLE_PARTY_POKE_MAX	(6)							//POKEPARTYのポケモン数(6が最大)
#define CASTLE_RET_WORK_MAX		(CASTLE_PARTY_POKE_MAX)		//ret_work[]
#define CASTLE_SEL_WORK_MAX		(CASTLE_PARTY_POKE_MAX)		//sel_poke_no[]

//戻り値
#define CASTLE_RET_CANCEL		(0xff)						//何もしないでキャンセルした

//使用CP
#define CP_USE_NUKEMITI			(50)						//抜け道

//桁数
#define CASTLE_KETA_CP			(4)
#define CASTLE_KETA_RANK		(1)
#define CASTLE_KETA_STATUS		(3)
#define CASTLE_KETA_LV			(3)
#define CASTLE_KETA_HP			(3)

//CASTLE_SCRWORKの取得コード(castle.sで使用)
//#define FC_ID_SET_MODE					(0)
//#define FC_ID_SET_LEVEL					(1)
#define FC_ID_SET_TYPE					(2)
#define FC_ID_GET_RET_WORK				(3)
#define FC_ID_GET_RENSYOU				(4)
#define FC_ID_INC_RENSYOU				(5)
//#define FC_ID_INC_TRADE_COUNT			(6)
#define FC_ID_SYSTEM_RESET				(7)
//#define FC_ID_CLEAR_PLAY_DATA			(8)
#define FC_ID_IS_SAVE_DATA_ENABLE		(9)
#define FC_ID_SAVE_REST_PLAY_DATA		(10)
//#define FC_ID_PUSH_NOW_LOCATION			(11)
//#define FC_ID_POP_NOW_LOCATION			(12)
//#define FC_ID_SET_NG					(13)
#define FC_ID_INC_ROUND					(14)
#define FC_ID_GET_MONSNO				(15)
#define FC_ID_GET_WAZANO				(16)
#define FC_ID_GET_TYPE					(17)
#define FC_ID_SET_TEMP_ITEM				(18)
#define FC_ID_GET_LAP					(19)
#define FC_ID_GET_TR_OBJ_CODE			(20)
#define FC_ID_SET_LOSE					(21)
#define FC_ID_SET_CLEAR					(22)
#define FC_ID_GET_ROUND					(23)
#define FC_ID_GET_RETIRE_FLAG			(24)
#define FC_ID_GET_HINSI_FLAG			(25)
#define FC_ID_GET_TRADE_YESNO_FLAG		(26)
#define FC_ID_BTL_WIN_CP_POINT			(27)
#define FC_ID_BTL_WIN_POKE_DATA			(28)
#define FC_ID_COMM_COMMAND_INITIALIZE	(29)
#define FC_ID_GET_PARENT_CHECK_FLAG		(30)
#define FC_ID_CLEAR_PARENT_CHECK_FLAG	(31)
#define FC_ID_SET_SEL_TYPE				(32)
#define FC_ID_NUKEMITI_CP_SUB			(33)
#define FC_ID_GET_PAIR_DECIDE_CHECK		(34)
#define FC_ID_CHECK_COMM_TYPE			(35)
#define FC_ID_LAP_NUM_WRITE				(36)
#define	FC_ID_SET_B_TOWER_PARTNER_DATA	(37)
#define	FC_ID_GET_BP_POINT				(38)
#define	FC_ID_GET_GUIDE_FLAG			(39)
#define FC_ID_CHECK_BRAIN				(40)
#define FC_ID_ENEMY_POKE_CREATE			(41)
#define FC_ID_ENEMY_POKE_CREATE_2		(42)
#define FC_ID_BRAIN_APPEAR_MSG_CHK		(43)
#define FC_ID_WIFI_MULTI_CP_TEMP		(44)

//d32r0501.evで使用
#define CASTLE_TOOL_CHK_ENTRY_POKE_NUM	(0)
#define CASTLE_TOOL_GET_CLEAR_FLAG		(1)
#define CASTLE_TOOL_GET_CLEAR_MONSNO	(2)
#define CASTLE_TOOL_SET_NEW_CHALLENGE	(3)
#define CASTLE_TOOL_SELECT_POKE			(4)

//通信バッファのサイズ(send_buf,recv_buf)
#define CASTLE_COMM_BUF_LEN			(40)				//34*2=68
#define CASTLE_HUGE_BUF_LEN			(512)//(1024)

//通信タイプ
//fssc_castle_sub.c
#define CASTLE_COMM_BASIC			(0)
#define CASTLE_COMM_TR				(1)
#define CASTLE_COMM_SEL				(2)
#define CASTLE_COMM_ENEMY			(3)
#define CASTLE_COMM_RETIRE			(4)
#define CASTLE_COMM_TRADE_YESNO		(5)
#define CASTLE_COMM_TEMOTI			(6)
//手持ち通信
#define CASTLE_COMM_MINE_PAIR			(10)
#define CASTLE_COMM_MINE_UP_TYPE		(11)
#define CASTLE_COMM_MINE_CSR_POS		(12)
#define CASTLE_COMM_MINE_MODORU			(13)
//敵トレーナー通信
#define CASTLE_COMM_ENEMY_PAIR			(20)
#define CASTLE_COMM_ENEMY_UP_TYPE		(21)
#define CASTLE_COMM_ENEMY_CSR_POS		(22)
#define CASTLE_COMM_ENEMY_MODORU		(23)
//ランク通信
#define CASTLE_COMM_RANK_PAIR			(30)
#define CASTLE_COMM_RANK_UP_TYPE		(31)
#define CASTLE_COMM_RANK_CSR_POS		(32)
#define CASTLE_COMM_RANK_MODORU			(33)

//通信時の1人のポケモン数
#define	CASTLE_COMM_POKE_NUM		(2)

//通信時の2人のポケモン数
#define	CASTLE_COMM_POKE_TOTAL_NUM	(2*2)

//総数(マルチなら敵の数が4など、1人あたりは2になる)
#define CASTLE_FLAG_SOLO			(0)					//2
#define CASTLE_FLAG_TOTAL			(1)					//2+2=4

//ランクの種類(かいふく、レンタル、じょうほう)
#define CASTLE_RANK_TYPE_KAIHUKU	(0)
#define CASTLE_RANK_TYPE_RENTAL		(1)
#define CASTLE_RANK_TYPE_INFO		(2)
#define CASTLE_RANK_TYPE_MAX		(3)

//通信人数
#define CASTLE_COMM_PLAYER_NUM		(2)

//親の決定
#define CASTLE_SCR_MENU_NONE		(0)
#define CASTLE_SCR_MENU_TEMOTI		(1)
#define CASTLE_SCR_MENU_TRAINER		(2)
//#define CASTLE_SCR_MENU_RANK		(3)
#define CASTLE_SCR_MENU_TAISEN		(4)
#define CASTLE_SCR_MENU_NUKEMITI	(5)
#define CASTLE_SCR_MENU_MAX			(6)

//「戻る」表示ウェイト
//#define CASTLE_MODORU_WAIT			(15)
#define CASTLE_MODORU_WAIT			(30)

#define CASTLE_NAME_CP_START_X		(16)		//左上の名前＋ＣＰウィンドウのＣＰ開始Ｘ

//レコードセットに渡す前に制限かけないとダメ！
//仕様書の「セーブデータについて」の欄に最大数が書いてある
#define CASTLE_RENSYOU_MAX			(9999)				//連勝数の最大数
#define CASTLE_CP_MAX				(9999)				//持っているCPの最大数
#define CASTLE_USE_CP_MAX			(9999)				//使用したCPの最大数


#endif	//__CASTLE_DEF_H__


