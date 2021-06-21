//==============================================================================
/**
 * @file	tower3.s
 * @brief	「バトルタワーシングル通路」フロンティアスクリプト(d31r0203,sp_d31r0203)
 * @author	nohara
 * @date	2007.05.30
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/b_tower_scr_def.h"


//--------------------------------------------------------------------
//
//					     スクリプト本体
//
//	FSW_LOCAL1		WK_SCENE_D31R0203を格納
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_tower_single_way_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了


//--------------------------------------------------------------------
//							座標定義
//--------------------------------------------------------------------
#define PLAYER_X		(8*7)
#define PLAYER_Y		(8*4)
#define PCWOMAN2_X		(8*7)
#define PCWOMAN2_Y		(8*4)

/********************************************************************/
//
//		SCENE_TOWER:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	single_way_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA			FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_TOWER:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_PCWOMAN2				(1)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

_ACTOR_LABEL	single_way_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,ON				/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							PCWOMAN2_X,PCWOMAN2_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


//====================================================================
//	配置物アクター
//====================================================================
//--------------------------------------------------------------
//	配置物リソースラベル
//--------------------------------------------------------------
_ARTICLE_RESOURCE_LABEL	article_door_resource
	_ARTICLE_RESOURCE_DATA	ACTID_TOWER_DOOR
	_ARTICLE_RESOURCE_DATA_END

//--------------------------------------------------------------
//	配置物アクター
//--------------------------------------------------------------
_ARTICLE_ACTOR_LABEL	article_door_actor
	//id,x,y,visible,anm_start,act_work
	_ARTICLE_ACTOR_DATA		ACTID_TOWER_DOOR, 72, 48, ON, OFF, ACTWORK_0
	_ARTICLE_ACTOR_DATA_END


/********************************************************************/
/*							アニメ									*/
/********************************************************************/

//--------------------------------------------------------------------
//受け付け嬢　In
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_pcwoman2_in
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機　In
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_player_in01
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA_END

//--------------------------------------------------------------------
//共有　Move01
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_pcwoman2_move01
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0203_player_move01
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//共有　Move02
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_pcwoman2_move02
	_ANIME_DATA	FC_WALK_R_8F,8
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0203_player_move02
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,8
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//共有　Move03
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_pcwoman2_move03
	_ANIME_DATA	FC_WALK_R_8F,12
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0203_player_move03
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,12
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//共有　Move04
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0203_pcwoman2_move04
	_ANIME_DATA	FC_WALK_R_8F,16
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0203_player_move04
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,16
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END


/********************************************************************/
/*							開始									*/
/********************************************************************/
fss_tower_single_way_start:

	/*右側のWIFIゲート*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI,comm_tower_6_char_set_wifi
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_RETRY,comm_tower_6_char_set_wifi
	_JUMP				comm_tower_6_char_set_single
	_END

fss_tower_single_way_start_sub:
	_CHAR_RESOURCE_SET	single_way_set_resource					/*キャラクタリソース登録*/
	_ACTOR_SET			single_way_set_actor					/*アクター登録*/

	/*リソース＆アクター登録*/
	_ARTICLE_RESOURCE_SET	article_door_resource
	_ARTICLE_ACTOR_SET		article_door_actor
	
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0203,FSW_LOCAL1

	//マップ切り替え前に主人公をバニッシュしておく

	/********************/
	//SCENE_CHANGE_LABEL
	/********************/
	_IFVAL_JUMP			FSW_LOCAL1,EQ,1,ev_tower_single_way		/*シングル通路*/
	_END

comm_tower_6_char_set_wifi:
	_LDVAL				FSW_PARAM6,WIFISW
	_JUMP				fss_tower_single_way_start_sub
	_END

comm_tower_6_char_set_single:
	_LDVAL				FSW_PARAM6,BFSM
	_JUMP				fss_tower_single_way_start_sub
	_END


/********************************************************************/
/*						シングル通路								*/
/********************************************************************/
ev_tower_single_way:
	_CALL				ev_tower_ways_comanm_in

	/*どのゲートに入るかランダム*/
	_GET_RND			FSW_ANSWER,4

	_SWITCH				FSW_ANSWER
	_CASE_JUMP			1,ev_tower_ways_comanm_move02
	_CASE_JUMP			2,ev_tower_ways_comanm_move03
	_CASE_JUMP			3,ev_tower_ways_comanm_move04
	_JUMP				ev_tower_ways_comanm_move01
	_END


/********************************************************************/
/*						シングル通路アニメ							*/
/********************************************************************/
ev_tower_ways_comanm_in:
#if 0
	/*エレベータドア*/
	_SET_UP_DOOR_ANIME	0,0,2,2,77
	_SEQ_OPEN_DOOR		77
	_WAIT_3D_ANIME		77
#endif

	/*アニメ動作テスト*/
	_SE_PLAY			SEQ_SE_DP_DOOR10
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_0, 0	//0番のアニメ実行
	_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_0		//アニメ終了待ち
	_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_0		//アニメ停止

	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0203_pcwoman2_in
	_OBJ_ANIME_WAIT

	//_OBJ_VISIBLE		OBJID_PLAYER
	_OBJ_ANIME			OBJID_PLAYER,anm_d31r0203_player_in01
	_OBJ_ANIME_WAIT

#if 0
	_SEQ_CLOSE_DOOR		77	/*ドア閉める*/
	_WAIT_3D_ANIME		77	/*アニメ終了待ち*/
	_FREE_3D_ANIME		77	/*ドアアニメ解放*/
#endif
	_RET


/********************************************************************/
/*					(ランダム)ゲートに入る							*/
/********************************************************************/
ev_tower_ways_comanm_move01:
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0203_pcwoman2_move01
	_OBJ_ANIME			OBJID_PLAYER,anm_d31r0203_player_move01
	_OBJ_ANIME_WAIT
	_JUMP				ev_tower_ways_common_out
	_END

ev_tower_ways_comanm_move02:
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0203_pcwoman2_move02
	_OBJ_ANIME			OBJID_PLAYER,anm_d31r0203_player_move02
	_OBJ_ANIME_WAIT
	_JUMP				ev_tower_ways_common_out
	_END

ev_tower_ways_comanm_move03:
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0203_pcwoman2_move03
	_OBJ_ANIME			OBJID_PLAYER,anm_d31r0203_player_move03
	_OBJ_ANIME_WAIT
	_JUMP				ev_tower_ways_common_out
	_END

ev_tower_ways_comanm_move04:
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0203_pcwoman2_move04
	_OBJ_ANIME			OBJID_PLAYER,anm_d31r0203_player_move04
	_OBJ_ANIME_WAIT
	_JUMP				ev_tower_ways_common_out
	_END


/********************************************************************/
/*					シングルバトルルームへ							*/
/********************************************************************/
ev_tower_ways_common_out:
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*次々とアクター削除*/
	_ARTICLE_ACTOR_FREE		ACTWORK_0
	
	/*リソース解放(アクターはワーク番号、リソースはACTIDで解放する事に注意)*/
	_ARTICLE_RESOURCE_FREE	ACTID_TOWER_DOOR

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6

	//_MAP_CHANGE		ZONE_ID_D31R0205,0,SB_ROOMS_PLAYER_SX,SB_ROOMS_PLAYER_SY,DIR_UP
	//_MAP_CHANGE		FSS_SCENEID_TOWER_SINGLE_BTL
	//_JUMP				fss_tower_single_btl_start
	_MAP_CHANGE_EX		FSS_SCENEID_TOWER_SINGLE_BTL,0
	_END


