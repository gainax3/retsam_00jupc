//==============================================================================
/**
 * @file	tower_6.s
 * @brief	「バトルタワー」フロンティアスクリプト(タワーマルチ対戦部屋)
 * @author	nohara
 * @date	2007.05.29
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../frontier_def.h"
	.include	"../../../include/msgdata/msg_d31r0206.h"
	.include	"../../../include/msgdata/msg_ev_win.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../field/b_tower_scr_def.h"
	.include	"../../fielddata/base_goods/goods_id_def.h"


//--------------------------------------------------------------------
//
//					     スクリプト本体
//
//	FSW_LOCAL0	通信結果などに使用
//	FSW_LOCAL1	アニメさせる自分のOBJID
//	FSW_LOCAL2	アニメさせるパートナーのOBJID
//	FSW_LOCAL3	(SCWK_TEMP0を置き換え、引継ぎに注意！)
//	FSW_LOCAL4	「しばらくおまちください」を表示してウィンドウを削除するか判別用
//	FSW_LOCAL5	記録処理中に使用
//	FSW_LOCAL6	記録したか
//
//	FSW_PARAM0	自分
//	FSW_PARAM1	パートナー
//	FSW_PARAM2	敵トレーナー1
//	FSW_PARAM3	敵トレーナー2
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_tower_6_start	//一番上のEVENT_DATAは自動実行
_EVENT_DATA_END						//終了

//--------------------------------------------------------------------
//					     デバック定義
//--------------------------------------------------------------------
//#define DEBUG_7_WIN					//7連勝デバック
//#define DEBUG_BTL_OFF					//バトルオフデバック
//#define DEBUG_BTL_LOSE_OFF			//バトル敗北オフデバック


//--------------------------------------------------------------------
//							座標定義
//--------------------------------------------------------------------
#define PLAYER_X		(8*15)//(8*16)
#define PLAYER_Y		(8*15)//(8*16)
#define PCWOMAN2_01_X	(8*5)//(8*4)
#define PCWOMAN2_01_Y	(8*13)//(8*8)
#define PCWOMAN2_02_X	(PCWOMAN2_01_X)
#define PCWOMAN2_02_Y	(8*15)//(8*10)
#define MINE_X			(8*15)//(8*16)
#define MINE_Y			(8*21)//(8*20)
#define PAIR_X			(8*17)//(8*18)
#define PAIR_Y			(MINE_Y)
#define SIO_USER_0_X	(MINE_X)
#define SIO_USER_0_Y	(MINE_Y)
#define SIO_USER_1_X	(PAIR_X)
#define SIO_USER_1_Y	(PAIR_Y)
#define ENEMY_X			(PAIR_X)
#define ENEMY_Y			(8*7)
#define ENEMY2_X		(MINE_X)
#define ENEMY2_Y		(ENEMY_Y)


/********************************************************************/
//
//		SCENE_TOWER:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
//AIマルチ
_RESOURCE_LABEL	default_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	//_CHAR_RESOURCE_DATA	FSW_PARAM0,WF2DC_C_MOVENORMAL	//自分
	_CHAR_RESOURCE_DATA		FSW_PARAM1,WF2DC_C_MOVENORMAL	//パートナー
	_CHAR_RESOURCE_DATA_END

//通信マルチ
_RESOURCE_LABEL	comm_multi_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	_CHAR_RESOURCE_DATA		FSW_PARAM6,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END

/*敵トレーナーの決定タイミングが途中なので分けた*/
_RESOURCE_LABEL	enemy_set_resource
	_CHAR_RESOURCE_DATA		FSW_PARAM2,WF2DC_C_MOVENORMAL	//敵1
	_CHAR_RESOURCE_DATA		FSW_PARAM3,WF2DC_C_MOVENORMAL	//敵2
	_CHAR_RESOURCE_DATA_END

//====================================================================
//	SCENE_TOWER:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
#define OBJID_PCWOMAN2_01			(1)
#define OBJID_PCWOMAN2_02			(2)
#define OBJID_ENEMY					(3)			//敵1
#define OBJID_ENEMY2				(4)			//敵2
#define OBJID_MINE					(5)			//AI：自分
#define OBJID_PAIR					(6)			//AI：パートナー
#define OBJID_SIO_USER_0			(7)			//通信：自分
#define OBJID_SIO_USER_1			(8)			//通信：パートナー

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

//AIマルチ
_ACTOR_LABEL	default_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,OFF/*自分自身アクターセット*/
	//係り
	_ACTOR_DATA				OBJID_PCWOMAN2_01,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							PCWOMAN2_01_X,PCWOMAN2_01_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PCWOMAN2_02,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							PCWOMAN2_02_X,PCWOMAN2_02_Y,ON,EVENTID_TEST_SCR_OBJ1
	//自分、パートナー
	_ACTOR_DATA				OBJID_MINE,FSW_PARAM0,WF2DMAP_WAY_C_UP, \
							MINE_X,MINE_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PAIR,FSW_PARAM1,WF2DMAP_WAY_C_UP, \
							PAIR_X,PAIR_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END

//通信マルチ
_ACTOR_LABEL	comm_multi_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,OFF/*自分自身アクターセット*/
	//係り
	_ACTOR_DATA				OBJID_PCWOMAN2_01,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							PCWOMAN2_01_X,PCWOMAN2_01_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_PCWOMAN2_02,FSW_PARAM6,WF2DMAP_WAY_C_RIGHT, \
							PCWOMAN2_02_X,PCWOMAN2_02_Y,ON,EVENTID_TEST_SCR_OBJ1
	//自分、パートナー
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP, \
							SIO_USER_0_X,SIO_USER_0_Y,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP, \
							SIO_USER_1_X,SIO_USER_1_Y,ON
	_ACTOR_DATA_END

/*敵トレーナーの決定タイミングが途中なので分けた*/
_ACTOR_LABEL	enemy_set_actor
	_ACTOR_DATA				OBJID_ENEMY,FSW_PARAM2,WF2DMAP_WAY_C_DOWN, \
							ENEMY_X,ENEMY_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA				OBJID_ENEMY2,FSW_PARAM3,WF2DMAP_WAY_C_DOWN, \
							ENEMY2_X,ENEMY2_Y,ON,EVENTID_TEST_SCR_OBJ1
	_ACTOR_DATA_END


/********************************************************************/
//							アニメ
/********************************************************************/

//--------------------------------------------------------------------
//自機/パートナー　初期位置へ移動
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_player_01
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END
_ANIME_LABEL	anm_d31r0206_partner_01
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA	FC_WALK_L_8F,4
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー/エネミー２　初期位置へ移動
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_enemy1_01
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,3
	_ANIME_DATA	FC_WALK_D_8F,3
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

_ANIME_LABEL	anm_d31r0206_enemy2_01
	_ANIME_DATA	FC_WALK_D_8F,1
	_ANIME_DATA	FC_WALK_R_8F,4
	_ANIME_DATA	FC_WALK_D_8F,2
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機/パートナー共通　対戦前に一歩前へ
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_player_02
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー共通　対戦前に一歩前へ
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_enemy_02
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
/*エネミー共通　対戦メッセージ表示時に足踏み*/
_ANIME_LABEL	anm_d31r0206_enemy_03
	_ANIME_DATA	FC_STAY_WALK_L_16F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//エネミー/エネミー２　敗戦後退場
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_enemy1_03
	_ANIME_DATA	FC_WALK_U_8F,3
	_ANIME_DATA	FC_WALK_L_8F,2
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END
_ANIME_LABEL	anm_d31r0206_enemy2_03
	_ANIME_DATA	FC_WALK_U_8F,2
	_ANIME_DATA	FC_WALK_L_8F,3
	_ANIME_DATA	FC_WALK_U_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機/パートナー　対戦勝利後　一歩引き振り向き
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_player_03
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//案内嬢　対戦勝利後　近づき
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_pcwoman2_01
	_ANIME_DATA	FC_WALK_R_8F,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//自機/パートナー　回復後振り向き
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_player_04
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END

//--------------------------------------------------------------------
//案内嬢　回復後下がる
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_pcwoman2_02
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA	FC_WALK_L_8F,1
	_ANIME_DATA	FC_DIR_R,1
	_ANIME_DATA_END
	
//--------------------------------------------------------------------
//自機　続きから
//--------------------------------------------------------------------
_ANIME_LABEL	anm_d31r0206_player_05
	_ANIME_DATA	FC_DIR_L,1
	_ANIME_DATA_END


/********************************************************************/
/*								開始								*/
/********************************************************************/
fss_tower_6_start:

	/*ワーク確保*/
	_BATTLE_REC_INIT

	/*FLAGラベル*/
	_CALL				ev_d31r0206_flag_change

	/*OBJラベル*/
	_CALL				ev_d31r0206_obj_change

	/*「きろくする」で使用するワークなのでクリアしておく*/
	_LDVAL				FSW_LOCAL6,0

	/*通信マルチ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,comm_tower_6_start
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_6_start

	/*AIマルチ*/
	_LDVAL				FSW_LOCAL1,OBJID_MINE
	_LDVAL				FSW_LOCAL2,OBJID_PAIR
	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	default_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			default_set_actor					/*アクター登録*/
	_JUMP				comm_tower_6_start2
	_END

comm_tower_6_start:

	/*勝敗に関係なく通信フロンティアに挑戦(通信は中断がないので先頭で処理しても大丈夫)*/
	_SCORE_ADD			SCORE_ID_FRONTIER_COMM

	_LDVAL				FSW_LOCAL1,OBJID_SIO_USER_0
	_LDVAL				FSW_LOCAL2,OBJID_SIO_USER_1
	_LDVAL				FSW_PARAM6,BFSW1
	_CHAR_RESOURCE_SET	comm_multi_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			comm_multi_set_actor				/*アクター登録*/
	_JUMP				comm_tower_6_start2
	_END

comm_tower_6_start2:
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*SCENEラベル*/
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0206,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_common_entry
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_tower_roomd_common_continue
	_END


/********************************************************************/
/*					初めて部屋に入場したとき						*/
/********************************************************************/
ev_tower_roomd_common_entry:
	//_LDVAL			WK_SCENE_D31R0206,3
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0206,3

	_CALL				ev_tower_roomd_comanm_in
	
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,ev_tower_roomd_siobtl_01
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,ev_tower_roomd_siobtl_01

	_JUMP				ev_tower_roomd_aibtl_01
	_END

//自機とパートナー入場
ev_tower_roomd_comanm_in:
	_OBJ_ANIME			FSW_LOCAL1,anm_d31r0206_player_01
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0206_partner_01
	_OBJ_ANIME_WAIT
	_RET

ev_tower_roomd_siobtl_01:
	/*SIOマルチバトル呼び出しコモン*/
	_CALL				ev_tower_roomd_common_siobtl_call

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
#ifndef BTOWER_AUTO_DEB
	_IFVAL_JUMP			FSW_LOCAL3,EQ,0,ev_tower_roomd_common_lose
#endif
#endif	/****************************************************************/

	//勝った時の処理
	_CALL				ev_tower_roomd_common_win_param
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_common_clear_ret

	//エネミー退場
	_CALL				ev_tower_roomd_comanm_eneout

	//プレイヤー振り向き
	_CALL				ev_tower_roomd_comanm_btl_end

	//ポケモン回復
	_CALL				ev_tower_roomd_common_heal
	_JUMP				ev_tower_roomd_siobtl_02
	_END

//連戦
ev_tower_roomd_siobtl_02:
	_BTOWER_TOOLS		BTWR_SUB_GET_NOW_ROUND,BTWR_NULL_PARAM,FSW_ANSWER
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG			msg_tower_82

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_tower_roomd_siobtl_02_rec

	_BMPLIST_INIT		24,13,0,0,FSW_ANSWER		//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0		//つづける
	_JUMP				ev_tower_roomd_roomd_siobtl_02_sub
	_END

ev_tower_roomd_siobtl_02_rec:

	/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
	_CALL				ev_tower_multi_dp_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_siobtl_03_rec	/*記録なし*/

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_siobtl_03_rec	/*記録なし*/

	_BMPLIST_INIT		23,11,0,0,FSW_ANSWER		//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0		//つづける
	_BMPLIST_MAKE_LIST	msg_tower_choice_13,FSEV_WIN_TALK_MSG_NONE,1		/*きろくする*/
	_JUMP				ev_tower_roomd_roomd_siobtl_02_sub
	_END

/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
ev_tower_roomd_siobtl_03_rec:
	_BMPLIST_INIT		24,13,0,0,FSW_ANSWER		//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0		//つづける
	_JUMP				ev_tower_roomd_roomd_siobtl_02_sub
	_END

ev_tower_roomd_roomd_siobtl_02_sub:
	_BMPLIST_MAKE_LIST	msg_tower_choice_08,FSEV_WIN_TALK_MSG_NONE,2		//リタイア
	_BMPLIST_START
	
	_TALK_CLOSE

	_LDVAL				FSW_LOCAL0,0				//リタイアが選ばれると1になる
	_IFVAL_JUMP			FSW_ANSWER,EQ,2,ev_tower_roomd_is_sio_retire

	/*きろくする*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_is_sio_rec

	_JUMP				ev_tower_roomd_siobtl_03
	_END

//続けるかどうかの選択(LOCAL0が1ならリタイア)
ev_tower_roomd_siobtl_03:
	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL6,0

	//お友達の選択を待っています
	_TALKMSG			msg_tower_114
	//選択待ち通信同期
	_COMM_RESET
	_COMM_SYNCHRONIZE	TOWER_COMM_MULTI_NEXT_SELECT

	_BTOWER_TOOLS		BTWR_SUB_RECV_BUF_CLEAR,BTWR_NULL_PARAM,FSW_ANSWER
	_JUMP				ev_tower_roomd_siobtl_03_retry
	_END

/*送信失敗したら再送信*/
ev_tower_roomd_siobtl_03_retry:
	//選択結果送信
	_BTOWER_SEND_BUF	2,FSW_LOCAL0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_siobtl_03_retry

	//選択結果受信
	_BTOWER_RECV_BUF	2,FSW_LOCAL0

	_TALK_CLOSE
	_SWITCH				FSW_LOCAL0	
	_CASE_JUMP			1,ev_tower_roomd_sio_retire	//リタイア

	//連戦を選択
	//案内嬢もとの位置へ
	_CALL				ev_tower_roomd_comanm_guide_out

	//バトル
	_JUMP				ev_tower_roomd_siobtl_01
	_END


/********************************************************************/
/*					マルチ対戦部屋in	コモン						*/
/********************************************************************/
//部屋から退場して受け付けに戻る
ev_tower_roomd_common_exit:
	/*ワーク開放*/
	_BATTLE_REC_EXIT

	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,ev_tower_roomd_sio_exit_multi
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,ev_tower_roomd_sio_exit

	//ウェイト
	_TIME_WAIT			30,FSW_ANSWER

	//受付戻りフラグセット
	//_LDVAL			WK_SCENE_D31R0201,1
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0201,1

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2_01
	_ACTOR_FREE			OBJID_PCWOMAN2_02
	_ACTOR_FREE			OBJID_MINE
	_ACTOR_FREE			OBJID_PAIR
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	//_CHAR_RESOURCE_FREE	FSW_PARAM0
	_CHAR_RESOURCE_FREE	FSW_PARAM1

	//_MAP_CHANGE			ZONE_ID_D31R0201,0,7,6,DIR_UP
	//_MAP_CHANGE			FSS_SCENEID_TOWER
	//_END

	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*マルチのみ*/
ev_tower_roomd_sio_exit_multi:
	/*TV処理*/
	_TV_TEMP_FRIEND_SET	FRONTIER_NO_TOWER
	_JUMP				ev_tower_roomd_sio_exit
	_END

//通信マルチ部屋から出て戻る
ev_tower_roomd_sio_exit:

	//ウェイト
	_TIME_WAIT			30,FSW_ANSWER
	
	//受付戻りフラグセット
	//_LDVAL			WK_SCENE_D31R0201,1
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0201,1
	
	//通信同期
	_COMM_RESET
	_COMM_SYNCHRONIZE	TOWER_COMM_MULTI_EXIT_WAIT

	/*メッセージウィンドウを削除する判別用*/
	_IFVAL_CALL			FSW_LOCAL4,EQ,100,ev_tower_roomd_sio_exit_close

	//WIFI(ポケセン地下)のみ
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_roomd_common_report_wifi

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	
	//通信同期
	_COMM_RESET
	_COMM_SYNCHRONIZE	TOWER_COMM_MULTI_SIO_END

	/*通信マルチ(ワイアレス通信のみ通信切断する)*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,comm_tower_6_direct_end
	//_IFVAL_CALL			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_6_direct_end
	
	_ACTOR_FREE			OBJID_PLAYER
	_ACTOR_FREE			OBJID_PCWOMAN2_01
	_ACTOR_FREE			OBJID_PCWOMAN2_02
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	_CHAR_RESOURCE_FREE	FSW_PARAM6
	//_CHAR_RESOURCE_FREE	FSW_PARAM0
	_CHAR_RESOURCE_FREE	FSW_PARAM1

	//_MAP_CHANGE_NONE_FADE	ZONE_ID_D31R0201,0,7,6,DIR_UP
	//_MAP_CHANGE_EX		FSS_SCENEID_TOWER,0

	//_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK

	//WIFI(ポケセン地下)のみ
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_roomd_common_exit_wifi

	//_END
	_SCRIPT_FINISH			/*_ENDして2Dマップ終了*/

/*メッセージウィンドウを削除する判別用*/
ev_tower_roomd_sio_exit_close:
	_TALK_CLOSE
	_RET


/********************************************************************/
/*						WIFIは待ち合わせ画面へ戻る					*/
//
/*	WiFi呼び出しの時はここで終了してしまうので、					*/
/*	ここでモードでワークの開放が必要								*/
/********************************************************************/
comm_tower_roomd_common_exit_wifi:
	_FR_WIFI_COUNTER_TOWER_CALL_AFTER

	_MAP_CHANGE_EX		FSS_SCENEID_WIFI_COUNTER,1
	_END

//WIFI(ポケセン地下)のみ
comm_tower_roomd_common_report_wifi:
	_TALKMSG_ALL_PUT	msg_tower_117
	_ADD_WAITICON
	_REPORT_DIV_SAVE	FSW_ANSWER
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE
	_TALK_CLOSE
	_RET


/********************************************************************/
/*					ワイアレス通信は通信切断						*/
/********************************************************************/
comm_tower_6_direct_end:
	_COMM_DIRECT_END
	_RET


/********************************************************************/
/*					続きからで部屋に入場した時						*/
/********************************************************************/
ev_tower_roomd_common_continue:
	//_LDVAL			WK_SCENE_D31R0206,3
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0206,3
	
	/*記録したワークにデータが書き込まれていないのでメニュー表示しない*/
	_LDVAL				FSW_LOCAL6,1

	_CALL				ev_tower_roomd_comanm_in
	_CALL				ev_tower_roomd_comanm_continue

	//「次は？人目の……」
	_JUMP				ev_tower_roomd_aibtl_02
	_END

//自機とパートナー/受け付け嬢　続きから始めたときの入場後
ev_tower_roomd_comanm_continue:
	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0206_pcwoman2_01
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0206_pcwoman2_01
	_OBJ_ANIME			FSW_LOCAL1,anm_d31r0206_player_05
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0206_player_05
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*					通常連戦 次は？人目の相手です					*/
/********************************************************************/
ev_tower_roomd_aibtl_02:
	_BTOWER_TOOLS		BTWR_SUB_GET_NOW_ROUND,BTWR_NULL_PARAM,FSW_ANSWER
	_NUMBER_NAME		0,FSW_ANSWER
	_TALKMSG			msg_tower_82

	/*まだ記録していない、中断復帰ではない時は「きろくする」メニュー追加*/
	_IFVAL_JUMP			FSW_LOCAL6,EQ,0,ev_tower_roomd_aibtl_02_rec

	_BMPLIST_INIT		24,11,0,0,FSW_ANSWER			//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_JUMP				ev_tower_roomd_aibtl_02_sub
	_END

ev_tower_roomd_aibtl_02_rec:
	_BMPLIST_INIT		23,9,0,0,FSW_ANSWER				//Bキャンセル無効
	_BMPLIST_MAKE_LIST	msg_tower_choice_06,FSEV_WIN_TALK_MSG_NONE,0			//つづける
	_BMPLIST_MAKE_LIST	msg_tower_choice_13,FSEV_WIN_TALK_MSG_NONE,1			/*きろくする*/
	_JUMP				ev_tower_roomd_aibtl_02_sub
	_END

ev_tower_roomd_aibtl_02_sub:
	_BMPLIST_MAKE_LIST	msg_tower_choice_07,FSEV_WIN_TALK_MSG_NONE,2			//やすむ
	_BMPLIST_MAKE_LIST	msg_tower_choice_08,FSEV_WIN_TALK_MSG_NONE,3			//リタイア
	_BMPLIST_START
	
	_TALK_CLOSE

	_SWITCH				FSW_ANSWER
	_CASE_JUMP			0,ev_tower_roomd_ai_next		//つづける
	_CASE_JUMP			1,ev_tower_roomd_is_ai_rec		/*きろくする*/
	_CASE_JUMP			2,ev_tower_roomd_is_ai_rest		//やすむ
	_CASE_JUMP			3,ev_tower_roomd_is_ai_retire	//リタイア
	_JUMP				ev_tower_roomd_ai_next			//つづける
	_END


/********************************************************************/
/*							つづける								*/
/********************************************************************/
ev_tower_roomd_ai_next:
	//案内嬢もとの位置へ
	_CALL				ev_tower_roomd_comanm_guide_out

	/*ワークをクリアしておく*/
	_LDVAL				FSW_LOCAL6,0

	//バトル
	_JUMP				ev_tower_roomd_aibtl_01
	_END

//回復後　プレイヤー振り向き＆案内嬢下がる
ev_tower_roomd_comanm_guide_out:
	_OBJ_ANIME			FSW_LOCAL1,anm_d31r0206_player_04
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0206_player_04
	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0206_pcwoman2_02
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0206_pcwoman2_02
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*						記録する(AIマルチ)							*/
/********************************************************************/
ev_tower_roomd_is_ai_rec:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower6_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_tower_roomd_aibtl_02

	_CALL			ev_tower6_room_rec_win		/*勝利の記録*/
	_JUMP			ev_tower_roomd_aibtl_02
	_END


/********************************************************************/
/*							共通記録*/
/********************************************************************/
/*勝ち*/
ev_tower6_room_rec_win:
	_CALL				ev_tower6_room_rec_common		/*ANSWER,LOCAL5使用中*/
	//_ADD_WK			FSW_LOCAL5,1
	//_SUB_WK			FSW_LOCAL5,1					/*ラウンド数ではなく勝利数なので*/
	_JUMP				ev_tower6_room_rec
	_END

/*負け*/
ev_tower6_room_rec_lose:
	_CALL				ev_tower6_room_rec_common		/*ANSWER,LOCAL5使用中*/
	_ADD_WK				FSW_LOCAL5,1
	_JUMP				ev_tower6_room_rec
	_END

/*共通部分*/
ev_tower6_room_rec_common:
	/*録画データセーブ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_RENSHOU_CNT,BTWR_NULL_PARAM,FSW_LOCAL5		/*連勝数*/
	_RET

ev_tower6_room_rec:
	_JUMP				ev_tower6_room_rec_sub
	_END

ev_tower6_room_rec_sub:
	/*記録したワークセット*/
	_LDVAL				FSW_LOCAL6,1

	_TALKMSG_ALL_PUT	msg_tower_101_05
	/*BTWR_MODE_MULTI(AIマルチのこと),BTWR_MODE_COMM_MULTI,BTWR_MODE_WIFI_MULTI*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER		/*モード*/
	_ADD_WAITICON
	_BATTLE_REC_SAVE	FRONTIER_NO_TOWER,FSW_ANSWER,FSW_LOCAL5,FSW_LOCAL5
	_DEL_WAITICON

	//_BATTLE_REC_LOAD

	_IFVAL_JUMP			FSW_LOCAL5,EQ,1,ev_tower6_room_rec_true

	/*「記録出来ませんでした」*/
	_TALKMSG			msg_tower_101_03
	_RET

ev_tower6_room_rec_true:
	_SE_PLAY			SEQ_SE_DP_SAVE
	/*「記録されました」*/
	_PLAYER_NAME		0
	_TALKMSG			msg_tower_101_02
	_RET
	
/*録画データがない時メッセージ*/
ev_tower6_room_rec_msg1:
	_TALKMSG			msg_tower_101_01
	_RET

/*すでに録画データがある時メッセージ*/
ev_tower6_room_rec_msg2:
	_TALKMSG			msg_tower_101_04
	_RET


/********************************************************************/
/*				AIマルチ中断して休むかどうか？						*/
/********************************************************************/
ev_tower_roomd_is_ai_rest:
	_TALKMSG			msg_tower_83
	_YES_NO_WIN			FSW_ANSWER

	_TALK_CLOSE

	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_common_rest
	_JUMP				ev_tower_roomd_aibtl_02
	_END

//休むときの処理
ev_tower_roomd_common_rest:
	//_LDVAL			WK_SCENE_D31R0201,2
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0201,2

	//プレイデータセーブ
	_BTOWER_TOOLS		BTWR_SUB_SAVE_REST_PLAY_DATA,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_WORK_RELEASE

	_TALKMSG			msg_tower_79
	_ADD_WAITICON
	_REPORT_SAVE		FSW_ANSWER
	//_REPORT_DIV_SAVE	FSW_ANSWER				/*通信していない(AIマルチの処理の流れ)*/
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_TALK_CLOSE

	/*ワーク開放*/
	_BATTLE_REC_EXIT

	//リセットコマンド
	_BTOWER_TOOLS		BTWR_TOOL_SYSTEM_RESET,BTWR_NULL_PARAM,FSW_ANSWER
	_END


/********************************************************************/
/*							リタイア								*/
/********************************************************************/
//AIマルチリタイアするかどうか？
ev_tower_roomd_is_ai_retire:
	_TALKMSG			msg_tower_84
	_YES_NO_WIN_EX		FSW_ANSWER

	_TALK_CLOSE

	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_common_retire
	_JUMP				ev_tower_roomd_aibtl_02
	_END

//リタイアするときの処理(録画する流れなし)
ev_tower_roomd_common_retire:
	//敗戦パラメータセット
	_BTOWER_TOOLS		BTWR_SUB_SET_LOSE_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

	//負けてたらLOCALWORk0に0を代入
	_LDVAL				FSW_LOCAL0,0

	_JUMP				ev_tower_roomd_common_exit
	_END

//負けて戻るときの処理
ev_tower_roomd_common_lose:

	//負けてたらLOCALWORk0に0を代入
	_LDVAL				FSW_LOCAL0,0

	_JUMP				ev_tower_roomd_common_rec_lose
	_END


/********************************************************************/
/*				「さきほどの　戦いを　記録しますか？」				*/
/********************************************************************/
ev_tower_roomd_common_rec_win:

#if 1
	/*通信マルチ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,comm_tower_roomd_common_rec_win_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_roomd_common_rec_win_check
	_JUMP				ev_tower_roomd_common_rec_ok_win
	_END

comm_tower_roomd_common_rec_win_check:
	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_common_rec_win_sub	/*記録なし*/

	/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
	_CALL				ev_tower_multi_dp_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_common_rec_win_sub	/*記録なし*/

	_JUMP				ev_tower_roomd_common_rec_ok_win
	_END

#endif

ev_tower_roomd_common_rec_ok_win:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower6_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_win_rec_yes		/*「はい」*/
	_JUMP				ev_tower_roomd_common_rec_win_sub
	_END

ev_tower_roomd_common_rec_win_sub:

	//クリアパラメータセット
	_BTOWER_TOOLS		BTWR_SUB_SET_CLEAR_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

	/*「７戦突破おめでとう」*/
	_TALKMSG			msg_tower_115

	/*「プレイヤーはＢＰをもらった」*/
	_BTOWER_TOOLS		BTWR_SUB_ADD_BATTLE_POINT,BTWR_NULL_PARAM,FSW_ANSWER
	_NUMBER_NAME		1,FSW_ANSWER
	_PLAYER_NAME		0
	_TALKMSG			msg_tower_116
	_ME_PLAY			ME_BP_GET
	_ME_WAIT
	
	/*通信の時は必ず「しばらくおまちください」表示*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,ev_tower_roomd_common_rec_win_sub3
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,ev_tower_roomd_common_rec_win_sub3

	_TALK_CLOSE
	_JUMP				ev_tower_roomd_common_exit
	_END

ev_tower_roomd_common_rec_win_sub3:	/*記録なしの流れ*/
	/*メッセージウィンドウを削除する判別用*/
	_LDVAL				FSW_LOCAL4,100
	_TALKMSG_NOSKIP		msg_tower_118
	_JUMP				ev_tower_roomd_common_exit
	_END

ev_tower_roomd_common_rec_lose:

	/*メッセージウィンドウを削除する判別用をクリア*/
	_LDVAL				FSW_LOCAL4,0

#if 1
	/*通信マルチ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,comm_tower_roomd_common_rec_lose_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,comm_tower_roomd_common_rec_lose_check
	_JUMP				ev_tower_roomd_common_rec_ok_lose
	_END

comm_tower_roomd_common_rec_lose_check:
	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_common_rec_lose_sub2	/*記録なし*/

	/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
	_CALL				ev_tower_multi_dp_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_common_rec_lose_sub	/*記録なし*/

	_JUMP				ev_tower_roomd_common_rec_ok_lose
	_END

#endif

/*「しばらくおまちください」*/
ev_tower_roomd_common_rec_lose_sub2:
	/*メッセージウィンドウを削除する判別用*/
	_LDVAL				FSW_LOCAL4,100
	_TALKMSG_NOSKIP		msg_tower_118
	_JUMP				ev_tower_roomd_common_rec_lose_sub
	_END

/*「きろくしますか？」*/
ev_tower_roomd_common_rec_ok_lose:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower6_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX		FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_lose_rec_yes		/*「はい」*/

	/*通信マルチ*/
	/*「しばらくおまちください」表示へ*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,ev_tower_roomd_common_rec_lose_sub2
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_WIFI_MULTI,ev_tower_roomd_common_rec_lose_sub2

	_TALK_CLOSE
	_JUMP				ev_tower_roomd_common_rec_lose_sub
	_END

ev_tower_roomd_common_rec_lose_sub:

	//敗戦パラメータセット
	_BTOWER_TOOLS		BTWR_SUB_SET_LOSE_SCORE,BTWR_NULL_PARAM,FSW_ANSWER

	_JUMP				ev_tower_roomd_common_exit
	_END

/*「はい」*/
ev_tower6_lose_rec_yes:
	_CALL				ev_tower6_room_rec_lose		/*敗北の記録*/
	_RET

ev_tower6_win_rec_yes:
	_CALL				ev_tower6_room_rec_win		/*勝利の記録*/
	_RET


/********************************************************************/
/*							AIマルチ対戦							*/
/********************************************************************/
ev_tower_roomd_aibtl_01:
	_CALL				ev_tower_roomd_common_aibtl_call

#ifndef DEBUG_BTL_LOSE_OFF	/********************************************/
#ifndef BTOWER_AUTO_DEB
	_IFVAL_JUMP			FSW_LOCAL3,EQ,0,ev_tower_roomd_common_lose
#endif
#endif	/****************************************************************/

	//勝った時の処理(クリアしてるかどうかをFSW_ANSWERにかえす)
	_CALL				ev_tower_roomd_common_win_param
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_common_clear_ret

	//エネミー退場
	_CALL				ev_tower_roomd_comanm_eneout

	//プレイヤー振り向き
	_CALL				ev_tower_roomd_comanm_btl_end

	//ポケモン回復
	_CALL				ev_tower_roomd_common_heal

	_JUMP				ev_tower_roomd_aibtl_02
	_END

//バトル勝利時パラメータセットコモン
ev_tower_roomd_common_win_param:
	//ラウンド数更新
	_BTOWER_TOOLS		BTWR_SUB_INC_ROUND,BTWR_NULL_PARAM,FSW_ANSWER

	//クリアしてるかチェック
	_BTOWER_TOOLS		BTWR_SUB_IS_CLEAR,BTWR_NULL_PARAM,FSW_ANSWER
	_RET

//クリアして戻るときの処理
ev_tower_roomd_common_clear_ret:
	//勝ってたらLOCAL0に1を代入
	_LDVAL				FSW_LOCAL0,1

	_JUMP				ev_tower_roomd_common_rec_win
	_END

//プレイヤー勝利後　エネミー退場
ev_tower_roomd_comanm_eneout:
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0206_enemy1_03
	_OBJ_ANIME			OBJID_ENEMY2,anm_d31r0206_enemy2_03
	_OBJ_ANIME_WAIT

	/*対戦トレーナー削除*/
	_ACTOR_FREE			OBJID_ENEMY
	_ACTOR_FREE			OBJID_ENEMY2
	_CHAR_RESOURCE_FREE	FSW_PARAM2
	_CHAR_RESOURCE_FREE	FSW_PARAM3

	//バニッシュフラグON
	//_FLAG_SET			FV_OBJID_ENEMY
	//_FLAG_SET			FV_OBJID_ENEMY2
	//_OBJ_DEL			OBJID_ENEMY
	//_OBJ_DEL			OBJID_ENEMY2
	_RET

//プレイヤー勝利後　プレイヤー振り向き&案内嬢近づき
ev_tower_roomd_comanm_btl_end:
	_OBJ_ANIME			FSW_LOCAL1,anm_d31r0206_player_03
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0206_player_03
	_OBJ_ANIME			OBJID_PCWOMAN2_01,anm_d31r0206_pcwoman2_01
	_OBJ_ANIME			OBJID_PCWOMAN2_02,anm_d31r0206_pcwoman2_01
	_OBJ_ANIME_WAIT
	_RET

ev_tower_roomd_common_heal:
	_TALKMSG			msg_tower_81
	_ME_PLAY			SEQ_ASA
	_ME_WAIT
	_PC_KAIFUKU
	_RET


/********************************************************************/
/*					AIマルチバトル呼び出しコモン					*/
/********************************************************************/
ev_tower_roomd_common_aibtl_call:
	//抽選
	_BTOWER_TOOLS		BTWR_SUB_CHOICE_BTL_PARTNER,BTWR_NULL_PARAM,FSW_ANSWER

	//エネミー入場
	_CALL				ev_tower_roomd_comanm_enein

	//対戦前台詞
	_CALL				ev_tower_roomd_comanm_ene1_msganm
	_TALKMSG_BTOWER_APPEAR	0
	_AB_KEYWAIT
	_TALK_CLOSE

	_CALL				ev_tower_roomd_comanm_ene2_msganm
	_TALKMSG_BTOWER_APPEAR	1
	_AB_KEYWAIT
	_TALK_CLOSE

	//互いに近づく
	_CALL				ev_tower_roomd_comanm_btl_start

	//_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	//_WIPE_FADE_END_CHECK
	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_WIPE

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	//_BTOWER_TOOLS		BTWR_SUB_LOCAL_BTL_CALL,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_BATTLE_CALL
	//_TRAINER_LOSE_CHECK FSW_ANSWER
	//_LDWK				FSW_PARAM3,FSW_ANSWER
	_BTOWER_CALL_GET_RESULT	FSW_LOCAL3
#endif	/****************************************************************/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET

//エネミー入場
ev_tower_roomd_comanm_enein:
	//キャラ指定
	_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,0,FSW_PARAM2
	_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,1,FSW_PARAM3
	//_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,0,FSW_ANSWER
	//_LDWK				OBJCHRWORK2,FSW_ANSWER
	//_BTOWER_TOOLS		BTWR_SUB_GET_ENEMY_OBJ,1,FSW_ANSWER
	//_LDWK				OBJCHRWORK3,FSW_ANSWER

	_CHAR_RESOURCE_SET	enemy_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			enemy_set_actor					/*アクター登録*/

	//バニッシュフラグOFF
	//_FLAG_RESET		FV_OBJID_ENEMY
	//_FLAG_RESET		FV_OBJID_ENEMY2
	//_OBJ_ADD			OBJID_ENEMY
	//_OBJ_ADD			OBJID_ENEMY2
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0206_enemy1_01	
	_OBJ_ANIME			OBJID_ENEMY2,anm_d31r0206_enemy2_01	
	_OBJ_ANIME_WAIT
	_RET

//バトル前　互いに一歩近づく
ev_tower_roomd_comanm_btl_start:
	_OBJ_ANIME			FSW_LOCAL1,anm_d31r0206_player_02
	_OBJ_ANIME			FSW_LOCAL2,anm_d31r0206_player_02
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0206_enemy_02
	_OBJ_ANIME			OBJID_ENEMY2,anm_d31r0206_enemy_02
	_OBJ_ANIME_WAIT
	_RET

/*バトル前メッセージ表示時にその場足踏み*/
ev_tower_roomd_comanm_ene1_msganm:
	_OBJ_ANIME			OBJID_ENEMY,anm_d31r0206_enemy_03
	_OBJ_ANIME_WAIT
	_RET

ev_tower_roomd_comanm_ene2_msganm:
	_OBJ_ANIME			OBJID_ENEMY2,anm_d31r0206_enemy_03
	_OBJ_ANIME_WAIT
	_RET


/********************************************************************/
/*				SIOマルチバトル呼び出しコモン						*/
/********************************************************************/
ev_tower_roomd_common_siobtl_call:
	//抽選
	_BTOWER_TOOLS		BTWR_SUB_CHOICE_BTL_PARTNER,BTWR_NULL_PARAM,FSW_ANSWER

	//エネミー入場
	_CALL				ev_tower_roomd_comanm_enein

	//対戦前台詞
	_CALL				ev_tower_roomd_comanm_ene1_msganm
	_TALKMSG_BTOWER_APPEAR	0
	_TIME_WAIT			30,FSW_ANSWER
	_TALK_CLOSE

	_CALL				ev_tower_roomd_comanm_ene2_msganm
	_TALKMSG_BTOWER_APPEAR	1
	_TIME_WAIT			30,FSW_ANSWER
	_TALK_CLOSE
	
	//互いに近づく
	_CALL				ev_tower_roomd_comanm_btl_start

	/*「しばらくおまちください」*/
	_TALKMSG_NOSKIP		msg_tower_118

	//通信同期
	_COMM_RESET
	_COMM_SYNCHRONIZE	TOWER_COMM_MULTI_BATTLE_START
	
	_TALK_CLOSE
	_ENCOUNT_EFFECT		FR_ENCOUNT_EFF_WIPE

#ifndef DEBUG_BTL_OFF	/************************************************/
	/*戦闘呼び出し*/
	//_BTOWER_TOOLS		BTWR_SUB_LOCAL_BTL_CALL,BTWR_NULL_PARAM,FSW_ANSWER
	_BTOWER_BATTLE_CALL
	//_TRAINER_LOSE_CHECK FSW_ANSWER
	//_LDWK				FSW_PARAM3,FSW_ANSWER
	_BTOWER_CALL_GET_RESULT	FSW_LOCAL3
#endif	/****************************************************************/

	/*フェードイン*/
	_BLACK_IN		SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_RET

//本当にリタイアを選択するかどうか？
ev_tower_roomd_is_sio_retire:
	_TALKMSG			msg_tower_84
	_YES_NO_WIN_EX		FSW_ANSWER

	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_siobtl_02

	//キャンセルが選ばれたのでFSW_LOCAL0に1を代入
	_LDVAL				FSW_LOCAL0,1
	_JUMP				ev_tower_roomd_siobtl_03
	_END

//本当にリタイア
ev_tower_roomd_sio_retire:
	_TALKMSG			msg_tower_113

	//ウェイト
	_TIME_WAIT			30,FSW_ANSWER
	_COMM_RESET
	_COMM_SYNCHRONIZE	TOWER_COMM_MULTI_RETIRE_WAIT

	_TALK_CLOSE
	_JUMP				ev_tower_roomd_common_retire	//リタイア
	_END


/********************************************************************/
/*						記録する(通信)								*/
/********************************************************************/
ev_tower_roomd_is_sio_rec:

	/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
	_CALL				ev_tower_multi_dp_check
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_roomd_siobtl_02	/*記録なし*/

	/*自分のROMよりも高いサーバーバージョンで動いていた時は「きろくする」出さない*/
	_BATTLE_REC_SERVER_VER_CHK	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_tower_roomd_siobtl_02	/*記録なし*/

	_JUMP				ev_tower_roomd_is_sio_rec_sub
	_END

ev_tower_roomd_is_sio_rec_sub:
	/*「さきほどの　戦いを　記録しますか？」*/
	_BATTLE_REC_DATA_OCC_CHECK	FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_tower6_room_rec_msg1		/*ない時*/
	_IFVAL_CALL			FSW_ANSWER,EQ,1,ev_tower6_room_rec_msg2		/*ある時*/

	_YES_NO_WIN_EX	FSW_ANSWER									/*いいえデフォルト*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,1,ev_tower_roomd_siobtl_02

	_CALL			ev_tower6_room_rec_win		/*勝利の記録*/
	_JUMP			ev_tower_roomd_siobtl_02
	_END


/********************************************************************/
/*						FLAG_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0206_flag_change:
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0206,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_d31r0206_flag_change_end
	//_FLAG_SET			FV_D31R0206_TRAINER01
	//_FLAG_SET			FV_D31R0206_TRAINER02

	/*
	  通信の子の時は親子の左右位置をいれかえる
	  カレントID取得関数は非接続時に呼ぶと0が返る
	*/

	//_COMM_GET_CURRENT_ID	FSW_LOCAL0
	_IFVAL_JUMP			FSW_LOCAL0,EQ,1,ev_d31r0206_flag_change_child
	
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM1

	/*非接続時と親のとき*/
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM2
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_LOCAL0
	//_LDWK				OBJCHRWORK0,FSW_LOCAL0
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM3
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_LOCAL0
	//_LDWK				OBJCHRWORK1,FSW_LOCAL0
	_RET

ev_d31r0206_flag_change_end:
	_RET

/*通信の子の時*/
ev_d31r0206_flag_change_child:
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM0
	_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM1

	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_PARAM2
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE,FSW_LOCAL0
	//_LDWK				OBJCHRWORK1,FSW_LOCAL0
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_PARAM3
	//_BTOWER_TOOLS		BTWR_SUB_GET_MINE_OBJ,BTWR_PTCODE_MINE2,FSW_LOCAL0
	//_LDWK				OBJCHRWORK0,FSW_LOCAL0
	_RET


/********************************************************************/
/*						OBJ_CHANGE_LABEL 							*/
/********************************************************************/
ev_d31r0206_obj_change:
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0206,FSW_ANSWER
	_IFVAL_CALL			FSW_ANSWER,NE,0,ev_d31r0206_obj_player
	_JUMP				ev_d31r0206_obj_change_02
	_END

ev_d31r0206_obj_change_02:
	_SAVE_EVENT_WORK_GET	WK_SCENE_D31R0206,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,3,ev_d31r0206_obj_pos_change_set
	_JUMP				ev_d31r0206_obj_char_set
	_END

ev_d31r0206_obj_player:
	//_OBJ_INVISIBLE	OBJID_MINE
	//半マスずらす
	//_PLAYER_POS_OFFSET_SET	8,0,0
	_RET

ev_d31r0206_obj_pos_change_set:
	//_OBJ_POS_CHANGE	OBJID_MINE,5,0,5,DIR_RIGHT
	//_OBJ_POS_CHANGE	OBJID_PAIR,5,0,6,DIR_RIGHT
	_JUMP				ev_d31r0206_obj_char_set
	_END

ev_d31r0206_obj_char_set:
	_RET


/********************************************************************/
/*		ワイアレスで、DPが含まれていたら、記録処理はいらない		*/
/********************************************************************/
ev_tower_multi_dp_check:
	/*ワイアレスの時*/
	_BTOWER_TOOLS		BTWR_SUB_GET_PLAY_MODE,BTWR_NULL_PARAM,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,BTWR_MODE_COMM_MULTI,ev_tower_roomd_is_sio_rec_check1

	_LDVAL				FSW_ANSWER,0
	_RET

/*ワイアレスで、DPが含まれていたら、記録処理はいらない*/
ev_tower_roomd_is_sio_rec_check1:
	_CHECK_DP_ROM_CODE	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_tower_multi_dp_check_ari

	_LDVAL				FSW_ANSWER,0
	_RET

ev_tower_multi_dp_check_ari:
	_LDVAL				FSW_ANSWER,1		/*DPあり*/
	_RET


