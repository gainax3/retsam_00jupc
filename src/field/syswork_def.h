//============================================================================================
/**
 * @file	syswork_def.h
 * @bfief	システムワーク定義
 * @author	Satoshi Nohara
 * @date	06.04.14
 */
//============================================================================================
#ifndef	__SYSWORK_DEF_H__
#define	__SYSWORK_DEF_H__


//============================================================================================
//	フラグ定義
//
//	スクリプト			定義を直接参照してよい！
//
//	プログラム			syswork.hにあるアクセス関数を使う！
//
//============================================================================================
#define		SYS_WORK_FIRST_POKE_NO		(0+OTHERS_WORK_START)	//最初のポケモンナンバー
//#define	SYS_WORK_POISON_STEP_COUNT	(1+OTHERS_WORK_START)	//毒歩数カウンター
#define		SYS_WORK_INTERVIEW_NO		(2+OTHERS_WORK_START)	//15-18のインタビューナンバー
#define		SYS_WORK_BTL_SEARCHER_BATTERY	(3+OTHERS_WORK_START)	//バトルサーチャーバッテリー
#define		SYS_WORK_BTL_SEARCHER_CLEAR	(4+OTHERS_WORK_START)	//バトルサーチャークリア
#define		SYS_WORK_OOKISA_RECORD		(5+OTHERS_WORK_START)	//大きさ比べ用レコード（固体値）
#define		SYS_WORK_HIDEMAP_01			(6+OTHERS_WORK_START)	//隠しマップ判定用ワークその１
#define		SYS_WORK_HIDEMAP_02			(7+OTHERS_WORK_START)	//隠しマップ判定用ワークその２
#define		SYS_WORK_HIDEMAP_03			(8+OTHERS_WORK_START)	//隠しマップ判定用ワークその３
#define		SYS_WORK_HIDEMAP_04			(9+OTHERS_WORK_START)	//隠しマップ判定用ワークその４
#define		SYS_WORK_PARK_WALK_COUNT	(10+OTHERS_WORK_START)	//自然公園での歩数カウント
#define		SYS_WORK_NEWS_DAYS_COUNT	(11+OTHERS_WORK_START)	//新聞社締め切りカウンタ
#define		SYS_WORK_POKELOT_RND1		(12+OTHERS_WORK_START)	//ポケモンクジ用32bit
#define		SYS_WORK_POKELOT_RND2		(13+OTHERS_WORK_START)	//ポケモンクジ用32bit
#define		SYS_WORK_UG_TALK_COUNT		(14+OTHERS_WORK_START)	//地下会話カウント用(ミカゲ用）
#define		SYS_WORK_PAIR_TRAINER_ID	(15+OTHERS_WORK_START)	//連れ歩きトレーナーID
#define		SYS_WORK_RENSYOU_COUNT		(16+OTHERS_WORK_START)	//スロット連勝最大数
#define		SYS_WORK_POKE_LEVEL_NO		(17+OTHERS_WORK_START)	//ポケモンレベルでアイテム入手
#define		SYS_WORK_DEPART_COUNT		(18+OTHERS_WORK_START)	//デパートでの購入回数
#define		SYS_WORK_HAIHU_EVENT01		(19+OTHERS_WORK_START)	//配布イベント判定用ワークその１
#define		SYS_WORK_HAIHU_EVENT02		(20+OTHERS_WORK_START)	//配布イベント判定用ワークその２
#define		SYS_WORK_HAIHU_EVENT03		(21+OTHERS_WORK_START)	//配布イベント判定用ワークその３
#define		SYS_WORK_HAIHU_EVENT04		(22+OTHERS_WORK_START)	//配布イベント判定用ワークその４
#define		SYS_WORK_UG_KASEKI_DIG_COUNT	(23+OTHERS_WORK_START)	//地下 かせきを掘った回数
#define		SYS_WORK_UG_TRAP_HIT_COUNT	(24+OTHERS_WORK_START)	//地下 トラップにかけた回数
#define		SYS_WORK_UG_TALK_COUNT2		(25+OTHERS_WORK_START)	//地下会話カウント用
#define		SYS_WORK_FRIENDLY_COUNT		(26+OTHERS_WORK_START)	//なつき度用歩数カウント
#define		SYS_WORK_PL_GRASS_GYM_TIME	(27+OTHERS_WORK_START)	//PL草ジム　時間カウント
#define		SYS_WORK_MAID_TURN			(28+OTHERS_WORK_START)	//メイド勝ち抜きターン数(目標)
#define		SYS_WORK_MAID_WIN_TURN		(29+OTHERS_WORK_START)	//メイド勝ち抜きターン数(勝ちターン)
#define		SYS_WORK_EVENT_T07R0201		(30+OTHERS_WORK_START)	//別荘イベントナンバー
#define		SYS_WORK_MEMORY_PRINT_TOWER		(31+OTHERS_WORK_START)	//記念プリント(タワー)
#define		SYS_WORK_MEMORY_PRINT_FACTORY	(32+OTHERS_WORK_START)	//記念プリント(ファクトリー)
#define		SYS_WORK_MEMORY_PRINT_STAGE		(33+OTHERS_WORK_START)	//記念プリント(ステージ)
#define		SYS_WORK_MEMORY_PRINT_CASTLE	(34+OTHERS_WORK_START)	//記念プリント(キャッスル)
#define		SYS_WORK_MEMORY_PRINT_ROULETTE	(35+OTHERS_WORK_START)	//記念プリント(ルーレット)
#define		SYS_WORK_UG_TOOL_GIVE_COUNT	(36+OTHERS_WORK_START)//地下 道具をあげた回数(★位置変えた)
#define		SYS_WORK_TW_EVSEQ			(37+OTHERS_WORK_START)	//破れた世界イベント進行状況
#define		SYS_WORK_ARUSEUSU			(38+OTHERS_WORK_START)	//アルセウスイベント
#define		SYS_WORK_SYEIMI				(39+OTHERS_WORK_START)	//シェイミイベント
#define		SYS_WORK_MUUBASU_GET		(40+OTHERS_WORK_START)	//ムーバス捕獲成功
#define		SYS_WORK_AI_GET				(41+OTHERS_WORK_START)	//アイ捕獲成功
#define		SYS_WORK_T07R0201_TALK		(42+OTHERS_WORK_START)	//別荘会話ナンバー
#define		SYS_WORK_TW_AKAGI_APPEAR		(43+OTHERS_WORK_START) //破れた世界アカギ出現管理 扱いは0,1のフラグ管理のみ
#define		SYS_WORK_PC_UG_ZONE_ID		(44+OTHERS_WORK_START)	//どこのポケセン地下か保存
#define		SYS_WORK_PC_UG_OBJ_ID		(45+OTHERS_WORK_START)	//ポケセン地下のOBJID保存
#define		SYS_WORK_FAIYAA_GET			(46+OTHERS_WORK_START)	//ファイヤー捕獲成功
#define		SYS_WORK_SANDAA_GET			(47+OTHERS_WORK_START)	//サンダー捕獲成功
#define		SYS_WORK_HURIIZAA_GET		(48+OTHERS_WORK_START)	//フリーザー捕獲成功
#define		SYS_WORK_T06R0101_TALK		(49+OTHERS_WORK_START)	//再戦施設ランダム会話のどちらか
#define		SYS_WORK_STAGE_TOTAL_LV		(50+OTHERS_WORK_START)	//ステージトータルレコードLV
#define		SYS_WORK_STAGE_OUEN			(51+OTHERS_WORK_START)	//ステージ応援キャラコード
#define		SYS_WORK_WIFI_FR_CLEAR_FLAG	(52+OTHERS_WORK_START)	//WIFIフロンティアのTEMPクリア
#define		SYS_WORK_T06R0101_OBJ1		(53+OTHERS_WORK_START)	//再戦施設OBJコード1
#define		SYS_WORK_T06R0101_OBJ2		(54+OTHERS_WORK_START)	//再戦施設OBJコード2
#define		SYS_WORK_T06R0101_OBJ3		(55+OTHERS_WORK_START)	//再戦施設OBJコード3
#define		SYS_WORK_T06R0101_OBJ4		(56+OTHERS_WORK_START)	//再戦施設OBJコード4
#define		SYS_WORK_D35R0102_REZI		(57+OTHERS_WORK_START)	//レジスチルイベント
#define		SYS_WORK_D35R0104_REZI		(58+OTHERS_WORK_START)	//レジアイスイベント
#define		SYS_WORK_D35R0106_REZI		(59+OTHERS_WORK_START)	//レジロックイベント

//ワークの最大数など確認する！
//0 - 63

//============================================================================================
//
//		配布イベント用定義
//
//============================================================================================
#define		HAIHUEVENT_ID_D30		0		///<配布イベント：ダーレイ（ダーク）
#define		HAIHUEVENT_ID_D18		1		///<配布イベント：ミーシエ（エウリス）
#define		HAIHUEVENT_ID_D05		2		///<配布イベント：アルセウス（アウス）
#define		HAIHUEVENT_ID_C04		3		///<配布イベント：エウリス（ロトム）

//============================================================================================
//
//		隠しマップ共通定義
//
//============================================================================================
#define		HIDEMAP_ID_D15	(0)				///<隠しマップ：D15
#define		HIDEMAP_ID_D30	(1)				///<配布マップ：D30
#define		HIDEMAP_ID_L04	(2)				///<隠しマップ：L04
#define		HIDEMAP_ID_D18	(3)				///<配布マップ：D18
#endif	//__SYSWORK_DEF_H__



