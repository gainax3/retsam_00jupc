//============================================================================================
/**
 * @file	plist_obj.h
 * @brief	ポケモンリストOBJ処理
 * @author	Hiroyuki Nakamura
 * @date	05.10.05
 */
//============================================================================================
#ifndef PLIST_OBJ_H
#define PLIST_OBJ_H
#undef GLOBAL
#ifdef PLIST_OBJ_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================
// キャラID
enum {
	PLA_CHAR_ID_BALL = 0,
	PLA_CHAR_ID_CURSOR,
	PLA_CHAR_ID_ENTER,
	PLA_CHAR_ID_STATUS,
	PLA_CHAR_ID_ICON1,
	PLA_CHAR_ID_ICON2,
	PLA_CHAR_ID_ICON3,
	PLA_CHAR_ID_ICON4,
	PLA_CHAR_ID_ICON5,
	PLA_CHAR_ID_ICON6,
	PLA_CHAR_ID_ITEM,
	PLA_CHAR_ID_BUTTON_EF,
};
/*
enum {
	PLA_PLTT_ID_OBJ = 0,
	PLA_PLTT_ID_STATUS,
	PLA_PLTT_ID_ICON,
	PLA_PLTT_ID_ITEM,
};
enum {
	PLA_CELL_ID_BALL = 0,
	PLA_CELL_ID_CURSOR,
	PLA_CELL_ID_ENTER,
	PLA_CELL_ID_STATUS,
	PLA_CELL_ID_ICON1,
	PLA_CELL_ID_ICON2,
	PLA_CELL_ID_ITEM,
};
enum {
	PLA_CANM_ID_BALL = 0,
	PLA_CANM_ID_CURSOR,
	PLA_CANM_ID_STATUS,
	PLA_CANM_ID_ENTER,
	PLA_CANM_ID_ICON1,
	PLA_CANM_ID_ICON2,
	PLA_CANM_ID_ITEM,
};
*/
enum {
	PL_ST_POKERUS = 0,	// ポケルス
	PL_ST_MAHI,			// 麻痺
	PL_ST_KOORI,		// 氷
	PL_ST_NEMURI,		// 眠り
	PL_ST_DOKU,			// 毒
	PL_ST_YAKEDO,		// 火傷
	PL_ST_HINSI,		// 瀕死

	PL_ST_NONE,			// 状態異常なし
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//---------------------------------------------------------------------------------------------
/**
 * セルアクター初期化
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeListCellActorInit( PLIST_WORK * wk );

//---------------------------------------------------------------------------------------------
/**
 * セルアクター追加
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeListCellActSet( PLIST_WORK * wk );

//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコン追加
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	px		X座標
 * @param	py		Y座標
 * @param	p_handleアーカイブハンドル
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeListIconAdd( PLIST_WORK * wk, u8 pos, u16 px, u16 py, ARCHANDLE* p_handle );

//---------------------------------------------------------------------------------------------
/**
 * ボールアクター追加
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	px		X座標
 * @param	py		Y座標
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeListBallActSet( PLIST_WORK * wk, u8 pos, u16 px, u16 py );

//---------------------------------------------------------------------------------------------
/**
 * セルアクター解放
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeListCellActRerease( PLIST_WORK * wk );

//---------------------------------------------------------------------------------------------
/**
 * 状態異常アイコン切り替え
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	st		状態
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_StatusIconChg( PLIST_WORK * wk, u8 pos, u8 st );

//---------------------------------------------------------------------------------------------
/**
 * アイテムアイコン切り替え
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	item	アイテム番号
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_ItemIconChg( PLIST_WORK * wk, u8 pos, u16 item );

//---------------------------------------------------------------------------------------------
/**
 * メールアイコン切り替え
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_MailIconChg( PLIST_WORK * wk, u8 pos );

//---------------------------------------------------------------------------------------------
/**
 * アイテムアイコン位置セット
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	x		X座標
 * @param	y		Y座標
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_ItemIconPosSet( PLIST_WORK * wk, u8 pos, s16 x, s16 y );

//---------------------------------------------------------------------------------------------
/**
 * カスタムボールアイコンセット
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_CustomIconPosSet( PLIST_WORK * wk, u8 pos );

//---------------------------------------------------------------------------------------------
/**
 * カスタムボールアイコン切り替え
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_CustomIconChg( PLIST_WORK * wk, u8 pos );

//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンアニメ
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeIconAnime( PLIST_WORK * wk );

//---------------------------------------------------------------------------------------------
/**
 * 選択カーソルを切り替え
 *
 * @param	wk		ポケモンリストのワーク
 * @param	pos		位置
 * @param	pal		パレット
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void PokeList_SelCursorChgDirect( PLIST_WORK * wk, u8 pos, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 * サブボタンエフェクト
 *
 * @param	wk		ポケモンリストのワーク
 * @param	x		X座標
 * @param	y		Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListObj_SubButtonEffInit( PLIST_WORK * wk, s16 x, s16 y );

//--------------------------------------------------------------------------------------------
/**
 * サブボタンエフェクトメイン
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListObj_SubButtonEffMain( PLIST_WORK * wk );

//--------------------------------------------------------------
/**
 * @brief   ポケモンアイコンを現在のパラメータで変更
 *
 * @param   wk		ポケモンリストのワーク
 * @param   pos		位置
 *
 * フォルムチェンジ用に用意
 */
//--------------------------------------------------------------
GLOBAL void PokeListIconChange( PLIST_WORK *wk, u8 pos );

#undef GLOBAL
#endif	// PLIST_OBJ_H
