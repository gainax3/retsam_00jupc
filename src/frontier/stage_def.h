//============================================================================================
/**
 * @file	stage_def.h
 * @bfief	「バトルステージ」定義
 * @author	Satoshi Nohara
 * @date	07.03.29
 */
//============================================================================================
#ifndef	__STAGE_DEF_H__
#define	__STAGE_DEF_H__


//============================================================================================
//
//	定義(enum禁止)
//
//============================================================================================
#define STAGE_LAP_ENEMY_MAX			(10)						//1週毎の敵トレーナーの最大数
//#define STAGE_LAP_MAX				(10)						///ステージの最大周回数
#define STAGE_LAP_MAX				(18)						///ステージの最大周回数
#define STAGE_LAP_MULTI_ENEMY_MAX	(STAGE_LAP_ENEMY_MAX*2)		//1週毎の2人の敵トレーナーの最大数
#define STAGE_TR_TYPE_MAX			(18)						//????も含む
#define STAGE_TYPE_LEVEL_MAX		(10)						//タイプレベルの最大

#define STAGE_ENTRY_POKE_MAX		(2)							//参加ポケモン数シングル=1 ダブル=2

//タイプの並び
#define TYPE_X_MAX					(4)							//横に並ぶ数
#define TYPE_Y_MAX					(5)							//縦に並ぶ数
#define DUMMY_TYPE					(0xfe)						//無視するポケモンタイプ
#define TYPE_XY_MAX					(TYPE_X_MAX * TYPE_Y_MAX)

//タイプ
#define STAGE_TYPE_SINGLE		(0)
#define STAGE_TYPE_DOUBLE		(1)
#define STAGE_TYPE_MULTI		(2)
#define STAGE_TYPE_WIFI_MULTI	(3)
#define STAGE_TYPE_MAX			(4)								//最大数

//トレーナータイプのデータテーブル定義
#define STAGE_RANK_TR1_NUM		(8)								//
#define STAGE_RANK_TR2_NUM		(4)								//専用
#define STAGE_RANK_TR_MAX		(STAGE_RANK_TR1_NUM+STAGE_RANK_TR2_NUM)

/////////////////////////////////////////////////////////////////////////////////////////
//STAGE_SCRWORKの取得コード(stage.sで使用)
#define FS_ID_SET_MODE				(0)
#define FS_ID_SET_LEVEL				(1)
#define FS_ID_SET_TYPE				(2)
#define FS_ID_GET_RET_WORK			(3)
#define FS_ID_GET_RENSYOU			(4)
#define FS_ID_INC_RENSYOU			(5)
#define FS_ID_INC_TRADE_COUNT		(6)
#define FS_ID_SYSTEM_RESET			(7)
//#define FS_ID_CLEAR_PLAY_DATA		(8)
#define FS_ID_IS_SAVE_DATA_ENABLE	(9)
#define FS_ID_SAVE_REST_PLAY_DATA	(10)
#define FS_ID_PUSH_NOW_LOCATION		(11)
#define FS_ID_POP_NOW_LOCATION		(12)
//#define FS_ID_SET_NG				(13)
#define FS_ID_INC_ROUND				(14)
#define FS_ID_GET_MONSNO			(15)
#define FS_ID_GET_WAZANO			(16)
#define FS_ID_GET_TYPE				(17)
#define FS_ID_GET_LARGE_TYPE		(18)
#define FS_ID_GET_LAP				(19)
#define FS_ID_GET_TR_OBJ_CODE		(20)
#define FS_ID_SET_LOSE				(21)
#define FS_ID_SET_CLEAR				(22)
#define FS_ID_GET_ROUND				(23)
#define FS_ID_GET_RETIRE_FLAG		(24)
//#define FS_ID_SET_PAIR_NAME			(25)
#define FS_ID_GET_TRADE_YESNO_FLAG	(26)
#define FS_ID_SET_TEMP_ITEM			(27)
#define FS_ID_INC_TYPE_LEVEL		(28)
#define FS_ID_COMM_COMMAND_INITIALIZE	(29)
#define FS_ID_CHECK_COMM_TYPE		(30)
#define FS_ID_EFF_FRAME_ON			(31)
#define	FS_ID_SET_B_TOWER_PARTNER_DATA	(32)
#define FS_ID_GET_TYPE_LEVEL		(33)
#define	FS_ID_GET_BP_POINT			(34)
#define	FS_ID_TYPE_LEVEL_MAX		(35)
#define FS_ID_SQRT					(36)
#define FS_ID_CHECK_BRAIN			(37)
#define FS_ID_WND_SET				(38)

//d32r0401.evで使用
#define STAGE_TOOL_CHK_ENTRY_POKE_NUM	(0)
#define STAGE_TOOL_GET_CLEAR_FLAG		(1)
#define STAGE_TOOL_GET_CLEAR_MONSNO		(2)
#define STAGE_TOOL_SET_NEW_CHALLENGE	(3)
#define STAGE_TOOL_SELECT_POKE			(4)

//通信バッファのサイズ(send_buf,recv_buf)
//#define STAGE_COMM_BUF_LEN			(40)
#define STAGE_COMM_BUF_LEN			(44)
#define STAGE_HUGE_BUF_LEN			(512)//(1024)

//通信タイプ
//fssc_stage_sub.c
#define STAGE_COMM_BASIC				(0)
#define STAGE_COMM_TR					(1)
#define STAGE_COMM_ENEMY				(2)
#define STAGE_COMM_RETIRE				(3)
#define STAGE_COMM_TEMOTI				(7)
//stage.c
#define STAGE_COMM_PAIR					(4)
#define STAGE_COMM_PAIR_POKETYPE		(5)
#define STAGE_COMM_PAIR_FINAL_ANSWER	(6)

//通信の時の1人のポケモン数
#define	STAGE_COMM_POKE_NUM			(2)

//総数(マルチなら敵の数が4など、1人あたりは2になる)
#define STAGE_FLAG_SOLO				(0)					//2
#define STAGE_FLAG_TOTAL			(1)					//2+2=4

//通信人数
#define STAGE_COMM_PLAYER_NUM		(2)

//ポケモンアクター表示位置
#define STAGE_POKE_ACT_X			(128)
#define STAGE_POKE_ACT_Y			(56)

//レコードセットに渡す前に制限かけないとダメ！
//仕様書の「セーブデータについて」の欄に最大数が書いてある
#define STAGE_RENSYOU_MAX			(9999)				//連勝数


#endif	//__STAGE_DEF_H__


