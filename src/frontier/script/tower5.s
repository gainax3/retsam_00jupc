//==============================================================================
/**
 * @file	tower5.s
 * @brief	「バトルタワーシングル対戦部屋」フロンティアスクリプト(d31r0205,sp_d31r0205)
 * @author	nohara
 * @date	2007.05.30
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../frontier_def.h"
	.include	"../../../include/msgdata/msg_d31r0205.h"
	.include	"../../../include/msgdata/msg_ev_win.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/syswork_def.h"
	.include	"../../field/b_tower_scr_def.h"


//--------------------------------------------------------------------
//
//					     スクリプト本体
//
//	FSW_LOCAL1	WK_SCENE_D31R0205を格納
//	FSW_LOCAL3	記録したか
//
//	FSW_PARAM0	主人公の見た目格納
//	FSW_PARAM2	敵トレーナーのOBJコード
//	FSW_PARAM3	SCWK_TEMP3を変更(次のスクリプトでも引き継いでいるかもしれないので注意！)
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_tower_single_btl_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END								//終了

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_BTL_OFF					//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック
//#define DEBUG_LEADER				//リーダーとバトル(21人目、49人目にnow_roundを変更しないとダメ)


//--------------------------------------------------------------------
//							座標定義
//--------------------------------------------------------------------
#define PLAYER_X		(8*15)//(8*16)
#define PLAYER_Y		(8*14)//(8*24)
#define PCWOMAN2_X		(8*6)//(8*16)
#define PCWOMAN2_Y		(8*13)//(8*10)
#define ENEMY_X			(8*16)//(8*16)
#define ENEMY_Y			(8*7)//(8*6)
#define MINE_X			(8*16)//(8*16)
#define MINE_Y			(8*21)//(8*24)


/********************************************************************/
//
//SCENE_TOWER_SINGLE_BTL:リソースラベル(シングルバトル時に常駐させるリソース群)
//
/********************************************************************/
_RESOURCE_LABEL	single_btl_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA			FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

/*敵トレーナーの決定タイミングが途中なので分けた*/
_RESOURCE_LABEL	enemy_set_resource
	_CHAR_RESOURCE_DATA			FSW_PARAM2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_TOWER_SINGLE_BTL:アクター(シングルバトル時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_PCWOMAN2				(1)
#define OBJID_ENEMY					(2)
#define OBJID_MINE					(3)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

_ACTOR_LABEL	single_btl_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_UP, \
							PLAYER_X,PLAYER_Y,OFF	/*自分自身のアクターセット*/
	_ACTOR_DATA				OBJID_PCWOMAN2,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							PCWOMAN2_X,PCWOMAN2_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_MINE,FSW_PARAM0,WF2DMAP_WAY_C_UP, \
							MINE_X,MINE_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

/*敵トレーナーの決定タイミングが途中なので分けた*/
_ACTOR_LABEL	enemy_set_actor
	_ACTOR_DATA				OBJID_ENEMY,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							ENEMY_X,ENEMY_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


/********************************************************************/
/*							アニメ									*/
/********************************************************************/

//--------------------------------------------------------------------
//自機　初期位置へ移動
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_player_01
	_ANIME_DATA	FC_WAIT_4F,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー　初期位置へ移動
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_enemy_01
	//_ANIME_DATA	FC_WALK_D_8F,1
	//_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA	FC_WALK_R_8F,3
	//_ANIME_DATA	FC_DIR_D,1
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機　対戦前に一歩前へ
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_player_02
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー　対戦前に一歩前へ
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_enemy_02
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー　敗戦後退場
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_enemy_03
	//_ANIME_DATA	FC_DIR_D,1
	_ANIME_DATA	FC_WALK_R_8F,1
	//_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA	FC_WALK_U_8F,2
	//_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA	FC_WALK_L_8F,3
	//_ANIME_DATA	FC_DIR_U,1
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機　対戦勝利後　一歩引き振り向き
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_player_03
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//案内嬢　対戦勝利後　近づき
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_pcwoman2_02
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機　回復後振り向き
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_player_04
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//案内嬢　回復後下がる
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_pcwoman2_03
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END
	
//--------------------------------------------------------------------
//自機　続きから
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_player_05
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//リーダー入場　初期位置へ移動
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0205_leader_01
	_ANIME_DATA	FC_WALK_R_16F,3
	_ANIME_DATA	FC_WALK_D_16F,3
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END


/********************************************************************/
/*							開始									*/
/********************************************************************/
fss_tower_single_btl_start:

	/*ワーク確保*/
	_BATTLE_REC_INIT

	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0205,FSW_LOCAL1

	/*FLAG_CHANGE*/
	_CALL				ev_d31r0205_flag_change

	/*OBJ_CHANGE*/
	_CALL				ev_d31r0205_obj_change

	/*右側のWIFIゲート*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI,comm_tower_5_char_set_wifi
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_RETRY,comm_tower_5_char_set_wifi
	_JUMP				comm_tower_5_char_set_single
	_END

fss_tower_single_btl_start_sub:
	_CHAR_RESOURCE_SET	single_btl_set_resource					/*キャラクタリソース登録*/
	_ACTOR_SET			single_btl_set_actor					/*アクター登録*/

	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/********************/
	//SCENE_CHANGE_LABEL
	/********************/
	_IFVAL_JUMP			FSW_LOCAL1,EQ,1,ev_tower_rooms_common_entry		/*最初から*/
	_IFVAL_JUMP			FSW_LOCAL1,EQ,2,ev_tower_rooms_common_continue	/*続きから*/
	_END

comm_tower_5_char_set_wifi:
	_LDVAL				FSW_PARAM6,WIFISW
	_JUMP				fss_tower_single_btl_start_sub
	_END

comm_tower_5_char_set_single:
	_LDVAL				FSW_PARAM6,BFSM
	_JUMP				fss_tower_single_btl_start_sub
	_END


/********************************************************************/
/*						FLAG_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0205_flag_change:
	_IFVAL_JUMP			FSW_LOCAL1,EQ,0,ev_d31r0205_flag_change_end

	//_FLAG_SET			FV_D31R0205_TRAINER

	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM0
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_LOCAL0
	//_LDWK				OBJCHRWORK0,FSW_LOCAL0
	_RET

ev_d31r0205_flag_change_end:
	_RET


/********************************************************************/
/*						OBJ_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0205_obj_change:
	//_OBJ_INVISIBLE	FLDOBJ_ID_PLAYER
	_IFVAL_JUMP			FSW_LOCAL1,EQ,3,ev_d31r0205_obj_pos_change
	_RET

ev_d31r0205_obj_pos_change:
	//_OBJ_POS_CHANGE	D31R0205_HERO,5,0,6,DIR_RIGHT
	_RET


/********************************************************************/
/*						初めて部屋に入場した時 						*/
/********************************************************************/
ev_tower_rooms_common_entry:
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0205,3
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0205,FSW_LOCAL1

	/*エラーチェック*/
	_BTOWER_TOOLS		BTWR_DEB_IS_WORK_NULL,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_rooms_common_exit

	_CALL				ev_tower_rooms_comanm_in
	_JUMP				ev_tower_rooms_sbtl_01
	_END


/********************************************************************/
/*						シングル対戦フロー	 						*/
/********************************************************************/
ev_tower_rooms_sbtl_01:
	_CALL				ev_tower_rooms_common_sbtl_call

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*負けて戻るときの処理*/
	_IFVAL_JUMP			FSW_PARAM3,EQ,0,ev_tower_rooms_common_lose
#endif	/****************************************************************/

	/*勝った時の処理(クリアしてるかどうかをFSW_ANSWERにかえす)*/
	_CALL				ev_tower_rooms_common_win_param
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_rooms_common_clear_ret

	//エネミー退場
	_CALL				ev_tower_rooms_comanm_eneout

	//プレイヤー振り向き
	_CALL				ev_tower_rooms_comanm_sbtl_end
	
	//連戦処理

	//ポケモン回復
	_CALL				ev_tower_rooms_common_heal

	//バトルリーダー戦チェック
	_JUMP				ev_tower_rooms_if_leader_battle
	_END

//負けて戻るときの処理
ev_tower_rooms_common_lose:
	_JUMP				ev_tower_rooms_common_rec_lose
	_END

//クリアして戻るときの処理
ev_tower_rooms_common_clear_ret:
	_JUMP				ev_tower_rooms_common_rec_win
	_END

//リーダーに勝利して戻るときの処理
ev_tower_rooms_leader_clear_ret:
	_JUMP				ev_tower_rooms_leader_rec_win
	_END


/********************************************************************/
/*				「さきほどの　戦いを　記録しますか？」				*/
/********************************************************************/
/*リーダーに勝ち*/
ev_tower_rooms_leader_rec_win:
	/*「記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower5_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_win_rec_yes		/*「はい」*/

	//クリアパラメータセット
	_BTOWER_TOOLS		BTWR_SUB_SET_CLEAR_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

#if 1
	/*「みごとタワータイクーンに勝利されましたね」*/
	_TALKMSG			msg_tower_101_02

	/*「プレイヤーはＢＰをもらった」*/
	_CALL				ev_tower5_battle_point_get
#endif

	_TALK_CLOSE
	_JUMP				ev_tower_rooms_common_exit
	_END

/*勝ち*/
ev_tower_rooms_common_rec_win:
	/*「記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower5_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_win_rec_yes		/*「はい」*/

	//クリアパラメータセット(セーブ後に処理する)
	_BTOWER_TOOLS		BTWR_SUB_SET_CLEAR_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

#if 1
	/*BTWR_MODE_RETRYは何もしない*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_RETRY,comm_tower_rooms_common_rec_win_retry
#endif

#if 1
	/*「７戦突破おめでとう」*/
	_TALKMSG			msg_tower_101_01

	/*「プレイヤーはＢＰをもらった」*/
	_CALL				ev_tower5_battle_point_get
#endif

	_TALK_CLOSE
	_JUMP				ev_tower_rooms_common_exit
	_END

/*wifiDLの「まえのへやにちょうせん」BPはあげない*/
comm_tower_rooms_common_rec_win_retry:
	/*「７戦突破おめでとう」*/
	//_TALKMSG			msg_tower_101_05				/*外で同じこというのでいらない*/
	_TALK_CLOSE
	_JUMP				ev_tower_rooms_common_exit
	_END

/*負け*/
ev_tower_rooms_common_rec_lose:
	/*「記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower5_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_lose_rec_yes		/*「はい」*/
	_TALK_CLOSE

	//敗戦パラメータセット(セーブ後に処理する)
	_BTOWER_TOOLS		BTWR_SUB_SET_LOSE_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

	_JUMP				ev_tower_rooms_common_exit
	_END

/*「はい」*/
ev_tower5_lose_rec_yes:
	_CALL				ev_tower5_room_rec_lose		/*敗北の記録*/
	_RET

ev_tower5_win_rec_yes:
	_CALL				ev_tower5_room_rec_win		/*勝利の記録*/
	_RET

/*「プレイヤーはＢＰをもらった」*/
ev_tower5_battle_point_get:
	_BTOWER_TOOLS		BTWR_SUB_ADD_BATTLE_POINT,BTWR_NULL_PARAM,FSW_ANSWER
	_NUMBER_NAME		1,FSW_ANSWER
	_PLAYER_NAME		0
	_TALKMSG			msg_tower_101_03
	_ME_PLAY			ME_BP_GET
	_ME_WAIT
	
	/*レポートにかきこんでいます*/
	//_TALKMSG			msg_tower_101_04
	_RET


/********************************************************************/
/*				プレイヤー勝利後　エネミー退場						*/
/********************************************************************/
ev_tower_rooms_comanm_eneout:
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0205_enemy_03
	_OBJ_ANIME_WAIT

	/*対戦トレーナー削除*/
	_ACTOR_FREE			OBJID_ENEMY
	_CHAR_RESOURCE_FREE	FSW_PARAM2

	//バニッシュフラグON
	//_FLAG_SET			FV_OBJID_ETC
	//_OBJ_DEL			OBJID_ENEMY
	_RET


/********************************************************************/
/*			プレイヤー勝利後　プレイヤー振り向き&案内嬢近づき		*/
/********************************************************************/
ev_tower_rooms_comanm_sbtl_end:
	_OBJ_ANIME			OBJID_MINE,anm_d31r0205_player_03
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0205_pcwoman2_02
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*							回復処理								*/
/********************************************************************/
ev_tower_rooms_common_heal:
	_TALKMSG			msg_tower_65
	_ME_PLAY			SEQ_ASA
	_ME_WAIT
	_PC_KAIFUKU
	_RET


/********************************************************************/
/*			シングルバトル勝利時パラメータセット					*/
/********************************************************************/
ev_tower_rooms_common_win_param:
	//ラウンド数更新
	_BTOWER_TOOLS		BTWR_SUB_INC_ROUND,BTWR_NULL_PARAM,FSW_ANSWER

	//クリアしてるかチェック
	_BTOWER_TOOLS		BTWR_SUB_IS_CLEAR,BTWR_NULL_PARAM,FSW_ANSWER
	_RET


/********************************************************************/
/*					続きからで部屋に入場した時						*/
/********************************************************************/
ev_tower_rooms_common_continue:
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0205,3
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0205,FSW_LOCAL1
	
	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL				FSW_LOCAL3,1

	//エラー処理
	_BTOWER_TOOLS		BTWR_DEB_IS_WORK_NULL,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_rooms_common_exit

	_CALL				ev_tower_rooms_comanm_in
	_CALL				ev_tower_rooms_comanm_continue

	//次は？人目の……
	_JUMP				ev_tower_rooms_if_leader_battle
	_END

//自機と受付嬢　入場
ev_tower_rooms_comanm_in:
	_OBJ_ANIME			OBJID_MINE,anm_d31r0205_player_01
	_OBJ_ANIME_WAIT
	_RET

//自機と受付嬢　続きから始めたときの入場後
ev_tower_rooms_comanm_continue:
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0205_pcwoman2_02
	_OBJ_ANIME			OBJID_MINE,anm_d31r0205_player_05
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*					部屋から退場して受付に戻る						*/
/********************************************************************/
ev_tower_rooms_common_exit:
	/*ワーク開放*/
	_BATTLE_REC_EXIT

	//ウェイト
	_TIME_WAIT			30,FSW_ANSWER

	//受付戻りフラグセット
	//_LDVAL				WK_SCENE_D31R0201,1
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0201,1

	/*現在のプレイモードを取得*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER

	//入ってきたゲート位置へ戻る
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			BTWR_MODE_WIFI,ev_tower_rooms_comexit_01
	_CASE_JUMP			BTWR_MODE_RETRY,ev_tower_rooms_comexit_01

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_MINE
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6

	//_MAP_CHANGE			ZONE_ID_D31R0201,0,11,6,DIR_UP
	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

ev_tower_rooms_comexit_01:
	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_MINE
	_ACTOR_FREE			OBJID_PCWOMAN2
	_CHAR_RESOURCE_FREE	FSW_PARAM6

	//_MAP_CHANGE			ZONE_ID_D31R0201,0,15,6,DIR_UP
	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/


/********************************************************************/
/*							テスト？								*/
/********************************************************************/
ev_tower_rooms_test_entry:
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0205,0
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0205,FSW_LOCAL1
	_CALL				ev_tower_rooms_common_sbtl_call
	_END


/********************************************************************/
/*					シングルバトル呼び出し							*/
/********************************************************************/
ev_tower_rooms_common_sbtl_call:
	/*対戦トレーナー抽選*/
	_BTOWER_TOOLS		BTWR_SUB_CHOICE_BTL_PARTNER,BTWR_NULL_PARAM,FSW_ANSWER

	/*エネミー入場*/
	_CALL				ev_tower_rooms_comanm_enein

	/*対戦前台詞*/
	_TALKMSG_BTOWER_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE

	/*互いに近づく*/
	_CALL				ev_tower_rooms_comanm_sbtl_start

	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK
	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_WIPE

	/*初戦をおこなったかセット*/
	_BTOWER_TOOLS		BTWR_SUB_SET_FIRST_BTL_FLAG,BTWR_NULL_PARAM,FSW_ANSWER

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	//_BTOWER_TOOLS		BTWR_SUB_LOCAL_BTL_CALL,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_BATTLE_CALL
	//_TRAINER_LOSE_CHECK FSW_ANSWER
	//_LDWK				FSW_PARAM3,FSW_ANSWER
	_BTOWER_CALL_GET_RESULT	FSW_PARAM3
#endif	/****************************************************************/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET

/*エネミー入場*/
ev_tower_rooms_comanm_enein:
	/*対戦トレーナーのOBJコード取得*/
	_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,0,FSW_PARAM2
	//_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,0,FSW_ANSWER
	//_LDWK				OBJCHRWORK1,FSW_ANSWER

	/*対戦トレーナー追加*/
	_CHAR_RESOURCE_SET	enemy_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			enemy_set_actor					/*アクター登録*/

	//バニッシュフラグOFF
	//_FLAG_RESET		FV_OBJID_ETC
	//_OBJ_ADD			OBJID_ENEMY
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0205_enemy_01	
	_OBJ_ANIME_WAIT
	_RET

/*シングルバトル前　互いに一歩近づく*/
ev_tower_rooms_comanm_sbtl_start:
	_OBJ_ANIME			OBJID_MINE,anm_d31r0205_player_02
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0205_enemy_02
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						シングル対戦								*/
/********************************************************************/
ev_tower_rooms_sbtl_02:
	//通常連戦 次は？人目の相手です
	_BTOWER_TOOLS		BTWR_SUB_GET_NOW_ROUND,BTWR_NULL_PARAM,FSW_PARAM3
	_NUMBER_NAME		0,FSW_PARAM3
	_TALKMSG			msg_tower_66

	////////////////////////////////////////////////////////////////////////////////
	/*ブレーンのリストへジャンプ*/

	//シングルバトルモードでない時はブレーンのリストへジャンプするかを飛ばす
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,NE,BTWR_MODE_SINGLE,ev_tower_rooms_sbtl_02_00

	//シングルならラウンドチェック
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_1ST,ev_tower_rooms_leader_comin_02
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_2ND,ev_tower_rooms_leader_comin_02
	////////////////////////////////////////////////////////////////////////////////

	_JUMP				ev_tower_rooms_sbtl_02_00
	_END

ev_tower_rooms_sbtl_02_00:
	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP			FSW_LOCAL3,EQ,0,ev_tower_rooms_sbtl_02_rec

	_BMPLIST_INIT		24,11,0,0,FSW_ANSWER			//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_JUMP				ev_tower_rooms_sbtl_02_sub
	_END

ev_tower_rooms_sbtl_02_rec:
	_BMPLIST_INIT		23,9,0,0,FSW_ANSWER				//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_BMPLIST_MAKE_LIST	msg_tower_choice_13,FSEV_WIN_TALK_MSG_NONE,1			/*きろくする*/
	_JUMP				ev_tower_rooms_sbtl_02_sub
	_END

ev_tower_rooms_sbtl_02_sub:
	_BMPLIST_MAKE_LIST	msg_tower_choice_07,FSEV_WIN_TALK_MSG_NONE,2			//やすむ
	_BMPLIST_MAKE_LIST	msg_tower_choice_08,FSEV_WIN_TALK_MSG_NONE,3			//リタイア
	_BMPLIST_START
	
	_TALK_CLOSE

	_SWITCH				FSW_ANSWER
	_CASE_JUMP			0,ev_tower_rooms_next			//つづける
	_CASE_JUMP			1,ev_tower_rooms_is_rec			/*きろくする*/
	_CASE_JUMP			2,ev_tower_rooms_is_rest		//やすむ
	_CASE_JUMP			3,ev_tower_rooms_is_retire		//リタイア
	_JUMP				ev_tower_rooms_next				//つづける
	_END


/********************************************************************/
/*							つづける								*/
/********************************************************************/
ev_tower_rooms_next:
	//案内嬢もとの位置へ
	_CALL				ev_tower_rooms_comanm_guide_out

	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	//バトル
	_JUMP				ev_tower_rooms_sbtl_01
	_END

//回復後　プレイヤー振り向き＆案内嬢下がる
ev_tower_rooms_comanm_guide_out:
	_OBJ_ANIME			OBJID_MINE,anm_d31r0205_player_04
	_OBJ_ANIME			OBJID_PCWOMAN2,anm_d31r0205_pcwoman2_03
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*							記録する								*/
/********************************************************************/
ev_tower_rooms_is_rec:
	/*「記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower5_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower5_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_tower_rooms_sbtl_02

	_CALL			ev_tower5_room_rec_win		/*勝利の記録*/
	_JUMP			ev_tower_rooms_sbtl_02
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_tower5_room_rec_win:
	_CALL				ev_tower5_room_rec_common		/*ANSWER,LOCAL5使用中*/
	//_ADD_WK			FSW_LOCAL5,1
	//_SUB_WK			FSW_LOCAL5,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_tower5_room_rec
	_END

/*負け*/
ev_tower5_room_rec_lose:
	_CALL				ev_tower5_room_rec_common		/*ANSWER,LOCAL5使用中*/
	_ADD_WK				FSW_LOCAL5,1
	_JUMP				ev_tower5_room_rec
	_END

/*共通部分*/
ev_tower5_room_rec_common:
	/*録画データセーブ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_LOCAL5		/*連勝数*/
	_RET

ev_tower5_room_rec:
	_JUMP				ev_tower5_room_rec_sub
	_END

ev_tower5_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL3,1

	_TALKMSG_ALL_PUT	msg_tower_100_05
	/*BTWR_MODE_SINGLE,BTWR_MODE_DOUBLE*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER		/*モード*/
	_ADD_WAITICON
	_BATTLE_REC_SAVE	FRONTIER_NO_TOWER,FSW_ANSWER,FSW_LOCAL5,FSW_LOCAL5
	_DEL_WAITICON

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_tower5_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_tower_100_03
	_RET

ev_tower5_room_rec_true:
	_SE_PLAY			SEQ_SE_DP_SAVE
	/*「記録されました」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_tower_100_02
	_RET
	
/*録画データがない時メッセージ*/
ev_tower5_room_rec_msg1:
	_TALKMSG			msg_tower_100_01
	_RET

/*すでに録画データがある時メッセージ*/
ev_tower5_room_rec_msg2:
	_TALKMSG			msg_tower_100_04
	_RET


/********************************************************************/
/*							やすむ									*/
/********************************************************************/
ev_tower_rooms_is_rest:
	//中断して休むかどうか？
	_TALKMSG			msg_tower_67

	_YES_NO_WIN			FSW_ANSWER

	_TALK_CLOSE
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_rooms_common_rest
	_JUMP				ev_tower_rooms_sbtl_02
	_END

//休むときの処理
ev_tower_rooms_common_rest:
	//_LDVAL				WK_SCENE_D31R0201,2
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0201,2		/*続きから再開されるようにする*/

	//プレイデータセーブ
	_BTOWER_TOOLS		BTWR_SUB_SAVE_REST_PLAY_DATA,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_WORK_RELEASE

	_TALKMSG			msg_tower_71
	_ADD_WAITICON

	_REPORT_SAVE		FSW_ANSWER
	//_REPORT_DIV_SAVE	FSW_ANSWER					/*通信していない*/
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_TALK_CLOSE

	/*ワーク開放*/
	_BATTLE_REC_EXIT

//	_VANISH_DUMMY_OBJ_DEL
	//リセットコマンド
	_BTOWER_TOOLS		BTWR_TOOL_SYSTEM_RESET,BTWR_NULL_PARAM,FSW_ANSWER
	_END


/********************************************************************/
/*							リタイヤ								*/
/********************************************************************/
ev_tower_rooms_is_retire:
	//リタイアするかどうか？
	_TALKMSG			msg_tower_68

	_YES_NO_WIN_EX		FSW_ANSWER

	_TALK_CLOSE

	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_rooms_common_retire
	_JUMP				ev_tower_rooms_sbtl_02
	_END

//リタイアするときの処理(記録する流れなし)
ev_tower_rooms_common_retire:
	//敗戦パラメータセット
	_BTOWER_TOOLS		BTWR_SUB_SET_LOSE_SCORE,BTWR_NULL_PARAM,FSW_ANSWER
	_JUMP				ev_tower_rooms_common_exit
	_END


/********************************************************************/
/*						リーダー対戦								*/
/********************************************************************/
ev_tower_rooms_if_leader_battle:
	//シングルバトルモードかどうか？
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,NE,BTWR_MODE_SINGLE,ev_tower_rooms_sbtl_02

#ifdef DEBUG_LEADER	/****************************************************/
	/*デバック：リーダーと戦闘*/
	_JUMP				ev_tower_rooms_leader_comin
#endif	/****************************************************************/

	//シングルならラウンドチェック
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_1ST,ev_tower_rooms_leader_comin
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_2ND,ev_tower_rooms_leader_comin
	_JUMP				ev_tower_rooms_sbtl_02
	_END

ev_tower_rooms_leader_comin:
	//タワータイクーンとの戦闘になります
	_TALKMSG			msg_tower_boss_01
	_JUMP				ev_tower_rooms_leader_comin_02
	_END

ev_tower_rooms_leader_comin_02:
	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP			FSW_LOCAL3,EQ,0,ev_tower_rooms_leader_comin_rec

	_BMPLIST_INIT		24,11,0,0,FSW_ANSWER			//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_JUMP				ev_tower_rooms_leader_comin_sub
	_END

ev_tower_rooms_leader_comin_rec:
	_BMPLIST_INIT		23,9,0,0,FSW_ANSWER				//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_BMPLIST_MAKE_LIST	msg_tower_choice_13,FSEV_WIN_TALK_MSG_NONE,1			/*きろくする*/
	_JUMP				ev_tower_rooms_leader_comin_sub
	_END

ev_tower_rooms_leader_comin_sub:
	_BMPLIST_MAKE_LIST	msg_tower_choice_07,FSEV_WIN_TALK_MSG_NONE,2			//やすむ
	_BMPLIST_MAKE_LIST	msg_tower_choice_08,FSEV_WIN_TALK_MSG_NONE,3			//リタイア
	_BMPLIST_START
	
	_TALK_CLOSE
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			0,ev_tower_rooms_leader_entry	//つづける
	_CASE_JUMP			1,ev_tower_rooms_is_rec			/*きろくする*/
	_CASE_JUMP			2,ev_tower_rooms_is_rest		//やすむ
	_CASE_JUMP			3,ev_tower_rooms_is_retire		//リタイア
	_JUMP				ev_tower_rooms_leader_entry		//つづける
	_END


/********************************************************************/
/*						つづける　リーダー入場						*/
/********************************************************************/
ev_tower_rooms_leader_entry:
	//案内嬢もとの位置へ
	_CALL				ev_tower_rooms_comanm_guide_out

	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL3,0

	//リーダーデータセット
	_BTOWER_TOOLS		BTWR_SUB_CHOICE_BTL_PARTNER,BTWR_NULL_PARAM,FSW_ANSWER
	
	//リーダー入場
	//_BGM_STOP			SEQ_BF_TOWWER
	//_BGM_PLAY			SEQ_PL_EYEBRAIN
	_CALL				ev_tower_rooms_comanm_leader_in

	//リーダー対戦前台詞
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_2ND,ev_tower_rooms_leader_talk_01

	_PLAYER_NAME		0
	_RIVAL_NAME			1
	_TALKMSG			msg_tower_boss_02
	_JUMP				ev_tower_rooms_leader_talk01_end
	_END

ev_tower_rooms_leader_talk01_end:
	_TALK_CLOSE

	//互いに近づく
	_CALL				ev_tower_rooms_comanm_sbtl_start

	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK
	_RECORD_INC			RECID_FRONTIER_BRAIN
	_BRAIN_ENCOUNT_EFFECT		FRONTIER_NO_TOWER

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	//_BTOWER_TOOLS		BTWR_SUB_LOCAL_BTL_CALL,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_BATTLE_CALL
	//_TRAINER_LOSE_CHECK FSW_ANSWER
	//_LDWK				FSW_PARAM3,FSW_ANSWER
	_BTOWER_CALL_GET_RESULT	FSW_PARAM3
#endif	/****************************************************************/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
	/*負けて戻るときの処理*/
	_IFVAL_JUMP			FSW_PARAM3,EQ,0,ev_tower_rooms_common_lose
#endif	/****************************************************************/

	//リーダー負け台詞
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,LEADER_SET_2ND,ev_tower_rooms_leader_talk_02

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_TOWER,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_NONE,ev_tower_meory_print_put_1st

	/*1stリーダー戦闘後msg*/
	_TALKMSG			msg_tower_boss_04
	_BTOWER_TOOLS		BTWR_SUB_SET_LEADER_CLEAR_FLAG,1,FSW_ANSWER

	_JUMP				ev_tower_rooms_leader_talk02_end
	_END

/*受付で1st記念プリントを貰えるようにする*/
ev_tower_meory_print_put_1st:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_TOWER,MEMORY_PRINT_PUT_1ST
	_RET

ev_tower_rooms_leader_talk02_end:
	_TALK_CLOSE
	
	_CALL				ev_tower_rooms_common_win_param

	//リーダー戦の場合、必ず終わり
	_JUMP				ev_tower_rooms_leader_clear_ret
	_END

//二回目
ev_tower_rooms_leader_talk_01:
	_TALKMSG			msg_tower_boss_03
	_JUMP				ev_tower_rooms_leader_talk01_end
	_END

ev_tower_rooms_leader_talk_02:

	/*現在の記念プリント状態を取得*/
	_SAVE_EVENT_WORK_GET	SYS_WORK_MEMORY_PRINT_TOWER,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,MEMORY_PRINT_PUT_OK_1ST,ev_tower_meory_print_put_2nd

	/*2ndリーダー戦闘後msg*/
	_TALKMSG			msg_tower_boss_05
	_BTOWER_TOOLS		BTWR_SUB_SET_LEADER_CLEAR_FLAG,2,FSW_ANSWER
	_JUMP				ev_tower_rooms_leader_talk02_end
	_END

/*受付で2nd記念プリントを貰えるようにする*/
ev_tower_meory_print_put_2nd:
	_SAVE_EVENT_WORK_SET	SYS_WORK_MEMORY_PRINT_TOWER,MEMORY_PRINT_PUT_2ND
	_RET


/********************************************************************/
/*							リーダー入場							*/
/********************************************************************/
ev_tower_rooms_comanm_leader_in:
	//ダミーでドッペルゲンガー入場
	//_LDVAL			OBJCHRWORK1,TOWERBOSS
	_LDVAL				FSW_PARAM2,TOWERBOSS

	/*対戦トレーナー追加*/
	_CHAR_RESOURCE_SET	enemy_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			enemy_set_actor					/*アクター登録*/

	//バニッシュフラグOFF
	//_FLAG_RESET		FV_OBJID_ETC
	//_OBJ_ADD			OBJID_ENEMY
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0205_leader_01
	_OBJ_ANIME_WAIT
	_RET


