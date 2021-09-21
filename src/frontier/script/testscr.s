//==============================================================================
/**
 * @file	testscr.s
 * @brief	簡単な説明を書く
 * @author	matsuda
 * @date	2007.03.30(金)
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../../../include/msgdata/msg_cmsg_acting.h"
	.include	"../../particledata/pl_frontier/frontier_particle_def.h"	//SPA
	.include	"../../particledata/pl_frontier/frontier_particle_lst.h"	//EMIT

//--------------------------------------------------------------------
//					     スクリプト本体
//--------------------------------------------------------------------
_EVENT_DATA		test_scr_obj3		//一番上のEVENT_DATAは自動実行
_EVENT_DATA		test_scr_obj1		//SCRID_TEST_SCR_OBJ1
_EVENT_DATA		test_scr_obj2		//SCRID_TEST_SCR_OBJ2
_EVENT_DATA_END						//終了

#define EVENTID_TEST_SCR_OBJ3		(0)
#define EVENTID_TEST_SCR_OBJ1		(1)
#define EVENTID_TEST_SCR_OBJ2		(2)

//--------------------------------------------------------------
//	リソースラベル(画面IN時に常駐させるリソース群)
//--------------------------------------------------------------
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA	//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		MAN3, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		GIRL1, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------
//	状況によって変えるリソース
// LOCAL0 = OBJCODE
//--------------------------------------------------------------
_RESOURCE_LABEL	etc_set_resource
	_CHAR_RESOURCE_DATA		FSW_LOCAL0, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------
//	アクター(画面IN時に常駐させるアクター群)
//--------------------------------------------------------------
#define OBJID_PLAYER	(0)
#define OBJID_MAN		(1)
#define OBJID_GIRL		(2)
#define OBJID_ETC		(3)
_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA	OBJID_PLAYER, WF2DMAP_WAY_C_DOWN, 8*16, 8*5, ON	//自分自身のアクターセット
	_ACTOR_DATA			OBJID_MAN, MAN3, WF2DMAP_WAY_C_DOWN, \
							8*16, 8*7, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_GIRL, GIRL1, WF2DMAP_WAY_C_LEFT, \
							8*20, 8*14, ON, EVENTID_TEST_SCR_OBJ2
	_ACTOR_DATA_END

//--------------------------------------------------------------
//	状況によって変えるアクター
// LOCAL0 = OBJCODE
//--------------------------------------------------------------
_ACTOR_LABEL	etc_set_actor
	_ACTOR_DATA			OBJID_ETC, FSW_LOCAL0, WF2DMAP_WAY_C_DOWN, \
							8*16, 8*15, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------
//	アニメーションデータ
//--------------------------------------------------------------
_ANIME_LABEL anm_player_walk_test
	_ANIME_DATA	FC_WAIT_16F,4
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL anm_girl_walk_test
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_R_8F,2
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL etc_anime_move
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA_END


//==============================================================================
//	SCENE2(マップ切り替え後)のリソースデータやアクターデータ
//==============================================================================
//--------------------------------------------------------------
//	SCENE2:リソースラベル(画面IN時に常駐させるリソース群)
//--------------------------------------------------------------
_RESOURCE_LABEL	default_scene2_resource
	_PLAYER_RESOURCE_DATA	//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		CAMPBOY, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		WAITRESS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		REPORTER, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		TOWERBOSS, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA		SEVEN5, WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------
//	SCENE2:アクター(画面IN時に常駐させるアクター群)
//--------------------------------------------------------------
#define OBJID_PLAYER	(0)
#define OBJID_ENEMY1	(1)
#define OBJID_ENEMY2	(2)
#define OBJID_ENEMY3	(3)
#define OBJID_ENEMY4	(4)
#define OBJID_ENEMY5	(5)
#define OBJID_ENEMY6	(6)
_ACTOR_LABEL	default_scene2_actor
	_PLAYER_ACTOR_DATA	OBJID_PLAYER, WF2DMAP_WAY_C_DOWN, 8*16, 8*5, ON	//自分自身のアクターセット
	_ACTOR_DATA			OBJID_ENEMY1, CAMPBOY, WF2DMAP_WAY_C_DOWN, \
							8*16, 8*10, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ENEMY2, WAITER, WF2DMAP_WAY_C_DOWN, \
							8*12, 8*10, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ENEMY3, WAITRESS, WF2DMAP_WAY_C_DOWN, \
							8*20, 8*10, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ENEMY4, REPORTER, WF2DMAP_WAY_C_DOWN, \
							8*10, 8*10, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ENEMY5, TOWERBOSS, WF2DMAP_WAY_C_DOWN, \
							8*14, 8*3, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA			OBJID_ENEMY6, SEVEN5, WF2DMAP_WAY_C_DOWN, \
							8*18, 8*3, ON, EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//--------------------------------------------------------------
//	SCENE2:アニメーションデータ
//--------------------------------------------------------------
_ANIME_LABEL scene2_anime_move_lr
	_ANIME_DATA	FC_WAIT_16F,4
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA_END

_ANIME_LABEL scene2_anime_move_ud
	_ANIME_DATA	FC_WAIT_16F,2
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA_END


//==============================================================================
//	
//==============================================================================
test_scr_obj1:
	_DUMMY
	_TIME_WAIT		10, FSW_LOCAL0
	_DUMMY
	_DUMMY
	_END

test_scr_obj2:
	_DUMMY
	_DUMMY
	_END

test_back:
	_TALK_CLOSE
	
test_scr_obj3:
//	_MAP_CHANGE_EX	FSS_SCENEID_TESTSCR3, 0

	_CHAR_RESOURCE_SET		default_set_resource		//キャラクタリソース登録
	_ACTOR_SET				default_set_actor			//アクター登録

	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	
	_OBJ_ANIME				OBJID_PLAYER, anm_player_walk_test
	_OBJ_ANIME				OBJID_GIRL, anm_girl_walk_test
	_OBJ_ANIME_WAIT
	
	_OBJMOVE_KYORO		OBJID_MAN, FC_DIR_D, 0, 0
	
	_DUMMY
	_TALKMSG msg_con_act_host_01

#if 0
	_PARTICLE_SPA_LOAD		SPAWORK_0, TEST_001_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_SPA_LOAD		SPAWORK_1, TEST_002_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_ADD_EMITTER	SPAWORK_0, TEST_001_000_EC001_1A
	_TIME_WAIT				30, FSW_LOCAL0
	_PARTICLE_ADD_EMITTER	SPAWORK_1, TEST_002_000_EC001_1C
	_PARTICLE_WAIT
	_PARTICLE_ADD_EMITTER	SPAWORK_0, TEST_001_000_EC001_1B
	_PARTICLE_WAIT
	_PARTICLE_SPA_EXIT		SPAWORK_0
#else
	_PARTICLE_SPA_LOAD		SPAWORK_0, STAGE_SPA, FSS_CAMERA_ORTHO
	_PARTICLE_SPA_LOAD		SPAWORK_1, ROULETTE_SPA, FSS_CAMERA_ORTHO

	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_KAMITAPE_L
	_TIME_WAIT				30, FSW_LOCAL0

	_PARTICLE_ADD_EMITTER	SPAWORK_1, ROULETTE_BR_KEMURI01_L
	_PARTICLE_WAIT

	_PARTICLE_ADD_EMITTER	SPAWORK_0, STAGE_BS_PASYA_01
	_PARTICLE_WAIT

	_PARTICLE_SPA_EXIT		SPAWORK_0
#endif

	_BMPMENU_INIT_EX	1,1,0,1,FSW_ANSWER			//Bキャンセル有効
	_BMPMENU_MAKE_LIST	CAMSG_CONTYPE_STYLE,0			//さんかする
	_BMPMENU_MAKE_LIST	CAMSG_CONTYPE_BEAUTIFUL,1			//せつめいをきく
	_BMPMENU_MAKE_LIST	CAMSG_CONTYPE_CUTE,2			//やめる
	_BMPMENU_MAKE_LIST	CAMSG_CONTYPE_STRONG,3			//やめる
	_BMPMENU_START

	_SWITCH			FSW_ANSWER
	_CASE_JUMP		0,test_jump0
	_CASE_JUMP		1,test_jump1
	_CASE_JUMP		2,test_jump2
	_CASE_JUMP		3,test_jump3
	_JUMP			test_back

test_jump0:
	_TALKMSG msg_con_a_explain_01
	_JUMP			test_talk_end

test_jump1:
	_TALKMSG msg_con_a_explain_02
	_YES_NO_WIN		FSW_ANSWER
	_IFVAL_JUMP		FSW_ANSWER,EQ,0,test_jump1_yes
	_JUMP			test_talk_end

test_jump1_yes:
	_TALK_CLOSE
	
	//-- マップ切り替え --//
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

//	_MAP_CHANGE		FSS_SCENEID_TESTSCR2
	_MAP_CHANGE_EX	FSS_SCENEID_TESTSCR3, 0
//	_MAP_CHANGE_EX	FSS_SCENEID_TESTSCR3, 1
//	_MAP_CHANGE_EX	FSS_SCENEID_TESTSCR3, 2

	_CHAR_RESOURCE_SET		default_scene2_resource		//キャラクタリソース登録
	_ACTOR_SET				default_scene2_actor			//アクター登録
	
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_OBJ_ANIME				OBJID_ENEMY1, scene2_anime_move_lr
	_OBJ_ANIME				OBJID_ENEMY2, scene2_anime_move_lr
	_OBJ_ANIME				OBJID_ENEMY3, scene2_anime_move_ud
	_OBJ_ANIME				OBJID_ENEMY4, scene2_anime_move_ud
	_OBJ_ANIME				OBJID_ENEMY5, scene2_anime_move_lr
	_OBJ_ANIME				OBJID_ENEMY6, scene2_anime_move_ud
	_OBJ_ANIME_WAIT
	
	_TALKMSG 		CAMSG_TALK_VOLTAGE_STATE_UP_4
	_JUMP			test_talk_end
	
test_jump2:
	_TALK_CLOSE
	
	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	
	_NAMEIN_PROC
	
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	
	_TALKMSG msg_con_a_prac_apreface
	_JUMP			test_talk_end


//--------------------------------------------------------------
//	OBJ切り替えテスト
//--------------------------------------------------------------
test_jump3:
	_TALK_CLOSE
	
	_LDVAL			FSW_LOCAL0, GIRL2
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_OBJ_ANIME		OBJID_ETC, etc_anime_move
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	GIRL2

	_TIME_WAIT		5, FSW_LOCAL3

	_LDVAL			FSW_LOCAL0, WOMAN1
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_OBJ_ANIME		OBJID_ETC, etc_anime_move
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_LOCAL0

	_TIME_WAIT		5, FSW_LOCAL3

	_LDVAL			FSW_LOCAL0, BIGMAN
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_OBJ_ANIME		OBJID_ETC, etc_anime_move
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_LOCAL0

	_TIME_WAIT		5, FSW_LOCAL3

	_LDVAL			FSW_LOCAL0, FIGHTER
	_CHAR_RESOURCE_SET	etc_set_resource
	_ACTOR_SET		etc_set_actor
	_OBJ_ANIME		OBJID_ETC, etc_anime_move
	_OBJ_ANIME_WAIT
	_ACTOR_FREE		OBJID_ETC
	_CHAR_RESOURCE_FREE	FSW_LOCAL0
	
	_JUMP			test_jump0


test_talk_end:
	_TALK_CLOSE

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	//_END
	_SCRIPT_FINISH






//==============================================================================
//	SCENE3(マップ切り替え後、疑似通信時の場面を想定した画面)のリソースデータやアクターデータ
//==============================================================================
//--------------------------------------------------------------
//	SCENE3:リソースラベル(画面IN時に常駐させるリソース群)
//--------------------------------------------------------------
_RESOURCE_LABEL	sio_set_resource
	_PLAYER_RESOURCE_DATA	//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA_END

//--------------------------------------------------------------
//	SCENE3:アクター(画面IN時に常駐させるアクター群)
//--------------------------------------------------------------
#define OBJID_SIO_PLAYER		(0)
#define OBJID_SIO_USER_0		(1)
#define OBJID_SIO_USER_1		(2)
_ACTOR_LABEL	sio_set_actor
	_PLAYER_ACTOR_DATA	OBJID_SIO_PLAYER, WF2DMAP_WAY_C_DOWN, 8*16, 8*5, OFF	//自分自身は非表示
	_SIO_PLAYER_ACTOR_DATA 0, OBJID_SIO_USER_0, WF2DMAP_WAY_C_DOWN, 8*16, 8*5, ON
	_SIO_PLAYER_ACTOR_DATA 1, OBJID_SIO_USER_1, WF2DMAP_WAY_C_DOWN, 8*16, 8*5, ON
	_ACTOR_DATA_END

//--------------------------------------------------------------
//	SCENE2:アニメーションデータ
//--------------------------------------------------------------
_ANIME_LABEL scene3_anime_move_left
	_ANIME_DATA	FC_WAIT_16F,4
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WAIT_16F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

_ANIME_LABEL scene3_anime_move_right
	_ANIME_DATA	FC_WAIT_16F,4
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WAIT_16F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

//==============================================================================
//	
//==============================================================================
sio_scene_start:
	_CHAR_RESOURCE_SET		sio_set_resource		//キャラクタリソース登録
	_ACTOR_SET				sio_set_actor			//アクター登録

	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	
	_OBJ_ANIME				OBJID_SIO_USER_0, scene3_anime_move_left
	_OBJ_ANIME				OBJID_SIO_USER_1, scene3_anime_move_right
	_OBJ_ANIME_WAIT
	_ACTOR_VISIBLE_SET		OBJID_SIO_USER_0, OFF
	_ACTOR_VISIBLE_SET		OBJID_SIO_USER_1, OFF
	
	_TIME_WAIT		30, FSW_LOCAL3

	_BLACK_OUT		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	//_END
	_SCRIPT_FINISH

