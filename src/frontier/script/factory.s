//==============================================================================
/**
 * @file	factory.s
 * @brief	「ファクトリー控え室」フロンティアスクリプト
 * @author	nohara
 * @date	2007.04.11
 */
//==============================================================================

	.text

	.include	"../frontier_def.h"
	.include	"../factory_def.h"
	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_factory_room.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/syswork_def.h"
	.include	"../../particledata/pl_frontier/frontier_particle_def.h"	//SPA
	.include	"../../particledata/pl_frontier/frontier_particle_lst.h"	//EMIT


//--------------------------------------------------------------------
//					     スクリプト本体
//
//	FSW_PARAM0	周回数計算用
//	FSW_PARAM1	リストで使用
//	FSW_PARAM2	敵トレーナー1のOBJコード
//	FSW_PARAM3	敵トレーナー2のOBJコード
//	FSW_PARAM6	案内する人
//	FSW_LOCAL2	通信定義
//	FSW_LOCAL3	記録したか
//	FSW_LOCAL5	録画計算に使用
//	FSW_LOCAL7	主人公の見た目
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_factory_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_BTL_OFF					//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック
//#define DEBUG_7BTL_OFF				//バトル7回オフデバック


/********************************************************************/
//
//		SCENE_FACTORY:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		ASSISTANTW,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		FSW_LOCAL7,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_set_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		ASSISTANTW,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	pcwoman2_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_FACTORY:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_SIO_USER_0			(1)
#define OBJID_SIO_USER_1			(2)
#define OBJID_ASSISTANTW			(3)
#define OBJID_MINE					(4)
#define OBJID_PCWOMAN2				(5)
#define OBJID_ETC					(98)
#define OBJID_ETC2					(99)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_ASSISTANTW,ASSISTANTW,WF2DMAP_WAY_C_DOWN, \
							8*16,8*10,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*24,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*16,8*24,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*18,8*24,ON
	_ACTOR_DATA				OBJID_ASSISTANTW,ASSISTANTW,WF2DMAP_WAY_C_DOWN, \
							8*16,8*10,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*24,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	pcwoman2_set_actor
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							8*12,8*22,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//戦闘後に戻ってきた時
_ACTOR_LABEL	btl_after_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_ASSISTANTW,ASSISTANTW,WF2DMAP_WAY_C_DOWN, \
							8*16,8*10,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*12,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	btl_after_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*16,8*12,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*18,8*12,ON
	_ACTOR_DATA				OBJID_ASSISTANTW,ASSISTANTW,WF2DMAP_WAY_C_DOWN, \
							8*16,8*10,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*12,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/********************************************************************/
//
//	SCENE_FACTORY_BTL:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_scene2_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_scene2_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_FACTORY_BTL:アクター(画面IN時に常駐させるアクター群)
//====================================================================
_ACTOR_LABEL	default_scene2_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	//自分自身のアクターセット
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*24,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_scene2_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	//自分自身のアクターセット
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*16,8*24,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*18,8*24,ON
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,8*24,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------------
//	状況によって変えるリソース
// LOCAL0 = OBJCODE
//--------------------------------------------------------------------
_RESOURCE_LABEL	etc_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	etc_set_resource2
	_CHAR_RESOURCE_DATA		FSW_PARAM3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------------
//	状況によって変えるアクター
// LOCAL0 = OBJCODE
//--------------------------------------------------------------------
_ACTOR_LABEL	etc_set_actor
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,8*8,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	etc_set_actor2
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*18,8*8,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ETC2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							8*16,8*8,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/********************************************************************/
//
//					アニメーションデータ
//
/********************************************************************/

//--------------------------------------------------------------------
//レンタル受付の前まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_center
	_ANIME_DATA	FC_WALK_U_8F,6
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_multi
	_ANIME_DATA	FC_WALK_U_8F,6
	_ANIME_DATA_END

//--------------------------------------------------------------------
//見送る
//--------------------------------------------------------------------
_ANIME_LABEL anm_pcwoman2_go_center
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//バトルルームへ移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_btl_room
	_ANIME_DATA	FC_WAIT_8F,2
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_room_multi
	_ANIME_DATA	FC_WAIT_8F,2
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
_ANIME_LABEL anm_man_left_walk
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_DIR_D,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//話す動作
//--------------------------------------------------------------------
_ANIME_LABEL anm_stay_walk_left
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//バトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_btl_center
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,5
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_center_multi
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,6
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_room_in
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_sio
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_multi
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_02
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームから出て行く
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_go_out
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_L_8F,4
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_sio
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_multi
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_L_8F,4
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//キョロキョロ
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_kyorokyoro
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END


/********************************************************************/
//
//						レンタル～対戦～交換
//
/********************************************************************/
fss_factory_start:
	//_EVENT_START

	/*lobbyで選択したタイプとレベルが必要*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_FACTORY_TYPE,FSW_LOCAL1	/*type*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_FACTORY_LEVEL,FSW_LOCAL2	/*level*/
	_SAVE_EVENT_WORK_GET	LOCALWORK3,FSW_LOCAL3				/*init*/

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	/*ここではまだワークが確保されていない*/
	//_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	//_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_start_multi
	_IFVAL_JUMP		FSW_LOCAL1,EQ,FACTORY_TYPE_MULTI,ev_factory_start_multi
	_IFVAL_JUMP		FSW_LOCAL1,EQ,FACTORY_TYPE_WIFI_MULTI,ev_factory_start_multi

	_LDVAL				FSW_PARAM6,BFSM
	_CHAR_RESOURCE_SET	default_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor			/*アクター登録*/
	_JUMP			ev_factory_start_data_set
	_END

ev_factory_start_multi:

	/*勝敗に関係なく通信フロンティアに挑戦(通信は中断がないので先頭で処理しても大丈夫)*/
	_SCORE_ADD			SCORE_ID_FRONTIER_COMM

	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor_multi		/*アクター登録*/
	_JUMP			ev_factory_start_data_set
	_END

ev_factory_start_data_set:

	/*誘導員*/
	_CHAR_RESOURCE_SET	pcwoman2_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			pcwoman2_set_actor			/*アクター登録*/

	/*ワーク確保*/
	_BATTLE_REC_INIT
	_FACTORY_WORK_ALLOC	FSW_LOCAL3,FSW_LOCAL1,FSW_LOCAL2

	/*レール追加*/
	_FACTORY_TOOL	FA_ID_ADD_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*通信：基本情報やりとり*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_comm_basic

	_JUMP			ev_factory_start_2
	_END

/*通信：基本情報やりとり*/
ev_factory_multi_comm_basic:
	_CALL			ev_factory_comm_command_initialize_1
	_JUMP			ev_factory_multi_comm_basic_retry
	_END

ev_factory_multi_comm_basic_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_BASIC
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_multi_comm_basic_retry		/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_BASIC_AFTER
	_COMM_RESET

	_JUMP			ev_factory_start_2
	_END


/********************************************************************/
//
/********************************************************************/
ev_factory_start_2:
	/*ワーク初期化*/
	_FACTORY_WORK_INIT	FSW_LOCAL3

	/*通信：トレーナー情報やりとり*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_comm_tr

	_JUMP			ev_factory_start_3
	_END

/*通信：トレーナー情報やりとり*/
ev_factory_multi_comm_tr:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_TR
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_multi_comm_tr		/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_TR_AFTER
	_COMM_RESET

	_JUMP			ev_factory_start_3
	_END


/********************************************************************/
//
/********************************************************************/
ev_factory_start_3:
	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL			FSW_LOCAL3,0

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_start_3_multi

	/*中央まで歩く*/
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_center
	_OBJ_ANIME		OBJID_MINE,anm_player_go_center
	_OBJ_ANIME		OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME_WAIT

	_JUMP			ev_factory_room_start
	_END

ev_factory_start_3_multi:

	/*中央まで歩く*/
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_center
	_OBJ_ANIME		OBJID_MINE,anm_player_go_center
	_OBJ_ANIME		OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_center
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_center_multi
	_OBJ_ANIME_WAIT

	_JUMP			ev_factory_room_start
	_END

ev_factory_room_start:

	/*誘導員削除*/
	_ACTOR_FREE		OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6

	/*今何人目か取得*/
	_FACTORY_TOOL	FA_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_renshou_0		/*0		*/

	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL			FSW_LOCAL3,1
	_JUMP			ev_factory_room_saikai							/*1-7	*/
	_END


/********************************************************************/
/*							連勝数 0								*/
/********************************************************************/
ev_factory_room_renshou_0:
	/*対戦トレーナーの情報を取得*/
	_CALL			ev_factory_room_tr_info_msg
	_JUMP			ev_factory_room_go_rental
	_END


/********************************************************************/
/*						トレーナー情報								*/
/********************************************************************/
ev_factory_room_tr_info_msg:

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_room_tr_info_msg

	_JUMP			ev_factory_room_tr_info_msg_2
	_END


/********************************************************************/
/*					トレーナー情報(敵トレーナー2人)					*/
/********************************************************************/
ev_factory_multi_room_tr_info_msg:
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_ENEMY_BEFORE
	_COMM_RESET
	_JUMP			ev_factory_multi_room_tr_info_msg_retry
	_END

ev_factory_multi_room_tr_info_msg_retry:
	/*通信：敵ポケモンやりとり*/
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_ENEMY
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_multi_room_tr_info_msg_retry	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_ENEMY_AFTER
	_COMM_RESET

	/*敵ポケモン生成*/
	_FACTORY_TOOL	FA_ID_ENEMY_POKE_CREATE,0,0,FSW_ANSWER

	/*周回数を取得*/
	_FACTORY_TOOL	FA_ID_GET_LAP,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM0,FSW_ANSWER
	_IFVAL_CALL		FSW_PARAM0,EQ,0,ev_factory_multi_lap_0	/*1周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,1,ev_factory_multi_lap_1	/*2周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,2,ev_factory_multi_lap_2	/*3周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,3,ev_factory_multi_lap_3	/*4周目*/
	_IFVAL_CALL		FSW_PARAM0,GE,4,ev_factory_multi_lap_4	/*5周目以降*/
	_RET

/*1周目*/
ev_factory_multi_lap_0:
	_FACTORY_TOOL	FA_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	1,FSW_ANSWER,0,0
	_FACTORY_TOOL	FA_ID_GET_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	2,FSW_ANSWER,0,0

	_FACTORY_TOOL	FA_ID_GET_MONSNO,2,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	3,FSW_ANSWER,0,0
	_FACTORY_TOOL	FA_ID_GET_MONSNO,3,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	4,FSW_ANSWER,0,0

	_TALKMSG	msg_factory_room_36
	_RET

/*2周目*/
ev_factory_multi_lap_1:
	_FACTORY_TOOL	FA_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	1,FSW_ANSWER,0,0

	_FACTORY_TOOL	FA_ID_GET_MONSNO,2,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	2,FSW_ANSWER,0,0

	_TALKMSG	msg_factory_room_37
	_RET

/*3周目*/
ev_factory_multi_lap_2:
	_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	_WAZA_NAME		0,FSW_ANSWER

	_FACTORY_TOOL	FA_ID_GET_WAZANO,2,0,FSW_ANSWER
	_WAZA_NAME		1,FSW_ANSWER

	_TALKMSG	msg_factory_room_38
	_RET

/*4周目*/
ev_factory_multi_lap_3:
	/*トレーナー１か２のどちらかのランダムだが、１、２とも知らないのでどちらでもいいはず*/
	_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	_WAZA_NAME		0,FSW_ANSWER

	_TALKMSG	msg_factory_room_39
	_RET

/*5周目*/
ev_factory_multi_lap_4:
	_JUMP			ev_factory_lap_4
	_END


/********************************************************************/
/*					トレーナー情報(敵トレーナー1人)					*/
/********************************************************************/
ev_factory_room_tr_info_msg_2:
	/*周回数を取得*/
	_FACTORY_TOOL	FA_ID_GET_LAP,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM0,FSW_ANSWER
	_IFVAL_CALL		FSW_PARAM0,EQ,0,ev_factory_lap_0	/*1周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,1,ev_factory_lap_1	/*2周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,2,ev_factory_lap_2	/*3周目*/
	_IFVAL_CALL		FSW_PARAM0,EQ,3,ev_factory_lap_3	/*4周目*/
	_IFVAL_CALL		FSW_PARAM0,GE,4,ev_factory_lap_4	/*5周目以降*/
	_RET

/*1周目*/
ev_factory_lap_0:
	//_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	//_WAZA_NAME		0,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	1,FSW_ANSWER,0,0

	//_FACTORY_TOOL	FA_ID_GET_WAZANO,1,0,FSW_ANSWER
	//_WAZA_NAME		2,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	3,FSW_ANSWER,0,0

	//_FACTORY_TOOL	FA_ID_GET_WAZANO,2,0,FSW_ANSWER
	//_WAZA_NAME		4,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,2,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	5,FSW_ANSWER,0,0

	_TALKMSG	msg_factory_room_30
	_RET

/*2周目*/
ev_factory_lap_1:
	//_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	//_WAZA_NAME		0,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	1,FSW_ANSWER,0,0

	//_FACTORY_TOOL	FA_ID_GET_WAZANO,1,0,FSW_ANSWER
	//_WAZA_NAME		2,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	3,FSW_ANSWER,0,0

	_TALKMSG	msg_factory_room_31
	_RET

/*3周目*/
ev_factory_lap_2:
	_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	_WAZA_NAME		0,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	1,FSW_ANSWER,0,0

	_TALKMSG	msg_factory_room_32
	_RET

/*4周目*/
ev_factory_lap_3:
	_FACTORY_TOOL	FA_ID_GET_WAZANO,0,0,FSW_ANSWER
	_WAZA_NAME		0,FSW_ANSWER
	//_FACTORY_TOOL	FA_ID_GET_POKE_TYPE,0,0,FSW_ANSWER
	//_TYPE_NAME		0,FSW_ANSWER

	//_FACTORY_TOOL	FA_ID_GET_POKE_TYPE,1,0,FSW_ANSWER
	//_TYPE_NAME		1,FSW_ANSWER

	//_FACTORY_TOOL	FA_ID_GET_POKE_TYPE,2,0,FSW_ANSWER
	//_TYPE_NAME		2,FSW_ANSWER

	_TALKMSG	msg_factory_room_33
	_RET

/*5周目*/
ev_factory_lap_4:
	_FACTORY_TOOL	FA_ID_GET_LARGE_TYPE,0,0,FSW_ANSWER

	_IFVAL_CALL		FSW_ANSWER,NE,0xff,ev_factory_lap_4_0		/*同じタイプが存在*/
	_IFVAL_CALL		FSW_ANSWER,EQ,0xff,ev_factory_lap_4_1		/*同じタイプがない*/
	_RET

ev_factory_lap_4_0:
	_TYPE_NAME		0,FSW_ANSWER
	_TALKMSG	msg_factory_room_34
	_RET

ev_factory_lap_4_1:
	_TALKMSG	msg_factory_room_35
	_RET

ev_factory_room_go_rental:
	/*「レンタルを　行います」*/
	_TALKMSG	msg_factory_room_1

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_room_go_rental

	_JUMP			ev_factory_room_go_rental_2
	_END

ev_factory_multi_room_go_rental:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_multi_room_go_rental_retry
	_END

ev_factory_multi_room_go_rental_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_RENTAL
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_multi_room_go_rental_retry	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*レンタルポケモン生成*/
	_FACTORY_TOOL	FA_ID_RENTAL_POKE_CREATE,0,0,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_RENTAL
	_COMM_RESET		/*画面切り替え前に初期化*/
	_JUMP			ev_factory_room_go_rental_2
	_END

ev_factory_room_go_rental_2:
	/*フェードアウト*/
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*レンタル呼び出し*/
	_TALK_CLOSE

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_rental_sync

	_FACTORY_RENTAL_CALL
	//_SET_MAP_PROC

	/*レンタルするだけでも交換回数が+1される*/
	_FACTORY_TOOL	FA_ID_INC_TRADE_COUNT,0,0,FSW_ANSWER

	_FACTORY_RENTAL_PARTY_SET		/*レンタルPOKEPARTYのセット*/

	/*レール追加*/
	_FACTORY_TOOL	FA_ID_ADD_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_go_rental_multi

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP			ev_factory_room_go
	_END

ev_factory_room_go_rental_multi:
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_RENTAL_AFTER
	_COMM_RESET
	_RET

/*通信マルチ*/
ev_factory_room_rental_sync:
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_RENTAL_CALL
	_COMM_RESET
	_RET


/********************************************************************/
/*						バトルへ向かう								*/
/********************************************************************/
ev_factory_room_go:
	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_go_multi

	/*「それでは　おくに　どうぞ」*/
	_TALKMSG		msg_factory_room_2
	_AB_KEYWAIT
	_TALK_CLOSE

	/*移動アニメ*/
	_OBJ_ANIME		OBJID_ASSISTANTW,anm_man_left_walk
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_room
	_OBJ_ANIME_WAIT

	_JUMP			ev_factory_room_go_2
	_END

ev_factory_room_go_multi:

	/*「それでは　おくに　どうぞ」*/
	_TALKMSG_NOSKIP	msg_factory_room_2
	_TIME_WAIT		FRONTIER_COMMON_WAIT,FSW_ANSWER
	_TALK_CLOSE

	_CALL			ev_factory_comm_command_initialize_2

	/*移動アニメ*/
	_OBJ_ANIME		OBJID_ASSISTANTW,anm_man_left_walk
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_btl_room_multi
	_OBJ_ANIME_WAIT

	_JUMP			ev_factory_room_go_2
	_END

ev_factory_room_go_2:
	/*対戦部屋へマップ切り替え*/
	_SE_PLAY		SEQ_SE_DP_KAIDAN2
	_SE_WAIT		SEQ_SE_DP_KAIDAN2
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_go_2_multi

	_ACTOR_FREE		OBJID_PLAYER
	_ACTOR_FREE		OBJID_ASSISTANTW
	_ACTOR_FREE		OBJID_MINE
	_CHAR_RESOURCE_FREE	ASSISTANTW
	//_CHAR_RESOURCE_FREE	FSW_LOCAL7

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	_MAP_CHANGE		FSS_SCENEID_FACTORY_BTL

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_scene2_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor		/*アクター登録*/
	
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*中央まで歩く*/
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_btl_center
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_center
	_OBJ_ANIME_WAIT

	/*対戦部屋の地面パレット切り替え*/
	_CALL			ev_btl_room_pal_chg

	/*ブレーン登場かチェック*/
	_FACTORY_TOOL	FA_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_leader_1st_02
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_factory_leader_2nd_02

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*敵トレーナーが中央まで歩く*/
	_CALL			ev_factory_trainer_set_1
	_OBJ_ANIME		OBJID_ETC,etc_anime_room_in
	_OBJ_ANIME_WAIT

	_TIME_WAIT		15,FSW_ANSWER

	_OBJ_ANIME			OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_FACTORY_TOOL	FA_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_FACTORY_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE
	_JUMP			ev_factory_room_go_2_sub
	_END

ev_factory_room_go_2_sub:
	_JUMP			ev_factory_battle
	_END

ev_factory_room_go_2_multi:
	_ACTOR_FREE		OBJID_SIO_USER_0
	_ACTOR_FREE		OBJID_SIO_USER_1
	_ACTOR_FREE		OBJID_ASSISTANTW
	_ACTOR_FREE		OBJID_MINE
	_CHAR_RESOURCE_FREE	ASSISTANTW

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	_MAP_CHANGE		FSS_SCENEID_FACTORY_BTL

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_scene2_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor_multi		/*アクター登録*/
	
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*中央まで歩く*/
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_btl_center
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_center
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_btl_center
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_btl_center_multi
	_OBJ_ANIME_WAIT

	/*対戦部屋の地面パレット切り替え*/
	_CALL			ev_btl_room_pal_chg

	/*敵トレーナーが中央まで歩く*/
	_CALL			ev_factory_trainer_set_2
	_OBJ_ANIME		OBJID_ETC,etc_anime_room_in_sio
	_OBJ_ANIME		OBJID_ETC2,etc_anime_room_in_multi
	_OBJ_ANIME_WAIT

	_TIME_WAIT		15,FSW_ANSWER

	_OBJ_ANIME		OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_FACTORY_TOOL	FA_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_FACTORY_APPEAR	0
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	_OBJ_ANIME		OBJID_ETC2,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_FACTORY_TOOL	FA_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_FACTORY_APPEAR	1
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait

	_JUMP			ev_factory_battle
	_END


/********************************************************************/
/*					トレーナーの見た目セット						*/
/********************************************************************/
ev_factory_trainer_set_1:
	_FACTORY_TOOL	FA_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_RET

ev_factory_trainer_set_2:
	_FACTORY_TOOL	FA_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	//_ACTOR_SET		etc_set_actor

	_FACTORY_TOOL	FA_ID_GET_TR_OBJ_CODE,1,0,FSW_ANSWER
	_LDWK			FSW_PARAM3,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource2
	_ACTOR_SET		etc_set_actor2
	_RET

ev_factory_trainer_set_brain:
	_LDVAL			FSW_PARAM2,BRAINS1
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_RET


/********************************************************************/
//
/********************************************************************/
ev_factory_battle:
	/*フェードアウト*/
	//_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_battle

	/*ブレーン登場かチェック*/
	_FACTORY_TOOL	FA_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_leader_1st_31
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_factory_leader_2nd_31

	_ENCOUNT_EFFECT	FR_ENCOUNT_EFF_CLOSE
	_JUMP			ev_factory_battle_2
	_END

ev_factory_leader_1st_31:
ev_factory_leader_2nd_31:
	_RECORD_INC		RECID_FRONTIER_BRAIN
	_BRAIN_ENCOUNT_EFFECT	FRONTIER_NO_FACTORY_LV50
	//_BRAIN_ENCOUNT_EFFECT	FRONTIER_NO_FACTORY_LV100
	_JUMP			ev_factory_battle_2
	_END

/*通信マルチ*/
ev_factory_multi_battle:
	/*通信：手持ちやりとり*/
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_TEMOTI
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_multi_battle	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*ペアのポケモン生成*/
	_FACTORY_TOOL	FA_ID_PAIR_POKE_CREATE,0,0,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_BATTLE
	_COMM_RESET

	_TALK_CLOSE
	_ENCOUNT_EFFECT	FR_ENCOUNT_EFF_CLOSE
	_JUMP			ev_factory_battle_2
	_END

ev_factory_battle_2:

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	_FACTORY_BATTLE_CALL
	_FACTORY_CALL_GET_RESULT
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*対戦部屋の地面パレット切り替え*/
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,4,0,FSW_ANSWER

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*戦闘結果で分岐*/
	_FACTORY_LOSE_CHECK		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_lose		/*敗北*/
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*勝利*/
	_RECORD_INC		RECID_FACTORY_WIN

	/*ブレーン登場かチェック*/
	_FACTORY_TOOL	FA_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_leader_1st_03
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_factory_leader_2nd_03

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	_JUMP			ev_factory_battle_2_sub
	_END

ev_factory_battle_2_sub:
	_CALL			ev_factory_enemy_tr_go_out			/*敵トレーナー退場アニメ*/
	_JUMP			ev_factory_battle_3
	_END

ev_factory_battle_3:
	_CALL			ev_factory_room_return			/*準備部屋に戻る*/
	_JUMP			ev_factory_battle_4
	_END

ev_factory_battle_4:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_factory_room_renshou_17
#endif	//DEBUG_FRONTIER_LOOP

	/*今何人目+1*/
	_FACTORY_TOOL	FA_ID_INC_ROUND,1,0,FSW_ANSWER

	/*連勝数+1*/
	_FACTORY_TOOL	FA_ID_INC_RENSYOU,0,0,FSW_ANSWER

#ifndef DEBUG_7BTL_OFF	/********************************************/
	/*今7人目でない時は頭に戻る*/
	_FACTORY_TOOL	FA_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,NE,7,ev_factory_room_renshou_17		/*連勝数が1-6の時*/
#endif	/************************************************************/

	_JUMP			ev_factory_room_7_win
	_END


/********************************************************************/
/*							7連勝した(記録)							*/
/********************************************************************/
ev_factory_room_7_win:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_7_win_bp_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_factory_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_7_win_bp
	_JUMP			ev_factory_room_7_win_rec_yes
	_END

ev_factory_room_7_win_bp_sub:
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_room_7_win_bp
	_END

/*「はい」*/
ev_factory_room_7_win_rec_yes:
	_CALL			ev_factory_room_rec_win		/*勝利の記録*/
	_JUMP			ev_factory_room_7_win_bp
	_END


/********************************************************************/
/*							7連勝した(BP取得)						*/
/********************************************************************/
ev_factory_room_7_win_bp:
	//7連勝(クリア)パラメータセット
	_FACTORY_TOOL	FA_ID_SET_CLEAR,0,0,FSW_ANSWER

	/*7連勝したので終了へ*/
	//_LDVAL			WK_SCENE_FACTORY_LOBBY,1				/*7連勝から始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LOBBY,1			/*7連勝から始まるようにしておく*/

	/*「バトルトレード　７せん　とっぱ　おめでとう」*/
	_TALKMSG		msg_factory_lobby_20

	_PLAYER_NAME	0											/*プレイヤー名セット*/

	/*バトルポイント追加*/
	_FACTORY_TOOL	FA_ID_GET_BP_POINT,0,0,FSW_ANSWER
	_NUMBER_NAME	1,FSW_ANSWER								/*数値セット*/
	_BTL_POINT_ADD	FSW_ANSWER

	/*「ＢＰをもらった」*/
	_TALKMSG		msg_factory_lobby_21
	_ME_PLAY		ME_BP_GET
	_ME_WAIT

	_JUMP			ev_factory_room_bp_end
	_END


/********************************************************************/
/*							7連勝した終了							*/
/********************************************************************/
ev_factory_room_bp_end:
	_JUMP			ev_factory_lose_timing
	_END


/********************************************************************/
/*							連勝数 1-7								*/
/********************************************************************/
ev_factory_room_renshou_17:
	/*「ポケモンを　かいふく　しましょう」*/
	_TALKMSG		msg_factory_room_3
	_ME_PLAY		SEQ_ASA
	_ME_WAIT

	/*バトル呼び出しの戦闘パラメータを生成する時に回復処理を入れている*/
	//_PC_KAIFUKU

	_JUMP			ev_factory_room_saikai
	_END


/********************************************************************/
/*						中断セーブからの再開						*/
/********************************************************************/
ev_factory_room_saikai:
	/*ブレーン登場かチェック*/
	_FACTORY_TOOL	FA_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_leader_1st_01
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_factory_leader_2nd_01

	_JUMP			ev_factory_room_saikai_00
	_END

ev_factory_room_saikai_00:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	_JUMP			ev_factory_room_saikai_02
	_END

ev_factory_room_saikai_02:
	/*「次は　○戦目ですよ」*/
	_FACTORY_TOOL	FA_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK			FSW_ANSWER,1
	_NUMBER_NAME	0,FSW_ANSWER
	_TALKMSG		msg_factory_room_4

	_JUMP			ev_factory_room_menu_make
	_END


/********************************************************************/
/*							メニュー生成							*/
/********************************************************************/
ev_factory_room_menu_make:

	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_menu_make_comm

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_factory_room_menu_make_04_set

	_JUMP			ev_factory_room_menu_make_03_set
	_END

/*通信の時のメニュー*/
ev_factory_room_menu_make_comm:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_menu_make_01_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_factory_room_menu_make_02_set

	_JUMP			ev_factory_room_menu_make_01_set
	_END

ev_factory_room_menu_make_01_set:
	_LDVAL				FSW_ANSWER,1
	_JUMP				ev_factory_room_menu_make_sub
	_END

ev_factory_room_menu_make_02_set:
	_LDVAL				FSW_ANSWER,2
	_JUMP				ev_factory_room_menu_make_sub
	_END

ev_factory_room_menu_make_03_set:
	_LDVAL				FSW_ANSWER,3
	_JUMP				ev_factory_room_menu_make_sub
	_END

ev_factory_room_menu_make_04_set:
	_LDVAL				FSW_ANSWER,4
	_JUMP				ev_factory_room_menu_make_sub
	_END

/*どのメニュー形式にするか*/
ev_factory_room_menu_make_sub:
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_factory_room_menu_make_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_factory_room_menu_make_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,3,ev_factory_room_menu_make_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,4,ev_factory_room_menu_make_04
	_END

ev_factory_room_menu_make_01:
	_BMPLIST_INIT_EX	24,13,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_JUMP			ev_factory_room_yasumu_next
	_END

ev_factory_room_menu_make_02:
	_BMPLIST_INIT_EX	23,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1	/*きろくする*/
	_JUMP			ev_factory_room_yasumu_next
	_END

ev_factory_room_menu_make_03:
	_BMPLIST_INIT_EX	24,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2	/*やすむ*/
	_JUMP			ev_factory_room_yasumu_next
	_END

ev_factory_room_menu_make_04:
	_BMPLIST_INIT_EX	23,9,0,0,FSW_PARAM1				/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1	/*きろくする*/
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2	/*やすむ*/
	_JUMP			ev_factory_room_yasumu_next
	_END

ev_factory_room_yasumu_next:
	_BMPLIST_MAKE_LIST	msg_factory_room_choice_04,FSEV_WIN_TALK_MSG_NONE,3	/*リタイア*/
	_BMPLIST_START
	_JUMP			ev_factory_room_menu_sel
	_END


/********************************************************************/
/*							メニュー結果							*/
/********************************************************************/
ev_factory_room_menu_sel:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_room_choice_01
#endif	//DEBUG_FRONTIER_LOOP

	_IFVAL_JUMP		FSW_PARAM1,EQ,0,ev_room_choice_01	/*つづける*/
	_IFVAL_JUMP		FSW_PARAM1,EQ,1,ev_room_choice_02	/*きろくする*/
	_IFVAL_JUMP		FSW_PARAM1,EQ,2,ev_room_choice_03	/*やすむ*/
	_IFVAL_JUMP		FSW_PARAM1,EQ,3,ev_room_choice_04	/*リタイア*/
	_JUMP			ev_room_choice_04
	_END


/********************************************************************/
/*						「つづける」								*/
/********************************************************************/
ev_room_choice_01:
	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_room_choice_01_multi
	_JUMP			ev_room_choice_01_2
	_END

/*通信マルチ*/
ev_room_choice_01_multi:
	/*パートナー名を表示*/
	_PAIR_NAME		0
	_TALKMSG		msg_factory_room_10
	_JUMP			ev_room_choice_01_multi_retry
	_END

ev_room_choice_01_multi_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_RETIRE
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER						/*0=つづける*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_room_choice_01_multi_retry	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*相手が選んだメニューを取得*/
	_FACTORY_TOOL	FA_ID_GET_RETIRE_FLAG,0,0,FSW_ANSWER

	/*パートナーがリタイアしたら*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_giveup_yes_multi	/*リタイア*/

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_MENU
	_COMM_RESET

	_JUMP			ev_room_choice_01_2
	_END

/*パートナーがリタイアしたら*/
ev_factory_room_giveup_yes_multi:
	/*パートナー名を表示*/
	_PAIR_NAME		0
	_TALKMSG_NOSKIP	msg_factory_room_12
	_TIME_WAIT		FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP			ev_factory_room_giveup_yes_multi_sync
	_END

ev_room_choice_01_2:
	/*ワークをクリアしておく*/
	_LDVAL			FSW_LOCAL3,0

	_FACTORY_BTL_AFTER_PARTY_SET		/*バトル後のPOKEPARTYのセット*/

	/*対戦トレーナーの情報を取得*/
	_CALL			ev_factory_room_tr_info_msg

	/*交換モード*/
	_FACTORY_TOOL	FA_ID_SET_MODE,FACTORY_MODE_TRADE,0,FSW_ANSWER

	/*「ポケモンの　こうかんを　しますか？」*/
	_TALKMSG		msg_factory_room_5

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_factory_room_trade_no
#endif	//DEBUG_FRONTIER_LOOP

	_YES_NO_WIN		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_trade_no
	_JUMP			ev_factory_room_trade_yes
	_END

/*「はい」*/
ev_factory_room_trade_yes:

	/*通信*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_trade_yes_multi

	_JUMP			ev_factory_room_trade_yes_pair
	_END

ev_factory_room_trade_yes_multi:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_room_trade_yes_multi_retry
	_END

ev_factory_room_trade_yes_multi_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_TRADE_YESNO
	_FACTORY_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER								/*0=交換したい*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_trade_yes_multi_retry	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*両方、片方が交換を選んでいたら交換画面へ進む*/
	_JUMP			ev_factory_room_trade_yes_pair
	_END

ev_factory_room_trade_yes_pair:
	/*フェードアウト*/
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_room_trade

	_JUMP			ev_factory_room_trade_yes_2
	_END

ev_factory_multi_room_trade:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_TRADE
	_COMM_RESET		/*画面切り替え前に初期化*/
	_JUMP			ev_factory_room_trade_yes_2
	_END

ev_factory_room_trade_yes_2:
	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_trade_sync

	/*交換呼び出し*/
	_FACTORY_TRADE_CALL
	_FACTORY_TRADE_POKE_CHANGE								/*中で交換回数+1*/

	/*レール追加*/
	_FACTORY_TOOL	FA_ID_ADD_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*現在の交換回数+1*/
	//_FACTORY_TOOL	FA_ID_INC_TRADE_COUNT,0,0,FSW_ANSWER	//

	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_go_trade_multi

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP			ev_factory_room_trade_after
	_END

/*通信マルチ*/
ev_factory_room_trade_sync:
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_TRADE_CALL
	_COMM_RESET
	_RET

ev_factory_room_go_trade_multi:
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_TRADE_AFTER
	_COMM_RESET
	_RET

/*「いいえ」*/
ev_factory_room_trade_no:
	/*通信*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_trade_no_multi

	_JUMP			ev_factory_room_trade_no_2
	_END

ev_factory_room_trade_no_multi:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_room_trade_no_multi_retry

ev_factory_room_trade_no_multi_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_TRADE_YESNO
	_FACTORY_SEND_BUF	FSW_LOCAL2,1,FSW_ANSWER								/*1=交換したくない*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_trade_no_multi_retry	/*送信失敗、再送信へ*/

	_FACTORY_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*相手が交換したいかを取得*/
	_FACTORY_TOOL	FA_ID_GET_TRADE_YESNO_FLAG,0,0,FSW_ANSWER

	/*パートナーが交換したいを選んでいたら*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_trade_yes_pair	/*交換画面へ進む*/

	/*二人とも交換を選ばなかった時*/
	_JUMP			ev_factory_room_trade_no_2
	_END

ev_factory_room_trade_no_2:
	_TALK_CLOSE
	_JUMP			ev_factory_room_trade_after
	_END

ev_factory_room_trade_after:
	_FACTORY_TRADE_AFTER_PARTY_SET			/*トレード後のPOKEPARTYのセット*/
	_JUMP			ev_factory_room_go		/*バトルへ*/
	_END


/********************************************************************/
/*							記録する*/
/********************************************************************/
ev_room_choice_02:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_factory_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_rec_no
	_JUMP			ev_factory_room_rec_yes
	_END

/*「はい」*/
ev_factory_room_rec_yes:
	_CALL			ev_factory_room_rec_win		/*勝利の記録*/
	_JUMP			ev_factory_room_saikai		/*メニューへ戻る*/
	_END

/*「いいえ」*/
ev_factory_room_rec_no:
	_JUMP			ev_factory_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_factory_room_rec_win:
	_CALL				ev_factory_room_rec_common		/*LOCAL5使用中*/
	//_ADD_WK			FSW_LOCAL5,1
	//_SUB_WK			FSW_LOCAL5,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_factory_room_rec
	_END

/*負け*/
ev_factory_room_rec_lose:
	_CALL				ev_factory_room_rec_common		/*LOCAL5使用中*/
	_ADD_WK				FSW_LOCAL5,1
	_JUMP				ev_factory_room_rec
	_END

/*共通部分*/
ev_factory_room_rec_common:
	//連勝数取得
	_FACTORY_TOOL		FA_ID_GET_RENSYOU,0,0,FSW_LOCAL5
	_RET

ev_factory_room_rec:
	_JUMP				ev_factory_room_rec_sub
	_END

ev_factory_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL3,1

	_TALKMSG_ALL_PUT	msg_factory_room_6_4
	_ADD_WAITICON
	_SAVE_EVENT_WORK_GET	WK_SCENE_FACTORY_LEVEL,FSW_ANSWER	/*level*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,FACTORY_LEVEL_50,ev_factory_room_rec_lv50
	_JUMP				ev_factory_room_rec_lv100
	_END

/*録画データセーブ*/
ev_factory_room_rec_lv50:
	_FACTORY_TOOL		FA_ID_GET_TYPE,0,0,FSW_ANSWER
	_BATTLE_REC_SAVE	FRONTIER_NO_FACTORY_LV50,FSW_ANSWER,FSW_LOCAL5,FSW_LOCAL5
	_JUMP				ev_factory_room_rec_sub2
	_END

ev_factory_room_rec_lv100:
	_FACTORY_TOOL		FA_ID_GET_TYPE,0,0,FSW_ANSWER
	_BATTLE_REC_SAVE	FRONTIER_NO_FACTORY_LV100,FSW_ANSWER,FSW_LOCAL5,FSW_LOCAL5
	_JUMP				ev_factory_room_rec_sub2
	_END

ev_factory_room_rec_sub2:
	_DEL_WAITICON

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_factory_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_factory_room_6_2
	_RET

ev_factory_room_rec_true:
	_SE_PLAY			SEQ_SE_DP_SAVE
	/*「記録されました」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_factory_room_6_1
	_RET
	
/*録画データがない時メッセージ*/
ev_factory_room_rec_msg1:
	_TALKMSG			msg_factory_room_6
	_RET

/*すでに録画データがある時メッセージ*/
ev_factory_room_rec_msg2:
	_TALKMSG			msg_factory_room_6_3
	_RET


/********************************************************************/
/*							やすむ*/
/********************************************************************/
ev_room_choice_03:
	/*「レポートをかいて終了しますか？」*/
	_TALKMSG		msg_factory_room_7
	_YES_NO_WIN		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_rest_no
	_JUMP			ev_factory_room_rest_yes
	_END

/*「はい」*/
ev_factory_room_rest_yes:
	//_LDVAL			WK_SCENE_FACTORY_LOBBY,2			/*続きから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LOBBY,2		/*続きから始まるようにしておく*/

	//プレイデータセーブ
	_FACTORY_TOOL	FA_ID_SAVE_REST_PLAY_DATA,0,0,FSW_ANSWER

	_CALL			ev_factory_save				/*07.08.24 _CALLに置き換えた*/
	_TALK_CLOSE

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_FACTORY_WORK_FREE

	//リセットコマンド
	_FACTORY_TOOL	FA_ID_SYSTEM_RESET,0,0,FSW_ANSWER

	_END

/*「いいえ」*/
ev_factory_room_rest_no:
	_JUMP			ev_factory_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							リタイア*/
/********************************************************************/
ev_room_choice_04:
	/*「バトルトレードの挑戦を中止する？」*/
	_TALKMSG		msg_factory_room_8
	_YES_NO_WIN_EX	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_giveup_no

	/*通信*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_giveup_yes_multi_wait_msg

	/*「レンタルポケモンと　お預かりしていたポケモンを　交換しましょう！」*/
	_TALKMSG		msg_factory_lobby_23

	_JUMP			ev_factory_room_giveup_yes
	_END

/*「少々お待ちください」*/
ev_factory_room_giveup_yes_multi_wait_msg:
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_room_giveup_yes_multi_retry
	_END

ev_factory_room_giveup_yes_multi_retry:
	_LDVAL			FSW_LOCAL2,FACTORY_COMM_RETIRE
	_FACTORY_SEND_BUF	FSW_LOCAL2,1,FSW_ANSWER								/*1=リタイヤ*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_room_giveup_yes_multi_retry	/*送信失敗、再送信へ*/

	//_FACTORY_RECV_BUF	FSW_LOCAL2
	//_COMM_RESET
	_JUMP			ev_factory_room_giveup_yes_multi_sync
	_END

ev_factory_room_giveup_yes_multi_sync:
	/*「レンタルポケモンと　お預かりしていたポケモンを　交換しましょう！」*/
	_TALKMSG		msg_factory_lobby_23

	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_GIVE
	//_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_MENU
	_COMM_RESET
	_JUMP			ev_factory_room_giveup_yes
	_END

/*「はい」*/
ev_factory_room_giveup_yes:
	//敗戦パラメータセット
	_FACTORY_TOOL	FA_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_FACTORY_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP				ev_factory_room_end_save
	_END

/*「いいえ」*/
ev_factory_room_giveup_no:
	_JUMP			ev_factory_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*								敗北*/
/********************************************************************/
ev_factory_lose:
	_CALL			ev_factory_enemy_tr_go_out				/*敵トレーナー退場アニメ*/
	_CALL			ev_factory_room_return					/*準備部屋に戻る*/

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_factory_lose_timing_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_factory_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER								/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_lose_rec_no	/*「いいえ」*/
	_JUMP			ev_factory_lose_rec_yes					/*「はい」*/
	_END

ev_factory_lose_timing_sub:
	_TALKMSG_NOSKIP	msg_factory_room_wait
	_JUMP			ev_factory_lose_timing_call
	_END

/*「はい」*/
ev_factory_lose_rec_yes:
	_CALL			ev_factory_room_rec_lose				/*敗北の記録*/
	_JUMP			ev_factory_lose_timing_call
	_END

/*「いいえ」*/
ev_factory_lose_rec_no:
	_JUMP			ev_factory_lose_timing_call
	_END

ev_factory_lose_timing_call:
	//敗戦パラメータセット
	_FACTORY_TOOL	FA_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_FACTORY_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP			ev_factory_lose_timing
	_END

ev_factory_lose_timing:
	/*「レンタルポケモンと　お預かりしていたポケモンを　交換しましょう！」*/
	_TALKMSG		msg_factory_lobby_23

	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_multi_lose_timing
	_JUMP			ev_factory_room_end_save
	_END

/*通信同期*/
ev_factory_multi_lose_timing:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_factory_room_wait

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_LOSE_END
	_COMM_RESET
	_JUMP				ev_factory_room_end_save
	_END

ev_factory_room_end_save:
	_CALL				ev_factory_save				/*07.08.24 _CALLに置き換えた*/
	_TALK_CLOSE
	_JUMP				ev_factory_room_end
	_END


/********************************************************************/
/*							共通終了								*/
/********************************************************************/
ev_factory_room_end:

	/*セーブ後に同期させる*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_factory_multi_end_timing

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_FACTORY_TOOL	FA_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,FACTORY_TYPE_MULTI,ev_factory_room_multi_tv

	_FACTORY_TOOL	FA_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,FACTORY_TYPE_WIFI_MULTI,ev_factory_room_end_wifi

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_FACTORY_WORK_FREE

	/*2Dマップ控え室へ*/
	//

	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*マルチのみ*/
ev_factory_room_multi_tv:
	/*TV処理*/
	_TV_TEMP_FRIEND_SET	FRONTIER_NO_FACTORY_LV50
	//_TV_TEMP_FRIEND_SET	FRONTIER_NO_FACTORY_LV100
	_RET


/********************************************************************/
/*						セーブ後に同期させる						*/	
/********************************************************************/
ev_factory_multi_end_timing:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_SAVE_AFTER
	_COMM_RESET			/*画面切り替え前に初期化*/
	_RET


/********************************************************************/
/*						WIFIは待ち合わせ画面へ戻る					*/
/********************************************************************/
ev_factory_room_end_wifi:
	/*フェードアウト*/
	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*レール削除*/
	_FACTORY_TOOL	FA_ID_DEL_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_FACTORY_WORK_FREE

	_MAP_CHANGE_EX		FSS_SCENEID_WIFI_COUNTER,1
	_END


/********************************************************************/
//							共通セーブ	
/********************************************************************/
ev_factory_save:
	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_factory_lobby_24
	_ADD_WAITICON
	_REPORT_DIV_SAVE	FSW_ANSWER
	_DEL_WAITICON
	_SE_PLAY		SEQ_SE_DP_SAVE
	_SE_WAIT		SEQ_SE_DP_SAVE
	_RET


/********************************************************************/
/*						共通：準備部屋に戻る(CALL)					*/	
/********************************************************************/
ev_factory_room_return:
	/*交換部屋に戻る*/
	_SE_PLAY		SEQ_SE_DP_KAIDAN2
	_SE_WAIT		SEQ_SE_DP_KAIDAN2
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_return_multi

	_ACTOR_FREE		OBJID_PLAYER
	_ACTOR_FREE		OBJID_MINE
	//_ACTOR_FREE		OBJID_ETC
	//_CHAR_RESOURCE_FREE	FSW_PARAM2
	//_CHAR_RESOURCE_FREE	FSW_LOCAL7

	_MAP_CHANGE		FSS_SCENEID_FACTORY

	/*レール追加*/
	_FACTORY_TOOL	FA_ID_ADD_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			btl_after_set_actor			/*アクター登録*/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET

ev_factory_room_return_multi:
	_ACTOR_FREE		OBJID_SIO_USER_0
	_ACTOR_FREE		OBJID_SIO_USER_1
	_ACTOR_FREE		OBJID_MINE
	//_ACTOR_FREE		OBJID_ETC
	//_CHAR_RESOURCE_FREE	FSW_PARAM2

	_MAP_CHANGE		FSS_SCENEID_FACTORY

	/*レール追加*/
	_FACTORY_TOOL	FA_ID_ADD_MAP_RAIL_TCB,0,0,FSW_ANSWER

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			btl_after_set_actor_multi	/*アクター登録*/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET


/********************************************************************/
/*						共通：敵トレーナー退場(CALL)				*/	
/********************************************************************/
ev_factory_enemy_tr_go_out:
	/*通信マルチ*/
	_FACTORY_TOOL	FA_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_enemy_tr_go_out_multi

	/*敵トレーナー退場*/
	_OBJ_ANIME		OBJID_ETC,etc_anime_go_out
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_RET

ev_factory_enemy_tr_go_out_multi:
	_CALL			ev_factory_comm_command_initialize_3

	/*敵トレーナー退場*/
	_OBJ_ANIME		OBJID_ETC,etc_anime_go_out_sio
	_OBJ_ANIME		OBJID_ETC2,etc_anime_go_out_multi
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_ACTOR_FREE		OBJID_ETC2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3
	_RET


/********************************************************************/
/*					対戦部屋の地面パレット切り替え					*/
/********************************************************************/
ev_btl_room_pal_chg:
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,0,0,FSW_ANSWER
	_TIME_WAIT		3,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,1,0,FSW_ANSWER
	_TIME_WAIT		3,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,2,0,FSW_ANSWER
	_TIME_WAIT		3,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,3,0,FSW_ANSWER
	_TIME_WAIT		3,FSW_ANSWER
	_FACTORY_TOOL	FA_ID_BTL_ROOM_PAL_CHG,4,0,FSW_ANSWER
	_RET


/********************************************************************/
/*						ブレーンの流れ								*/
/********************************************************************/
ev_factory_leader_1st_01:
ev_factory_leader_2nd_01:

	/*記録したワークにデータが書き込まれていない = 中断復帰として考える*/
	/*登場しますよメッセージは表示しない*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,1,ev_factory_room_saikai_00

	/*一度ブレーン登場メッセージを表示したか*/
	_FACTORY_TOOL	FA_ID_BRAIN_APPEAR_MSG_CHK,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_factory_room_saikai_00

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*「ブレーンとの勝負になります！覚悟はいいですか？」*/
	_TALKMSG			msg_factory_room_boss_01
	_JUMP				ev_factory_room_saikai_02
	_END

ev_factory_leader_1st_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*登場*/
	_CALL				ev_factory_leader_appear

	/*戦闘前のメッセージ(1周目)*/
	_CALL				ev_factory_leader_common
	_TALKMSG			msg_factory_room_boss_02
	_TALK_CLOSE

	_JUMP				ev_factory_room_go_2_sub
	_END

ev_factory_leader_2nd_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*登場*/
	_CALL				ev_factory_leader_appear

	/*戦闘前のメッセージ(2周目)*/
	_CALL				ev_factory_leader_common
	_TALKMSG			msg_factory_room_boss_03
	_TALK_CLOSE

	_JUMP				ev_factory_room_go_2_sub
	_END

/*共通処理*/
ev_factory_leader_common:
	/*主人公のポケモン名をBuf0,1,2にセット*/
	_FACTORY_TOOL		FA_ID_BRAIN_PLAYER_POKE_NAME,0,0,FSW_ANSWER

	_GET_RND			FSW_ANSWER,90				/*0 - 89の間でランダム*/
	_ADD_WK				FSW_ANSWER,10				/*10 - 99の間にする*/
	_NUMBER_NAME		3,FSW_ANSWER
	_RET

/*登場*/
ev_factory_leader_appear:
	/*透明にしてブレーンが中央まで歩く*/
	_CALL				ev_factory_trainer_set_brain
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain
	_OBJ_ANIME_WAIT

	/*キョロキョロ*/
	_OBJ_ANIME		OBJID_MINE,anm_player_kyorokyoro
	_OBJ_ANIME_WAIT

	/*地震セット*/
	//_SHAKE_SET		0,2,0,6
	//_SHAKE_WAIT

	/*パーティクル演出*/
	_PARTICLE_SPA_LOAD	SPAWORK_0,FACTORY_SPA, FSS_CAMERA_ORTHO

	_PARTICLE_ADD_EMITTER	SPAWORK_0, FACTORY_BF_WARP01
	_PARTICLE_ADD_EMITTER	SPAWORK_0, FACTORY_BF_WARP02
	_PARTICLE_ADD_EMITTER	SPAWORK_0, FACTORY_BF_WARP03
	_SE_PLAY			SEQ_SE_PL_FAC01

	/*地震セット*/
	_TIME_WAIT			36,FSW_ANSWER
	//_SE_PLAY			SE_SHAKE
	_SHAKE_SET			0,3,2,10
	//_SHAKE_WAIT				/*なくてもよいが、終了してからマップ遷移などするようにする*/

	/*パーティクル処理中にブレーンを表示*/
	_TIME_WAIT			11,FSW_ANSWER
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_02
	_OBJ_ANIME_WAIT

	//_SE_STOP			SE_SHAKE
	_PARTICLE_WAIT

	_PARTICLE_SPA_EXIT	SPAWORK_0
	_RET


/********************************************************************/
/*						ブレーンに勝利後の流れ						*/
/********************************************************************/
ev_factory_leader_1st_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_FACTORY,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_NONE,ev_factory_meory_print_put_1st

	/*戦闘後のメッセージ(1周目)*/
	_TALKMSG			msg_factory_room_boss_04
	_TALK_CLOSE
	_JUMP				ev_factory_battle_2_sub
	_END

/*受付で1st記念プリントを貰えるようにする*/
ev_factory_meory_print_put_1st:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_FACTORY,MEMORY_PRINT_PUT_1ST
	_RET

ev_factory_leader_2nd_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_factory_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_FACTORY,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_PUT_OK_1ST,ev_factory_meory_print_put_2nd

	/*戦闘後のメッセージ(2周目)*/
	_TALKMSG			msg_factory_room_boss_05
	_TALK_CLOSE
	_JUMP				ev_factory_battle_2_sub
	_END

/*受付で2nd記念プリントを貰えるようにする*/
ev_factory_meory_print_put_2nd:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_FACTORY,MEMORY_PRINT_PUT_2ND
	_RET


/********************************************************************/
/*						連勝数を取得(FSW_ANSWER使用)				*/
/********************************************************************/
ev_factory_rensyou_get:
	/*連勝数取得*/
	_FACTORY_TOOL	FA_ID_GET_RENSYOU,0,0,FSW_ANSWER

	/*すでに9999の時は、0 オリジンの補正をかけない*/
	_IFVAL_JUMP		FSW_ANSWER,GE,FACTORY_RENSYOU_MAX,ev_factory_rensyou_ret

	_ADD_WK			FSW_ANSWER,1
	_RET

ev_factory_rensyou_ret:
	_RET


/********************************************************************/
/*																	*/
/********************************************************************/
ev_factory_comm_command_initialize_1:
	_TIME_WAIT			1,FSW_ANSWER
	_FACTORY_TOOL		FA_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_INIT_1
	_COMM_RESET
	_RET

ev_factory_comm_command_initialize_2:
	_TIME_WAIT			1,FSW_ANSWER
	_FACTORY_TOOL		FA_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_INIT_2
	_COMM_RESET
	_RET

ev_factory_comm_command_initialize_3:
	_TIME_WAIT			1,FSW_ANSWER
	_FACTORY_TOOL		FA_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FACTORY_INIT_3
	_COMM_RESET
	_RET


