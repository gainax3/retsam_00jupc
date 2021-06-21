//==============================================================================
/**
 * @file	roulette.s
 * @brief	「ルーレット」フロンティアスクリプト
 * @author	nohara
 * @date	2007.09.05
 */
//==============================================================================

	.text

	.include	"../frontier_def.h"
	.include	"../roulette_def.h"
	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_roulette_room.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/syswork_def.h"
	.include	"../../particledata/pl_frontier/frontier_particle_def.h"	//SPA
	.include	"../../particledata/pl_frontier/frontier_particle_lst.h"	//EMIT

//--------------------------------------------------------------------
//					     スクリプト本体
//
//	FSW_PARAM0	録画処理で使用
//	FSW_PARAM1	リスト処理に使用
//	FSW_PARAM2	敵トレーナー1のOBJコード
//	FSW_PARAM3	敵トレーナー2のOBJコード
//	FSW_PARAM4	1=回復シーケンスを飛ばす(記録するも飛ばす)
//	FSW_PARAM5	パネル処理で使用
//	FSW_PARAM6	案内人見た目
//	FSW_LOCAL2	通信定義
//	FSW_LOCAL3	記録したか
//	FSW_LOCAL5	パネル処理で使用
//	FSW_LOCAL6	パネル処理で使用
//	FSW_LOCAL7	主人公の見た目
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_roulette_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了

#define PARTICLE_PANEL_EFF_WAIT		(60)	//演出パーティクルの共通ウェイト

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_BTL_OFF				//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック
//#define DEBUG_7BTL_OFF				//バトル7回オフデバック

/********************************************************************/
//
//		SCENE_ROULETTE:リソースラベル(画面IN時に常駐させるリソース群)
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

//====================================================================
//	SCENE_ROULETTE:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_SIO_USER_0			(1)
#define OBJID_SIO_USER_1			(2)
#define OBJID_MAN					(3)
#define OBJID_MINE					(4)
#define OBJID_PEOPLE1				(5)
#define OBJID_PEOPLE2				(6)
#define OBJID_PEOPLE3				(7)
#define OBJID_PEOPLE4				(8)
#define OBJID_PEOPLE5				(9)
//#define OBJID_PEOPLE6				(10)
#define OBJID_PEOPLE7				(11)
#define OBJID_PEOPLE8				(12)
#define OBJID_PEOPLE11				(13)
#define OBJID_PEOPLE12				(14)
#define OBJID_PEOPLE13				(15)
#define OBJID_PEOPLE14				(16)
#define OBJID_PEOPLE15				(17)
#define OBJID_PEOPLE16				(18)
#define OBJID_PCWOMAN2				(20)
#define OBJID_KAIHUKU				(97)
#define OBJID_ETC					(98)
#define OBJID_ETC2					(99)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

//通路アクター
_ACTOR_LABEL	way_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*16,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_UP, \
							8*23,8*24,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*23,8*24,OFF,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	way_set_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,8*16,OFF	/*自分自身のアクターセット*/
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*23,8*24,OFF
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*23,8*24,OFF
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_UP, \
							8*23,8*24,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


/********************************************************************/
//
//	SCENE_ROULETTE_BTL:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	default_scene2_resource
	//_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット	//通路からあるのでいらない
	//_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL			//すでにいるのでいらない
	_CHAR_RESOURCE_DATA		SUNGLASSES,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	default_scene2_resource_multi
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_LOCAL7, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		SUNGLASSES,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	people_scene_resource
	_CHAR_RESOURCE_DATA		MAN1,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		BIGMAN,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		MOUNT,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		IDOL,WF2DC_C_MOVENORMAL
	//_CHAR_RESOURCE_DATA		CAMPBOY,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	pcwoman2_scene2_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_ROULETTE_BTL:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define PLAYER_Y	(8*38-4)	//(8*38-4)
#define MINE_Y		(8*38)		//(8*38-4)

_ACTOR_LABEL	default_scene2_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,PLAYER_Y,OFF	//自分自身のアクタセット
	_ACTOR_DATA				OBJID_MINE,FSW_LOCAL7,WF2DMAP_WAY_C_UP, \
							8*16,MINE_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MAN,SUNGLASSES,WF2DMAP_WAY_C_UP, \
							8*16,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	default_scene2_actor_multi
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP,8*15,PLAYER_Y,OFF	//自分自身のアクタセット
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP,8*16,MINE_Y,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP,8*18,MINE_Y,ON
	_ACTOR_DATA				OBJID_MAN,SUNGLASSES,WF2DMAP_WAY_C_UP, \
							8*16,8*14,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	pcwoman2_scene2_actor
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							8*14,MINE_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------------
//	観客アクター
//--------------------------------------------------------------------
_ACTOR_LABEL	default_scene3_actor
	_ACTOR_DATA				OBJID_PEOPLE1,MAN1,WF2DMAP_WAY_C_RIGHT, \
							8*8,8*30,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE2,BIGMAN,WF2DMAP_WAY_C_LEFT, \
							8*10,8*30,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE3,IDOL,WF2DMAP_WAY_C_DOWN, \
							8*22,8*32,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE4,MOUNT,WF2DMAP_WAY_C_UP, \
							8*24,8*36,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE5,MOUNT,WF2DMAP_WAY_C_DOWN, \
							8*4,8*34,ON,EVENTID_TEST_SCR_OBJ1
	//_ACTOR_DATA				OBJID_PEOPLE6,BADMAN,WF2DMAP_WAY_C_DOWN, \
	//						8*4,8*32,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE7,IDOL,WF2DMAP_WAY_C_DOWN, \
							8*24,8*30,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE8,MAN1,WF2DMAP_WAY_C_DOWN, \
							8*26,8*30,ON,EVENTID_TEST_SCR_OBJ1
	/*同じキャラを複数配置*/
	_ACTOR_DATA				OBJID_PEOPLE11,MAN1,WF2DMAP_WAY_C_RIGHT, \
							8*8,8*32,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE12,IDOL,WF2DMAP_WAY_C_LEFT, \
							8*10,8*36,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE13,MOUNT,WF2DMAP_WAY_C_DOWN, \
							8*22,8*30,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE14,BIGMAN,WF2DMAP_WAY_C_RIGHT, \
							8*28,8*36,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE15,MAN1,WF2DMAP_WAY_C_RIGHT, \
							8*18,8*34,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PEOPLE16,MAN1,WF2DMAP_WAY_C_UP, \
							8*12,8*30,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------------
//	状況によって変えるリソース
// LOCAL0 = OBJCODE
//--------------------------------------------------------------------
_RESOURCE_LABEL	etc_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

_RESOURCE_LABEL	etc_set_resource2
	_CHAR_RESOURCE_DATA		FSW_PARAM2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		FSW_PARAM3,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------------
//	状況によって変えるアクター
// LOCAL0 = OBJCODE
//--------------------------------------------------------------------
_ACTOR_LABEL	etc_set_actor
	//_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
	//						8*30,8*18,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,8*26,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

_ACTOR_LABEL	etc_set_actor2
	//_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
	//						8*30,8*20,ON,EVENTID_TEST_SCR_OBJ1
	//_ACTOR_DATA			OBJID_ETC2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
	//						8*30,8*18,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ETC,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							8*16,8*26,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ETC2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							8*18,8*26,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


/********************************************************************/
//
//		回復してくれるスタッフ
//
/********************************************************************/
_RESOURCE_LABEL	kaihuku_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------------
//--------------------------------------------------------------------
_ACTOR_LABEL	kaihuku_set_actor
	_ACTOR_DATA			OBJID_KAIHUKU,FSW_PARAM6,WF2DMAP_WAY_C_UP, \
							8*6,8*28,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


//====================================================================
//	配置物アクター
//====================================================================
//--------------------------------------------------------------
//	ランプ配置物リソースラベル
//--------------------------------------------------------------
_ARTICLE_RESOURCE_LABEL	article_ramp_resource
	_ARTICLE_RESOURCE_DATA	ACTID_ROULETTE_RAMP
	_ARTICLE_RESOURCE_DATA_END

//--------------------------------------------------------------
//	ランプ配置物アクター
//--------------------------------------------------------------
#define RAMP_L_X			(24)
#define RAMP_R_X			(257)
#define RAMP_SINGLE_Y1		(56)

#define RAMP_WAIT			(45)

/*シングルL*/
_ARTICLE_ACTOR_LABEL	article_ramp_single_l_actor
	//id,x,y,visible,anm_start,act_work
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_RAMP, RAMP_L_X, RAMP_SINGLE_Y1, ON, OFF, ACTWORK_0
	_ARTICLE_ACTOR_DATA_END

/*シングルR*/
_ARTICLE_ACTOR_LABEL	article_ramp_single_r_actor
	//id,x,y,visible,anm_start,act_work
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_RAMP, RAMP_R_X, RAMP_SINGLE_Y1, ON, OFF, ACTWORK_1
	_ARTICLE_ACTOR_DATA_END

//--------------------------------------------------------------
//	ポケパネル配置物リソースラベル
//--------------------------------------------------------------
_ARTICLE_RESOURCE_LABEL	article_pokepanel_resource
	_ARTICLE_RESOURCE_DATA	ACTID_ROULETTE_POKEPANEL
	_ARTICLE_RESOURCE_DATA_END

//--------------------------------------------------------------
//	ポケパネル配置物アクター
//--------------------------------------------------------------
#define POKEPANEL_L_X			(44)
#define POKEPANEL_R_X			(212)
#define POKEPANEL_SINGLE_Y1		(42)
#define POKEPANEL_SINGLE_Y2		(82)

/*シングルL*/
_ARTICLE_ACTOR_LABEL	article_pokepanel_single_l_actor
	//id,x,y,visible,anm_start,act_work
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_POKEPANEL, POKEPANEL_L_X, POKEPANEL_SINGLE_Y1, ON, OFF, ACTWORK_4
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_POKEPANEL, POKEPANEL_L_X, POKEPANEL_SINGLE_Y2, ON, OFF, ACTWORK_5
	_ARTICLE_ACTOR_DATA_END

/*シングルR*/
_ARTICLE_ACTOR_LABEL	article_pokepanel_single_r_actor
	//id,x,y,visible,anm_start,act_work
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_POKEPANEL, POKEPANEL_R_X, POKEPANEL_SINGLE_Y1, ON, OFF, ACTWORK_6
	_ARTICLE_ACTOR_DATA		ACTID_ROULETTE_POKEPANEL, POKEPANEL_R_X, POKEPANEL_SINGLE_Y2, ON, OFF, ACTWORK_7
	_ARTICLE_ACTOR_DATA_END


/********************************************************************/
//
//					アニメーションデータ
//
/********************************************************************/

//--------------------------------------------------------------------
//対戦部屋まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_top
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1		/*表示*/
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,7
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1		/*非表示*/
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_top_multi
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1		/*表示*/
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,7
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1		/*非表示*/
	_ANIME_DATA_END

_ANIME_LABEL anm_man_go_top
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_WALK_L_8F,7
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1		/*非表示*/
	_ANIME_DATA_END

//--------------------------------------------------------------------
//話す動作
//--------------------------------------------------------------------
_ANIME_LABEL anm_stay_walk_left
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//開始位置まで移動
//--------------------------------------------------------------------
_ANIME_LABEL anm_player_go_center_scroll
	_ANIME_DATA	FC_WALK_U_8F,11
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center
	_ANIME_DATA	FC_WALK_U_8F,6
	_ANIME_DATA	FC_WALK_L_8F,4
	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_multi
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_WALK_U_8F,6
	_ANIME_DATA	FC_WALK_L_8F,4
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//見送る
//--------------------------------------------------------------------
_ANIME_LABEL anm_pcwoman2_go_center
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームの中央まで移動
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_room_in_single
	//_ANIME_DATA	FC_WALK_L_8F,3
	//_ANIME_DATA_END
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in
	//_ANIME_DATA	FC_WALK_L_8F,3
	//_ANIME_DATA_END
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_multi
	//_ANIME_DATA	FC_WALK_L_8F,3
	//_ANIME_DATA_END
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_go_btl_walk_left
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_go_btl_walk_right
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain
	//_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1		/*非表示*/
	//_ANIME_DATA	FC_WALK_L_8F,7
	//_ANIME_DATA	FC_WALK_D_8F,7
	//_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1		/*表示*/
	//_ANIME_DATA_END
	_ANIME_DATA	FC_SYSCMD_VISIBLE_OFF,1		/*非表示*/
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_SYSCMD_VISIBLE_ON,1		/*表示*/
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_12
	_ANIME_DATA	FC_STAY_WALK_R_4F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_room_in_brain_2
	_ANIME_DATA	FC_STAY_WALK_R_4F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_WALK_U_8F,3

	_ANIME_DATA	FC_STAY_WALK_L_4F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_WALK_R_8F,4

	_ANIME_DATA	FC_STAY_WALK_U_4F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1

	_ANIME_DATA	FC_WALK_U_8F,4
	_ANIME_DATA	FC_STAY_WALK_L_4F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_L_2F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_scroll_brain
	_ANIME_DATA	FC_WALK_D_8F,7
	_ANIME_DATA_END

_ANIME_LABEL anm_player_go_center_scroll_brain2
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_U_8F,7
	_ANIME_DATA_END

//--------------------------------------------------------------------
//主人公　ブレーンを見る
//--------------------------------------------------------------------
_ANIME_LABEL anm_mine_brain_down
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_mine_brain_down2
	_ANIME_DATA	FC_WAIT_32F,1
	_ANIME_DATA	FC_WAIT_32F,1
	_ANIME_DATA	FC_WAIT_32F,1
	_ANIME_DATA	FC_WAIT_16F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームから出て行く
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_go_out
	//_ANIME_DATA	FC_WALK_R_8F,8
	//_ANIME_DATA_END
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,8
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_go_out_multi
	//_ANIME_DATA	FC_WALK_R_8F,8
	//_ANIME_DATA_END
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_D_8F,8
	_ANIME_DATA_END

//--------------------------------------------------------------------
//敵トレーナーがバトルルームから出て行く(戦闘しない時)
//--------------------------------------------------------------------
_ANIME_LABEL etc_anime_go_out_btl_off
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA	FC_WALK_D_8F,8
	_ANIME_DATA_END

//--------------------------------------------------------------------
//プレイヤーが一歩下がる
//--------------------------------------------------------------------
_ANIME_LABEL mine_anime_go_out
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//パネルを見る
//--------------------------------------------------------------------
_ANIME_LABEL anm_panel_see_01
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_panel_see_02
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL anm_panel_see_03
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//観客	中心を見る
//--------------------------------------------------------------------
_ANIME_LABEL people_anime_11
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_12
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_13
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_14
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_15
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_16
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_17
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_18
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_18_2
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//観客が集まる
//--------------------------------------------------------------------
_ANIME_LABEL people_anime_01
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA_END

_ANIME_LABEL people_anime_02
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_03
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA_END

_ANIME_LABEL people_anime_04
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA_END

_ANIME_LABEL people_anime_05
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA_END

_ANIME_LABEL people_anime_06
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_07
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA_END

_ANIME_LABEL people_anime_08
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA_END

_ANIME_LABEL people_anime_02_2
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_15_5
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_16_5
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//観客	上を見る
//--------------------------------------------------------------------
_ANIME_LABEL people_anime_up_01
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_up_02
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_up_03
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//観客	入場時の動き
//--------------------------------------------------------------------
_ANIME_LABEL people_anime_40
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_41
	_ANIME_DATA	FC_STAY_WALK_D_16F,1
	_ANIME_DATA	FC_STAY_WALK_D_16F,1
	_ANIME_DATA	FC_STAY_WALK_U_16F,1
	_ANIME_DATA	FC_STAY_WALK_U_16F,1
	_ANIME_DATA	FC_STAY_WALK_D_16F,1
	_ANIME_DATA	FC_STAY_WALK_D_16F,1
	_ANIME_DATA_END

_ANIME_LABEL people_anime_42
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	それではルーレットいってみよー
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_50
#if 0
	_ANIME_DATA	FC_STAY_WALK_L_4F,1
	_ANIME_DATA	FC_STAY_WALK_U_2F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
#else
	_ANIME_DATA	FC_WALK_D_8F,1
#endif
	_ANIME_DATA_END

/*元の位置に戻す*/
_ANIME_LABEL pcwoman2_anime_55
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL pcwoman2_anime_57
#if 0
	_ANIME_DATA	FC_STAY_WALK_L_4F,1
	_ANIME_DATA	FC_STAY_WALK_U_4F,1
	_ANIME_DATA	FC_STAY_WALK_R_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_4F,1
#else
	_ANIME_DATA	FC_STAY_WALK_D_2F,1
	_ANIME_DATA	FC_STAY_WALK_D_4F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
#endif
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	右を向く
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_80
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	左を向く
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_90
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	話す
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_60
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL pcwoman2_anime_65
	//_ANIME_DATA	FC_STAY_WALK_U_8F,1
	_ANIME_DATA	FC_WAIT_8F,1
	//_ANIME_DATA	FC_STAY_WALK_L_4F,1
	_ANIME_DATA	FC_DIR_L,1
	//_ANIME_DATA	FC_STAY_WALK_D_4F,1
	_ANIME_DATA	FC_DIR_D,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	きのみ、アイテムを渡す
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_70
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA_END

_ANIME_LABEL pcwoman2_anime_72
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA_END

//--------------------------------------------------------------------
//司会	きのみ、アイテムを渡して戻る
//--------------------------------------------------------------------
_ANIME_LABEL pcwoman2_anime_75				/*BP*/
	_ANIME_DATA	FC_WALK_R_8F,3
	//_ANIME_DATA	FC_WALK_U_8F,1
	//_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL pcwoman2_anime_75_item			/*アイテム、きのみ*/
	_ANIME_DATA	FC_WALK_R_8F,3
	//_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

_ANIME_LABEL pcwoman2_anime_77
	_ANIME_DATA	FC_WALK_L_8F,3
	//_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_STAY_WALK_D_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//回復	近づいてくる
//--------------------------------------------------------------------
_ANIME_LABEL kaihuku_anime_10
	_ANIME_DATA	FC_WALK_U_4F,5
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END

_ANIME_LABEL kaihuku_anime_15
	_ANIME_DATA	FC_WALK_D_4F,6
	_ANIME_DATA_END

//--------------------------------------------------------------------
//主人公	回復を見る
//--------------------------------------------------------------------
_ANIME_LABEL player_kaihuku_anime_10
	_ANIME_DATA	FC_STAY_WALK_L_8F,1
	_ANIME_DATA_END

_ANIME_LABEL player_kaihuku_anime_15
	_ANIME_DATA	FC_STAY_WALK_R_8F,1
	_ANIME_DATA_END


/********************************************************************/
//
//							通路
//
/********************************************************************/
fss_roulette_start:
	//_EVENT_START

	/*lobbyで選択したタイプとレベルが必要*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_ROULETTE_TYPE,FSW_LOCAL1	/*type*/
	_SAVE_EVENT_WORK_GET	LOCALWORK3,FSW_LOCAL3				/*init*/
	_SAVE_EVENT_WORK_GET	LOCALWORK2,FSW_LOCAL4				/*pos1*/
	_SAVE_EVENT_WORK_GET	LOCALWORK5,FSW_LOCAL5				/*pos2*/
	_SAVE_EVENT_WORK_GET	LOCALWORK6,FSW_LOCAL6				/*pos3*/

	/*ワーク確保*/
	_BATTLE_REC_INIT
	_ROULETTE_WORK_ALLOC	FSW_LOCAL3,FSW_LOCAL1,FSW_LOCAL4,FSW_LOCAL5,FSW_LOCAL6,FSW_ANSWER

	/*主人公の見た目格納*/
	_GET_MINE_OBJ		FSW_LOCAL7

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_start_multi_way

	_LDVAL				FSW_PARAM6,BFSM
	_CALL				ev_roulette_actor_set_way
	_JUMP				ev_roulette_start_data_set_way
	_END

ev_roulette_start_multi_way:
	_CALL				ev_roulette_comm_command_initialize_1

	/*勝敗に関係なく通信フロンティアに挑戦(通信は中断がないので先頭で処理しても大丈夫)*/
	_SCORE_ADD			SCORE_ID_FRONTIER_COMM

	_LDVAL				FSW_PARAM6,BFSW1
	_CALL				ev_roulette_actor_set_way_multi
	_JUMP				ev_roulette_start_data_set_way
	_END

ev_roulette_start_data_set_way:
	_CALL				ev_roulette_rouka_pal_black							/*暗くする*/

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_go_btl_multi_way

	/*対戦部屋まで歩く*/
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_man_go_top
	_OBJ_ANIME			OBJID_MINE,anm_player_go_top
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
	_JUMP				ev_roulette_go_btl_sub
	_END

ev_roulette_go_btl_multi_way:
	/*対戦部屋まで歩く*/
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_man_go_top
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_top
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_top_multi
	_OBJ_ANIME_WAIT

	/*対戦部屋へマップ切り替え*/
	_SE_PLAY			SEQ_SE_DP_KAIDAN2
	_SE_WAIT			SEQ_SE_DP_KAIDAN2
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_JUMP				ev_roulette_go_btl_sub
	_END


/********************************************************************/
//
//							対戦部屋
//
/********************************************************************/
ev_roulette_go_btl_sub:
	_MAP_CHANGE			FSS_SCENEID_ROULETTE_BTL

	_LDVAL				FSW_PARAM4,0					/*回復シーケンス判断ワーククリア*/

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_start_multi

	_LDVAL				FSW_PARAM6,BFSM
	_CALL				ev_roulette_actor_set_btl
	_JUMP				ev_roulette_start_data_set
	_END

ev_roulette_start_multi:
	_ROULETTE_TOOL		FR_ID_SET_MAIN_BG,0,0,FSW_ANSWER/*背景変更(内部で通信タイプかチェック)*/

	_LDVAL				FSW_PARAM6,BFSW1
	_CALL				ev_roulette_actor_set_btl_multi
	_JUMP				ev_roulette_start_data_set
	_END


/********************************************************************/
//
/********************************************************************/
ev_roulette_start_data_set:
	_TIME_WAIT			1,FSW_ANSWER

	_CALL				ev_roulette_actor_set_btl_pcwoman
	_CALL				ev_roulette_actor_set_btl_people

	/*ポケパネルリソース＋アクターセット*/
	_CALL				ev_pokepanel_set

	_CALL				ev_roulette_btl_pal_black				/*暗くする*/

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*通信：基本情報やりとり*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_multi_comm_basic

	_JUMP				ev_roulette_start_2
	_END

/*通信：基本情報やりとり*/
ev_roulette_multi_comm_basic:
	/*通路に入ってすぐに移動*/
	//_CALL				ev_roulette_comm_command_initialize_1

//	_JUMP				ev_roulette_multi_comm_basic_retry
//	_END

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_BASIC
	_COMM_RESET

	_CALL				ev_roulette_multi_comm_basic_call
	_JUMP				ev_roulette_start_2
	_END

ev_roulette_multi_comm_basic_retry:
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_BASIC
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_multi_comm_basic_retry	/*送信失敗は、再送信へ*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
	_JUMP				ev_roulette_start_2
	_END


/********************************************************************/
//
/********************************************************************/
/*通信：基本情報やりとり*/
ev_roulette_multi_comm_basic_call:
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_BASIC
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_multi_comm_basic_call	/*送信失敗は、再送信へ*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
	_RET


/********************************************************************/
//
/********************************************************************/
ev_roulette_start_2:
	/*ワーク初期化*/
	_ROULETTE_WORK_INIT	FSW_LOCAL3

	/*通信：トレーナー情報やりとり*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_multi_comm_tr_2

	_JUMP				ev_roulette_start_3
	_END

ev_roulette_multi_comm_tr_2:

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_TR
	_COMM_RESET

	_JUMP				ev_roulette_multi_comm_tr
	_END

/*通信：トレーナー情報やりとり*/
ev_roulette_multi_comm_tr:
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_TR
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_multi_comm_tr	/*送信失敗は、再送信へ*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_POKE_DATA
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	1										/*大量データON*/
	_JUMP				ev_roulette_multi_comm_poke
	_END

ev_roulette_multi_comm_poke:
	/*通信：手持ちやりとり*/
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_TEMOTI
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_multi_comm_poke	/*送信失敗は、再送信へ*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET
	
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_BOTH_0
	_COMM_RESET

	_COMM_SET_WIFI_BOTH_NET	0										/*大量データOFF*/
	_JUMP				ev_roulette_start_3
	_END


/********************************************************************/
//
/********************************************************************/
ev_roulette_start_3:
	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_start_3_multi

	/*開始位置まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll
	_OBJ_ANIME			OBJID_MINE,anm_player_go_center
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME			OBJID_PEOPLE2,people_anime_40			/*bigman 上下に移動、左右見る*/
	_OBJ_ANIME			OBJID_PEOPLE5,people_anime_41			/*mount その場歩き*/
	_OBJ_ANIME			OBJID_PEOPLE7,people_anime_42			/*idol 左右にキョロキョロ*/
	_OBJ_ANIME_WAIT

	_JUMP				ev_roulette_room_start
	_END

ev_roulette_start_3_multi:
	/*開始位置まで歩く*/
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_player_go_center
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_player_go_center_multi
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_pcwoman2_go_center
	_OBJ_ANIME			OBJID_PEOPLE2,people_anime_40			/*bigman 上下に移動、左右見る*/
	_OBJ_ANIME			OBJID_PEOPLE5,people_anime_41			/*mount その場歩き*/
	_OBJ_ANIME			OBJID_PEOPLE7,people_anime_42			/*idol 左右にキョロキョロ*/
	_OBJ_ANIME_WAIT
	_JUMP				ev_roulette_room_start
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_roulette_room_start:
	
	/*誘導員削除*/
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6

	/*観客削除*/
	_CALL				ev_roulette_people_del

	/*今何人目か取得*/
	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_room_renshou_0		/*0	*/

	_CALL				ev_roulette_room_set_sub

	/*再開した時に司会の向きを下にする*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_65
	_OBJ_ANIME_WAIT

	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL				FSW_LOCAL3,1
	_JUMP				ev_roulette_room_saikai							/*1-7*/
	_END


/********************************************************************/
/*							0人目									*/
/********************************************************************/
ev_roulette_room_renshou_0:
	_CALL			ev_roulette_room_set_sub
	_JUMP			ev_roulette_room_go_rental
	_END

ev_roulette_room_set_sub:
	_TIME_WAIT		1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL	FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_roulette_multi_room_enemy_poke_send2

	/*(シングル、マルチ親用)敵のPOKEPARTYセット*/
	_ROULETTE_BTL_BEFORE_PARTY_SET

	_CALL			ev_roulette_poke_itemkeep_icon_add_mine			/*(MINE)ポケモンアイコン追加*/
	_RET


/********************************************************************/
/*					敵ポケモンデータを通信							*/
/********************************************************************/
ev_roulette_multi_room_enemy_poke_send2:

	/*recieve_countをクリアしておく(08.06.18 保険処理)*/
	/*同期後に処理すると、片方が先に進んで受信をしたあとに、クリアが入るかも知れないのでダメ！*/
	_ROULETTE_TOOL	FR_ID_RECIEVE_COUNT_CLEAR,0,0,FSW_ANSWER

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_ENEMY_BEFORE
	_COMM_RESET	

	_JUMP			ev_roulette_multi_room_enemy_poke_send2_retry
	_END

ev_roulette_multi_room_enemy_poke_send2_retry:
	/*通信：敵ポケモンやりとり*/
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_ENEMY
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_multi_room_enemy_poke_send2_retry/*失敗再送信*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_ENEMY_AFTER
	_COMM_RESET	

	/*敵ポケモンを生成*/
	_ROULETTE_TOOL		FR_ID_ENEMY_POKE_CREATE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_ENEMY_POKE_CREATE_2,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER
	_RET


/********************************************************************/
/*																	*/
/********************************************************************/
ev_roulette_room_go_rental:
	/*「ようこそバトルルーレットへ！」*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_65
	_OBJ_ANIME_WAIT
	_TALKMSG			msg_roulette_room_20
	_SE_PLAY			SEQ_SE_DP_DENDOU				/*歓声*/

	/*1人目の時は、道具預かりメッセージ表示*/
	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_roulette_room_first_msg

//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]デバック]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if 0

/*
	_PARTICLE_SPA_LOAD	SPAWORK_0, ROULETTE_SPA, FSS_CAMERA_ORTHO

	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI01_L
	_TIME_WAIT			120,FSW_ANSWER

	_PARTICLE_SPA_EXIT	SPAWORK_0
	_TIME_WAIT			120,FSW_ANSWER
*/

//////////////////////////////////////////////////////////////////////

	_LDVAL				FSW_LOCAL5,0
	_LDVAL				FSW_LOCAL6,0
	_CALL				debug_particle_test		//0
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//5
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//10
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//15
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//20
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//25
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//30
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//35
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//40
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//45
	_CALL				debug_particle_test
	_CALL				debug_particle_test		//47

	_LDVAL				FSW_LOCAL5,0
	_LDVAL				FSW_LOCAL6,0
#endif
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_multi_room_go_rental

	_JUMP				ev_roulette_room_go_rental_2
	_END

ev_roulette_multi_room_go_rental:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_roulette_room_wait
	_JUMP				ev_roulette_multi_room_go_rental_retry
	_END

ev_roulette_multi_room_go_rental_retry:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_RENTAL
	_COMM_RESET			/*画面切り替え前に初期化*/
	_JUMP				ev_roulette_room_go_rental_2
	_END

/*デバック(FSW_LOCAL5,6使用)*/
debug_particle_test:
	_PARTICLE_SPA_LOAD	SPAWORK_0, ROULETTE_SPA, FSS_CAMERA_ORTHO

	_ROULETTE_TOOL		FR_ID_ADD_EMITTER,FSW_LOCAL5,FSW_LOCAL6,FSW_ANSWER
	_TIME_WAIT			120,FSW_ANSWER

	_PARTICLE_SPA_EXIT	SPAWORK_0
	_TIME_WAIT			60,FSW_ANSWER
	_ADD_WK				FSW_LOCAL5,1		/*emit_noインクリメント*/
	_RET

/*1人目の時は、道具預かりメッセージ表示*/
ev_roulette_room_first_msg:
	_TALKMSG			msg_roulette_room_1
	_RET


/********************************************************************/
/*		「○回目のゲームです！さあ！チャレンジャーは誰だ！」		*/
/********************************************************************/
ev_roulette_room_go_rental_2:
	/*連勝数取得*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_msg_01_single
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_msg_01_multi
	_END

ev_roulette_msg_01_single:

	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK				FSW_ANSWER,1
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG			msg_roulette_room_21
	_AB_KEYWAIT
	_TALK_CLOSE
	_CALL				ev_room_choice_24				/*トレーナー登場*/

	/*リングデモ*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_50
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_21_01
	_AB_KEYWAIT

	_JUMP				ev_roulette_rank_go
	_END

ev_roulette_msg_01_multi:
	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK				FSW_ANSWER,1
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG_NOSKIP		msg_roulette_room_21
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_TALK_CLOSE
	_CALL				ev_room_choice_24				/*トレーナー登場*/

	/*リングデモ*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_50
	_OBJ_ANIME_WAIT

	_TALKMSG_NOSKIP		msg_roulette_room_21_01
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_CALL
	_COMM_RESET			/*画面切り替え前に初期化*/
	_JUMP				ev_roulette_rank_go
	_END


/********************************************************************/
/*						ルーレット画面へ							*/
/********************************************************************/
ev_roulette_rank_go:
	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_leader_1st_12
	_IFVAL_CALL			FSW_ANSWER,EQ,2,ev_roulette_leader_2nd_12

	/*連勝数を取得(ブレーンチェック用)(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get

	/*司会を元の位置に戻しておく*/
	//_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_55
	//_OBJ_ANIME_WAIT

	_CALL				ev_roulette_poke_itemkeep_icon_del		/*ポケモンアイコン削除*/
	_CALL				ev_roulette_poke_itemkeep_icon_resource_del	/*リソース削除*/

	/*ポケパネルリソース＋アクター削除*/
	_CALL				ev_pokepanel_del

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_rank_go_multi

	_ROULETTE_CALL
	//_SET_MAP_PROC

	/*ランプリソース*/
	_ARTICLE_RESOURCE_SET	article_ramp_resource

	/*ポケパネルリソース＋アクターセット*/
	_CALL					ev_pokepanel_set

	/*通信マルチ*/
	//_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL5
	//_IFVAL_CALL			FSW_LOCAL5,EQ,0,ev_roulette_single_ramp_set
	//_IFVAL_CALL			FSW_LOCAL5,EQ,1,ev_roulette_multi_ramp_set

	_CALL				ev_roulette_poke_itemkeep_icon_add		/*ポケモンアイコン追加*/

	_ROULETTE_TOOL		FR_ID_SET_PANEL_BG,0,0,FSW_ANSWER		/*パネル変更*/

	/*決定したイベントパネルを実行*/
	_ROULETTE_DECIDE_EV_NO_FUNC

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_comm_command_initialize_2

	/*通信：基本情報やりとり*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_multi_comm_basic_call

	_CALL				ev_roulette_btl_pal_black				/*BG暗くする*/
	_CALL				ev_roulette_obj_pal_black				/*OBJ暗くする*/

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP				ev_room_choice_23
	_END

/*パーティクル削除(ブレーン専用)*/
ev_roulette_leader_1st_12:
ev_roulette_leader_2nd_12:
	_PARTICLE_SPA_EXIT	SPAWORK_3
	_RET

/*同期*/
ev_roulette_rank_go_multi:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_CALL_2
	_COMM_RESET			/*画面切り替え前に初期化*/
	_RET


/********************************************************************/
/*						ランプアクター登録							*/
/********************************************************************/
ev_roulette_single_l_ramp_actor_set:
	_ARTICLE_ACTOR_SET		article_ramp_single_l_actor
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_0,0		//0番のアニメ実行
	//_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_0		//アニメ停止
	_RET

ev_roulette_single_r_ramp_actor_set:
	_ARTICLE_ACTOR_SET		article_ramp_single_r_actor
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_1,1		//1番のアニメ実行
	//_ARTICLE_ACTOR_ANIME_STOP	ACTWORK_1		//アニメ停止
	_RET


/********************************************************************/
/*						ランプアクター削除							*/
/********************************************************************/
ev_roulette_single_l_ramp_actor_free:
	_ARTICLE_ACTOR_FREE		ACTWORK_0
	_RET

ev_roulette_single_r_ramp_actor_free:
	_ARTICLE_ACTOR_FREE		ACTWORK_1
	_RET


/********************************************************************/
/*					決定したイベント演出							*/
/********************************************************************/
ev_room_choice_23:
	_PARTICLE_SPA_LOAD	SPAWORK_0, ROULETTE_SPA, FSS_CAMERA_ORTHO

	/*決定したイベントナンバーを取得*/
	_ROULETTE_TOOL		FR_ID_GET_DECIDE_EV_NO,0,0,FSW_ANSWER

	_SE_PLAY			SEQ_SE_DP_DENDOU								/*歓声*/

	_SWITCH				FSW_ANSWER
	_CASE_JUMP			ROULETTE_EV_ENEMY_HP_DOWN,ev_roulette_seq00		//敵
	_CASE_JUMP			ROULETTE_EV_ENEMY_DOKU,ev_roulette_seq01
	_CASE_JUMP			ROULETTE_EV_ENEMY_MAHI,ev_roulette_seq02
	_CASE_JUMP			ROULETTE_EV_ENEMY_YAKEDO,ev_roulette_seq03
	_CASE_JUMP			ROULETTE_EV_ENEMY_NEMURI,ev_roulette_seq04
	_CASE_JUMP			ROULETTE_EV_ENEMY_KOORI,ev_roulette_seq05
	_CASE_JUMP			ROULETTE_EV_ENEMY_SEED_GET,ev_roulette_seq06
	_CASE_JUMP			ROULETTE_EV_ENEMY_ITEM_GET,ev_roulette_seq07
	_CASE_JUMP			ROULETTE_EV_ENEMY_LV_UP,ev_roulette_seq08
	_CASE_JUMP			ROULETTE_EV_MINE_HP_DOWN,ev_roulette_seq09		//味方
	_CASE_JUMP			ROULETTE_EV_MINE_DOKU,ev_roulette_seq10
	_CASE_JUMP			ROULETTE_EV_MINE_MAHI,ev_roulette_seq11
	_CASE_JUMP			ROULETTE_EV_MINE_YAKEDO,ev_roulette_seq12
	_CASE_JUMP			ROULETTE_EV_MINE_NEMURI,ev_roulette_seq13
	_CASE_JUMP			ROULETTE_EV_MINE_KOORI,ev_roulette_seq14
	_CASE_JUMP			ROULETTE_EV_MINE_SEED_GET,ev_roulette_seq15
	_CASE_JUMP			ROULETTE_EV_MINE_ITEM_GET,ev_roulette_seq16
	_CASE_JUMP			ROULETTE_EV_MINE_LV_UP,ev_roulette_seq17
	_CASE_JUMP			ROULETTE_EV_PLACE_HARE,ev_roulette_seq18		//場に起きる
	_CASE_JUMP			ROULETTE_EV_PLACE_AME,ev_roulette_seq19
	_CASE_JUMP			ROULETTE_EV_PLACE_SUNAARASI,ev_roulette_seq20
	_CASE_JUMP			ROULETTE_EV_PLACE_ARARE,ev_roulette_seq21
	_CASE_JUMP			ROULETTE_EV_PLACE_KIRI,ev_roulette_seq22
	_CASE_JUMP			ROULETTE_EV_PLACE_TRICK,ev_roulette_seq23
	_CASE_JUMP			ROULETTE_EV_PLACE_SPEED_UP,ev_roulette_seq25
	_CASE_JUMP			ROULETTE_EV_PLACE_SPEED_DOWN,ev_roulette_seq26
	_CASE_JUMP			ROULETTE_EV_PLACE_RANDOM,ev_roulette_seq27
	_CASE_JUMP			ROULETTE_EV_EX_POKE_CHANGE,ev_roulette_seq24	//特殊
	_CASE_JUMP			ROULETTE_EV_EX_BP_GET,ev_roulette_seq28
	_CASE_JUMP			ROULETTE_EV_EX_BTL_WIN,ev_roulette_seq29
	_CASE_JUMP			ROULETTE_EV_EX_SUKA	,ev_roulette_seq30
	_CASE_JUMP			ROULETTE_EV_EX_BP_GET_BIG,ev_roulette_seq31
	_END


/********************************************************************/
/*							敵HPイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq00:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_roulette_seq00_single_02
	_IFVAL_JUMP			FSW_LOCAL6,EQ,1,ev_roulette_seq00_multi_02
	_END

ev_roulette_seq00_sub:
	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_spot_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq00_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq00_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq00_single_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_PINK03
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_PINK04
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_PINK03BASE
	_JUMP				ev_roulette_seq00_sub
	_END

ev_roulette_seq00_multi_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_PINK03
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_PINK04
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_PINK04BASE
	_JUMP				ev_roulette_seq00_sub
	_END

ev_roulette_seq00_single:
	_TALKMSG			msg_roulette_room_23
	_RET

ev_roulette_seq00_multi:
	_TALKMSG			msg_roulette_room_24
	_RET


/********************************************************************/
/*							敵どくイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq01:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_PURPLE03
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_PURPLE04
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq01_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq01_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq01_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq01_single_safe

	_TALKMSG			msg_roulette_room_27
	_RET

ev_roulette_seq01_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq01_multi_safe

	_TALKMSG			msg_roulette_room_28
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq01_single_safe:
	_TALKMSG			msg_roulette_room_30_23
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq01_multi_safe:
	_TALKMSG			msg_roulette_room_30_24
	_RET


/********************************************************************/
/*							敵まひイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq02:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_YELLOW03
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_YELLOW04
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq02_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq02_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq02_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq02_single_safe

	_TALKMSG			msg_roulette_room_29
	_RET

ev_roulette_seq02_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq02_multi_safe

	_TALKMSG			msg_roulette_room_30
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq02_single_safe:
	_TALKMSG			msg_roulette_room_30_25
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq02_multi_safe:
	_TALKMSG			msg_roulette_room_30_26
	_RET


/********************************************************************/
/*							敵やけどイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq03:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_RED03
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_RED04
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq03_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq03_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq03_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq03_single_safe

	_TALKMSG			msg_roulette_room_30_01
	_RET

ev_roulette_seq03_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq03_multi_safe

	_TALKMSG			msg_roulette_room_30_02
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq03_single_safe:
	_TALKMSG			msg_roulette_room_30_27
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq03_multi_safe:
	_TALKMSG			msg_roulette_room_30_28
	_RET


/********************************************************************/
/*							敵ねむりイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq04:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_WHITE03
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_WHITE04
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq04_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq04_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq04_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq04_single_safe

	_TALKMSG			msg_roulette_room_30_03
	_RET

ev_roulette_seq04_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq04_multi_safe

	_TALKMSG			msg_roulette_room_30_04
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq04_single_safe:
	_TALKMSG			msg_roulette_room_30_29
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq04_multi_safe:
	_TALKMSG			msg_roulette_room_30_30
	_RET


/********************************************************************/
/*							敵こおりイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq05:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_AO03
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_AO04
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq05_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq05_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq05_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq05_single_safe

	_TALKMSG			msg_roulette_room_30_05
	_RET

ev_roulette_seq05_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq05_multi_safe

	_TALKMSG			msg_roulette_room_30_06
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq05_single_safe:
	_TALKMSG			msg_roulette_room_30_31
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq05_multi_safe:
	_TALKMSG			msg_roulette_room_30_32
	_RET


/********************************************************************/
/*							敵きのみイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq06:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_enemy

	_TALK_CLOSE
	_CALL				staff_anm_03

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_ITEM,1,0,FSW_ANSWER
	_ITEM_NAME			2,FSW_ANSWER										/*アイテム名セット*/

	/*話す*/
	//_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	//_OBJ_ANIME_WAIT

	/*アイテム持っているアイコン表示*/
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,0,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,1,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,2,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,3,1

	_TALKMSG			msg_roulette_room_30_08_01
	_TALK_CLOSE
	_CALL				staff_anm_04

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq06_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq06_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq06_single:
	_TALKMSG			msg_roulette_room_30_07
	_RET

ev_roulette_seq06_multi:
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_TALKMSG			msg_roulette_room_30_08
	_RET


/********************************************************************/
/*							敵アイテムイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq07:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_enemy

	_TALK_CLOSE
	_CALL				staff_anm_03

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_ITEM,1,0,FSW_ANSWER
	_ITEM_NAME			2,FSW_ANSWER										/*アイテム名セット*/

	/*アイテム持っているアイコン表示*/
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,0,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,1,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,2,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,3,1

	_TALKMSG			msg_roulette_room_30_08_01
	_TALK_CLOSE
	_CALL				staff_anm_04

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq07_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq07_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq07_single:
	_TALKMSG			msg_roulette_room_30_07
	_RET

ev_roulette_seq07_multi:
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_TALKMSG			msg_roulette_room_30_08
	_RET


/********************************************************************/
/*							敵レベルイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq08:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_CALL				se_ramp_enemy

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_roulette_seq08_single_02
	_IFVAL_JUMP			FSW_LOCAL6,EQ,1,ev_roulette_seq08_multi_02
	_END

ev_roulette_seq08_sub:
	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_spot_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq08_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq08_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq08_single_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_GREEN03
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_GREEN04
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_GREEN03BASE
	_JUMP				ev_roulette_seq08_sub
	_END

ev_roulette_seq08_multi_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_GREEN03
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_GREEN04
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_GREEN04BASE
	_JUMP				ev_roulette_seq08_sub
	_END

ev_roulette_seq08_single:
	_TALKMSG			msg_roulette_room_25
	_RET

ev_roulette_seq08_multi:
	_TALKMSG			msg_roulette_room_26
	_RET


/********************************************************************/
/*							味方ＨＰイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq09:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_roulette_seq09_single_02
	_IFVAL_JUMP			FSW_LOCAL6,EQ,1,ev_roulette_seq09_multi_02
	_END

ev_roulette_seq09_sub:
	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_spot_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq09_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq09_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq09_single_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_PINK01
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_PINK02
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_PINK01BASE
	_JUMP				ev_roulette_seq09_sub
	_END

ev_roulette_seq09_multi_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_PINK01
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_PINK02
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_PINK02BASE
	_JUMP				ev_roulette_seq09_sub
	_END

ev_roulette_seq09_single:
	_TALKMSG			msg_roulette_room_23
	_RET

ev_roulette_seq09_multi:
	_TALKMSG			msg_roulette_room_24
	_RET


/********************************************************************/
/*							味方どくイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq10:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_PURPLE01
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_PURPLE02
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq10_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq10_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq10_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq10_single_safe

	_TALKMSG			msg_roulette_room_27
	_RET

ev_roulette_seq10_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq10_multi_safe

	_TALKMSG			msg_roulette_room_28
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq10_single_safe:
	_TALKMSG			msg_roulette_room_30_23
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq10_multi_safe:
	_TALKMSG			msg_roulette_room_30_24
	_RET


/********************************************************************/
/*							味方まひイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq11:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_YELLOW01
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_YELLOW02
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq11_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq11_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq11_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq11_single_safe

	_TALKMSG			msg_roulette_room_29
	_RET

ev_roulette_seq11_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq11_multi_safe

	_TALKMSG			msg_roulette_room_30
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq11_single_safe:
	_TALKMSG			msg_roulette_room_30_25
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq11_multi_safe:
	_TALKMSG			msg_roulette_room_30_26
	_RET


/********************************************************************/
/*						味方やけどイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq12:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_RED01
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_RED02
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq12_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq12_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq12_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq12_single_safe

	_TALKMSG			msg_roulette_room_30_01
	_RET

ev_roulette_seq12_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq12_multi_safe

	_TALKMSG			msg_roulette_room_30_02
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq12_single_safe:
	_TALKMSG			msg_roulette_room_30_27
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq12_multi_safe:
	_TALKMSG			msg_roulette_room_30_28
	_RET


/********************************************************************/
/*							味方ねむりイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq13:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_WHITE01
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_WHITE02
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq13_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq13_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq13_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq13_single_safe

	_TALKMSG			msg_roulette_room_30_03
	_RET

ev_roulette_seq13_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq13_multi_safe

	_TALKMSG			msg_roulette_room_30_04
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq13_single_safe:
	_TALKMSG			msg_roulette_room_30_29
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq13_multi_safe:
	_TALKMSG			msg_roulette_room_30_30
	_RET


/********************************************************************/
/*							味方こおりイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq14:
	//_SE_PLAY			SEQ_SE_PL_FR01
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_LDVAL				FSW_LOCAL5,ROULETTE_BS_LASOR_AO01
	_LDVAL				FSW_LOCAL6,ROULETTE_BS_LASOR_AO02
	_CALL				se_laser_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq14_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq14_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq14_single:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq14_single_safe

	_TALKMSG			msg_roulette_room_30_05
	_RET

ev_roulette_seq14_multi:
	/*誰も効果を発揮しなかった時*/
	_ROULETTE_TOOL		FR_ID_GET_POKE_SAFE_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq14_multi_safe

	_TALKMSG			msg_roulette_room_30_06
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq14_single_safe:
	_TALKMSG			msg_roulette_room_30_31
	_RET

/*誰も効果を発揮しなかった時*/
ev_roulette_seq14_multi_safe:
	_TALKMSG			msg_roulette_room_30_32
	_RET


/********************************************************************/
/*							味方木の実イベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq15:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine

	_TALK_CLOSE
	_CALL				staff_anm_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_ITEM,0,0,FSW_ANSWER
	_ITEM_NAME			2,FSW_ANSWER										/*アイテム名セット*/

	/*アイテム持っているアイコン表示*/
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,0,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,1,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,2,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,3,1

	_TALKMSG			msg_roulette_room_30_08_01
	_TALK_CLOSE
	_CALL				staff_anm_02_item

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq15_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq15_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq15_single:
	_TALKMSG			msg_roulette_room_30_07
	_RET

ev_roulette_seq15_multi:
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_TALKMSG			msg_roulette_room_30_08
	_RET


/********************************************************************/
/*							味方アイテムイベント					*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq16:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine

	_TALK_CLOSE
	_CALL				staff_anm_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_ITEM,0,0,FSW_ANSWER
	_ITEM_NAME			2,FSW_ANSWER										/*アイテム名セット*/

	/*アイテム持っているアイコン表示*/
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,0,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,1,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,2,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,3,1

	_TALKMSG			msg_roulette_room_30_08_01
	_TALK_CLOSE
	_CALL				staff_anm_02_item

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq16_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq16_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq16_single:
	_TALKMSG			msg_roulette_room_30_07
	_RET

ev_roulette_seq16_multi:
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_TALKMSG			msg_roulette_room_30_08
	_RET


/********************************************************************/
/*							味方レベルイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq17:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	/*司会左を向く*/
	_CALL				ev_roulette_man_left

	_CALL				se_ramp_mine

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_roulette_seq17_single_02
	_IFVAL_JUMP			FSW_LOCAL6,EQ,1,ev_roulette_seq17_multi_02
	_END

ev_roulette_seq17_sub:
	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_spot_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq17_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq17_multi

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq17_single_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_GREEN01
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_GREEN02
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_GREEN01BASE
	_JUMP				ev_roulette_seq17_sub
	_END

ev_roulette_seq17_multi_02:
	_LDVAL				FSW_LOCAL5,ROULETTE_BR_SPOT_GREEN01
	_LDVAL				FSW_LOCAL6,ROULETTE_BR_SPOT_GREEN02
	_LDVAL				FSW_PARAM5,ROULETTE_BR_SPOT_GREEN02BASE
	_JUMP				ev_roulette_seq17_sub
	_END

ev_roulette_seq17_single:
	_TALKMSG			msg_roulette_room_25
	_RET

ev_roulette_seq17_multi:
	_TALKMSG			msg_roulette_room_26
	_RET


/********************************************************************/
/*							晴れイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq18:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_mirror_red

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_09								/*晴れ*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							雨イベント								*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq19:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_mirror_blue

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_10								/*雨*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							砂嵐イベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq20:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				se_smoke_01

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_11								/*砂嵐*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							あられイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq21:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				ev_roulette_btl_pal_white							/*元に戻す*/
	_CALL				se_mirror_white

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_12								/*あられ*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							霧イベント								*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq22:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				se_smoke_02

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_13								/*霧*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							トリックイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq23:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	_CALL				se_smoke_03

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_14								/*トリック*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							ポケモンチェンジイベント				*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq24:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_all

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq24_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq24_multi

	/*走査線を赤く点滅に変更*/
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_4,2		//2番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_5,6		//6番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_6,3		//3番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_7,7		//7番のアニメ実行

	_SE_PLAY			SEQ_SE_DP_Z_SEARCH									/*チェンジ音*/

	/*順番に消していく*/
	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,0,3,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,0,3,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,3,0
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,3,0
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,0,2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,0,2,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,2,0
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,2,0
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,0,1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,0,1,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,1,0
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,1,0
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,0,0,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,0,0
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,0,0
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_ICON_CHANGE,0,0,FSW_ANSWER					/*表示位置変更*/

	_SE_PLAY			SEQ_SE_DP_Z_SEARCH									/*チェンジ音*/

	/*順番に表示していく*/
	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,1,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,1,0,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,0,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,0,1
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,1,1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,1,1,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,1,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,1,1
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,1,2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,1,2,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,2,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,2,1
	_TIME_WAIT			14,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_MINE_CHG_POKE_ICON_VANISH,1,3,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ENEMY_CHG_POKE_ICON_VANISH,1,3,FSW_ANSWER
	_ROULETTE_CHG_ITEMKEEP_VANISH	0,3,1
	_ROULETTE_CHG_ITEMKEEP_VANISH	1,3,1
	_TIME_WAIT			14,FSW_ANSWER

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_15								/*ポケモンチェンジ*/
	_SE_STOP			SEQ_SE_DP_Z_SEARCH									/*チェンジ音*/

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq24_single_2
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq24_multi_2

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

/*パネルを見る*/
ev_roulette_seq24_single:
	_OBJ_ANIME			OBJID_MINE,anm_panel_see_01
	_OBJ_ANIME			OBJID_ETC,anm_panel_see_01
	_OBJ_ANIME_WAIT
	_RET

ev_roulette_seq24_multi:
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_panel_see_01
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_panel_see_01
	_OBJ_ANIME			OBJID_ETC,anm_panel_see_01
	_OBJ_ANIME			OBJID_ETC2,anm_panel_see_01
	_OBJ_ANIME_WAIT
	_RET

/*元の向きに戻す*/
ev_roulette_seq24_single_2:
	_OBJ_ANIME			OBJID_MINE,anm_panel_see_02
	_OBJ_ANIME			OBJID_ETC,anm_panel_see_03
	_OBJ_ANIME_WAIT
	_RET

ev_roulette_seq24_multi_2:
	_OBJ_ANIME			OBJID_SIO_USER_0,anm_panel_see_02
	_OBJ_ANIME			OBJID_SIO_USER_1,anm_panel_see_02
	_OBJ_ANIME			OBJID_ETC,anm_panel_see_03
	_OBJ_ANIME			OBJID_ETC2,anm_panel_see_03
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*							速度UPイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq25:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine

	_CALL				se_monitor_01

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_16								/*ルーレット速度UP*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							速度DOWNイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq26:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine

	_CALL				se_monitor_01

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_17								/*ルーレット速度DOWN*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							速度RANDOMイベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq27:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine

	_CALL				se_monitor_01

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_18								/*ルーレット挙動RANDOM*/

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							BP追加イベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq28:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine
	_TALK_CLOSE

	_CALL				staff_anm_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_PANEL_BP,0,0,FSW_ANSWER
	_RECORD_ADD			RECID_ROULETTE_BP,FSW_ANSWER
	_BTL_POINT_ADD		FSW_ANSWER											/*バトルポイント追加*/
	_NUMBER_NAME		2,FSW_ANSWER

	/*「○さんに○BPをプレゼントします」*/
	/*通信マルチ*/
	_SE_STOP			SEQ_SE_DP_DENDOU
	_ME_PLAY			ME_ROULETTE_BP_GET
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq28_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq28_multi
	_ME_WAIT
	_TALK_CLOSE

	/*元の位置に戻る*/
	_CALL				staff_anm_02

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_19_02

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END

ev_roulette_seq28_single:
	_TALKMSG			msg_roulette_room_30_19
	_RET

ev_roulette_seq28_multi:
	_TALKMSG			msg_roulette_room_30_19_01
	_RET


/********************************************************************/
/*							BP大追加イベント						*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq31:
	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_mine
	_TALK_CLOSE

	_CALL				staff_anm_01

	_ROULETTE_TOOL		FR_ID_TR_NAME,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_TR_NAME,1,1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_PANEL_BP,0,0,FSW_ANSWER
	_RECORD_ADD			RECID_ROULETTE_BP,FSW_ANSWER
	_BTL_POINT_ADD		FSW_ANSWER											/*バトルポイント追加*/
	_NUMBER_NAME		2,FSW_ANSWER

	/*通信マルチ*/
	_SE_STOP			SEQ_SE_DP_DENDOU
	_ME_PLAY			ME_ROULETTE_BP_GET
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL6
	_IFVAL_CALL			FSW_LOCAL6,EQ,0,ev_roulette_seq28_single
	_IFVAL_CALL			FSW_LOCAL6,EQ,1,ev_roulette_seq28_multi
	_ME_WAIT
	_TALK_CLOSE

	_CALL				staff_anm_02

	/*話す*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_60
	_OBJ_ANIME_WAIT

	_TALKMSG			msg_roulette_room_30_19_02

	_SE_PLAY			SEQ_SE_DP_DENDOU									/*歓声*/
	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*							★戦闘スルーイベント					*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq29:
	/*回復シーケンスを飛ばす*/
	_LDVAL				FSW_PARAM4,1

	_TALKMSG			msg_roulette_room_22								/*今回のイベントこちら*/

	_CALL				se_ramp_enemy
	_TALK_CLOSE

	/*敵のアイコンアクターのみ削除*/
	_CALL				ev_roulette_poke_itemkeep_icon_del_enemy

	_CALL				ev_roulette_battle_btl_off							/*戦闘なし退場*/

	/*司会右を向く*/
	_CALL				ev_roulette_man_right

	_TALKMSG			msg_roulette_room_30_20								/*戦闘スルー*/

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_seq29_single
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_seq29_multi
	_END

ev_roulette_seq29_single:
	_AB_KEYWAIT
	_JUMP				ev_roulette_seq29_sub
	_END

ev_roulette_seq29_multi:
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	//_AB_KEYWAIT

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_BTL_WIN_PANEL
	_COMM_RESET

	_JUMP				ev_roulette_seq29_sub
	_END

ev_roulette_seq29_sub:
	_TALK_CLOSE

	/*ブーイングSE*/
	//_SE_PLAY			SEQ_SE_DP_SELECT
	//_SE_WAIT			SEQ_SE_DP_SELECT

	/*司会を元の位置に戻しておく*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_55
	_OBJ_ANIME_WAIT

	_ROULETTE_TOOL		FR_ID_PANEL_BG_OFF,0,0,FSW_ANSWER					/*パネル非表示*/

	_JUMP				ev_roulette_battle_3								/*戦闘勝利後の流れへ*/
	_END


/********************************************************************/
/*							★スカイベント							*/
/*	FSW_LOCAL5,6使用												*/
/********************************************************************/
ev_roulette_seq30:
	_TALKMSG			msg_roulette_room_30_21								/*ざんねん*/

	/*ブーイングSE*/
	//_SE_PLAY			SEQ_SE_DP_SELECT
	//_SE_WAIT			SEQ_SE_DP_SELECT

	_JUMP				ev_roulette_room_go_2								/*たいせんへ*/
	_END


/********************************************************************/
/*					SE+ランプ演出(FSW_LOCAL5使用)					*/
/********************************************************************/
se_ramp_mine:
	_CALL				ev_roulette_single_l_ramp_actor_set

	_SE_PLAY			SEQ_SE_PL_FR05
	//_SE_WAIT			SEQ_SE_PL_FR05
	//_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_0		//アニメ終了待ち
	_TIME_WAIT			RAMP_WAIT,FSW_ANSWER

	_CALL				ev_roulette_single_l_ramp_actor_free
	_RET

se_ramp_enemy:
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL5
	_CALL				ev_roulette_single_r_ramp_actor_set

	_SE_PLAY			SEQ_SE_PL_FR05
	//_SE_WAIT			SEQ_SE_PL_FR05
	//_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_1		//アニメ終了待ち
	_TIME_WAIT			RAMP_WAIT,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL5
	_CALL				ev_roulette_single_r_ramp_actor_free
	_RET

se_ramp_all:
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_LOCAL5
	_CALL				ev_roulette_single_l_ramp_actor_set
	_CALL				ev_roulette_single_r_ramp_actor_set

	_SE_PLAY			SEQ_SE_PL_FR05
	//_SE_WAIT			SEQ_SE_PL_FR05
	//_ARTICLE_ACTOR_ANIME_WAIT	ACTWORK_0		//アニメ終了待ち
	_TIME_WAIT			RAMP_WAIT,FSW_ANSWER

	_CALL				ev_roulette_single_l_ramp_actor_free
	_CALL				ev_roulette_single_r_ramp_actor_free
	_RET


/********************************************************************/
/*						SE+ミラーボール演出							*/
/********************************************************************/
se_mirror_blue:
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_BLUE
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_BLUEBG
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR04
	_RET

se_mirror_red:
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_RED
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_REDBG
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR04
	_RET

se_mirror_white:
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_WHITE
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_MIRROR_WHITEBG
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR04
	_RET


/********************************************************************/
/*						SE+スモーク演出								*/
/********************************************************************/
se_smoke_01:
	_SE_PLAY			SEQ_SE_PL_FR03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI01_L
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI01_R
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR03
	_RET

se_smoke_02:
	_SE_PLAY			SEQ_SE_PL_FR03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI02_L
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI02_R
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR03
	_RET

se_smoke_03:
	_SE_PLAY			SEQ_SE_PL_FR03
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI03_L
	_PARTICLE_ADD_EMITTER	SPAWORK_0, ROULETTE_BR_KEMURI03_R
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR03
	_RET


/********************************************************************/
/*SE+レーザー演出(FSW_LOCAL5,6使用 ファイル名がなぜか"BS"なので注意)*/
/********************************************************************/
se_laser_01:
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_ADD_EMITTER	SPAWORK_0,FSW_LOCAL5
	_PARTICLE_ADD_EMITTER	SPAWORK_0,FSW_LOCAL6
	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR04
	_RET


/********************************************************************/
/*			SE+スポットライト演出(FSW_LOCAL5,6,FSW_PARAM5使用)		*/
/********************************************************************/
se_spot_01:
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_ADD_EMITTER	SPAWORK_0,FSW_LOCAL5
	_PARTICLE_ADD_EMITTER	SPAWORK_0,FSW_LOCAL6
	_PARTICLE_ADD_EMITTER	SPAWORK_0,FSW_PARAM5

	//_PARTICLE_WAIT
	_TIME_WAIT			PARTICLE_PANEL_EFF_WAIT,FSW_ANSWER
	_SE_WAIT			SEQ_SE_PL_FR04
	_RET


/********************************************************************/
/*						SE+モニター点滅								*/
/********************************************************************/
se_monitor_01:
	_SE_PLAY			SEQ_SE_PL_FR05
	//_SE_WAIT			SEQ_SE_PL_FR05
	_RET


/********************************************************************/
/*					受付に手渡されるアニメ							*/
/********************************************************************/
/*味方側*/
staff_anm_01:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_70
	_OBJ_ANIME_WAIT
	_RET

staff_anm_02_item:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_75_item
	_OBJ_ANIME_WAIT
	_RET

staff_anm_02:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_75
	_OBJ_ANIME_WAIT
	_RET

/*敵側*/
staff_anm_03:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_72
	_OBJ_ANIME_WAIT
	_RET

staff_anm_04:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_77
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						トレーナー登場								*/
/********************************************************************/
ev_room_choice_24:
	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_send_sel_taisen
	_JUMP				ev_roulette_taisen_go
	_END

ev_roulette_taisen_go:
	_JUMP				ev_roulette_room_etc_add
	_END

ev_roulette_send_sel_taisen:
	/*「しばらくお待ちください」*/
	//_TALKMSG_NOSKIP	msg_roulette_room_wait
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_CHANGE
	_COMM_RESET			/*画面切り替え前に初期化*/

	//_TALK_CLOSE
	_JUMP				ev_roulette_room_etc_add
	_END


/********************************************************************/
/*																	*/
/********************************************************************/
ev_roulette_comm_command_initialize_1:
	_TIME_WAIT			1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_INIT_1
	_COMM_RESET
	_RET

ev_roulette_comm_command_initialize_2:
	_TIME_WAIT			1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_INIT_2
	_COMM_RESET
	_RET

ev_roulette_comm_command_initialize_3:
	_TIME_WAIT			1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_INIT_3
	_COMM_RESET
	_RET

ev_roulette_comm_command_initialize_4:
	_TIME_WAIT			1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_INIT_4
	_COMM_RESET
	_RET


/********************************************************************/
/*						トレーナー登場2								*/
/********************************************************************/
ev_roulette_room_etc_add:
	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_go_multi

	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_leader_1st_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_leader_2nd_02

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*敵トレーナーが中央まで歩く*/
	_CALL				ev_roulette_trainer_set_1
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_single
	_OBJ_ANIME_WAIT

	_CALL				ev_roulette_poke_itemkeep_icon_add_enemy	/*(ENEMY)ポケモンアイコン追加*/
	_RET

ev_roulette_room_go_multi:
	/*敵トレーナーが中央まで歩く*/
	_CALL				ev_roulette_trainer_set_2
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in
	_OBJ_ANIME			OBJID_ETC2,etc_anime_room_in_multi
	_OBJ_ANIME_WAIT

	_CALL				ev_roulette_poke_itemkeep_icon_add_enemy	/*(ENEMY)ポケモンアイコン追加*/
	_RET


/********************************************************************/
/*																	*/
/********************************************************************/
ev_roulette_room_go_2:
	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_msg_02_single
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_msg_02_multi
	_END

ev_roulette_msg_02_single:
	_AB_KEYWAIT
	_TALK_CLOSE

	/*司会を元の位置に戻しておく*/
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_55
	_OBJ_ANIME_WAIT

	/*「それではバトルスタート！」*/
	_TALKMSG			msg_roulette_room_30_22
	_AB_KEYWAIT
	_TALK_CLOSE

	_TIME_WAIT			15,FSW_ANSWER

	_OBJ_ANIME			OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_ROULETTE_TOOL	FR_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_ROULETTE_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE

	/*一歩前へ*/
	_OBJ_ANIME			OBJID_MINE,anm_go_btl_walk_left
	_OBJ_ANIME			OBJID_ETC,anm_go_btl_walk_right
	_OBJ_ANIME_WAIT

	_JUMP				ev_roulette_battle
	_END

ev_roulette_msg_02_multi:
	/*途中に文字送りがあるので、ウェイトではなく、キー待ちを入れてしまう*/
	//_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_AB_KEYWAIT

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_roulette_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_PANEL_INFO
	_COMM_RESET			/*画面切り替え前に初期化*/

	//_AB_KEYWAIT
	//_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER

	/*司会を元の位置に戻しておく*/
	_TALK_CLOSE
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_55
	_OBJ_ANIME_WAIT

	/*「それではバトルスタート！」*/
	_TALKMSG_NOSKIP		msg_roulette_room_30_22
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_TALK_CLOSE

	_TIME_WAIT			15,FSW_ANSWER

	_OBJ_ANIME			OBJID_ETC,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_ROULETTE_TOOL	FR_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_ROULETTE_APPEAR	0
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	_OBJ_ANIME			OBJID_ETC2,anm_stay_walk_left
	_OBJ_ANIME_WAIT

	/*対戦前台詞*/
	_ROULETTE_TOOL	FR_ID_SET_B_TOWER_PARTNER_DATA,0,0,FSW_ANSWER
	_TALKMSG_ROULETTE_APPEAR	1
	_TIME_WAIT		30,FSW_ANSWER
	_TALK_CLOSE

	/*一歩前へ*/
	_OBJ_ANIME		OBJID_SIO_USER_0,anm_go_btl_walk_left
	_OBJ_ANIME		OBJID_SIO_USER_1,anm_go_btl_walk_left
	_OBJ_ANIME		OBJID_ETC,anm_go_btl_walk_right
	_OBJ_ANIME		OBJID_ETC2,anm_go_btl_walk_right
	_OBJ_ANIME_WAIT

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_roulette_room_wait

	_JUMP				ev_roulette_battle
	_END


/********************************************************************/
/*					トレーナーの見た目セット						*/
/********************************************************************/
ev_roulette_trainer_set_1:
	_ROULETTE_TOOL		FR_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK				FSW_PARAM2,FSW_ANSWER
	_CALL				ev_roulette_actor_set_btl_tr
	_RET

ev_roulette_trainer_set_2:
	_ROULETTE_TOOL		FR_ID_GET_TR_OBJ_CODE,0,0,FSW_ANSWER
	_LDWK				FSW_PARAM2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_GET_TR_OBJ_CODE,1,0,FSW_ANSWER
	_LDWK				FSW_PARAM3,FSW_ANSWER
	_CALL				ev_roulette_actor_set_btl_tr_multi
	_RET

ev_roulette_trainer_set_brain:
	_LDVAL				FSW_PARAM2,BRAINS4
	_CALL				ev_roulette_actor_set_btl_tr
	_RET


/********************************************************************/
/*						お互い一歩前へ								*/
/********************************************************************/
ev_roulette_battle:
	/*フェードアウト*/
	//_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_multi_battle_2

	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_leader_1st_33
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_leader_2nd_33

	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_BOX_LR
	_JUMP				ev_roulette_battle_2
	_END

ev_roulette_leader_1st_33:
ev_roulette_leader_2nd_33:
	_PARTICLE_SPA_EXIT	SPAWORK_0					/*エンカウントエフェクト前にパーティクル削除*/

	_RECORD_INC			RECID_FRONTIER_BRAIN
	_BRAIN_ENCOUNT_EFFECT	FRONTIER_NO_ROULETTE
	_JUMP				ev_roulette_battle_2
	_END


/*通信マルチ*/
ev_roulette_multi_battle_2:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_BATTLE
	_COMM_RESET

	_TALK_CLOSE
	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_BOX_LR
	_JUMP				ev_roulette_battle_2
	_END


/********************************************************************/
//
/********************************************************************/
ev_roulette_battle_2:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	_CALL				ev_roulette_poke_itemkeep_icon_del			/*ポケモンアイコン削除*/
	_CALL				ev_roulette_poke_itemkeep_icon_resource_del	/*リソース削除*/

	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_battle_2_0
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_battle_2_0

	/*ブレーン以外は、このタイミングでパーティクルを削除する*/
	_PARTICLE_SPA_EXIT	SPAWORK_0
	_JUMP				ev_roulette_battle_2_0
	_END

ev_roulette_battle_2_0:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get

	/*リソース解放(アクターはワーク番号、リソースはACTIDで解放する事に注意)*/
	_ARTICLE_RESOURCE_FREE	ACTID_ROULETTE_RAMP

	/*ポケパネルリソース＋アクター削除*/
	_CALL				ev_pokepanel_del

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	_ROULETTE_BATTLE_CALL
	_ROULETTE_CALL_GET_RESULT
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	/*とりあえず敵のアイコンは表示しない*/
	_CALL			ev_roulette_poke_itemkeep_icon_add_mine			/*(MINE)ポケモンアイコン追加*/
	//_CALL				ev_roulette_poke_itemkeep_icon_add		/*ポケモンアイコン追加*/

	_ROULETTE_TOOL		FR_ID_SET_MAIN_BG,0,0,FSW_ANSWER/*背景変更(内部で通信タイプかチェック)*/

	/*ポケパネルリソース＋アクターセット*/
	_CALL					ev_pokepanel_set

	_CALL				ev_roulette_btl_pal_black					/*暗くする*/
	_CALL				ev_roulette_obj_pal_black					/*OBJ暗くする*/

	/*フェードイン*/
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

#ifndef DEBUG_FRONTIER_LOOP

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*戦闘結果で分岐*/
	_ROULETTE_LOSE_CHECK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_lose		/*敗北*/
#endif	/****************************************************************/

#endif	//DEBUG_FRONTIER_LOOP

	_RECORD_INC			RECID_ROULETTE

	/*ポケモンチェンジイベントの時はアイコンを逆にしてから消す必要があるかも？*/
	/*決定したイベントナンバーを取得*/
	//_ROULETTE_TOOL		FR_ID_GET_DECIDE_EV_NO,0,0,FSW_ANSWER
	//_IFVAL_CALL			FSW_ANSWER,EQ,ROULETTE_EV_PLACE_POKE_CHANGE,ev_btl_after_poke_icon_sub

	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_leader_1st_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_leader_2nd_03

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	_JUMP				ev_roulette_battle_2_sub
	_END

ev_roulette_battle_2_sub:
	_CALL				ev_roulette_battle_2_2								/*退場*/
	_JUMP				ev_roulette_battle_3								/*戦闘勝利後の流れへ*/
	_END


/********************************************************************/
/*								勝利								*/
/********************************************************************/
ev_roulette_battle_2_2:
	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_battle_2_multi

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out
	_OBJ_ANIME			OBJID_MINE,mine_anime_go_out
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_RET

ev_roulette_battle_2_multi:
	_CALL				ev_roulette_comm_command_initialize_3

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out
	_OBJ_ANIME			OBJID_ETC2,etc_anime_go_out_multi
	_OBJ_ANIME			OBJID_SIO_USER_0,mine_anime_go_out
	_OBJ_ANIME			OBJID_SIO_USER_1,mine_anime_go_out
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_ETC
	_ACTOR_FREE			OBJID_ETC2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3
	_RET


/********************************************************************/
/*						戦闘をスルーした時の退場					*/
/********************************************************************/
ev_roulette_battle_btl_off:
	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_battle_2_multi_btl_off

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out_btl_off
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_RET

ev_roulette_battle_2_multi_btl_off:
	_CALL				ev_roulette_comm_command_initialize_4

	/*敵トレーナー退場*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_go_out_btl_off
	_OBJ_ANIME			OBJID_ETC2,etc_anime_go_out_btl_off
	_OBJ_ANIME_WAIT
	_ACTOR_FREE			OBJID_ETC
	_ACTOR_FREE			OBJID_ETC2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3
	_RET


/********************************************************************/
//
/********************************************************************/
ev_roulette_battle_3:
	_JUMP				ev_roulette_battle_4
	_END

ev_roulette_battle_3_multi:
	//_ACTOR_FREE			OBJID_SIO_USER_0
	//_ACTOR_FREE			OBJID_SIO_USER_1

	/*主人公の見た目格納*/
	//_GET_MINE_OBJ		FSW_LOCAL7

	//_CHAR_RESOURCE_SET	default_set_resource_multi	/*キャラクタリソース登録*/
	_JUMP				ev_roulette_battle_4
	_END


/********************************************************************/
//
/********************************************************************/
ev_roulette_battle_4:
	_CALL				ev_roulette_battle_5
	_CALL				ev_roulette_battle_6
	_JUMP				ev_roulette_battle_7
	_END

ev_roulette_battle_5:

#ifdef DEBUG_FRONTIER_LOOP
	_RET
#endif	//DEBUG_FRONTIER_LOOP

	/*今何人目+1*/
	_ROULETTE_TOOL		FR_ID_INC_ROUND,1,0,FSW_ANSWER

	/*連勝数+1*/
	_ROULETTE_TOOL		FR_ID_INC_RENSYOU,0,0,FSW_ANSWER
	_RET

ev_roulette_battle_6:
	/*通信：基本情報やりとり*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_multi_comm_basic_call
	_RET

ev_roulette_battle_7:

#ifndef DEBUG_7BTL_OFF	/********************************************/
	/*今7人目でない時は頭に戻る*/
	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,NE,7,ev_roulette_room_renshou_17		/*連勝数が1-6の時*/
#endif	/************************************************************/

	_JUMP				ev_roulette_room_7_win
	_END


/********************************************************************/
/*							7連勝した(記録)							*/
/********************************************************************/
ev_roulette_room_7_win:

	/*戦闘がないパネルの時は飛ばす*/
	_IFVAL_JUMP			FSW_PARAM4,GE,1,ev_roulette_room_7_win_bp

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_roulette_room_7_win_bp_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_roulette_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_7_win_bp
	_JUMP				ev_roulette_room_7_win_rec_yes
	_END

ev_roulette_room_7_win_bp_sub:
	_TALKMSG_NOSKIP		msg_roulette_room_wait
	_JUMP				ev_roulette_room_7_win_bp
	_END

/*「はい」*/
ev_roulette_room_7_win_rec_yes:
	_CALL				ev_roulette_room_rec_win		/*勝利の記録*/
	_JUMP				ev_roulette_room_7_win_bp
	_END


/********************************************************************/
/*							7連勝した(BP取得)						*/
/********************************************************************/
ev_roulette_room_7_win_bp:
	//7連勝(クリア)パラメータセット
	_ROULETTE_TOOL		FR_ID_SET_CLEAR,0,0,FSW_ANSWER

	/*7連勝したので終了へ*/
	//_LDVAL			WK_SCENE_ROULETTE_LOBBY,1				/*7連勝から始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_ROULETTE_LOBBY,1			/*7連勝から始まるようにしておく*/

	/*「７せん　とっぱ　おめでとう」*/
	_TALKMSG			msg_roulette_room_31

	_PLAYER_NAME		0										/*プレイヤー名セット*/

	/*バトルポイント追加*/
	_ROULETTE_TOOL		FR_ID_GET_BP_POINT,0,0,FSW_ANSWER
	_NUMBER_NAME		1,FSW_ANSWER								/*数値セット*/
	_BTL_POINT_ADD		FSW_ANSWER

	/*「ＢＰをもらった」*/
	_TALKMSG			msg_roulette_room_32
	_ME_PLAY			ME_BP_GET
	_ME_WAIT

	_JUMP				ev_roulette_room_bp_end
	_END


/********************************************************************/
/*							7連勝した終了							*/
/********************************************************************/
ev_roulette_room_bp_end:
	_JUMP				ev_roulette_lose_timing
	_END


/********************************************************************/
/*							連勝数 1-7								*/
/********************************************************************/
ev_roulette_room_renshou_17:

	/*回復シーケンスを飛ばす*/
	_IFVAL_JUMP			FSW_PARAM4,GE,1,ev_roulette_room_renshou_17_sub

	/*回復の人登場*/
	_CALL				ev_roulette_actor_set_btl_kaihuku
	_OBJ_ANIME			OBJID_KAIHUKU,kaihuku_anime_10
	_OBJ_ANIME_WAIT

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_renshou_17_multi

	_OBJ_ANIME			OBJID_MINE,player_kaihuku_anime_10
	_OBJ_ANIME_WAIT
	_JUMP				ev_roulette_room_renshou_17_common
	_END

/*マルチ　回復の方を見る*/
ev_roulette_room_renshou_17_multi:
	_OBJ_ANIME			OBJID_SIO_USER_0,player_kaihuku_anime_10
	_OBJ_ANIME			OBJID_SIO_USER_1,player_kaihuku_anime_10
	_OBJ_ANIME_WAIT
	_JUMP				ev_roulette_room_renshou_17_common
	_END

ev_roulette_room_renshou_17_common:
	/*「ポケモンの状態を治します」*/
	_TALKMSG			msg_roulette_room_3
	_ME_PLAY			SEQ_ASA
	_ME_WAIT
	_TALK_CLOSE

	_PC_KAIFUKU

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_renshou_17_multi2

	/*回復の人去っていく*/
	_OBJ_ANIME			OBJID_KAIHUKU,kaihuku_anime_15
	_OBJ_ANIME			OBJID_MINE,player_kaihuku_anime_15
	_OBJ_ANIME_WAIT
	_JUMP				ev_roulette_room_renshou_17_common2
	_END

ev_roulette_room_renshou_17_multi2:
	_OBJ_ANIME			OBJID_KAIHUKU,kaihuku_anime_15
	_OBJ_ANIME			OBJID_SIO_USER_0,player_kaihuku_anime_15
	_OBJ_ANIME			OBJID_SIO_USER_1,player_kaihuku_anime_15
	_OBJ_ANIME_WAIT
	_JUMP				ev_roulette_room_renshou_17_common2
	_END

ev_roulette_room_renshou_17_common2:
	_ACTOR_FREE			OBJID_KAIHUKU
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	_JUMP				ev_roulette_room_saikai
	_END

ev_roulette_room_renshou_17_sub:
	_JUMP				ev_roulette_room_saikai
	_END


/********************************************************************/
/*						中断セーブからの再開						*/
/********************************************************************/
ev_roulette_room_saikai:
	/*ブレーン登場かチェック*/
	_ROULETTE_TOOL		FR_ID_CHECK_BRAIN,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_leader_1st_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_leader_2nd_01

	_JUMP				ev_roulette_room_saikai_00
	_END

ev_roulette_room_saikai_00:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	_JUMP				ev_roulette_room_saikai_02
	_END

ev_roulette_room_saikai_02:
	/*連勝数をセット*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*「次は　○戦目ですよ」*/
	_ROULETTE_TOOL		FR_ID_GET_ROUND,0,0,FSW_ANSWER
	_ADD_WK				FSW_ANSWER,1
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG			msg_roulette_room_4

	_JUMP			ev_roulette_room_menu_make
	_END


/********************************************************************/
/*							メニュー生成							*/
/********************************************************************/
ev_roulette_room_menu_make:

	_ROULETTE_TOOL	FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_roulette_room_menu_make_comm

	/*戦闘がないパネルの時は「きろくする」メニューなし*/
	_IFVAL_JUMP		FSW_PARAM4,EQ,1,ev_roulette_room_menu_make_03_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_roulette_room_menu_make_04_set

	_JUMP			ev_roulette_room_menu_make_03_set
	_END

/*通信の時のメニュー*/
ev_roulette_room_menu_make_comm:

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_roulette_room_menu_make_01_set

	/*戦闘がないパネルの時は「きろくする」メニューなし*/
	_IFVAL_JUMP		FSW_PARAM4,EQ,1,ev_roulette_room_menu_make_01_set

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,0,ev_roulette_room_menu_make_02_set

	_JUMP			ev_roulette_room_menu_make_01_set
	_END

ev_roulette_room_menu_make_01_set:
	_LDVAL				FSW_ANSWER,1
	_JUMP				ev_roulette_room_menu_make_sub
	_END

ev_roulette_room_menu_make_02_set:
	_LDVAL				FSW_ANSWER,2
	_JUMP				ev_roulette_room_menu_make_sub
	_END

ev_roulette_room_menu_make_03_set:
	_LDVAL				FSW_ANSWER,3
	_JUMP				ev_roulette_room_menu_make_sub
	_END

ev_roulette_room_menu_make_04_set:
	_LDVAL				FSW_ANSWER,4
	_JUMP				ev_roulette_room_menu_make_sub
	_END

/*どのメニュー形式にするか*/
ev_roulette_room_menu_make_sub:
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_menu_make_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_roulette_room_menu_make_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,3,ev_roulette_room_menu_make_03
	_IFVAL_JUMP			FSW_ANSWER,EQ,4,ev_roulette_room_menu_make_04
	_END

ev_roulette_room_menu_make_01:
	_BMPLIST_INIT_EX	24,13,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_JUMP			ev_roulette_room_yasumu_next
	_END

ev_roulette_room_menu_make_02:
	_BMPLIST_INIT_EX	23,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1	/*きろくする*/
	_JUMP			ev_roulette_room_yasumu_next
	_END

ev_roulette_room_menu_make_03:
	_BMPLIST_INIT_EX	24,11,0,0,FSW_PARAM1			/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2	/*やすむ*/
	_JUMP			ev_roulette_room_yasumu_next
	_END

ev_roulette_room_menu_make_04:
	_BMPLIST_INIT_EX	23,9,0,0,FSW_PARAM1				/*Bキャンセル無効*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_01,FSEV_WIN_TALK_MSG_NONE,0	/*つづける*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_02,FSEV_WIN_TALK_MSG_NONE,1	/*きろくする*/
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_03,FSEV_WIN_TALK_MSG_NONE,2	/*やすむ*/
	_JUMP			ev_roulette_room_yasumu_next
	_END

ev_roulette_room_yasumu_next:
	_BMPLIST_MAKE_LIST	msg_roulette_room_choice_04,FSEV_WIN_TALK_MSG_NONE,3	/*リタイア*/
	_BMPLIST_START
	_JUMP			ev_roulette_room_menu_sel
	_END


/********************************************************************/
/*							メニュー結果							*/
/********************************************************************/
ev_roulette_room_menu_sel:

#ifdef DEBUG_FRONTIER_LOOP
	_JUMP			ev_room_choice_01
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
	_LDVAL				FSW_PARAM4,0				/*回復シーケンス判断ワーククリア*/

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_room_choice_01_multi
	_JUMP				ev_room_choice_01_2
	_END

/*通信マルチ*/
ev_room_choice_01_multi:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	_TALKMSG			msg_roulette_room_10
	_JUMP				ev_room_choice_01_multi_retry
	_END

ev_room_choice_01_multi_retry:
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_RETIRE
	_ROULETTE_SEND_BUF	FSW_LOCAL2,0,FSW_ANSWER							/*0=つづける*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_room_choice_01_multi_retry	/*送信失敗は、再送信へ*/

	_ROULETTE_RECV_BUF	FSW_LOCAL2
	_COMM_RESET

	/*相手が選んだメニューを取得*/
	_ROULETTE_TOOL		FR_ID_GET_RETIRE_FLAG,0,0,FSW_ANSWER

	/*パートナーがリタイアしたら*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_giveup_yes_multi	/*リタイア*/

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_MENU
	_COMM_RESET

	_JUMP				ev_room_choice_01_2
	_END

/*パートナーがリタイアしたら*/
ev_roulette_room_giveup_yes_multi:
	/*パートナー名を表示*/
	_PAIR_NAME			0
	_TALKMSG_NOSKIP		msg_roulette_room_12
	_TIME_WAIT			FRONTIER_COMMON_WAIT,FSW_ANSWER
	_JUMP				ev_roulette_room_giveup_yes_multi_sync
	_END


/********************************************************************/
/**/
/********************************************************************/
ev_room_choice_01_2:
	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	/*次の敵ポケモンを生成*/
	_ROULETTE_BTL_AFTER_PARTY_SET		/*バトル後のPOKEPARTYのセット*/

	_TIME_WAIT			1,FSW_ANSWER

	/*通信マルチ*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_multi_room_enemy_poke_send2

	_JUMP				ev_roulette_room_go_rental_2
	_END


/********************************************************************/
/*							記録する*/
/********************************************************************/
ev_room_choice_02:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_roulette_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_roulette_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER										/*いいえデフォルト*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_rec_no
	_JUMP				ev_roulette_room_rec_yes
	_END

/*「はい」*/
ev_roulette_room_rec_yes:
	_CALL				ev_roulette_room_rec_win	/*勝利の記録*/
	_JUMP				ev_roulette_room_saikai		/*メニューへ戻る*/
	_END

/*「いいえ」*/
ev_roulette_room_rec_no:
	_JUMP				ev_roulette_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_roulette_room_rec_win:
	_CALL				ev_roulette_room_rec_common		/*ANSWER,PARAM0使用中*/
	//_ADD_WK			FSW_PARAM0,1
	//_SUB_WK			FSW_PARAM0,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_roulette_room_rec
	_END

/*負け*/
ev_roulette_room_rec_lose:
	_CALL				ev_roulette_room_rec_common		/*ANSWER,PARAM0使用中*/
	_ADD_WK				FSW_PARAM0,1
	_JUMP				ev_roulette_room_rec
	_END

/*共通部分*/
ev_roulette_room_rec_common:
	/*録画データセーブ*/
	_ROULETTE_TOOL		FR_ID_GET_RENSYOU,0,0,FSW_PARAM0
	_RET

ev_roulette_room_rec:
	_JUMP				ev_roulette_room_rec_sub
	_END

ev_roulette_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL3,1

	_TALKMSG_ALL_PUT	msg_roulette_room_6_4
	_ROULETTE_TOOL		FR_ID_GET_TYPE,0,0,FSW_ANSWER
	_ADD_WAITICON
	_BATTLE_REC_SAVE	FRONTIER_NO_ROULETTE,FSW_ANSWER,FSW_PARAM0,FSW_PARAM0
	_DEL_WAITICON

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_PARAM0,EQ,1,ev_roulette_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_roulette_room_6_2
	_RET

ev_roulette_room_rec_true:
	_SE_PLAY			SEQ_SE_DP_SAVE
	/*「記録されました」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_roulette_room_6_1
	_RET

/*録画データがない時メッセージ*/
ev_roulette_room_rec_msg1:
	_TALKMSG			msg_roulette_room_6
	_RET

/*すでに録画データがある時メッセージ*/
ev_roulette_room_rec_msg2:
	_TALKMSG			msg_roulette_room_6_3
	_RET


/********************************************************************/
/*							やすむ*/
/********************************************************************/
ev_room_choice_03:
	/*「レポートをかいて終了しますか？」*/
	_TALKMSG			msg_roulette_room_7
	_YES_NO_WIN			FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_rest_no
	_JUMP				ev_roulette_room_rest_yes
	_END

/*「はい」*/
ev_roulette_room_rest_yes:
	//_LDVAL			WK_SCENE_ROULETTE_LOBBY,2			/*続きから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_ROULETTE_LOBBY,2		/*続きから始まるようにしておく*/

	//プレイデータセーブ
	_ROULETTE_TOOL		FR_ID_SAVE_REST_PLAY_DATA,0,0,FSW_ANSWER

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_ROULETTE_WORK_FREE

	_CALL				ev_roulette_save			/*07.08.24 _CALLに置き換えた*/

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	//リセットコマンド
	_ROULETTE_TOOL		FR_ID_SYSTEM_RESET,0,0,FSW_ANSWER
	_END

/*「いいえ」*/
ev_roulette_room_rest_no:
	_JUMP				ev_roulette_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*							リタイア*/
/********************************************************************/
ev_room_choice_04:
	/*「バトルキャッスルの挑戦を中止する？」*/
	_TALKMSG			msg_roulette_room_8
	_YES_NO_WIN_EX		FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_giveup_no

	/*通信*/
	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_room_giveup_yes_multi_wait_msg

	/*「お預かりしていた道具を　お返しします！」*/
	_TALKMSG			msg_roulette_room_34

	_JUMP				ev_roulette_room_giveup_yes
	_END

/*「しばらくお待ちください」*/
ev_roulette_room_giveup_yes_multi_wait_msg:
	_TALKMSG_NOSKIP		msg_roulette_room_wait
	_JUMP				ev_roulette_room_giveup_yes_multi_retry
	_END

ev_roulette_room_giveup_yes_multi_retry:
	_LDVAL				FSW_LOCAL2,ROULETTE_COMM_RETIRE
	_ROULETTE_SEND_BUF	FSW_LOCAL2,1,FSW_ANSWER									/*1=リタイヤ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_roulette_room_giveup_yes_multi_retry	/*送信失敗は再送信*/

	//_ROULETTE_RECV_BUF	FSW_LOCAL2
	//_COMM_RESET
	_JUMP				ev_roulette_room_giveup_yes_multi_sync
	_END

ev_roulette_room_giveup_yes_multi_sync:
	/*「お預かりしていた道具を　お返しします！」*/
	_TALKMSG			msg_roulette_room_34

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_roulette_room_wait

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_GIVE
	//_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_MENU
	_COMM_RESET
	_JUMP				ev_roulette_room_giveup_yes
	_END

/*「はい」*/
ev_roulette_room_giveup_yes:
	//敗戦パラメータセット
	_ROULETTE_TOOL		FR_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_ROULETTE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_ROULETTE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP				ev_roulette_room_end_save
	_END
	_END

/*「いいえ」*/
ev_roulette_room_giveup_no:
	_JUMP				ev_roulette_room_saikai		/*メニューへ戻る*/
	_END


/********************************************************************/
/*								敗北*/
/********************************************************************/
ev_roulette_lose:
	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,ev_roulette_lose_timing_sub

	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,0,ev_roulette_room_rec_msg1		/*ない時*/
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_roulette_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER								/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_roulette_lose_rec_no	/*「いいえ」*/
	_JUMP			ev_roulette_lose_rec_yes				/*「はい」*/
	_END

ev_roulette_lose_timing_sub:
	_TALKMSG_NOSKIP	msg_roulette_room_wait
	_JUMP			ev_roulette_lose_timing_call
	_END

/*「はい」*/
ev_roulette_lose_rec_yes:
	_CALL			ev_roulette_room_rec_lose				/*敗北の記録*/
	_JUMP			ev_roulette_lose_timing_call
	_END

/*「いいえ」*/
ev_roulette_lose_rec_no:
	_JUMP			ev_roulette_lose_timing_call
	_END

ev_roulette_lose_timing_call:
	//敗戦パラメータセット
	_ROULETTE_TOOL		FR_ID_SET_LOSE,0,0,FSW_ANSWER

	//_LDVAL			WK_SCENE_ROULETTE_LOBBY,3			/*リタイヤから始まるようにしておく*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_ROULETTE_LOBBY,3		/*リタイヤから始まるようにしておく*/

	_JUMP			ev_roulette_lose_timing
	_END

ev_roulette_lose_timing:
	/*「お預かりしていた道具を　お返しします！」*/
	_TALKMSG		msg_roulette_room_34

	_ROULETTE_TOOL	FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_roulette_multi_lose_timing
	_JUMP			ev_roulette_room_end_save
	_END

/*通信同期*/
ev_roulette_multi_lose_timing:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP	msg_roulette_room_wait

	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_LOSE_END
	_COMM_RESET
	_JUMP				ev_roulette_room_end_save
	_END

ev_roulette_room_end_save:
	_CALL				ev_roulette_save			/*07.08.24 _CALLに置き換えた*/
	_TALK_CLOSE
	_JUMP				ev_roulette_room_end
	_END


/********************************************************************/
/*							共通終了								*/
/********************************************************************/
ev_roulette_room_end:

	/*セーブ後に同期させる*/
	_ROULETTE_TOOL	FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL		FSW_ANSWER,EQ,1,ev_roulette_multi_end_timing

	/*フェードアウト*/
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*このタイミングではMINEのアイコンが出ている予定*/
	_CALL				ev_roulette_poke_itemkeep_icon_del_mine	/*MINEポケモンアイコン削除*/
	//_CALL				ev_roulette_poke_itemkeep_icon_del		/*ポケモンアイコン削除*/
	_CALL				ev_roulette_poke_itemkeep_icon_resource_del	/*リソース削除*/

	/*ポケパネルリソース＋アクター削除*/
	_CALL				ev_pokepanel_del

	_ROULETTE_TOOL		FR_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,ROULETTE_TYPE_MULTI,ev_roulette_room_multi_tv

	_ROULETTE_TOOL		FR_ID_GET_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,ROULETTE_TYPE_WIFI_MULTI,ev_roulette_room_end_wifi

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_ROULETTE_WORK_FREE

	/*2Dマップ控え室へ*/
	//

	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*マルチのみ*/
ev_roulette_room_multi_tv:
	/*TV処理*/
	_TV_TEMP_FRIEND_SET	FRONTIER_NO_ROULETTE
	_RET


/********************************************************************/
/*						セーブ後に同期させる						*/	
/********************************************************************/
ev_roulette_multi_end_timing:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_ROULETTE_SAVE_AFTER
	_COMM_RESET			/*画面切り替え前に初期化*/
	_RET


/********************************************************************/
/*						WIFIは待ち合わせ画面へ戻る					*/	
/********************************************************************/
ev_roulette_room_end_wifi:
	/*フェードアウト*/
	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	/*ワーク開放*/
	_BATTLE_REC_EXIT
	_ROULETTE_WORK_FREE

	_MAP_CHANGE_EX		FSS_SCENEID_WIFI_COUNTER,1
	_END


/********************************************************************/
//							共通セーブ	
/********************************************************************/
ev_roulette_save:
	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_roulette_room_36
	_ADD_WAITICON
	_REPORT_DIV_SAVE	FSW_ANSWER
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE
	_RET


/********************************************************************/
/*(MINE)ポケモンアイコン、アイテム持っているアクター追加、リソース追加(CALL)*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_add_mine:
	/*ポケモンアイコン、アイテム持っているリソースセット*/
	_ROULETTE_TOOL		FR_ID_RESOURCE_SET_POKE_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_RESOURCE_SET_ITEMKEEP_ICON,0,0,FSW_ANSWER

	/*ポケモンアイコン、アイテム持っているアクター追加*/
	_ROULETTE_TOOL		FR_ID_ADD_ITEMKEEP_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ADD_POKE_ICON,0,0,FSW_ANSWER
	_RET


/********************************************************************/
/*(ENEMY)ポケモンアイコン、アイテム持っているアクター追加(CALL)		*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_add_enemy:
	/*ポケモンアイコン、アイテム持っているアクター追加*/
	_ROULETTE_TOOL		FR_ID_ADD_ITEMKEEP_ICON,1,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ADD_POKE_ICON,1,0,FSW_ANSWER
	_RET


/********************************************************************/
/*(ALL)ポケモンアイコン、アイテム持っているアクター追加、リソース追加(CALL)*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_add:
	/*ポケモンアイコン、アイテム持っているリソースセット*/
	_ROULETTE_TOOL		FR_ID_RESOURCE_SET_POKE_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_RESOURCE_SET_ITEMKEEP_ICON,0,0,FSW_ANSWER

	/*ポケモンアイコン、アイテム持っているアクター追加*/
	_ROULETTE_TOOL		FR_ID_ADD_ITEMKEEP_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ADD_ITEMKEEP_ICON,1,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ADD_POKE_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_ADD_POKE_ICON,1,0,FSW_ANSWER
	_RET


/********************************************************************/
/*	(ALL)ポケモンアイコン、アイテム持っているアクター削除(CALL)		*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_del:
	/*ポケモンアイコン、アイテム持っているアクター削除*/
	_ROULETTE_TOOL		FR_ID_DEL_ITEMKEEP_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_DEL_ITEMKEEP_ICON,1,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_DEL_POKE_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_DEL_POKE_ICON,1,0,FSW_ANSWER
	_RET


/********************************************************************/
/*	(ALL)ポケモンアイコン、アイテム持っているリソース削除(CALL)		*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_resource_del:
	_ROULETTE_TOOL		FR_ID_RESOURCE_FREE_POKE_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_RESOURCE_FREE_ITEMKEEP_ICON,0,0,FSW_ANSWER
	_RET


/********************************************************************/
/*(MINE)ポケモンアイコン、アイテム持っているアクター削除(CALL)		*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_del_mine:
	/*ポケモンアイコン、アイテム持っているアクター削除*/
	_ROULETTE_TOOL		FR_ID_DEL_ITEMKEEP_ICON,0,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_DEL_POKE_ICON,0,0,FSW_ANSWER
	_RET


/********************************************************************/
/*(ENEMY)ポケモンアイコン、アイテム持っているアクター削除(CALL)		*/	
/********************************************************************/
ev_roulette_poke_itemkeep_icon_del_enemy:
	/*ポケモンアイコン、アイテム持っているアクター削除*/
	_ROULETTE_TOOL		FR_ID_DEL_ITEMKEEP_ICON,1,0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_DEL_POKE_ICON,1,0,FSW_ANSWER
	_RET


/********************************************************************/
/*						ブレーンの流れ								*/
/********************************************************************/
ev_roulette_leader_1st_01:
ev_roulette_leader_2nd_01:

	/*記録したワークにデータが書き込まれていない = 中断復帰として考える*/
	/*登場しますよメッセージは表示しない*/
	_IFVAL_JUMP		FSW_LOCAL3,EQ,1,ev_roulette_room_saikai_00

	/*一度ブレーン登場メッセージを表示したか*/
	_ROULETTE_TOOL	FR_ID_BRAIN_APPEAR_MSG_CHK,0,0,FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_roulette_room_saikai_00

	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*「ブレーンとの勝負になります！覚悟はいいですか？」*/
	_TALKMSG			msg_roulette_room_boss_01
	_JUMP				ev_roulette_room_saikai_02
	_END

ev_roulette_leader_1st_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*登場*/
	_CALL				ev_roulette_leader_appear

	/*戦闘前のメッセージ(1周目)*/
	_TALKMSG			msg_roulette_room_boss_02
	_TALK_CLOSE
	_RET

ev_roulette_leader_2nd_02:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*登場*/
	_CALL				ev_roulette_leader_appear

	/*戦闘前のメッセージ(2周目)*/
	_TALKMSG			msg_roulette_room_boss_03
	_TALK_CLOSE
	_RET

/*登場*/
ev_roulette_leader_appear:

	_CALL				ev_roulette_actor_set_btl_people

	/*観客 中心を見る*/
	_OBJ_ANIME			OBJID_PEOPLE1,people_anime_11			/*左上*/
	_OBJ_ANIME			OBJID_PEOPLE2,people_anime_12			/*左上*/
	_OBJ_ANIME			OBJID_PEOPLE3,people_anime_13
	_OBJ_ANIME			OBJID_PEOPLE4,people_anime_14
	_OBJ_ANIME			OBJID_PEOPLE5,people_anime_15
	//_OBJ_ANIME			OBJID_PEOPLE6,people_anime_16
	_OBJ_ANIME			OBJID_PEOPLE7,people_anime_17
	_OBJ_ANIME			OBJID_PEOPLE8,people_anime_18
	_OBJ_ANIME			OBJID_PEOPLE11,people_anime_11
	_OBJ_ANIME			OBJID_PEOPLE12,people_anime_12
	_OBJ_ANIME			OBJID_PEOPLE13,people_anime_13
	_OBJ_ANIME			OBJID_PEOPLE14,people_anime_13
	_OBJ_ANIME			OBJID_PEOPLE15,people_anime_18_2
	_OBJ_ANIME			OBJID_PEOPLE16,people_anime_11
	_OBJ_ANIME_WAIT

	_CALL				ev_roulette_trainer_set_brain
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain
	_OBJ_ANIME_WAIT

	/*パーティクル追加(ブレーン専用) 08.03.08*/
	_SE_PLAY			SEQ_SE_PL_FR04
	_PARTICLE_SPA_LOAD	SPAWORK_3, ROULETTE_BRAIN_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_3,ROULETTE_BRAIN_BR_MIRROR_YELLOW
	_PARTICLE_ADD_EMITTER	SPAWORK_3,ROULETTE_BRAIN_BR_MIRROR_YELLOWBG
	//_PARTICLE_WAIT

	/*スクロール*/
	_OBJ_ANIME			OBJID_MINE,anm_mine_brain_down
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_brain
	_OBJ_ANIME_WAIT

	/*周り集まる*/
	_OBJ_ANIME			OBJID_PEOPLE1,people_anime_01			/*左上*/
	_OBJ_ANIME			OBJID_PEOPLE2,people_anime_02			/*左上*/
	_OBJ_ANIME			OBJID_PEOPLE3,people_anime_03
	_OBJ_ANIME			OBJID_PEOPLE4,people_anime_04
	_OBJ_ANIME			OBJID_PEOPLE5,people_anime_05
	//_OBJ_ANIME			OBJID_PEOPLE6,people_anime_06
	_OBJ_ANIME			OBJID_PEOPLE7,people_anime_07
	_OBJ_ANIME			OBJID_PEOPLE8,people_anime_08
	_OBJ_ANIME			OBJID_PEOPLE11,people_anime_01
	_OBJ_ANIME			OBJID_PEOPLE12,people_anime_02_2
	_OBJ_ANIME			OBJID_PEOPLE13,people_anime_03
	//_OBJ_ANIME			OBJID_PEOPLE14,people_anime_03		/*動かさない*/
	_OBJ_ANIME			OBJID_PEOPLE15,people_anime_15_5
	_OBJ_ANIME			OBJID_PEOPLE16,people_anime_16_5
	_OBJ_ANIME_WAIT

	_SE_PLAY			SEQ_SE_DP_DENDOU						/*歓声*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_12
	_OBJ_ANIME_WAIT
	_SE_WAIT			SEQ_SE_DP_DENDOU

	/*ブレーンが中央まで歩く*/
	_OBJ_ANIME			OBJID_ETC,etc_anime_room_in_brain_2
	_OBJ_ANIME			OBJID_MINE,anm_mine_brain_down2
	_OBJ_ANIME			OBJID_PLAYER,anm_player_go_center_scroll_brain2
	_OBJ_ANIME			OBJID_PEOPLE1,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE2,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE3,people_anime_up_02
	_OBJ_ANIME			OBJID_PEOPLE4,people_anime_up_03
	_OBJ_ANIME			OBJID_PEOPLE5,people_anime_up_02
	//_OBJ_ANIME			OBJID_PEOPLE6,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE7,people_anime_up_02
	_OBJ_ANIME			OBJID_PEOPLE8,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE11,people_anime_up_03
	_OBJ_ANIME			OBJID_PEOPLE12,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE13,people_anime_up_01
	//_OBJ_ANIME			OBJID_PEOPLE14,people_anime_up_01
	_OBJ_ANIME			OBJID_PEOPLE15,people_anime_up_03
	_OBJ_ANIME			OBJID_PEOPLE16,people_anime_up_01
	_OBJ_ANIME_WAIT

	/*観客削除*/
	_CALL				ev_roulette_people_del

	_CALL				ev_roulette_poke_itemkeep_icon_add_enemy	/*(ENEMY)ポケモンアイコン追加*/
	_RET

ev_roulette_people_del:
	_ACTOR_FREE			OBJID_PEOPLE1
	_ACTOR_FREE			OBJID_PEOPLE2
	_ACTOR_FREE			OBJID_PEOPLE3
	_ACTOR_FREE			OBJID_PEOPLE4
	_ACTOR_FREE			OBJID_PEOPLE5
	//_ACTOR_FREE			OBJID_PEOPLE6
	_ACTOR_FREE			OBJID_PEOPLE7
	_ACTOR_FREE			OBJID_PEOPLE8
	_ACTOR_FREE			OBJID_PEOPLE11
	_ACTOR_FREE			OBJID_PEOPLE12
	_ACTOR_FREE			OBJID_PEOPLE13
	_ACTOR_FREE			OBJID_PEOPLE14
	_ACTOR_FREE			OBJID_PEOPLE15
	_ACTOR_FREE			OBJID_PEOPLE16
	_CHAR_RESOURCE_FREE	MAN1
	_CHAR_RESOURCE_FREE	BIGMAN
	_CHAR_RESOURCE_FREE	MOUNT
	_CHAR_RESOURCE_FREE	IDOL
	//_CHAR_RESOURCE_FREE CAMPBOY
	_RET


/********************************************************************/
/*						ブレーンに勝利後のmsg						*/
/********************************************************************/
ev_roulette_leader_1st_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_ROULETTE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_NONE,ev_roulette_meory_print_put_1st

	/*戦闘後のメッセージ(1周目)*/
	_TALKMSG			msg_roulette_room_boss_04
	_TALK_CLOSE
	_JUMP				ev_roulette_battle_2_sub
	_END

/*受付で1st記念プリントを貰えるようにする*/
ev_roulette_meory_print_put_1st:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_ROULETTE,MEMORY_PRINT_PUT_1ST
	_RET

ev_roulette_leader_2nd_03:
	/*連勝数を取得(FSW_ANSWER使用)*/
	_CALL				ev_roulette_rensyou_get
	_NUMBER_NAME		0,FSW_ANSWER

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_ROULETTE,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_PUT_OK_1ST,ev_roulette_meory_print_put_2nd

	/*戦闘後のメッセージ(2周目)*/
	_TALKMSG			msg_roulette_room_boss_05
	_TALK_CLOSE
	_JUMP				ev_roulette_battle_2_sub
	_END

/*受付で2nd記念プリントを貰えるようにする*/
ev_roulette_meory_print_put_2nd:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_ROULETTE,MEMORY_PRINT_PUT_2ND
	_RET


/********************************************************************/
/*				ポケパネルリソース＋アクターセット					*/
/********************************************************************/
ev_pokepanel_set:
	_ARTICLE_RESOURCE_SET	article_pokepanel_resource

	_ARTICLE_ACTOR_SET		article_pokepanel_single_l_actor
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_4,0		//0番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_5,4		//4番のアニメ実行

	_ARTICLE_ACTOR_SET		article_pokepanel_single_r_actor
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_6,1		//1番のアニメ実行
	_ARTICLE_ACTOR_ANIME_START	ACTWORK_7,5		//5番のアニメ実行

#if 0
	/*半透明設定*/
	_ARTICLE_ACTOR_OBJ_MODE_SET	ACTWORK_4,1
	_ARTICLE_ACTOR_OBJ_MODE_SET	ACTWORK_5,1
	_ARTICLE_ACTOR_OBJ_MODE_SET	ACTWORK_6,1
	_ARTICLE_ACTOR_OBJ_MODE_SET	ACTWORK_7,1
#endif

	_RET

/********************************************************************/
/*				ポケパネルリソース＋アクター削除					*/
/********************************************************************/
ev_pokepanel_del:
	/*ポケパネルアクター削除*/
	_ARTICLE_ACTOR_FREE		ACTWORK_4
	_ARTICLE_ACTOR_FREE		ACTWORK_5
	_ARTICLE_ACTOR_FREE		ACTWORK_6
	_ARTICLE_ACTOR_FREE		ACTWORK_7

	/*リソース解放(アクターはワーク番号、リソースはACTIDで解放する事に注意)*/
	_ARTICLE_RESOURCE_FREE	ACTID_ROULETTE_POKEPANEL
	_RET


/********************************************************************/
/*							司会右を向く							*/
/********************************************************************/
ev_roulette_man_right:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_80
	_OBJ_ANIME_WAIT
	_RET

ev_roulette_man_left:
	_OBJ_ANIME			OBJID_MAN,pcwoman2_anime_90
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*					連勝数を取得(FSW_ANSWER使用)					*/
/********************************************************************/
ev_roulette_rensyou_get:
	_ROULETTE_TOOL		FR_ID_GET_RENSYOU,0,0,FSW_ANSWER

	/*すでに9999の時は、0 オリジンの補正をかけない*/
	_IFVAL_JUMP			FSW_ANSWER,GE,ROULETTE_RENSYOU_MAX,ev_roulette_rensyou_ret

	_ADD_WK				FSW_ANSWER,1
	_RET

ev_roulette_rensyou_ret:
	_RET


/********************************************************************/
/*						パレット操作(暗くする)						*/
/********************************************************************/
ev_roulette_rouka_pal_black:
	_ROULETTE_TOOL		FR_ID_EFF_PAL_CHG,8,0,FSW_ANSWER
	_RET

ev_roulette_btl_pal_black:
	_ROULETTE_TOOL		FR_ID_EFF_PAL_CHG,8,1,FSW_ANSWER
	_RET

/********************************************************************/
/*						パレット操作(元に戻す)						*/
/********************************************************************/
ev_roulette_btl_pal_white:
	_ROULETTE_TOOL		FR_ID_EFF_PAL_CHG,0,1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,0,0,FSW_ANSWER	/*OBJ*/
	_RET


/********************************************************************/
/*ルーレット画面、戦闘画面から復帰した時にOBJパレット操作(暗くする)	*/
//
//	司会
//	対戦トレーナー(パートナー)
//	主人公(パートナー)
/********************************************************************/
ev_roulette_obj_pal_black:
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MAN,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_ETC,FSW_ANSWER

	_ROULETTE_TOOL		FR_ID_CHECK_COMM_TYPE,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_roulette_obj_pal_black_multi

	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MINE,FSW_ANSWER
	_RET

ev_roulette_obj_pal_black_multi:
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_ETC2,FSW_ANSWER
	_RET


/********************************************************************/
/*			リソース登録とアクター登録とパレット操作				*/
/********************************************************************/
ev_roulette_actor_set_way:
	_CHAR_RESOURCE_SET	default_set_resource			/*キャラクタリソース登録*/
	_ACTOR_SET			way_set_actor					/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PCWOMAN2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MINE,FSW_ANSWER
	_RET

ev_roulette_actor_set_way_multi:
	_CHAR_RESOURCE_SET	default_set_resource_multi		/*キャラクタリソース登録*/
	_ACTOR_SET			way_set_actor_multi				/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PCWOMAN2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_1,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl:
	_CHAR_RESOURCE_SET	default_scene2_resource			/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor			/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MINE,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MAN,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_multi:
	_CHAR_RESOURCE_SET	default_scene2_resource_multi	/*キャラクタリソース登録*/
	_ACTOR_SET			default_scene2_actor_multi		/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_MAN,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_0,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_SIO_USER_1,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_pcwoman:
	_CHAR_RESOURCE_SET	pcwoman2_scene2_resource		/*キャラクタリソース登録*/
	_ACTOR_SET			pcwoman2_scene2_actor			/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PCWOMAN2,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_people:
	_CHAR_RESOURCE_SET	people_scene_resource			/*観客*/
	_ACTOR_SET			default_scene3_actor			/*観客アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE1,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE2,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE3,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE4,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE5,FSW_ANSWER
	//_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE6,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE7,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE8,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE11,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE12,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE13,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE14,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE15,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_PEOPLE16,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_tr:
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET			etc_set_actor
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_ETC,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_tr_multi:
	_CHAR_RESOURCE_SET	etc_set_resource2
	_ACTOR_SET			etc_set_actor2
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_ETC,FSW_ANSWER
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_ETC2,FSW_ANSWER
	_RET

ev_roulette_actor_set_btl_kaihuku:
	_CHAR_RESOURCE_SET	kaihuku_set_resource			/*キャラクタリソース登録*/
	_ACTOR_SET			kaihuku_set_actor				/*アクター登録*/
	_ROULETTE_TOOL		FR_ID_OBJ_PAL_CHG,8,OBJID_KAIHUKU,FSW_ANSWER
	_RET


