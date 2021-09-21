//=============================================================================
/**
 * @file	comm_direct_counter_def.h
 * @bfief	通信ダイレクトコーナーのカウンター スクリプトと共用の定義
 * @author	katsumi ohno
 * @date	05/08/04
 */
//=============================================================================

#ifndef __COMM_DIRECT_COUNTER_DEF_H__
#define __COMM_DIRECT_COUNTER_DEF_H__


#define DBC_TIM_BATTLE_MPDS (3)  // 対戦部屋->バトル
#define DBC_TIM_BATTLE_DSOK (4)  // バトルモードになった
#define DBC_TIM_BATTLE_MPOK (5)  // 歩きモードになった


#define DBC_TIM_BATTLE_BACK (30)    // 対戦部屋待機状態位置移動完了
#define DBC_TIM_BATTLE_PAUSE (92)    // 対戦部屋待機状態
#define DBC_TIM_BATTLE_START (93)    // 対戦部屋終了 > バトル
#define DBC_TIM_BATTLE_EXIT  (94)    // 対戦部屋出て行くとき
#define DBC_TIM_BATTLE_EXIT2  (91)    // 対戦部屋出て行くとき  もう一回

#define DBC_TIM_BATTLE_TR (95)    // 対戦部屋トレーナーカード交換待機
#define DBC_TIM_BATTLE_ROOMIN	(96) // 対戦部屋に入るとき
#define DBC_TIM_BATTLE_DSCHANGE (97)  // データ形式切り替え
#define DBC_TIM_BATTLE_DSEND (98) // 切り替え完了
//-----------------------------------------------------------------------------
//ステージ
//-----------------------------------------------------------------------------
#define DBC_TIM_STAGE_POKETYPE_SEL	(100)	// ポケモンタイプ選択前の同期
#define DBC_TIM_STAGE_BATTLE		(101)	// 戦闘前の同期
#define DBC_TIM_STAGE_MENU			(102)	// つづける選んだ後の同期
#define DBC_TIM_STAGE_TR_ENEMY		(103)	// トレーナーとポケモンを通信終了の同期
#define DBC_TIM_STAGE_END			(104)	// タイプ選択画面終了の同期
#define DBC_TIM_STAGE_LOSE_END		(105)	// 戦闘負け終了の同期
#define DBC_TIM_STAGE_SAVE_AFTER	(106)	// セーブ後のマップ遷移前の同期
#define DBC_TIM_STAGE_GIVE			(107)	// やめる選んだ後の同期
//d32r0401.ev
#define STAGE_COMM_MULTI_POKE_SELECT	(108)
#define STAGE_COMM_MULTI_AUTO_SAVE		(109)
#define STAGE_COMM_MULTI_GO_SAVE		(110)
//
#define DBC_TIM_STAGE_INIT			(111)	// 通信初期化の後の同期
#define DBC_TIM_STAGE_TYPE_INIT		(112)	// 通信初期化後のタイプ選択画面の同期
//-----------------------------------------------------------------------------
//キャッスル
//-----------------------------------------------------------------------------
#define DBC_TIM_CASTLE_RENTAL		(120)	// レンタル画面へ行く前の同期
#define DBC_TIM_CASTLE_BATTLE		(121)	// 戦闘前の同期
#define DBC_TIM_CASTLE_MENU			(122)	// つづけるを選んだ後の同期
#define DBC_TIM_CASTLE_INIT			(123)	// 通信初期化の後の同期
#define DBC_TIM_CASTLE_GIVE			(124)	// やめるを選んだ後の同期
#define DBC_TIM_CASTLE_POKE_DATA	(125)	// ポケモンデータ送信前の同期
#define DBC_TIM_CASTLE_CHANGE		(126)	// 別画面に行く前の同期
#define DBC_TIM_CASTLE_MENU_BEFORE	(127)	// メニュー表示前の同期
#define DBC_TIM_CASTLE_LOSE_END		(128)	// 戦闘負け終了の同期
#define DBC_TIM_CASTLE_SAVE_AFTER	(129)	// セーブ後のマップ遷移前の同期
//手持ち
#define DBC_TIM_CASTLE_MINE_UP		(130)	// 手持ち：ランクアップ後のメッセージ終了の同期
#define DBC_TIM_CASTLE_MINE_END		(131)	// 手持ち：画面終了の同期
#define DBC_TIM_CASTLE_MINE_CANCEL	(132)	// 手持ち：「戻る」キャンセルの同期
//敵トレーナー
#define DBC_TIM_CASTLE_ENEMY_UP		(133)	// 敵：ランクアップ後のメッセージ終了の同期
#define DBC_TIM_CASTLE_ENEMY_END	(134)	// 敵：画面終了の同期
#define DBC_TIM_CASTLE_ENEMY_CANCEL	(135)	// 敵：「戻る」キャンセルの同期
//d32r0501.ev
#define CASTLE_COMM_MULTI_POKE_SELECT	(136)
#define CASTLE_COMM_MULTI_AUTO_SAVE		(137)
#define CASTLE_COMM_MULTI_GO_SAVE		(138)
#define CASTLE_COMM_MULTI_SIO_END		(139)
//-----------------------------------------------------------------------------
//ルーレット
//-----------------------------------------------------------------------------
#define DBC_TIM_ROULETTE_RENTAL			(140)	// トレーナー登場前の同期
#define DBC_TIM_ROULETTE_BATTLE			(141)	// 戦闘前の同期
#define DBC_TIM_ROULETTE_MENU			(142)	// つづけるを選んだ後の同期
#define DBC_TIM_ROULETTE_CALL			(143)	// ルーレット画面へ行く前の同期
#define DBC_TIM_ROULETTE_GIVE			(144)	// やめるを選んだ後の同期
#define DBC_TIM_ROULETTE_INIT			(145)	// 通信初期化の後の同期
#define DBC_TIM_ROULETTE_CHANGE			(146)	// 別画面に行く前の同期
#define DBC_TIM_ROULETTE_BTL_WIN_PANEL	(147)	// 戦闘勝利パネルの同期
#define DBC_TIM_ROULETTE_LOSE_END		(148)	// 戦闘負け終了の同期
#define DBC_TIM_ROULETTE_PANEL_INFO		(149)	// パネルの説明後の同期
#define DBC_TIM_ROULETTE_SAVE_AFTER		(150)	// セーブ後のマップ遷移前の同期
//ランク
#define DBC_TIM_ROULETTE_UP				(151)	// ランク：ランクアップ後のメッセージ終了の同期
#define DBC_TIM_ROULETTE_END			(152)	// ランク：画面終了の同期
#define DBC_TIM_ROULETTE_CANCEL			(153)	// ランク：「戻る」キャンセルの同期
//d32r0601.ev
#define ROULETTE_COMM_MULTI_POKE_SELECT	(154)
#define ROULETTE_COMM_MULTI_AUTO_SAVE	(155)
#define ROULETTE_COMM_MULTI_GO_SAVE		(156)
#define ROULETTE_COMM_MULTI_SIO_END		(157)
//
#define DBC_TIM_ROULETTE_POKE_DATA		(158)	// ポケモンデータ送信前の同期
//-----------------------------------------------------------------------------
//ファクトリー
//-----------------------------------------------------------------------------
#define DBC_TIM_FACTORY_INIT			(159)	// 通信初期化の後の同期
#define DBC_TIM_FACTORY_RENTAL			(160)	// レンタル画面へ行く前の同期
#define DBC_TIM_FACTORY_BATTLE			(161)	// 戦闘前の同期
#define DBC_TIM_FACTORY_MENU			(162)	// つづける選んだ後の同期
#define DBC_TIM_FACTORY_TRADE			(163)	// 交換画面へ行く前の同期
#define DBC_TIM_FACTORY_RENTAL_END		(164)	// レンタル画面終了の同期
#define DBC_TIM_FACTORY_TRADE_END		(165)	// 交換画面終了の同期
#define DBC_TIM_FACTORY_LOSE_END		(166)	// 戦闘負け終了の同期
#define DBC_TIM_FACTORY_SAVE_AFTER		(167)	// セーブ後のマップ遷移前の同期
#define DBC_TIM_FACTORY_GIVE			(168)	// やめる選んだ後の同期
//d32r0301.ev
#define FACTORY_COMM_MULTI_AUTO_SAVE	(169)
//-----------------------------------------------------------------------------
//フロンティアWiFi受付
//-----------------------------------------------------------------------------
#define DBC_TIM_FR_WIFI_COUNTER_YAMERU	(170)	// 「やめる」選択した後の同期
#define DBC_TIM_FR_WIFI_COUNTER_GO		(171)	// 各施設へ行く前の同期
#define DBC_TIM_FR_WIFI_COUNTER_END		(172)	// 引き続き遊ばないで終了する時の同期
#define DBC_TIM_FR_WIFI_COUNTER_GO2		(173)	// 各施設へ行く前の最終同期
#define DBC_TIM_FR_WIFI_COUNTER_TOWER	(174)	// タワーのデータ交換前の同期
#define DBC_TIM_FR_WIFI_COUNTER_SAVE	(175)	// セーブ前の同期
#define DBC_TIM_FR_WIFI_COUNTER_INIT_1	(176)	// 通信初期化の後の同期
#define DBC_TIM_FR_WIFI_COUNTER_INIT_2	(177)	// 通信初期化の後の同期
#define DBC_TIM_FR_WIFI_COUNTER_END_2	(178)	// クラブに戻る前の同期
//-----------------------------------------------------------------------------
//通信コマンド初期化後に同期
//-----------------------------------------------------------------------------
#define DBC_TIM_ROULETTE_INIT_1			(180)	// 通信初期化の後の同期
#define DBC_TIM_ROULETTE_INIT_2			(181)	// 通信初期化の後の同期
#define DBC_TIM_ROULETTE_INIT_3			(182)	// 通信初期化の後の同期
#define DBC_TIM_ROULETTE_INIT_4			(183)	// 通信初期化の後の同期
#define DBC_TIM_ROULETTE_INIT_5			(199)	// 通信初期化の後の同期(専用画面)

#define DBC_TIM_STAGE_INIT_1			(185)	// 通信初期化の後の同期
#define DBC_TIM_STAGE_INIT_2			(186)	// 通信初期化の後の同期
#define DBC_TIM_STAGE_INIT_3			(187)	// 通信初期化の後の同期
#define DBC_TIM_STAGE_INIT_4			(188)	// 通信初期化の後の同期

#define DBC_TIM_CASTLE_INIT_1			(190)	// 通信初期化の後の同期
#define DBC_TIM_CASTLE_INIT_2			(191)	// 通信初期化の後の同期
#define DBC_TIM_CASTLE_INIT_3			(192)	// 通信初期化の後の同期
#define DBC_TIM_CASTLE_INIT_4			(193)	// 通信初期化の後の同期

#define DBC_TIM_FACTORY_INIT_1			(195)	// 通信初期化の後の同期
#define DBC_TIM_FACTORY_INIT_2			(196)	// 通信初期化の後の同期
#define DBC_TIM_FACTORY_INIT_3			(197)	// 通信初期化の後の同期
#define DBC_TIM_FACTORY_INIT_4			(198)	// 通信初期化の後の同期

#define DBC_TIM_CASTLE_MINE_CALL		(240)	// あいて画面へ進む前の同期
#define DBC_TIM_CASTLE_ENEMY_CALL		(241)	// あいて画面へ進む前の同期
//-----------------------------------------------------------------------------
//ルーレット
//-----------------------------------------------------------------------------
#define DBC_TIM_ROULETTE_CALL_2			(200)	// 専用画面に行く前に再度の同期
#define DBC_TIM_ROULETTE_PANEL			(201)	// パネル選出後の同期
#define DBC_TIM_ROULETTE_BOTH_0			(202)	// 大量データoff前の同期
#define DBC_TIM_ROULETTE_BASIC			(203)	// 基本データ送信前の同期
#define DBC_TIM_ROULETTE_TR				(204)	// トレーナーデータ送信前の同期
#define DBC_TIM_ROULETTE_ENEMY_BEFORE	(205)	// 敵トレーナー送信前の同期
#define DBC_TIM_ROULETTE_ENEMY_AFTER	(206)	// 敵トレーナー送信後の同期
//-----------------------------------------------------------------------------
//キャッスル
//-----------------------------------------------------------------------------
#define DBC_TIM_CASTLE_BOTH_0			(210)	// 大量データoff前の同期
#define DBC_TIM_CASTLE_BASIC_AFTER		(211)	// 基本情報送信後の同期
#define DBC_TIM_CASTLE_TR_BEFORE		(212)	// トレーナーデータ送信前の同期
#define DBC_TIM_CASTLE_ENEMY_BEFORE		(213)	// 敵ポケモン生成前の同期
#define DBC_TIM_CASTLE_ENEMY_AFTER		(214)	// 敵ポケモン生成後の同期
#define DBC_TIM_CASTLE_MINE_INIT		(215)	// 通信初期化後の手持ち画面の同期
#define DBC_TIM_CASTLE_ENEMY_INIT		(216)	// 通信初期化後のあいて画面の同期
//-----------------------------------------------------------------------------
//ステージ
//-----------------------------------------------------------------------------
#define DBC_TIM_STAGE_BOTH_1			(220)	// 大量データon前の同期
#define DBC_TIM_STAGE_BOTH_0			(221)	// 大量データoff前の同期
#define DBC_TIM_STAGE_TR_AFTER			(222)	// トレーナーデータ送信後の同期
#define DBC_TIM_STAGE_INIT_BEFORE		(223)	// 通信初期化の前の同期
#define DBC_TIM_STAGE_TYPE_SEL_CALL		(224)	// タイプ選択画面へ進む前の同期
//-----------------------------------------------------------------------------
//ファクトリー
//-----------------------------------------------------------------------------
#define DBC_TIM_FACTORY_BASIC_AFTER		(230)	// 基本情報送信後の同期
#define DBC_TIM_FACTORY_TR_AFTER		(231)	// トレーナーデータ送信後の同期
#define DBC_TIM_FACTORY_ENEMY_AFTER		(232)	// 敵ポケモン送信後の同期
#define DBC_TIM_FACTORY_TRADE_AFTER		(233)	// 交換終了後の同期
#define DBC_TIM_FACTORY_RENTAL_AFTER	(234)	// レンタル終了後の同期
#define DBC_TIM_FACTORY_RENTAL_CALL		(235)	// レンタル画面へ進む前の同期
#define DBC_TIM_FACTORY_TRADE_CALL		(236)	// レンタル画面へ進む前の同期
#define DBC_TIM_FACTORY_START_INIT		(237)	// 通信初期化後のレンタル交換画面の同期
#define DBC_TIM_FACTORY_ENEMY_BEFORE	(238)	// 敵ポケモン送信前の同期


#define REGULATION_RET_NONE  (3)   // レギュレーションいらない
#define REGULATION_RET_OK  (1)   // レギュレーション完了
#define REGULATION_RET_END (2)   // レギュレーションを選択せずに終了


#endif //__COMM_DIRECT_COUNTER_DEF_H__
