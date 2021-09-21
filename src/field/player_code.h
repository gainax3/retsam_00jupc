//******************************************************************************
/**
 *
 * @file	player_code.h
 * @brief	自機シンボル参照　※スクリプト参照アリ　故にenum禁止
 * @author	kagaya
 * @data	05.08.03
 *
 */
//******************************************************************************
#ifndef PLAYER_CODE_H_FILE
#define PLAYER_CODE_H_FILE

//==============================================================================
//	define
//==============================================================================
//--------------------------------------------------------------
//	DEBUG
//--------------------------------------------------------------
#define DEBUG_PLAYER_B_BTN_FLIP

//--------------------------------------------------------------
///	自機形態
//--------------------------------------------------------------
#define HERO_FORM_NORMAL 		(0x00)		///<2足
#define HERO_FORM_CYCLE			(0x01)		///<自転車
#define HERO_FORM_SWIM			(0x02)		///<波乗り
#define HERO_FORM_MAX			(0x06)		///<形態最大

//以下描画識別 実際には設定されない
#define HERO_DRAWFORM_SP		(0x10)		///<秘伝技
#define HERO_DRAWFORM_WATER		(0x11)		///<水遣り
#define HERO_DRAWFORM_CONTEST	(0x12)		///<コンテスト
#define HERO_DRAWFORM_FISHING	(0x13)		///<釣り
#define HERO_DRAWFORM_POKETCH	(0x14)		///<ポケッチ
#define HERO_DRAWFORM_SAVE		(0x15)		///<セーブ
#define HERO_DRAWFORM_BANZAI	(0x16)		///<万歳
#define HERO_DRAWFORM_BS		(0x17)		///<バトルサーチャー
#define HERO_DRAWFORM_TW		(0x18)		///<破れた世界
#define HERO_DRAWFORM_TWSWIM	(0x19)		///<破れた世界波乗り
#define HERO_DRAWFORM_TWSAVE	(0x1a)		///<破れた世界セーブ
#define HERO_DRAWFORM_TWPOKETCH	(0x1b)		///<破れた世界ポケッチ
#define HERO_DRAWFORM_GSTGYM			(0x1c)	///<ゴーストジム
#define HERO_DRAWFORM_GSTGYM_SAVE		(0x1d)	///<ゴーストジムセーブ
#define HERO_DRAWFORM_GSTGYM_POKETCH	(0x1e)	///<ゴーストジムポケッチ

//自転車は一種類になりました
#define HERO_FORM_CYCLE_R (HERO_FORM_CYCLE)
#define HERO_FORM_CYCLE_D (HERO_FORM_CYCLE)

//--------------------------------------------------------------
///	リクエスト
//--------------------------------------------------------------
#define HERO_REQBIT_NON			(0)							///<リクエストなし
#define HERO_REQBIT_NORMAL		(1<<0)						///<２足移動に変更
#define HERO_REQBIT_CYCLE		(1<<1)						///<自転車移動に変更
#define HERO_REQBIT_SWIM		(1<<2)						///<波乗り移動に変更
#define HERO_REQBIT_UNDER_OFF	(1<<3)						///<足元判定を無効にする
#define HERO_REQBIT_WATER		(1<<4)						///<水遣り自機に変更する
#define HERO_REQBIT_FISHING		(1<<5)						///<釣り自機へ表示変更
#define HERO_REQBIT_POKETCH		(1<<6)						///<ポケッチ自機へ表示変更
#define HERO_REQBIT_SAVE		(1<<7)						///<セーブ自機へ表示変更
#define HERO_REQBIT_BANZAI		(1<<8)						///<万歳自機へ表示変更
#define HERO_REQBIT_BS			(1<<9)						///<BS自機へ表示変更
#define HERO_REQBIT_MAX			(10)						///<リクエスト総数

#endif //PLAYER_CODE_H_FILE
