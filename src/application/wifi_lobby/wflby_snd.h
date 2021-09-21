//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_snd.h
 *	@brief		広場音楽ヘッダ
 *	@author		tomoya takahashi
 *	@data		2008.01.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_SND_H__
#define __WFLBY_SND_H__

#include "system/snd_tool.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

#define WFLBY_SND_JUMP		( SEQ_SE_PL_DANSA5 )		// 乗り物に乗る音

#define WFLBY_SND_MINIGAME	( SEQ_SE_DP_DECIDE )		// ミニゲームカウンターに入った音
//#define WFLBY_SND_MINIGAME	( SEQ_SE_PL_TIMER02 )		// ミニゲームカウンターに入った音

#define WFLBY_SND_CURSOR	( SEQ_SE_DP_SELECT )		// リストのカーソル音

#define WFLBY_SND_STAFF		( SEQ_SE_DP_SELECT )		// スタッフに話しかけたとき

#define WFLBY_SND_PLIN		( SEQ_SE_DP_TELE2 )			// 人が降りてくる音
#define WFLBY_SND_PLOUT		( SEQ_SE_DP_TELE2 )			// 人が上る音

#define WFLBY_SND_DOUZOU	( SEQ_SE_PL_EFF02 )			// 銅像に話しかけ(おもちゃが動く音)
#define WFLBY_SND_DOUZOU2	( SEQ_SE_PL_EFF04 )			// 銅像に話しかけ(おもちゃが動く音)ミュウ

//未使用
//#define WFLBY_SND_NEWSIN	( SEQ_SE_PL_BREC16 )		// ロビーニュースに入った音
//#define WFLBY_SND_WLDTMIN	( SEQ_SE_PL_BREC16 )		// 世界時計に入った音
//#define WFLBY_SND_FOOTIN	( SEQ_SE_PL_BREC16 )		// 足跡ボードに入った音
#define WFLBY_SND_NEWSIN	( SEQ_SE_DP_SELECT )		// ロビーニュースに入った音
#define WFLBY_SND_WLDTMIN	( SEQ_SE_DP_SELECT )		// 世界時計に入った音
#define WFLBY_SND_FOOTIN	( SEQ_SE_DP_SELECT )		// 足跡ボードに入った音

#define WFLBY_SND_TALK		( SEQ_SE_DP_BUTTON9 )		// 話しかけ音

#define WFLBY_SND_PROFILE	( SEQ_SE_DP_SELECT )		// Xボタンで自分のプロフィールを開く

//-----------------------------------------------------------------------------
//
//	フロート関連			
//
//	鳴き声＋エフェクト＋モーターがなるかも？
//
//	乗り物動かすボタンは共通
//	○_PV = 鳴き声ならすボタン(鳴き声処理＋SEを再生)
//	○_EX = 特殊動きボタン
//-----------------------------------------------------------------------------
#define WFLBY_SND_RIDE_INVALIDITY			(0xfffe)	// SEを再生しない定義

//#define WFLBY_SND_RIDE_BUTTON01	( SEQ_SE_PL_FLOAT03 )// フロート搭乗時に下ボタン(乗物を動かす)
#define WFLBY_SND_RIDE_BUTTON01	( SEQ_SE_PL_FLOAT13 )	// フロート搭乗時に下ボタン(乗物を動かす)
//#define WFLBY_SND_RIDE_BUTTON02	( SEQ_SE_PL_FLOAT03 )// フロート搭乗時に下ボタン(ライト動かす)
//#define WFLBY_SND_RIDE_BUTTON03	( SEQ_SE_PL_FLOAT03 )// フロート搭乗時に下ボタン(鳴き声ならす)
//↑ここはSnd_PMVoicePlay( monsno, 0 );でお願いします

//01(アチャモ)
//トサカが揺れる
#define WFLBY_SND_RIDE_BUTTON_ATYAMO_PV		( WFLBY_SND_RIDE_INVALIDITY )	//目の表情が変わる
#define WFLBY_SND_RIDE_BUTTON_ATYAMO_EX		( SEQ_SE_PL_FLOAT10 )	//トサカが揺れる

//02(リザードン)
#define WFLBY_SND_RIDE_BUTTON_RIZAADON_PV	( SEQ_SE_PL_FIRE )		//炎が出る
//#define WFLBY_SND_RIDE_BUTTON_RIZAADON_PV	( WFLBY_SND_RIDE_INVALIDITY )		//炎が出る
#define WFLBY_SND_RIDE_BUTTON_RIZAADON_EX	( SEQ_SE_PL_FLOAT03 )	//羽がひらく

//03(フシギダネ)
#define WFLBY_SND_RIDE_BUTTON_HUSIGIDANE_PV	( SEQ_SE_PL_WHIP )		//つるの鞭が出る
#define WFLBY_SND_RIDE_BUTTON_HUSIGIDANE_EX	( SEQ_SE_PL_FLOAT11 )	//背中のつぼみがはまって飛び出す

//04(マスキッパ)
#define WFLBY_SND_RIDE_BUTTON_HAEZIGOKU_PV	( SEQ_SE_PL_FLOAT03 )	//口が動く
#define WFLBY_SND_RIDE_BUTTON_HAEZIGOKU_EX	( SEQ_SE_PL_FLOAT10 )	//手が動く

//05(ポッチャマ)
#define WFLBY_SND_RIDE_BUTTON_POTTYAMA_PV	( SEQ_SE_PL_FLOAT10 )	//足が動く
//#define WFLBY_SND_RIDE_BUTTON_POTTYAMA_EX	( SEQ_SE_PL_FLOAT10 )	//手が回る
#define WFLBY_SND_RIDE_BUTTON_POTTYAMA_EX	( SEQ_SE_PL_FLOAT12 )	//手が回る

//06(ギャラドス)
#define WFLBY_SND_RIDE_BUTTON_GYARADOSU_PV	( SEQ_SE_PL_WATER )			//水が出る
//#define WFLBY_SND_RIDE_BUTTON_GYARADOSU_PV	( WFLBY_SND_RIDE_INVALIDITY )	//水が出る
#define WFLBY_SND_RIDE_BUTTON_GYARADOSU_EX	( SEQ_SE_PL_FLOAT10 )	//尻尾が動く

//07(ピカチュウ)
//#define WFLBY_SND_RIDE_BUTTON_PIKATYUU_PV	( SEQ_SE_PL_FLOAT03 )	//顔の表情が変わる
#define WFLBY_SND_RIDE_BUTTON_PIKATYUU_PV	( WFLBY_SND_RIDE_INVALIDITY )	//顔の表情が変わる
#define WFLBY_SND_RIDE_BUTTON_PIKATYUU_EX	( SEQ_SE_PL_FLOAT10 )	//尻尾が動く

//08(ジバコイル)
//#define WFLBY_SND_RIDE_BUTTON_DEKAKOIRU_PV	( SEQ_SE_PL_FLOAT03 )		//目が動く
#define WFLBY_SND_RIDE_BUTTON_DEKAKOIRU_PV	( WFLBY_SND_RIDE_INVALIDITY )	//目が動く
#define WFLBY_SND_RIDE_BUTTON_DEKAKOIRU_EX	( SEQ_SE_PL_ELECTRO )	//電気が出る

//09(ミュウ)
//#define WFLBY_SND_RIDE_BUTTON_MYUU_PV		( SEQ_SE_PL_FLOAT03 )	//足が動く
#define WFLBY_SND_RIDE_BUTTON_MYUU_PV		( WFLBY_SND_RIDE_INVALIDITY )	//足が動く
#define WFLBY_SND_RIDE_BUTTON_MYUU_EX		( SEQ_SE_PL_FLOAT10 )	//尻尾が動く
#define WFLBY_SND_RIDE_BUTTON_MYUU_PV_2		( SEQ_SE_PL_KIRAKIRA4 )	//キラキラエフェクト(3台目のみ)


//-----------------------------------------------------------------------------
//
//	タッチトイ関連
//
//-----------------------------------------------------------------------------
#if 0	//2ch
#define WFLBY_SND_TOUCH_TOY01_1	( SEQ_SE_PL_HIROBA01 )	// ベル
#define WFLBY_SND_TOUCH_TOY01_2	( SEQ_SE_PL_HIROBA01_2 )// ベル
#define WFLBY_SND_TOUCH_TOY01_3	( SEQ_SE_PL_HIROBA01_3 )// ベル
#else	//1ch
#define WFLBY_SND_TOUCH_TOY01_1	( SEQ_SE_PL_HIROBA01_4 )// ベル
#define WFLBY_SND_TOUCH_TOY01_2	( SEQ_SE_PL_HIROBA01_5 )// ベル
#define WFLBY_SND_TOUCH_TOY01_3	( SEQ_SE_PL_HIROBA01_6 )// ベル
#endif

#if 1	//2ch(ここだけ1chだと聞こえないため)
#define WFLBY_SND_TOUCH_TOY02_1	( SEQ_SE_PL_HIROBA03 )	// ドラム
#define WFLBY_SND_TOUCH_TOY02_2	( SEQ_SE_PL_HIROBA03_2 )// ドラム
#define WFLBY_SND_TOUCH_TOY02_3	( SEQ_SE_PL_HIROBA03_3 )// ドラム
#else
#define WFLBY_SND_TOUCH_TOY02_1	( SEQ_SE_PL_HIROBA03_4 )// ドラム
#define WFLBY_SND_TOUCH_TOY02_2	( SEQ_SE_PL_HIROBA03_5 )// ドラム
#define WFLBY_SND_TOUCH_TOY02_3	( SEQ_SE_PL_HIROBA03_6 )// ドラム
#endif	//1ch

#if 0	//2ch
#define WFLBY_SND_TOUCH_TOY03_1	( SEQ_SE_PL_HIROBA02 )	// シンバル
#define WFLBY_SND_TOUCH_TOY03_2	( SEQ_SE_PL_HIROBA02_2 )// シンバル
#define WFLBY_SND_TOUCH_TOY03_3	( SEQ_SE_PL_HIROBA02_3 )// シンバル
#else	//1ch
#define WFLBY_SND_TOUCH_TOY03_1	( SEQ_SE_PL_HIROBA02_4 )// シンバル
#define WFLBY_SND_TOUCH_TOY03_2	( SEQ_SE_PL_HIROBA02_5 )// シンバル
#define WFLBY_SND_TOUCH_TOY03_3	( SEQ_SE_PL_HIROBA02_6 )// シンバル
#endif

#define WFLBY_SND_TOUCH_TOY04_1	( SEQ_SE_PL_HIROBA70 )	// リップル(はもん)
#define WFLBY_SND_TOUCH_TOY04_2	( SEQ_SE_PL_HIROBA70_2 )// リップル(はもん)
#define WFLBY_SND_TOUCH_TOY04_3	( SEQ_SE_PL_HIROBA70_3 )// リップル(はもん)
#define WFLBY_SND_TOUCH_TOY05_1	( SEQ_SE_PL_HIROBA100 )	// シグナル
#define WFLBY_SND_TOUCH_TOY05_2	( SEQ_SE_PL_HIROBA100_2)// シグナル
#define WFLBY_SND_TOUCH_TOY05_3	( SEQ_SE_PL_HIROBA100_3)// シグナル
#define WFLBY_SND_TOUCH_TOY06_1	( SEQ_SE_PL_HIROBA05 )	// スイング
#define WFLBY_SND_TOUCH_TOY06_2	( SEQ_SE_PL_HIROBA05_2 )// スイング
#define WFLBY_SND_TOUCH_TOY06_3	( SEQ_SE_PL_HIROBA05_3 )// スイング
#define WFLBY_SND_TOUCH_TOY07_1	( SEQ_SE_PL_W030 )		// クラッカー(小)
#define WFLBY_SND_TOUCH_TOY07_2	( SEQ_SE_PL_W030_2 )	// クラッカー
#define WFLBY_SND_TOUCH_TOY07_3	( SEQ_SE_PL_W030_3 )	// クラッカー(大)
#define WFLBY_SND_TOUCH_TOY08_1	( SEQ_SE_PL_W019 )		// 紙吹雪
#define WFLBY_SND_TOUCH_TOY08_2	( SEQ_SE_PL_W019_2 )	// 紙吹雪
#define WFLBY_SND_TOUCH_TOY08_3	( SEQ_SE_PL_W019_3 )	// 紙吹雪
#define WFLBY_SND_TOUCH_TOY09_1	( SEQ_SE_PL_HIROBA30 )	// 風船
#define WFLBY_SND_TOUCH_TOY09_2	( SEQ_SE_PL_HIROBA30 )	// 風船
#define WFLBY_SND_TOUCH_TOY09_3	( SEQ_SE_PL_HIROBA30 )	// 風船

//タッチトイの風船
#define WFLBY_SND_BALLOON_01	( SEQ_SE_PL_HIROBA20 )	// 割れる
#define WFLBY_SND_BALLOON_02	( SEQ_SE_PL_SUTYA )		// 主人公着地
//↑割れる音と同時なのでいらないかも？

//花火
#define WFLBY_SND_FIREWORKS01	( SEQ_SE_PL_140_3 )		// 設置花火(小さい打ち上げも込み)
//↑ループ音なので、終了時にSnd_SeStopBySeqNo( WFLBY_SND_FIREWORKS01, 0 );を呼んでください
//#define WFLBY_SND_FIREWORKS02	( SEQ_SE_PL_140 )		// 打上花火(最後に打ちあがる花火)
#define WFLBY_SND_FIREWORKS02	( SEQ_SE_PL_140_2 )		// 打上花火(最後に打ちあがる花火)

//タッチトイを交換する
//#define WFLBY_SND_TOUCH_TOY_CHG	( SEQ_SE_PL_EFF03 )		// 現状はSEで試す
#define WFLBY_SND_TOUCH_TOY_CHG	( SEQ_SE_PL_EFF03_4 )		// 現状はSEで試す



// プロフィールにタイプを入れる音
#define WFLBY_SND_TYPE_SET	( SEQ_SE_DP_BUTTON3 )


//-----------------------------------------------------------------------------
/**
 *			アンケート関連
 */
//-----------------------------------------------------------------------------
#define ANKETO_SND_RECV_WAIT	( SEQ_SE_PL_BREC80 )	// 受信中
#define ANKETO_SND_RECV			( SEQ_SE_DP_BUTTON9 )	// 受信
#define ANKETO_SND_SEND_WAIT	( SEQ_SE_PL_BREC80 )	// 送信中
#define ANKETO_SND_SEND			( SEQ_SE_DP_BUTTON9 )	// 送信
#define ANKETO_SND_CURSOR		( SEQ_SE_DP_SELECT78 )	// cursor音
//#define ANKETO_SND_SELECT		( SEQ_SE_DP_PIRORIRO2 )	// 選択
#define ANKETO_SND_SELECT		( SEQ_SE_DP_SELECT )	// 選択(すぐ送信にいくので短い音)
//#define ANKETO_SND_BAR		( SEQ_SE_PL_CON_034 )	// バー表示
#define ANKETO_SND_BAR			( SEQ_SE_PL_PINPON2 )	// バー表示


#endif		// __WFLBY_SND_H__


