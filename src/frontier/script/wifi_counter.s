//==============================================================================
/**
 * @file	fr_wifi.s
 * @brief	「バトルフロンティアWiFI受付」
 * @author	nohara
 * @date	2007.07.27
 */
//==============================================================================

	.text

	.include	"../frontier_seq_def.h"
	.include	"../../../include/field/evwkdef.h"
	.include	"../../fielddata/script/saveflag.h"
	.include	"../../fielddata/script/savework.h"
	.include	"../../../include/msgdata/msg_bf_info.h"
	.include	"../frontier_def.h"
	.include	"../factory_def.h"
	.include	"../stage_def.h"
	.include	"../castle_def.h"
	.include	"../roulette_def.h"
	.include	"../../field/syswork_def.h"

//--------------------------------------------------------------------
//
//					     スクリプト本体
//
//	FSW_PARAM0		施設No
//	FSW_PARAM1		通信タイプ
//	FSW_PARAM2		ステージ計算用、引き続き遊ぶか
//	FSW_PARAM3		***
//	FSW_PARAM6		友達と連勝が食い違っているかの結果
//	FSW_LOCAL0		***
//	FSW_LOCAL1		***
//	FSW_LOCAL2		選択した手持ちpos1(LOCALWORK2にコピー)
//	FSW_LOCAL3		init
//	FSW_LOCAL4		やめる
//	FSW_LOCAL5		選択した手持ちpos2(LOCALWORK5にコピー)
//	SYS_WORK_WIFI_FR_CLEAR_FLAG	クリアフラグを取得
//
//--------------------------------------------------------------------
_EVENT_DATA		fss_wifi_counter		//一番上のEVENT_DATAは自動実行
_EVENT_DATA		fss_wifi_counter_return	//施設から戻ってきた時
_EVENT_DATA_END							//終了


//--------------------------------------------------------------------
//							座標定義
//--------------------------------------------------------------------
//#define PLAYER_X		(8*16)
//#define PLAYER_X		(8*13)
#define PLAYER_X		(8*14)
#define PLAYER_Y		(8*14)
//#define PCWOMAN2_01_X	(8*16)
//#define PCWOMAN2_01_Y	(8*10)
//#define SIO_USER_0_X	(8*16)
#define SIO_USER_0_X	(8*14)
//#define SIO_USER_0_Y	(8*14)
#define SIO_USER_0_Y	(8*8)
//#define SIO_USER_1_X	(8*18)
#define SIO_USER_1_X	(8*16)
//#define SIO_USER_1_Y	(8*14)
#define SIO_USER_1_Y	(8*8)


/********************************************************************/
//
//	SCENEID_WIFI_COUNTER:リソースラベル(画面IN時に常駐させるリソース群)
//
/********************************************************************/
//通信マルチ
_RESOURCE_LABEL	comm_multi_set_resource
	_PLAYER_RESOURCE_DATA		//自分自身(HEROorHEROINE)のキャラセット
	_SIO_PLAYER_RESOURCE_DATA	//通信プレイヤー全員のキャラセット
	//_CHAR_RESOURCE_DATA		PCWOMAN2,WF2DC_C_MOVENORMAL
	_CHAR_RESOURCE_DATA_END


//====================================================================
//	SCENEID_WIFI_COUNTER:アクター(画面IN時に常駐させるアクター群)
//====================================================================
#define OBJID_PLAYER				(0)
//#define OBJID_PCWOMAN2_01			(1)
#define OBJID_SIO_USER_0			(2)
#define OBJID_SIO_USER_1			(3)

//スクリプトID(今後対応予定)
#define EVENTID_TEST_SCR_OBJ1		(1)

//通信マルチ
_ACTOR_LABEL	comm_multi_set_actor
	_PLAYER_ACTOR_DATA		OBJID_PLAYER,WF2DMAP_WAY_C_DOWN, \
							PLAYER_X,PLAYER_Y,OFF	/*自分自身のアクターセット*/
	//_ACTOR_DATA				OBJID_PCWOMAN2_01,PCWOMAN2,WF2DMAP_WAY_C_DOWN, \
	//						PCWOMAN2_01_X,PCWOMAN2_01_Y,ON,EVENTID_TEST_SCR_OBJ1
	_SIO_PLAYER_ACTOR_DATA	0,OBJID_SIO_USER_0,WF2DMAP_WAY_C_UP, \
							SIO_USER_0_X,SIO_USER_0_Y,ON
	_SIO_PLAYER_ACTOR_DATA	1,OBJID_SIO_USER_1,WF2DMAP_WAY_C_UP, \
							SIO_USER_1_X,SIO_USER_1_Y,ON
	_ACTOR_DATA_END


/********************************************************************/
/*								開始								*/
/********************************************************************/
fss_wifi_counter:

	/*回復*/
	_PC_KAIFUKU

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_BF_TOWWER

	_FR_WIFI_COUNTER_WORK_ALLOC
	//_FR_WIFI_COUNTER_WORK_FREE

	_TIME_WAIT			1,FSW_ANSWER
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_INIT_1
	_COMM_RESET

	_CHAR_RESOURCE_SET	comm_multi_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			comm_multi_set_actor				/*アクター登録*/
	_JUMP				tower_wayd_common2
	_END

tower_wayd_common2:
	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK
	_JUMP				bf_no_sel_menu
	_END

bf_no_sel_menu:
	/*「どの施設に挑戦しますか？」*/
	_TALKMSG			msg_bf_info_001

	/*メニュー作成*/
	_BMPLIST_INIT_EX	18,5,0,1,FSW_ANSWER									/*Bキャンセル有効*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_01,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_TOWER			/*バトルタワー*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_02,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_FACTORY_LV50	/*バトルファクトリー*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_03,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_CASTLE			/*バトルキャッスル*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_04,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_STAGE			/*バトルステージ*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_05,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_ROULETTE		/*バトルルーレット*/
	_BMPLIST_MAKE_LIST	msg_bf_info_choice_06,FSEV_WIN_TALK_MSG_NONE,FRONTIER_NO_YAMERU			/*やめる*/
	_BMPLIST_START

	_IFVAL_JUMP		FSW_ANSWER,EQ,FRONTIER_NO_TOWER,ev_bf_info_choice_01	/*バトルタワー*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,FRONTIER_NO_FACTORY_LV50,ev_bf_info_choice_02	/*ファクトリー*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,FRONTIER_NO_CASTLE,ev_bf_info_choice_03	/*バトルキャッスル*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,FRONTIER_NO_STAGE,ev_bf_info_choice_04	/*バトルステージ*/
	_IFVAL_JUMP		FSW_ANSWER,EQ,FRONTIER_NO_ROULETTE,ev_bf_info_choice_05	/*バトルルーレット*/
	_JUMP			ev_bf_info_choice_06									/*やめる*/
	_END


/********************************************************************/
/*							バトルタワー							*/
/********************************************************************/
ev_bf_info_choice_01:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_TOWER,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_TOWER
	//_SAVE_EVENT_WORK_SET	WK_SCENE_TOWER_TYPE,TOWER_TYPE_WIFI_MULTI		/*type*/
	_SAVE_EVENT_WORK_SET	LOCALWORK3,0									/*init*/
	_JUMP					ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*							バトルファクトリー						*/
/********************************************************************/
ev_bf_info_choice_02:
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_TYPE,FACTORY_TYPE_WIFI_MULTI	/*type*/
	_SAVE_EVENT_WORK_SET	LOCALWORK3,0									/*init*/
	_JUMP					ev_bf_info_choice_factory_level
	_END

ev_bf_info_choice_factory_level:
	/*「レベル５０と　オープンレベル　どちらに挑戦する？」*/
	_TALKMSG			msg_bf_info_020

	_BMPMENU_INIT_EX	20,11,0,1,FSW_ANSWER			/*Bキャンセル有効*/
	_BMPMENU_MAKE_LIST	msg_bf_info_choice_07,0			/*レベル５０*/
	_BMPMENU_MAKE_LIST	msg_bf_info_choice_08,1			/*オープンレベル*/
	_BMPMENU_MAKE_LIST	msg_bf_info_choice_06,2			/*やめる*/
	_BMPMENU_START

	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_choice_07_level
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_bf_info_choice_08_level
	_JUMP				bf_no_sel_menu					/*施設選択へ*/
	_END

ev_bf_info_choice_07_level:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_FACTORY_LV50,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_FACTORY_LV50

	//_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_LEVEL,50,0,FSW_ANSWER
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LEVEL,FACTORY_LEVEL_50			/*level*/
	_JUMP				ev_bf_info_send_retire_flag
	_END

ev_bf_info_choice_08_level:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_FACTORY_LV100,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_FACTORY_LV100

	//_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_LEVEL,100,0,FSW_ANSWER
	_SAVE_EVENT_WORK_SET	WK_SCENE_FACTORY_LEVEL,FACTORY_LEVEL_OPEN		/*level*/
	_JUMP				ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*							バトルキャッスル						*/
/********************************************************************/
ev_bf_info_choice_03:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_CASTLE,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_CASTLE
	_SAVE_EVENT_WORK_SET	WK_SCENE_CASTLE_TYPE,CASTLE_TYPE_WIFI_MULTI		/*type*/
	_SAVE_EVENT_WORK_SET	LOCALWORK3,0									/*init*/
	_JUMP					ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*							バトルステージ							*/
/********************************************************************/
ev_bf_info_choice_04:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_STAGE,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_STAGE
	_SAVE_EVENT_WORK_SET	WK_SCENE_STAGE_TYPE,STAGE_TYPE_WIFI_MULTI		/*type*/
	_SAVE_EVENT_WORK_SET	LOCALWORK3,0									/*init(08.06.28)*/
	_JUMP					ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*							バトルルーレット						*/
/********************************************************************/
ev_bf_info_choice_05:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_ROULETTE,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,0
	_LDVAL					FSW_PARAM0,FRONTIER_NO_ROULETTE
	_SAVE_EVENT_WORK_SET	WK_SCENE_ROULETTE_TYPE,ROULETTE_TYPE_WIFI_MULTI	/*type*/
	_SAVE_EVENT_WORK_SET	LOCALWORK3,0									/*init*/
	_JUMP					ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*								やめる								*/
/********************************************************************/
ev_bf_info_choice_06:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_SET_BF_NO,FRONTIER_NO_YAMERU,0,FSW_ANSWER
	_LDVAL					FSW_LOCAL4,1
	_JUMP					ev_bf_info_send_retire_flag
	_END


/********************************************************************/
/*						やめるを選んだか送信						*/
/********************************************************************/
ev_bf_info_send_retire_flag:
	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_005
	_JUMP				ev_bf_info_send_retire_flag_retry
	_END

ev_bf_info_send_retire_flag_retry:
	_LDVAL				FSW_PARAM1,FRONTIER_COMM_YAMERU
	_FR_WIFI_COUNTER_SEND_BUF	FSW_PARAM1,FSW_LOCAL4,0,FSW_ANSWER			/*1=やめる*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_send_retire_flag_retry	/*送信失敗は再送信*/

	_FR_WIFI_COUNTER_RECV_BUF	FSW_PARAM1
	_COMM_RESET

	/*自分が「やめる」選んだら*/
	//_IFVAL_JUMP			FSW_LOCAL4,EQ,1,ev_bf_info_game_end
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_BF_NO,0,0,FSW_ANSWER
	_IFVAL_JUMP				FSW_ANSWER,EQ,FRONTIER_NO_YAMERU,ev_bf_info_game_end

	/*相手が選んだメニューを取得*/
	_FR_WIFI_COUNTER_YAMERU_CHECK	FSW_ANSWER

	/*パートナーが「やめる」を選んだら*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,bf_no_sel_yameru

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_YAMERU
	_COMM_RESET

	_JUMP				ev_bf_info_sel_check
	_END


/********************************************************************/
/*						やめるが選ばれた							*/
/********************************************************************/
bf_no_sel_yameru:
	_JUMP				ev_bf_info_game_end_pair
	_END


/********************************************************************/
/*					選択した施設が同じかチェック					*/
/********************************************************************/
ev_bf_info_sel_check:
	_LDVAL				FSW_PARAM1,FRONTIER_COMM_SEL
	_FR_WIFI_COUNTER_SEND_BUF	FSW_PARAM1,0,0,FSW_ANSWER				/*施設No*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_sel_check			/*送信失敗は再送信*/

	_FR_WIFI_COUNTER_RECV_BUF	FSW_PARAM1
	_COMM_RESET

	/*お互いが選んだ施設ナンバーを比較*/
	_FR_WIFI_COUNTER_BFNO_CHECK	FSW_PARAM0,FSW_ANSWER

	_IFVAL_JUMP			FSW_ANSWER,EQ,1,ev_bf_info_sel_ok				/*OK*/

	/*「選んだ施設が相手と違います」*/
	_TALKMSG			msg_bf_info_004
	_JUMP				bf_no_sel_menu									/*施設選択へ*/
	_END


/********************************************************************/
/*						選択した施設が同じ							*/
/********************************************************************/
ev_bf_info_sel_ok:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_BF_NO,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_TOWER,ev_bf_info_pokelist_start	/*タワー*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_STAGE,ev_bf_info_pokelist_start	/*ステージ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_CASTLE,ev_bf_info_pokelist_start	/*キャッスル*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_ROULETTE,ev_bf_info_pokelist_start/*ルーレット*/
	_JUMP				ev_bf_info_pokelist_after	/*リスト呼び出しの必要なし*/
	_END


/********************************************************************/
/*						ポケモンリスト呼び出し						*/
/********************************************************************/
ev_bf_info_pokelist_start:
	/*「参加するポケモンを選んでください」*/
	_TALKMSG			msg_bf_info_003

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_TALK_CLOSE

	/*ポケモンリスト選択情報のクリア*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_POKELIST_SEL_CLEAR,0,0,FSW_ANSWER

	/*ポケモンリスト呼び出し*/
	_JUMP				ev_bf_info_pokelist_call
	_END

/*ポケモンステータス*/
ev_bf_info_pokestatus_call:
	_FR_WIFI_COUNTER_POKELIST	FSW_PARAM0,FR_WIFI_POKESEL_PST_CALL,FSW_ANSWER
	_FR_WIFI_COUNTER_POKELIST	FSW_PARAM0,FR_WIFI_POKESEL_PST_WAIT,FSW_ANSWER
	_JUMP						ev_bf_info_pokelist_call
	_END

/*ポケモンリスト*/
ev_bf_info_pokelist_call:
	_FR_WIFI_COUNTER_POKELIST	FSW_PARAM0,FR_WIFI_POKESEL_PLIST_CALL,FSW_ANSWER
	_FR_WIFI_COUNTER_POKELIST	FSW_PARAM0,FR_WIFI_POKESEL_PLIST_WAIT,FSW_ANSWER
	_IFVAL_JUMP					FSW_ANSWER,EQ,FR_WIFI_POKESEL_PST_CALL,ev_bf_info_pokestatus_call

	/*選択したポケモンの並び番号取得*/
	_FR_WIFI_COUNTER_POKELIST_GET_RESULT	FSW_LOCAL2,FSW_LOCAL5
	_SAVE_EVENT_WORK_SET	LOCALWORK2,FSW_LOCAL2						/*pos1*/
	_SAVE_EVENT_WORK_SET	LOCALWORK5,FSW_LOCAL5						/*pos2*/

	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_JUMP				ev_bf_info_send_pokelist_after
	_END

/********************************************************************/
/*				ポケモンリストの結果を送受信						*/
/********************************************************************/
ev_bf_info_send_pokelist_after:

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_005
	_JUMP				ev_bf_info_send_pokelist_after2
	_END

ev_bf_info_send_pokelist_after2:
	/*手持ち位置を送信して、ポケモンナンバーとアイテムナンバーをチェック*/
	/*リストでやめるを選んだかもチェック*/
	_LDVAL				FSW_PARAM1,FRONTIER_COMM_MONSNO_ITEMNO
	_FR_WIFI_COUNTER_SEND_BUF	FSW_PARAM1,FSW_LOCAL2,FSW_LOCAL5,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_send_pokelist_after2		/*送信失敗は再送信*/

	_FR_WIFI_COUNTER_RECV_BUF	FSW_PARAM1
	_COMM_RESET

	/*キャンセルチェック*/
	_IFVAL_JUMP			FSW_LOCAL2,EQ,0xff,bf_no_sel_menu				/*施設選択へ*/

	/*パートナーのキャンセルチェック*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_PAIR_POKELIST_CANCEL,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_pokelist_pair_cancel

	/*手持ちー＞ポケモン番号変換　同時にタマゴチェックも行っています*/
	//_TEMOTI_MONSNO		FSW_LOCAL2,FSW_LOCAL1
	//_IFVAL_JUMP			FSW_LOCAL1,EQ,0,ev_lobby_choice_04			/*タマゴ*/
	/*ポケモンリストでタマゴが選択出来ないのかも*/

	/*施設ごとに分岐*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_BF_NO,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_TOWER,ev_bf_info_pokelist_tower_result
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_STAGE,ev_bf_info_pokelist_stage_result
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_CASTLE,ev_bf_info_pokelist_castle_result
	_IFVAL_JUMP			FSW_ANSWER,EQ,FRONTIER_NO_ROULETTE,ev_bf_info_pokelist_roulette_result
	_END


/********************************************************************/
/*					パートナーのキャンセルチェック					*/
/********************************************************************/
ev_bf_info_pokelist_pair_cancel:
	/*「○さんの都合が悪いようです」*/
	_PAIR_NAME			0
	_TALKMSG			msg_bf_info_002
	_JUMP				bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*					タワーのポケモンリスト結果をチェック			*/
/********************************************************************/
ev_bf_info_pokelist_tower_result:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_CHECK_ENTRY,FRONTIER_NO_TOWER,0,FSW_ANSWER
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			1,ev_wifi_tower_gate_connect_poke_same01	/*1匹目が被り*/
	_CASE_JUMP			2,ev_wifi_tower_gate_connect_poke_same02	/*2匹目が被り*/
	_CASE_JUMP			3,ev_wifi_tower_gate_connect_poke_same03	/*1,2匹とも被り*/
	_JUMP				ev_bf_info_pokelist_after
	_END


/********************************************************************/
/*					タワー　ポケモンが被っている					*/
/********************************************************************/
/*ポケモンが一匹被っているとき(一匹目)*/
ev_wifi_tower_gate_connect_poke_same01:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_010
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが一匹被っているとき(二匹目)*/
ev_wifi_tower_gate_connect_poke_same02:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_010
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが二匹被っているとき*/
ev_wifi_tower_gate_connect_poke_same03:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0

	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		1,FSW_ANSWER,0,0

	_TALKMSG				msg_bf_info_011
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*				ステージのポケモンリスト結果をチェック				*/
/********************************************************************/
ev_bf_info_pokelist_stage_result:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_CHECK_ENTRY,FRONTIER_NO_STAGE,0,FSW_ANSWER
	_IFVAL_JUMP				FSW_ANSWER,EQ,1,ev_wifi_stage_gate_connect_poke_same01	/*違うmonsno*/

#if 0
	相手が記録を消して挑戦していると、

	消していない側は、連勝中で、
	消した側は、、初挑戦になるので、

	連勝中のポケモンがいるかなどのチェックの前に、
	連勝数が食い違っているかをチェックする必要がある

	友達はインデックスとプロファイルIDがあるので、
	再度同じインデックスに同じ友達を登録しても、
	違うプロファイルIDになる
#endif

	/*ステージのみ先に、連勝数が食い違っているかチェック*/
	_CALL				ev_bf_info_pair_rensyou_ng_check_call
	_IFVAL_JUMP			FSW_PARAM6,EQ,0,ev_bf_info_pokelist_after_sub	/*食い違っていたら新規挑戦*/

	/*連勝中かチェック*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_STAGE_GET_CLEAR_FLAG,0,0,FSW_ANSWER
	_IFVAL_JUMP				FSW_ANSWER,EQ,0,ev_bf_info_pokelist_after				/*新規挑戦*/

	/*連勝中のポケモンナンバーと一致するかチェック*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_STAGE_GET_CLEAR_MONSNO,0,0,FSW_PARAM2
	_POKEMON_NAME_EXTRA		0,FSW_PARAM2,0,0

	/*出場ポケモンナンバー取得*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		1,FSW_ANSWER,0,0
	_IFWK_JUMP				FSW_ANSWER,EQ,FSW_PARAM2,ev_bf_info_pokelist_after		/*連勝中*/

	/*連勝中で、新規ポケモンで挑戦しようとした時*/
	_JUMP			ev_bf_info_stage_new
	_END


/********************************************************************/
/*					ステージ　記録があるけどどうする？				*/
/********************************************************************/
ev_bf_info_stage_new:
	/*「○での連勝記録がありますが、記録を消して□で新たに挑戦しますか？」*/
	_TALKMSG			msg_bf_info_025
	_BMPMENU_INIT_EX	25,13,1,1,FSW_ANSWER			/*Bキャンセル有効*/
	_BMPMENU_MAKE_LIST	msg_bf_info_025_01,0			/*はい*/
	_BMPMENU_MAKE_LIST	msg_bf_info_025_02,1			/*いいえ*/
	_BMPMENU_START

	/*「相手の選択を待っています！しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_031

	_LDWK				FSW_PARAM2,FSW_ANSWER			/*退避*/
	_JUMP				ev_bf_info_stage_new_retry
	_END

ev_bf_info_stage_new_retry:
	_LDVAL				FSW_PARAM1,FRONTIER_COMM_STAGE_RECORD_DEL
	_FR_WIFI_COUNTER_SEND_BUF	FSW_PARAM1,FSW_PARAM2,0,FSW_ANSWER		/*0=はい、1=いいえ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_stage_new_retry		/*送信失敗は再送信*/

	_FR_WIFI_COUNTER_RECV_BUF	FSW_PARAM1
	_COMM_RESET

	/*相手が「いいえ」を選んだ時*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_PAIR_STAGE_RECORD_DEL,0,0,FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,1,bf_no_sel_yameru_stage
	_IFVAL_JUMP			FSW_ANSWER,EQ,FSEV_WIN_B_CANCEL,bf_no_sel_yameru_stage

	/*自分が「いいえ」を選んだ時*/
	_IFVAL_JUMP			FSW_PARAM2,EQ,1,bf_no_sel_yameru_stage
	_IFVAL_JUMP			FSW_PARAM2,EQ,FSEV_WIN_B_CANCEL,bf_no_sel_yameru_stage

	/*お互いが「はい」を選んだ*/
	_JUMP				ev_bf_info_stage_new_yes		/*はい*/
	_END

/*「もう一度選択してください」*/
bf_no_sel_yameru_stage:
	_TALKMSG			msg_bf_info_026
	_JUMP				bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*					ステージ　記録を消す　はい						*/
/********************************************************************/
ev_bf_info_stage_new_yes:
	/*タイプごとのレベル、7連勝したかフラグのクリア、現在の連勝数をクリア*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_STAGE_SET_NEW_CHALLENGE,0,0,FSW_ANSWER
	_JUMP					ev_bf_info_pokelist_after
	_END


/********************************************************************/
/*					ステージ　ポケモンが同じでない					*/
/********************************************************************/
ev_wifi_stage_gate_connect_poke_same01:
	_TALKMSG			msg_bf_info_015
	_JUMP				bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*				キャッスルのポケモンリスト結果をチェック			*/
/********************************************************************/
ev_bf_info_pokelist_castle_result:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_CHECK_ENTRY,FRONTIER_NO_CASTLE,0,FSW_ANSWER
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			1,ev_wifi_castle_gate_connect_poke_same01	/*1匹目が被り*/
	_CASE_JUMP			2,ev_wifi_castle_gate_connect_poke_same02	/*2匹目が被り*/
	_CASE_JUMP			3,ev_wifi_castle_gate_connect_poke_same03	/*1,2匹とも被り*/
	_JUMP				ev_bf_info_pokelist_after
	_END


/********************************************************************/
/*					キャッスル　ポケモンが被っている				*/
/********************************************************************/
/*ポケモンが一匹被っているとき(一匹目)*/
ev_wifi_castle_gate_connect_poke_same01:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_013
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが一匹被っているとき(二匹目)*/
ev_wifi_castle_gate_connect_poke_same02:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_013
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが二匹被っているとき*/
ev_wifi_castle_gate_connect_poke_same03:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0

	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		1,FSW_ANSWER,0,0

	_TALKMSG				msg_bf_info_014
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*	ルーレットのポケモンリスト結果をチェック(キャッスルと処理同じ)	*/
/********************************************************************/
ev_bf_info_pokelist_roulette_result:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_CHECK_ENTRY,FRONTIER_NO_ROULETTE,0,FSW_ANSWER
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			1,ev_wifi_roulette_gate_connect_poke_same01	/*1匹目が被り*/
	_CASE_JUMP			2,ev_wifi_roulette_gate_connect_poke_same02	/*2匹目が被り*/
	_CASE_JUMP			3,ev_wifi_roulette_gate_connect_poke_same03	/*1,2匹とも被り*/
	_JUMP				ev_bf_info_pokelist_after
	_END


/********************************************************************/
/*		ルーレット　ポケモンが被っている(キャッスルと処理同じ)		*/
/********************************************************************/
/*ポケモンが一匹被っているとき(一匹目)*/
ev_wifi_roulette_gate_connect_poke_same01:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_013
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが一匹被っているとき(二匹目)*/
ev_wifi_roulette_gate_connect_poke_same02:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0
	_TALKMSG				msg_bf_info_013
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END

/*ポケモンが二匹被っているとき*/
ev_wifi_roulette_gate_connect_poke_same03:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,0,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		0,FSW_ANSWER,0,0

	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_ENTRY_MONSNO,1,0,FSW_ANSWER
	_POKEMON_NAME_EXTRA		1,FSW_ANSWER,0,0

	_TALKMSG				msg_bf_info_014
	_JUMP					bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*						ポケモンリスト終了後						*/
/********************************************************************/
ev_bf_info_pokelist_after:

	/*連勝数が食い違っているかチェック*/
	_CALL				ev_bf_info_pair_rensyou_ng_check_call

	_JUMP				ev_bf_info_pokelist_after_sub
	_END

ev_bf_info_pokelist_after_sub:
#if 1
	/*クリアフラグを取得してクリア*/
	/*クリアフラグを取得して、0だったら、連勝レコードを0にしておく(友達手帳の記録のため)*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_CLEAR_FLAG,0,0,FSW_ANSWER
	_SAVE_EVENT_WORK_SET	SYS_WORK_WIFI_FR_CLEAR_FLAG,FSW_ANSWER
#endif

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_SAVE
	_COMM_RESET

	/*「レポートに書き込んでいます　電源を切らないで下さい」*/
	_TALKMSG_ALL_PUT	msg_bf_info_028
	_ADD_WAITICON
	_REPORT_DIV_SAVE	FSW_ANSWER
	_DEL_WAITICON
	_SE_PLAY			SEQ_SE_DP_SAVE
	_SE_WAIT			SEQ_SE_DP_SAVE

	/*「それではがんばってください」*/
	_TALKMSG_NOSKIP		msg_bf_info_027
	_TIME_WAIT			10, FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_GO
	_COMM_RESET

	_TALK_CLOSE

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_ACTOR_FREE			OBJID_PLAYER
	//_ACTOR_FREE			OBJID_PCWOMAN2_01
	_ACTOR_FREE			OBJID_SIO_USER_0
	_ACTOR_FREE			OBJID_SIO_USER_1
	//_CHAR_RESOURCE_FREE	PCWOMAN2

	_RECORD_INC			RECID_FRONTIER_COUNT

	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_BF_NO,0,0,FSW_ANSWER
	_SWITCH				FSW_ANSWER
	_CASE_JUMP			FRONTIER_NO_TOWER,ev_wifi_tower_start
	_CASE_JUMP			FRONTIER_NO_FACTORY_LV50,ev_wifi_factory_start
	_CASE_JUMP			FRONTIER_NO_FACTORY_LV100,ev_wifi_factory_start
	_CASE_JUMP			FRONTIER_NO_STAGE,ev_wifi_stage_start
	_CASE_JUMP			FRONTIER_NO_CASTLE,ev_wifi_castle_start
	_CASE_JUMP			FRONTIER_NO_ROULETTE,ev_wifi_roulette_start
	_END


/********************************************************************/
/*				連勝数が食い違っているかチェック*/
/********************************************************************/
ev_bf_info_pair_rensyou_ng_check_call:
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_CHECK_PAIR_RENSYOU,0,0,FSW_ANSWER

	/*結果を退避*/
	_LDWK				FSW_PARAM6,FSW_ANSWER

	_IFVAL_CALL			FSW_ANSWER,EQ,0,ev_bf_info_pair_rensyou_ng
	_RET

/*連勝数が食い違っていたことを伝える*/
ev_bf_info_pair_rensyou_ng:
	_TALKMSG			msg_bf_info_006
	_TALKMSG_NOSKIP		msg_bf_info_005
	_RET


/********************************************************************/
/********************************************************************/
ev_wifi_tower_start:
	/*タワー画面呼び出し前*/

	/*特殊*/
	//_FR_WIFI_COUNTER_WORK_FREE
	_FR_WIFI_COUNTER_TOWER_CALL_BEFORE				/*この中でワーク確保 BTWR_MODE_WIFI_MULTI*/

	/*マルチENTRYを設定*/
	_SAVE_EVENT_WORK_SET	WK_SCENE_D31R0206,1		/*new game マルチ*/

	/*タワーのデータ交換前の同期*/
	_CALL				ev_wifi_counter_synchronize_tower

	_JUMP				ev_wifi_tower_tr_data_retry
	_END

/*対戦トレーナーデータ送信*/
ev_wifi_tower_tr_data_retry:
	_FR_WIFI_COUNTER_TOWER_SEND_TR_DATA	FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_wifi_tower_tr_data_retry

	/*対戦トレーナーデータ受信*/
	_FR_WIFI_COUNTER_TOWER_RECV_TR_DATA

	/*特殊*/
	//tower6.sの中で_FR_WIFI_COUNTER_TOWER_CALL_AFTERを呼んでいる

	/*通信同期*/
	_CALL				ev_wifi_counter_synchronize
	_MAP_CHANGE_EX		FSS_SCENEID_TOWER_MULTI_WAY,0
	_END

ev_wifi_factory_start:
	_FR_WIFI_COUNTER_WORK_FREE

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_PL_BF_FACTORY

	/*通信同期*/
	_CALL				ev_wifi_counter_synchronize
	_MAP_CHANGE_EX		FSS_SCENEID_FACTORY,0
	_END

ev_wifi_stage_start:
	_FR_WIFI_COUNTER_WORK_FREE

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_PL_BF_STAGE

	/*通信同期*/
	_CALL				ev_wifi_counter_synchronize
	_MAP_CHANGE_EX		FSS_SCENEID_STAGE_WAY,0
	_END

ev_wifi_castle_start:
	_FR_WIFI_COUNTER_WORK_FREE

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_PL_BF_CASTLE02

	/*通信同期*/
	_CALL				ev_wifi_counter_synchronize
	_MAP_CHANGE_EX		FSS_SCENEID_CASTLE_ROOM,0
	_END

ev_wifi_roulette_start:
	_FR_WIFI_COUNTER_WORK_FREE

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_PL_BF_ROULETTE

	/*通信同期*/
	_CALL				ev_wifi_counter_synchronize
	_MAP_CHANGE_EX		FSS_SCENEID_ROULETTE_WAY,0
	_END

/*マップ遷移前の同期*/
ev_wifi_counter_synchronize:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_GO2
	_COMM_RESET
	_RET

/*タワーのデータ交換前の同期*/
ev_wifi_counter_synchronize_tower:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_TOWER
	_COMM_RESET
	_RET


/********************************************************************/
/*						施設から戻ってきた時						*/
/********************************************************************/
fss_wifi_counter_return:

	/*日時の記録をつける*/
	_FR_WIFI_COUNTER_LIST_SET_LAST_PLAY_DATE

	_FR_WIFI_COUNTER_WORK_ALLOC
	//_FR_WIFI_COUNTER_WORK_FREE

	_TIME_WAIT			1,FSW_ANSWER
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_COMM_COMMAND_INITIALIZE,0,0,FSW_ANSWER
	_TIME_WAIT			1,FSW_ANSWER

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_INIT_2
	_COMM_RESET

	_CHAR_RESOURCE_SET	comm_multi_set_resource				/*キャラクタリソース登録*/
	_ACTOR_SET			comm_multi_set_actor				/*アクター登録*/

	/*BGMのフェードを入れないとダメかも*/
	_BGM_PLAY			SEQ_BF_TOWWER

	_BLACK_IN			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	/*「○さんともう一度遊びますか？」*/
	_PAIR_NAME			0
	_TALKMSG			msg_bf_info_030
	_YES_NO_WIN			FSW_ANSWER
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_wifi_counter_return_yes
	_JUMP				ev_wifi_counter_return_no
	_END

ev_wifi_counter_return_yes:
	/*「相手の選択を待っています！しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_031
	_LDVAL				FSW_PARAM2,0
	_JUMP				ev_bf_info_send_game_continue
	_END
	
ev_wifi_counter_return_no:
	/*「相手の選択を待っています！しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_031
	_LDVAL				FSW_PARAM2,1
	_JUMP				ev_bf_info_send_game_continue
	_END


/********************************************************************/
/*						引き続き遊ぶかを送信						*/
/********************************************************************/
ev_bf_info_send_game_continue:
	_LDVAL				FSW_PARAM1,FRONTIER_COMM_GAME_CONTINUE
	_FR_WIFI_COUNTER_SEND_BUF	FSW_PARAM1,FSW_PARAM2,0,FSW_ANSWER		/*0=はい、1=いいえ*/
	_IFVAL_JUMP			FSW_ANSWER,EQ,0,ev_bf_info_send_game_continue	/*送信失敗は再送信*/

	_FR_WIFI_COUNTER_RECV_BUF	FSW_PARAM1
	_COMM_RESET

	/*相手が選んだメニューを取得*/
	_FR_WIFI_COUNTER_YAMERU_CHECK	FSW_ANSWER

	/*パートナーが「いいえ」を選んだら*/
	_FR_WIFI_COUNTER_TOOL	FWIFI_ID_GET_PAIR_GAME_CONTINUE,0,0,FSW_ANSWER
	_IFVAL_JUMP				FSW_ANSWER,EQ,1,ev_bf_info_game_end_pair

	/*自分が「いいえ」を選んだら*/
	_IFVAL_JUMP			FSW_PARAM2,EQ,1,ev_bf_info_game_end

	_JUMP				bf_no_sel_menu					/*施設選択へ*/
	_END


/********************************************************************/
/*					(友達)引き続き遊ばない							*/
/********************************************************************/
ev_bf_info_game_end_pair:
	_PAIR_NAME			0
	_TALKMSG_NOSKIP		msg_bf_info_032

	/*「しばらくお待ちください」*/
	_TALKMSG_NOSKIP		msg_bf_info_005

	_JUMP				ev_bf_info_game_end
	_END
	
/********************************************************************/
/*						引き続き遊ばない							*/
/********************************************************************/
ev_bf_info_game_end:
	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_END
	_COMM_RESET

	_TALK_CLOSE
	_JUMP				ev_bf_info_end
	_END


/********************************************************************/
/*						ワーク開放して終了							*/
/********************************************************************/
ev_bf_info_end:
	/*WiFiマッチング画面へ*/
	//_END

	_BLACK_OUT			SCR_WIPE_DIV,SCR_WIPE_SYNC
	_WIPE_FADE_END_CHECK

	_FR_WIFI_COUNTER_WORK_FREE

	/*通信同期*/
	_COMM_RESET
	_COMM_SYNCHRONIZE	DBC_TIM_FR_WIFI_COUNTER_END_2
	_COMM_RESET

	_SCRIPT_FINISH		/*_ENDして2Dマップ終了*/


