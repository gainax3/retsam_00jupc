//==============================================================================
/**
 * @file	stage.s
 * @brief	「バトルステージ」フロンティアスクリプト
 * @author	nohara
 * @date	2007.06.11
 */
//==============================================================================

	.text

	.include	"../frontier_def.h"
	.include	"../stage_def.h"
	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_stage_room.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/syswork_def.h"
	.include	"../../particledata/pl_frontier/frontier_particle_def.h"	//SPA
	.include	"../../particledata/pl_frontier/frontier_particle_lst.h"	//EMIT


//--------------------------------------------------------------------
//					     スクリプト本体
//
//	FSW_PARAM0	リストで使用
//	FSW_PARAM1	主人公の見た目格納
//	FSW_PARAM2	敵トレーナー1のOBJコード
//	FSW_PARAM3	敵トレーナー2のOBJコード
//	FSW_PARAM5	初回か
//	FSW_PARAM6	誘導員のOBJコード
//	FSW_LOCAL3	記録したか
//	FSW_LOCAL5	記録計算用
//	FSW_LOCAL6	通信定義
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_stage_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_10_WIN					//10連勝デバック
//#define DEBUG_BTL_OFF					//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック
//#define DEBUG_ANM_OFF				//アニメオフデバック
//#define DEBUG_7BTL_OFF				//バトル10回オフデバック


/********************************************************************/
//
//		SCENE_STAGE:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_set_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

/*トレーナーで使用するOBJコードは使用してはダメ！*/
_RESOURCE_LABEL	default_set_resource_boygirl
	_CHAR_RESOURCE_DATA		SHOPM1,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		SHOPW1,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

/*対戦部屋*/
_RESOURCE_LABEL	btl_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		MAID,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	btl_set_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		MAID,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_STAGE:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_SIO_USER_0			(1)
#define OBJID_SIO_USER_1			(2)
#define OBJID_PCWOMAN2				(3)
#define OBJID_SHOPM1				(4)
#define OBJID_SHOPW1				(5)
#define OBJID_SHOPM2				(6)
#define OBJID_SHOPW2				(7)
#define OBJID_SHOPM3				(8)
#define OBJID_SHOPW3				(9)
#define OBJID_MINE					(10)
#define OBJID_MAID					(11)
#define OBJID_ETC					(98)
#define OBJID_ETC2					(99)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

_ACTOR_LABEL	way_set_actor
	_PLAYER_ACTOR_DATA	OBJID_PLAYER,WF2DMAP_WAY_C_DOWN,8*15,8*14,OFF/*自分自身のアクタセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							8*16,8*4,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_PARAM1,WF2DMAP_WAY_C_DOWN, \
							8*16,8*4,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	way_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN,8*15,8*14,OFF/*自分自身のアクタセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_DOWN,8*16,8*4,OFF
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_DOWN,8*16,8*4,OFF
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							8*16,8*4,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/*バトル*/
_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA	OBJID_PLAYER,WF2DMAP_WAY_C_DOWN,8*15,8*16,OFF/*自分自身のアクタセット*/
//_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN,8*16,8*16,ON/*自分自身のアクタセット*/
	_ACTOR_DATA				OBJID_MAID,MAID,WF2DMAP_WAY_C_RIGHT, \
							8*6,8*40,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_PARAM1,WF2DMAP_WAY_C_DOWN, \
							8*16,8*16,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN,8*15,8*16,OFF/*自分自身のアクタセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_DOWN,8*16,8*16,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_DOWN,8*16,8*14,ON
	_ACTOR_DATA				OBJID_MAID,MAID,WF2DMAP_WAY_C_RIGHT, \
							8*6,8*40,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/********************************************************************/
//
//	SCENE_STAGE_BTL:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_scene2_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		CAMPBOY, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITRESS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		REPORTER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		TOWERBOSS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		SEVEN5, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_scene2_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		CAMPBOY, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITRESS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		REPORTER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		TOWERBOSS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		SEVEN5, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_STAGE_BTL:アクター(画面IN時に常駐させるアクター群)
//====================================================================

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
/*先頭歩く(1人目はOFFにしておく)*/
_ACTOR_LABEL	etc_set_actor
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,8*28,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/*後ろついてくる*/
_ACTOR_LABEL	etc_set_actor2
	_ACTOR_DATA			OBJID_ETC2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							8*16,8*26,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/********************************************************************/
//
//					アニメーションデータ
//
/********************************************************************/

//--------------------------------------------------------------------
//対戦部屋まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_door
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,9
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_door_multi
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,8
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_door_pcwoman
#if 0
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,10
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
#else
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,9
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
#endif
	_ANIME_DATA_END

//--------------------------------------------------------------------
//話す動作
//--------------------------------------------------------------------
_ANIME_LABEL anm_stay_walk_left
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_center_scroll
	_ANIME_DATA	FC_WALK_D_8F,12
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_scroll_2
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_scroll_3
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_scroll_4
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center
	_ANIME_DATA	FC_WALK_D_8F,12
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

/*通信親(位置上)*/
_ANIME_LABEL anm_sio_player_go_center
	_ANIME_DATA	FC_WALK_D_8F,12
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

/*通信子(位置下)*/
_ANIME_LABEL anm_player_go_center_multi
	_ANIME_DATA	FC_WALK_D_8F,13
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_pcwoman
	_ANIME_DATA	FC_WALK_D_8F,11
	_ANIME_DATA	FC_WALK_L_8F,5
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//両者前へ
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_btl_room
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_room_multi
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_room_etc
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//主人公の前まで来る
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_btl_room_100
	_ANIME_DATA	FC_WALK_L_8F,5
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_btl_room_101
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_room_in
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,6
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

/*通信(位置上)*/
_ANIME_LABEL etc_anime_room_in_sio
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,6
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

/*通信(位置下)*/
_ANIME_LABEL etc_anime_room_in_multi
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,7
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA	FC_WALK_U_8F,6
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA	FC_WAIT_32F,1
	_ANIME_DATA	FC_WAIT_32F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_2
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_2_2
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_2_3
	_ANIME_DATA	FC_WALK_D_8F,4
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームから出て行く
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_go_out
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,7
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

/*通信(位置下)*/
_ANIME_LABEL etc_anime_go_out_sio
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,8
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

/*通信(位置上)*/
_ANIME_LABEL etc_anime_go_out_multi
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,8
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//一歩右へ
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_right
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//一歩左へ
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_left
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//メイド　一歩左へ
//--------------------------------------------------------------------
_ANIME_LABEL anm_maid_go_left
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//入り口まで戻す
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_center_scroll_back
	_ANIME_DATA	FC_WALK_U_8F,12
	_ANIME_DATA_END

//--------------------------------------------------------------------
//右を向く
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_dir_right
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END


/********************************************************************/
//
//								通路
//
/********************************************************************/
fss_stage_start:
	//_EVENT_START

	/*lobbyで選択したタイプが必要*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_STAGE_TYPE,FSW_LOCAL1	/*type*/
	_SAVE_EVENT_WORK_GET	LOCALWORK3,FSW_LOCAL3			/*init*/
	_SAVE_EVENT_WORK_GET	LOCALWORK2,FSW_LOCAL2			/*選択して手持ちポケモン位置*/
	_SAVE_EVENT_WORK_GET	LOCALWORK5,FSW_LOCAL0			/*選択して手持ちポケモン位置2*/

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_PARAM1

	/*初回判別ワーク*/
	_LDVAL				FSW_PARAM5,0

	/*ワーク確保*/
	_BATTLE_REC_INIT
	_STAGE_WORK_ALLOC	FSW_LOCAL3,FSW_LOCAL1,FSW_LOCAL2,FSW_LOCAL0

	_STAGE_TOOL			FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_stage_start_multi

	_LDVAL				FSW_PARAM6,BFSM
	_CHAR_RESOURCE_SET	default_set_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			way_set_actor				/*アクター登録*/
	_JUMP				ev_stage_start_data_set
	_END

ev_stage_start_multi:
	_CALL				ev_stage_comm_command_initialize_1

	/*勝敗に関係なく通信フロンティアに挑戦(通信は中断がないので先頭で処理しても大丈夫)*/
	_SCORE_ADD			SCORE_ID_FRONTIER_COMM

	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			way_set_actor_multi			/*アクター登録*/
	_JUMP				ev_stage_start_data_set
	_END

ev_stage_start_data_set:
	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_STAGE_TOOL			FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_stage_start_data_set_del

	/*対戦部屋まで歩く*/
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_player_go_door_pcwoman
	_OBJ_ANIME			OBJID_MINE,anm_player_go_door
	_OBJ_ANIME_WAIT

	/*対戦部屋へマップ切り替え*/
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2
	_ACTOR_FREE			OBJID_MINE
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_JUMP				ev_stage_start_data_set_del_sub
	_END

ev_stage_start_data_set_del:
	/*対戦部屋まで歩く*/
	_OBJ_ANIME		OBJID_PCWOMAN2,anm_player_go_door_pcwoman
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_door
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_door_multi
	_OBJ_ANIME_WAIT

	/*対戦部屋へマップ切り替え*/
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_ACTOR_FREE			OBJID_PCWOMAN2
	//_ACTOR_FREE			OBJID_MINE
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_JUMP				ev_stage_start_data_set_del_sub
	_END


/********************************************************************/
//
//						対戦部屋(ステージ)
//
/********************************************************************/
ev_stage_start_data_set_del_sub:
	_MAP_CHANGE			FSS_SCENEID_STAGE_BTL

	/*ここではまだワークが確保されていない*/
	_STAGE_TOOL			FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_stage_start_multi_btl_room

	_CHAR_RESOURCE_SET	btl_set_resource			/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor			/*アクター登録*/
	_JUMP				ev_stage_start_data_set_btl_room
	_END

ev_stage_start_multi_btl_room:
	_CHAR_RESOURCE_SET	btl_set_resource_multi		/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor_multi		/*アクター登録*/
	_JUMP				ev_stage_start_data_set_btl_room
	_END

ev_stage_start_data_set_btl_room:
	/*ポケモン表示*/
	_TEMOTI_POKE_ACT_ADD	FSW_LOCAL2,STAGE_POKE_ACT_X,STAGE_POKE_ACT_Y,POKE_DISP_MANAGER_ID_0, 0, 0x0000

	/*スポットライト、ストロボ*/
	_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_SPOT01
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_SPOT01_BG
	_CALL				call_se_pasya_01			/*ストロボ*/
	//_PARTICLE_WAIT

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_WND_SET			72,160,230,192,0,1						/*ウィンドウON*/
	_JUMP				ev_stage_start_2
	_END


/********************************************************************/
//
/********************************************************************/
ev_stage_start_2:
	_JUMP				ev_stage_start_3
	_END


/********************************************************************/
//
/********************************************************************/
ev_stage_start_3:
	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

#ifdef DEBUG_10_WIN	/************************************************/
	_JUMP				ev_stage_room_18_win
#endif	/************************************************************/

	//_SE_PLAY			SEQ_SE_DP_DENDOU				/*歓声*/

	/*通信マルチ*/
	_STAGE_TOOL			FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_stage_start_3_multi

#ifndef DEBUG_ANM_OFF	/********************************************/
	/*中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll
	//_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center
	_OBJ_ANIME			OBJID_MINE,anm_player_go_center
	_OBJ_ANIME_WAIT
#endif	/************************************************************/

	_JUMP				ev_stage_room_start
	_END

ev_stage_start_3_multi:
	/*通路に入ってすぐに移動*/
	//_CALL			ev_stage_comm_command_initialize_1

	/*中央まで歩く*/
	_OBJ_ANIME		OBJID_PLAYER,anm_player_go_center_scroll
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_sio_player_go_center
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_center_multi
	_OBJ_ANIME_WAIT

	_JUMP			ev_stage_room_start
	_END

ev_stage_room_start:
	_WND_SET			10,160,230,192,0,0						/*ウィンドウOFF*/

	/*スポットライト、ストロボを消す*/
	//_PARTICLE_SPA_EXIT	SPAWORK_0

	//_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_0,STAGE_BS_SPOT02		/*広がるスポットライト*/
	//_PARTICLE_WAIT
	_TIME_WAIT		30,FSW_ANSWER

	/*ホワイトアウト*/
	_WHITE_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*多重面を表示*/
	_STAGE_TOOL			FS_ID_EFF_FRAME_ON,0,0,FSW_ANSWER

	/*広がるスポットライトを消す*/
	_PARTICLE_SPA_EXIT	SPAWORK_0

	//_SE_STOP		SEQ_SE_PL_HAND

	/*ホワイトイン*/
	_WHITE_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*ポケモン削除*/
	_TEMOTI_POKE_ACT_DEL	POKE_DISP_MANAGER_ID_0

	/*今何人目か取得*/
	_STAGE_TOOL		FS_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_renshou_0			/*0		*/

	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL			FSW_LOCAL3,1
	_JUMP			ev_stage_room_saikai							/*1-7	*/
	_END


/********************************************************************/
/*							連勝数 0								*/
/********************************************************************/
ev_stage_room_renshou_0:
	/*「どのタイプと対戦する？」*/
	_TALKMSG		msg_stage_room_40

	/*タイプ選択画面呼び出し前に、タイプのランクが全て挑戦済みの最大になっていたら調整をかける*/
	_STAGE_TOOL		FS_ID_TYPE_LEVEL_MAX,0,0,FSW_ANSWER

	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_multi_room_go_rental

	_JUMP			ev_stage_room_go_rental_2
	_END

ev_stage_multi_room_go_rental:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_stage_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_BOTH_1
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	1												/*大量データON*/

	_JUMP				ev_stage_multi_room_go_rental_retry
	_END

ev_stage_multi_room_go_rental_retry:

	/*通信：手持ちやりとり*/
	_LDVAL				FSW_LOCAL6,STAGE_COMM_TEMOTI
	_STAGE_SEND_BUF		FSW_LOCAL6,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_stage_multi_room_go_rental_retry	/*送信失敗は、再送信へ*/

	_STAGE_RECV_BUF		FSW_LOCAL6
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_BOTH_0
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	0												/*大量データOFF*/

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_POKETYPE_SEL
	_COMM_RESET		/*画面切り替え前に初期化*/

	_JUMP			ev_stage_room_go_rental_2
	_END

ev_stage_room_go_rental_2:
	/*フェードアウト*/
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_stage_room_type_sel_sync

	/*タイプ選択画面呼び出し*/
	_STAGE_POKE_TYPE_SEL_CALL
	//_SET_MAP_PROC

	/*多重面を表示*/
	_STAGE_TOOL			FS_ID_EFF_FRAME_ON,0,0,FSW_ANSWER

	/*平方根を計算*/
	_STAGE_TOOL			FS_ID_SQRT,0,0,FSW_ANSWER

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*敵トレーナー、敵ポケモンを選出*/
	_STAGE_WORK_ENEMY_SET	FSW_LOCAL3

	_JUMP			ev_stage_room_go
	_END

/*通信マルチ*/
ev_stage_room_type_sel_sync:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_TYPE_SEL_CALL
	_COMM_RESET
	_RET


/********************************************************************/
/*						バトルへ向かう								*/
/********************************************************************/
ev_stage_room_go:

	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_go_2_multi

	/*初回ではない時は右を向くアニメを入れる*/
	_IFVAL_CALL		FSW_PARAM5,NE,0,ev_stage_room_go_right_anm
	_LDVAL			FSW_PARAM5,1

	/*ブレーン登場かチェック*/
	_STAGE_TOOL		FS_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_leader_1st_02
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_stage_leader_2nd_02

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*敵トレーナーが中央まで歩く*/
	_CALL			ev_stage_trainer_set_1
	_OBJ_ANIME		OBJID_ETC,etc_anime_room_in
	_OBJ_ANIME_WAIT

	_TIME_WAIT		15,FSW_ANSWER

	_OBJ_ANIME		OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_STAGE_TOOL	FS_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_STAGE_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE

	/*両者前へアニメ*/
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_ETC,anm_player_go_btl_room_etc
	_OBJ_ANIME_WAIT

	_JUMP				ev_stage_room_go_sub
	_END

ev_stage_room_go_sub:
	_JUMP			ev_stage_battle
	_END

ev_stage_room_go_2_multi:

	/*初回ではない時は右を向くアニメを入れる*/
	_IFVAL_CALL		FSW_PARAM5,NE,0,ev_stage_room_go_right_anm_sio
	_LDVAL			FSW_PARAM5,1

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_INIT_BEFORE
	_COMM_RESET

	_CALL			ev_stage_comm_command_initialize_2

	_JUMP			ev_stage_room_go_2_multi_retry
	_END

ev_stage_room_go_2_multi_retry:
	_LDVAL			FSW_LOCAL6,STAGE_COMM_TR
	_STAGE_SEND_BUF	FSW_LOCAL6,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_go_2_multi_retry	/*送信失敗は、再送信へ*/

	_STAGE_RECV_BUF	FSW_LOCAL6
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_TR_AFTER
	_COMM_RESET

	_JUMP			ev_stage_room_go_3_multi
	_END

ev_stage_room_go_3_multi:
	_LDVAL			FSW_LOCAL6,STAGE_COMM_ENEMY
	_STAGE_SEND_BUF	FSW_LOCAL6,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_go_3_multi	/*送信失敗は、再送信へ*/

	_STAGE_RECV_BUF	FSW_LOCAL6
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_TR_ENEMY

	/*敵トレーナーが中央まで歩く*/
	_CALL			ev_stage_trainer_set_2
	_OBJ_ANIME		OBJID_ETC,etc_anime_room_in_sio
	_OBJ_ANIME		OBJID_ETC2,etc_anime_room_in_multi
	_OBJ_ANIME_WAIT

	_TIME_WAIT		15,FSW_ANSWER

	_OBJ_ANIME		OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_STAGE_TOOL		FS_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_STAGE_APPEAR	0
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	_OBJ_ANIME		OBJID_ETC2,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_STAGE_TOOL		FS_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_STAGE_APPEAR	1
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	/*両者前へアニメ*/
	//_OBJ_ANIME		OBJID_PLAYER,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_btl_room_multi
	_OBJ_ANIME		OBJID_ETC,anm_player_go_btl_room_etc
	_OBJ_ANIME		OBJID_ETC2,anm_player_go_btl_room_etc
	_OBJ_ANIME_WAIT

	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_stage_room_wait

	_JUMP			ev_stage_battle
	_END

/*初回ではない時は右を向くアニメを入れる*/
ev_stage_room_go_right_anm:
	/*主人公前を見るアニメ*/
	_OBJ_ANIME		OBJID_MINE,anm_player_dir_right
	_OBJ_ANIME		OBJID_MAID,anm_maid_go_left
	_OBJ_ANIME_WAIT
	_RET

ev_stage_room_go_right_anm_sio:
	/*主人公前を見るアニメ*/
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_dir_right
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_dir_right
	_OBJ_ANIME		OBJID_MAID,anm_maid_go_left
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*					トレーナーの見た目セット						*/
/********************************************************************/
ev_stage_trainer_set_1:
	_STAGE_TOOL		FS_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_RET

ev_stage_trainer_set_2:
	//_STAGE_TOOL		FS_ID_GET_TR_OBJ_CODE,1,0,FSW_ANSWER
	_STAGE_TOOL		FS_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK			FSW_PARAM2,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor

	//_STAGE_TOOL		FS_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_STAGE_TOOL		FS_ID_GET_TR_OBJ_CODE,1,0,FSW_ANSWER
	_LDWK			FSW_PARAM3,FSW_ANSWER
	_CHAR_RESOURCE_SET	etc_set_resource2
	_ACTOR_SET		etc_set_actor2
	_RET

ev_stage_trainer_set_brain:
	_LDVAL			FSW_PARAM2,BRAINS2
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_RET


/********************************************************************/
//
/********************************************************************/
ev_stage_battle:
	/*フェードアウト*/
	//_BLACK_OUT	SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_multi_battle_2

	/*ブレーン登場かチェック*/
	_STAGE_TOOL		FS_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_leader_1st_33
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_stage_leader_2nd_33

	_ENCOUNT_EFFECT	FR_ENCOUNT_EFF_BOX_D
	_JUMP			ev_stage_battle_2
	_END

ev_stage_leader_1st_33:
ev_stage_leader_2nd_33:
	_RECORD_INC		RECID_FRONTIER_BRAIN
	_BRAIN_ENCOUNT_EFFECT	FRONTIER_NO_STAGE
	_JUMP			ev_stage_battle_2
	_END

/*通信マルチ*/
ev_stage_multi_battle_2:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_BATTLE
	_COMM_RESET

	_TALK_CLOSE
	_ENCOUNT_EFFECT	FR_ENCOUNT_EFF_BOX_D
	_JUMP			ev_stage_battle_2
	_END


/********************************************************************/
//
/********************************************************************/
ev_stage_battle_2:

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	_STAGE_BATTLE_CALL
	_STAGE_CALL_GET_RESULT
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*所持アイテムを元に戻す*/
	_STAGE_TOOL		FS_ID_SET_TEMP_ITEM,0,0,FSW_ANSWER

	/*多重面を表示*/
	_STAGE_TOOL			FS_ID_EFF_FRAME_ON,0,0,FSW_ANSWER

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*戦闘結果で分岐*/
	_STAGE_LOSE_CHECK		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_lose		/*敗北*/
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*勝利*/
	_RECORD_INC		RECID_STAGE_WIN

	/*タイプのレベルを上げる*/
	_STAGE_TOOL		FS_ID_INC_TYPE_LEVEL,0,0,FSW_ANSWER

	/*タイプのレベルを取得*/
	_STAGE_TOOL		FS_ID_GET_TYPE_LEVEL,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,STAGE_TYPE_LEVEL_MAX,ev_stage_record_inc

	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_battle_2_multi

	/*ブレーン登場かチェック*/
	_STAGE_TOOL		FS_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_leader_1st_03
	_IFVAL_JUMP		FSW_ANSWER,EQ,2,ev_stage_leader_2nd_03

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_CALL			se_kami_01							/*紙1*/
	_PARTICLE_WAIT
	_PARTICLE_SPA_EXIT	SPAWORK_0

	_JUMP			ev_stage_battle_2_sub
	_END

/*タイプのレベルが10に勝利したらレコード更新*/
ev_stage_record_inc:
	_RECORD_INC		RECID_STAGE_RANK10_WIN
	_RET
	
ev_stage_battle_2_sub:
	/*敵トレーナー退場*/
	_OBJ_ANIME		OBJID_ETC,etc_anime_go_out
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_PARAM2

	/*主人公一歩下がる*/
	_OBJ_ANIME		OBJID_MAID,anm_player_go_right
	_OBJ_ANIME		OBJID_MINE,anm_player_go_left
	_OBJ_ANIME_WAIT

	_JUMP			ev_stage_battle_3
	_END

ev_stage_battle_2_multi:
	//
	_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_CALL			se_kami_01							/*紙1*/
	_PARTICLE_WAIT
	_PARTICLE_SPA_EXIT	SPAWORK_0

	_CALL			ev_stage_comm_command_initialize_3

	/*敵トレーナー退場*/
	_OBJ_ANIME		OBJID_ETC,etc_anime_go_out_sio
	_OBJ_ANIME		OBJID_ETC2,etc_anime_go_out_multi
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_ACTOR_FREE		OBJID_ETC2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3

	/*主人公一歩下がる*/
	_OBJ_ANIME		OBJID_MAID,anm_player_go_right
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_player_go_left
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_player_go_left
	_OBJ_ANIME_WAIT

	_JUMP			ev_stage_battle_3
	_END

ev_stage_battle_3:
	_JUMP			ev_stage_battle_4
	_END

ev_stage_battle_4:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_stage_room_renshou_17
#endif	//DEBUG_FRONTIER_LOOP

	/*今何人目+1*/
	_STAGE_TOOL		FS_ID_INC_ROUND,1,0,FSW_ANSWER

	/*連勝数+1*/
	_STAGE_TOOL		FS_ID_INC_RENSYOU,0,0,FSW_ANSWER

#ifndef DEBUG_7BTL_OFF	/********************************************/
	/*今10人目でない時は頭に戻る*/
	_STAGE_TOOL	FS_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,NE,STAGE_LAP_ENEMY_MAX,ev_stage_room_renshou_17	/*連勝数が1-10の時*/
#endif	/************************************************************/

	_JUMP			ev_stage_room_18_win
	_END


/********************************************************************/
/*							10連勝した(記録)						*/
/********************************************************************/
ev_stage_room_18_win:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_18_win_bp_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_stage_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_stage_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_18_win_bp
	_JUMP			ev_stage_room_18_win_rec_yes
	_END

ev_stage_room_18_win_bp_sub:
	_TALKMSG_NOSKIP	msg_stage_room_wait
	_JUMP			ev_stage_room_18_win_bp
	_END

/*「はい」*/
ev_stage_room_18_win_rec_yes:
	_CALL			ev_stage_room_rec_win		/*勝利の記録*/
	_JUMP			ev_stage_room_18_win_bp
	_END


/********************************************************************/
/*							10連勝した(BP取得)						*/
/********************************************************************/
ev_stage_room_18_win_bp:
	//10連勝(クリア)パラメータセット
	_STAGE_TOOL	FS_ID_SET_CLEAR,0,0,FSW_ANSWER

	/*10連勝したので終了へ*/
	//_LDVAL			WK_SCENE_STAGE_LOBBY,1				/*10連勝から始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_STAGE_LOBBY,1			/*10連勝から始まるようにしておく*/

	/*「バトルトレード　７せん　とっぱ　おめでとう」*/
	_STAGE_TOOL		FS_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	0,FSW_ANSWER,0,0
	_TALKMSG		msg_stage_room_20

	_PLAYER_NAME	0											/*プレイヤー名セット*/

	/*バトルポイント追加*/
	_STAGE_TOOL		FS_ID_GET_BP_POINT,0,0,FSW_ANSWER
	_NUMBER_NAME	1,FSW_ANSWER								/*数値セット*/
	_BTL_POINT_ADD	FSW_ANSWER

	/*「ＢＰをもらった」*/
	_TALKMSG		msg_stage_lobby_21
	_ME_PLAY		ME_BP_GET
	_ME_WAIT

	/*TV処理(関数内で、シングルの時だけ挑戦しているモンスターナンバーをセットしている)*/
	_TV_TEMP_STAGE_SET	

	_JUMP			ev_stage_room_bp_end
	_END


/********************************************************************/
/*							10連勝した終了							*/
/********************************************************************/
ev_stage_room_bp_end:
	_JUMP			ev_stage_lose_timing
	_END


/********************************************************************/
/*							連勝数 1-10								*/
/********************************************************************/
ev_stage_room_renshou_17:
	/*「ポケモンを　かいふく　しましょう」*/
	_STAGE_TOOL		FS_ID_GET_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA	0,FSW_ANSWER,0,0
	_TALKMSG		msg_stage_room_3
	_ME_PLAY		SEQ_ASA
	_ME_WAIT

	_PC_KAIFUKU
	_JUMP			ev_stage_room_saikai
	_END


/********************************************************************/
/*						中断セーブからの再開						*/
/********************************************************************/
ev_stage_room_saikai:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	_JUMP			ev_stage_room_saikai_02
	_END
	
ev_stage_room_saikai_02:
	/*「次は　○戦目ですよ」*/
	_STAGE_TOOL		FS_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK			FSW_ANSWER,1
	_NUMBER_NAME	0,FSW_ANSWER
	_TALKMSG		msg_stage_room_4
	_JUMP			ev_stage_room_menu_make
	_END


/********************************************************************/
/*							メニュー生成							*/
/********************************************************************/
ev_stage_room_menu_make:

	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_menu_make_comm

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_stage_room_menu_make_04_set

	_JUMP			ev_stage_room_menu_make_03_set
	_END

/*通信の時のメニュー*/
ev_stage_room_menu_make_comm:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_menu_make_01_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_stage_room_menu_make_02_set

	_JUMP			ev_stage_room_menu_make_01_set
	_END

ev_stage_room_menu_make_01_set:
	_LDVAL				FSW_ANSWER,1
	_JUMP				ev_stage_room_menu_make_sub
	_END

ev_stage_room_menu_make_02_set:
	_LDVAL				FSW_ANSWER,2
	_JUMP				ev_stage_room_menu_make_sub
	_END

ev_stage_room_menu_make_03_set:
	_LDVAL				FSW_ANSWER,3
	_JUMP				ev_stage_room_menu_make_sub
	_END

ev_stage_room_menu_make_04_set:
	_LDVAL				FSW_ANSWER,4
	_JUMP				ev_stage_room_menu_make_sub
	_END

/*どのメニュー形式にするか*/
ev_stage_room_menu_make_sub:
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_stage_room_menu_make_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_stage_room_menu_make_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,3,ev_stage_room_menu_make_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,4,ev_stage_room_menu_make_04
	_END

ev_stage_room_menu_make_01:
	_BMPLIST_INIT_EX	24,13,0,0,FSW_PARAM0			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_JUMP			ev_stage_room_yasumu_next
	_END

ev_stage_room_menu_make_02:
	_BMPLIST_INIT_EX	23,11,0,0,FSW_PARAM0			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1		/*きろくする*/
	_JUMP			ev_stage_room_yasumu_next
	_END

ev_stage_room_menu_make_03:
	_BMPLIST_INIT_EX	24,11,0,0,FSW_PARAM0			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2		/*やすむ*/
	_JUMP			ev_stage_room_yasumu_next
	_END

ev_stage_room_menu_make_04:
	_BMPLIST_INIT_EX	23,9,0,0,FSW_PARAM0				/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0		/*つづける*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1		/*きろくする*/
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2		/*やすむ*/
	_JUMP			ev_stage_room_yasumu_next
	_END

ev_stage_room_yasumu_next:
	_BMPLIST_MAKE_LIST	msg_stage_room_choice_04,FSEV_WIN_TALK_MSG_NONE,3		/*リタイア*/
	_BMPLIST_START
	_JUMP			ev_stage_room_menu_sel
	_END


/********************************************************************/
/*							メニュー結果							*/
/********************************************************************/
ev_stage_room_menu_sel:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_room_choice_01
#endif	//DEBUG_FRONTIER_LOOP

	_IFVAL_JUMP		FSW_PARAM0,EQ,0,ev_room_choice_01	/*つづける*/
	_IFVAL_JUMP		FSW_PARAM0,EQ,1,ev_room_choice_02	/*きろくする*/
	_IFVAL_JUMP		FSW_PARAM0,EQ,2,ev_room_choice_03	/*やすむ*/
	_IFVAL_JUMP		FSW_PARAM0,EQ,3,ev_room_choice_04	/*リタイア*/
	_JUMP			ev_room_choice_04
	_END


/********************************************************************/
/*						「つづける」								*/
/********************************************************************/
ev_room_choice_01:
	/*通信マルチ*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_room_choice_01_multi
	_JUMP			ev_room_choice_01_2
	_END

/*通信マルチ*/
ev_room_choice_01_multi:
	/*パートナー名を表示*/
	_PAIR_NAME		0
	_TALKMSG		msg_stage_room_10
	_JUMP			ev_room_choice_01_multi_retry
	_END

ev_room_choice_01_multi_retry:
	_LDVAL			FSW_LOCAL6,STAGE_COMM_RETIRE
	_STAGE_SEND_BUF	FSW_LOCAL6,0,FSW_ANSWER					/*0=つづける*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_room_choice_01_multi_retry	/*送信失敗は、再送信へ*/

	_STAGE_RECV_BUF	FSW_LOCAL6
	_COMM_RESET

	/*相手が選んだメニューを取得*/
	_STAGE_TOOL		FS_ID_GET_RETIRE_FLAG,0,0,FSW_ANSWER

	/*パートナーがリタイアしたら*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_giveup_yes_multi	/*リタイア*/

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_MENU
	_COMM_RESET

	_JUMP			ev_room_choice_01_2
	_END

/*パートナーがリタイアしたら*/
ev_stage_room_giveup_yes_multi:
	/*パートナー名を表示*/
	_PAIR_NAME		0
	_TALKMSG_NOSKIP		msg_stage_room_12
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER

	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_stage_room_wait
	_JUMP			ev_stage_room_giveup_yes_multi_sync
	_END

ev_room_choice_01_2:
	/*ワークをクリアしておく*/
	_LDVAL			FSW_LOCAL3,0

	_JUMP			ev_stage_room_trade_yes_pair
	_END

ev_stage_room_trade_yes_pair:
	_JUMP			ev_stage_room_renshou_0
	_END


/********************************************************************/
/*							記録する*/
/********************************************************************/
ev_room_choice_02:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_stage_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_stage_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_rec_no
	_JUMP			ev_stage_room_rec_yes
	_END

/*「はい」*/
ev_stage_room_rec_yes:
	_CALL			ev_stage_room_rec_win		/*勝利の記録*/
	_JUMP			ev_stage_room_saikai		/*メニューへ戻る*/
	_END

/*「いいえ」*/
ev_stage_room_rec_no:
	_JUMP			ev_stage_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_stage_room_rec_win:
	_CALL				ev_stage_room_rec_common		/*ANSWER,LOCAL5使用中*/
	//_ADD_WK			FSW_LOCAL5,1
	//_SUB_WK			FSW_LOCAL5,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_stage_room_rec
	_END

/*負け*/
ev_stage_room_rec_lose:
	_CALL				ev_stage_room_rec_common		/*ANSWER,LOCAL5使用中*/
	_ADD_WK				FSW_LOCAL5,1
	_JUMP				ev_stage_room_rec
	_END

/*共通部分*/
ev_stage_room_rec_common:
	/*録画データセーブ*/
	_STAGE_TOOL			FS_ID_GET_RENSYOU,0,0,FSW_LOCAL5
	_RET

ev_stage_room_rec:
	_JUMP				ev_stage_room_rec_sub
	_END

ev_stage_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL3,1

	_TALKMSG_ALL_PUT	msg_stage_room_6_4
	_STAGE_TOOL			FS_ID_GET_TYPE,0,0,FSW_ANSWER
	_ADD_WAITICON
	_BATTLE_REC_SAVE	FRONTIER_NO_STAGE,FSW_ANSWER,FSW_LOCAL5,FSW_LOCAL5
	_DEL_WAITICON

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_stage_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_stage_room_6_2
	_RET

ev_stage_room_rec_true:
	/*「記録されました」*/
	_SE_PLAY			SEQ_SE_DP_SAVE
	_PLAYER_NAME		0
	_TALKMSG			msg_stage_room_6_1
	_RET
	
/*録画データがない時メッセージ*/
ev_stage_room_rec_msg1:
	_TALKMSG			msg_stage_room_6
	_RET

/*すでに録画データがある時メッセージ*/
ev_stage_room_rec_msg2:
	_TALKMSG			msg_stage_room_6_3
	_RET


/********************************************************************/
/*							やすむ*/
/********************************************************************/
ev_room_choice_03:
	/*「レポートをかいて終了しますか？」*/
	_TALKMSG		msg_stage_room_7
	_YES_NO_WIN		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_rest_no
	_JUMP			ev_stage_room_rest_yes
	_END

/*「はい」*/
ev_stage_room_rest_yes:
	//_LDVAL			WK_SCENE_STAGE_LOBBY,2			/*続きから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_STAGE_LOBBY,2		/*続きから始まるようにしておく*/

	//プレイデータセーブ
	_STAGE_TOOL	FS_ID_SAVE_REST_PLAY_DATA,0,0,FSW_ANSWER

	//_CALL			ev_stage_save			/*07.08.24 _CALLに置き換えた*/
	_CALL			ev_stage_save_rest

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_STAGE_WORK_FREE

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	//リセットコマンド
	_STAGE_TOOL	FS_ID_SYSTEM_RESET,0,0,FSW_ANSWER

	_END

/*「いいえ」*/
ev_stage_room_rest_no:
	_JUMP			ev_stage_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							リタイア*/
/********************************************************************/
ev_room_choice_04:
	/*「バトルトレードの挑戦を中止する？」*/
	_TALKMSG		msg_stage_room_8
	_YES_NO_WIN_EX	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_giveup_no

	/*通信*/
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_room_giveup_yes_multi_wait_msg

	_JUMP			ev_stage_room_giveup_yes
	_END

/*「少々お待ちください」*/
ev_stage_room_giveup_yes_multi_wait_msg:
	_TALKMSG_NOSKIP	msg_stage_room_wait
	_JUMP			ev_stage_room_giveup_yes_multi_retry
	_END

ev_stage_room_giveup_yes_multi_retry:
	_LDVAL			FSW_LOCAL6,STAGE_COMM_RETIRE
	_STAGE_SEND_BUF	FSW_LOCAL6,1,FSW_ANSWER						/*1=リタイヤ*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_room_giveup_yes_multi_retry	/*送信失敗は、再送信へ*/

	//_STAGE_RECV_BUF	FSW_LOCAL6
	//_COMM_RESET
	_JUMP			ev_stage_room_giveup_yes_multi_sync
	_END

ev_stage_room_giveup_yes_multi_sync:

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_GIVE
	//_COMM_SYNCHRONIZE	DBC_TIM_STAGE_MENU
	_COMM_RESET
	_JUMP			ev_stage_room_giveup_yes
	_END

/*「はい」*/
ev_stage_room_giveup_yes:
	//敗戦パラメータセット
	_STAGE_TOOL		FS_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_STAGE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_STAGE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP			ev_stage_room_end_save
	_END

/*「いいえ」*/
ev_stage_room_giveup_no:
	_JUMP			ev_stage_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*								敗北*/
/********************************************************************/
ev_stage_lose:
	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_stage_lose_timing_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_stage_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_stage_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER							/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_lose_rec_no/*「いいえ」*/
	_JUMP			ev_stage_lose_rec_yes				/*「はい」*/
	_END

ev_stage_lose_timing_sub:
	_TALKMSG_NOSKIP	msg_stage_room_wait
	_JUMP			ev_stage_lose_timing_call
	_END

/*「はい」*/
ev_stage_lose_rec_yes:
	_CALL			ev_stage_room_rec_lose		/*敗北の記録*/
	_JUMP			ev_stage_lose_timing_call
	_END

/*「いいえ」*/
ev_stage_lose_rec_no:
	_JUMP			ev_stage_lose_timing_call
	_END

ev_stage_lose_timing_call:
	//敗戦パラメータセット
	_STAGE_TOOL		FS_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_STAGE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_STAGE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP			ev_stage_lose_timing
	_END

ev_stage_lose_timing:
	_STAGE_TOOL		FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_stage_multi_lose_timing
	_JUMP			ev_stage_room_end_save
	_END

/*通信同期*/
ev_stage_multi_lose_timing:
	/*「少々お待ちください」*/
	_TALKMSG_NOSKIP	msg_stage_room_wait

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_LOSE_END
	_COMM_RESET
	_JUMP				ev_stage_room_end_save
	_END

ev_stage_room_end_save:
	_CALL				ev_stage_save			/*07.08.24 _CALLに置き換えた*/
	_TALK_CLOSE
	_JUMP				ev_stage_room_end
	_END


/********************************************************************/
/*							共通終了								*/
/********************************************************************/
ev_stage_room_end:

	/*セーブ後に同期させる*/
	_STAGE_TOOL			FS_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_stage_multi_end_timing

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_STAGE_TOOL		FS_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,STAGE_TYPE_MULTI,ev_stage_room_multi_tv

	_STAGE_TOOL		FS_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,STAGE_TYPE_WIFI_MULTI,ev_stage_room_end_wifi

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_STAGE_WORK_FREE

	/*2Dマップ控え室へ*/
	//

	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*マルチのみ*/
ev_stage_room_multi_tv:
	/*TV処理*/
	_TV_TEMP_FRIEND_SET	FRONTIER_NO_STAGE
	_RET


/********************************************************************/
/*						セーブ後に同期させる						*/	
/********************************************************************/
ev_stage_multi_end_timing:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_SAVE_AFTER
	_COMM_RESET			/*画面切り替え前に初期化*/
	_RET


/********************************************************************/
/*						WIFIは待ち合わせ画面へ戻る					*/	
/********************************************************************/
ev_stage_room_end_wifi:
	/*フェードアウト*/
	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_STAGE_WORK_FREE

	_MAP_CHANGE_EX		FSS_SCENEID_WIFI_COUNTER,1
	_END


/********************************************************************/
//							外部セーブなし(休む)
/********************************************************************/
ev_stage_save_rest:
	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_stage_lobby_24

	_ADD_WAITICON

	_CALL			ev_stage_save_sub

	_DEL_WAITICON
	_SE_PLAY		SEQ_SE_DP_SAVE
	_SE_WAIT		SEQ_SE_DP_SAVE
	_RET


/********************************************************************/
//							共通セーブ	
/********************************************************************/
ev_stage_save:
	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_stage_lobby_24

	_ADD_WAITICON

	/*バトルステージの外部連勝記録を書き込み*/
	/*セーブ失敗の分岐はいらない(08.02.26 松田さんより)*/
	_STAGE_RENSHOU_COPY_EXTRA	FSW_ANSWER

	/*(WIFI_MULTI以外のタイプ)外部セーブ＋通常セーブが実行されなかった時と、*/
	/*レコード的に外部セーブの必要がなかった時は、通常セーブをかける*/
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_stage_save_sub

	_DEL_WAITICON
	_SE_PLAY		SEQ_SE_DP_SAVE
	_SE_WAIT		SEQ_SE_DP_SAVE
	_RET

/*(WIFI_MULTI以外のタイプ)外部セーブ＋通常セーブが実行されなかった時と、*/
/*レコード的に外部セーブの必要がなかった時は、通常セーブをかける*/
ev_stage_save_sub:
	_REPORT_DIV_SAVE	FSW_ANSWER
	_RET

#if 0
	_ACTOR_FREE		OBJID_PCWOMAN2
	_ACTOR_FREE		OBJID_MINE
	/*観客削除*/
	_ACTOR_FREE		OBJID_SHOPM1
	_ACTOR_FREE		OBJID_SHOPW1
#endif


/********************************************************************/
/*							ストロボ演出							*/
/********************************************************************/
#define STAGE_PASYA_01_NUM	(2)
#define STAGE_PASYA_02_NUM	(5)
#define STAGE_PASYA_03_NUM	(10)

call_se_pasya_01:
	_SE_PLAY			SEQ_SE_DP_DENDOU				/*歓声*/
	//_SE_PLAY			SEQ_SE_PL_HAND					/*BGMでも鳴っているのでいらない*/
	//_SE_PLAY			SEQ_SE_PL_KATI
	_STAGE_TOOL			FS_ID_GET_LAP,0,0,FSW_ANSWER						/*周回数取得*/
	_IFVAL_JUMP			FSW_ANSWER,LT,STAGE_PASYA_01_NUM,se_pasya_01		/*ストロボ小*/
	_IFVAL_JUMP			FSW_ANSWER,LT,STAGE_PASYA_02_NUM,se_pasya_02		/*ストロボ中*/
	_JUMP				se_pasya_03											/*ストロボ大*/
	_END

se_pasya_01:
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYA_01
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYA_02
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA01
	_RET

se_pasya_02:
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA02
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA02_BG
	_RET

se_pasya_03:
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03BG
	_RET


/********************************************************************/
/*						紙ふぶき(小)演出							*/
/********************************************************************/
se_kami_01:
	/*ラウンド数を取得*/
	_STAGE_TOOL		FS_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK			FSW_ANSWER,1
	_IFVAL_JUMP		FSW_ANSWER,EQ,STAGE_LAP_ENEMY_MAX,se_kami_02	/*連勝数が10の時*/

	_SE_PLAY				SEQ_SE_DP_UG_022
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_01_L
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_01_R
	_SE_WAIT				SEQ_SE_DP_UG_022
	_SE_PLAY				SEQ_SE_DP_DENDOU
	_RET


/********************************************************************/
/*						紙ふぶき(大)演出							*/
/********************************************************************/
se_kami_02:
	_SE_PLAY				SEQ_SE_DP_UG_022
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_L1
	//_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_L2
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_R1
	//_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_R2
	_SE_WAIT				SEQ_SE_DP_UG_022
	_SE_PLAY				SEQ_SE_DP_DENDOU
	_RET


/********************************************************************/
/*						紙ふぶき+銀テープ演出						*/
/********************************************************************/
se_kami_03:
	_SE_PLAY				SEQ_SE_DP_UG_022
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_L1
	//_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_L2
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_R1
	//_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMI_02_R2
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMITAPE_L
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMITAPE_R
	_SE_WAIT				SEQ_SE_DP_UG_022
	_SE_PLAY				SEQ_SE_DP_DENDOU
	_RET


/********************************************************************/
/*						ブレーンの流れ								*/
/********************************************************************/
ev_stage_leader_1st_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*登場*/
	_CALL				ev_stage_leader_appear

	/*戦闘前のメッセージ(1周目)*/
	_TALKMSG			msg_stage_room_boss_02
	_TALK_CLOSE

	/*元の位置に戻る*/
	_CALL			ev_stage_leader_msg_after

	_JUMP				ev_stage_room_go_sub
	_END

ev_stage_leader_2nd_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*登場*/
	_CALL				ev_stage_leader_appear

	/*戦闘前のメッセージ(2周目)*/
	_TALKMSG			msg_stage_room_boss_03
	_TALK_CLOSE

	/*元の位置に戻る*/
	_CALL			ev_stage_leader_msg_after

	_JUMP				ev_stage_room_go_sub
	_END

/*元の位置に戻る*/
ev_stage_leader_msg_after:
	_OBJ_ANIME		OBJID_ETC,anm_player_go_btl_room_101
	_OBJ_ANIME_WAIT

	_TIME_WAIT			30,FSW_ANSWER

	/*両者前へアニメ*/
	_OBJ_ANIME		OBJID_MINE,anm_player_go_btl_room
	_OBJ_ANIME		OBJID_ETC,anm_player_go_btl_room_etc
	_OBJ_ANIME_WAIT
	_RET

/*登場*/
ev_stage_leader_appear:

	_CALL				ev_stage_trainer_set_brain

	/*多重面を非表示*/
	_STAGE_TOOL			FS_ID_EFF_FRAME_ON,1,0,FSW_ANSWER

	/*ポケモン表示*/
	_TEMOTI_POKE_ACT_ADD	FSW_LOCAL2,STAGE_POKE_ACT_X,STAGE_POKE_ACT_Y,POKE_DISP_MANAGER_ID_0, 0, 0x0000

	_WND_SET			10,160,230,192,0,0						/*ウィンドウOFF*/

	_TIME_WAIT			10,FSW_ANSWER

	_WND_SET			10,160,230,192,54,1						/*ウィンドウON*/

	/*入り口まで戻す*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_back
	_OBJ_ANIME_WAIT

	/*パーティクル演出*/
	_PARTICLE_SPA_LOAD		SPAWORK_0,STAGE_SPA, FSS_CAMERA_ORTHO

	/*左右動きスポットライト*/
	_PARTICLE_ADD_EMITTER	SPAWORK_0,STAGE_BS_SPOT00
	_PARTICLE_ADD_EMITTER	SPAWORK_0,STAGE_BS_SPOT00_BG	/*スポットライト*/
	//_PARTICLE_WAIT
	//_TIME_WAIT			60,FSW_ANSWER
	_TIME_WAIT			90,FSW_ANSWER

	_PARTICLE_ADD_EMITTER	SPAWORK_0,STAGE_BS_SPOT01		/*スポットライト*/

	/*ブレーン登場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_3
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_DENDOU					/*歓声*/
	//_TIME_WAIT			30,FSW_ANSWER
	_SE_WAIT			SEQ_SE_DP_DENDOU

	/*ブレーンが中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_2
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_2
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_DENDOU					/*歓声*/
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03BG
	_SE_WAIT			SEQ_SE_DP_DENDOU

	_WND_SET			10,160,230,192,10,0					/*ウィンドウOFF*/

	/*ブレーンが中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_3
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_2_2
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_DENDOU					/*歓声*/
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYAPASYA03BG
	_SE_WAIT			SEQ_SE_DP_DENDOU

	/*ブレーンが中央まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_4
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_2_3
	_OBJ_ANIME_WAIT

	/*ポケモン削除*/
	_TEMOTI_POKE_ACT_DEL	POKE_DISP_MANAGER_ID_0

	/*スポットライトを消す*/
	//_PARTICLE_SPA_EXIT	SPAWORK_0

	//_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_0,STAGE_BS_SPOT02		/*広がるスポットライト*/
	//_PARTICLE_WAIT
	_TIME_WAIT			30,FSW_ANSWER

	/*ホワイトアウト*/
	_WHITE_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*多重面を表示*/
	_STAGE_TOOL			FS_ID_EFF_FRAME_ON,0,0,FSW_ANSWER

	/*スポットライト、ストロボを消す*/
	_PARTICLE_SPA_EXIT	SPAWORK_0

	/*ホワイトイン*/
	_WHITE_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*主人公の前に来る*/
	_OBJ_ANIME		OBJID_ETC,anm_player_go_btl_room_100
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						ブレーンに勝利後の流れ						*/
/********************************************************************/
ev_stage_leader_1st_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

	/*ブレーンを倒した時*/
	_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_CALL				se_kami_03						/*紙テープ*/
	_PARTICLE_WAIT
	_PARTICLE_SPA_EXIT	SPAWORK_0

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_STAGE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_NONE,ev_stage_meory_print_put_1st

	/*戦闘後のメッセージ(1周目)*/
	_TALKMSG			msg_stage_room_boss_04
	_TALK_CLOSE
	_JUMP				ev_stage_battle_2_sub
	_END

/*受付で1st記念プリントを貰えるようにする*/
ev_stage_meory_print_put_1st:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_STAGE,MEMORY_PRINT_PUT_1ST
	_RET

ev_stage_leader_2nd_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL			ev_stage_rensyou_get
	_NUMBER_NAME	0,FSW_ANSWER

#if 1
	/*ブレーンを倒した時*/
	_PARTICLE_SPA_LOAD	SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_CALL				se_kami_03						/*紙テープ*/
	_PARTICLE_WAIT
	_PARTICLE_SPA_EXIT	SPAWORK_0
#endif

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_STAGE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_PUT_OK_1ST,ev_stage_meory_print_put_2nd

	/*戦闘後のメッセージ(2周目)*/
	_TALKMSG			msg_stage_room_boss_05
	_TALK_CLOSE
	_JUMP				ev_stage_battle_2_sub
	_END

/*受付で2nd記念プリントを貰えるようにする*/
ev_stage_meory_print_put_2nd:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_STAGE,MEMORY_PRINT_PUT_2ND
	_RET


/********************************************************************/
/*					連勝数を取得(FSW_ANSWER使用)					*/
/********************************************************************/
ev_stage_rensyou_get:
	/*連勝数取得*/
	_STAGE_TOOL			FS_ID_GET_RENSYOU,0,0,FSW_ANSWER

	/*すでに9999の時は、0 オリジンの補正をかけない*/
	_IFVAL_JUMP			FSW_ANSWER,GE,STAGE_RENSYOU_MAX,ev_stage_rensyou_ret

	_ADD_WK				FSW_ANSWER,1
	_RET

ev_stage_rensyou_ret:
	_RET


/********************************************************************/
/*																	*/
/********************************************************************/
ev_stage_comm_command_initialize_1:
	_TIME_WAIT			1,FSW_ANSWER
	_STAGE_TOOL			FS_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_INIT_1
	_COMM_RESET
	_RET

ev_stage_comm_command_initialize_2:
	_TIME_WAIT			1,FSW_ANSWER
	_STAGE_TOOL			FS_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_INIT_2
	_COMM_RESET
	_RET

ev_stage_comm_command_initialize_3:
	_TIME_WAIT			1,FSW_ANSWER
	_STAGE_TOOL			FS_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_STAGE_INIT_3
	_COMM_RESET
	_RET


