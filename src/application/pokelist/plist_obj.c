//============================================================================================
/**
 * @file	plist_obj.c
 * @brief	ポケモンリストOBJ処理
 * @author	Hiroyuki Nakamura
 * @date	05.10.05
 */
//============================================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/clact_tool.h"
#include "system/window.h"
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
#include "system/bmp_menu.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/pokeparty.h"
#include "poketool/poke_regulation.h"
#include "itemtool/item.h"
#include "itemtool/myitem.h"
#include "application/app_tool.h"

#define	PLIST_OBJ_H_GLOBAL
#include "application/pokelist.h"
#include "plist_sys.h"
#include "plist_obj.h"
#include "application/plist_gra.naix"


//============================================================================================
//	定数定義
//============================================================================================
#define	CLA_SUB_SURFACE_OFFSET	( 256 * FX32_ONE )	// サブ画面のサーフェースオフセット


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void PokeListIconCellActMake( PLIST_WORK * wk );
static u8 IconMonsCheck( PL_PANEL_DATA * dat );


//============================================================================================
//	グローバル変数
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
void PokeListCellActorInit( PLIST_WORK * wk )
{
	GF_Disp_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON

	initVramTransferManagerHeap( 32, HEAPID_POKELIST );

	wk->csp = CATS_AllocMemory( HEAPID_POKELIST );
	wk->crp = CATS_ResourceCreate( wk->csp );

	{
		TCATS_OAM_INIT	coi = {
			0, 128,
			0, 32,
			0, 128,
			0, 32,
		};
		TCATS_CHAR_MANAGER_MAKE ccmm = {
			PLIST_ACT_MAX+6,		// ＋ポケモンアイコン
			1024,
			1024,
			GX_OBJVRAMMODE_CHAR_1D_32K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		CATS_SystemInit( wk->csp, &coi, &ccmm, 32 );
		CATS_ClactSetInit( wk->csp, wk->crp, PLIST_ACT_MAX+6 );
	}

	{
		CLACT_U_EASYRENDER_DATA * rd = CATS_EasyRenderGet(wk->csp);
		CLACT_U_SetSubSurfaceMatrix( rd, 0, CLA_SUB_SURFACE_OFFSET );
	}

	{
		TCATS_RESOURCE_FILE_LIST list = {
			"data/plist_chr.resdat",		// キャラクタ
			"data/plist_pal.resdat",		// パレット
			"data/plist_cell.resdat",		// セル
			"data/plist_canm.resdat",		// セルアニメ
			NULL,							// マルチセル
			NULL,							// マルチセルアニメ
			"data/plist_h.cldat"			// ヘッダー
		};
		CATS_ResourceLoadBinary( wk->csp, wk->crp, &list );
	}
}


//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコン追加
 *
 * @param	wk		ポケモンリストワーク
 * @param	pos		何匹目か
 * @param	px		X座標
 * @param	py		Y座標
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
void PokeListIconAdd( PLIST_WORK * wk, u8 pos, u16 px, u16 py, ARCHANDLE* p_handle )
{
	POKEMON_PARAM * pp;
	TCATS_OBJECT_ADD_PARAM	prm;
	u32	egg;

	pp = PokeParty_GetMemberPointer( wk->dat->pp, pos );

	wk->panel[pos].mons_px = px;
	wk->panel[pos].mons_py = py;

	CATS_ChangeResourceCharArcH(
		wk->csp, wk->crp,
		p_handle, PokeIconCgxArcIndexGetByPP(pp), 0, PLA_CHAR_ID_ICON1+pos );

	egg = PokeParaGet( pp, ID_PARA_tamago_flag, NULL );

	prm.no = PLA_CHAR_ID_ICON1+pos;		///< リソース内データの登録番号

	prm.x = px;				///< [ X ] 座標
	prm.y = py;				///< [ Y ] 座標
	prm.z = 0;				///< [ Z ] 座標

	prm.anm = 0;													///< アニメ番号
	prm.pri = 0;													///< 優先順位
	prm.pal = PokeIconPalNumGet(wk->panel[pos].mons,wk->panel[pos].form,egg)+3;	///< パレット番号
	prm.d_area = NNS_G2D_VRAM_TYPE_2DMAIN;							///< 描画エリア

	prm.param1 = 0;
	prm.param2 = 0;
	prm.param3 = 0;
	prm.param4 = 0;

	wk->panel[pos].icon_cwp = CATS_ObjectAdd( wk->csp, wk->crp, &prm );
}

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
void PokeListIconChange( PLIST_WORK *wk, u8 pos )
{
	POKEMON_PARAM * pp;
	ARCHANDLE* p_handle;
	int after_monsno, after_form;
    u32 sp10;
    void * arcData;
    NNSG2dCharacterData * charData;
    BOOL res;

	pp = PokeParty_GetMemberPointer( wk->dat->pp, pos );
	after_monsno = PokeParaGet(pp, ID_PARA_monsno, NULL);
	after_form = PokeParaGet(pp, ID_PARA_form_no, NULL);
	
	p_handle = ArchiveDataHandleOpen( ARC_POKEICON, HEAPID_POKELIST );

    // MatchComment: this function changed quite a bit
    sp10 = NNS_G2dGetImageLocation(CLACT_ImageProxyGet(wk->panel[pos].icon_cwp), NNS_G2D_VRAM_TYPE_2DMAIN);
    arcData = ArcUtil_HDL_Load(p_handle, PokeIconCgxArcIndexGetByPP(pp), FALSE, HEAPID_POKELIST, ALLOC_BOTTOM);
    res = NNS_G2dGetUnpackedCharacterData(arcData, &charData);
    if (res) {
        DC_InvalidateRange(charData->pRawData, charData->szByte);
        GXS_LoadOBJ(charData->pRawData, sp10, charData->szByte);
    }
    sys_FreeMemoryEz(arcData);

	CATS_ObjectPaletteSet(wk->panel[pos].icon_cwp, 
		PokeIconPalNumGet(after_monsno, after_form, 0) + 3);

	ArchiveDataHandleClose( p_handle );	
}






//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// セルアクターパラメータ
static const TCATS_OBJECT_ADD_PARAM	ActAddParam[] =
{
	{	// 選択カーソル
		PLA_CHAR_ID_CURSOR,
		64, 24, 0,
		1, 3, 0, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// 入れ替えカーソル
		PLA_CHAR_ID_CURSOR,
		64, 72, 0,
		2, 2, 0, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// けってい
		PLA_CHAR_ID_ENTER,
		232, 168, 0,
		2, 1, 0, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// もどる
		PLA_CHAR_ID_ENTER,
		232, 184, 0,
		2, 1, 0, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// 状態異常アイコン１
		PLA_CHAR_ID_STATUS,
		36, 44, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// 状態異常アイコン２
		PLA_CHAR_ID_STATUS,
		164, 52, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// 状態異常アイコン３
		PLA_CHAR_ID_STATUS,
		36, 92, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// 状態異常アイコン４
		PLA_CHAR_ID_STATUS,
		164, 100, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// 状態異常アイコン５
		PLA_CHAR_ID_STATUS,
		36, 140, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// 状態異常アイコン６
		PLA_CHAR_ID_STATUS,
		164, 148, 0,
		0, 1, 2, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// アイテムアイコン１
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// アイテムアイコン２
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// アイテムアイコン３
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// アイテムアイコン４
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// アイテムアイコン５
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// アイテムアイコン６
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		0, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// カスタムボールアイコン１
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// カスタムボールアイコン２
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// カスタムボールアイコン３
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// カスタムボールアイコン４
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// カスタムボールアイコン５
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},
	{	// カスタムボールアイコン６
		PLA_CHAR_ID_ITEM,
		164, 148, 0,
		2, 0, 6, NNS_G2D_VRAM_TYPE_2DMAIN,
		0, 0, 0, 0
	},

	{	// ボタンエフェクト
		PLA_CHAR_ID_BUTTON_EF,
		164, 148, 0,
		0, 0, 0, NNS_G2D_VRAM_TYPE_2DSUB,
		0, 0, 0, 0
	}
};



//---------------------------------------------------------------------------------------------
/**
 * セルアクター追加
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
void PokeListCellActSet( PLIST_WORK * wk )
{
	u32	i;

	wk->act_work[PLIST_ACT_CURSOR1] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[0] );
	wk->act_work[PLIST_ACT_CURSOR2] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[1] );
	wk->act_work[PLIST_ACT_ENTER] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[2] );
	wk->act_work[PLIST_ACT_EXIT] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[3] );

	wk->act_work[PLIST_ACT_BUTTON_EF] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[22] );

	for( i=0; i<6; i++ ){
		wk->act_work[PLIST_ACT_ST1+i] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[4+i] );
		wk->panel[i].st_px = ActAddParam[4+i].x;
		wk->panel[i].st_py = ActAddParam[4+i].y;
		CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_ST1+i], 0 );
		wk->act_work[PLIST_ACT_ITEM1+i] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[10+i] );
		wk->act_work[PLIST_ACT_CB1+i] = CATS_ObjectAdd( wk->csp, wk->crp, &ActAddParam[16+i] );
	}
	CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_CURSOR2], 0 );
	CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_BUTTON_EF], 0 );
}

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
void PokeListBallActSet( PLIST_WORK * wk, u8 pos, u16 px, u16 py )
{
	TCATS_OBJECT_ADD_PARAM	prm;

	prm.no = PLA_CHAR_ID_BALL;		///< リソース内データの登録番号

	prm.x = px;				///< [ X ] 座標
	prm.y = py;				///< [ Y ] 座標
	prm.z = 0;				///< [ Z ] 座標

	prm.anm = 0;			///< アニメ番号
	prm.pri = 1;			///< 優先順位
	prm.pal = 0;			///< パレット番号
	prm.d_area = NNS_G2D_VRAM_TYPE_2DMAIN;	///< 描画エリア

	prm.param1 = 0;
	prm.param2 = 0;
	prm.param3 = 0;
	prm.param4 = 0;

	wk->act_work[PLIST_ACT_BALL1+pos] = CATS_ObjectAdd( wk->csp, wk->crp, &prm );
}


//---------------------------------------------------------------------------------------------
/**
 * セルアクター解放
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
void PokeListCellActRerease( PLIST_WORK * wk )
{
	CATS_ResourceDestructor( wk->csp, wk->crp );
	CATS_FreeMemory( wk->csp );
}



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
void PokeList_StatusIconChg( PLIST_WORK * wk, u8 pos, u8 st )
{
	CLACT_WORK_PTR * awk = &wk->act_work[PLIST_ACT_ST1+pos];

	if( st == PL_ST_NONE ){
		CLACT_SetDrawFlag( *awk, 0 );
		return;
	}
	CLACT_AnmChg( *awk, st );
	CLACT_SetDrawFlag( *awk, 1 );
}

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
void PokeList_ItemIconChg( PLIST_WORK * wk, u8 pos, u16 item )
{
	CLACT_WORK_PTR * awk = &wk->act_work[PLIST_ACT_ITEM1+pos];

	if( item == 0 ){
		CLACT_SetDrawFlag( *awk, 0 );
		return;
	}
	if( ItemMailCheck( item ) == TRUE ){
		CLACT_AnmChg( *awk, 1 );
	}else{
		CLACT_AnmChg( *awk, 0 );
	}
	CLACT_SetDrawFlag( *awk, 1 );
}

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
void PokeList_MailIconChg( PLIST_WORK * wk, u8 pos )
{
	CLACT_WORK_PTR * awk = &wk->act_work[PLIST_ACT_ITEM1+pos];

	CLACT_AnmChg( *awk, 1 );
	CLACT_SetDrawFlag( *awk, 1 );
}

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
void PokeList_ItemIconPosSet( PLIST_WORK * wk, u8 pos, s16 x, s16 y )
{
	CLACT_WORK_PTR * awk = &wk->act_work[PLIST_ACT_ITEM1+pos];

	wk->panel[pos].item_px = x+8;
	wk->panel[pos].item_py = y+8;
	CATS_ObjectPosSet( *awk, wk->panel[pos].item_px, wk->panel[pos].item_py );
}

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
void PokeList_CustomIconPosSet( PLIST_WORK * wk, u8 pos )
{
	CATS_ObjectPosSet(
		wk->act_work[PLIST_ACT_CB1+pos], wk->panel[pos].item_px+8, wk->panel[pos].item_py );
}

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
void PokeList_CustomIconChg( PLIST_WORK * wk, u8 pos )
{
	CLACT_WORK_PTR * awk = &wk->act_work[PLIST_ACT_CB1+pos];

	if( wk->panel[pos].cb == 0 ){
		CLACT_SetDrawFlag( *awk, 0 );
		return;
	}
	CLACT_SetDrawFlag( *awk, 1 );
}






//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンアニメ切り替え
 *
 * @param	awk		アクターワーク
 * @param	anm		アニメ番号
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
static void PokeIconAnmChg( CLACT_WORK_PTR awp, u8 anm )
{
	if( CLACT_AnmGet( awp ) == anm ){ return; }
	CLACT_AnmFrameSet( awp, 0 );
	CLACT_AnmChg( awp, anm );
}

//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンアニメチェック
 *
 * @param	dat		パネルデータ
 *
 * @return	アニメ番号
 */
//---------------------------------------------------------------------------------------------
static u8 PokeIconAnmCheck( PL_PANEL_DATA * dat )
{
	if( dat->hp == 0 ){
		OS_Printf( "HP : %d\n", dat->hp );
		return POKEICON_ANM_DEATH;
	}

	if( dat->st != PL_ST_NONE && dat->st != PL_ST_POKERUS && dat->st != PL_ST_HINSI ){
		return POKEICON_ANM_STCHG;
	}

	switch( GetHPGaugeDottoColor( dat->hp, dat->mhp, PL_HP_DOTTO_MAX ) ){
	case HP_DOTTO_MAX:
		return POKEICON_ANM_HPMAX;
	case HP_DOTTO_GREEN:	// 緑
		return POKEICON_ANM_HPGREEN;
	case HP_DOTTO_YELLOW:	// 黄
		return POKEICON_ANM_HPYERROW;
	case HP_DOTTO_RED:		// 赤
		return POKEICON_ANM_HPRED;
	}

	return POKEICON_ANM_DEATH;
}

//---------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンアニメ
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
void PokeIconAnime( PLIST_WORK * wk )
{
	PL_PANEL_DATA * dat;
	u16	i;
	u16	anm;


	for( i=0; i<6; i++ ){
		dat = &wk->panel[i];
		if( dat->flg == 0 ){ continue; }

		if( wk->chg_wk.flg == 1 && ( wk->chg_wk.pos[0] == i || wk->chg_wk.pos[1] == i ) ){
			anm = POKEICON_ANM_DEATH;
		}else{
			anm = PokeIconAnmCheck( dat );
		}

		PokeIconAnmChg( dat->icon_cwp, anm );
		CLACT_AnmFrameChg( dat->icon_cwp, FX32_ONE );

		if( wk->pos == i && anm != POKEICON_ANM_DEATH && anm != POKEICON_ANM_STCHG ){
			if( CLACT_AnmFrameGet( dat->icon_cwp ) == 0 ){
				CATS_ObjectPosSet( dat->icon_cwp, dat->mons_px, dat->mons_py-3 );
			}else{
				CATS_ObjectPosSet( dat->icon_cwp, dat->mons_px, dat->mons_py+1 );
			}
			continue;
		}
		CATS_ObjectPosSet( dat->icon_cwp, dat->mons_px, dat->mons_py );
	}
}

//---------------------------------------------------------------------------------------------
/**
 * アイコンがあるかをチェック
 *
 * @param	dat		パネルデータ
 *
 * @retval	"TRUE = あり"
 * @retval	"FALSE = なし"
 */
//---------------------------------------------------------------------------------------------
static u8 IconMonsCheck( PL_PANEL_DATA * dat )
{
	// ？アイコン
	if( dat->mons == 0 || dat->mons > MONSNO_END ){
		return FALSE;
	}
	return TRUE;
}


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
void PokeList_SelCursorChgDirect( PLIST_WORK * wk, u8 pos, u8 pal )
{
	u8	px, py;

	PointerWkPosGet( &wk->mv_tbl[pos], &px, &py );

	CLACT_AnmChg(
		wk->act_work[PLIST_ACT_CURSOR1], PokeListSelCursorCheck( wk->dat->type, pos ) );
	CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_CURSOR1], 1 );
	CATS_ObjectPosSet( wk->act_work[PLIST_ACT_CURSOR1], px, py );
	CATS_ObjectPaletteSet( wk->act_work[PLIST_ACT_CURSOR1], pal );
}



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
void PokeListObj_SubButtonEffInit( PLIST_WORK * wk, s16 x, s16 y )
{
	VecFx32 vec;
		
	vec.x = x * FX32_ONE;
	vec.y = y * FX32_ONE;
	vec.y +=  CLA_SUB_SURFACE_OFFSET;
	vec.z = 0;
	
	CLACT_SetMatrix( wk->act_work[PLIST_ACT_BUTTON_EF], &vec );	

	CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_BUTTON_EF], 1 );
//	CATS_ObjectPosSet( wk->act_work[PLIST_ACT_BUTTON_EF], x, y );
	CLACT_AnmFrameSet( wk->act_work[PLIST_ACT_BUTTON_EF], 0 );
	CLACT_AnmChg( wk->act_work[PLIST_ACT_BUTTON_EF], 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * サブボタンエフェクトメイン
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PokeListObj_SubButtonEffMain( PLIST_WORK * wk )
{
	if( CLACT_GetDrawFlag( wk->act_work[PLIST_ACT_BUTTON_EF] ) == 1 ){
		CLACT_AnmFrameChg( wk->act_work[PLIST_ACT_BUTTON_EF], FX32_ONE );

		if( CLACT_AnmFrameGet( wk->act_work[PLIST_ACT_BUTTON_EF] ) == 2 ){
			CLACT_SetDrawFlag( wk->act_work[PLIST_ACT_BUTTON_EF], 0 );
		}
	}
}
