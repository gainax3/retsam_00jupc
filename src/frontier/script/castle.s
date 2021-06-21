//==============================================================================
/**
 * @file	castle.s
 * @brief	「キャッスル控え室」フロンティアスクリプト
 * @author	nohara
 * @date	2007.07.04
 */
//==============================================================================

	.text

	.include	"../frontier_def.h"
	.include	"../castle_def.h"
	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_castle_room.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/syswork_def.h"
	.include	"../../particledata/pl_frontier/frontier_particle_def.h"	//SPA
	.include	"../../particledata/pl_frontier/frontier_particle_lst.h"	//EMIT

//--------------------------------------------------------------------
//					     スクリプト本体
//
//	FSW_PARAM0	記録計算用
//	FSW_PARAM1	リスト用
//	FSW_PARAM2	敵トレーナー1のOBJコード
//	FSW_PARAM3	敵トレーナー2のOBJコード
//	FSW_PARAM4	取得したCPの値
//	FSW_PARAM5	初回か判別
//	FSW_PARAM6	誘導員のOBJコード
//
//	FSW_LOCAL1	タイプ
//	FSW_LOCAL2	通信定義
//	FSW_LOCAL3	記録したか
//	FSW_LOCAL4	保存なし
//	FSW_LOCAL5	対戦かパスか判別
//	FSW_LOCAL6	保存なし
//	FSW_LOCAL7	自分の見た目
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_castle_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_BTL_OFF				//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック
//#define DEBUG_7BTL_OFF				//バトル7回オフデバック

//--------------------------------------------------------------------
//							定義
//--------------------------------------------------------------------
#define CASTLE_PASS_LAP_NUM		(3)	//ぬけみちが登場する周回数


/********************************************************************/
//
//		SCENE_CASTLE:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		BRAINS3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_set_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		BRAINS3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	pcwoman2_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_CASTLE:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_SIO_USER_0			(1)
#define OBJID_SIO_USER_1			(2)
#define OBJID_BRAIN3_INFO			(3)
#define OBJID_MINE					(4)
#define OBJID_PRINCESS				(5)
#define OBJID_BRAIN3_BTL_ROOM		(6)			/*CPを渡す演出用*/
#define OBJID_PCWOMAN2				(7)
#define OBJID_ETC					(98)
#define OBJID_ETC2					(99)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_LEFT,8*34,8*14,OFF/*自分自身のアクタセット*/
	_ACTOR_DATA				OBJID_BRAIN3_INFO,BRAINS3,WF2DMAP_WAY_C_RIGHT, \
							8*26,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_LEFT, \
							8*34,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*34,8*14,OFF	/*自分自身のアクタセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_LEFT,8*34,8*14,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_LEFT,8*34,8*16,ON
	_ACTOR_DATA				OBJID_BRAIN3_INFO,BRAINS3,WF2DMAP_WAY_C_RIGHT, \
							8*26,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	pcwoman2_set_actor
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							8*34,8*12,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//戦闘後に戻ってきた時
_ACTOR_LABEL	btl_after_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_LEFT,8*28,8*14,OFF/*自分自身のアクタセット*/
	_ACTOR_DATA				OBJID_BRAIN3_INFO,BRAINS3,WF2DMAP_WAY_C_RIGHT, \
							8*26,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_LEFT, \
							8*28,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	btl_after_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_LEFT,8*28,8*14,OFF/*自分自身のアクタセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_LEFT,8*28,8*14,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_LEFT,8*28,8*16,ON
	_ACTOR_DATA				OBJID_BRAIN3_INFO,BRAINS3,WF2DMAP_WAY_C_RIGHT, \
							8*26,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/********************************************************************/
//
//	SCENE_CASTLE_BTL:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_scene2_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		PRINCESS,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_scene2_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		PRINCESS,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	brain_set_resource
	_CHAR_RESOURCE_DATA		BRAINS3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_CASTLE_BTL:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define PLAYER_Y					(8*28)

_ACTOR_LABEL	default_scene2_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*18,OFF	//自分自身のアクターセット
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,PLAYER_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PRINCESS,PRINCESS,WF2DMAP_WAY_C_DOWN, \
							8*16,8*4,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_scene2_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*18,OFF	//自分自身のアクターセット
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*16,PLAYER_Y,ON	/*先頭*/
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*16,PLAYER_Y,ON
	_ACTOR_DATA				OBJID_PRINCESS,PRINCESS,WF2DMAP_WAY_C_DOWN, \
							8*16,8*4,ON,EVENTID_TEST_SCR_OBJ1
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
#define ETC_Y		(8*12)		//(8*13+4)

_ACTOR_LABEL	etc_set_actor
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,ETC_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	etc_set_actor2
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,ETC_Y,ON,EVENTID_TEST_SCR_OBJ1				//先頭
	_ACTOR_DATA			OBJID_ETC2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							8*16,ETC_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------------
//	後から追加するアクター
// LOCAL0 = OBJCODE
//--------------------------------------------------------------------
#define BRAIN_Y		(8*26)		//(8*24-4)

_ACTOR_LABEL	brain_set_actor
	_ACTOR_DATA			OBJID_BRAIN3_BTL_ROOM,BRAINS3,WF2DMAP_WAY_C_DOWN, \
							8*16,BRAIN_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


/********************************************************************/
//
//	SCENE_CASTLE_BYPATH:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_scene3_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_scene3_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_CASTLE_BYPATH:アクター(画面IN時に常駐させるアクター群)
//====================================================================
_ACTOR_LABEL	default_scene3_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*6,8*24,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_scene3_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*14,OFF	/*自分自身のアクターセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*6,8*24,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*8,8*24,ON
	_ACTOR_DATA_END


/********************************************************************/
//
//					アニメーションデータ
//
/********************************************************************/

//--------------------------------------------------------------------
//執事前まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_center
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_multi
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA_END

//--------------------------------------------------------------------
//見送る
//--------------------------------------------------------------------
_ANIME_LABEL anm_pcwoman2_go_center
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
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
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//抜け道へ移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_nukemiti_room
	_ANIME_DATA	FC_WAIT_8F,2
	_ANIME_DATA	FC_WALK_L_8F,7
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_nukemiti_room_multi
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_L_8F,8
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL anm_man_go_nukemiti_room
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
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
_ANIME_LABEL anm_scroll_go_btl_center
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_center
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_center_sio
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_center_multi
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//抜け道を抜ける
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_nukemiti_center
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_R_8F,11
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_nukemiti_center_multi
	_ANIME_DATA	FC_WALK_U_8F,5
	_ANIME_DATA	FC_WALK_R_8F,11
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_room_in
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_sio
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_multi
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

/*ブレーン非表示で定位置へ*/
_ANIME_LABEL etc_anime_room_in_brain
	//_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

/*ブレーン表示*/
_ANIME_LABEL etc_anime_room_in_brain_02
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームから出て行く
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_go_out
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_sio
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_multi
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_brain
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_D_8F,5
	_ANIME_DATA_END

//--------------------------------------------------------------------
//ブレーンがバトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL brain_anime_room_in
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//ブレーンがバトルルームから出て行く
//--------------------------------------------------------------------
_ANIME_LABEL brain_anime_go_out
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA	FC_WALK_D_8F,6
	_ANIME_DATA_END

//--------------------------------------------------------------------
//プリンセス	話す
//--------------------------------------------------------------------
_ANIME_LABEL princess_anime_talk
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END


/********************************************************************/
//
//						レンタル～対戦～交換
//
/********************************************************************/
fss_castle_start:
	//_EVENT_START

	/*lobbyで選択したタイプとレベルが必要*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_CASTLE_TYPE,FSW_LOCAL1	/*type*/
	_SAVE_EVENT_WORK_GET	LOCALWORK3,FSW_LOCAL3				/*init*/
	_SAVE_EVENT_WORK_GET	LOCALWORK2,FSW_LOCAL4				/*pos1*/
	_SAVE_EVENT_WORK_GET	LOCALWORK5,FSW_LOCAL5				/*pos2*/
	_SAVE_EVENT_WORK_GET	LOCALWORK6,FSW_LOCAL6				/*pos3*/

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	/*初回フラグ*/
	_LDVAL				FSW_PARAM5,0

	/*ここではまだワークが確保されていない*/
	//_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	//_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_start_multi
	_IFVAL_JUMP			FSW_LOCAL1,EQ,CASTLE_TYPE_MULTI,ev_castle_start_multi
	_IFVAL_JUMP			FSW_LOCAL1,EQ,CASTLE_TYPE_WIFI_MULTI,ev_castle_start_multi

	_LDVAL				FSW_PARAM6,BFSM
	_CHAR_RESOURCE_SET	default_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor			/*アクター登録*/
	_JUMP				ev_castle_start_data_set
	_END

ev_castle_start_multi:

	/*勝敗に関係なく通信フロンティアに挑戦(通信は中断がないので先頭で処理しても大丈夫)*/
	_SCORE_ADD			SCORE_ID_FRONTIER_COMM

	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor_multi		/*アクター登録*/
	_JUMP				ev_castle_start_data_set
	_END


/********************************************************************/
//
/********************************************************************/
ev_castle_start_data_set:
	_CHAR_RESOURCE_SET	pcwoman2_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			pcwoman2_set_actor			/*アクター登録*/

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*ワーク確保*/
	_BATTLE_REC_INIT
	_CASTLE_WORK_ALLOC	FSW_LOCAL3,FSW_LOCAL1,FSW_LOCAL4,FSW_LOCAL5,FSW_LOCAL6,FSW_ANSWER

	/*通信：基本情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_comm_basic

	_JUMP				ev_castle_start_2
	_END

/*通信：基本情報やりとり*/
ev_castle_multi_comm_basic:
	_CALL				ev_castle_comm_command_initialize_1

//	_JUMP				ev_castle_multi_comm_basic_retry
//	_END
	_CALL				ev_castle_multi_comm_basic_call

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_BASIC_AFTER
	_COMM_RESET

	_JUMP				ev_castle_start_2
	_END

ev_castle_multi_comm_basic_retry:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_BASIC
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_comm_basic_retry	/*送信失敗は、再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
	_JUMP				ev_castle_start_2
	_END


/********************************************************************/
//
/********************************************************************/
/*通信：基本情報やりとり*/
ev_castle_multi_comm_basic_call:
	_JUMP				ev_castle_multi_comm_basic_call_retry
	_END

ev_castle_multi_comm_basic_call_retry:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_BASIC
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_comm_basic_call_retry/*送信失敗は再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
	_RET


/********************************************************************/
//
/********************************************************************/
ev_castle_start_2:
	/*ワーク初期化*/
	_CASTLE_WORK_INIT	FSW_LOCAL3

	/*通信：トレーナー情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_comm_tr_2

	_JUMP				ev_castle_start_3
	_END

ev_castle_multi_comm_tr_2:

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_TR_BEFORE
	_COMM_RESET

	_JUMP				ev_castle_multi_comm_tr
	_END

/*通信：トレーナー情報やりとり*/
ev_castle_multi_comm_tr:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_TR
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_comm_tr	/*送信失敗は、再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_POKE_DATA
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	1										/*大量データON*/
	_JUMP				ev_castle_multi_comm_poke
	_END

ev_castle_multi_comm_poke:
	/*通信：手持ちやりとり*/
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_TEMOTI
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_comm_poke	/*送信失敗は、再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_BOTH_0
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	0										/*大量データOFF*/
	_JUMP				ev_castle_start_3
	_END


/********************************************************************/
//
/********************************************************************/
ev_castle_start_3:
	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_start_3_multi

	/*中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center
	_OBJ_ANIME			OBJID_MINE,anm_player_go_center
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME_WAIT

	_JUMP				ev_castle_room_start
	_END

ev_castle_start_3_multi:
	/*中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_center
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_center_multi
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME_WAIT
	_JUMP				ev_castle_room_start
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_room_start:
	/*今何人目か取得*/
	_CASTLE_TOOL		FC_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_room_renshou_0		/*0	*/

	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL				FSW_LOCAL3,1
	_JUMP				ev_castle_room_saikai							/*1-7*/
	_END


/********************************************************************/
/*							0人目									*/
/********************************************************************/
ev_castle_room_renshou_0:
	/*通信マルチ*/
	_CASTLE_TOOL	FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_castle_multi_room_enemy_poke_send2_2

	/*(シングル、マルチ親用)敵のPOKEPARTYセット*/
	_CASTLE_BTL_BEFORE_PARTY_SET

	_JUMP			ev_castle_room_go_rental
	_END


/********************************************************************/
/*					敵ポケモンデータを通信							*/
/********************************************************************/
ev_castle_multi_room_enemy_poke_send2_2:

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_ENEMY_BEFORE
	_COMM_RESET			/*画面切り替え前に初期化*/

	_JUMP			ev_castle_multi_room_enemy_poke_send2
	_END

ev_castle_multi_room_enemy_poke_send2:
	/*通信：敵ポケモンやりとり*/
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_ENEMY
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_room_enemy_poke_send2/*送信失敗は再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_ENEMY_AFTER
	_COMM_RESET			/*画面切り替え前に初期化*/

	/*敵ポケモン生成*/
	_CASTLE_TOOL		FC_ID_ENEMY_POKE_CREATE,0,0,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_ENEMY_POKE_CREATE_2,0,0,FSW_ANSWER
	_RET


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_room_go_rental:

	_CASTLE_TOOL		FC_ID_GET_GUIDE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_room_go_first
	_JUMP				ev_castle_room_go_second
	_END

/*「お客様のポケモンの持っている道具を預かります」*/
ev_castle_room_go_first:
	_TALKMSG			msg_castle_room_1			/*初回*/
	_JUMP				ev_castle_room_go_sub
	_END

ev_castle_room_go_second:
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_1_01		/*2回目以降*/
	_JUMP				ev_castle_room_go_sub
	_END

ev_castle_room_go_sub:
	//_SE_PLAY			SEQ_SE_DP_SUTYA
	//_SE_PLAY			SEQ_SE_PL_BAG_030

	/*ここでCP足すと、通信時にずれがでてしまうのでダメ*/
	/*周の最初にCP+10*/
	//_CASTLE_TOOL		FC_ID_GET_TYPE,0,0,FSW_ANSWER
	//_CASTLE_ADD_CP		FSW_ANSWER,10

	/*3周目か取得*/
	_CASTLE_TOOL		FC_ID_GET_LAP,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,CASTLE_PASS_LAP_NUM,ev_castle_room_lap_3

	/*「それでは何をするか選んでください」*/
	_TALKMSG			msg_castle_room_20

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_room_go_rental

	_JUMP				ev_castle_room_go_rental_2
	_END

/*ぬけみち選択出来るようになったよメッセージ*/
ev_castle_room_lap_3:
	_TALKMSG			msg_castle_room_26
	_RET

ev_castle_multi_room_go_rental:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_multi_room_go_rental_retry
	_END

ev_castle_multi_room_go_rental_retry:
#if 0
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_RENTAL
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_multi_room_go_rental_retry/*送信失敗は再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
#endif

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_RENTAL
	_COMM_RESET			/*画面切り替え前に初期化*/
	_JUMP				ev_castle_room_go_rental_2
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_room_go_rental_2:

	/*「それでは何をするか選んでください」*/
	//_TALKMSG			msg_castle_room_20
	_TALKMSG_ALL_PUT	msg_castle_room_21				/*てもちの説明(会話ウィンドウを表示する)*/

	/*CPウィンドウ表示*/
	_CP_WIN_WRITE
	_JUMP				ev_castle_room_go_rental_2_sub
	_END

ev_castle_room_go_rental_2_sub:
	/*メニュー作成*/
	/*3周目以上か取得*/
	_CASTLE_TOOL		FC_ID_GET_LAP,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,LT,CASTLE_PASS_LAP_NUM,ev_castle_room_lap_3_make_list_lt
	_IFVAL_CALL			FSW_LOCAL6,GE,CASTLE_PASS_LAP_NUM,ev_castle_room_lap_3_make_list_ge

	_BMPLIST_MAKE_LIST	msg_castle_room_choice_10,msg_castle_room_21,1	/*てもち*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_11,msg_castle_room_22,2	/*トレーナー*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_13,msg_castle_room_24,4	/*たいせん*/

	/*抜け道作成*/
	_CASTLE_TOOL		FC_ID_GET_LAP,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,GE,CASTLE_PASS_LAP_NUM,ev_castle_room_lap_3_make_list_ge_nukemiti

	_BMPLIST_START

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP				ev_room_choice_24
#endif	//DEBUG_FRONTIER_LOOP

	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_room_choice_21	/*てもち*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_room_choice_22	/*トレーナー*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,4,ev_room_choice_24	/*たいせん*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,5,ev_room_choice_25	/*ぬけみち*/

	/*通信のみリスト強制削除でここに抜けてくる*/
	//_JUMP			ev_castle_send_sel2
	_JUMP			ev_castle_send_sel

	/*
	■子が先に決定
	子送信:		選択リクエスト
	親受信:		リスト抜ける　
	親送信:		それでいいよ　
	子受信:		親の決定をもらう 
	親子:		親の決定フラグ待ち

	■親が先に決定
	親送信:		選択リクエストこれでいこう
	子受信:		リスト抜ける
	子送信:		特に親が受信されても意味のないデータを送る
	親受信:		すでに親決定がされているのでセットはされずに受信
	親子:		親の決定フラグ待ち

	*/
	_END

/*2周目までのウィンドウ位置*/
ev_castle_room_lap_3_make_list_lt:
	_BMPLIST_INIT_EX	24,11,0,0,FSW_ANSWER									/*Bキャンセル無効*/
	//_BMPLIST_INIT_EX	24,11,0,0,FSW_ANSWER									/*Bキャンセル無効*/
	_RET

/*3周目以降に選択出来る*/
ev_castle_room_lap_3_make_list_ge:

	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_lap_3_make_list_lt
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_room_lap_3_make_list_lt

	_BMPLIST_INIT_EX	24,9,0,0,FSW_ANSWER										/*Bキャンセル無効*/
	//_BMPLIST_INIT_EX	24,11,0,0,FSW_ANSWER									/*Bキャンセル無効*/
	_RET

////////////////////////////////
/*ここでANSWERは使用不可！！！*/
////////////////////////////////
ev_castle_room_lap_3_make_list_ge_nukemiti:

	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_PARAM4
	_IFVAL_JUMP			FSW_PARAM4,EQ,1,ev_castle_leader_nukemiti_off
	_IFVAL_JUMP			FSW_PARAM4,EQ,2,ev_castle_leader_nukemiti_off

	_BMPLIST_MAKE_LIST	msg_castle_room_choice_14,msg_castle_room_25,5	/*ぬけみち*/
	//_BMPLIST_MAKE_LIST	msg_castle_room_choice_14,msg_castle_room_25,5		/*ぬけみち*/
	_RET

/*ぬけみちを表示させない*/
ev_castle_leader_nukemiti_off:
	_RET


/********************************************************************/
/*							てもち									*/
/********************************************************************/
ev_room_choice_21:
	/*選択したタイプをセット*/
	_CASTLE_TOOL		FC_ID_SET_SEL_TYPE,CASTLE_SCR_MENU_TEMOTI,0,FSW_ANSWER

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_send_sel_temoti
	_JUMP				ev_castle_temoti_go
	_END

ev_castle_send_sel_temoti:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_send_sel
	_END

ev_castle_temoti_go:
	/*相手の選択が採用されたかチェック*/
	_CASTLE_TOOL		FC_ID_GET_PAIR_DECIDE_CHECK,0,0,FSW_ANSWER
	//_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_castle_pair_temoti_msg
	_CASTLE_TOOL		FC_ID_CLEAR_PARENT_CHECK_FLAG,0,0,FSW_ANSWER		/*クリアしておく*/

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*手持ち画面呼び出し*/
	_TALK_CLOSE

	/*CPウィンドウ削除*/
	_CP_WIN_DEL

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_mine_call_sync

	_CASTLE_MINE_CALL
	//_SET_MAP_PROC

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_comm_command_initialize_2

	/*通信：基本情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_multi_comm_basic_call

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP				ev_castle_room_go_rental_2
	_END

/*相手の選択が採用されたメッセージ*/
ev_castle_pair_temoti_msg:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	_TALKMSG			msg_castle_room_26

	//また同期を取る？

	_RET

/*通信マルチ*/
ev_castle_mine_call_sync:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_MINE_CALL
	_COMM_RESET
	_RET


/********************************************************************/
/*							トレーナー								*/
/********************************************************************/
ev_room_choice_22:
	/*選択したタイプをセット*/
	_CASTLE_TOOL		FC_ID_SET_SEL_TYPE,CASTLE_SCR_MENU_TRAINER,0,FSW_ANSWER

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_send_sel_aite
	_JUMP				ev_castle_trainer_go
	_END

ev_castle_send_sel_aite:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_send_sel
	_END

ev_castle_trainer_go:
	/*相手の選択が採用されたかチェック*/
	_CASTLE_TOOL		FC_ID_GET_PAIR_DECIDE_CHECK,0,0,FSW_ANSWER
	//_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_castle_pair_trainer_msg
	_CASTLE_TOOL		FC_ID_CLEAR_PARENT_CHECK_FLAG,0,0,FSW_ANSWER		/*クリアしておく*/

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*敵トレーナー画面呼び出し*/
	_TALK_CLOSE

	/*CPウィンドウ削除*/
	_CP_WIN_DEL

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_enemy_call_sync

	_CASTLE_ENEMY_CALL
	//_SET_MAP_PROC

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_comm_command_initialize_3

	/*通信：基本情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_multi_comm_basic_call

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP				ev_castle_room_go_rental_2
	_END

/*相手の選択が採用されたメッセージ*/
ev_castle_pair_trainer_msg:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	//_TALKMSG			msg_castle_room_27

	//また同期を取る？

	_RET

/*通信マルチ*/
ev_castle_enemy_call_sync:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_ENEMY_CALL
	_COMM_RESET
	_RET


/********************************************************************/
/*							たいせん								*/
/********************************************************************/
ev_room_choice_24:
	/*選択したタイプをセット*/
	_CASTLE_TOOL		FC_ID_SET_SEL_TYPE,CASTLE_SCR_MENU_TAISEN,0,FSW_ANSWER

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_send_sel_taisen
	_JUMP				ev_castle_taisen_go
	_END

ev_castle_taisen_go:
	_LDVAL				FSW_LOCAL5,0										/*対戦かパスかの判別用*/
	_CASTLE_TOOL		FC_ID_CLEAR_PARENT_CHECK_FLAG,0,0,FSW_ANSWER		/*クリアしておく*/
	_JUMP				ev_castle_room_go
	_END

ev_castle_send_sel_taisen:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_send_sel
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_comm_command_initialize_1:
	_TIME_WAIT			1,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_INIT_1
	_COMM_RESET
	_RET

ev_castle_comm_command_initialize_2:
	_TIME_WAIT			1,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_INIT_2
	_COMM_RESET
	_RET

ev_castle_comm_command_initialize_3:
	_TIME_WAIT			1,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_INIT_3
	_COMM_RESET
	_RET

ev_castle_comm_command_initialize_4:
	_TIME_WAIT			1,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_INIT_4
	_COMM_RESET
	_RET


/********************************************************************/
/*							ぬけみち								*/
/********************************************************************/
ev_room_choice_25:
	/*「○ＣＰかかりますがよろしいですか？」*/
	_NUMBER_NAME		0,CP_USE_NUKEMITI
	_TALKMSG			msg_castle_room_29_1
	_YES_NO_WIN			FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_go_rental_2_sub	/*「いいえ」*/

	/*CPが足りないかチェック*/
	_CASTLE_GET_CP		FSW_LOCAL1,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,LT,CP_USE_NUKEMITI,ev_castle_room_go_rental_3

	/*選択したタイプをセット*/
	_CASTLE_TOOL		FC_ID_SET_SEL_TYPE,CASTLE_SCR_MENU_NUKEMITI,0,FSW_ANSWER

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_send_sel
	_JUMP				ev_castle_nukemiti_go
	_END

ev_castle_room_go_rental_3:
	/*「CPが足りません」*/
	_TALKMSG			msg_castle_room_29_4
	_JUMP				ev_castle_room_go_rental_2_sub
	_END

ev_castle_nukemiti_go:
	_LDVAL				FSW_LOCAL5,1										/*対戦かパスかの判別用*/

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_nukemiti_go_comm
	_JUMP				ev_castle_nukemiti_go_normal
	_END

ev_castle_nukemiti_go_comm:
	/*相手の選択が採用されたかチェック*/
	_CASTLE_TOOL		FC_ID_GET_PAIR_DECIDE_CHECK,0,0,FSW_ANSWER
	//_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_castle_pair_nukemiti_msg

	_CASTLE_TOOL		FC_ID_NUKEMITI_CP_SUB,0,0,FSW_ANSWER
	_CASTLE_TOOL		FC_ID_CLEAR_PARENT_CHECK_FLAG,0,0,FSW_ANSWER		/*クリアしておく*/

	/*「ぬけみちはこっちじゃよ！」*/
	//_TALKMSG_NOSKIP		msg_castle_room_29_5
	//_TIME_WAIT			15,FSW_ANSWER
	//_JUMP				ev_castle_nukemiti_go_2

	/*対戦部屋に行く流れに変更(08/01/08)*/
	_JUMP				ev_castle_room_go
	_END

ev_castle_nukemiti_go_normal:
	_CASTLE_TOOL		FC_ID_GET_TYPE,0,0,FSW_ANSWER
	_CASTLE_SUB_CP		FSW_ANSWER,CP_USE_NUKEMITI

	/*「ぬけみちはこっちじゃよ！」*/
	//_TALKMSG			msg_castle_room_29_5
	//_AB_KEYWAIT
	//_JUMP				ev_castle_nukemiti_go_2

	/*対戦部屋に行く流れに変更(08/01/08)*/
	_JUMP				ev_castle_room_go
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_nukemiti_go_2:
	_TALK_CLOSE

	/*CPウィンドウ削除*/
	_CP_WIN_DEL

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_go_nukemiti_multi

	/*移動アニメ*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_nukemiti_room
	_OBJ_ANIME			OBJID_BRAIN3_INFO,anm_man_go_nukemiti_room
	_OBJ_ANIME			OBJID_MINE,anm_player_go_nukemiti_room
	_OBJ_ANIME_WAIT

	_JUMP				ev_castle_room_go_nukemiti_2
	_END

/*相手の選択が採用されたメッセージ*/
ev_castle_pair_nukemiti_msg:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	//_TALKMSG			msg_castle_room_29

	//また同期を取る？

	_RET

ev_castle_room_go_nukemiti_multi:
	/*移動アニメ*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_nukemiti_room
	_OBJ_ANIME			OBJID_BRAIN3_INFO,anm_man_go_nukemiti_room
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_nukemiti_room
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_nukemiti_room_multi
	_OBJ_ANIME_WAIT

	_JUMP				ev_castle_room_go_nukemiti_2
	_END

ev_castle_room_go_nukemiti_2:
	/*抜け道部屋へマップ切り替え*/
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_go_nukemiti_2_multi

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_BRAIN3_INFO
	_ACTOR_FREE			OBJID_MINE
	_CHAR_RESOURCE_FREE	BRAINS3

	_MAP_CHANGE			FSS_SCENEID_CASTLE_BYPATH

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_scene3_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene3_actor		/*アクター登録*/
	
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*抜け道を抜ける*/
	//_OBJ_ANIME			OBJID_PLAYER,anm_player_go_nukemiti_center
	_OBJ_ANIME			OBJID_MINE,anm_player_go_nukemiti_center
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_CALL				ev_castle_room_return			/*準備部屋に戻る*/
	_JUMP				ev_castle_room_go_nukemiti_3
	_END

ev_castle_room_go_nukemiti_2_multi:
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_ACTOR_FREE			OBJID_BRAIN3_INFO
	_CHAR_RESOURCE_FREE	BRAINS3

	_MAP_CHANGE			FSS_SCENEID_CASTLE_BYPATH

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_CHAR_RESOURCE_SET	default_scene3_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene3_actor_multi		/*アクター登録*/
	
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*抜け道を抜ける*/
	//_OBJ_ANIME			OBJID_PLAYER,anm_player_go_nukemiti_center
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_nukemiti_center
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_nukemiti_center_multi
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_CALL				ev_castle_room_return			/*準備部屋に戻る*/
	_JUMP				ev_castle_room_go_nukemiti_3
	_END


/********************************************************************/
/*					CPはもらえないが先へ進める						*/
/********************************************************************/
ev_castle_room_go_nukemiti_3:
	_CALL				ev_castle_battle_5

	/*PPなどのポケモンデータ更新(敵の正体などのフラグクリアが必要)*/
	_CASTLE_TOOL		FC_ID_BTL_WIN_POKE_DATA,0,0,FSW_ANSWER

	//_JUMP				ev_castle_battle_7

	/*今7人目でない時は頭に戻る(回復しますメッセージはない！)*/
	_CASTLE_TOOL		FC_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,NE,7,ev_castle_room_saikai		/*連勝数が1-6の時*/

	/*バトル記録データはない！*/
	_JUMP				ev_castle_room_7_win_bp						/*7連勝*/
	_END


/********************************************************************/
/*						バトルへ向かう								*/
/********************************************************************/
ev_castle_room_go:
	/*CPウィンドウ削除*/
	_CP_WIN_DEL

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_go_multi

	/*「それでは　おくに　どうぞ」*/
	_TALKMSG			msg_castle_room_2
	_AB_KEYWAIT
	_TALK_CLOSE

	/*移動アニメ*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_btl_room
	_OBJ_ANIME			OBJID_MINE,anm_player_go_btl_room
	_OBJ_ANIME_WAIT

	_JUMP				ev_castle_room_go_2
	_END

ev_castle_room_go_multi:
	/*「それでは　おくに　どうぞ」*/
	_TALKMSG_NOSKIP		msg_castle_room_2
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_TALK_CLOSE

	/*移動アニメ*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_btl_room
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_btl_room
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_btl_room_multi
	_OBJ_ANIME_WAIT
	_JUMP				ev_castle_room_go_2
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_castle_room_go_2:
	/*対戦部屋へマップ切り替え*/
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*初回のみPCWOMAN2を削除*/
	_IFVAL_CALL			FSW_PARAM5,EQ,0,ev_castle_room_pcwoman2_del

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_go_2_multi

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_BRAIN3_INFO
	_ACTOR_FREE			OBJID_MINE
	_CHAR_RESOURCE_FREE	BRAINS3

	_MAP_CHANGE			FSS_SCENEID_CASTLE_BTL
	_CALL				ev_castle_room_common_01

	_CHAR_RESOURCE_SET	default_scene2_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor		/*アクター登録*/
	
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_scroll_go_btl_center
	_OBJ_ANIME			OBJID_MINE,anm_player_go_btl_center
	_OBJ_ANIME_WAIT

	/*スクロールが終わったらキラキラさせる*/
	_PARTICLE_ADD_EMITTER	SPAWORK_0,CASTLE_BC_KIRA

	/*パスを選択していたら*/
	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_castle_pass

	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_leader_1st_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_leader_2nd_02

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*敵トレーナーが中央まで歩く*/
	_CALL				ev_castle_trainer_set_1
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in
	_OBJ_ANIME_WAIT

	_TIME_WAIT			15,FSW_ANSWER

	_OBJ_ANIME			OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_CASTLE_TOOL	FC_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_CASTLE_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE

	_JUMP				ev_castle_room_go_2_sub
	_END

ev_castle_room_go_2_sub:
	_JUMP				ev_castle_battle
	_END

ev_castle_room_go_2_multi:
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_ACTOR_FREE			OBJID_BRAIN3_INFO
	_CHAR_RESOURCE_FREE	BRAINS3

	_MAP_CHANGE			FSS_SCENEID_CASTLE_BTL
	_CALL				ev_castle_room_common_01

	_CHAR_RESOURCE_SET	default_scene2_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor_multi		/*アクター登録*/
	
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_scroll_go_btl_center
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_btl_center_sio
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_btl_center_multi
	_OBJ_ANIME_WAIT

	/*スクロールが終わったらキラキラさせる*/
	_PARTICLE_ADD_EMITTER	SPAWORK_0,CASTLE_BC_KIRA

	/*パスを選択していたら*/
	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_castle_pass

	/*敵トレーナーが中央まで歩く*/
	_CALL				ev_castle_trainer_set_2
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_sio
	_OBJ_ANIME			OBJID_ETC2,etc_anime_room_in_multi
	_OBJ_ANIME_WAIT

	_TIME_WAIT			15,FSW_ANSWER

	_OBJ_ANIME			OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_CASTLE_TOOL	FC_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_CASTLE_APPEAR	0
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	_OBJ_ANIME			OBJID_ETC2,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_CASTLE_TOOL	FC_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_CASTLE_APPEAR	1
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait

	_JUMP			ev_castle_battle
	_END

/*初回のみPCWOMAN2を削除*/
ev_castle_room_pcwoman2_del:
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_LDVAL				FSW_PARAM5,1
	_RET

/*シングル、マルチの共通処理*/
ev_castle_room_common_01:
	/*パーティクル演出*/
	_PARTICLE_SPA_LOAD	SPAWORK_0,CASTLE_SPA,FSS_CAMERA_ORTHO

	/*周回数によってBGの一部を変更する*/
	_CASTLE_TOOL		FC_ID_LAP_NUM_WRITE,0,0,FSW_ANSWER

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7
	_RET


/********************************************************************/
/*						パスを選択していたら						*/
/********************************************************************/
ev_castle_pass:

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_pass_multi

	/*パスした時のお嬢様のメッセージ*/
	_TALKMSG			msg_castle_room_35
	_AB_KEYWAIT
	_TALK_CLOSE
	_JUMP				ev_castle_battle_3_2
	_END

ev_castle_pass_multi:
	/*パスした時のお嬢様のメッセージ*/
	_TALKMSG_NOSKIP		msg_castle_room_35
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	//_TALK_CLOSE

	_TALKMSG_NOSKIP		msg_castle_room_wait

	//_JUMP				ev_castle_battle_3_2
	_CALL				ev_castle_battle_5
	_CALL				ev_castle_battle_6_2
	_TALK_CLOSE												/*★*/
	_JUMP				ev_castle_battle_3_sub
	_END


/********************************************************************/
/*					トレーナーの見た目セット						*/
/********************************************************************/
ev_castle_trainer_set_1:
	_CASTLE_TOOL		FC_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK				FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET			etc_set_actor
	_RET

ev_castle_trainer_set_2:
	_CASTLE_TOOL		FC_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK				FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	//_ACTOR_SET			etc_set_actor

	_CASTLE_TOOL		FC_ID_GET_TR_OBJ_CODE,1,0,FSW_ANSWER
	_LDWK				FSW_PARAM3,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource2
	_ACTOR_SET			etc_set_actor2
	_RET

/*ブレーンセット*/
ev_castle_trainer_set_brain:
	_LDVAL				FSW_PARAM2,BRAINS3
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET			etc_set_actor
	_RET


/********************************************************************/
//
/********************************************************************/
ev_castle_battle:
	/*フェードアウト*/
	//_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*エンカウントエフェクト前にキラキラ終了させる*/
	_PARTICLE_SPA_EXIT	SPAWORK_0

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_battle_2

	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_leader_1st_33
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_leader_2nd_33

	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_ZIGZAG
	_JUMP				ev_castle_battle_2
	_END

ev_castle_leader_1st_33:
ev_castle_leader_2nd_33:
	_RECORD_INC			RECID_FRONTIER_BRAIN
	_BRAIN_ENCOUNT_EFFECT	FRONTIER_NO_CASTLE
	_JUMP				ev_castle_battle_2
	_END

/*通信マルチ*/
ev_castle_multi_battle_2:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_BATTLE
	_COMM_RESET

	_TALK_CLOSE
	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_ZIGZAG
	_JUMP				ev_castle_battle_2
	_END


/********************************************************************/
//
/********************************************************************/
ev_castle_battle_2:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	//_PARTICLE_SPA_EXIT	SPAWORK_0

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	_CASTLE_BATTLE_CALL
	_CASTLE_CALL_GET_RESULT
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*パーティクル演出*/
	_PARTICLE_SPA_LOAD	SPAWORK_0,CASTLE_SPA,FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_0,CASTLE_BC_KIRA

	/*周回数によってBGの一部を変更する*/
	_CASTLE_TOOL		FC_ID_LAP_NUM_WRITE,0,0,FSW_ANSWER

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*戦闘結果で分岐*/
	_CASTLE_LOSE_CHECK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_lose		/*敗北*/
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*勝利*/
	_RECORD_INC			RECID_CASTLE_WIN

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_battle_2_multi

	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_leader_1st_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_leader_2nd_03

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out
	_OBJ_ANIME_WAIT
	_JUMP				ev_castle_battle_2_sub
	_END

ev_castle_battle_2_sub:
	_ACTOR_FREE			OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_JUMP				ev_castle_battle_3
	_END

ev_castle_battle_2_multi:
	_CALL				ev_castle_comm_command_initialize_4

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out_sio		/*下*/
	_OBJ_ANIME			OBJID_ETC2,etc_anime_go_out_multi	/*上*/
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_ETC
	_ACTOR_FREE			OBJID_ETC2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3

	_JUMP				ev_castle_battle_3
	_END


/********************************************************************/
/*					準備部屋に戻る前にすること						*/
/********************************************************************/
ev_castle_battle_3:
	_CALL				ev_castle_battle_5
	_CALL				ev_castle_battle_6
	_JUMP				ev_castle_battle_3_sub
	_END

/*パスの時*/
ev_castle_battle_3_2:
	_CALL				ev_castle_battle_5
	_CALL				ev_castle_battle_6_2
	_JUMP				ev_castle_battle_3_sub
	_END

ev_castle_battle_3_sub:
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_ACTOR_FREE			OBJID_PRINCESS
	_CHAR_RESOURCE_FREE	PRINCESS

	_CALL				ev_castle_room_return			/*準備部屋に戻る*/
	_JUMP				ev_castle_battle_4
	_END


/********************************************************************/
//
/********************************************************************/
ev_castle_battle_4:
	_JUMP				ev_castle_battle_7
	_END

ev_castle_battle_5:

#ifdef DEBUG_FRONTIER_LOOP
	_RET
#endif	//DEBUG_FRONTIER_LOOP

	/*今何人目+1*/
	_CASTLE_TOOL		FC_ID_INC_ROUND,1,0,FSW_ANSWER

	/*連勝数+1*/
	_CASTLE_TOOL		FC_ID_INC_RENSYOU,0,0,FSW_ANSWER
	_RET

ev_castle_battle_6:
	_PLAYER_NAME		1

	/*CP配布計算*/
	_CASTLE_TOOL		FC_ID_BTL_WIN_CP_POINT,0,0,FSW_PARAM4			/*退避*/
	_NUMBER_NAME		0,FSW_PARAM4

	_OBJ_ANIME			OBJID_PRINCESS,princess_anime_talk
	_OBJ_ANIME_WAIT

	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg00
	_JUMP				ev_castle_single_msg00
	_END

ev_castle_battle_6_sub:
	_TALK_CLOSE

	/*PPなどのポケモンデータ更新*/
	_CASTLE_TOOL		FC_ID_BTL_WIN_POKE_DATA,0,0,FSW_ANSWER

	_CHAR_RESOURCE_SET	brain_set_resource
	_ACTOR_SET			brain_set_actor
	_OBJ_ANIME			OBJID_BRAIN3_BTL_ROOM,brain_anime_room_in
	_OBJ_ANIME_WAIT

	/*取得CPによってメッセージをかえる*/
	_PLAYER_NAME		1
	_IFVAL_JUMP			FSW_PARAM4,GE,40,ev_castle_cp_num_01
	_IFVAL_JUMP			FSW_PARAM4,GE,25,ev_castle_cp_num_02
	_IFVAL_JUMP			FSW_PARAM4,GE,15,ev_castle_cp_num_03
	_JUMP				ev_castle_cp_num_04
	_END


/********************************************************************/
/*							CP渡すメッセージ						*/
/********************************************************************/
ev_castle_cp_num_01:
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg01
	_JUMP				ev_castle_single_msg01
	_END

ev_castle_cp_num_02:
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg02
	_JUMP				ev_castle_single_msg02
	_END

ev_castle_cp_num_03:
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg03
	_JUMP				ev_castle_single_msg03
	_END

ev_castle_cp_num_04:
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg04
	_JUMP				ev_castle_single_msg04
	_END

ev_castle_cp_num_common:
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_multi_msg05
	_JUMP				ev_castle_single_msg05
	_END

ev_castle_cp_num_common_sub:
	_ME_PLAY			ME_CP_GET
	_ME_WAIT
	_TALK_CLOSE

	_OBJ_ANIME			OBJID_BRAIN3_BTL_ROOM,brain_anime_go_out
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_BRAIN3_BTL_ROOM
	_CHAR_RESOURCE_FREE	BRAINS3
	////////////////////////////////////////////////////////////////////

	/*通信：基本情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_cp_num_multi
	_RET


/********************************************************************/
/*			CPを渡した後「しばらくおまちください」マルチ用			*/
/********************************************************************/
ev_castle_cp_num_multi:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_CALL				ev_castle_multi_comm_basic_call
	_TALK_CLOSE												/*★*/
	_RET


/********************************************************************/
/*							パス用									*/
/********************************************************************/
ev_castle_battle_6_2:
	/*CP配布計算*/
	//_CASTLE_TOOL		FC_ID_BTL_WIN_CP_POINT,0,0,FSW_ANSWER

	/*PPなどのポケモンデータ更新*/
	_CASTLE_TOOL		FC_ID_BTL_WIN_POKE_DATA,0,0,FSW_ANSWER

	/*通信：基本情報やりとり*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_multi_comm_basic_call
	_RET

ev_castle_battle_7:

#ifndef DEBUG_7BTL_OFF	/********************************************/
	/*今7人目でない時は頭に戻る*/
	_CASTLE_TOOL		FC_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,NE,7,ev_castle_room_renshou_17		/*連勝数が1-6の時*/
#endif	/************************************************************/

	_JUMP				ev_castle_room_7_win
	_END


/********************************************************************/
/*							7連勝した(記録)							*/
/********************************************************************/
ev_castle_room_7_win:
	/*パスを選択していたら*/
	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_castle_room_7_win_bp

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_castle_room_7_win_bp_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_castle_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_7_win_bp
	_JUMP				ev_castle_room_7_win_rec_yes
	_END

ev_castle_room_7_win_bp_sub:
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_room_7_win_bp
	_END

/*「はい」*/
ev_castle_room_7_win_rec_yes:
	_CALL				ev_castle_room_rec_win		/*勝利の記録*/
	_JUMP				ev_castle_room_7_win_bp
	_END


/********************************************************************/
/*							7連勝した(BP取得)						*/
/********************************************************************/
ev_castle_room_7_win_bp:
	//7連勝(クリア)パラメータセット
	_CASTLE_TOOL		FC_ID_SET_CLEAR,0,0,FSW_ANSWER

	/*7連勝したので終了へ*/
	//_LDVAL			WK_SCENE_CASTLE_LOBBY,1				/*7連勝から始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_CASTLE_LOBBY,1			/*7連勝から始まるようにしておく*/

	/*「７せん　とっぱ　おめでとう」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_31

	_PLAYER_NAME		0										/*プレイヤー名セット*/

	/*バトルポイント追加*/
	_CASTLE_TOOL		FC_ID_GET_BP_POINT,0,0,FSW_ANSWER
	_NUMBER_NAME		1,FSW_ANSWER							/*数値セット*/
	_BTL_POINT_ADD		FSW_ANSWER

	/*「ＢＰをもらった」*/
	_TALKMSG			msg_castle_room_32
	_ME_PLAY			ME_BP_GET
	_ME_WAIT

	_JUMP				ev_castle_room_bp_end
	_END


/********************************************************************/
/*							7連勝した終了							*/
/********************************************************************/
ev_castle_room_bp_end:
	_JUMP				ev_castle_lose_timing
	_END


/********************************************************************/
/*							連勝数 1-7								*/
/********************************************************************/
ev_castle_room_renshou_17:
	/*パスを選択していたら*/
	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_castle_room_saikai

	/*瀕死のポケモンがいるか取得*/
	_CASTLE_TOOL		FC_ID_GET_HINSI_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_room_saikai	/*いない*/

	/*「ポケモンの状態を治します」*/
	_TALKMSG			msg_castle_room_3
	//_ME_PLAY			SEQ_ASA
	//_ME_WAIT
	_SE_PLAY			SEQ_SE_DP_UG_020
	_SE_WAIT			SEQ_SE_DP_UG_020

	/*バトル呼び出しの戦闘パラメータを生成する時に状態異常＋気絶回復処理を入れている*/
	//_PC_KAIFUKU

	_JUMP				ev_castle_room_saikai
	_END


/********************************************************************/
/*						中断セーブからの再開						*/
/********************************************************************/
ev_castle_room_saikai:
	/*ブレーン登場かチェック*/
	_CASTLE_TOOL		FC_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_leader_1st_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_leader_2nd_01

	_JUMP				ev_castle_room_saikai_00
	_END

ev_castle_room_saikai_00:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_menu_before_wait

	/*「次は　○戦目ですよ」*/
	_CASTLE_TOOL		FC_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK				FSW_ANSWER,1
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG			msg_castle_room_4
	_JUMP				ev_castle_room_saikai_02_sub
	_END

/*メニュー表示前の同期*/
ev_castle_room_menu_before_wait:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_MENU_BEFORE
	_COMM_RESET

	/*連勝数をセット*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*「次は　○戦目ですよ」*/
	_CASTLE_TOOL		FC_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK				FSW_ANSWER,1
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG_ALL_PUT	msg_castle_room_4
	_JUMP				ev_castle_room_saikai_02_sub
	_END

ev_castle_room_saikai_02_sub:
	_JUMP			ev_castle_room_menu_make
	_END


/********************************************************************/
/*							メニュー生成							*/
/********************************************************************/
ev_castle_room_menu_make:
	_CASTLE_TOOL	FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_castle_room_menu_make_comm

	/*パスを選択していたら「きろくする」メニューなし*/
	_IFVAL_JUMP		FSW_LOCAL5,EQ,1,ev_castle_room_menu_make_03_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_castle_room_menu_make_04_set

	_JUMP			ev_castle_room_menu_make_03_set
	_END

/*通信の時のメニュー*/
ev_castle_room_menu_make_comm:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_castle_room_menu_make_01_set

	/*パスを選択していたら「きろくする」メニューなし*/
	_IFVAL_JUMP		FSW_LOCAL5,EQ,1,ev_castle_room_menu_make_01_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_castle_room_menu_make_02_set

	_JUMP			ev_castle_room_menu_make_01_set
	_END

ev_castle_room_menu_make_01_set:
	_LDVAL				FSW_ANSWER,1
	_JUMP				ev_castle_room_menu_make_sub
	_END

ev_castle_room_menu_make_02_set:
	_LDVAL				FSW_ANSWER,2
	_JUMP				ev_castle_room_menu_make_sub
	_END

ev_castle_room_menu_make_03_set:
	_LDVAL				FSW_ANSWER,3
	_JUMP				ev_castle_room_menu_make_sub
	_END

ev_castle_room_menu_make_04_set:
	_LDVAL				FSW_ANSWER,4
	_JUMP				ev_castle_room_menu_make_sub
	_END

/*どのメニュー形式にするか*/
ev_castle_room_menu_make_sub:
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_menu_make_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_castle_room_menu_make_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,3,ev_castle_room_menu_make_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,4,ev_castle_room_menu_make_04
	_END

ev_castle_room_menu_make_01:
	_BMPLIST_INIT_EX	24,13,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_JUMP			ev_castle_room_yasumu_next
	_END

ev_castle_room_menu_make_02:
	_BMPLIST_INIT_EX	23,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1		/*きろくする*/
	_JUMP			ev_castle_room_yasumu_next
	_END

ev_castle_room_menu_make_03:
	_BMPLIST_INIT_EX	24,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2		/*やすむ*/
	_JUMP			ev_castle_room_yasumu_next
	_END

ev_castle_room_menu_make_04:
	_BMPLIST_INIT_EX	23,9,0,0,FSW_PARAM1				/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1		/*きろくする*/
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2		/*やすむ*/
	_JUMP			ev_castle_room_yasumu_next
	_END

ev_castle_room_yasumu_next:
	_BMPLIST_MAKE_LIST	msg_castle_room_choice_04,FSEV_WIN_TALK_MSG_NONE,3		/*リタイア*/
	_BMPLIST_START
	_JUMP			ev_castle_room_menu_sel
	_END


/********************************************************************/
/*							メニュー結果							*/
/********************************************************************/
ev_castle_room_menu_sel:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP				ev_room_choice_01
#endif	//DEBUG_FRONTIER_LOOP

	_IFVAL_JUMP			FSW_PARAM1,EQ,0,ev_room_choice_01	/*つづける*/
	_IFVAL_JUMP			FSW_PARAM1,EQ,1,ev_room_choice_02	/*きろくする*/
	_IFVAL_JUMP			FSW_PARAM1,EQ,2,ev_room_choice_03	/*やすむ*/
	_IFVAL_JUMP			FSW_PARAM1,EQ,3,ev_room_choice_04	/*リタイア*/
	_JUMP				ev_room_choice_04
	_END


/********************************************************************/
/*						「つづける」								*/
/********************************************************************/
ev_room_choice_01:
	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_room_choice_01_multi
	_JUMP				ev_room_choice_01_2
	_END

/*通信マルチ*/
ev_room_choice_01_multi:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	_TALKMSG			msg_castle_room_10
	_JUMP				ev_room_choice_01_multi_retry
	_END

ev_room_choice_01_multi_retry:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_RETIRE
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER							/*0=つづける*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_room_choice_01_multi_retry	/*送信失敗は、再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*相手が選んだメニューを取得*/
	_CASTLE_TOOL		FC_ID_GET_RETIRE_FLAG,0,0,FSW_ANSWER

	/*パートナーがリタイアしたら*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_giveup_yes_multi	/*リタイア*/

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_MENU
	_COMM_RESET

	_JUMP				ev_room_choice_01_2
	_END

/*パートナーがリタイアしたら*/
ev_castle_room_giveup_yes_multi:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	_TALKMSG_NOSKIP		msg_castle_room_12
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_room_giveup_yes_multi_sync
	_END


/********************************************************************/
/**/
/********************************************************************/
ev_room_choice_01_2:
	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	/*次の敵ポケモンを生成*/
	_CASTLE_BTL_AFTER_PARTY_SET		/*バトル後のPOKEPARTYのセット*/

	/*通信マルチ*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_multi_room_enemy_poke_send2_2

	_JUMP				ev_castle_room_go_rental_2
	_END


/********************************************************************/
/*							記録する*/
/********************************************************************/
ev_room_choice_02:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_castle_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_rec_no
	_JUMP				ev_castle_room_rec_yes
	_END

/*「はい」*/
ev_castle_room_rec_yes:
	_CALL				ev_castle_room_rec_win		/*勝利の記録*/
	_JUMP				ev_castle_room_saikai		/*メニューへ戻る*/
	_END

/*「いいえ」*/
ev_castle_room_rec_no:
	_JUMP				ev_castle_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_castle_room_rec_win:
	_CALL				ev_castle_room_rec_common		/*ANSWER,PARAM0使用中*/
	//_ADD_WK			FSW_PARAM0,1
	//_SUB_WK			FSW_PARAM0,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_castle_room_rec
	_END

/*負け*/
ev_castle_room_rec_lose:
	_CALL				ev_castle_room_rec_common		/*ANSWER,PARAM0使用中*/
	_ADD_WK				FSW_PARAM0,1
	_JUMP				ev_castle_room_rec
	_END

/*共通部分*/
ev_castle_room_rec_common:
	/*録画データセーブ*/
	_CASTLE_TOOL		FC_ID_GET_RENSYOU,0,0,FSW_PARAM0
	_RET

ev_castle_room_rec:
	_JUMP				ev_castle_room_rec_sub
	_END

ev_castle_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL3,1

	/*WIFIマルチ対応 録画前にCPレコードを退避して、挑戦時のCPに上書き*/
	_CASTLE_TOOL		FC_ID_WIFI_MULTI_CP_TEMP,0,0,FSW_ANSWER

	_TALKMSG_ALL_PUT	msg_castle_room_6_4
	_CASTLE_TOOL		FC_ID_GET_TYPE,0,0,FSW_ANSWER
	_ADD_WAITICON
	_BATTLE_REC_SAVE	FRONTIER_NO_CASTLE,FSW_ANSWER,FSW_PARAM0,FSW_PARAM0
	_DEL_WAITICON

	/*WIFIマルチ対応 録画後に退避しておいた現在のCPレコードで上書き*/
	_CASTLE_TOOL		FC_ID_WIFI_MULTI_CP_TEMP,1,0,FSW_ANSWER

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_PARAM0,EQ,1,ev_castle_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_castle_room_6_2
	_RET

ev_castle_room_rec_true:
	_SE_PLAY			SEQ_SE_DP_SAVE
	/*「記録されました」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_6_1
	_RET
	
/*録画データがない時メッセージ*/
ev_castle_room_rec_msg1:
	_TALKMSG			msg_castle_room_6
	_RET

/*すでに録画データがある時メッセージ*/
ev_castle_room_rec_msg2:
	_TALKMSG			msg_castle_room_6_3
	_RET


/********************************************************************/
/*							やすむ*/
/********************************************************************/
ev_room_choice_03:
	/*「レポートをかいて終了しますか？」*/
	_TALKMSG			msg_castle_room_7
	_YES_NO_WIN			FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_rest_no
	_JUMP				ev_castle_room_rest_yes
	_END

/*「はい」*/
ev_castle_room_rest_yes:
	//_LDVAL			WK_SCENE_CASTLE_LOBBY,2			/*続きから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_CASTLE_LOBBY,2		/*続きから始まるようにしておく*/

	//プレイデータセーブ
	_CASTLE_TOOL		FC_ID_SAVE_REST_PLAY_DATA,0,0,FSW_ANSWER

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_CASTLE_WORK_FREE

	_CALL				ev_castle_save			/*07.08.24 _CALLに置き換えた*/

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	//リセットコマンド
	_CASTLE_TOOL		FC_ID_SYSTEM_RESET,0,0,FSW_ANSWER
	_END

/*「いいえ」*/
ev_castle_room_rest_no:
	_JUMP				ev_castle_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							リタイア*/
/********************************************************************/
ev_room_choice_04:
	/*「バトルキャッスルの挑戦を中止する？」*/
	_TALKMSG			msg_castle_room_8
	_YES_NO_WIN_EX		FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_giveup_no

	/*通信*/
	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_giveup_yes_multi_wait_msg

	/*「お預かりしていた道具を　お返しします」*/
	_TALKMSG			msg_castle_room_34

	_JUMP				ev_castle_room_giveup_yes
	_END

/*「しばらくお待ちください」*/
ev_castle_room_giveup_yes_multi_wait_msg:
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP				ev_castle_room_giveup_yes_multi_retry
	_END

ev_castle_room_giveup_yes_multi_retry:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_RETIRE
	_CASTLE_SEND_BUF	FSW_LOCAL2,1,FSW_ANSWER									/*1=リタイヤ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_room_giveup_yes_multi_retry	/*送信失敗は再送信*/

	//_CASTLE_RECV_BUF	FSW_LOCAL2
	//_COMM_RESET
	_JUMP				ev_castle_room_giveup_yes_multi_sync
	_END

ev_castle_room_giveup_yes_multi_sync:
	/*「お預かりしていた道具を　お返しします」*/
	_TALKMSG			msg_castle_room_34

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_GIVE
	//_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_MENU
	_COMM_RESET
	_JUMP				ev_castle_room_giveup_yes
	_END

/*「はい」*/
ev_castle_room_giveup_yes:
	//敗戦パラメータセット
	_CASTLE_TOOL		FC_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_CASTLE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_CASTLE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP				ev_castle_room_end_save
	_END

/*「いいえ」*/
ev_castle_room_giveup_no:
	_JUMP				ev_castle_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*								敗北*/
/********************************************************************/
ev_castle_lose:
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_ACTOR_FREE			OBJID_PRINCESS
	_CHAR_RESOURCE_FREE	PRINCESS
	_CALL				ev_castle_room_return			/*準備部屋に戻る*/

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_castle_lose_timing_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_castle_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_castle_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER								/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_castle_lose_rec_no	/*「いいえ」*/
	_JUMP			ev_castle_lose_rec_yes					/*「はい」*/
	_END

ev_castle_lose_timing_sub:
	_TALKMSG_NOSKIP		msg_castle_room_wait
	_JUMP			ev_castle_lose_timing_call
	_END

/*「はい」*/
ev_castle_lose_rec_yes:
	_CALL			ev_castle_room_rec_lose					/*敗北の記録*/
	_JUMP			ev_castle_lose_timing_call
	_END

/*「いいえ」*/
ev_castle_lose_rec_no:
	_JUMP			ev_castle_lose_timing_call
	_END

ev_castle_lose_timing_call:
	//敗戦パラメータセット
	_CASTLE_TOOL		FC_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_CASTLE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_CASTLE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP			ev_castle_lose_timing
	_END

ev_castle_lose_timing:
	/*「お預かりしていた道具を　お返しします」*/
	_TALKMSG		msg_castle_room_34

	_CASTLE_TOOL	FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_castle_multi_lose_timing
	_JUMP			ev_castle_room_end_save
	_END

/*通信同期*/
ev_castle_multi_lose_timing:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_castle_room_wait

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_LOSE_END
	_COMM_RESET
	_JUMP				ev_castle_room_end_save
	_END

ev_castle_room_end_save:
	_CALL				ev_castle_save			/*07.08.24 _CALLに置き換えた*/
	_TALK_CLOSE
	_JUMP				ev_castle_room_end
	_END


/********************************************************************/
/*							共通終了								*/
/********************************************************************/
ev_castle_room_end:

	/*セーブ後に同期させる*/
	_CASTLE_TOOL	FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_castle_multi_end_timing

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_CASTLE_TOOL		FC_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,CASTLE_TYPE_MULTI,ev_castle_room_multi_tv

	_CASTLE_TOOL		FC_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,CASTLE_TYPE_WIFI_MULTI,ev_castle_room_end_wifi

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_CASTLE_WORK_FREE

	/*2Dマップ控え室へ*/
	//

	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*マルチのみ*/
ev_castle_room_multi_tv:
	/*TV処理*/
	_TV_TEMP_FRIEND_SET	FRONTIER_NO_CASTLE
	_RET


/********************************************************************/
/*						セーブ後に同期させる						*/	
/********************************************************************/
ev_castle_multi_end_timing:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_SAVE_AFTER
	_COMM_RESET			/*画面切り替え前に初期化*/
	_RET


/********************************************************************/
/*						WIFIは待ち合わせ画面へ戻る					*/	
/********************************************************************/
ev_castle_room_end_wifi:
	/*フェードアウト*/
	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_CASTLE_WORK_FREE

	_MAP_CHANGE_EX		FSS_SCENEID_WIFI_COUNTER,1
	_END


/********************************************************************/
//							共通セーブ	
/********************************************************************/
ev_castle_save:
	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_castle_room_36
	_ADD_WAITICON
	_REPORT_DIV_SAVE	FSW_ANSWER
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE
	_RET


/********************************************************************/
/*			ランク、ぬけみちなどの選択を通信(たいせん以外)			*/
/********************************************************************/
ev_castle_send_sel:
	_LDVAL				FSW_LOCAL2,CASTLE_COMM_SEL
	_CASTLE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_castle_send_sel	/*送信失敗は、再送信へ*/

	_CASTLE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*親の決定待ち*/
	//_CASTLE_PARENT_WAIT	FSW_ANSWER
	_JUMP				ev_castle_send_sel2
	_END

ev_castle_send_sel2:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_CASTLE_CHANGE
	_COMM_RESET			/*画面切り替え前に初期化*/

	/*親の決定を取得して処理する*/
	_CASTLE_TOOL		FC_ID_GET_PARENT_CHECK_FLAG,0,0,FSW_ANSWER
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			CASTLE_SCR_MENU_TEMOTI,ev_castle_temoti_go			/*てもち*/
	_CASE_JUMP			CASTLE_SCR_MENU_TRAINER,ev_castle_trainer_go		/*トレーナー*/
	_CASE_JUMP			CASTLE_SCR_MENU_TAISEN,ev_castle_taisen_go			/*たいせん*/
	_CASE_JUMP			CASTLE_SCR_MENU_NUKEMITI,ev_castle_nukemiti_go		/*ぬけみち*/
	_END


/********************************************************************/
/*					共通処理：準備部屋に戻る(CALL)					*/
/********************************************************************/
ev_castle_room_return:
	_PARTICLE_SPA_EXIT	SPAWORK_0

	_CASTLE_TOOL		FC_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_castle_room_return_multi

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_MINE
	//_ACTOR_FREE		OBJID_ETC
	//_CHAR_RESOURCE_FREE	FSW_PARAM2

	_MAP_CHANGE			FSS_SCENEID_CASTLE_ROOM

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_LDVAL				FSW_PARAM6,BFSM
	_CHAR_RESOURCE_SET	default_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			btl_after_set_actor			/*アクター登録*/

	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET

ev_castle_room_return_multi:
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	//_ACTOR_FREE		OBJID_ETC
	//_CHAR_RESOURCE_FREE	FSW_PARAM2

	_MAP_CHANGE			FSS_SCENEID_CASTLE_ROOM

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			btl_after_set_actor_multi	/*アクター登録*/

	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET


/********************************************************************/
/*						ブレーンの流れ								*/
/********************************************************************/
ev_castle_leader_1st_01:
ev_castle_leader_2nd_01:

	/*記録したワークにデータが書き込まれていない = 中断復帰として考える*/
	/*登場しますよメッセージは表示しない*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,1,ev_castle_room_saikai_00

	/*一度ブレーン登場メッセージを表示したか*/
	_CASTLE_TOOL	FC_ID_BRAIN_APPEAR_MSG_CHK,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_castle_room_saikai_00

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*「ブレーンとの勝負になります！覚悟はいいですか？」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_boss_01
	_JUMP				ev_castle_room_saikai_02_sub
	_END

ev_castle_leader_1st_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*登場*/
	_CALL				ev_castle_leader_appear

	/*戦闘前のメッセージ(1周目)*/
	_TALKMSG			msg_castle_room_boss_02
	_TALK_CLOSE

	_JUMP				ev_castle_room_go_2_sub
	_END

ev_castle_leader_2nd_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*登場*/
	_CALL				ev_castle_leader_appear

	/*戦闘前のメッセージ(2周目)*/
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_boss_03
	_TALK_CLOSE

	_JUMP				ev_castle_room_go_2_sub
	_END

/*登場*/
ev_castle_leader_appear:
	/*透明にしてブレーンが中央まで歩く*/
	_CALL				ev_castle_trainer_set_brain
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain
	_OBJ_ANIME_WAIT

	/*パーティクル演出*/
	//

	/*表示*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_02
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						ブレーンに勝利後のmsg						*/
/********************************************************************/
ev_castle_leader_1st_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_CASTLE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_NONE,ev_castle_meory_print_put_1st

	/*戦闘後のメッセージ(1周目)*/
	_PLAYER_NAME		0
	_TALKMSG			msg_castle_room_boss_04
	_TALK_CLOSE
	_JUMP				ev_castle_battle_2_sub_brain
	_END

/*受付で1st記念プリントを貰えるようにする*/
ev_castle_meory_print_put_1st:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_CASTLE,MEMORY_PRINT_PUT_1ST
	_RET

ev_castle_leader_2nd_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_castle_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_CASTLE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_PUT_OK_1ST,ev_castle_meory_print_put_2nd

	/*戦闘後のメッセージ(2周目)*/
	_TALKMSG			msg_castle_room_boss_05
	_TALK_CLOSE
	_JUMP				ev_castle_battle_2_sub_brain
	_END

/*受付で2nd記念プリントを貰えるようにする*/
ev_castle_meory_print_put_2nd:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_CASTLE,MEMORY_PRINT_PUT_2ND
	_RET

/*ブレーン専用退場*/
ev_castle_battle_2_sub_brain:
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out_brain
	_OBJ_ANIME_WAIT
	_JUMP				ev_castle_battle_2_sub
	_END


/********************************************************************/
/*					連勝数を取得(FSW_ANSWER使用)					*/
/********************************************************************/
ev_castle_rensyou_get:
	/*連勝数取得*/
	_CASTLE_TOOL		FC_ID_GET_RENSYOU,0,0,FSW_ANSWER

	/*すでに9999の時は、0 オリジンの補正をかけない*/
	_IFVAL_JUMP			FSW_ANSWER,GE,CASTLE_RENSYOU_MAX,ev_castle_rensyou_ret

	_ADD_WK				FSW_ANSWER,1
	_RET

ev_castle_rensyou_ret:
	_RET


/********************************************************************/
/*		(カトレア)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg00:
	_TALKMSG			msg_castle_room_30
	_AB_KEYWAIT
	_JUMP				ev_castle_battle_6_sub
	_END

ev_castle_multi_msg00:
	_TALKMSG_NOSKIP		msg_castle_room_30
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_battle_6_sub
	_END


/********************************************************************/
/*		(コクラン01)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg01:
	_TALKMSG			msg_castle_room_30_01_2
	_JUMP				ev_castle_cp_num_common
	_END

ev_castle_multi_msg01:
	_TALKMSG_NOSKIP		msg_castle_room_30_01
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_cp_num_common
	_END


/********************************************************************/
/*		(コクラン02)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg02:
	_TALKMSG			msg_castle_room_30_02_2
	_JUMP				ev_castle_cp_num_common
	_END

ev_castle_multi_msg02:
	_TALKMSG_NOSKIP		msg_castle_room_30_02
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_cp_num_common
	_END


/********************************************************************/
/*		(コクラン03)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg03:
	_TALKMSG			msg_castle_room_30_03_2
	_JUMP				ev_castle_cp_num_common
	_END

ev_castle_multi_msg03:
	_TALKMSG_NOSKIP		msg_castle_room_30_03
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_cp_num_common
	_END


/********************************************************************/
/*		(コクラン04)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg04:
	_TALKMSG			msg_castle_room_30_04_2
	_JUMP				ev_castle_cp_num_common
	_END

ev_castle_multi_msg04:
	_TALKMSG_NOSKIP		msg_castle_room_30_04
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_castle_cp_num_common
	_END


/********************************************************************/
/*		(コクラン05)シングルか、通信かでメッセージ表示方法を分ける	*/
/********************************************************************/
ev_castle_single_msg05:
	_TALKMSG			msg_castle_room_30_05
	/*ME再生に行くのでキー待ちいらない*/
	_JUMP				ev_castle_cp_num_common_sub
	_END

ev_castle_multi_msg05:
	_TALKMSG_NOSKIP		msg_castle_room_30_05
	_JUMP				ev_castle_cp_num_common_sub
	_END


