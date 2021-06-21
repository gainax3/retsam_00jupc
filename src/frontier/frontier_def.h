//============================================================================================
/**
 * @file	frontier_def.h
 * @bfief	「フロンティア」共通定義
 * @author	Satoshi Nohara
 * @date	07.07.31
 */
//============================================================================================
#ifndef	__FRONTIER_DEF_H__
#define	__FRONTIER_DEF_H__


//==============================================================================
//	デバック定義
//==============================================================================
//スタートボタンを押すと、Ａボタン押しっぱなし状態になります
//ポケモンリスト選択が終了したら、スタートボタンを押してください
//#define DEBUG_FRONTIER_LOOP					//定義有効で同じラウンドをループ


//============================================================================================
//
//	全施設　共通定義(enum禁止)
//
//============================================================================================
#define FRONTIER_COMM_PLAYER_NUM		(2)			//通信人数
#define FRONTIER_COMM_POKE_NUM			(2)			//通信ポケモン数

#define WIFI_COUNTER_ENTRY_POKE_MAX		(2)			//参加

//通信タイプ
#define FRONTIER_COMM_SEL				(0)
#define FRONTIER_COMM_YAMERU			(1)
#define FRONTIER_COMM_MONSNO_ITEMNO		(2)
#define FRONTIER_COMM_STAGE_RECORD_DEL	(4)
#define FRONTIER_COMM_GAME_CONTINUE		(5)

//施設タイプ
#define FRONTIER_NO_NONE				(0)
#define FRONTIER_NO_TOWER				(1)
#define FRONTIER_NO_FACTORY_LV50		(2)
#define FRONTIER_NO_FACTORY_LV100		(3)
#define FRONTIER_NO_CASTLE				(4)			//注意　キャッスル、ステージの順番
#define FRONTIER_NO_STAGE				(5)
#define FRONTIER_NO_ROULETTE			(6)
#define FRONTIER_NO_YAMERU				(7)

//ポケモンリスト、ステータス
#define FR_WIFI_POKESEL_PLIST_CALL		(0)				//ポケモンリスト呼び出し
#define FR_WIFI_POKESEL_PLIST_WAIT		(1)				//ポケモンリスト終了待ち
#define FR_WIFI_POKESEL_PST_CALL		(2)				//ポケモンステータス呼び出し
#define FR_WIFI_POKESEL_PST_WAIT		(3)				//ポケモンステータス終了待ち
#define FR_WIFI_POKESEL_EXIT			(4)				//終了

//エンカウントエフェクト
#define FR_ENCOUNT_EFF_WIPE				(0)
#define FR_ENCOUNT_EFF_BOX_D			(1)
#define FR_ENCOUNT_EFF_BOX_LR			(2)
#define FR_ENCOUNT_EFF_CLOSE			(3)
#define FR_ENCOUNT_EFF_ZIGZAG			(4)

//FRWIFI_SCRWORKの取得コード(wifi_counter.sで使用)
#define FWIFI_ID_COMM_COMMAND_INITIALIZE	(0)
#define FWIFI_ID_GET_PAIR_POKELIST_CANCEL	(1)
#define FWIFI_ID_CHECK_ENTRY				(2)
#define FWIFI_ID_STAGE_GET_CLEAR_FLAG		(3)
#define FWIFI_ID_STAGE_GET_CLEAR_MONSNO		(4)
#define FWIFI_ID_GET_ENTRY_MONSNO			(5)
#define FWIFI_ID_STAGE_SET_NEW_CHALLENGE	(6)
#define FWIFI_ID_SET_BF_NO					(7)
#define FWIFI_ID_GET_PAIR_STAGE_RECORD_DEL	(8)
#define FWIFI_ID_GET_BF_NO					(9)
#define FWIFI_ID_GET_PAIR_GAME_CONTINUE		(10)
#define FWIFI_ID_CHECK_PAIR_RENSYOU			(11)
#define FWIFI_ID_GET_CLEAR_FLAG				(12)
#define FWIFI_ID_POKELIST_SEL_CLEAR			(13)

//全施設の共通ウェイト
#define FRONTIER_COMMON_WAIT				(30)

//トレーナーAI(src\battle\tr_ai_def.h参照)
#define FR_AI_YASEI		(0)														//野生戦(基本AI)
#define FR_AI_BASIC		(AI_THINK_BIT_BASIC)									//基本AI(攻撃型AI)
#define FR_AI_EXPERT	(AI_THINK_BIT_BASIC | AI_THINK_BIT_ATTACK | AI_THINK_BIT_EXPERT)//expertAI

//フロンティアのセーブモード
#define FR_MODE_CLEAR	(0)					//クリア
#define FR_MODE_LOSE	(1)					//負け
#define FR_MODE_REST	(2)					//休む


//--------------------------------------------------------------------
//					     ブレーン登場連勝数
//
//タワーは、src/field/b_tower_fld.cにある
//--------------------------------------------------------------------
#define FACTORY_LEADER_SET_1ST		(21)
#define FACTORY_LEADER_SET_2ND		(49)

#define STAGE_LEADER_SET_1ST		(50)
#define STAGE_LEADER_SET_2ND		(170)

#define ROULETTE_LEADER_SET_1ST		(21)
#define ROULETTE_LEADER_SET_2ND		(49)

#define CASTLE_LEADER_SET_1ST		(21)
#define CASTLE_LEADER_SET_2ND		(49)


//--------------------------------------------------------------------
//			     ブレーンのトレーナーインデックス
//--------------------------------------------------------------------
#define STAGE_LEADER_TR_INDEX_1ST	(307)	//100戦目(0オリジン)
#define STAGE_LEADER_TR_INDEX_2ND	(308)	//170戦目


//--------------------------------------------------------------------
//			     フロンティアの記念プリントの定義
//--------------------------------------------------------------------
#define MEMORY_PRINT_NONE			(0)		//なにもなし
#define MEMORY_PRINT_PUT_1ST		(1)		//1回目のブレーンに勝利して記念プリントをあげれる状態
#define MEMORY_PRINT_PUT_OK_1ST		(2)		//1回目のブレーンに勝利した記念プリントをもらっている
#define MEMORY_PRINT_PUT_2ND		(3)		//2回目のブレーンに勝利して記念プリントをあげれる状態
#define MEMORY_PRINT_PUT_OK_2ND		(4)		//2回目のブレーンに勝利した記念プリントをもらっている


#endif	//__FRONTIER_DEF_H__


