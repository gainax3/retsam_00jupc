//==============================================================================
/**
 * @file	frontier_seq_def.h
 * @brief	フロンティア用スクリプトコマンドのマクロ定義ファイル
 * @author	matsuda
 * @date	2007.03.28(水)
 */
//==============================================================================

#define	__ASM_NO_DEF_
//	.include	"../../../include/battle/battle_common.h"
//	.include	"../../../include/battle/battle_server.h"
//	.include	"../../../include/battle/wazano_def.h"
//	.include	"../../../include/battle/tokusyu_def.h"
//	.include	"../../../include/msgdata/msg_fightmsg_dp.h"
//	.include	"../../../include/poketool/poke_tool.h"
//	.include	"../../../include/poketool/tr_tool.h"
//	.include	"../../../include/poketool/waza_tool.h"
//	.include	"../../../include/itemtool/itemequip.h"
//	.include	"../../../include/savedata/record.h"
//	.include	"../battle_snd_def.h"
	.include	"../../data/sound/pl_sound_data.sadl"

#define __ASM_ONLY_INC_
	.include	"../fs_usescript.h"

//データを自然な境界にアラインするかの設定
	.option alignment off

	.macro	INIT_CMD
DEF_CMD_COUNT	=	0
	.endm
		
	.macro	DEF_CMD	symname
\symname	=	DEF_CMD_COUNT
DEF_CMD_COUNT	=	( DEF_CMD_COUNT + 1 )
	.endm

//命令シンボル宣言
	INIT_CMD
	DEF_CMD	FR_DUMMY
	DEF_CMD	FR_END
	DEF_CMD FR_SCRIPT_FINISH
	DEF_CMD FR_MAP_CHANGE
	DEF_CMD FR_MAP_CHANGE_EX
	DEF_CMD	FR_TIME_WAIT
	DEF_CMD FR_LD_WK_VAL
	DEF_CMD FR_LD_WK_WK
	DEF_CMD FR_ADD_WK
	DEF_CMD FR_SUB_WK
	DEF_CMD FR_JUMP
	DEF_CMD FR_IF_JUMP
	DEF_CMD FR_CALL
	DEF_CMD FR_RET
	DEF_CMD FR_IF_CALL
	DEF_CMD	FR_TALK_MSG_ALL_PUT
	DEF_CMD	FR_TALK_MSG_NO_SKIP
	DEF_CMD	FR_TALK_MSG
	DEF_CMD FR_TALK_WIN_CLOSE
	DEF_CMD FR_WIPE_FADE_START
	DEF_CMD FR_WIPE_FADE_END_CHECK
	DEF_CMD FR_BMPMENU_INIT
	DEF_CMD FR_BMPMENU_INIT_EX
	DEF_CMD FR_BMPMENU_MAKE_LIST
	DEF_CMD FR_BMPMENU_MAKE_LIST_TALK_MSG
	DEF_CMD FR_BMPMENU_START
	DEF_CMD FR_BMPLIST_INIT
	DEF_CMD FR_BMPLIST_INIT_EX
	DEF_CMD FR_BMPLIST_MAKE_LIST
	DEF_CMD FR_BMPLIST_START
	DEF_CMD FR_BMPLIST_DEL
	DEF_CMD FR_YES_NO_WIN
	DEF_CMD FR_CP_WK_VAL
	DEF_CMD FR_CP_WK_WK
	DEF_CMD FR_CHAR_RESOURCE_SET
	DEF_CMD FR_CHAR_RESOURCE_FREE
	DEF_CMD FR_ACTOR_SET
	DEF_CMD FR_ACTOR_FREE
	DEF_CMD FR_ACTOR_VISIBLE_SET
	DEF_CMD FR_ACTOR_BG_PRI_SET
	DEF_CMD FR_OBJ_ANIME
	DEF_CMD FR_OBJ_ANIME_WAIT
	DEF_CMD FR_ARTICLE_RESOURCE_SET
	DEF_CMD FR_ARTICLE_RESOURCE_FREE
	DEF_CMD FR_ARTICLE_ACTOR_SET
	DEF_CMD FR_ARTICLE_ACTOR_FREE
	DEF_CMD FR_ARTICLE_ACTOR_VISIBLE_SET
	DEF_CMD FR_ARTICLE_ACTOR_FLIP_H_SET
	DEF_CMD FR_ARTICLE_ACTOR_OBJ_MODE_SET
	DEF_CMD FR_ARTICLE_ACTOR_ANIME_START
	DEF_CMD FR_ARTICLE_ACTOR_ANIME_STOP
	DEF_CMD FR_ARTICLE_ACTOR_ANIME_WAIT
	DEF_CMD FR_NAMEIN_PROC
	DEF_CMD	FR_COMM_TIMING_SYNC_START
	DEF_CMD	FR_COMM_TEMP_DATA_RESET
	DEF_CMD	FR_COMM_DIRECT_END
	DEF_CMD	FR_GET_RND
	DEF_CMD	FR_PC_KAIFUKU
	DEF_CMD	FR_ABKEY_WAIT
	DEF_CMD	FR_ABKEY_TIMEWAIT
	DEF_CMD	FR_LOSE_CHECK
	DEF_CMD	FR_SAVE_EVENT_WORK_SET			//セーブワークに値をセット
	DEF_CMD	FR_SAVE_EVENT_WORK_GET			//セーブワークの値を取得
	DEF_CMD FR_ENCOUNT_EFFECT
	DEF_CMD FR_GET_MINE_OBJ
	DEF_CMD FR_PARTICLE_SPA_LOAD
	DEF_CMD FR_PARTICLE_SPA_EXIT
	DEF_CMD FR_PARTICLE_ADD_EMITTER
	DEF_CMD FR_PARTICLE_WAIT
	DEF_CMD FR_TEMOTI_POKE_ACT_ADD
	DEF_CMD FR_TEMOTI_POKE_ACT_DEL
	DEF_CMD FR_BRAIN_ENCOUNT_EFFECT
	DEF_CMD FR_RECORD_INC
	DEF_CMD FR_RECORD_ADD
	DEF_CMD FR_SCORE_ADD
	DEF_CMD FR_CHECK_DP_ROM_CODE
	DEF_CMD FR_SHAKE_SET
	DEF_CMD FR_SHAKE_WAIT
	DEF_CMD FR_WND_SET
	DEF_CMD FR_WND_WAIT
	
	//CPウィンドウ
	DEF_CMD	FR_CP_WIN_WRITE
	DEF_CMD	FR_CP_WIN_DEL
	DEF_CMD	FR_CP_WRITE
	//OBJ動作コード
	DEF_CMD FR_OBJMOVE_RESET
	DEF_CMD FR_OBJMOVE_KYORO
	//サウンド
	DEF_CMD	FR_SE_PLAY
	DEF_CMD	FR_SE_STOP
	DEF_CMD	FR_SE_WAIT
	DEF_CMD	FR_ME_PLAY
	DEF_CMD	FR_ME_WAIT
	DEF_CMD	FR_BGM_PLAY
	DEF_CMD	FR_BGM_STOP
	//ファクトリー	
	DEF_CMD	FR_FACTORY_WORK_ALLOC		//ワーク確保
	DEF_CMD	FR_FACTORY_WORK_INIT		//ワーク初期化
	DEF_CMD	FR_FACTORY_WORK_FREE		//ワーク開放
	DEF_CMD	FR_FACTORY_RENTAL_CALL		//ファクトリーレンタル呼び出し
	DEF_CMD	FR_FACTORY_CALL_GET_RESULT	//ファクトリー呼び出し結果取得
	DEF_CMD	FR_FACTORY_BATTLE_CALL		//ファクトリー戦闘呼び出し
	DEF_CMD	FR_FACTORY_TRADE_CALL		//ファクトリー交換呼び出し
	DEF_CMD	FR_FACTORY_RENTAL_PARTY_SET			//ファクトリーレンタルPOKEPARTYのセット
	DEF_CMD	FR_FACTORY_BTL_AFTER_PARTY_SET		//ファクトリー戦闘後のPOKEPARTYのセット
	DEF_CMD	FR_FACTORY_TRADE_POKE_CHANGE		//ファクトリートレード後のポケモン変更
	DEF_CMD	FR_FACTORY_TRADE_AFTER_PARTY_SET	//ファクトリートレード後のPOKEPARTYのセット
	DEF_CMD	FR_FACTORY_TOOL				//ファクトリーFACTORY_SCRWORKの操作
	DEF_CMD	FR_FACTORY_LOSE_CHECK		//ファクトリー敗北チェック
	DEF_CMD	FR_FACTORY_SEND_BUF				//ファクトリー通信send_buf
	DEF_CMD	FR_FACTORY_RECV_BUF				//ファクトリー通信recv_buf
	DEF_CMD	FR_FACTORY_APPEAR_MSG
	//録画
	DEF_CMD	FR_BATTLE_REC_INIT
	DEF_CMD	FR_BATTLE_REC_SAVE
	DEF_CMD	FR_BATTLE_REC_LOAD
	DEF_CMD	FR_BATTLE_REC_EXIT
	DEF_CMD	FR_BATTLE_REC_DATA_OCC_CHECK
	DEF_CMD	FR_BATTLE_REC_SERVER_VER_CHK

	//セーブ
	DEF_CMD	FR_REPORT_SAVE
	DEF_CMD	FR_REPORT_DIV_SAVE
	DEF_CMD	FR_EXTRA_SAVE_INIT
	DEF_CMD	FR_EXTRA_SAVE_INIT_CHECK
	DEF_CMD	FR_STAGE_RENSHOU_COPY_EXTRA
	DEF_CMD FR_WAITICON_ADD
	DEF_CMD FR_WAITICON_DEL
	//ワードセット
	DEF_CMD	FR_ITEM_NAME
	DEF_CMD	FR_NUMBER_NAME
	DEF_CMD	FR_PLAYER_NAME
	DEF_CMD	FR_PAIR_NAME
	DEF_CMD	FR_WAZA_NAME
	DEF_CMD	FR_POKEMON_NAME_EXTRA
	DEF_CMD	FR_TYPE_NAME
	DEF_CMD	FR_RIVAL_NAME
	//バトルポイント
	DEF_CMD	FR_GET_BTL_POINT
	DEF_CMD	FR_BTL_POINT_ADD
	DEF_CMD	FR_BTL_POINT_SUB
	//バトルタワー
	DEF_CMD	FR_BTOWER_TOOLS
	DEF_CMD	FR_BTOWER_APPEAR_MSG
	DEF_CMD	FR_BTOWER_WORK_RELEASE		//バトルタワーワーク解放
	DEF_CMD	FR_BTOWER_BATTLE_CALL
	DEF_CMD	FR_BTOWER_CALL_GET_RESULT	//バトルタワー呼び出し結果取得
	DEF_CMD	FR_BTOWER_SEND_BUF			//バトルタワー汎用データ送信
	DEF_CMD	FR_BTOWER_RECV_BUF			//バトルタワー汎用データ受信
	//ステージ
	DEF_CMD	FR_STAGE_WORK_ALLOC			//ワーク確保
	DEF_CMD	FR_STAGE_WORK_ENEMY_SET		//敵トレーナー、敵ポケモンセット
	DEF_CMD	FR_STAGE_WORK_FREE			//ワーク開放
	DEF_CMD	FR_STAGE_POKE_TYPE_SEL_CALL	//ステージポケモンタイプ選択画面呼び出し
	DEF_CMD	FR_STAGE_CALL_GET_RESULT	//ステージ呼び出し結果取得
	DEF_CMD	FR_STAGE_BATTLE_CALL		//ステージ戦闘呼び出し
	DEF_CMD	FR_STAGE_TOOL				//ステージFACTORY_SCRWORKの操作
	DEF_CMD	FR_STAGE_LOSE_CHECK			//ステージ敗北チェック
	DEF_CMD	FR_STAGE_SEND_BUF			//ステージ通信send_buf
	DEF_CMD	FR_STAGE_RECV_BUF			//ステージ通信recv_buf
	DEF_CMD	FR_STAGE_APPEAR_MSG
	//
	DEF_CMD	FR_DEBUG_PRINT
	//キャッスル
	DEF_CMD	FR_CASTLE_WORK_ALLOC		//ワーク確保
	DEF_CMD	FR_CASTLE_WORK_INIT			//ワーク初期化
	DEF_CMD	FR_CASTLE_WORK_FREE			//ワーク開放
	DEF_CMD	FR_CASTLE_MINE_CALL			//手持ち画面呼び出し
	DEF_CMD	FR_CASTLE_CALL_GET_RESULT	//キャッスル呼び出し結果取得
	DEF_CMD	FR_CASTLE_BATTLE_CALL		//キャッスル戦闘呼び出し
	DEF_CMD	FR_CASTLE_ENEMY_CALL		//敵トレーナー呼び出し
	DEF_CMD	FR_CASTLE_BTL_BEFORE_PARTY_SET	//キャッスル戦闘前のPOKEPARTYのセット
	DEF_CMD	FR_CASTLE_BTL_AFTER_PARTY_SET	//キャッスル戦闘後のPOKEPARTYのセット
	DEF_CMD	FR_CASTLE_TOOL				//キャッスルCASTLE_SCRWORKの操作
	DEF_CMD	FR_CASTLE_LOSE_CHECK		//キャッスル敗北チェック
	DEF_CMD	FR_CASTLE_SEND_BUF				//キャッスル通信send_buf
	DEF_CMD	FR_CASTLE_RECV_BUF				//キャッスル通信recv_buf
	DEF_CMD	FR_CASTLE_RANKUP_CALL		//ランクアップ呼び出し
	DEF_CMD	FR_CASTLE_PARENT_CHECK_WAIT		//親の決定待ち
	DEF_CMD	FR_CASTLE_GET_CP
	DEF_CMD	FR_CASTLE_SUB_CP
	DEF_CMD	FR_CASTLE_ADD_CP
	DEF_CMD	FR_CASTLE_APPEAR_MSG
	//WiFi受付
	DEF_CMD	FR_WIFI_COUNTER_WORK_ALLOC			//
	DEF_CMD	FR_WIFI_COUNTER_WORK_FREE			//
	DEF_CMD	FR_WIFI_COUNTER_SEND_BUF			//WiFi受付通信send_buf
	DEF_CMD	FR_WIFI_COUNTER_RECV_BUF			//WiFi受付通信recv_buf
	DEF_CMD	FR_WIFI_COUNTER_BFNO_CHECK			//
	DEF_CMD	FR_WIFI_COUNTER_POKELIST			//
	DEF_CMD	FR_WIFI_COUNTER_POKELIST_GET_RESULT	//
	DEF_CMD	FR_WIFI_COUNTER_YAMERU_CHECK		//
	DEF_CMD	FR_WIFI_COUNTER_TOOL				//FRWIFI_SCRWORKの操作
	DEF_CMD	FR_WIFI_COUNTER_TOWER_CALL_BEFORE	//
	DEF_CMD	FR_WIFI_COUNTER_TOWER_SEND_TR_DATA	//
	DEF_CMD	FR_WIFI_COUNTER_TOWER_RECV_TR_DATA	//
	DEF_CMD	FR_WIFI_COUNTER_TOWER_CALL_AFTER	//
	DEF_CMD	FR_WIFI_COUNTER_LIST_SET_LAST_PLAY_DATE	//
	//ルーレット
	DEF_CMD	FR_ROULETTE_WORK_ALLOC				//ワーク確保
	DEF_CMD	FR_ROULETTE_WORK_INIT				//ワーク初期化
	DEF_CMD	FR_ROULETTE_WORK_FREE				//ワーク開放
	DEF_CMD	FR_ROULETTE_CALL_GET_RESULT			//ルーレット呼び出し結果取得
	DEF_CMD	FR_ROULETTE_BATTLE_CALL				//ルーレット戦闘呼び出し
	DEF_CMD	FR_ROULETTE_BTL_BEFORE_PARTY_SET	//ルーレット戦闘前のPOKEPARTYのセット
	DEF_CMD	FR_ROULETTE_BTL_AFTER_PARTY_SET		//ルーレット戦闘後のPOKEPARTYのセット
	DEF_CMD	FR_ROULETTE_TOOL					//ルーレットCASTLE_SCRWORKの操作
	DEF_CMD	FR_ROULETTE_LOSE_CHECK				//ルーレット敗北チェック
	DEF_CMD	FR_ROULETTE_SEND_BUF				//ルーレット通信send_buf
	DEF_CMD	FR_ROULETTE_RECV_BUF				//ルーレット通信recv_buf
	DEF_CMD	FR_ROULETTE_CALL					//ルーレット画面呼び出し
	DEF_CMD	FR_ROULETTE_APPEAR_MSG
	DEF_CMD	FR_ROULETTE_DECIDE_EV_NO_FUNC		//決定したパネルイベント処理実行
	DEF_CMD	FR_ROULETTE_CHG_ITEMKEEP_VANISH		//アイテム持っているアイコンのバニッシュ

	DEF_CMD	FR_TV_TEMP_STAGE_SET
	DEF_CMD	FR_TV_TEMP_FRIEND_SET
	DEF_CMD	FR_COMM_SET_WIFI_BOTH_NET

//=============================================================================
/**
 *	イベントデータ関連
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	イベントデータ　テーブル宣言
 */
//-----------------------------------------------------------------------------
	.macro	_EVENT_DATA	adrs
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	イベントデータ　テーブル終了
 */
//-----------------------------------------------------------------------------
	.macro	_EVENT_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//==============================================================================
//
//	リソース関連
//
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	リソースデータラベル
 */
//-----------------------------------------------------------------------------
	.macro	_RESOURCE_LABEL label
	.align	4
	\label:
	.endm

//--------------------------------------------------------------
/**
 * リソースデータ
 */
//--------------------------------------------------------------
	.macro	_CHAR_RESOURCE_DATA	charid, movetype
	.short	\charid
	.byte	\movetype
	.endm

//--------------------------------------------------------------
/**
 * プレイヤーキャラ(HEROorHEROINE)のキャラクタリソースデータ
 */
//--------------------------------------------------------------
	.macro	_PLAYER_RESOURCE_DATA
	_CHAR_RESOURCE_DATA		FSS_CODE_MYSELF, 0, WF2DC_C_MOVERUN
	.endm

//--------------------------------------------------------------
/**
 * 通信相手のプレイヤーキャラ(HEROorHEROINE)のキャラクタリソースデータ
 */
//--------------------------------------------------------------
	.macro	_SIO_PLAYER_RESOURCE_DATA
	_CHAR_RESOURCE_DATA		FSS_CODE_SIO_PLAYER, 0, WF2DC_C_MOVERUN
	.endm

//--------------------------------------------------------------
/**
 * リソースデータ終端
 */
//--------------------------------------------------------------
	.macro	_CHAR_RESOURCE_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//--------------------------------------------------------------
/**
 * キャラクタリソース登録
 */
//--------------------------------------------------------------
	.macro	_CHAR_RESOURCE_SET	adrs
	.short	FR_CHAR_RESOURCE_SET
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * キャラクタリソース削除
 */
//--------------------------------------------------------------
	.macro	_CHAR_RESOURCE_FREE	charid
	.short	FR_CHAR_RESOURCE_FREE
	.short	\charid
	.endm


//==============================================================================
//
//	アクター関連
//
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	アクターデータラベル
 */
//-----------------------------------------------------------------------------
	.macro	_ACTOR_LABEL label
	.align	4
	\label:
	.endm

//--------------------------------------------------------------
/**
 * アクターデータ(PLAYID指定)
 */
//--------------------------------------------------------------
	.macro	_ACTOR_DATA		objid, charid, way, x, y, visible, event_id
	.short	\objid
	.short	\charid
	.byte	\way
	.short	\x			//Xは体の中心
	.short	\y			//Yは足元
	.byte	\visible
	.byte	\event_id
	.endm

//--------------------------------------------------------------
/**
 * プレイヤーキャラ(HEROorHEROINE)のアクターデータ
 */
//--------------------------------------------------------------
	.macro	_PLAYER_ACTOR_DATA	objid, way, x, y, visible
	_ACTOR_DATA				\objid, FSS_CODE_MYSELF, \way, \x, \y, \visible, 0
	.endm

//--------------------------------------------------------------
/**
 * 通信キャラクタのアクターデータ
 */
//--------------------------------------------------------------
	.macro	_SIO_PLAYER_ACTOR_DATA	netid, objid, way, x, y, visible
	.short	\objid
	.short	FSS_CODE_SIO_PLAYER
	.byte	\way
	.short	\x			//Xは体の中心
	.short	\y			//Yは足元
	.byte	\visible
	.byte	0
	.byte	\netid
	.endm

//--------------------------------------------------------------
/**
 * アクターデータ終端
 */
//--------------------------------------------------------------
	.macro	_ACTOR_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//--------------------------------------------------------------
/**
 * アクター登録
 */
//--------------------------------------------------------------
	.macro	_ACTOR_SET	adrs
	.short	FR_ACTOR_SET
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * アクター削除
 */
//--------------------------------------------------------------
	.macro	_ACTOR_FREE		objid
	.short	FR_ACTOR_FREE
	.short	\objid
	.endm

//--------------------------------------------------------------
/**
 * アクター表示・非表示設定
 */
//--------------------------------------------------------------
	.macro	_ACTOR_VISIBLE_SET		objid, visible_flg
	.short	FR_ACTOR_VISIBLE_SET
	.short	\objid
	.byte	\visible_flg
	.endm

//--------------------------------------------------------------
/**
 * アクターBG優先設定
 */
//--------------------------------------------------------------
	.macro	_ACTOR_BG_PRI_SET		objid, pri
	.short	FR_ACTOR_BG_PRI_SET
	.short	\objid
	.short	\pri
	.endm


//==============================================================================
//
//	OBJアニメ
//
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	アニメ
 */
//-----------------------------------------------------------------------------
	.macro	_OBJ_ANIME	obj_id,adrs
	.short	FR_OBJ_ANIME
	.short	\obj_id
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	アニメーションデータラベル
 */
//-----------------------------------------------------------------------------
	.macro	_ANIME_LABEL label
	.align	4
	\label:
	.endm

//-----------------------------------------------------------------------------
/**
 *	アニメーションデータ
 */
//-----------------------------------------------------------------------------
	.macro	_ANIME_DATA	code,num
	.short	\code
	.short	\num
	.endm

//--------------------------------------------------------------
/**
 *	アニメーションデータ終了
 */
//--------------------------------------------------------------
	.macro	_ANIME_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//-----------------------------------------------------------------------------
/**
 *	アニメウェイト
 */
//-----------------------------------------------------------------------------
	.macro	_OBJ_ANIME_WAIT
	.short	FR_OBJ_ANIME_WAIT
	.endm


//==============================================================================
//
//	配置物リソース関連
//
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	配置物リソースデータラベル
 */
//-----------------------------------------------------------------------------
	.macro	_ARTICLE_RESOURCE_LABEL label
	.align	4
	\label:
	.endm

//--------------------------------------------------------------
/**
 * 配置物リソースデータ
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_RESOURCE_DATA	act_id
	.short	\act_id
	.endm

//--------------------------------------------------------------
/**
 * 配置物リソースデータ終端
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_RESOURCE_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//--------------------------------------------------------------
/**
 * 配置物リソース登録
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_RESOURCE_SET	adrs
	.short	FR_ARTICLE_RESOURCE_SET
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * 配置物リソース削除
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_RESOURCE_FREE	act_id
	.short	FR_ARTICLE_RESOURCE_FREE
	.short	\act_id
	.endm


//==============================================================================
//
//	配置物アクター関連
//
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	配置物アクターデータラベル
 */
//-----------------------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_LABEL label
	.align	4
	\label:
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクターデータ
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_DATA		act_id, x, y, visible, anm_start, set_actwork_no
	.short	\act_id
	.short	\x
	.short	\y
	.short	\visible
	.byte	\anm_start
	.byte	\set_actwork_no
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクターデータ終端
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_DATA_END
	.short	FSS_DATA_END_CODE
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター登録
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_SET	adrs
	.short	FR_ARTICLE_ACTOR_SET
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター削除
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_FREE		actwork_no
	.short	FR_ARTICLE_ACTOR_FREE
	.short	\actwork_no
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター表示・非表示設定
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_VISIBLE_SET		actwork_no, visible_flg
	.short	FR_ARTICLE_ACTOR_VISIBLE_SET
	.short	\actwork_no
	.byte	\visible_flg
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクターHフリップ設定
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_FLIP_H_SET		actwork_no
	.short	FR_ARTICLE_ACTOR_FLIP_H_SET
	.short	\actwork_no
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクターOBJモード設定(0=通常,1=半透明)
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_OBJ_MODE_SET		actwork_no,flag
	.short	FR_ARTICLE_ACTOR_OBJ_MODE_SET
	.short	\actwork_no
	.short	\flag
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター：アニメ開始
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_ANIME_START		actwork_no, seq_no
	.short	FR_ARTICLE_ACTOR_ANIME_START
	.short	\actwork_no
	.short	\seq_no
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター：アニメ停止
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_ANIME_STOP		actwork_no
	.short	FR_ARTICLE_ACTOR_ANIME_STOP
	.short	\actwork_no
	.endm

//--------------------------------------------------------------
/**
 * 配置物アクター：アニメ終了待ち
 */
//--------------------------------------------------------------
	.macro	_ARTICLE_ACTOR_ANIME_WAIT		actwork_no
	.short	FR_ARTICLE_ACTOR_ANIME_WAIT
	.short	\actwork_no
	.endm


//==============================================================================
//
//	基本コマンド
//
//==============================================================================

//-----------------------------------------------------------------------------
/**
 *	ダミー
 */
//-----------------------------------------------------------------------------
	.macro	_DUMMY
	.short	FR_DUMMY
	.endm

//-----------------------------------------------------------------------------
/**
 *	スクリプトの終了
 */
//-----------------------------------------------------------------------------
	.macro	_END
	.short	FR_END
	.endm

//-----------------------------------------------------------------------------
/**
 *	2Dマップの終了
 */
//-----------------------------------------------------------------------------
	.macro	_SCRIPT_FINISH
	.short	FR_SCRIPT_FINISH
	.endm

//-----------------------------------------------------------------------------
/**
 *	マップ切り替え
 */
//-----------------------------------------------------------------------------
	.macro	_MAP_CHANGE		scene_id
	.short	FR_MAP_CHANGE
	.short	\scene_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	マップ切り替え(スクリプトファイルまるごと切り替え)
 *
 * param_work, answerの値は保持されます。local_work, registerは破棄されます
 */
//-----------------------------------------------------------------------------
	.macro	_MAP_CHANGE_EX	scene_id, event_id
	.short	FR_MAP_CHANGE_EX
	.short	\scene_id
	.short	\event_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	ウェイト
 */
//-----------------------------------------------------------------------------
	.macro	_TIME_WAIT time,ret_wk
	.short	FR_TIME_WAIT
	.short	\time
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	展開メッセージを表示(2byte)(一括表示)
 *
 *	@param	msg_id	表示するメッセージID
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_ALL_PUT msg_id
	.short	FR_TALK_MSG_ALL_PUT
	.short	\msg_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	展開メッセージを表示(2byte)(スキップなし)
 *
 *	@param	msg_id	表示するメッセージID
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_NOSKIP msg_id
	.short	FR_TALK_MSG_NO_SKIP
	.short	\msg_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	展開メッセージを表示(2byte)
 *
 *	@param	msg_id	表示するメッセージID
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG msg_id
	.short	FR_TALK_MSG
	.short	\msg_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	会話ウィンドウ閉じる
 */
//-----------------------------------------------------------------------------
	.macro	_TALK_CLOSE
	.short	FR_TALK_WIN_CLOSE
	.endm

//==============================================================================
//	ワイプ
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ワイプフェードスタート
 *
 *	注意！　フェードにかかるLCD、BG面の設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_WIPE_FADE_START		div,sync,type,color
	.short	FR_WIPE_FADE_START
	.short	\div
	.short	\sync
	.short	\type
	.short	\color
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワイプフェード終了チェック
 *
 *	注意！　チェックするLCDの設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_WIPE_FADE_END_CHECK
	.short	FR_WIPE_FADE_END_CHECK
	.endm

//-----------------------------------------------------------------------------
/**
 *	ブラックアウト
 *
 *	注意！　フェードにかかるLCD、BG面の設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_BLACK_OUT	div,sync
	_WIPE_FADE_START	\div,\sync,WIPE_TYPE_FADEOUT,WIPE_FADE_BLACK
	.endm

//-----------------------------------------------------------------------------
/**
 *	ブラックイン
 *
 *	注意！　フェードにかかるLCD、BG面の設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_BLACK_IN	div,sync
	_WIPE_FADE_START	\div,\sync,WIPE_TYPE_FADEIN,WIPE_FADE_BLACK
	.endm

//-----------------------------------------------------------------------------
/**
 *	ホワイトアウト
 *
 *	注意！　フェードにかかるLCD、BG面の設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_WHITE_OUT	div,sync
	_WIPE_FADE_START	\div,\sync,WIPE_TYPE_FADEOUT,WIPE_FADE_WHITE
	.endm

//-----------------------------------------------------------------------------
/**
 *	ホワイトイン
 *
 *	注意！　フェードにかかるLCD、BG面の設定は固定にしている
 */
//-----------------------------------------------------------------------------
	.macro	_WHITE_IN	div,sync
	_WIPE_FADE_START	\div,\sync,WIPE_TYPE_FADEIN,WIPE_FADE_WHITE
	.endm

//==============================================================================
//	
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	はい、いいえ選択
 */
//-----------------------------------------------------------------------------
	//.macro	_YES_NO_WIN ret_wk
	//.short	FR_YES_NO_WIN
	//.short	\ret_wk
	//.endm
	
	.macro	_YES_NO_WINDOW ret_wk,csr_pos
	.short	FR_YES_NO_WIN
	.short	\ret_wk
	.byte	\csr_pos
	.endm

	.macro	_YES_NO_WIN ret_wk
	_YES_NO_WINDOW	\ret_wk,0
	.endm

	.macro	_YES_NO_WIN_EX ret_wk
	_YES_NO_WINDOW	\ret_wk,1
	.endm

//==============================================================================
//	BMPメニュー
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	BMPメニュー　初期化
 */
//-----------------------------------------------------------------------------
	.macro	_BMPMENU_INIT x,y,cursor,cancel,ret_wk
	.short	FR_BMPMENU_INIT
	.byte	\x
	.byte	\y
	.byte	\cursor
	.byte	\cancel
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPメニュー　初期化(読み込んでいるgmmファイルを使用する)
 */
//-----------------------------------------------------------------------------
	.macro	_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk
	.short	FR_BMPMENU_INIT_EX
	.byte	\x
	.byte	\y
	.byte	\cursor
	.byte	\cancel
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPメニュー　リスト作成(shortバージョン)
 */
//-----------------------------------------------------------------------------
	.macro	_BMPMENU_MAKE_LIST msg_id,param
	.short	FR_BMPMENU_MAKE_LIST
	.short	\msg_id
	.short	\param
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPメニュー　リスト作成(shortバージョン)(会話ウィンメッセージ指定あり)
 */
//-----------------------------------------------------------------------------
	.macro	_BMPMENU_MAKE_LIST_TALK_MSG msg_id,talk_msg_id,param
	.short	FR_BMPMENU_MAKE_LIST_TALK_MSG
	.short	\msg_id
	.short	\talk_msg_id
	.short	\param
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPメニュー　開始
 */
//-----------------------------------------------------------------------------
	.macro	_BMPMENU_START
	.short	FR_BMPMENU_START
	.endm

//==============================================================================
//	BMPリスト
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	BMPリスト　初期化
 */
//-----------------------------------------------------------------------------
	.macro	_BMPLIST_INIT x,y,cursor,cancel,ret_wk
	.short	FR_BMPLIST_INIT
	.byte	\x
	.byte	\y
	.byte	\cursor
	.byte	\cancel
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPリスト　初期化(読み込んでいるgmmファイルを使用する)
 */
//-----------------------------------------------------------------------------
	.macro	_BMPLIST_INIT_EX x,y,cursor,cancel,ret_wk
	.short	FR_BMPLIST_INIT_EX
	.byte	\x
	.byte	\y
	.byte	\cursor
	.byte	\cancel
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPリスト　リスト作成(shortバージョン)
 */
//-----------------------------------------------------------------------------
	.macro	_BMPLIST_MAKE_LIST msg_id,talk_msg_id,param
	.short	FR_BMPLIST_MAKE_LIST
	.short	\msg_id
	.short	\talk_msg_id
	.short	\param
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPリスト　開始
 */
//-----------------------------------------------------------------------------
	.macro	_BMPLIST_START
	.short	FR_BMPLIST_START
	.endm

//-----------------------------------------------------------------------------
/**
 *	BMPリスト　強制削除	バトルキャッスルのみで使用
 */
//-----------------------------------------------------------------------------
	.macro	_BMPLIST_DEL
	.short	FR_BMPLIST_DEL
	.endm

//==============================================================================
//	代入
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ワークに値を代入
 */
//-----------------------------------------------------------------------------
	.macro	_LDVAL	wk,val
	.short	FR_LD_WK_VAL
	.short	\wk
	.short	\val
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワークにワークの値を代入
 */
//-----------------------------------------------------------------------------
	.macro	_LDWK	wk1,wk2
	.short	FR_LD_WK_WK
	.short	\wk1
	.short	\wk2
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワークに値を足す
 */
//-----------------------------------------------------------------------------
	.macro	_ADD_WK wk,num
	.short	FR_ADD_WK
	.short	\wk
	.short	\num
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワークに値を弾く
 */
//-----------------------------------------------------------------------------
	.macro	_SUB_WK wk,num
	.short	FR_SUB_WK
	.short	\wk
	.short	\num
	.endm

//==============================================================================
//	比較
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ワークと値の比較
 */
//-----------------------------------------------------------------------------
	.macro	_CMPVAL	wk,val
	.short	FR_CP_WK_VAL
	.short	\wk
	.short	\val
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワークとワークの比較
 */
//-----------------------------------------------------------------------------
	.macro	_CMPWK	wk1,wk2
	.short	FR_CP_WK_WK
	.short	\wk1
	.short	\wk2
	.endm

//==============================================================================
//	ジャンプ命令
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ジャンプ
 */
//-----------------------------------------------------------------------------
	.macro	_JUMP adrs
	.short	FR_JUMP
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	IFジャンプ
 */
//-----------------------------------------------------------------------------
	.macro	_IF_JUMP cond,adrs
	.short	FR_IF_JUMP
	.byte	\cond
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	値を比較、条件どおりの場合分岐(ジャンプ)
 */
//-----------------------------------------------------------------------------
	.macro	_IFVAL_JUMP wk,cond,val,adrs
	_CMPVAL	\wk,\val
	.short	FR_IF_JUMP
	.byte	\cond
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	値を比較、条件どおりの場合分岐(コール)
 */
//-----------------------------------------------------------------------------
	.macro	_IFVAL_CALL wk,cond,val,adrs
	_CMPVAL	\wk,\val
	.short	FR_IF_CALL
	.byte	\cond
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	ワークを比較、条件どおりの場合分岐(ジャンプ)
 */
//-----------------------------------------------------------------------------
	.macro	_IFWK_JUMP wk,cond,wk2,adrs
	_CMPWK	\wk,\wk2
	.short	FR_IF_JUMP
	.byte	\cond
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	コール
 */
//-----------------------------------------------------------------------------
	.macro	_CALL adrs
	.short	FR_CALL
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	コールを呼び出したアドレスに戻る
 */
//-----------------------------------------------------------------------------
	.macro	_RET
	.short	FR_RET
	.endm

//-----------------------------------------------------------------------------
/**
 *	IFコール
 */
//-----------------------------------------------------------------------------
	.macro	_IF_CALL cond,adrs
	.short	FR_IF_CALL
	.byte	\cond
	.long	((\adrs-.)-4)
	.endm

//-----------------------------------------------------------------------------
/**
 *	switch ～ case 構文
 *
 *  スクリプト制御ワークで確保されるワーク(SCWK_REG0)を使用しているので、
 *	FLAG_CHANGE,INIT_CHANGEなどでは使用できません！
 *	SCENE_CHANGEは使用できます。
 */
//-----------------------------------------------------------------------------
	.macro		_SWITCH wk
	_LDWK		FSW_REG0,\wk
	.endm

	.macro		_CASE_JUMP	val,adr
	_CMPVAL		FSW_REG0,\val
	_IF_JUMP	EQ,\adr
	.endm

//-----------------------------------------------------------------------------
/**
 * スクリプトウィンドウ表示中にBキャンセル
 */
//-----------------------------------------------------------------------------
	.macro		_CASE_CANCEL	adr
	_CMPVAL		FSW_REG0,FSEV_WIN_B_CANCEL
	_IF_JUMP	EQ,\adr
	.endm

//-----------------------------------------------------------------------------
/**
 *	名前入力画面呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_NAMEIN_PROC
	.short	FR_NAMEIN_PROC
	.endm

//=============================================================================
/**
 *	通信
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	同期
 */
//-----------------------------------------------------------------------------
	.macro	_COMM_SYNCHRONIZE	no
	.short	FR_COMM_TIMING_SYNC_START
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 * 　リセット	
 */
//-----------------------------------------------------------------------------
	.macro	_COMM_RESET
	.short	FR_COMM_TEMP_DATA_RESET
	.endm

//-----------------------------------------------------------------------------
/**
 * 　通信切断
 */
//-----------------------------------------------------------------------------
	.macro	_COMM_DIRECT_END
	.short	FR_COMM_DIRECT_END
	.endm

//-----------------------------------------------------------------------------
/**
 *	ランダム数取得
 */
//-----------------------------------------------------------------------------
	.macro	_GET_RND ret_wk,num
	.short	FR_GET_RND
	.short	\ret_wk
	.short	\num
	.endm

//-----------------------------------------------------------------------------
/**
 *	ポケセン回復
 */
//-----------------------------------------------------------------------------
	.macro	_PC_KAIFUKU
	.short	FR_PC_KAIFUKU
	.endm

//-----------------------------------------------------------------------------
/**
 *	キー待ち
 */
//-----------------------------------------------------------------------------
	.macro	_AB_KEYWAIT
	.short	FR_ABKEY_WAIT
	.endm

//-----------------------------------------------------------------------------
/**
 * キー待ち　OR　時間待ち
 */
//-----------------------------------------------------------------------------
	.macro	_AB_TIME_KEYWAIT	wait
	.short	FR_ABKEY_TIMEWAIT
	.short	\wait
	.endm

//-----------------------------------------------------------------------------
/**
 *	トレーナー敗北チェック
 */
//-----------------------------------------------------------------------------
	.macro	_TRAINER_LOSE_CHECK ret_wk
	.short	FR_LOSE_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーセーブワークに値をセット
 */
//-----------------------------------------------------------------------------
	.macro	_SAVE_EVENT_WORK_SET	work_no,param
	.short	FR_SAVE_EVENT_WORK_SET
	.short	\work_no
	.short	\param
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーセーブワークの値を取得
 */
//-----------------------------------------------------------------------------
	.macro	_SAVE_EVENT_WORK_GET	work_no,work
	.short	FR_SAVE_EVENT_WORK_GET
	.short	\work_no
	.short	\work
	.endm

//-----------------------------------------------------------------------------
/**
 *	エンカウントエフェクト
 */
//-----------------------------------------------------------------------------
	.macro	_ENCOUNT_EFFECT		effect_no
	.short	FR_ENCOUNT_EFFECT
	.short	\effect_no
	.endm

//-----------------------------------------------------------------------------
/**
 *	主人公の人物OBJコードを取得
 */
//-----------------------------------------------------------------------------
	.macro	_GET_MINE_OBJ		ret_wk
	.short	FR_GET_MINE_OBJ
	.short	\ret_wk
	.endm


//==============================================================================
//	パーティクル
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	SPAリソース読み込み
 */
//-----------------------------------------------------------------------------
	.macro	_PARTICLE_SPA_LOAD		work_id, spa_no, camera_type
	.short	FR_PARTICLE_SPA_LOAD
	.short	\work_id
	.short	\spa_no
	.short	\camera_type
	.endm

//-----------------------------------------------------------------------------
/**
 *	SPAリソース解放
 */
//-----------------------------------------------------------------------------
	.macro	_PARTICLE_SPA_EXIT		work_id
	.short	FR_PARTICLE_SPA_EXIT
	.short	\work_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	エミッタ生成
 */
//-----------------------------------------------------------------------------
	.macro	_PARTICLE_ADD_EMITTER		work_id, emit_no
	.short	FR_PARTICLE_ADD_EMITTER
	.short	\work_id
	.short	\emit_no
	.endm

//-----------------------------------------------------------------------------
/**
 *	パーティクル終了待ち
 */
//-----------------------------------------------------------------------------
	.macro	_PARTICLE_WAIT
	.short	FR_PARTICLE_WAIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	ポケモンアクター表示
 */
//-----------------------------------------------------------------------------
	.macro	_TEMOTI_POKE_ACT_ADD	pos,x,y,id,evy,next_rgb
	.short	FR_TEMOTI_POKE_ACT_ADD
	.short	\pos
	.short	\x
	.short	\y
	.short	\id
	.short	\evy
	.short	\next_rgb
	.endm

//-----------------------------------------------------------------------------
/**
 *	ポケモンアクター削除
 */
//-----------------------------------------------------------------------------
	.macro	_TEMOTI_POKE_ACT_DEL	id
	.short	FR_TEMOTI_POKE_ACT_DEL
	.short	\id
	.endm

//-----------------------------------------------------------------------------
/**
 *	フロンティアブレーン戦のエンカウントエフェクト発動
 */
//-----------------------------------------------------------------------------
	.macro	_BRAIN_ENCOUNT_EFFECT	effect_no
	.short	FR_BRAIN_ENCOUNT_EFFECT
	.short	\effect_no
	.endm

//-----------------------------------------------------------------------------
/**
 *	レコードインクリメント
 */
//-----------------------------------------------------------------------------
	.macro	_RECORD_INC	recid
	.short	FR_RECORD_INC
	.short	\recid
	.endm

//-----------------------------------------------------------------------------
/**
 *	レコード追加
 */
//-----------------------------------------------------------------------------
	.macro	_RECORD_ADD	recid,val
	.short	FR_RECORD_ADD
	.short	\recid
	.short	\val
	.endm

//-----------------------------------------------------------------------------
/**
 *	スコア追加
 */
//-----------------------------------------------------------------------------
	.macro	_SCORE_ADD	recid
	.short	FR_SCORE_ADD
	.short	\recid
	.endm

//-----------------------------------------------------------------------------
/**
 *	自分、相手のどちらかがDPかチェック
 */
//-----------------------------------------------------------------------------
	.macro	_CHECK_DP_ROM_CODE	ret_wk
	.short	FR_CHECK_DP_ROM_CODE
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	地震セット
 */
//-----------------------------------------------------------------------------
	.macro	_SHAKE_SET	x, y, wait, loop
	.short	FR_SHAKE_SET
	.short	\x
	.short	\y
	.short	\wait
	.short	\loop
	.endm

//-----------------------------------------------------------------------------
/**
 *	地震終了待ち
 */
//-----------------------------------------------------------------------------
	.macro	_SHAKE_WAIT
	.short	FR_SHAKE_WAIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	Window機能セット
 */
//-----------------------------------------------------------------------------
	.macro	_WND_SET	x1, y1, x2, y2, wait, on_off
	.short	FR_WND_SET
	.short	\x1
	.short	\y1
	.short	\x2
	.short	\y2
	.short	\wait
	.short	\on_off
	.endm

//-----------------------------------------------------------------------------
/**
 *	Window機能終了待ち
 */
//-----------------------------------------------------------------------------
	.macro	_WND_WAIT
	.short	FR_WND_WAIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	CPウィンドウ表示
 */
//-----------------------------------------------------------------------------
	.macro	_CP_WIN_WRITE
	.short	FR_CP_WIN_WRITE
	.endm

//-----------------------------------------------------------------------------
/**
 *	CPウィンドウ削除
 */
//-----------------------------------------------------------------------------
	.macro	_CP_WIN_DEL
	.short	FR_CP_WIN_DEL
	.endm

//-----------------------------------------------------------------------------
/**
 *	CP表示
 */
//-----------------------------------------------------------------------------
	.macro	_CP_WRITE
	.short	FR_CP_WRITE
	.endm

//==============================================================================
//	OBJ動作コードせってい
//==============================================================================
//--------------------------------------------------------------
/**
 *	OBJ動作コード設定：リセットする(OBJ動作なしにする)
 */
//--------------------------------------------------------------
	.macro	_OBJMOVE_RESET		objid
	.short	FR_OBJMOVE_RESET
	.short	\objid
	.endm

//--------------------------------------------------------------
/**
 *	OBJ動作コード設定：きょろきょろ
 */
//--------------------------------------------------------------
	.macro	_OBJMOVE_KYORO		objid, front_dir, dir_flg, first_wait
	.short	FR_OBJMOVE_KYORO
	.short	\objid
	.short	\front_dir			//FC_DIR_???
	.short	\dir_flg			//0 or 1。　frontがDOWNの場合、0だと、左＞右、1だと、右＞左
	.short	\first_wait			//初期ウェイト
	.endm


//=============================================================================
//	サウンド
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	SEを鳴らす
 */
//-----------------------------------------------------------------------------
	.macro	_SE_PLAY no
	.short	FR_SE_PLAY
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 *	SEを止める
 */
//-----------------------------------------------------------------------------
	.macro	_SE_STOP no
	.short	FR_SE_STOP
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 *	SE終了待ち
 */
//-----------------------------------------------------------------------------
	.macro	_SE_WAIT	no
	.short	FR_SE_WAIT
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 *	MEを鳴らす
 */
//-----------------------------------------------------------------------------
	.macro	_ME_PLAY no
	.short	FR_ME_PLAY
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 *	ME終了待ち
 */
//-----------------------------------------------------------------------------
	.macro	_ME_WAIT
	.short	FR_ME_WAIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	BGMを鳴らす
 */
//-----------------------------------------------------------------------------
	.macro	_BGM_PLAY no
	.short	FR_BGM_PLAY
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 *	BGMを止める
 */
//-----------------------------------------------------------------------------
	.macro	_BGM_STOP no
	.short	FR_BGM_STOP
	.short	\no
	.endm


//==============================================================================
//	ファクトリー
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ファクトリーワーク確保
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_WORK_ALLOC	init,type,level
	.short	FR_FACTORY_WORK_ALLOC
	.short	\init
	.short	\type
	.short	\level
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーワーク初期化
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_WORK_INIT	init
	.short	FR_FACTORY_WORK_INIT
	.short	\init
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーワーク開放
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_WORK_FREE
	.short	FR_FACTORY_WORK_FREE
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーレンタル呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_RENTAL_CALL
	.short	FR_FACTORY_RENTAL_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー呼び出し結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_CALL_GET_RESULT
	.short	FR_FACTORY_CALL_GET_RESULT
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー戦闘呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_BATTLE_CALL
	.short	FR_FACTORY_BATTLE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー交換呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_TRADE_CALL
	.short	FR_FACTORY_TRADE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーレンタルPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_RENTAL_PARTY_SET
	.short	FR_FACTORY_RENTAL_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーバトル後のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_BTL_AFTER_PARTY_SET
	.short	FR_FACTORY_BTL_AFTER_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー交換後のポケモン変更
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_TRADE_POKE_CHANGE
	.short	FR_FACTORY_TRADE_POKE_CHANGE
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー交換後のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_TRADE_AFTER_PARTY_SET
	.short	FR_FACTORY_TRADE_AFTER_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリーFACTORY_SCRWORKの操作
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_TOOL	code,param1,param2,ret_work
	.short	FR_FACTORY_TOOL
	.byte	\code
	.byte	\param1
	.byte	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー敗北チェック
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_LOSE_CHECK	ret_wk
	.short	FR_FACTORY_LOSE_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー通信send_buf
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_SEND_BUF	type,param,ret_wk
	.short	FR_FACTORY_SEND_BUF
	.short	\type
	.short	\param
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー通信recv_buf(wkにtypeを代入して渡して下さい)
 */
//-----------------------------------------------------------------------------
	.macro	_FACTORY_RECV_BUF	wk
	.short	FR_FACTORY_RECV_BUF
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ファクトリー対戦前メッセージを表示(tr_idx)
 *  @param	tr_idx	一人目か二人目か？
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_FACTORY_APPEAR	tr_id	
	.short	FR_FACTORY_APPEAR_MSG
	.byte	\tr_id
	.endm

//==============================================================================
//	録画
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	データ初期化
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_INIT
	.short	FR_BATTLE_REC_INIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	セーブ
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_SAVE	fr_no,type,rensyou,ret_wk
	.short	FR_BATTLE_REC_SAVE
	.short	\fr_no
	.short	\type
	.short	\rensyou
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ロード
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_LOAD
	.short	FR_BATTLE_REC_LOAD
	.endm

//-----------------------------------------------------------------------------
/**
 *	破棄
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_EXIT
	.short	FR_BATTLE_REC_EXIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	録画データがすでにあるかチェック
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_DATA_OCC_CHECK	ret_wk
	.short	FR_BATTLE_REC_DATA_OCC_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	自分のROMよりもサーバーバージョンが高いか取得
 */
//-----------------------------------------------------------------------------
	.macro	_BATTLE_REC_SERVER_VER_CHK	ret_wk
	.short	FR_BATTLE_REC_SERVER_VER_CHK
	.short	\ret_wk
	.endm

//==============================================================================
//	セーブ
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	レポート書き込み
 */
//-----------------------------------------------------------------------------
	.macro	_REPORT_SAVE	ret_wk
	.short	FR_REPORT_SAVE
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	レポート書き込み(分割)
 */
//-----------------------------------------------------------------------------
	.macro	_REPORT_DIV_SAVE	ret_wk
	.short	FR_REPORT_DIV_SAVE
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	外部セーブ初期化
 */
//-----------------------------------------------------------------------------
	.macro	_EXTRA_SAVE_INIT
	.short	FR_EXTRA_SAVE_INIT
	.endm

//-----------------------------------------------------------------------------
/**
 *	外部セーブ初期化済みかチェック
 */
//-----------------------------------------------------------------------------
	.macro	_EXTRA_SAVE_INIT_CHECK	ret_wk
	.short	FR_EXTRA_SAVE_INIT_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	バトルステージの外部連勝記録を書き込み、セーブを行う
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_RENSHOU_COPY_EXTRA	ret_wk
	.short	FR_STAGE_RENSHOU_COPY_EXTRA
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	待機アイコン表示
 */
//-----------------------------------------------------------------------------
	.macro	_ADD_WAITICON
	.short	FR_WAITICON_ADD
	.endm

//-----------------------------------------------------------------------------
/**
 *	待機アイコン消去
 */
//-----------------------------------------------------------------------------
	.macro	_DEL_WAITICON
	.short	FR_WAITICON_DEL
	.endm

//==============================================================================
//	ワードセット
//==============================================================================

//-----------------------------------------------------------------------------
/**
 *	アイテム名をセット
 */
//-----------------------------------------------------------------------------
	.macro	_ITEM_NAME	idx,itemno
	.short	FR_ITEM_NAME
	.byte	\idx
	.short	\itemno
	.endm

//-----------------------------------------------------------------------------
/**
 *	数値をセット
 */
//-----------------------------------------------------------------------------
	.macro	_NUMBER_NAME	idx,number
	.short	FR_NUMBER_NAME
	.byte	\idx
	.short	\number
	.endm

//-----------------------------------------------------------------------------
/**
 *	主人公名
 */
//-----------------------------------------------------------------------------
	.macro	_PLAYER_NAME	idx
	.short	FR_PLAYER_NAME
	.byte	\idx
	.endm

//-----------------------------------------------------------------------------
/**
 *	パートナー名
 */
//-----------------------------------------------------------------------------
	.macro	_PAIR_NAME	idx
	.short	FR_PAIR_NAME
	.byte	\idx
	.endm

//-----------------------------------------------------------------------------
/**
 *	技名
 */
//-----------------------------------------------------------------------------
	.macro	_WAZA_NAME	idx,waza
	.short	FR_WAZA_NAME
	.byte	\idx
	.short	\waza
	.endm

//-----------------------------------------------------------------------------
/**
 *	モンスターナンバーからポケモン名
 */
//-----------------------------------------------------------------------------
	.macro	_POKEMON_NAME_EXTRA	idx,mons,sex,flag
	.short	FR_POKEMON_NAME_EXTRA
	.byte	\idx
	.short	\mons
	.short	\sex
	.byte	\flag
	.endm

//-----------------------------------------------------------------------------
/**
 *	タイプの名前
 */
//-----------------------------------------------------------------------------
	.macro	_TYPE_NAME	idx,type
	.short	FR_TYPE_NAME
	.byte	\idx
	.short	\type
	.endm

//-----------------------------------------------------------------------------
/**
 *	ライバル名
 */
//-----------------------------------------------------------------------------
	.macro	_RIVAL_NAME	idx
	.short	FR_RIVAL_NAME
	.byte	\idx
	.endm

//==============================================================================
//	バトルポイント
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	所持バトルポイント取得
 */
//-----------------------------------------------------------------------------
	.macro	_GET_BTL_POINT	ret_wk
	.short	FR_GET_BTL_POINT
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	所持バトルポイント加算
 */
//-----------------------------------------------------------------------------
	.macro	_BTL_POINT_ADD	value
	.short	FR_BTL_POINT_ADD
	.short	\value
	.endm

//-----------------------------------------------------------------------------
/**
 *	所持バトルポイント減算
 */
//-----------------------------------------------------------------------------
	.macro	_BTL_POINT_SUB	value
	.short	FR_BTL_POINT_SUB
	.short	\value
	.endm

//==============================================================================
//	バトルタワー
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	バトルタワー コマンドツール呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_TOOLS	cmd,param,ret_wk
	.short	FR_BTOWER_TOOLS
	.short	\cmd
	.short	\param
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワー対戦前メッセージを表示(tr_idx)
 *  @param	tr_idx	一人目か二人目か？
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_BTOWER_APPEAR	tr_id	
	.short	FR_BTOWER_APPEAR_MSG
	.byte	\tr_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	バトルタワーワーク解放
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_WORK_RELEASE
	.short	FR_BTOWER_WORK_RELEASE
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワー戦闘呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_BATTLE_CALL
	.short	FR_BTOWER_BATTLE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワー呼び出し結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_CALL_GET_RESULT	ret_wk
	.short	FR_BTOWER_CALL_GET_RESULT
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	バトルタワー 汎用データ送信
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_SEND_BUF	mode,param,ret_wk
	.short	FR_BTOWER_SEND_BUF
	.short	\mode
	.short	\param
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	バトルタワー 汎用データ受信
 *
 *	(モードが格納されているワークのIDと、戻り値を格納するワークのIDを渡す)
 */
//-----------------------------------------------------------------------------
	.macro	_BTOWER_RECV_BUF	mode,ret_wk
	.short	FR_BTOWER_RECV_BUF
	.short	\mode
	.short	\ret_wk
	.endm

//==============================================================================
//	ステージ
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	ステージワーク確保
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_WORK_ALLOC	init,type,pos,pos2
	.short	FR_STAGE_WORK_ALLOC
	.short	\init
	.short	\type
	.short	\pos
	.short	\pos2
	.endm

//-----------------------------------------------------------------------------
/**
 *	敵トレーナー、敵ポケモンセット
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_WORK_ENEMY_SET	init
	.short	FR_STAGE_WORK_ENEMY_SET
	.short	\init
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージワーク開放
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_WORK_FREE
	.short	FR_STAGE_WORK_FREE
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージポケモンタイプ選択呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_POKE_TYPE_SEL_CALL
	.short	FR_STAGE_POKE_TYPE_SEL_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ呼び出し結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_CALL_GET_RESULT
	.short	FR_STAGE_CALL_GET_RESULT
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ戦闘呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_BATTLE_CALL
	.short	FR_STAGE_BATTLE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージSTAGE_SCRWORKの操作
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_TOOL	code,param1,param2,ret_work
	.short	FR_STAGE_TOOL
	.byte	\code
	.byte	\param1
	.byte	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ敗北チェック
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_LOSE_CHECK	ret_wk
	.short	FR_STAGE_LOSE_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ通信send_buf
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_SEND_BUF	type,param,ret_work
	.short	FR_STAGE_SEND_BUF
	.short	\type
	.short	\param
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ通信recv_buf(wkにtypeを代入して渡して下さい)
 */
//-----------------------------------------------------------------------------
	.macro	_STAGE_RECV_BUF	wk
	.short	FR_STAGE_RECV_BUF
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ステージ対戦前メッセージを表示(tr_idx)
 *  @param	tr_idx	一人目か二人目か？
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_STAGE_APPEAR	tr_id	
	.short	FR_STAGE_APPEAR_MSG
	.byte	\tr_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	デバック
 */
//-----------------------------------------------------------------------------
	.macro	_DEBUG_PRINT	wk_id
	.short	FR_DEBUG_PRINT
	.short	\wk_id
	.endm

//==============================================================================
//	キャッスル
//==============================================================================
//-----------------------------------------------------------------------------
/**
 *	キャッスルワーク確保
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_WORK_ALLOC	init,type,pos1,pos2,pos3,work
	.short	FR_CASTLE_WORK_ALLOC
	.short	\init
	.short	\type
	.short	\pos1
	.short	\pos2
	.short	\pos3
	.short	\work
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスルワーク初期化
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_WORK_INIT	init
	.short	FR_CASTLE_WORK_INIT
	.short	\init
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスルワーク開放
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_WORK_FREE
	.short	FR_CASTLE_WORK_FREE
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル手持ち画面呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_MINE_CALL
	.short	FR_CASTLE_MINE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル呼び出し結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_CALL_GET_RESULT
	.short	FR_CASTLE_CALL_GET_RESULT
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル戦闘呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_BATTLE_CALL
	.short	FR_CASTLE_BATTLE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル敵トレーナー呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_ENEMY_CALL
	.short	FR_CASTLE_ENEMY_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル戦闘前のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_BTL_BEFORE_PARTY_SET
	.short	FR_CASTLE_BTL_BEFORE_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスルバトル後のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_BTL_AFTER_PARTY_SET
	.short	FR_CASTLE_BTL_AFTER_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスルCASTLE_SCRWORKの操作
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_TOOL	code,param1,param2,ret_work
	.short	FR_CASTLE_TOOL
	.byte	\code
	.byte	\param1
	.byte	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル敗北チェック
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_LOSE_CHECK	ret_wk
	.short	FR_CASTLE_LOSE_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル通信send_buf
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_SEND_BUF	type,param,ret_work
	.short	FR_CASTLE_SEND_BUF
	.short	\type
	.short	\param
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル通信recv_buf(wkにtypeを代入して渡して下さい)
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_RECV_BUF	wk
	.short	FR_CASTLE_RECV_BUF
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスルランクアップ呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_RANKUP_CALL
	.short	FR_CASTLE_RANKUP_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	親の決定待ち
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_PARENT_WAIT	ret_wk
	.short	FR_CASTLE_PARENT_CHECK_WAIT
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	CP取得
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_GET_CP	type,ret_wk
	.short	FR_CASTLE_GET_CP
	.short	\type
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	CP減らす
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_SUB_CP	type,num
	.short	FR_CASTLE_SUB_CP
	.short	\type
	.short	\num
	.endm

//-----------------------------------------------------------------------------
/**
 *	CP増やす
 */
//-----------------------------------------------------------------------------
	.macro	_CASTLE_ADD_CP	type,num
	.short	FR_CASTLE_ADD_CP
	.short	\type
	.short	\num
	.endm

//-----------------------------------------------------------------------------
/**
 *	キャッスル対戦前メッセージを表示(tr_idx)
 *  @param	tr_idx	一人目か二人目か？
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_CASTLE_APPEAR	tr_id	
	.short	FR_CASTLE_APPEAR_MSG
	.byte	\tr_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付ワーク確保
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_WORK_ALLOC
	.short	FR_WIFI_COUNTER_WORK_ALLOC
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付ワーク開放
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_WORK_FREE
	.short	FR_WIFI_COUNTER_WORK_FREE
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付通信send_buf
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_SEND_BUF	type,param,param2,ret_work
	.short	FR_WIFI_COUNTER_SEND_BUF
	.short	\type
	.short	\param
	.short	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付通信recv_buf(wkにtypeを代入して渡して下さい)
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_RECV_BUF	wk
	.short	FR_WIFI_COUNTER_RECV_BUF
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付 施設No比較
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_BFNO_CHECK	no,wk
	.short	FR_WIFI_COUNTER_BFNO_CHECK
	.short	\no
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付 ポケモンリスト
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_POKELIST	bf_no,type,ret_wk
	.short	FR_WIFI_COUNTER_POKELIST
	.short	\bf_no
	.short	\type
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付 ポケモンリスト選択結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_POKELIST_GET_RESULT	ret_wk,ret_wk2
	.short	FR_WIFI_COUNTER_POKELIST_GET_RESULT
	.short	\ret_wk
	.short	\ret_wk2
	.endm

//-----------------------------------------------------------------------------
/**
 *	WiFi受付 やめる　選択チェック
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_YAMERU_CHECK	ret_wk
	.short	FR_WIFI_COUNTER_YAMERU_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	FRWIFI_SCRWORKの操作
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_TOOL	code,param1,param2,ret_work
	.short	FR_WIFI_COUNTER_TOOL
	.byte	\code
	.byte	\param1
	.byte	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワー呼び出し前の準備
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_TOWER_CALL_BEFORE
	.short	FR_WIFI_COUNTER_TOWER_CALL_BEFORE
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワートレーナーデータ送信
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_TOWER_SEND_TR_DATA	ret_wk
	.short	FR_WIFI_COUNTER_TOWER_SEND_TR_DATA
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワートレーナーデータ受信
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_TOWER_RECV_TR_DATA
	.short	FR_WIFI_COUNTER_TOWER_RECV_TR_DATA
	.endm

//-----------------------------------------------------------------------------
/**
 *	タワー呼び出し後の後始末
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_TOWER_CALL_AFTER
	.short	FR_WIFI_COUNTER_TOWER_CALL_AFTER
	.endm

//-----------------------------------------------------------------------------
/**
 *	日時の記録をつける(フロンティア専用)
 */
//-----------------------------------------------------------------------------
	.macro	_FR_WIFI_COUNTER_LIST_SET_LAST_PLAY_DATE
	.short	FR_WIFI_COUNTER_LIST_SET_LAST_PLAY_DATE
	.endm


//==============================================================================
//	ルーレット
//==============================================================================

//-----------------------------------------------------------------------------
/**
 *	ルーレットワーク確保
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_WORK_ALLOC	init,type,pos1,pos2,pos3,work
	.short	FR_ROULETTE_WORK_ALLOC
	.short	\init
	.short	\type
	.short	\pos1
	.short	\pos2
	.short	\pos3
	.short	\work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレットワーク初期化
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_WORK_INIT	init
	.short	FR_ROULETTE_WORK_INIT
	.short	\init
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレットワーク開放
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_WORK_FREE
	.short	FR_ROULETTE_WORK_FREE
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット呼び出し結果取得
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_CALL_GET_RESULT
	.short	FR_ROULETTE_CALL_GET_RESULT
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット戦闘呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_BATTLE_CALL
	.short	FR_ROULETTE_BATTLE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット戦闘前のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_BTL_BEFORE_PARTY_SET
	.short	FR_ROULETTE_BTL_BEFORE_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレットバトル後のPOKEPARTYのセット
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_BTL_AFTER_PARTY_SET
	.short	FR_ROULETTE_BTL_AFTER_PARTY_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレットROULETTE_SCRWORKの操作
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_TOOL	code,param1,param2,ret_work
	.short	FR_ROULETTE_TOOL
	.byte	\code
	.short	\param1
	.short	\param2
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット敗北チェック
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_LOSE_CHECK	ret_wk
	.short	FR_ROULETTE_LOSE_CHECK
	.short	\ret_wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット通信send_buf
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_SEND_BUF	type,param,ret_work
	.short	FR_ROULETTE_SEND_BUF
	.short	\type
	.short	\param
	.short	\ret_work
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット通信recv_buf(wkにtypeを代入して渡して下さい)
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_RECV_BUF	wk
	.short	FR_ROULETTE_RECV_BUF
	.short	\wk
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット呼び出し
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_CALL
	.short	FR_ROULETTE_CALL
	.endm

//-----------------------------------------------------------------------------
/**
 *	ルーレット対戦前メッセージを表示(tr_idx)
 *  @param	tr_idx	一人目か二人目か？
 */
//-----------------------------------------------------------------------------
	.macro	_TALKMSG_ROULETTE_APPEAR	tr_id	
	.short	FR_ROULETTE_APPEAR_MSG
	.byte	\tr_id
	.endm

//-----------------------------------------------------------------------------
/**
 *	決定したイベントパネル処理を実行
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_DECIDE_EV_NO_FUNC
	.short	FR_ROULETTE_DECIDE_EV_NO_FUNC
	.endm

//-----------------------------------------------------------------------------
/**
 *	アイテム持っているアイコンのバニッシュ
 */
//-----------------------------------------------------------------------------
	.macro	_ROULETTE_CHG_ITEMKEEP_VANISH	param1,param2,param3
	.short	FR_ROULETTE_CHG_ITEMKEEP_VANISH
	.short	\param1
	.short	\param2
	.short	\param3
	.endm

//-----------------------------------------------------------------------------
/**
 *	TV:ステージ
 */
//-----------------------------------------------------------------------------
	.macro	_TV_TEMP_STAGE_SET
	.short	FR_TV_TEMP_STAGE_SET
	.endm

//-----------------------------------------------------------------------------
/**
 *	TV:フロンティアで仲良し
 */
//-----------------------------------------------------------------------------
	.macro	_TV_TEMP_FRIEND_SET		no
	.short	FR_TV_TEMP_FRIEND_SET
	.short	\no
	.endm

//-----------------------------------------------------------------------------
/**
 * WIFIで一気にデータを送る時の対応
 */
//-----------------------------------------------------------------------------
	.macro	_COMM_SET_WIFI_BOTH_NET	flag
	.short	FR_COMM_SET_WIFI_BOTH_NET
	.short	\flag
	.endm


