//==============================================================================
/**
 * @file	tower.s
 * @brief	「バトルタワー」フロンティアスクリプト(タワーマルチ通路)
 * @author	nohara
 * @date	2007.05.29
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
//	FSW_LOCAL1		WK_SCENE_D31R0204を格納
//	FSW_LOCAL2		アニメさせる左側のOBJID
//	FSW_LOCAL3		アニメさせる右側のOBJID
//	FSW_PARAM2		主人公の見た目格納(AIマルチの自分)
//	FSW_PARAM3		パートナーの見た目格納(AIマルチの5人集)
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_tower_wayd_common	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了


//--------------------------------------------------------------------
//							座標定義
//--------------------------------------------------------------------
#define PLAYER_X		(8*17)
#define PLAYER_Y		(8*11)
#define PCWOMAN2_01_X	(8*7)
#define PCWOMAN2_01_Y	(8*4)
#define PCWOMAN2_02_X	(8*29)
#define PCWOMAN2_02_Y	(PCWOMAN2_01_Y)
#define MINE_X			(PCWOMAN2_01_X)
#define MINE_Y			(8*4)
#define PAIR_X			(PCWOMAN2_02_X)
#define PAIR_Y			(MINE_Y)
#define SIO_USER_0_X	(PCWOMAN2_01_X)
#define SIO_USER_0_Y	(8*4)
#define SIO_USER_1_X	(PCWOMAN2_02_X)
#define SIO_USER_1_Y	(SIO_USER_0_Y)


/********************************************************************/
//
//		SCENE_TOWER:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
//AIマルチ
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		FSW_PARAM2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		FSW_PARAM3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//通信マルチ
_RESOURCE_LABEL	comm_multi_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END


//====================================================================
//	SCENE_TOWER:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_PCWOMAN2_01			(1)
#define OBJID_PCWOMAN2_02			(2)
#define OBJID_MINE					(3)		//AIマルチの自分(主人公は画面中心で非表示)
#define OBJID_PAIR					(4)		//AIマルチのパートナー
#define OBJID_SIO_USER_0			(5)
#define OBJID_SIO_USER_1			(6)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

//AIマルチ
_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2_01,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							PCWOMAN2_01_X,PCWOMAN2_01_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PCWOMAN2_02,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							PCWOMAN2_02_X,PCWOMAN2_02_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							MINE_X,MINE_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PAIR,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							PAIR_X,PAIR_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//通信マルチ
_ACTOR_LABEL	comm_multi_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2_01,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							PCWOMAN2_01_X,PCWOMAN2_01_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PCWOMAN2_02,FSW_PARAM6,WF2DMAP_WAY_C_DOWN, \
							PCWOMAN2_02_X,PCWOMAN2_02_Y,OFF,EVENTID_TEST_SCR_OBJ1
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_DOWN, \
							SIO_USER_0_X,SIO_USER_0_Y,OFF
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_DOWN, \
							SIO_USER_1_X,SIO_USER_1_Y,OFF
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
	_ARTICLE_ACTOR_DATA		ACTID_TOWER_DOOR, 216, 48, ON, OFF, ACTWORK_1
	_ARTICLE_ACTOR_DATA_END


/********************************************************************/
/*							アニメ									*/
/********************************************************************/

//--------------------------------------------------------------------
//トレーナー　In
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0204_pcwoman2_in
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0204_trainer_in
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA_END

//--------------------------------------------------------------------
//トレーナー01　walk
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0204_trainer_right
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0204_trainer_left
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_L_8F,5
	_ANIME_DATA_END

//--------------------------------------------------------------------
//トレーナー out　
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0204_trainer_out
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//受け付け嬢　walk
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0204_pcwoman2_right
	_ANIME_DATA	FC_WALK_R_8F,5
	_ANIME_DATA FC_WALK_U_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0204_pcwoman2_left
	_ANIME_DATA	FC_WALK_L_8F,5
	_ANIME_DATA FC_WALK_U_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA_END
	
_ANIME_LABEL	anm_d31r0204_pcwoman2_out
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1
	_ANIME_DATA_END


/********************************************************************/
/*								開始								*/
/********************************************************************/
fss_tower_wayd_common:
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0204,FSW_LOCAL1

	_CALL				ev_d31r0204_flag_change
	_CALL				ev_d31r0204_obj_change

	/*リソース＆アクター登録*/
	_LDVAL				FSW_PARAM6,BFSW1
	_SE_PLAY			SEQ_SE_DP_DOOR10
	_ARTICLE_RESOURCE_SET	article_door_resource
	_ARTICLE_ACTOR_SET		article_door_actor
	
	/*右側はHフリップさせる*/
	_ARTICLE_ACTOR_FLIP_H_SET	ACTWORK_1

	/*プレイモード取得*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER

	/*通信マルチ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,tower_wayd_comm_multi
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,tower_wayd_comm_multi

	/*AIマルチ*/
	_LDVAL				FSW_LOCAL2,OBJID_MINE
	_LDVAL				FSW_LOCAL3,OBJID_PAIR
	_CHAR_RESOURCE_SET	default_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor					/*アクター登録*/
	_JUMP				tower_wayd_common2
	_END

tower_wayd_comm_multi:
	_LDVAL				FSW_LOCAL2,OBJID_SIO_USER_0
	_LDVAL				FSW_LOCAL3,OBJID_SIO_USER_1
	_CHAR_RESOURCE_SET	comm_multi_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			comm_multi_set_actor				/*アクター登録*/
	_JUMP				tower_wayd_common2
	_END

tower_wayd_common2:
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_CALL				ev_tower_wayd_comanm_in
	_CALL				ev_tower_wayd_comanm_walk
	_CALL				ev_tower_wayd_comanm_out

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*次々とアクター削除*/
	_ARTICLE_ACTOR_FREE		ACTWORK_0
	_ARTICLE_ACTOR_FREE		ACTWORK_1
	
	/*リソース解放(アクターはワーク番号、リソースはACTIDで解放する事に注意)*/
	_ARTICLE_RESOURCE_FREE	ACTID_TOWER_DOOR

	/*プレイモード取得*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER

	/*通信マルチ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,tower_wayd_comm_multi2
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,tower_wayd_comm_multi2

	/*AIマルチ*/
	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2_01
	_ACTOR_FREE			OBJID_PCWOMAN2_02
	_ACTOR_FREE			OBJID_MINE
	_ACTOR_FREE			OBJID_PAIR
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3
	_JUMP				tower_wayd_common3
	_END

tower_wayd_comm_multi2:
	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2_01
	_ACTOR_FREE			OBJID_PCWOMAN2_02
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_JUMP				tower_wayd_common3
	_END

tower_wayd_common3:
	//_MAP_CHANGE		ZONE_ID_D31R0206,0,SB_ROOMD_PLAYER_SX,SB_ROOMD_PLAYER_SY,DIR_UP
	_MAP_CHANGE_EX		FSS_SCENEID_TOWER_MULTI_BTL,0
	_END


/********************************************************************/
/*						FLAG_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0204_flag_change:
	_IFVAL_JUMP			FSW_LOCAL1,EQ,0,ev_d31r0204_flag_change_end

	/*
	  通信の子の時は親子の左右位置をいれかえる
	  カレントID取得関数は非接続時に呼ぶと0が返る
	*/

	//_COMM_GET_CURRENT_ID	FSW_LOCAL0
	_IFVAL_JUMP			FSW_LOCAL0,EQ,1,ev_d31r0204_flag_change_child

	/*非接続時と親のとき*/
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM2
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM3
#if 0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM3
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_LOCAL0
	//_LDWK				OBJCHRWORK0,FSW_LOCAL0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM2
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_LOCAL0
	//_LDWK				OBJCHRWORK1,FSW_LOCAL0
#endif
	_JUMP				ev_d31r0204_flag_change_end
	_END

ev_d31r0204_flag_change_end:
	_RET

/*通信の子の時*/
ev_d31r0204_flag_change_child:
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM2
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM3
#if 0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM2
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_LOCAL0
	//_LDWK				OBJCHRWORK1,FSW_LOCAL0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM3
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_LOCAL0
	//_LDWK				OBJCHRWORK0,FSW_LOCAL0
#endif
	_RET


/********************************************************************/
/*				バトルタワー　マルチ通路アニメコモン				*/
/********************************************************************/
ev_tower_wayd_comanm_in:
#if 0
	//エレベータドアopen
	_SET_UP_DOOR_ANIME	0,0,3,2,77
	_SET_UP_DOOR_ANIME	0,0,14,2,78
	_SEQ_OPEN_DOOR		77
	_SEQ_OPEN_DOOR		78
	_WAIT_3D_ANIME		77
	_WAIT_3D_ANIME		78
#endif

	/*アニメ動作テスト*/
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_0, 0	//0番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_1, 0	//0番のアニメ実行
	_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_0		//アニメ終了待ち
	_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_0		//アニメ停止
	_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_1		//アニメ停止

	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0204_pcwoman2_in
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0204_pcwoman2_in
	_OBJ_ANIME_WAIT

	//_OBJ_VISIBLE		OBJID_PLAYER
	//_OBJ_VISIBLE		OBJID_MINE

	//_OBJ_ANIME			OBJID_PLAYER,anm_d31r0204_trainer_in
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0204_trainer_in
	_OBJ_ANIME			FSW_LOCAL3,anm_d31r0204_trainer_in
	_OBJ_ANIME_WAIT

#if 0
	_SEQ_CLOSE_DOOR		77	//ドア閉める
	_SEQ_CLOSE_DOOR		78	//ドア閉める
	_WAIT_3D_ANIME		77	//アニメ終了待ち
	_WAIT_3D_ANIME		78	//アニメ終了待ち
	_FREE_3D_ANIME		77	//ドアアニメ解放	
	_FREE_3D_ANIME		78	//ドアアニメ解放	
#endif
	_RET

ev_tower_wayd_comanm_walk:
	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0204_pcwoman2_right
	//_OBJ_ANIME			OBJID_PLAYER,anm_d31r0204_trainer_right
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0204_trainer_right
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0204_pcwoman2_left
	_OBJ_ANIME			FSW_LOCAL3,anm_d31r0204_trainer_left
	_OBJ_ANIME_WAIT
	_RET

ev_tower_wayd_comanm_out:
	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0204_pcwoman2_out
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0204_pcwoman2_out
	//_OBJ_ANIME			OBJID_PLAYER,anm_d31r0204_trainer_out
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0204_trainer_out
	_OBJ_ANIME			FSW_LOCAL3,anm_d31r0204_trainer_out
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						OBJ_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0204_obj_change:
	_IFVAL_CALL			FSW_LOCAL1,NE,0,ev_d31r0204_obj_player
	//_OBJ_INVISIBLE		OBJID_PLAYER
	//_OBJ_INVISIBLE		OBJID_MINE
	_RET

ev_d31r0204_obj_player:
	//_OBJ_INVISIBLE		OBJID_PLAYER
	//半マスずらす
	//_PLAYER_POS_OFFSET_SET	8,0,0	
	_RET
	

