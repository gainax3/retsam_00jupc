//============================================================================================
/**
 * @file	window.h
 * @brief	ウィンドウ表示
 * @author	Hiroyuki Nakamura
 * @date	2005.10.13
 */
//============================================================================================
#ifndef WINDOW_H
#define WINDOW_H

#ifndef	__ASM_NO_DEF_
#include "poketool/poke_tool.h"
#endif

#undef GLOBAL
#ifdef WINDOW_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


// 看板
#define BOARD_TYPE_TOWN	(0)	// タウンマップ
#define	BOARD_TYPE_ROAD	(1)	// 標識
#define	BOARD_TYPE_POST	(2)	// 表札
#define	BOARD_TYPE_INFO	(3)	// 掲示板

// ポケモンウィンドウ
#define	POKEWIN_CMD_NONE	( 0 )	// コマンドなし
#define	POKEWIN_CMD_END		( 1 )	// 終了コマンド
#define	POKEWIN_CMD_ANM		( 2 )	// アニメコマンド
#define	POKEWIN_CMD_MOVE	( 3 )	// アニメ中


//アセンブラでincludeされている場合は、下の宣言を無視できるようにifndefで囲んである
#ifndef	__ASM_NO_DEF_

//============================================================================================
//	定数定義
//============================================================================================
#define	MENU_WIN_CGX_SIZ	( 9 )		// メニューウィンドウのキャラ数
#define	TALK_WIN_CGX_SIZ	( 18+12 )		// 会話ウィンドウのキャラ数
#define	BOARD_WIN_CGX_SIZ	( 18+12 )		// 看板ウィンドウのキャラ数
#define	BOARD_MAP_CGX_SIZ	( 24 )		// 看板のマップのキャラ数
// 町看板、道看板の全キャラ数
#define	BOARD_DATA_CGX_SIZ	( BOARD_WIN_CGX_SIZ + BOARD_MAP_CGX_SIZ )

enum {
	WINDOW_TRANS_ON = 0,
	WINDOW_TRANS_OFF
};

// メニューウィンドウのタイプ
enum {
	MENU_TYPE_SYSTEM = 0,	// 選択ウィンドウ（システムウィンドウ）
	MENU_TYPE_FIELD,		// フィールドメニュー
	MENU_TYPE_UG			// 地下メニュー
};




//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * メニューウィンドウのキャラをセット
 *
 * @param	ini			BGLデータ
 * @param	frmnum		BGフレーム
 * @param	cgx			キャラ転送位置
 * @param	win_num		ウィンドウ番号
 * @param	heap		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void MenuWinCgxSet( GF_BGL_INI * ini, u8 frmnum, u16 cgx, u8 win_num, u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * メニューウィンドウパレットのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 MenuWinPalArcGet(void);

//--------------------------------------------------------------------------------------------
/**
 * メニューウィンドウのグラフィックをセット
 *
 * @param	ini			BGLデータ
 * @param	frmnum		BGフレーム
 * @param	cgx			キャラ転送位置
 * @param	pal			パレット番号
 * @param	win_num		ウィンドウ番号
 * @param	heap		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void MenuWinGraphicSet(
				GF_BGL_INI * ini, u8 frmnum, u16 cgx, u8 pal, u8 win_num, u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * メニューウィンドウを描画
 *
 * @param	win			BMPウィンドウデータ
 * @param	trans_sw	転送スイッチ
 * @param	win_cgx		ウィンドウキャラ位置
 * @param	pal			パレット
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpMenuWinWrite( GF_BGL_BMPWIN * win, u8 trans_sw, u16 win_cgx, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 *	メニューウィンドウをクリア
 *
 * @param	win			BMPウィンドウデータ
 * @param	trans_sw	転送スイッチ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpMenuWinClear( GF_BGL_BMPWIN * win, u8 trans_sw );

//--------------------------------------------------------------------------------------------
/**
 *	会話ウインドウキャラのアーカイブインデックス取得
 *
 * @param	id		ウィンドウID
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 TalkWinCgxArcGet( u32 id );

//--------------------------------------------------------------------------------------------
/**
 *	会話ウインドウパレットのアーカイブインデックス取得
 *
 * @param	id		ウィンドウID
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 TalkWinPalArcGet( u32 id );

//--------------------------------------------------------------------------------------------
/**
 *	会話ウインドウセット
 *
 * @param	ini			BGLデータ
 * @param	frm			BGフレームナンバー(bg_sys)
 * @param	cgx			キャラセット位置
 * @param	pal			使用パレット
 * @param	win_num		ウィンドウ番号
 * @param	heap		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void TalkWinGraphicSet(
				GF_BGL_INI * ini, u8 frmnum, u16 cgx, u8 pal, u8 win_num, u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * 会話ウィンドウのカラー０のドットを指定カラーに変更する
 *
 * @param	ini			BGLデータ
 * @param	frmnum		BGフレームナンバー(bg_sys)
 * @param	cgx			キャラセット位置
 * @param	cal			変更カラー
 * @param	heap		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void TalkWinGraphicNullSet(
				GF_BGL_INI * ini, u8 frmnum, u16 cgx, u8 col, u8 win_num, u32 heap );


//--------------------------------------------------------------------------------------------
/**
 * 会話ウィンドウスクリーンデータを、BitmapWindowに関連付けられた
 * BGフレームのスクリーンバッファにセットする
 *
 * @param   win			[in]BitmapWindow
 * @param   charno		cgxデータがセットされている先頭キャラナンバ
 * @param   palno		スクリーン作成時に使用するパレットナンバ
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpTalkWinScreenSet( GF_BGL_BMPWIN * win, u32 charno, u32 palno );


//--------------------------------------------------------------------------------------------
/**
 * 会話ウィンドウを描画
 *
 * @param	win			BMPウィンドウデータ
 * @param	trans_sw	転送スイッチ
 * @param	win_cgx		ウィンドウキャラ位置
 * @param	pal			パレット
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpTalkWinWrite( GF_BGL_BMPWIN * win, u8 trans_sw, u16 win_cgx, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 *	会話ウィンドウをクリア
 *
 * @param	win			BMPウィンドウデータ
 * @param	trans_sw	転送スイッチ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpTalkWinClear( GF_BGL_BMPWIN * win, u8 trans_sw );



//--------------------------------------------------------------------------------------------
/**
 * 待機アイコン表示タスク追加
 *
 * @param	win		会話ウィンドウのBMP
 * @param	cgxpos	会話ウィンドウ枠キャラ（TalkWinGraphicSetなど）の転送位置
 *
 * @return	待機アイコンのワーク
 */
//--------------------------------------------------------------------------------------------
GLOBAL void * TimeWaitIconAdd( GF_BGL_BMPWIN * win, u32 cgxpos );

//--------------------------------------------------------------------------------------------
/**
 * 待機アイコン表示タスク削除リクエスト
 *
 * @param	work	待機アイコンのワーク
 *
 * @return	none
 *
 *	workも削除されます
 */
//--------------------------------------------------------------------------------------------
GLOBAL void TimeWaitIconDel( void * work );

//--------------------------------------------------------------------------------------------
/**
 * 待機アイコン表示タスク削除リクエスト
 *
 * @param	work	待機アイコンのワーク
 *
 * @return	none
 *
 *	workも削除されます  SCRは書き換えません
 */
//--------------------------------------------------------------------------------------------
GLOBAL void TimeWaitIconTaskDel( void * work );


//--------------------------------------------------------------------------------------------
/**
 * 看板ウインドウセット
 *
 * @param	ini			BGLデータ
 * @param	frm			BGフレームナンバー(bg_sys)
 * @param	cgx			キャラセット位置
 * @param	pal			使用パレット
 * @param	type		看板タイプ
 * @param	map			マップ番号（タウンマップ、標識）
 * @param	heap		ヒープID
 *
 * @return	none
 *
 *	type = *_POST, *_INFO の場合は、map = 0 で良い
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BoardWinGraphicSet(
				GF_BGL_INI * ini, u8 frmnum, u16 cgx, u8 pal, u8 type, u16 map, u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * 看板ウィンドウを描画
 *
 * @param	win			BMPウィンドウデータ
 * @param	trans_sw	転送スイッチ
 * @param	win_cgx		ウィンドウキャラ位置
 * @param	pal			パレット
 * @param	type		看板タイプ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpBoardWinWrite( GF_BGL_BMPWIN * win, u8 trans_sw, u16 win_cgx, u8 pal, u8 type );

//--------------------------------------------------------------------------------------------
/**
 *	看板ウィンドウをクリア
 *
 * @param	win			BMPウィンドウデータ
 * @param	type		看板タイプ
 * @param	trans_sw	転送スイッチ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BmpBoardWinClear( GF_BGL_BMPWIN * win, u8 type, u8 trans_sw );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンウィンドウ表示（番号、性別指定）
 *
 * @param	bgl		BGLデータ
 * @param	frm		表示BGフレーム
 * @param	px		表示X座標
 * @param	py		表示Y座標
 * @param	pal		ウィンドウパレット
 * @param	cgx		ウィンドウキャラ
 * @param	mons	ポケモン番号
 * @param	sex		ポケモンの性別
 * @param	heap	ヒープID
 *
 * @return	コマンドのポインタ
 *
 *	表示座標は、枠を含めない値を指定すること
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 * PokeWindowPut(
				GF_BGL_INI * bgl, u8 frm, u8 px, u8 py,
				u8 pal, u16 cgx, u16 mons, u8 sex, int heap );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンウィンドウ表示（POKEMON_PARAM指定）
 *
 * @param	bgl		BGLデータ
 * @param	frm		表示BGフレーム
 * @param	px		表示X座標
 * @param	py		表示Y座標
 * @param	pal		ウィンドウパレット
 * @param	cgx		ウィンドウキャラ
 * @param	pp		POKEMON_PARAM
 * @param	heap	ヒープID
 *
 * @return	コマンドのポインタ
 *
 *	表示座標は、枠を含めない値を指定すること
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 * PokeWindowPutPP(
				GF_BGL_INI * bgl, u8 frm, u8 px, u8 py,
				u8 pal, u16 cgx, POKEMON_PARAM * pp, int heap );


#endif	__ASM_NO_DEF_

#undef GLOBAL
#endif	// WINDOW_H
