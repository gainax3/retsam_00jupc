//==============================================================================
/**
 * @file	br_tool.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2008.03.04(火)
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================
#include "common.h"
#include "system/procsys.h"
#include "system/arc_tool.h"
#include "system/arc_util.h"
#include "system/pm_str.h"
#include "system/clact_tool.h"
#include "system/msgdata_util.h"
#include "system/wordset.h"
#include "msgdata/msg.naix"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"

#include "br_private.h"

#include "field/situation_local.h"
#include "savedata/misc.h"

#include "gflib/strcode.h"
#include "gflib\msg_print.h"
#include "system\fontproc.h"
#include "system\msgdata.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_battle_rec.h"


// =============================================================================
//
//
//	■デバッグ用
//
//
// =============================================================================
#ifdef PM_DEBUG
#ifdef BR_AUTO_SERCH
	int g_debug_auto = 0;
	int	g_debug_wait = 0;
	u64 g_debug_count = 0;
#endif
#endif


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
static void ErrorNameSet(STRBUF *dest_str, int heap_id);


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	heapID	
 *
 * @retval	WORDSET*	
 *
 */
//--------------------------------------------------------------
WORDSET* BR_WORDSET_Create( u32 heapID )
{
	//WORDSET_DEFAULT_SETNUM = 8
	WORDSET* wset = WORDSET_CreateEx( 4, WORDSET_COUNTRY_BUFLEN, heapID );	///< 最大で3ワードだったから4で。
	
	return wset;
}

// =============================================================================
//
//
//	■カラー変更
//
//
// =============================================================================
int BR_FadeColorCode_Get( BR_WORK* wk )
{
	u32 bg_color_table[] = {
		LOGO_COLOR_B,
		LOGO_COLOR_B2,
		LOGO_COLOR_B3,
		LOGO_COLOR_B4,
		LOGO_COLOR_B5,
		LOGO_COLOR_B6,
		LOGO_COLOR_B7,
//		LOGO_COLOR_B8,
//		LOGO_COLOR_B9,
	};
	
	return 	bg_color_table[ wk->sys.color_type ];
}

int BR_FadeColorCode_Get_BootLine( BR_WORK* wk )
{
	u32 bg_color_table[] = {
		LINE_COLOR_B,
		LINE_COLOR_B2,
		LINE_COLOR_B3,
		LINE_COLOR_B4,
		LINE_COLOR_B5,
		LINE_COLOR_B6,
		LINE_COLOR_B7,
//		LINE_COLOR_B8,
//		LINE_COLOR_B9,
	};
	
	if ( wk->mode != BR_MODE_BROWSE ){
		return LINE_COLOR_G;
	}
	return 	bg_color_table[ wk->sys.color_type ];
}

//Recoder_ColorChange
/*	
	GET_PAL_FONT = 0,
	GET_PAL_PHOTO,
	GET_PAL_OBJ,
	GET_PAL_BG,	
*/
int BR_ColorPaletteID_Get( BR_WORK* wk, int type )
{
	int font[] = {
		NARC_batt_rec_gra_batt_rec_font_NCLR,
		NARC_batt_rec_gra_batt_rec_font2_NCLR,
		NARC_batt_rec_gra_batt_rec_font3_NCLR,
		NARC_batt_rec_gra_batt_rec_font4_NCLR,
		NARC_batt_rec_gra_batt_rec_font5_NCLR,
		NARC_batt_rec_gra_batt_rec_font6_NCLR,
		NARC_batt_rec_gra_batt_rec_font7_NCLR,
//		NARC_batt_rec_gra_batt_rec_font8_NCLR,
//		NARC_batt_rec_gra_batt_rec_font9_NCLR,
	};
	int photo[] = {
		NARC_batt_rec_gra_batt_rec_photo_NCLR,
		NARC_batt_rec_gra_batt_rec_photo2_NCLR,
		NARC_batt_rec_gra_batt_rec_photo3_NCLR,
		NARC_batt_rec_gra_batt_rec_photo4_NCLR,
		NARC_batt_rec_gra_batt_rec_photo5_NCLR,
		NARC_batt_rec_gra_batt_rec_photo6_NCLR,
		NARC_batt_rec_gra_batt_rec_photo7_NCLR,
//		NARC_batt_rec_gra_batt_rec_photo8_NCLR,
//		NARC_batt_rec_gra_batt_rec_photo9_NCLR,
	};
	int obj[] = {
		NARC_batt_rec_gra_batt_rec_browse_obj_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj2_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj3_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj4_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj5_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj6_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_obj7_NCLR,
//		NARC_batt_rec_gra_batt_rec_browse_obj8_NCLR,
//		NARC_batt_rec_gra_batt_rec_browse_obj9_NCLR,
	};
	int bg[] = {
		NARC_batt_rec_gra_batt_rec_browse_bg_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg2_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg3_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg4_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg5_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg6_NCLR,
		NARC_batt_rec_gra_batt_rec_browse_bg7_NCLR,
//		NARC_batt_rec_gra_batt_rec_browse_bg8_NCLR,
//		NARC_batt_rec_gra_batt_rec_browse_bg9_NCLR,
	};
	int no;
	
	no = wk->sys.color_type;
	
	switch ( type ){
	case 0: return font[ no ];
	case 1: return photo[ no ];
	case 2: return obj[ no ];
	case 3:	return bg[ no ];
	}
	
	GF_ASSERT( 0 );
	
	return font[ 0 ];
}

//--------------------------------------------------------------
/**
 * @brief	カラータイプ設定
 *
 * @param	wk	
 * @param	type	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ColorPaletteChange( BR_WORK* wk, int type )
{
	PALETTE_FADE_PTR pfd = wk->sys.pfd;
	CATS_SYS_PTR csp = wk->sys.csp;
	CATS_RES_PTR crp = wk->sys.crp;
	ARCHANDLE*	 hdl = wk->sys.p_handle;
	
	PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_BG ),	HEAPID_BR, FADE_MAIN_BG, 0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );	
	PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_BG ),	HEAPID_BR, FADE_SUB_BG,	 0x20 * BG_PAL_BASE_END, eBG_PAL_BASE_0 );	
	PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_FONT ), HEAPID_BR, FADE_MAIN_BG, 0x20, eBG_PAL_FONT*16 );	
	PaletteWorkSet_Arc( pfd, ARC_BR_GRA, BR_ColorPaletteID_Get( wk, GET_PAL_FONT ), HEAPID_BR, FADE_SUB_BG,	 0x20, eBG_PAL_FONT*16 );
	
	Cursor_ColorTypeChange( wk->cur_wk, wk->sys.color_type );
	Cursor_ColorTypeChange( wk->cur_wk, wk->sys.color_type );

	CATS_FreeResourcePltt( crp, eID_OAM_SIDE_BAR_M );
	CATS_FreeResourcePltt( crp, eID_OAM_SIDE_BAR_S );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_MAIN_OBJ, csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_SIDE_BAR_M );
	CATS_LoadResourcePlttWorkArcH( pfd, FADE_SUB_OBJ,  csp, crp, hdl, BR_ColorPaletteID_Get( wk, GET_PAL_OBJ ), FALSE, OAM_TAG_PAL_MAX, NNS_G2D_VRAM_TYPE_2DSUB,  eID_OAM_SIDE_BAR_S );
}

// ----------------------------------------------------------------------------
//
//
//	■ソフトフェード関数郡
//
//
// ----------------------------------------------------------------------------
// -----------------------------------------
//
//	■ ソフトフェード操作構造体
//
// -----------------------------------------
typedef struct _FADE_SYS {
	
	BOOL	active;				///< TRUE = 動作中
	TCB_PTR	tcb;
	
	FADEREQ req;
	u16		start_pos;
	u16		col_num;
	u16		next_rgb;
	u8		start_evy;
	u8		end_evy;
	u8		evy;
	s8		param;
	s8		wait;
	s8		timer;
	
	PALETTE_FADE_PTR	pfd;	///< pfd
	
} FADE_SYS;

static void PalGrayScale(u16 *pal, u16 pal_size);

static void PalGrayScale(u16 *pal, u16 pal_size)
{
	int i, r, g, b;
	u32 c;

	for(i = 0; i < pal_size; i++){
		r = ((*pal) & 0x1f);
		g = (((*pal) >> 5) & 0x1f);
		b = (((*pal) >> 10) & 0x1f);

		c = RGBtoY(r,g,b);

		*pal = (u16)((c<<10)|(c<<5)|c);
		pal++;
	}
}

//--------------------------------------------------------------
/**
 * @brief	フェードのかかっていない状態に
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void PaletteFadeClear( BR_WORK* wk )
{
	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, 0, wk->sys.logo_color );	///< main	oam
	ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, 0, wk->sys.logo_color );	///< main	bg
	ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, 0, wk->sys.logo_color );	///< sub	oam
	ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 0xFFFF, 0, wk->sys.logo_color );	///< sub	bg
}


//--------------------------------------------------------------
/**
 * @brief	50音にソートされたデータ取得
 *
 * @param	heap	
 * @param	idx	
 * @param	p_arry_num	
 *
 * @retval	u16*	
 *
 */
//--------------------------------------------------------------
u16* ZukanSortDataGet_50on( int heap, int idx, int* p_arry_num )
{
	u32 size;
	u16* p_buf;
	
	// 読み込み
	p_buf = ArcUtil_LoadEx( ARC_ZUKAN_DATA, NARC_zukan_data_zkn_sort_aiueo_dat, FALSE, heap, ALLOC_TOP, &size );

	*p_arry_num = size / sizeof( u16 );

	return p_buf;
}


// =============================================================================
//
//
//	■拡張パラメータ操作
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	フラグを立てる
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void ExParam2_Set( BR_WORK* wk, BOOL flag )
{
	wk->ex_param2 = flag;
}


//--------------------------------------------------------------
/**
 * @brief	フラグを取得
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL ExParam2_Get( BR_WORK* wk )
{
	return wk->ex_param2;
}


// =============================================================================
//
//
//	■割り込みようメッセージ　送信できないとか
//
//
// =============================================================================
void Tool_InfoMessage( BR_WORK* wk, int no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	
	if ( wk->bInfo == TRUE ){ return; }

	wk->bInfo = TRUE;

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_S, FALSE );
	GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	G2S_BlendNone();
	GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 1, 19, 30, 4, eBG_PAL_FONT, 32 );

	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

void Tool_InfoMessageSet( BR_WORK* wk, int no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	
	if ( wk->bInfo == FALSE ){ return; }

	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );	
}

void Tool_InfoMessageDel( BR_WORK* wk )
{
	if ( wk->bInfo == FALSE ){ return; }
	wk->bInfo = FALSE;
	
	GF_BGL_BmpWinOff( &wk->info_win );
	GF_BGL_BmpWinDel( &wk->info_win );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
	
	
	GF_BGL_PrioritySet( GF_BGL_FRAME2_S, eBR_BG2_PRI );
	
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
					  GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,	7, 8 );
		
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2,
					   GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ,  7, 8 );
}

void InfoMessageDel( BR_WORK* wk )
{	
	GF_BGL_BmpWinOff( &wk->info_win );
	GF_BGL_BmpWinDel( &wk->info_win );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
}

void Tool_InfoMessageMain( BR_WORK* wk, int no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	
	if ( wk->bInfo == TRUE ){ return; }

	wk->bInfo = TRUE;

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_M, FALSE );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 1, 19, 30, 4, eBG_PAL_FONT, 32 );

	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

void Tool_InfoMessageMainSet( BR_WORK* wk, int no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	
	if ( wk->bInfo == FALSE ){ return; }

	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );	
}

void Tool_InfoMessageMainDel( BR_WORK* wk )
{
	if ( wk->bInfo == FALSE ){ return; }
	
	wk->bInfo = FALSE;
	
	GF_BGL_BmpWinOff( &wk->info_win );
	GF_BGL_BmpWinDel( &wk->info_win );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
}

void Tool_WiFiEnd_Message( BR_WORK* wk, int no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	
	if ( wk->bInfo == TRUE ){ return; }
	if ( wk->bWiFiEnd == TRUE ){ return; }

	wk->bInfo = TRUE;
	wk->bWiFiEnd = TRUE;

	G2S_BlendNone();
	GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_S, FALSE );
	GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 1, 19, 30, 4, eBG_PAL_FONT, 32 );

	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

// =============================================================================
//
//
//	■　GPP_WORK からプロフィール画面を一括して作る (スクリーンは自前で)
//
//
// =============================================================================
static void BR_prof_WinAdd_Top( GPP_WORK* gwk, BR_WORK* wk );
static void BR_prof_WinDel_Top( GPP_WORK* gwk, BR_WORK* wk );
static void PokeIcon_Visible( GPP_WORK* gwk, int flag );
static void PokeIcon_Del( GPP_WORK* gwk, BR_WORK* wk );
static void PokeIcon_Add( GPP_WORK* gwk, BR_WORK* wk );
static void HeroIcon_Visible( GPP_WORK* gwk, int flag );
static void HeroIcon_Del( GPP_WORK* gwk, BR_WORK* wk );
static void HeroIcon_Add( GPP_WORK* gwk, BR_WORK* wk );

#ifdef NONEQUIVALENT
static void BR_prof_WinAdd_Top( GPP_WORK* gwk, BR_WORK* wk )
{
	int i;
	int ofs;
	GF_BGL_BMPWIN* win;
	STRBUF*	str;

	{
		s16 win_p[][4] = {
			{  4, 4, 24, 2 },	///< 見出し
			{ 15, 8,  6, 2 },	///< ねんれい
			{ 16, 8, 12, 2 },	///< ｘｘさい
			{  4,11, 11, 2 },	///< すんでいる～
			{  4,13, 24, 2 },	///< 国・地域
			{  4,15, 24, 2 },	///< 地域
			{  4,17, 11, 2 },	///< 自己紹介
			{  4,19, 24, 4 },	///< 簡易文
			{ 0xFF,0,0,0 },
			{  4,21, 24, 2 },	///< データナンバー			
		};
		int mes_id[] = {
			msg_20, msg_20, msg_20,	msg_20,
			msg_20,	msg_20,	msg_20, msg_20,
			msg_20,	msg_20,	msg_20, msg_20,
		};
		
		ofs = 1;
		for ( i = 0; i < 8; i++ ){
						
			win = &gwk->win[ i ];				
			str = MSGMAN_AllocString( wk->sys.man, mes_id[ i ] );				
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M,
							  win_p[i][0], win_p[i][1], win_p[i][2], win_p[i][3], eBG_PAL_FONT, ofs );				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, str, BR_print_x_Get( win, str ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			ofs += ( win_p[i][2] * win_p[i][3] );
			STRBUF_Delete( str );
		}
//		OS_Printf( "win add char end %d\n", ofs );
	}
	
	///< 文字描画
	{
		STRBUF* 	str1;
		STRBUF* 	str2;
		STRBUF*		tmp;
		WORDSET*	wset;
		GF_BGL_BMPWIN* win;
		GDS_PROFILE_PTR	gpp;
		
		gpp = gwk->gpp;
		wset = BR_WORDSET_Create( HEAPID_BR );
		
		///< 見出し
		win = &gwk->win[ 0 ];
		str1 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );			///< 名前
		BR_ErrorStrChange( wk, str1 );
		str2 = MSGMAN_AllocString( wk->sys.man, msg_13 );			///< ～ のプロフィール
		tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ

		WORDSET_RegisterWord( wset, 0, str1, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, tmp, str2 );
	
		GF_BGL_BmpWinDataFill( win, 0x00 );
		GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
		GF_BGL_BmpWinOnVReq( win );

		STRBUF_Delete( str1 );
		STRBUF_Delete( str2 );
		STRBUF_Delete( tmp );
		WORDSET_ClearAllBuffer( wset );
		
		///< 年齢
		{
			int age = GDS_Profile_GetMonthBirthday( gpp );
		#if 0	
			win = &gwk->win[ 1 ];
			tmp = MSGMAN_AllocString( wk->sys.man, msg_14 );				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			STRBUF_Delete( tmp );
		#endif	
			win  = &gwk->win[ 2 ];			
			str1 = MSGMAN_AllocString( wk->sys.man, msg_15 );
			str2 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );
			BR_ErrorStrChange( wk, str2 );
			tmp  = STRBUF_Create( 255, HEAPID_BR );	
			
			STRBUF_SetNumber( str2, age, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			
			WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
			WORDSET_ExpandStr( wset, tmp, str1 );
				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			
			STRBUF_Delete( str1 );
			STRBUF_Delete( str2 );
			STRBUF_Delete( tmp );
			WORDSET_ClearAllBuffer( wset );
		}
		
		///< すんでいるばしょ
		{
			win = &gwk->win[ 3 ];
			tmp = MSGMAN_AllocString( wk->sys.man, msg_16 );				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			STRBUF_Delete( tmp );
			
			{		
				int country = GDS_Profile_GetNation( gpp );
				int area	= GDS_Profile_GetArea( gpp );
				
//				OS_Printf( " country = %d area = %d\n", country, area );
				
				win = &gwk->win[ 4 ];							
				GF_BGL_BmpWinDataFill( win, 0x00 );
				
				if ( country == 0 ){
					///< 国コード未設定
					tmp = MSGMAN_AllocString( wk->sys.man, msg_21 );
				//	GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
					GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
					GF_BGL_BmpWinOnVReq( win );				
					STRBUF_Delete( tmp );
				}
				else {
					///< 国名表示
					tmp  = STRBUF_Create( 255, HEAPID_BR );
					str1 = MSGMAN_AllocString( wk->sys.man, msg_22 );
					WORDSET_RegisterCountryName( wset, 0, country);
					WORDSET_ExpandStr( wset, tmp, str1 );
				//	GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
					GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
					GF_BGL_BmpWinOnVReq( win );
					STRBUF_Delete( tmp );
					STRBUF_Delete( str1 );
					
					if ( area != 0 ){
						///< 地域名表示
						win = &gwk->win[ 5 ];							
						GF_BGL_BmpWinDataFill( win, 0x00 );
						tmp	 = STRBUF_Create( 255, HEAPID_BR );
						str1 = MSGMAN_AllocString( wk->sys.man, msg_23 );
						WORDSET_RegisterLocalPlaceName( wset, 0, country, area );
						WORDSET_ExpandStr( wset, tmp, str1 );
				//		GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
						GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 4, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
						GF_BGL_BmpWinOnVReq( win );
						STRBUF_Delete( tmp );
						STRBUF_Delete( str1 );
					}
				}
				
				WORDSET_ClearAllBuffer( wset );
			}
		}
		
		///< じこしょうかい
		{
			win = &gwk->win[ 6 ];
			tmp = MSGMAN_AllocString( wk->sys.man, msg_17 );				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
			GF_BGL_BmpWinOnVReq( win );
			STRBUF_Delete( tmp );
			
			{
				PMS_DATA	dest;
								
				win = &gwk->win[ 7 ];
				
				tmp = GDS_Profile_GetSelfIntroduction( gpp, &dest, HEAPID_BR );
				
				if ( tmp == NULL ){
					tmp = PMSDAT_ToString( &dest, HEAPID_BR );
				}
				
				GF_BGL_BmpWinDataFill( win, 0x00 );
				GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );
				GF_BGL_BmpWinOnVReq( win );
				
				STRBUF_Delete( tmp );				
			}
		}
		
		WORDSET_Delete( wset );
	}
}
#else

const s16 win_p[][4] = {
    {  4, 4, 24, 2 },	///< 見出し
    { 15, 8,  6, 2 },	///< ねんれい
    { 16, 8, 12, 2 },	///< ｘｘさい
    {  4,11, 11, 2 },	///< すんでいる～
    {  4,13, 24, 2 },	///< 国・地域
    {  4,15, 24, 2 },	///< 地域
    {  4,17, 11, 2 },	///< 自己紹介
    {  4,19, 24, 4 },	///< 簡易文
    { 0xFF,0,0,0 },
    {  4,21, 24, 2 },	///< データナンバー			
};
const int mes_id[] = {
    msg_20, msg_20, msg_20,	msg_20,
    msg_20,	msg_20,	msg_20, msg_20,
    msg_20,	msg_20,	msg_20, msg_20,
};

asm static void BR_prof_WinAdd_Top( GPP_WORK* gwk, BR_WORK* wk )
{
    push {r3, r4, r5, r6, r7, lr}
	sub sp, #0xd8
	ldr r3, =win_p // _02232004
	add r7, r1, #0
	str r0, [sp, #0x14]
	add r2, sp, #0x88
	mov r1, #0x28
_02231C86:
	ldrh r0, [r3]
	add r3, r3, #2
	strh r0, [r2]
	add r2, r2, #2
	sub r1, r1, #1
	bne _02231C86
	ldr r4, =mes_id // _02232008
	add r3, sp, #0x58
	mov r2, #6
_02231C98:
	ldmia r4!, {r0, r1}
	stmia r3!, {r0, r1}
	sub r2, r2, #1
	bne _02231C98
	mov r0, #1
	str r0, [sp, #0x24]
	mov r0, #0
	ldr r4, [sp, #0x14]
	str r0, [sp, #0x28]
	add r0, sp, #0x58
	add r4, #0xc
	str r0, [sp, #0x1c]
	add r5, sp, #0x88
_02231CB2:
	ldr r1, [sp, #0x1c]
	ldr r0, [r7, #0x48]
	ldr r1, [r1, #0]
	bl MSGMAN_AllocString
	add r6, r0, #0
	add r0, r4, #0
	bl GF_BGL_BmpWinInit
	mov r0, #2
	ldrsh r0, [r5, r0]
	mov r3, #0
	add r1, r4, #0
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp]
	mov r0, #4
	ldrsh r0, [r5, r0]
	mov r2, #2
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp, #4]
	mov r0, #6
	ldrsh r0, [r5, r0]
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp, #8]
	mov r0, #0xe
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x24]
	lsl r0, r0, #0x10
	lsr r0, r0, #0x10
	str r0, [sp, #0x10]
	ldrsh r3, [r5, r3]
	ldr r0, [r7, #0x24]
	lsl r3, r3, #0x18
	lsr r3, r3, #0x18
	bl GF_BGL_BmpWinAdd
	add r0, r4, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r4, #0
	add r1, r6, #0
	bl BR_print_x_Get
	add r3, r0, #0
	mov r0, #0
	str r0, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	add r0, r4, #0
	add r2, r6, #0
	bl GF_STR_PrintColor
	add r0, r4, #0
	bl GF_BGL_BmpWinOnVReq
	mov r0, #4
	ldrsh r1, [r5, r0]
	mov r0, #6
	ldrsh r0, [r5, r0]
	add r2, r1, #0
	mul r2, r0
	ldr r0, [sp, #0x24]
	add r0, r0, r2
	str r0, [sp, #0x24]
	add r0, r6, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x1c]
	add r4, #0x10
	add r0, r0, #4
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x28]
	add r5, #8
	add r0, r0, #1
	str r0, [sp, #0x28]
	cmp r0, #8
	blt _02231CB2
	ldr r0, [sp, #0x14]
	ldr r0, [r0, #0]
	str r0, [sp, #0x20]
	mov r0, #0x66
	bl BR_WORDSET_Create
	add r4, r0, #0
	ldr r5, [sp, #0x14]
	ldr r0, [sp, #0x20]
	mov r1, #0x66
	add r5, #0xc
	bl GDS_Profile_CreateNameString
	str r0, [sp, #0x2c]
	ldr r1, [sp, #0x2c]
	add r0, r7, #0
	bl BR_ErrorStrChange
	ldr r0, [r7, #0x48]
	mov r1, #0xd
	bl MSGMAN_AllocString
	str r0, [sp, #0x30]
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	add r6, r0, #0
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	mov r1, #0
	str r0, [sp, #4]
	ldr r2, [sp, #0x2c]
	add r0, r4, #0
	add r3, r1, #0
	bl WORDSET_RegisterWord
	ldr r2, [sp, #0x30]
	add r0, r4, #0
	add r1, r6, #0
	bl WORDSET_ExpandStr
	add r0, r5, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r1, r6, #0
	bl BR_print_x_Get
	mov r1, #0
	add r3, r0, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	add r2, r6, #0
	str r0, [sp, #8]
	add r0, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	ldr r0, [sp, #0x2c]
	bl STRBUF_Delete
	ldr r0, [sp, #0x30]
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add r0, r4, #0
	bl WORDSET_ClearAllBuffer
	ldr r0, [sp, #0x20]
	bl GDS_Profile_GetMonthBirthday
	str r0, [sp, #0x34]
	ldr r5, [sp, #0x14]
	ldr r0, [r7, #0x48]
	mov r1, #0xf
	add r5, #0x2c
	bl MSGMAN_AllocString
	str r0, [sp, #0x38]
	ldr r0, [sp, #0x20]
	mov r1, #0x66
	bl GDS_Profile_CreateNameString
	str r0, [sp, #0x3c]
	ldr r1, [sp, #0x3c]
	add r0, r7, #0
	bl BR_ErrorStrChange
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	add r6, r0, #0
	ldr r2, [sp, #0x34]
	add r0, r4, #0
	mov r1, #0
	bl WORDSET_RegisterMonthName
	ldr r2, [sp, #0x38]
	add r0, r4, #0
	add r1, r6, #0
	bl WORDSET_ExpandStr
	add r0, r5, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r1, r6, #0
	bl BR_print_x_Get
	mov r1, #0
	add r3, r0, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	add r2, r6, #0
	str r0, [sp, #8]
	add r0, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	ldr r0, [sp, #0x38]
	bl STRBUF_Delete
	ldr r0, [sp, #0x3c]
	bl STRBUF_Delete
	add r0, r6, #0
	bl STRBUF_Delete
	add r0, r4, #0
	bl WORDSET_ClearAllBuffer
	ldr r5, [sp, #0x14]
	ldr r0, [r7, #0x48]
	add r5, #0x3c
	mov r1, #0x10
	bl MSGMAN_AllocString
	add r6, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r1, r6, #0
	bl BR_print_x_Get
	mov r1, #0
	add r3, r0, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	add r2, r6, #0
	str r0, [sp, #8]
	add r0, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	add r0, r6, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x20]
	bl GDS_Profile_GetNation
	add r6, r0, #0
	ldr r0, [sp, #0x20]
	bl GDS_Profile_GetArea
	ldr r5, [sp, #0x14]
	str r0, [sp, #0x40]
	add r5, #0x4c
	add r0, r5, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	cmp r6, #0
	bne _02231F18
	ldr r0, [r7, #0x48]
	mov r1, #0x15
	bl MSGMAN_AllocString
	mov r1, #0
	add r6, r0, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	add r2, r6, #0
	str r0, [sp, #8]
	add r0, r5, #0
	add r3, r1, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	add r0, r6, #0
	bl STRBUF_Delete
	b _02231FD2
_02231F18:
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	str r0, [sp, #0x44]
	ldr r0, [r7, #0x48]
	mov r1, #0x16
	bl MSGMAN_AllocString
	str r0, [sp, #0x48]
	add r0, r4, #0
	mov r1, #0
	add r2, r6, #0
	bl WORDSET_RegisterCountryName
	ldr r1, [sp, #0x44]
	ldr r2, [sp, #0x48]
	add r0, r4, #0
	bl WORDSET_ExpandStr
	mov r1, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	ldr r2, [sp, #0x44]
	str r0, [sp, #8]
	add r0, r5, #0
	add r3, r1, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	ldr r0, [sp, #0x44]
	bl STRBUF_Delete
	ldr r0, [sp, #0x48]
	bl STRBUF_Delete
	ldr r0, [sp, #0x40]
	cmp r0, #0
	beq _02231FD2
	ldr r0, [sp, #0x14]
	mov r1, #0
	str r0, [sp, #0x18]
	add r0, #0x5c
	str r0, [sp, #0x18]
	bl GF_BGL_BmpWinDataFill
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	add r5, r0, #0
	ldr r0, [r7, #0x48]
	mov r1, #0x17
	bl MSGMAN_AllocString
	str r0, [sp, #0x4c]
	ldr r3, [sp, #0x40]
	add r0, r4, #0
	mov r1, #0
	add r2, r6, #0
	bl WORDSET_RegisterLocalPlaceName
	ldr r2, [sp, #0x4c]
	add r0, r4, #0
	add r1, r5, #0
	bl WORDSET_ExpandStr
	mov r1, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	add r2, r5, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x18]
	mov r3, #4
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x18]
	bl GF_BGL_BmpWinOnVReq
	add r0, r5, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x4c]
	bl STRBUF_Delete
_02231FD2:
	add r0, r4, #0
	bl WORDSET_ClearAllBuffer
	ldr r5, [sp, #0x14]
	ldr r0, [r7, #0x48]
	add r5, #0x6c
	mov r1, #0x11
	bl MSGMAN_AllocString
	add r6, r0, #0
	add r0, r5, #0
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	add r0, r5, #0
	add r1, r6, #0
	bl BR_print_x_Get
	mov r1, #0
	add r3, r0, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223200C
	b _02232010
	// .align 2, 0
// _02232004: .4byte win_p
// _02232008: .4byte mes_id
// _0223200C: .4byte 0x000F0D00
_02232010:
	add r2, r6, #0
	str r0, [sp, #8]
	add r0, r5, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	add r0, r5, #0
	bl GF_BGL_BmpWinOnVReq
	add r0, r6, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x14]
	add r1, sp, #0x50
	add r0, #0x7c
	str r0, [sp, #0x14]
	ldr r0, [sp, #0x20]
	mov r2, #0x66
	bl GDS_Profile_GetSelfIntroduction
	add r5, r0, #0
	bne _02232046
	add r0, sp, #0x50
	mov r1, #0x66
	bl PMSDAT_ToString
	add r5, r0, #0
_02232046:
	ldr r0, [sp, #0x14]
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r1, #0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _0223207C
	add r2, r5, #0
	str r0, [sp, #8]
	ldr r0, [sp, #0x14]
	add r3, r1, #0
	str r1, [sp, #0xc]
	bl GF_STR_PrintColor
	ldr r0, [sp, #0x14]
	bl GF_BGL_BmpWinOnVReq
	add r0, r5, #0
	bl STRBUF_Delete
	add r0, r4, #0
	bl WORDSET_Delete
	add sp, #0xd8
	pop {r3, r4, r5, r6, r7, pc}
	// .align 2, 0
	// _0223207C: .4byte 0x000F0D00
}
#endif

static void BR_prof_WinDel_Top( GPP_WORK* gwk, BR_WORK* wk )
{
	int i;
	
	for ( i = 0; i < 8; i++ ){
		GF_BGL_BmpWinOff( &gwk->win[ i ] );
		GF_BGL_BmpWinDel( &gwk->win[ i ] );		
	}	
}

static void PokeIcon_Visible( GPP_WORK* gwk, int flag )
{
	if ( gwk->cap_poke ){
		CATS_ObjectEnableCap( gwk->cap_poke, flag );
	}
}

static void PokeIcon_Del( GPP_WORK* gwk, BR_WORK* wk )
{
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_POKE_ICON );	
	
	if ( gwk->cap_poke ){
		CATS_ActorPointerDelete_S( gwk->cap_poke );
		gwk->cap_poke = NULL;
	}
}

static void PokeIcon_Add( GPP_WORK* gwk, BR_WORK* wk )
{	
	int egg;
	int monsno;
	int palno;
	int form;
	u8	trans_pal;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	//パレット
	trans_pal = CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp,
		ARC_POKEICON, PokeIconPalArcIndexGet(), 0, POKEICON_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON );
	//セル
	{
		int arc = PokeIcon64kCellArcIndexGet();
		CATS_LoadResourceCellArc( csp, crp, ARC_POKEICON, arc, 0, eID_OAM_POKE_ICON );
	}
	//セルアニメ
	{
		int arc = PokeIcon64kCellAnmArcIndexGet();
		CATS_LoadResourceCellAnmArc( csp, crp, ARC_POKEICON, arc, 0, eID_OAM_POKE_ICON );
	}

	{		
		int gra_id;
		
		monsno = GDS_Profile_GetMonsNo( gwk->gpp );
		form   = GDS_Profile_GetFormNo( gwk->gpp );
		egg	   = GDS_Profile_GetEggFlag( gwk->gpp );
		
		if ( monsno == 0 ){ return; }
		
		gra_id = PokeIconCgxArcIndexGetByMonsNumber( monsno, egg, form );
		
		CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_POKEICON, gra_id, 0,
						 							NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON );
	
		coap.x		= 72 + 4;
		coap.y		= 64 - 4;
		coap.z		= 0;		
		coap.anm	= 0;
		coap.pri	= 0;
		coap.pal	= 0;
		coap.d_area = CATS_D_AREA_MAIN;
		coap.bg_pri = 0;
		coap.vram_trans = 0;

		coap.id[0] = eID_OAM_POKE_ICON;
		coap.id[1] = eID_OAM_POKE_ICON;
		coap.id[2] = eID_OAM_POKE_ICON;
		coap.id[3] = eID_OAM_POKE_ICON;
		coap.id[4] = CLACT_U_HEADER_DATA_NONE;
		coap.id[5] = CLACT_U_HEADER_DATA_NONE;
		
		gwk->cap_poke = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );

		palno = PokeIconPalNumGet( monsno, form, egg );
		CATS_ObjectPaletteOffsetSetCap( gwk->cap_poke, trans_pal + palno );
		CATS_ObjectAnimeSeqSetCap( gwk->cap_poke, POKEICON_ANM_HPMAX );
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void HeroIcon_Visible( GPP_WORK* gwk, int flag )
{
	CATS_ObjectEnableCap( gwk->cap_hero, flag );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void HeroIcon_Del( GPP_WORK* gwk, BR_WORK* wk )
{	
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_HERO_ICON );
	
	CATS_ActorPointerDelete_S( gwk->cap_hero );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static const int wifi_tr_type_tbl[ BR_UNION_CHAR_MAX ][ 4 ] = {
	{
		BOY1,
		TRTYPE_SCHOOLB,
		NARC_wifi2dchar_boy1_NCLR,
		NARC_wifi2dchar_boy1_NCGR, 
	},
	{
		BOY3,
		TRTYPE_MUSHI,
		NARC_wifi2dchar_boy3_NCLR,
		NARC_wifi2dchar_boy3_NCGR, 
	},
	{
		MAN3,
		TRTYPE_ELITEM,
		NARC_wifi2dchar_man3_NCLR,
		NARC_wifi2dchar_man3_NCGR, 
	},
	{
		BADMAN,
		TRTYPE_HEADS,
		NARC_wifi2dchar_badman_NCLR,
		NARC_wifi2dchar_badman_NCGR, 
	},
	{
		EXPLORE,
		TRTYPE_ISEKI,
		NARC_wifi2dchar_explore_NCLR,
		NARC_wifi2dchar_explore_NCGR, 
	},
	{
		FIGHTER,
		TRTYPE_KARATE,
		NARC_wifi2dchar_fighter_NCLR,
		NARC_wifi2dchar_fighter_NCGR, 
	},
	{
		GORGGEOUSM,
		TRTYPE_PRINCE,
		NARC_wifi2dchar_gorggeousm_NCLR,
		NARC_wifi2dchar_gorggeousm_NCGR, 
	},
	{
		MYSTERY,
		TRTYPE_ESPM,
		NARC_wifi2dchar_mystery_NCLR,
		NARC_wifi2dchar_mystery_NCGR, 
	},
	{
		GIRL1,
		TRTYPE_MINI,
		NARC_wifi2dchar_girl1_NCLR,
		NARC_wifi2dchar_girl1_NCGR, 
	},
	{
		GIRL2,
		TRTYPE_BATTLEG,
		NARC_wifi2dchar_girl2_NCLR,
		NARC_wifi2dchar_girl2_NCGR, 
	},
	{
		WOMAN2,
		TRTYPE_SISTER,
		NARC_wifi2dchar_woman2_NCLR,
		NARC_wifi2dchar_woman2_NCGR, 
	},
	{
		WOMAN3,
		TRTYPE_ELITEW,
		NARC_wifi2dchar_woman3_NCLR,
		NARC_wifi2dchar_woman3_NCGR, 
	},
	{
		IDOL,
		TRTYPE_IDOL,
		NARC_wifi2dchar_idol_NCLR,
		NARC_wifi2dchar_idol_NCGR, 
	},
	{
		LADY,
		TRTYPE_MADAM,
		NARC_wifi2dchar_lady_NCLR,
		NARC_wifi2dchar_lady_NCGR, 
	},
	{
		COWGIRL,
		TRTYPE_COWGIRL,
		NARC_wifi2dchar_cowgirl_NCLR,
		NARC_wifi2dchar_cowgirl_NCGR, 
	},
	{
		GORGGEOUSW,
		TRTYPE_PRINCESS,
		NARC_wifi2dchar_gorggeousw_NCLR,
		NARC_wifi2dchar_gorggeousw_NCGR, 
	},	
};


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	trtype	
 * @param	data	1 = TR_TYPE, 2 = ncl, 3 = ncg 4 = trtype_name(オリジナルサイキッカーおとことか)
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int WiFi_TR_DataGet( int trtype, int data )
{
	int index = trtype;
	
	if ( trtype > BR_UNION_CHAR_MAX || trtype < 0 ){
		GF_ASSERT_MSG( 0, "trtype 異常 = %d\n", trtype );		
		index = 0;
	}
	
	if ( data == 4 ){
	//	OS_Printf( " trtype = %d\n", index );
		return msg_trtype_000 + index;
	}

	if ( data >= 5 ){
		return 0;
	}

	return wifi_tr_type_tbl[ index ][ data ];
}


static void HeroIcon_Add( GPP_WORK* gwk, BR_WORK* wk )
{
	int sex;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;

	int	ncl = NARC_wifi2dchar_pl_boy01_NCLR;
	int ncg = NARC_wifi2dchar_pl_boy01_NCGR;
	int	nce = NARC_wifi2dchar_npc_NCER;
	int nca = NARC_wifi2dchar_npc_NANR;	

	sex = GDS_Profile_GetSex( gwk->gpp );

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	if ( wk->ex_param1 == LOADDATA_MYREC ){
		
		if ( sex == PM_FEMALE ){
			ncl = NARC_wifi2dchar_pl_girl01_NCLR;
			ncg = NARC_wifi2dchar_pl_girl01_NCGR;
		}
	}
	else {
		int view = GDS_Profile_GetTrainerView( gwk->gpp );
		
		if ( view >= BR_UNION_CHAR_MAX ){ view = 0; }
		
		ncl = WiFi_TR_DataGet( view, 2 );
		ncg = WiFi_TR_DataGet( view, 3 );
	}
	
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp, ARC_WIFI2DCHAR, ncl, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );
	CATS_LoadResourceCellArc( csp, crp, ARC_WIFI2DCHAR, nce, 0, eID_OAM_HERO_ICON );
	CATS_LoadResourceCellAnmArc( csp, crp, ARC_WIFI2DCHAR, nca, 0, eID_OAM_HERO_ICON );
	CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_WIFI2DCHAR, ncg, 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );

	coap.x		= 40;
	coap.y		= 56+4;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 0;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_MAIN;
	coap.bg_pri = 0;
	coap.vram_trans = 0;
			
	coap.id[0] = eID_OAM_HERO_ICON;
	coap.id[1] = eID_OAM_HERO_ICON;
	coap.id[2] = eID_OAM_HERO_ICON;
	coap.id[3] = eID_OAM_HERO_ICON;
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	gwk->cap_hero = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
	CATS_ObjectAnimeSeqSetCap( gwk->cap_hero, eHERO_BOTTOM );
	CATS_ObjectUpdateCap( gwk->cap_hero );
}


//--------------------------------------------------------------
/**
 * @brief	GPPデータ作成
 *
 * @param	gwk	
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void GppDataMake( GPP_WORK* gwk, BR_WORK* wk )
{
	BR_prof_WinAdd_Top( gwk, wk );
	HeroIcon_Add( gwk, wk );
	PokeIcon_Add( gwk, wk );
}


//--------------------------------------------------------------
/**
 * @brief	GPPデータ解放
 *
 * @param	gwk	
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void GppDataFree( GPP_WORK* gwk, BR_WORK* wk )
{
	BR_prof_WinDel_Top( gwk, wk );
	GppData_IconEnable( gwk, FALSE );
	HeroIcon_Del( gwk, wk );
	PokeIcon_Del( gwk, wk );
}


//--------------------------------------------------------------
/**
 * @brief	GPPデータ非表示
 *
 * @param	gwk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void GppData_IconEnable( GPP_WORK* gwk, int flag )
{
	PokeIcon_Visible( gwk, flag );
	HeroIcon_Visible( gwk, flag );
}

// =============================================================================
//
//
//	■リスト処理
//
//
// =============================================================================
static void CountryPrint( TOUCH_LIST* tl, BR_WORK* wk )
{
	STRBUF*		str1;
	STRBUF*		tmp;
	WORDSET*	wset;
	GF_BGL_BMPWIN* win;
	
	int country = GDS_Profile_GetNation( wk->br_gpp[ 0 ] );
	int area	= GDS_Profile_GetArea( wk->br_gpp[ 0 ] );
	
	win = &tl->win;
	if ( country == 0 && area == 0 ){
		str1 = MSGMAN_AllocString( wk->sys.man, msg_722 );	
	}
	else {
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		
		if ( area != 0 ){
			str1  = STRBUF_Create( 255, HEAPID_BR );
			tmp = MSGMAN_AllocString( wk->sys.man, msg_23 );
			WORDSET_RegisterLocalPlaceName( wset, 0, country, area );
			WORDSET_ExpandStr( wset, str1, tmp );
		}
		else {
			str1  = STRBUF_Create( 255, HEAPID_BR );
			tmp = MSGMAN_AllocString( wk->sys.man, msg_22 );
			WORDSET_RegisterCountryName( wset, 0, country );
			WORDSET_ExpandStr( wset, str1, tmp );			
		}
		STRBUF_Delete( tmp );
		WORDSET_Delete( wset );				
	}
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 16, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
}
//--------------------------------------------------------------
/**
 * @brief	リストの更新
 *
 * @param	tl	
 * @param	wk	
 * @param	head	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_Init( TOUCH_LIST* tl, BR_WORK* wk, const TOUCH_LIST_HEADER* head )
{
	int i;
	
	GF_BGL_BMPWIN* win;
	STRBUF* 	str;
	
//	memset( tl, 0, sizeof( TOUCH_LIST ) );

	win = &tl->win;

	tl->max = head->data_max;
	tl->min = 0;
//	tl->now = 0;
	tl->old = 0;
	tl->lim = head->lim;
	tl->head = head;
	tl->man	 = wk->sys.man;
	tl->page = ( tl->max / tl->lim ) + 1;
	
	if ( tl->max < tl->lim ){
		 tl->lim = tl->max;
		 tl->view.t_stoper = tl->lim - 1;
		 tl->view.b_stoper = tl->lim - 1;
	}
//	tl->bView = TRUE;	
//	OS_Printf( "list max = %d\n", tl->max );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, head->frame, head->px, head->py, head->sx, head->sy, eBG_PAL_FONT, head->ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	for ( i = 0; i < tl->lim; i++ ){
		
		str = MSGMAN_AllocString( tl->man, head->info[ i ].msg );
		GF_STR_PrintColor( win, FONT_SYSTEM, str, 0, ( head->list_height * 16 ) * i, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
		STRBUF_Delete( str );
		if ( head->info[ i ].msg == msg_16 ){
			CountryPrint( tl, wk );
		}
	}
	GF_BGL_BmpWinOnVReq( win );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	tl	
 * @param	wk	
 * @param	man	
 * @param	head	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_InitEx( TOUCH_LIST* tl, BR_WORK* wk, MSGDATA_MANAGER* man, const TOUCH_LIST_HEADER* head )
{
	int i;
	
	GF_BGL_BMPWIN* win;
	STRBUF* 	str;
	
//	memset( tl, 0, sizeof( TOUCH_LIST ) );

	win = &tl->win;

	tl->max = head->data_max;
	tl->min = 0;
//	tl->now = 0;				///< 引き継ぐ
	tl->old = 0;				///< リストの再描画を促すために初期化 判定は now != old = 描画
	tl->lim = head->lim;
	tl->head = head;
	if ( man ){
		tl->man	 = man;
	}
	else {
		tl->man	 = wk->sys.man;
	}
	tl->page = ( tl->max / tl->lim ) + 1;
	
//	tl->bView = TRUE;	
	tl->view.t_stoper = tl->lim / 2;
	tl->view.b_stoper = tl->lim / 2;
	if ( tl->max < tl->lim ){
		 tl->lim = tl->max;
		 tl->view.t_stoper = tl->lim - 1;
		 tl->view.b_stoper = tl->lim - 1;
	}
//	OS_Printf( "stoper = %d %d\n", tl->view.t_stoper, tl->view.b_stoper );
//	OS_Printf( "list max = %d\n", tl->max );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, head->frame, head->px, head->py, head->sx, head->sy, eBG_PAL_FONT, head->ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	
	if ( head->info ){
		for ( i = 0; i < tl->lim; i++ ){		
			str = MSGMAN_AllocString( tl->man, head->info[ i ].msg );
			GF_STR_PrintColor( win, FONT_SYSTEM, str, 0, ( head->list_height * 16 ) * i, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
			STRBUF_Delete( str );
		}
	}
	else {
		int win_no;
		STRBUF* str1;
		STRBUF*	str_num;
		STRBUF* str_colon;
		int width;	
		for ( i = 0; i < tl->lim; i++ ){
			str1 	= STRBUF_Create( 255, HEAPID_BR );
			str_num = STRBUF_Create( 255, HEAPID_BR );
			str_colon = MSGMAN_AllocString( wk->sys.man, msg_607 );
			
			STRBUF_SetNumber( str_num, ( i + 1 ), 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			STRBUF_SetStringCode( str1, wk->recv_data.outline_data_tmp[ i ]->profile.name );
			BR_ErrorStrChange( wk, str1 );
			width = FontProc_GetPrintStrWidth( FONT_SYSTEM, str_num, 0 );

			GF_STR_PrintColor( win, FONT_SYSTEM, str_num, 	16 - width, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_STR_PrintColor( win, FONT_SYSTEM, str_colon, 16, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_STR_PrintColor( win, FONT_SYSTEM, str1, 		22, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );

			STRBUF_Delete( str1 );			
			STRBUF_Delete( str_num );		
			STRBUF_Delete( str_colon );
		}
	}
	
	
	GF_BGL_BmpWinOnVReq( win );
}

#ifdef NONEQUIVALENT
void TouchList_InitRanking( TOUCH_LIST* tl, BR_WORK* wk, MSGDATA_MANAGER* man, const TOUCH_LIST_HEADER* head, int ranking_no, int group_no, STRBUF* unit )
{
	int i;
	
	GF_BGL_BMPWIN* win;
	STRBUF* 	str;
	STRBUF*		str1;
	STRBUF*		str2;
	STRBUF*		str3;
	STRBUF*		temp;
	WORDSET*	wset;
	int now_rank  = 0;
	u64 now_param = 0;
	int rank_table[ 30 ] = { 0 };
	STRBUF*		str_num;

	win = &tl->win;

	tl->max = head->data_max;
	tl->min = 0;
//	tl->now = 0;				///< 引き継ぐ
	tl->old = 0;				///< リストの再描画を促すために初期化 判定は now != old = 描画
	tl->lim = head->lim;
	tl->head = head;
	if ( man ){
		tl->man	 = man;
	}
	else {
		tl->man	 = wk->sys.man;
	}
	tl->page = ( tl->max / tl->lim ) + 1;
	
	if ( tl->max < tl->lim ){
		 tl->lim = tl->max;
		 tl->view.t_stoper = tl->lim - 1;
		 tl->view.b_stoper = tl->lim - 1;
	}	
//	tl->bView = TRUE;
	tl->view.t_stoper = tl->lim / 2;
	tl->view.b_stoper = tl->lim / 2;
//	OS_Printf( "stoper = %d %d\n", tl->view.t_stoper, tl->view.b_stoper );
//	OS_Printf( "list max = %d\n", tl->max );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, head->frame, head->px, head->py, head->sx, head->sy, eBG_PAL_FONT, head->ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	
	wset = BR_WORDSET_Create( HEAPID_BR );
	
	
	{
		u64 score = tl->head->info[ 0 ].param_ex;
		int num[] = { GT_TRAINER_VIEW_MAX, GT_YEAR_MONTH_NUM, GT_MONSNO_RANKING_MAX };
		now_param = score;
		rank_table[ 0 ] = now_rank;		

		for ( i = 1; i < num[ group_no ]; i++ ){				
			u64 score = tl->head->info[ i ].param_ex;
			
			if ( score == now_param ){
				rank_table[ i ] = now_rank;
			}
			else {
				now_param = score;
				rank_table[ i ] = i;
				now_rank = i;
			}
		//	OS_Printf( "rank = %2d\n", rank_table[ i ] );
		}
	}
	
	for ( i = 0; i < tl->lim; i++ ){		
		str1 = MSGMAN_AllocString( wk->sys.man, msg_510 + group_no );		///< 順位：トレーナータイプ or 生まれ月 or すきなポケモン
//		str1 = MSGMAN_AllocString( wk->sys.man, msg_510 + ranking_no );		///< 順位：トレーナータイプ or 生まれ月 or すきなポケモン
		str2 = STRBUF_Create( 255, HEAPID_BR );
		temp = STRBUF_Create( 255, HEAPID_BR );		
		str_num  = STRBUF_Create( 255, HEAPID_BR );
		
	//	OS_Printf( "ranking data = %d\n", tl->head->info[ i ].param );
		
		switch ( group_no ){
		case 0:
			{
				int view  = tl->head->info[ i ].param;
				str3 = MSGMAN_AllocString( tl->man, view );
			}
			break;
		case 1:
			{
				int month = tl->head->info[ i ].param;
				str3 = STRBUF_Create( 255, HEAPID_BR );
				STRBUF_SetNumber( str3, month, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			}
			break;
		case 2:
			{
				STRCODE code[ 255 ];
				int monsno = tl->head->info[ i ].param;
				str3 = STRBUF_Create( 255, HEAPID_BR );
				MSGDAT_MonsNameGet( monsno, HEAPID_BR, code );
				STRBUF_SetStringCode( str3, code );
			}
			break;
		}
		{
			u64 score = tl->head->info[ i ].param_ex;
			STRBUF_SetNumber64( str_num, score, Number_to_Unit_Get( score ), NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
		}
		STRBUF_SetNumber( str2, rank_table[ i ] + 1, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
		WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	///< 順位
		WORDSET_RegisterWord( wset, 1, str3, 0, TRUE, PM_LANG );	///< 項目
		WORDSET_ExpandStr( wset, temp, str1 );
		
		GF_STR_PrintColor( win, FONT_SYSTEM, temp, 0, i * ( 16 * tl->head->list_height ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
		if ( tl->head->list_height == 2 ){
			int width = FontProc_GetPrintStrWidth( FONT_SYSTEM, str_num, 0 );
			GF_STR_PrintColor( win, FONT_SYSTEM, str_num, 16, ( i * ( 16 * tl->head->list_height ) ) + 16, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			if ( unit ){
				GF_STR_PrintColor( win, FONT_SYSTEM, unit, 16 + width, ( i * ( 16 * tl->head->list_height ) ) + 16, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			}
		}
		STRBUF_Delete( str1 );
		STRBUF_Delete( str2 );
		STRBUF_Delete( str3 );
		STRBUF_Delete( temp );
		
		STRBUF_Delete( str_num );
		
		WORDSET_ClearAllBuffer( wset );	
	}	

	GF_BGL_BmpWinOnVReq( win );	
	WORDSET_Delete( wset );
}
#else

extern void _s32_div_f(void);

const int TouchList_InitRanking_num[] = { GT_TRAINER_VIEW_MAX, GT_YEAR_MONTH_NUM, GT_MONSNO_RANKING_MAX };

asm void TouchList_InitRanking( TOUCH_LIST* tl, BR_WORK* wk, MSGDATA_MANAGER* man, const TOUCH_LIST_HEADER* head, int ranking_no, int group_no, STRBUF* unit )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x1fc
	sub sp, #0xe0
	add r4, r0, #0
	ldr r0, [sp, #0x2f4]
	add r5, r3, #0
	str r0, [sp, #0x2f4]
	ldr r0, [sp, #0x2f8]
	str r1, [sp, #0x14]
	str r0, [sp, #0x2f8]
	mov r0, #0
	str r0, [sp, #0x40]
	add r6, sp, #0x264
	add r1, r0, #0
	mov r3, #7
_02232796:
	stmia r6!, {r0, r1}
	stmia r6!, {r0, r1}
	sub r3, r3, #1
	bne _02232796
	stmia r6!, {r0, r1}
	add r0, r4, #0
	str r0, [sp, #0x50]
	add r0, #0x18
	str r0, [sp, #0x50]
	ldr r0, [r5, #4]
	str r0, [r4, #4]
	mov r0, #0
	str r0, [r4, #0]
	str r0, [r4, #0xc]
	ldr r0, [r5, #0x24]
	cmp r2, #0
	str r0, [r4, #0x10]
	str r5, [r4, #0x28]
	beq _022327C0
	str r2, [r4, #0x34]
	b _022327C6
_022327C0:
	ldr r0, [sp, #0x14]
	ldr r0, [r0, #0x48]
	str r0, [r4, #0x34]
_022327C6:
	ldr r0, [r4, #4]
	ldr r1, [r4, #0x10]
	bl _s32_div_f
	add r0, r0, #1
	str r0, [r4, #0x14]
	ldr r1, [r4, #4]
	ldr r0, [r4, #0x10]
	cmp r1, r0
	bge _022327E6
	str r1, [r4, #0x10]
	sub r0, r1, #1
	str r0, [r4, #0x40]
	ldr r0, [r4, #0x10]
	sub r0, r0, #1
	str r0, [r4, #0x44]
_022327E6:
	ldr r1, [r4, #0x10]
	lsr r0, r1, #0x1f
	add r0, r1, r0
	asr r0, r0, #1
	str r0, [r4, #0x40]
	ldr r1, [r4, #0x10]
	lsr r0, r1, #0x1f
	add r0, r1, r0
	asr r0, r0, #1
	str r0, [r4, #0x44]
	ldr r0, [sp, #0x50]
	bl GF_BGL_BmpWinInit
	ldr r0, [r5, #0x10]
	ldr r1, [sp, #0x50]
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp]
	ldr r0, [r5, #0x14]
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp, #4]
	ldr r0, [r5, #0x18]
	lsl r0, r0, #0x18
	lsr r0, r0, #0x18
	str r0, [sp, #8]
	mov r0, #0xe
	str r0, [sp, #0xc]
	ldr r0, [r5, #0x1c]
	lsl r0, r0, #0x10
	lsr r0, r0, #0x10
	str r0, [sp, #0x10]
	ldr r0, [sp, #0x14]
	ldr r2, [r5, #0x20]
	ldr r3, [r5, #0xc]
	lsl r2, r2, #0x18
	lsl r3, r3, #0x18
	ldr r0, [r0, #0x24]
	lsr r2, r2, #0x18
	lsr r3, r3, #0x18
	bl GF_BGL_BmpWinAdd
	ldr r0, [sp, #0x50]
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r0, #0x66
	bl BR_WORDSET_Create
	add r2, sp, #0x58
	ldr r3, =TouchList_InitRanking_num // _02232AA4
	add r5, r0, #0
	ldmia r3!, {r0, r1}
	add r7, r2, #0
	stmia r2!, {r0, r1}
	ldr r0, [r3, #0]
	mov r6, #1
	str r0, [r2, #0]
	ldr r0, [r4, #0x28]
	ldr r1, [r0, #0]
	ldr r0, [r1, #8]
	str r0, [sp, #0x38]
	ldr r0, [r1, #0xc]
	str r0, [sp, #0x3c]
	mov r0, #0
	str r0, [sp, #0x264]
	ldr r0, [sp, #0x2f4]
	lsl r0, r0, #2
	ldr r1, [r7, r0]
	cmp r1, #1
	ble _022328B0
	add r0, r1, #0
	mov r2, #0x10
	add r3, sp, #0x268
	mov ip, r0
_0223287C:
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #0]
	add r0, r0, r2
	ldr r1, [r0, #0xc]
	ldr r7, [r0, #8]
	ldr r0, [sp, #0x3c]
	str r1, [sp, #0x54]
	eor r1, r0
	ldr r0, [sp, #0x38]
	eor r0, r7
	orr r0, r1
	bne _0223289A
	ldr r0, [sp, #0x40]
	str r0, [r3, #0]
	b _022328A4
_0223289A:
	ldr r0, [sp, #0x54]
	str r7, [sp, #0x38]
	str r0, [sp, #0x3c]
	str r6, [r3, #0]
	str r6, [sp, #0x40]
_022328A4:
	add r6, r6, #1
	mov r0, ip
	add r2, #0x10
	add r3, r3, #4
	cmp r6, r0
	blt _0223287C
_022328B0:
	mov r0, #0
	str r0, [sp, #0x18]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	bgt _022328BC
	b _02232A90
_022328BC:
	add r0, sp, #0x264
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x2f4]
	ldr r6, [sp, #0x18]
	str r0, [sp, #0x1c]
	add r0, #0x58
	str r0, [sp, #0x1c]
_022328CA:
	ldr r0, [sp, #0x14]
	ldr r1, [sp, #0x1c]
	ldr r0, [r0, #0x48]
	bl MSGMAN_AllocString
	str r0, [sp, #0x4c]
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	str r0, [sp, #0x48]
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	str r0, [sp, #0x34]
	ldr r0, [sp, #0x2f4]
	cmp r0, #0
	beq _02232904
	cmp r0, #1
	beq _0223292A
	cmp r0, #2
	beq _0223294A
	b _02232984
_02232904:
	ldr r1, [r4, #0x28]
	ldr r0, [r4, #0x34]
	ldr r1, [r1, #0]
	add r1, r1, r6
	ldr r1, [r1, #4]
	bl MSGMAN_AllocString
	str r0, [sp, #0x44]
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x44]
	add r0, r5, #0
	mov r1, #1
	mov r3, #0
	bl WORDSET_RegisterWord
	b _02232984
_0223292A:
	ldr r0, [r4, #0x28]
	mov r1, #0x66
	ldr r0, [r0, #0]
	add r0, r0, r6
	ldr r0, [r0, #4]
	str r0, [sp, #0x30]
	mov r0, #0xff
	bl STRBUF_Create
	str r0, [sp, #0x44]
	ldr r2, [sp, #0x30]
	add r0, r5, #0
	mov r1, #1
	bl WORDSET_RegisterMonthName
	b _02232984
_0223294A:
	ldr r0, [r4, #0x28]
	mov r1, #0x66
	ldr r0, [r0, #0]
	add r0, r0, r6
	ldr r0, [r0, #4]
	str r0, [sp, #0x2c]
	mov r0, #0xff
	bl STRBUF_Create
	str r0, [sp, #0x44]
	ldr r0, [sp, #0x2c]
	mov r1, #0x66
	add r2, sp, #0x64
	bl MSGDAT_MonsNameGet
	ldr r0, [sp, #0x44]
	add r1, sp, #0x64
	bl STRBUF_SetStringCode
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x44]
	add r0, r5, #0
	mov r1, #1
	mov r3, #0
	bl WORDSET_RegisterWord
_02232984:
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #0]
	add r1, r0, r6
	ldr r0, [r1, #8]
	str r0, [sp, #0x24]
	ldr r0, [r1, #0xc]
	str r0, [sp, #0x28]
	ldr r0, [sp, #0x24]
	ldr r1, [sp, #0x28]
	bl Number_to_Unit_Get
	add r3, r0, #0
	mov r0, #0
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r0, [sp, #0x34]
	ldr r1, [sp, #0x24]
	ldr r2, [sp, #0x28]
	bl STRBUF_SetNumber64
	mov r0, #1
	str r0, [sp]
	ldr r1, [sp, #0x20]
	ldr r0, [sp, #0x48]
	ldr r1, [r1, #0]
	mov r2, #2
	add r1, r1, #1
	mov r3, #1
	bl STRBUF_SetNumber
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	mov r1, #0
	str r0, [sp, #4]
	ldr r2, [sp, #0x48]
	add r0, r5, #0
	add r3, r1, #0
	bl WORDSET_RegisterWord
	ldr r2, [sp, #0x4c]
	add r0, r5, #0
	add r1, r7, #0
	bl WORDSET_ExpandStr
	ldr r0, [r4, #0x28]
	add r2, r7, #0
	ldr r0, [r0, #8]
	lsl r1, r0, #4
	ldr r0, [sp, #0x18]
	mul r1, r0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _02232AA8
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x50]
	add r3, r1, #0
	bl GF_STR_PrintColor
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #8]
	cmp r0, #2
	bne _02232A56
	ldr r0, [sp, #0x2f8]
	cmp r0, #0
	beq _02232A56
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x34]
	add r0, r5, #0
	mov r1, #2
	mov r3, #0
	bl WORDSET_RegisterWord
	ldr r2, [sp, #0x2f8]
	add r0, r5, #0
	add r1, r7, #0
	bl WORDSET_ExpandStr
	ldr r0, [r4, #0x28]
	add r2, r7, #0
	ldr r0, [r0, #8]
	mov r3, #0x10
	lsl r1, r0, #4
	ldr r0, [sp, #0x18]
	mul r1, r0
	add r1, #0x10
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _02232AA8
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x50]
	bl GF_STR_PrintColor
_02232A56:
	ldr r0, [sp, #0x4c]
	bl STRBUF_Delete
	ldr r0, [sp, #0x48]
	bl STRBUF_Delete
	ldr r0, [sp, #0x44]
	bl STRBUF_Delete
	add r0, r7, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x34]
	bl STRBUF_Delete
	add r0, r5, #0
	bl WORDSET_ClearAllBuffer
	ldr r0, [sp, #0x20]
	ldr r1, [r4, #0x10]
	add r0, r0, #4
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x18]
	add r6, #0x10
	add r0, r0, #1
	str r0, [sp, #0x18]
	cmp r0, r1
	bge _02232A90
	b _022328CA
_02232A90:
	ldr r0, [sp, #0x50]
	bl GF_BGL_BmpWinOnVReq
	add r0, r5, #0
	bl WORDSET_Delete
	add sp, #0x1fc
	add sp, #0xe0
	pop {r4, r5, r6, r7, pc}
	nop
// _02232AA4: .4byte TouchList_InitRanking_num
// _02232AA8: .4byte 0x000F0D00
}
#endif

//--------------------------------------------------------------
/**
 * @brief	ボックス用の初期化
 *
 * @param	tl	
 * @param	wk	
 * @param	head	
 * @param	str[20][2]	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_InitBoxShot( TOUCH_LIST* tl, BR_WORK* wk, const TOUCH_LIST_HEADER* head, STRBUF* str[20][2] )
{
	int i;

	GF_BGL_BMPWIN* win;
	win = &tl->win;

	tl->max = head->data_max;
	tl->min = 0;
//	tl->now = 0;
	tl->old = 0;
	tl->lim = head->lim;
	tl->head = head;
	tl->man	 = wk->sys.man;
	tl->page = ( tl->max / tl->lim ) + 1;
		
	tl->view.t_stoper = tl->lim / 2;
	tl->view.b_stoper = tl->lim / 2;
	
	if ( tl->max < tl->lim ){
		 tl->lim = tl->max;
		 tl->view.t_stoper = tl->lim - 1;
		 tl->view.b_stoper = tl->lim - 1;
	}
	tl->bView = TRUE;
//	OS_Printf( "list max = %d\n", tl->max );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, head->frame, head->px, head->py, head->sx, head->sy, eBG_PAL_FONT, head->ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );	

	for ( i = 0; i < tl->lim; i++ ){
		GF_STR_PrintColor( win, FONT_SYSTEM, str[ i ][ 0 ],   0, ( ( head->list_height * 8 ) * i ) + 4, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
		GF_STR_PrintColor( win, FONT_SYSTEM, str[ i ][ 1 ], 136, ( ( head->list_height * 8 ) * i ) + 4, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
	}
	GF_BGL_BmpWinOnVReq( win );
}

//--------------------------------------------------------------
/**
 * @brief	ボックス用のメイン
 *
 * @param	tl	
 * @param	wk	
 * @param	str[20][2]	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int TouchList_MainBoxShot( TOUCH_LIST* tl, BR_WORK* wk, STRBUF* str[20][2] )
{
	int i;
	int param = 0;
	GF_BGL_BMPWIN* win;
	
	if ( tl->view.this == tl->old ){ return param; }	
	if ( tl->lim == tl->max ){ return param; }
	
	win = &tl->win;
	
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	{
		int i;
		int s = 0;
		int e;
				
		if ( tl->view.l_pos >= tl->view.t_stoper ){
			s = tl->view.this - tl->view.t_stoper;
			if ( tl->view.this >= ( tl->max - ( tl->view.t_stoper + 1 ) ) ){
				s = tl->max - tl->lim;
			}
		}

		for ( i = 0; i < tl->lim; i++ ){
			if ( str[ s + i ][ 0 ] == NULL ){ continue; }
			GF_STR_PrintColor( win, FONT_SYSTEM, str[ s + i ][ 0 ],   0, ( ( tl->head->list_height * 8 ) * i ) + 4, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_STR_PrintColor( win, FONT_SYSTEM, str[ s + i ][ 1 ], 136, ( ( tl->head->list_height * 8 ) * i ) + 4, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
		}
	}
	GF_BGL_BmpWinOn( win );	

	tl->old = tl->view.this;

	return param;
}

//--------------------------------------------------------------
/**
 * @brief	リストの更新
 *
 * @param	tl	
 * @param	wk	
 * @param	head	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
#ifdef NONEQUIVALENT
int TouchList_Ranking_Main( TOUCH_LIST* tl, BR_WORK* wk, int ranking_no, int group_no, STRBUF* unit )
{
	int i;
	int param = 0;	
	GF_BGL_BMPWIN* win;
	STRBUF*		str1;
	STRBUF*		str2;
	STRBUF*		str3;
	STRBUF*		temp;
	STRBUF*		str_num;
	WORDSET*	wset;
	
	if ( tl->now == tl->old ){ return param; }	
	if ( tl->lim == tl->max ){ return param; }
	
	win = &tl->win;
	
	wset = BR_WORDSET_Create( HEAPID_BR );
	
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	{
		int no = 0;
		int now_rank  = 0;
		u64 now_param = 0;
		int s = tl->now;
		int e = s + tl->lim;
		int rank_table[ 30 ] = { 0 };
				
		if ( e >= tl->max ){
			 s = tl->max - tl->lim;
			 e = tl->max;
		}
		
		{
			u64 score = tl->head->info[ 0 ].param_ex;
			int num[] = { GT_TRAINER_VIEW_MAX, GT_YEAR_MONTH_NUM, GT_MONSNO_RANKING_MAX };
			now_param = score;
			rank_table[ 0 ] = now_rank;
			

			for ( i = 1; i < num[ group_no ]; i++ ){				
				u64 score = tl->head->info[ i ].param_ex;
				
				if ( score == now_param ){
					rank_table[ i ] = now_rank;
				}
				else {
					now_param = score;
					rank_table[ i ] = i;
					now_rank = i;
				}
			//	OS_Printf( "rank = %2d\n", rank_table[ i ] );
			}
		}
		
		for ( i = s; i < e; i++ ){		
			str1 = MSGMAN_AllocString( wk->sys.man, msg_510 + group_no );		///< 順位：トレーナータイプ or 生まれ月 or すきなポケモン
//			str1 = MSGMAN_AllocString( wk->sys.man, msg_510 + ranking_no );		///< 順位：トレーナータイプ or 生まれ月 or すきなポケモン			
			str2 = STRBUF_Create( 255, HEAPID_BR );
			temp = STRBUF_Create( 255, HEAPID_BR );		
			str_num  = STRBUF_Create( 255, HEAPID_BR );
			
		//	OS_Printf( "ranking data = %d\n", tl->head->info[ i ].param );
			
			switch ( group_no ){
			case 0:
				{
					int view  = tl->head->info[ i ].param;
					str3 = MSGMAN_AllocString( tl->man, view );
				}
				break;
			case 1:
				{
					int month = tl->head->info[ i ].param;
					str3 = STRBUF_Create( 255, HEAPID_BR );
					STRBUF_SetNumber( str3, month, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
				}
				break;
			case 2:
				{
					STRCODE code[ 255 ];
					int monsno = tl->head->info[ i ].param;
					str3 = STRBUF_Create( 255, HEAPID_BR );
					MSGDAT_MonsNameGet( monsno, HEAPID_BR, code );
					STRBUF_SetStringCode( str3, code );
				}
				break;
			}
			{
				u64 score = tl->head->info[ i ].param_ex;
				score = tl->head->info[ i ].param_ex;					
				STRBUF_SetNumber64( str_num, score, Number_to_Unit_Get( score ), NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);		
			}
			STRBUF_SetNumber( str2, rank_table[ i ] + 1, 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);			
			WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	///< 順位
			WORDSET_RegisterWord( wset, 1, str3, 0, TRUE, PM_LANG );	///< 項目
			WORDSET_ExpandStr( wset, temp, str1 );
			
			GF_STR_PrintColor( win, FONT_SYSTEM, temp, 0, no * ( 16 * tl->head->list_height ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			if ( tl->head->list_height == 2 ){
				int width = FontProc_GetPrintStrWidth( FONT_SYSTEM, str_num, 0 );
				GF_STR_PrintColor( win, FONT_SYSTEM, str_num, 16, ( no * ( 16 * tl->head->list_height ) ) + 16, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
				if ( unit ){
					GF_STR_PrintColor( win, FONT_SYSTEM, unit, 16 + width, ( no * ( 16 * tl->head->list_height ) ) + 16, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
				}
			}
			no++;
			STRBUF_Delete( str1 );
			STRBUF_Delete( str2 );
			STRBUF_Delete( str3 );
			STRBUF_Delete( temp );
			
			STRBUF_Delete( str_num );
			
			WORDSET_ClearAllBuffer( wset );	
		}
	}	

	GF_BGL_BmpWinOnVReq( win );	
	WORDSET_Delete( wset );
	
	tl->old = tl->now;
	
	return param;	
}
#else
const int TouchList_Ranking_Main_num[] = { GT_TRAINER_VIEW_MAX, GT_YEAR_MONTH_NUM, GT_MONSNO_RANKING_MAX };

asm int TouchList_Ranking_Main( TOUCH_LIST* tl, BR_WORK* wk, int ranking_no, int group_no, STRBUF* unit )
{
	push {r4, r5, r6, r7, lr}
	sub sp, #0x1fc
	sub sp, #0xe8
	add r4, r0, #0
	ldr r0, [sp, #0x2f8]
	str r1, [sp, #0x10]
	str r0, [sp, #0x2f8]
	mov r0, #8
	ldrsh r1, [r4, r0]
	ldr r0, [r4, #0xc]
	str r3, [sp, #0x14]
	cmp r1, r0
	bne _02232C9A
	add sp, #0x1fc
	add sp, #0xe8
	mov r0, #0
	pop {r4, r5, r6, r7, pc}
_02232C9A:
	ldr r1, [r4, #0x10]
	ldr r0, [r4, #4]
	cmp r1, r0
	bne _02232CAA
	add sp, #0x1fc
	add sp, #0xe8
	mov r0, #0
	pop {r4, r5, r6, r7, pc}
_02232CAA:
	add r0, r4, #0
	str r0, [sp, #0x58]
	add r0, #0x18
	str r0, [sp, #0x58]
	mov r0, #0x66
	bl BR_WORDSET_Create
	add r5, r0, #0
	ldr r0, [sp, #0x58]
	mov r1, #0
	bl GF_BGL_BmpWinDataFill
	mov r0, #0
	str r0, [sp, #0x44]
	str r0, [sp, #0x40]
	mov r0, #8
	ldrsh r0, [r4, r0]
	ldr r1, [r4, #0x10]
	add r3, sp, #0x26c
	str r0, [sp, #0x18]
	add r0, r0, r1
	str r0, [sp, #0x34]
	ldr r0, [sp, #0x44]
	mov r2, #7
	add r1, r0, #0
_02232CDC:
	stmia r3!, {r0, r1}
	stmia r3!, {r0, r1}
	sub r2, r2, #1
	bne _02232CDC
	stmia r3!, {r0, r1}
	ldr r1, [r4, #4]
	ldr r0, [sp, #0x34]
	cmp r0, r1
	blt _02232CF6
	ldr r0, [r4, #0x10]
	str r1, [sp, #0x34]
	sub r0, r1, r0
	str r0, [sp, #0x18]
_02232CF6:
	ldr r3, =TouchList_Ranking_Main_num // _02232F60
	add r2, sp, #0x60
	ldmia r3!, {r0, r1}
	add r7, r2, #0
	stmia r2!, {r0, r1}
	ldr r0, [r3, #0]
	mov r6, #1
	str r0, [r2, #0]
	ldr r0, [r4, #0x28]
	ldr r1, [r0, #0]
	ldr r0, [r1, #8]
	str r0, [sp, #0x38]
	ldr r0, [r1, #0xc]
	str r0, [sp, #0x3c]
	mov r0, #0
	str r0, [sp, #0x26c]
	ldr r0, [sp, #0x14]
	lsl r0, r0, #2
	ldr r1, [r7, r0]
	cmp r1, #1
	ble _02232D5C
	add r0, r1, #0
	mov r2, #0x10
	add r3, sp, #0x270
	mov ip, r0
_02232D28:
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #0]
	add r0, r0, r2
	ldr r1, [r0, #0xc]
	ldr r7, [r0, #8]
	ldr r0, [sp, #0x3c]
	str r1, [sp, #0x5c]
	eor r1, r0
	ldr r0, [sp, #0x38]
	eor r0, r7
	orr r0, r1
	bne _02232D46
	ldr r0, [sp, #0x40]
	str r0, [r3, #0]
	b _02232D50
_02232D46:
	ldr r0, [sp, #0x5c]
	str r7, [sp, #0x38]
	str r0, [sp, #0x3c]
	str r6, [r3, #0]
	str r6, [sp, #0x40]
_02232D50:
	add r6, r6, #1
	mov r0, ip
	add r2, #0x10
	add r3, r3, #4
	cmp r6, r0
	blt _02232D28
_02232D5C:
	ldr r1, [sp, #0x18]
	ldr r0, [sp, #0x34]
	cmp r1, r0
	blt _02232D66
	b _02232F46
_02232D66:
	add r0, r1, #0
	lsl r6, r0, #4
	add r1, sp, #0x26c
	lsl r0, r0, #2
	add r0, r1, r0
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x14]
	str r0, [sp, #0x1c]
	add r0, #0x58
	str r0, [sp, #0x1c]
_02232D7A:
	ldr r0, [sp, #0x10]
	ldr r1, [sp, #0x1c]
	ldr r0, [r0, #0x48]
	bl MSGMAN_AllocString
	str r0, [sp, #0x54]
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	str r0, [sp, #0x50]
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	add r7, r0, #0
	mov r0, #0xff
	mov r1, #0x66
	bl STRBUF_Create
	str r0, [sp, #0x48]
	ldr r0, [sp, #0x14]
	cmp r0, #0
	beq _02232DB4
	cmp r0, #1
	beq _02232DDA
	cmp r0, #2
	beq _02232DFA
	b _02232E34
_02232DB4:
	ldr r1, [r4, #0x28]
	ldr r0, [r4, #0x34]
	ldr r1, [r1, #0]
	add r1, r1, r6
	ldr r1, [r1, #4]
	bl MSGMAN_AllocString
	str r0, [sp, #0x4c]
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x4c]
	add r0, r5, #0
	mov r1, #1
	mov r3, #0
	bl WORDSET_RegisterWord
	b _02232E34
_02232DDA:
	ldr r0, [r4, #0x28]
	mov r1, #0x66
	ldr r0, [r0, #0]
	add r0, r0, r6
	ldr r0, [r0, #4]
	str r0, [sp, #0x30]
	mov r0, #0xff
	bl STRBUF_Create
	str r0, [sp, #0x4c]
	ldr r2, [sp, #0x30]
	add r0, r5, #0
	mov r1, #1
	bl WORDSET_RegisterMonthName
	b _02232E34
_02232DFA:
	ldr r0, [r4, #0x28]
	mov r1, #0x66
	ldr r0, [r0, #0]
	add r0, r0, r6
	ldr r0, [r0, #4]
	str r0, [sp, #0x2c]
	mov r0, #0xff
	bl STRBUF_Create
	str r0, [sp, #0x4c]
	ldr r0, [sp, #0x2c]
	mov r1, #0x66
	add r2, sp, #0x6c
	bl MSGDAT_MonsNameGet
	ldr r0, [sp, #0x4c]
	add r1, sp, #0x6c
	bl STRBUF_SetStringCode
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x4c]
	add r0, r5, #0
	mov r1, #1
	mov r3, #0
	bl WORDSET_RegisterWord
_02232E34:
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #0]
	add r1, r0, r6
	ldr r0, [r1, #8]
	str r0, [sp, #0x24]
	ldr r0, [r1, #0xc]
	str r0, [sp, #0x28]
	ldr r0, [sp, #0x24]
	ldr r1, [sp, #0x28]
	bl Number_to_Unit_Get
	add r3, r0, #0
	mov r0, #0
	str r0, [sp]
	mov r0, #1
	str r0, [sp, #4]
	ldr r0, [sp, #0x48]
	ldr r1, [sp, #0x24]
	ldr r2, [sp, #0x28]
	bl STRBUF_SetNumber64
	mov r0, #1
	str r0, [sp]
	ldr r1, [sp, #0x20]
	ldr r0, [sp, #0x50]
	ldr r1, [r1, #0]
	mov r2, #2
	add r1, r1, #1
	mov r3, #1
	bl STRBUF_SetNumber
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	mov r1, #0
	str r0, [sp, #4]
	ldr r2, [sp, #0x50]
	add r0, r5, #0
	add r3, r1, #0
	bl WORDSET_RegisterWord
	ldr r2, [sp, #0x54]
	add r0, r5, #0
	add r1, r7, #0
	bl WORDSET_ExpandStr
	ldr r0, [r4, #0x28]
	add r2, r7, #0
	ldr r0, [r0, #8]
	lsl r1, r0, #4
	ldr r0, [sp, #0x44]
	mul r1, r0
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _02232F64
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x58]
	add r3, r1, #0
	bl GF_STR_PrintColor
	ldr r0, [r4, #0x28]
	ldr r0, [r0, #8]
	cmp r0, #2
	bne _02232F06
	ldr r0, [sp, #0x2f8]
	cmp r0, #0
	beq _02232F06
	mov r0, #1
	str r0, [sp]
	mov r0, #2
	str r0, [sp, #4]
	ldr r2, [sp, #0x48]
	add r0, r5, #0
	mov r1, #2
	mov r3, #0
	bl WORDSET_RegisterWord
	ldr r2, [sp, #0x2f8]
	add r0, r5, #0
	add r1, r7, #0
	bl WORDSET_ExpandStr
	ldr r0, [r4, #0x28]
	add r2, r7, #0
	ldr r0, [r0, #8]
	mov r3, #0x10
	lsl r1, r0, #4
	ldr r0, [sp, #0x44]
	mul r1, r0
	add r1, #0x10
	str r1, [sp]
	mov r0, #0xff
	str r0, [sp, #4]
	ldr r0, =0x000F0D00 // _02232F64
	mov r1, #0
	str r0, [sp, #8]
	mov r0, #0
	str r0, [sp, #0xc]
	ldr r0, [sp, #0x58]
	bl GF_STR_PrintColor
_02232F06:
	ldr r0, [sp, #0x44]
	add r0, r0, #1
	str r0, [sp, #0x44]
	ldr r0, [sp, #0x54]
	bl STRBUF_Delete
	ldr r0, [sp, #0x50]
	bl STRBUF_Delete
	ldr r0, [sp, #0x4c]
	bl STRBUF_Delete
	add r0, r7, #0
	bl STRBUF_Delete
	ldr r0, [sp, #0x48]
	bl STRBUF_Delete
	add r0, r5, #0
	bl WORDSET_ClearAllBuffer
	ldr r0, [sp, #0x20]
	add r6, #0x10
	add r0, r0, #4
	str r0, [sp, #0x20]
	ldr r0, [sp, #0x18]
	add r1, r0, #1
	ldr r0, [sp, #0x34]
	str r1, [sp, #0x18]
	cmp r1, r0
	bge _02232F46
	b _02232D7A
_02232F46:
	ldr r0, [sp, #0x58]
	bl GF_BGL_BmpWinOnVReq
	add r0, r5, #0
	bl WORDSET_Delete
	mov r0, #8
	ldrsh r0, [r4, r0]
	str r0, [r4, #0xc]
	mov r0, #0
	add sp, #0x1fc
	add sp, #0xe8
	pop {r4, r5, r6, r7, pc}
	// .align 2, 0
// _02232F60: .4byte TouchList_Ranking_Main_num
// _02232F64: .4byte 0x000F0D00
}
#endif

//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
int TouchList_Main( TOUCH_LIST* tl, BR_WORK* wk )
{
	int i;
	int param = 0;
	GF_BGL_BMPWIN* win;
	STRBUF* 	str;
	
	///< タッチ判定
	if ( tl->head->hit != NULL ){	
		for ( i = 0; i < tl->lim; i++ ){			
			if ( GF_TP_SingleHitTrg( &tl->head->hit[ i ] ) ){
				TouchAction( wk );
				param = tl->head->info[ tl->now + i ].param;
			}
		}
	}
	
//	if ( param )OS_Printf( "param = %d\n", param );
	
	if ( tl->now == tl->old ){ return param; }	
	if ( tl->lim == tl->max ){ return param; }
	
	win = &tl->win;
	
	GF_BGL_BmpWinDataFill( win, 0x00 );	
	{
		int no = 0;
		int s = tl->now;
		int e = s + tl->lim;
				
		if ( e >= tl->max ){
			 s = tl->max - tl->lim;
			 e = tl->max;
		}

		for ( i = s; i < e; i++ ){
			str = MSGMAN_AllocString( tl->man, tl->head->info[ i ].msg );
			GF_STR_PrintColor( win, FONT_SYSTEM, str, 0, ( tl->head->list_height * 16 ) * no, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			STRBUF_Delete( str );			
			no++;
		}		
	}
	GF_BGL_BmpWinOn( win );	

	tl->old = tl->now;

	return param;
}


//--------------------------------------------------------------
/**
 * @brief	可視範囲モード
 *
 * @param	tl	
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
int TouchList_ViewMode_Main( TOUCH_LIST* tl, BR_WORK* wk )
{
	int i;
	int param = 0;
	GF_BGL_BMPWIN* win;
	STRBUF* 	str;
	
	if ( tl->view.this == tl->old ){ return param; }	
	if ( tl->lim == tl->max ){ return param; }
	
	win = &tl->win;

	GF_BGL_BmpWinDataFill( win, 0x00 );	
	{
		int i;
		int s = 0;
		int win_no;
		STRBUF* str1;
		STRBUF*	str_num;
		STRBUF* str_colon;
		int width;
		int e;
				
		if ( tl->view.l_pos >= tl->view.t_stoper ){
			s = tl->view.this - tl->view.t_stoper;
			if ( tl->view.this >= ( tl->max - ( tl->view.t_stoper + 1 ) ) ){
				s = tl->max - tl->lim;
			}
		}
		
	//	OS_Printf( "s = %d\n", s );
				
		for ( i = 0; i < tl->lim; i++ ){
			str1 	= STRBUF_Create( 255, HEAPID_BR );
			str_num = STRBUF_Create( 255, HEAPID_BR );
			str_colon = MSGMAN_AllocString( wk->sys.man, msg_607 );
			
			STRBUF_SetNumber( str_num, ( s + i + 1 ), 2, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT);
			STRBUF_SetStringCode( str1, wk->recv_data.outline_data_tmp[ s + i ]->profile.name );
			BR_ErrorStrChange( wk, str1 );
			width = FontProc_GetPrintStrWidth( FONT_SYSTEM, str_num, 0 );

			GF_STR_PrintColor( win, FONT_SYSTEM, str_num, 	16 - width, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_STR_PrintColor( win, FONT_SYSTEM, str_colon, 16, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_STR_PrintColor( win, FONT_SYSTEM, str1, 		22, 4 + ( i * 24 ), MSG_NO_PUT, PRINT_COL_NORMAL, NULL );

			STRBUF_Delete( str1 );			
			STRBUF_Delete( str_num );		
			STRBUF_Delete( str_colon );
		}
	}
	GF_BGL_BmpWinOn( win );

	tl->old = tl->view.this;

	return param;	
}


//--------------------------------------------------------------
/**
 * @brief	可視範囲モードの値設定
 *
 * @param	tl	
 * @param	val	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_ViewMode_ValueSet( TOUCH_LIST* tl, s16 val )
{
	s16 pos  = 0;
	int move = 0xFF;
	
	if ( val < 0 ){
		///< ↑
		if ( tl->view.this > 0 ){
			 tl->view.this--;
			move = 0;
			Snd_SePlay( eBR_SND_SCROLL );
		}
	}
	else if ( val > 0 ){
		///< ↓
		if ( tl->view.this < tl->max - 1 ){
			 tl->view.this++;
			move = 1;
			Snd_SePlay( eBR_SND_SCROLL );
		}
	}
	
	switch ( move ){
	case 0:
		if ( tl->view.this < tl->view.b_stoper ){
			pos = tl->view.this;
		}
		else if ( tl->view.this >= ( tl->max - tl->view.b_stoper ) ){
			pos = tl->view.b_stoper + 1;
			if ( tl->view.this >= ( tl->max - tl->view.b_stoper ) ){
				pos += ( tl->view.this - ( tl->max - tl->view.b_stoper ) );
			}
		}
		else {
			pos = tl->view.b_stoper;
			if ( tl->view.this <= tl->view.b_stoper ){
				pos += ( tl->view.this - tl->view.b_stoper );
			}
		}
		break;

	case 1:
		if ( tl->view.this <= tl->view.t_stoper ){
			pos = tl->view.this;
		}
		else {
			pos = tl->view.t_stoper;			 
			if ( tl->view.this >= ( tl->max - tl->view.t_stoper ) ){
				pos += ( tl->view.this - ( tl->max - tl->view.t_stoper - 1 ) );
			}
		}
		break;
	}

	if ( pos < 0 ){ GF_ASSERT( 0 ); }
		 
	if ( move != 0xFF ){
		tl->view.l_pos = pos;
//		OS_Printf( "pos   = %d\n", pos );
//		OS_Printf( "l_pos = %d\n", tl->view.l_pos );
//		OS_Printf( "this  = %d\n", tl->view.this );
//		OS_Printf( "max   = %d\n", tl->max );
	}
	{
		s16 s = 0;
		if ( tl->view.l_pos >= tl->view.t_stoper ){
			s = tl->view.this - tl->view.t_stoper;
			if ( tl->view.this >= ( tl->max - ( tl->view.t_stoper + 1 ) ) ){
				s = tl->max - tl->lim;
			}
		}
		tl->now = s;
//		OS_Printf( "now   = %d\n", pos );
//		OS_Printf( "l_pos = %d\n", tl->view.l_pos );
		TouchList_ViewModeCursorEnableCheck( tl );
	}
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	tl	
 * @param	val	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_ValueSet( TOUCH_LIST* tl, s16 val )
{
	s16 old = tl->now;
	
	tl->now += val;
	
	if ( tl->now < 0 ){
		 tl->now = 0;
	}	
	if ( tl->lim == tl->max ){
		 tl->now = 0;
	}	
	if ( tl->now + tl->lim > tl->max ){
		 tl->now = tl->max - tl->lim;
	}
	
	if ( old != tl->now ){
		 Snd_SePlay( eBR_SND_SCROLL );
	}
	
	TouchList_CursorEnableCheck( tl );
}

//--------------------------------------------------------------
/**
 * @brief	リストの終了
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchList_Exit( TOUCH_LIST* wk )
{
	GF_BGL_BmpWinOff( &wk->win );
	GF_BGL_BmpWinDel( &wk->win );
	
//	memset( wk, 0, sizeof( TOUCH_LIST ) );
}

void TouchList_WorkClear( TOUCH_LIST* wk )
{
	memset( wk, 0, sizeof( TOUCH_LIST ) );	
}

// =============================================================================
//
//
//	■リスト用カーソル
//
//
// =============================================================================
void TouchList_CursorAdd( TOUCH_LIST* tl, BR_WORK* br_wk, int d_area )
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	ARCHANDLE*		 hdl = br_wk->sys.p_handle;
	GF_BGL_INI*		 bgl = br_wk->sys.bgl;
	CATS_SYS_PTR	 csp = br_wk->sys.csp;
	CATS_RES_PTR	 crp = br_wk->sys.crp;
	PALETTE_FADE_PTR pfd = br_wk->sys.pfd;
	
	CATS_LoadResourceCharArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_cursor_NCGR, FALSE, d_area,  eID_LIST_CUR );
	CATS_LoadResourceCellArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_cursor_NCER, FALSE, eID_LIST_CUR );
	CATS_LoadResourceCellAnmArcH( csp, crp, hdl, NARC_batt_rec_gra_batt_rec_cursor_NANR, FALSE, eID_LIST_CUR );
	
	coap.x		= 128;
	coap.y		=  96;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 1;
	coap.pal	= 0;
	coap.d_area = d_area;
	coap.bg_pri = eBR_BG0_PRI;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	coap.id[0]		= eID_LIST_CUR;
	coap.id[2]		= eID_LIST_CUR;
	coap.id[3]		= eID_LIST_CUR;
	if ( d_area == NNS_G2D_VRAM_TYPE_2DMAIN ){
		coap.id[1]		= eID_OAM_SIDE_BAR_M;
	}
	else {
		coap.id[1]		= eID_OAM_SIDE_BAR_S;
	}
	
	tl->cap[ 0 ] = CATS_ObjectAdd_S( br_wk->sys.csp, br_wk->sys.crp, &coap );
	tl->cap[ 1 ] = CATS_ObjectAdd_S( br_wk->sys.csp, br_wk->sys.crp, &coap );
	CATS_ObjectPaletteSetCap( tl->cap[ 0 ], 2 );
	CATS_ObjectPaletteSetCap( tl->cap[ 1 ], 2 );
	CATS_ObjectAnimeSeqSetCap( tl->cap[ 0 ], 0 );
	CATS_ObjectAnimeSeqSetCap( tl->cap[ 1 ], 0 );
	CATS_ObjectUpdateCap( tl->cap[ 0 ] );
	CATS_ObjectUpdateCap( tl->cap[ 1 ] );
	CATS_ObjectFlipSetCap( tl->cap[ 1 ], CLACT_FLIP_V );
	
	if ( d_area == CATS_D_AREA_MAIN ){
		CATS_ObjectPosSetCap( tl->cap[ 0 ], 256 / 2,  24 );
		CATS_ObjectPosSetCap( tl->cap[ 1 ], 256 / 2, 120 );
	}
	else {
		CATS_ObjectPosSetCap( tl->cap[ 0 ], 256 / 2,  24 + 64 );
		CATS_ObjectPosSetCap( tl->cap[ 1 ], 256 / 2, 120 + 64 );
	}
	TouchList_CursorEnableCheck( tl );
}

void TouchList_CursorPosSet( TOUCH_LIST* tl, s16 ty, s16 by )
{
	CATS_ObjectPosSetCap( tl->cap[ 0 ], 256 / 2, ty );
	CATS_ObjectPosSetCap( tl->cap[ 1 ], 256 / 2, by );	
}

void TouchList_CursorEnableCheck( TOUCH_LIST* tl )
{
	CATS_ObjectEnableCap( tl->cap[ 0 ], TRUE );
	CATS_ObjectEnableCap( tl->cap[ 1 ], TRUE );
	if ( tl->now == 0 ){
		CATS_ObjectEnableCap( tl->cap[ 0 ], FALSE );
	}
	if ( tl->now + tl->lim >= tl->max ){
		CATS_ObjectEnableCap( tl->cap[ 1 ], FALSE );
	}
	CATS_ObjectUpdateCapEx( tl->cap[ 0 ] );
	CATS_ObjectUpdateCapEx( tl->cap[ 1 ] );
}

void TouchList_ViewModeCursorEnableCheck( TOUCH_LIST* tl )
{
	CATS_ObjectEnableCap( tl->cap[ 0 ], TRUE );
	CATS_ObjectEnableCap( tl->cap[ 1 ], TRUE );
	if ( tl->now == 0 ){
		CATS_ObjectEnableCap( tl->cap[ 0 ], FALSE );
	}
//	if ( tl->now >= tl->lim ){
	if ( tl->now + tl->lim >= tl->max ){
		CATS_ObjectEnableCap( tl->cap[ 1 ], FALSE );
	}
	if ( tl->lim >= tl->max ){
		CATS_ObjectEnableCap( tl->cap[ 0 ], FALSE );
		CATS_ObjectEnableCap( tl->cap[ 1 ], FALSE );
	}
	CATS_ObjectUpdateCapEx( tl->cap[ 0 ] );
	CATS_ObjectUpdateCapEx( tl->cap[ 1 ] );
}

void TouchList_CursorDel( TOUCH_LIST* tl, BR_WORK* wk )
{
	CATS_FreeResourceChar( wk->sys.crp, eID_LIST_CUR );
	CATS_FreeResourceCell( wk->sys.crp, eID_LIST_CUR );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_LIST_CUR );
	CATS_ActorPointerDelete_S( tl->cap[ 0 ] );
	CATS_ActorPointerDelete_S( tl->cap[ 1 ] );
}

// =============================================================================
//
//
//	■スライダー部分
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	スライダーの登録
 *
 * @param	wk	
 * @param	br_wk	
 * @param	d_area	
 *
 * @retval	static CATS_ACT_PTR	
 *
 */
//--------------------------------------------------------------
static CATS_ACT_PTR Slider_Add( BR_SLIDER* wk, BR_WORK* br_wk, int d_area, int anmno )
{
	CATS_ACT_PTR cap;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	coap.x		= 128;
	coap.y		=  96;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 1;
	coap.pal	= 0;
	coap.d_area = d_area;
	coap.bg_pri = eBR_BG0_PRI;
	coap.vram_trans = 0;
	
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	coap.id[0]		= eID_OAM_EX_TAG;
	coap.id[2]		= eID_OAM_EX_TAG;
	coap.id[3]		= eID_OAM_EX_TAG;
	if ( d_area == NNS_G2D_VRAM_TYPE_2DMAIN ){
		coap.id[1]		= eID_OAM_SIDE_BAR_M;
	}
	else {
		coap.id[1]		= eID_OAM_SIDE_BAR_S;
	}	
	cap = CATS_ObjectAdd_S( br_wk->sys.csp, br_wk->sys.crp, &coap );	
	CATS_ObjectPaletteSetCap( cap, 2 );
	CATS_ObjectAnimeSeqSetCap( cap, anmno );
	CATS_ObjectUpdateCap( cap );
	
	return cap;	
}

//--------------------------------------------------------------
/**
 * @brief	スライダーの削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Slider_Del( BR_SLIDER* wk )
{
	CATS_ActorPointerDelete_S( wk->cap[ 0 ] );
	CATS_ActorPointerDelete_S( wk->cap[ 1 ] );
}

//--------------------------------------------------------------
/**
 * @brief	スライダーの非表示
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Slider_Enable( BR_SLIDER* wk, int flag )
{
	CATS_ObjectEnableCap( wk->cap[ 0 ], flag );
	CATS_ObjectEnableCap( wk->cap[ 1 ], flag );
}

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Slider_Init( BR_SLIDER* wk, BR_WORK* br_wk )
{	
	Slider_InitEx( wk, br_wk, eTAG_SLIDER );
}

void Slider_InitEx( BR_SLIDER* wk, BR_WORK* br_wk, int anmno )
{
	wk->cap[ 0 ] = Slider_Add( wk, br_wk, CATS_D_AREA_SUB, anmno );
	wk->cap[ 1 ] = Slider_Add( wk, br_wk, CATS_D_AREA_SUB, anmno );

	CATS_ObjectPosSetCap( wk->cap[ 0 ],   0 + 24, 72 + 64 );
	CATS_ObjectPosSetCap( wk->cap[ 1 ], 256 - 24, 72 + 64 );
}

void Slider_WorkClear( BR_SLIDER* wk )
{
	memset( wk, 0, sizeof( BR_SLIDER ) );	
}

//--------------------------------------------------------------
/**
 * @brief	スライダー終了処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Slider_Exit( BR_SLIDER* wk )
{
	Slider_Del( wk );
//	memset( wk, 0, sizeof( BR_SLIDER ) );
}

//--------------------------------------------------------------
/**
 * @brief	データの取得
 *
 * @param	wk	
 * @param	check	
 *
 * @retval	s16	
 *
 */
//--------------------------------------------------------------
void Slider_Value_CheckSet( BR_SLIDER* wk, u32 y )
{
	s32 check = y - wk->old_y;
	
//	if ( wk->value < 0 && check > 0 ){ wk->value = 0; }
//	if ( wk->value > 0 && check < 0 ){ wk->value = 0; }	
	if ( ( check * check ) >= ( BB_SLIDER_MOVE_VALUE * BB_SLIDER_MOVE_VALUE ) ) {		///< 一定量の動作があった場合
		
		if ( check < 0 ){
			wk->value = -1;						///< 移動値として設定する
		}
		else {
			wk->value = +1;
		}
		wk->old_y = y;							///< 現在地を基点に置きなおし
	}
	else {
		wk->value = 0;
	}	
//	if ( wk->value != 0 ){ OS_Printf( "%d\n",wk->value ); }
}

///< スライダーの当たり判定
static const TP_HIT_TBL slider_hit_tbl[] = {
	{ 8, 128,   0 +  8,   0 + 32 },
	{ 8, 128, 256 - 32, 256 -  8 },
};


//--------------------------------------------------------------
/**
 * @brief	アニメ
 *
 * @param	wk	
 * @param	lim	
 * @param	max	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Slider_AnimeCheck( BR_SLIDER* wk, TOUCH_LIST* tl )
{
	if ( tl->bView ){
	//	OS_Printf( "ビューモード\n" );
		if ( tl->max < 2 ){
			wk->bAnime = FALSE;
		}
		else {
			wk->bAnime = TRUE;
		}		
	}
	else {
	//	OS_Printf( "通常リストモード\n" );
		if ( tl->lim >= tl->max ){
			wk->bAnime = FALSE;
		}
		else {
			wk->bAnime = TRUE;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	スライダーの当たり判定
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void Slider_HitCheck_Main( BR_SLIDER* wk )
{
	u32  x, y;
	s16  check;
	BOOL bCont;
	BOOL bSliderHit = FALSE;
	
	bCont = GF_TP_GetPointCont( &x, &y );

	if ( wk->bAnime ){
		CATS_ObjectUpdateCapEx( wk->cap[ 0 ] );
		CATS_ObjectUpdateCapEx( wk->cap[ 1 ] );
	}

	if ( wk->bHit == FALSE && bCont == FALSE ){			///< 触った事ない ＆ 触ってない
	
		wk->bHit  = FALSE;
		wk->frame = 0;
		wk->old_x = 0;
		wk->old_y = 0;			
		wk->value = 0;
		return;
	}
	
	if ( GF_TP_SingleHitCont( &slider_hit_tbl[ 0 ] )		///< スライダーとの当たり判定
	||	 GF_TP_SingleHitCont( &slider_hit_tbl[ 1 ] ) ){
		bSliderHit = TRUE;
	}
	
	if ( bSliderHit ){
		if ( !wk->bHit ){									///< フレーム0なら初期化
			wk->bHit  = TRUE;
			wk->old_x = x;
			wk->old_y = y;
			wk->frame = BB_SLIDER_HIT_FRAME;
//			OS_Printf( "すらいだー触った\n");
		}
		else {
			wk->frame--;
			Slider_Value_CheckSet( wk, y );			///< 移動量の取得
//			OS_Printf( "すらいだー触ってる\n");
		}
	}
	else {
		if ( wk->bHit ){
		//	Slider_Value_CheckSet( wk, y );			///< 移動量の取得			
			wk->bHit  = FALSE;
			wk->frame = 0;
			wk->old_x = 0;
			wk->old_y = 0;
//			OS_Printf( "すらいだーはなした\n");
		}
	}
}



//--------------------------------------------------------------
/**
 * @brief	スコア加算
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_AddScore( BR_WORK* wk, u32 id )
{
	RECORD* rec;	
	
	rec = SaveData_GetRecord( wk->save );
	
	RECORD_Score_Add( rec, id );
}

//--------------------------------------------------------------
/**
 * @brief	brs を生成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void RecData_To_brs( BR_WORK* wk )
{
//	VIDEO_SEARCH_WORK* vwk = wk->sub_work;
	LOAD_RESULT result;
	
	BattleRec_Init( wk->save, HEAPID_BR_MANAGER, &result );
	{
//		GDS_PROFILE_PTR			profile	= ( GDS_PROFILE_PTR )( &vwk->rec_data->profile );
//		BATTLE_REC_HEADER_PTR	head	= ( BATTLE_REC_HEADER_PTR )( &vwk->rec_data->head );
//		BATTLE_REC_WORK_PTR		rec		= ( BATTLE_REC_WORK_PTR )( &vwk->rec_data->rec );
		GDS_PROFILE_PTR			profile	= ( GDS_PROFILE_PTR )( &wk->recv_data.rec_data_tmp->profile );
		BATTLE_REC_HEADER_PTR	head	= ( BATTLE_REC_HEADER_PTR )( &wk->recv_data.rec_data_tmp->head );
		BATTLE_REC_WORK_PTR		rec		= ( BATTLE_REC_WORK_PTR )( &wk->recv_data.rec_data_tmp->rec );
		
		BattleRec_DataSet( profile, head, rec, NULL, wk->save );
	}
//	BattleRec_Load( wk->save, HEAPID_BASE_APP, &result, NULL, 0 );	
//	OS_Printf( "brs_load result = %d\n", result );
}


//--------------------------------------------------------------
/**
 * @brief	DISP切り替えTCB
 *
 * @param	tcb	
 * @param	work
 * @retval	none
 *
 */
//--------------------------------------------------------------
static void DispChange_TCB( TCB_PTR tcb, void* work )
{
	BR_DISP_SW* wk = work;
	
	BR_ChangeDisplay( wk->disp_sw );
	WirelessIconOAM_Trans();
	wk->bChange = TRUE;
	
	TCB_Delete( tcb );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	mode	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ChangeDisplayVIntr( BR_WORK* wk, int mode )
{
	wk->disp.disp_sw = mode;
	wk->disp.bChange = FALSE;
	
	VIntrTCB_Add( DispChange_TCB, &wk->disp, eVINTR_TCB_PRI_CHANGE );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_IsChangeDisplay( BR_WORK* wk )
{
	return wk->disp.bChange;
}


//--------------------------------------------------------------
/**
 * @brief	描画
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Draw_60fps_TCB( TCB_PTR tcb, void* work )
{
	BR_WORK* wk = work;
	
	/// 描画フラグ チェック
	/// 多重起動フラグ チェック
	if ( wk->sys.crp == NULL ){ return; }
	
	if ( IsCATS_SystemActive( wk ) == TRUE ){
	//	OS_Printf( " CATS操作中に来た \n" );
		return;
	}
	
	if ( wk->sys.bDraw == TRUE ){ 
	//	OS_Printf( " 期間中に来た \n" );
		return;
	}
	wk->sys.bDraw = TRUE;

	SoftSpriteTextureTrans( wk->sys.ssm_p );

	CATS_Draw( wk->sys.crp );
//	OS_Printf( " Vカウンタ　%d\n ", GX_GetVCount() );
	CATS_RenderOamTrans();
//	PaletteFadeTrans( wk->sys.pfd );

	wk->sys.bDraw = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	描画TCB
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_DrawTCB_Add( BR_WORK* wk )
{
	wk->view_tcb = VIntrTCB_Add( Draw_60fps_TCB, wk, eVINTR_TCB_PRI_BAR );
}

//--------------------------------------------------------------
/**
 * @brief	セーブする brs から
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL RecData_Save( BR_WORK* wk, int no, u8 secure )
{
	SAVE_RESULT result;
	
//	brs->head.secure
#if 0
	if ( wk->ex_param3 == TRUE ){
		///< 再生していない
		result = BattleRec_GDS_RecvData_Save( wk->save, no, FALSE, &wk->save_work1, &wk->save_work2 );
	}
	else {
		///< 再生したことがある
		result = BattleRec_GDS_RecvData_Save( wk->save, no, TRUE,  &wk->save_work1, &wk->save_work2 );
	}
#endif
	result = BattleRec_GDS_RecvData_Save( wk->save, no, secure, &wk->save_work1, &wk->save_work2 );
		
	if ( result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST ){
		return FALSE;
	}
	
	return TRUE;
}


//--------------------------------------------------------------
/**
 * @brief	フラグ設定
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void RecData_RecStopFlagSet( BR_WORK* wk, BOOL flag )
{
	OS_Printf( " ビデオ落としてきて、まだ見てねーフラグ = ON\n" );
	/// TRUE = ON = みてねー
	wk->ex_param3 = flag;
}

//--------------------------------------------------------------
/**
 * @brief	30件のデータ取得共通レスポンスコールバック
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_BattleVideoSearch(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	BR_WORK* wk = work;	
	BR_RECV_DATA* recv_data = &wk->recv_data;
	
	OS_Printf( "録画データ３０件受信\n");
	if( error_info->occ == TRUE ){
		OS_Printf( "エラー！\n");
	}
	else {
		OS_Printf( "正常に取得！\n");
		recv_data->outline_data_num = GDS_RAP_RESPONSE_BattleVideoSearch_Download_RecvPtr_Set( BR_GDSRAP_WorkGet( wk ), recv_data->outline_data_tmp, 30 );
		
		{
			int i;
			
			for ( i = 0; i < recv_data->outline_data_num; i++ ){
				GF_ASSERT_MSG( wk->recv_data.outline_data_tmp[ i ] != NULL, "%d番目に空っぽのデータを受信したよ\n",i );
				MI_CpuCopy8( wk->recv_data.outline_data_tmp[ i ], &wk->recv_data.outline_data[ i ], sizeof( GT_BATTLE_REC_OUTLINE_RECV ) );
				wk->recv_data.outline_secure_flag[ i ] = wk->recv_data.outline_data_tmp[ i ]->head.secure;
			//	OS_Printf( " secure flag = %d\n", wk->recv_data.outline_secure_flag[ i ] );
			}
			
		}
	}
	
	OS_Printf( "件数 = %d\n",recv_data->outline_data_num);
}

//--------------------------------------------------------------
/**
 * @brief	データの読み込み
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SaveData_GppHead_Load( BR_WORK* wk )
{
	int i;
	LOAD_RESULT result;	
	for ( i = 0; i < 4; i++ ){		
		BattleRec_Load( wk->save, HEAPID_BR, &result, NULL, i );		
		switch ( result ){
		case RECLOAD_RESULT_NULL:
			 wk->br_head[ i ] = NULL;
			 wk->br_gpp[ i ]  = NULL;
			 if ( i == 0 ){
				wk->br_gpp[ i ] = GDS_Profile_AllocMemory( HEAPID_BR );
				GDS_Profile_MyDataSet( wk->br_gpp[ i ], wk->save );
			 }
			break;
		case RECLOAD_RESULT_NG:
			 wk->br_head[ i ] = NULL;
			 wk->br_gpp[ i ]  = NULL;
			 if ( i == 0 ){
				wk->br_gpp[ i ] = GDS_Profile_AllocMemory( HEAPID_BR );
				GDS_Profile_MyDataSet( wk->br_gpp[ i ], wk->save );
			 }
			break;
		case RECLOAD_RESULT_ERROR:
			 wk->br_head[ i ] = NULL;
			 wk->br_gpp[ i ]  = NULL;
			 if ( i == 0 ){
				wk->br_gpp[ i ] = GDS_Profile_AllocMemory( HEAPID_BR );
				GDS_Profile_MyDataSet( wk->br_gpp[ i ], wk->save );
			 }
			break;
		case RECLOAD_RESULT_OK:
			 wk->br_head[ i ] = BattleRec_HeaderAllocCopy( HEAPID_BR );			
			 if ( i == 0 ){
				wk->br_gpp[ i ] = GDS_Profile_AllocMemory( HEAPID_BR );
				GDS_Profile_MyDataSet( wk->br_gpp[ i ], wk->save );
			 }
			 else {				
				wk->br_gpp[ i ] = BattleRec_GDSProfileAllocCopy( HEAPID_BR );
			 }
			break;			
		}				
		if ( BattleRec_DataExistCheck() == TRUE ){
			BattleRec_Exit();	
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SaveData_GppHead_Delete( BR_WORK* wk )
{
	int i;

	for ( i = 0; i < 4; i++ ){
		
		if ( wk->br_head[ i ] != NULL ){
			sys_FreeMemoryEz( wk->br_head[ i ] );
			wk->br_head[ i ] = NULL;
		}
		
		if ( wk->br_gpp[ i ] != NULL ){
			sys_FreeMemoryEz( wk->br_gpp[ i ] );
			wk->br_gpp[ i ] = NULL;
		}
	}	
}


// =============================================================================
//
//
//	■ポケモンアイコン
//
//
// =============================================================================
///< キャラ以外
void PokemonIcon_ResLoad( BR_WORK* wk )
{
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	
	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp,	ARC_POKEICON, PokeIconPalArcIndexGet(), 0, POKEICON_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON );
	CATS_LoadResourceCellArc( csp, crp, ARC_POKEICON, PokeIcon64kCellArcIndexGet(), 0, eID_OAM_POKE_ICON );
	CATS_LoadResourceCellAnmArc( csp, crp, ARC_POKEICON, PokeIcon64kCellAnmArcIndexGet(), 0, eID_OAM_POKE_ICON );
}

void PokemonIcon_ResFree( BR_WORK* wk )
{
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_POKE_ICON );
}

CATS_ACT_PTR PokemonIcon_Add( BR_WORK* wk, int id, int monsno, int form, int egg )
{
	CATS_ACT_PTR cap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;
	int palno;
	
	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
		
	if ( monsno == 0 ){ return NULL; }
	
	CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_POKEICON, PokeIconCgxArcIndexGetByMonsNumber( monsno, egg, form ), 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON + id );
		
	coap.x		= 0;
	coap.y		= 0;
	coap.z		= 0;
	coap.anm	= 0;
	coap.pri	= 0;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_MAIN;
	coap.bg_pri = 0;
	coap.vram_trans = 0;

	coap.id[ 0 ] = eID_OAM_POKE_ICON + id;
	coap.id[ 1 ] = eID_OAM_POKE_ICON;
	coap.id[ 2 ] = eID_OAM_POKE_ICON;
	coap.id[ 3 ] = eID_OAM_POKE_ICON;
	coap.id[ 4 ] = CLACT_U_HEADER_DATA_NONE;
	coap.id[ 5 ] = CLACT_U_HEADER_DATA_NONE;
	
	cap = CATS_ObjectAdd_S( csp, crp, &coap );
		
	palno  = PokeIconPalNumGet( monsno, form, egg );
	
	CATS_ObjectPaletteOffsetSetCap( cap, eOAM_PAL_POKE_ICON + palno );
	
	CATS_ObjectAnimeSeqSetCap( cap, POKEICON_ANM_HPMAX );
	
	CATS_ObjectUpdateCap( cap );

	return cap;	
}


//--------------------------------------------------------------
/**
 * @brief	ポケモンのアーカイブを開く
 *
 * @param	heap	
 *
 * @retval	ARCHANDLE*	
 *
 */
//--------------------------------------------------------------
ARCHANDLE* PokemonGraphic_ArcOpen( int heap )
{
	ARCHANDLE* hdl;
	
	hdl = ArchiveDataHandleOpen( ARC_POKEICON, HEAPID_BR );
	
	return hdl;
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンのアーカイブ閉じる
 *
 * @param	hdl	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void PokemonGraphic_ArcClose( ARCHANDLE* hdl )
{
	ArchiveDataHandleClose( hdl );	
}

CATS_ACT_PTR PokemonIcon_AddHDL( BR_WORK* wk, ARCHANDLE* hdl, int id, int monsno, int form, int egg )
{
	CATS_ACT_PTR cap;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;
	int palno;
	
	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
		
	if ( monsno == 0 ){ return NULL; }
	
	CATS_LoadResourceCharArcModeAdjustAreaCont_Handle( csp, crp, hdl, PokeIconCgxArcIndexGetByMonsNumber( monsno, egg, form ), 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON + id );
		
	coap.x		= 0;
	coap.y		= -48;
	coap.z		= 0;
	coap.anm	= 0;
	coap.pri	= 0;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_MAIN;
	coap.bg_pri = 0;
	coap.vram_trans = 0;

	coap.id[ 0 ] = eID_OAM_POKE_ICON + id;
	coap.id[ 1 ] = eID_OAM_POKE_ICON;
	coap.id[ 2 ] = eID_OAM_POKE_ICON;
	coap.id[ 3 ] = eID_OAM_POKE_ICON;
	coap.id[ 4 ] = CLACT_U_HEADER_DATA_NONE;
	coap.id[ 5 ] = CLACT_U_HEADER_DATA_NONE;
	
	cap = CATS_ObjectAdd_S( csp, crp, &coap );
		
	palno  = PokeIconPalNumGet( monsno, form, egg );
	
	CATS_ObjectPaletteOffsetSetCap( cap, eOAM_PAL_POKE_ICON + palno );
	
	CATS_ObjectAnimeSeqSetCap( cap, POKEICON_ANM_HPMAX );
	
	CATS_ObjectUpdateCap( cap );

	return cap;	
}

void PokemonIcon_Del( BR_WORK* wk, int id, CATS_ACT_PTR cap )
{	
	int i;

	if ( cap == NULL){ return; }
		
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON + id );
	CATS_ActorPointerDelete_S( cap );
}


void PokemonIcon_DelActor( BR_WORK* wk, int id, CATS_ACT_PTR cap )
{	
	int i;

	if ( cap == NULL){ return; }
		
	CATS_ActorPointerDelete_S( cap );
}

// =============================================================================
//
//
//	■ビデオ情報描画システム
//
//
// =============================================================================
static void video_hero_add( VIDEO_WORK* vwk, BR_WORK* wk )
{
	int sex;
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	int	ncl = NARC_wifi2dchar_hero_NCLR;
	int ncg = NARC_wifi2dchar_hero_NCGR;
	int	nce = NARC_wifi2dchar_npc_NCER;
	int nca = NARC_wifi2dchar_npc_NANR;
	
	sex = GDS_Profile_GetSex( vwk->gpp );

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	if ( wk->ex_param1 == LOADDATA_MYREC ){
		
		if ( sex == PM_FEMALE ){
			ncl = NARC_wifi2dchar_heroine_NCLR;
			ncg = NARC_wifi2dchar_heroine_NCGR;
		}
	}
	else {
		int view = GDS_Profile_GetTrainerView( vwk->gpp );
		
	//	OS_Printf( " view = %d\n", view );
		
		if ( view >= BR_UNION_CHAR_MAX ){ view = 0; }
		
		ncl = WiFi_TR_DataGet( view, 2 );
		ncg = WiFi_TR_DataGet( view, 3 );
	}
	
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp, ARC_WIFI2DCHAR, ncl, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );
	CATS_LoadResourceCellArc( csp, crp, ARC_WIFI2DCHAR, nce, 0, eID_OAM_HERO_ICON );
	CATS_LoadResourceCellAnmArc( csp, crp, ARC_WIFI2DCHAR, nca, 0, eID_OAM_HERO_ICON );
	CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_WIFI2DCHAR, ncg, 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_HERO_ICON );

	coap.x		= 40;
	coap.y		= 56+4;
	coap.z		= 0;		
	coap.anm	= 0;
	coap.pri	= 0;
	coap.pal	= 0;
	coap.d_area = CATS_D_AREA_MAIN;
	coap.bg_pri = 0;
	coap.vram_trans = 0;
			
	coap.id[0] = eID_OAM_HERO_ICON;
	coap.id[1] = eID_OAM_HERO_ICON;
	coap.id[2] = eID_OAM_HERO_ICON;
	coap.id[3] = eID_OAM_HERO_ICON;
	coap.id[4] = CLACT_U_HEADER_DATA_NONE;
	coap.id[5] = CLACT_U_HEADER_DATA_NONE;
	
	vwk->cap_hero = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
	CATS_ObjectAnimeSeqSetCap( vwk->cap_hero, eHERO_BOTTOM );
	CATS_ObjectUpdateCap( vwk->cap_hero );
}

static void video_win_add( VIDEO_WORK* vwk, BR_WORK* wk )
{
	int i;
	int ofs;
	GF_BGL_BMPWIN* win;
	STRBUF*	str;

	{
		s16 win_p[][4] = {
			{  4, 4, 24, 2 },
			{  4, 6, 24, 4 },
			{  4,11,  5, 2 },
			{  4,13,  5, 2 },
			{  4,21, 24, 2 },
		};
		int mes_id[] = {
			msg_20, msg_20, 
			msg_20,	msg_11_2,
			msg_20,	msg_20,	
		};
		
		ofs = 1;
		for ( i = 0; i < 5; i++ ){
			
			if ( win_p[i][0] == 0xFF ){ break; }
			
			win = &vwk->win[ i ];				
			str = MSGMAN_AllocString( wk->sys.man, mes_id[ i ] );				
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M,
							  win_p[i][0], win_p[i][1], win_p[i][2], win_p[i][3], eBG_PAL_FONT, ofs );				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, str, BR_print_x_Get( win, str ), 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_BGL_BmpWinOnVReq( win );
			ofs += ( win_p[i][2] * win_p[i][3] );
			STRBUF_Delete( str );
		}
	//	OS_Printf( "win add char end %d\n", ofs );
	}
	
	///< 文字描画
	{
		STRBUF* 	str1;
		STRBUF* 	str2;
		STRBUF*		tmp;
		WORDSET*	wset;
		GF_BGL_BMPWIN* win;
		GDS_PROFILE_PTR	gpp;
		
		gpp = vwk->gpp;
		wset = BR_WORDSET_Create( HEAPID_BR );
		
		///< 見出し
		win = &vwk->win[ 0 ];
		str1 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );			///< 名前
		BR_ErrorStrChange( wk, str1 );
		str2 = MSGMAN_AllocString( wk->sys.man, msg_09 );			///< ～ の記録
		tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ

		WORDSET_RegisterWord( wset, 0, str1, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, tmp, str2 );
	
		GF_BGL_BmpWinDataFill( win, 0x00 );
		GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
		GF_BGL_BmpWinOnVReq( win );

		STRBUF_Delete( str1 );
		STRBUF_Delete( str2 );
		STRBUF_Delete( tmp );
		WORDSET_ClearAllBuffer( wset );
		
		///< 施設名
		{
			int no = RecHeader_ParamGet( vwk->br_head, RECHEAD_IDX_MODE, 0 );
			win = &vwk->win[ 1 ];
			tmp = MSGMAN_AllocString( wk->sys.man, msg_rule_000 + no );	///< ｘｘｘせんめ
				
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_BGL_BmpWinOnVReq( win );

			STRBUF_Delete( tmp );
			WORDSET_ClearAllBuffer( wset );
		}
		
		
		///< 連勝
		{
			int num = RecHeader_ParamGet( vwk->br_head, RECHEAD_IDX_COUNTER, 0);
			
			if ( num != 0 ){
				win = &vwk->win[ 2 ];
				str1 = STRBUF_Create( 255, HEAPID_BR );						///< 数字
				str2 = MSGMAN_AllocString( wk->sys.man, msg_11 );			///< ｘｘｘせんめ
				tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ
				
				STRBUF_SetNumber( str1, num, 4, NUMBER_DISPTYPE_LEFT, NUMBER_CODETYPE_DEFAULT);
					
				WORDSET_RegisterWord( wset, 0, str1, 0, TRUE, PM_LANG );	
				WORDSET_ExpandStr( wset, tmp, str2 );
			
				GF_BGL_BmpWinDataFill( win, 0x00 );
				GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
				GF_BGL_BmpWinOnVReq( win );

				STRBUF_Delete( str1 );
				STRBUF_Delete( str2 );
				STRBUF_Delete( tmp );
				WORDSET_ClearAllBuffer( wset );
			}
			else {
				win = &vwk->win[ 3 ];
				GF_BGL_BmpWinDataFill( win, 0x00 );
				GF_BGL_BmpWinOnVReq( win );
			}
		}
		
		
		///< データナンバー	
		{
			u64 data = RecHeader_ParamGet( vwk->br_head, RECHEAD_IDX_DATA_NUMBER, 0 );
			u64	dtmp1 = data;
			u32	dtmp2[ 3 ];
			STRBUF* num1 = STRBUF_Create( 255, HEAPID_BR );
			STRBUF* num2 = STRBUF_Create( 255, HEAPID_BR );
			STRBUF* num3 = STRBUF_Create( 255, HEAPID_BR );

			win = &vwk->win[ 4 ];

			dtmp2[ 0 ] = dtmp1 % 100000;	///< 3block
			dtmp1 /= 100000;
			dtmp2[ 1 ] = dtmp1 % 100000;	///< 2block
			dtmp1 /= 100000;
			dtmp2[ 2 ] = dtmp1;				///< 1block

			{
				int check1 = ( dtmp2[ 2 ] / 10 ) % 10;

				if ( ( check1 == 0 ) && ( data != 0 ) ){
					str2 = MSGMAN_AllocString( wk->sys.man, msg_12_2 );		///< ｘｘｘせんめ
					OS_Printf( "特殊なデータナンバー\n");
				}
				else {
					str2 = MSGMAN_AllocString( wk->sys.man, msg_12 );		///< ｘｘｘせんめ
				}
			}
			tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ
			
			STRBUF_SetNumber( num1, dtmp2[ 0 ], 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
			STRBUF_SetNumber( num2, dtmp2[ 1 ], 5, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
			STRBUF_SetNumber( num3, dtmp2[ 2 ], 2, NUMBER_DISPTYPE_ZERO, NUMBER_CODETYPE_DEFAULT);
						
			WORDSET_RegisterWord( wset, 2, num1, 0, TRUE, PM_LANG );
			WORDSET_RegisterWord( wset, 1, num2, 0, TRUE, PM_LANG );
			WORDSET_RegisterWord( wset, 0, num3, 0, TRUE, PM_LANG );	
			WORDSET_ExpandStr( wset, tmp, str2 );
		
			OS_Printf( " %2d - %5d - %5d \n", dtmp2[ 2 ], dtmp2[ 1 ], dtmp2[ 0 ] );
					
			GF_BGL_BmpWinDataFill( win, 0x00 );
			GF_STR_PrintColor( win, FONT_SYSTEM, tmp, BR_print_x_Get( win, tmp ), 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );
			GF_BGL_BmpWinOnVReq( win );

			STRBUF_Delete( num1 );
			STRBUF_Delete( num2 );
			STRBUF_Delete( num3 );
			STRBUF_Delete( str2 );
			STRBUF_Delete( tmp );
			WORDSET_ClearAllBuffer( wset );
		}
		
		WORDSET_Delete( wset );
	}	
}

static void video_win_del( VIDEO_WORK* vwk, BR_WORK* wk )
{
	int i;
	
	for ( i = 0; i < 5; i++ ){
		GF_BGL_BmpWinOff( &vwk->win[ i ] );
		GF_BGL_BmpWinDel( &vwk->win[ i ] );		
	}			
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	vwk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
BOOL VideoCheck2vs2_Head( BATTLE_REC_HEADER_PTR head )
{
	int	type;
	BOOL b2vs2 = FALSE; 
	
	type = RecHeader_ParamGet( head, RECHEAD_IDX_MODE, 0 );
	
	switch ( type ){
	case GT_BATTLE_MODE_TOWER_MULTI:
	case GT_BATTLE_MODE_FACTORY50_MULTI:
	case GT_BATTLE_MODE_FACTORY100_MULTI:
	case GT_BATTLE_MODE_STAGE_MULTI:
	case GT_BATTLE_MODE_CASTLE_MULTI:
	case GT_BATTLE_MODE_ROULETTE_MULTI:
	case GT_BATTLE_MODE_COLOSSEUM_MULTI:
		b2vs2 = TRUE;
		break;
	default:
		b2vs2 = FALSE;
		break;
	};
	
	return b2vs2;
}

//--------------------------------------------------------------
/**
 * @brief	2vs2で戦ったかのチェック
 *
 * @param	vwk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VideoCheck2vs2( VIDEO_WORK* vwk )
{	
	return VideoCheck2vs2_Head( vwk->br_head );
}

//--------------------------------------------------------------
/**
 * @brief	2vs2 BG 取得
 *
 * @param	vwk	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int ProfileBGGet( VIDEO_WORK* vwk )
{
	if ( VideoCheck2vs2( vwk ) == TRUE ){
		return NARC_batt_rec_gra_batt_rec_bg1ub_data_NSCR;
	}
	
	return NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR;
}

static void video_poke_add( VIDEO_WORK* vwk, BR_WORK* wk )
{	
	int i;
	int monsno;
	int form;
	int no = 0;
	
	TCATS_OBJECT_ADD_PARAM_S coap;
	int palno;
	int gra_id;
	int monsno_tbl[ 12 ];
	int form_tbl[ 12 ];
	int type;
	
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	
	s16 pos_tbl[][ 12 ][ 2 ] = {
		{
			{ 11*8, 13*8 },{ 14*8, 13*8 },{ 17*8, 13*8 },{ 20*8, 13*8 },{ 23*8, 13*8 },{ 26*8, 13*8 },
			{  3*8, 19*8 },{  6*8, 19*8 },{  9*8, 19*8 },{ 12*8, 19*8 },{ 15*8, 19*8 },{ 18*8, 19*8 },
		},
		{
			{  3*8, 17*8 },{  6*8, 17*8 },{  9*8, 17*8 },	///< 自分
			{  5*8, 20*8 },{  8*8, 20*8 },{ 11*8, 20*8 },	///< 自分
			{ 18*8, 17*8 },{ 21*8, 17*8 },{ 24*8, 17*8 },	///< 相手
			{ 20*8, 20*8 },{ 23*8, 20*8 },{ 26*8, 20*8 },	///< 相手
		},
	};

	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;

	//パレット
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp,
								  ARC_POKEICON, PokeIconPalArcIndexGet(), 0, POKEICON_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON );
	//セル
	{
		int arc = PokeIcon64kCellArcIndexGet();
		CATS_LoadResourceCellArc( csp, crp, ARC_POKEICON, arc, 0, eID_OAM_POKE_ICON );
	}
	//セルアニメ
	{
		int arc = PokeIcon64kCellAnmArcIndexGet();
		CATS_LoadResourceCellAnmArc( csp, crp, ARC_POKEICON, arc, 0, eID_OAM_POKE_ICON );
	}
	
	///< テーブルの作成
	{	
		int temp_no = 0;
		int temp_monsno = 0;
		int temp_form = 0;
		int add = 6;
		int si = 0;
		int ei = add;
		
		type = 0;

		///< 2vs2の場合
		if ( VideoCheck2vs2( vwk ) == TRUE ){
			add = 3;
			ei = 3;
			type = 1;
		}
		do {
			for ( i = si; i < ei; i++ ){
				monsno_tbl[ temp_no ] = 0xFF;
				
				temp_monsno = RecHeader_ParamGet( vwk->br_head, RECHEAD_IDX_MONSNO, i );
				temp_form	= RecHeader_ParamGet( vwk->br_head, RECHEAD_IDX_FORM_NO, i );

				if ( temp_monsno == 0 ){ continue; }
				
				monsno_tbl[ temp_no ] = temp_monsno;
				form_tbl[ temp_no]	  = temp_form;
				
				temp_no++;
			}
			for ( i = temp_no; i < ei; i++ ){	
				monsno_tbl[ temp_no ] = 0;
				form_tbl[ temp_no ]	  = 0;
				temp_no++;
			}		
			si += add;
			ei += add;
			
		} while ( ei <= 12 );
	}
	
	for ( i = 0; i < 12; i++ ){
				
		vwk->cap_poke[ i ] = NULL;
		
		monsno = monsno_tbl[ i ];
		
		if ( monsno == 0 ){ continue; }		///< たまご無し
		
		form = form_tbl[ i ];
		
		gra_id = PokeIconCgxArcIndexGetByMonsNumber( monsno, 0, form );
		
		CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_POKEICON, gra_id, 0,
						 							NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON + no );

		coap.x		= pos_tbl[ type ][ i ][ 0 ] + 12 - 4;
		coap.y		= pos_tbl[ type ][ i ][ 1 ] - 12;
		coap.z		= 0;		
		coap.anm	= 0;
		coap.pri	= 0;
		coap.pal	= 0;
		coap.d_area = CATS_D_AREA_MAIN;
		coap.bg_pri = 0;
		coap.vram_trans = 0;
				
		coap.id[0] = eID_OAM_POKE_ICON + no;
		coap.id[1] = eID_OAM_POKE_ICON;
		coap.id[2] = eID_OAM_POKE_ICON;
		coap.id[3] = eID_OAM_POKE_ICON;
		coap.id[4] = CLACT_U_HEADER_DATA_NONE;
		coap.id[5] = CLACT_U_HEADER_DATA_NONE;
		
		vwk->cap_poke[ no ] = CATS_ObjectAdd_S( wk->sys.csp, wk->sys.crp, &coap );
		
		palno  = PokeIconPalNumGet( monsno, form, 0 );
	//	CLACT_PaletteOffsetChgAddTransPlttNo( vwk->cap_poke[ no ]->act, palno );
		CATS_ObjectPaletteOffsetSetCap( vwk->cap_poke[ no ], eOAM_PAL_POKE_ICON + palno );
		CATS_ObjectAnimeSeqSetCap( vwk->cap_poke[ no ], POKEICON_ANM_HPMAX );
		CATS_ObjectUpdateCap( vwk->cap_poke[ no ] );
		CATS_ObjectPriSetCap( vwk->cap_poke[ no ], 12 - i );
		
	//	OS_Printf( "no = %2d\n", no );
		no++;	
	}
}

void VideoDataMake( VIDEO_WORK* vwk, BR_WORK* wk )
{
	video_win_add( vwk, wk );
	video_poke_add( vwk, wk );
//	video_hero_add( vwk, wk );
}

void VideoDataFree( VIDEO_WORK* vwk, BR_WORK* wk )
{
	int i;
	
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_POKE_ICON );	
	
	for ( i = 0; i < 12; i++ ){
		if ( vwk->cap_poke[ i ] ){
			CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON + i );
			CATS_ActorPointerDelete_S( vwk->cap_poke[ i ] );	
			
			vwk->cap_poke[ i ] = NULL;
		}
	}
	
#if 0	
	CATS_FreeResourceChar( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_HERO_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_HERO_ICON );
	
	CATS_ActorPointerDelete_S( vwk->cap_hero );
	vwk->cap_hero = NULL;
#endif	
	video_win_del( vwk, wk );
}

void VideoData_IconEnable( VIDEO_WORK* vwk, int flag )
{
	int i;
	
	for ( i = 0; i < 12; i++ ){
		if ( vwk->cap_poke[ i ] ){
			CATS_ObjectEnableCap( vwk->cap_poke[ i ], flag );
		}
	}
//	CATS_ObjectEnableCap( vwk->cap_hero, flag );
}


//--------------------------------------------------------------
/**
 * @brief	ウィンドウに対して、文字列をセンタリングする
 *
 * @param	win	
 * @param	str	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int BR_print_x_Get( GF_BGL_BMPWIN* win, STRBUF* str )
{
	int len = FontProc_GetPrintStrWidth( FONT_SYSTEM, str, 0 );
	int x	= ( GF_BGL_BmpWinGet_SizeX( win ) * 8 - len ) / 2;
	
	return x;
}


//--------------------------------------------------------------
/**
 * @brief	セーブ用のワークを初期化する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_SaveWork_Clear( BR_WORK* wk )
{
	wk->save_work1 = 0;
	wk->save_work2 = 0;
}


//--------------------------------------------------------------
/**
 * @brief	セーブデータを削除する
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_Executed_RecDelete( BR_WORK* wk )
{
#if 0
	BattleRec_SaveDataErase(wk->save, HEAPID_BR, wk->ex_param1);
#else
	MISC *misc = SaveData_GetMisc(wk->save);
	EX_CERTIFY_SAVE_KEY key, old_key;
	u8 flag;
	
	MISC_ExtraSaveKeyGet(misc, EXDATA_ID_BATTLE_REC_MINE + wk->ex_param1, &key, &old_key, &flag);
	MISC_ExtraSaveKeySet(misc, EXDATA_ID_BATTLE_REC_MINE + wk->ex_param1, 
		EX_CERTIFY_SAVE_KEY_NO_DATA, EX_CERTIFY_SAVE_KEY_NO_DATA,  flag);
#endif
	return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	ほぼ共通のプライオリティ
 *
 * @param	none	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void PrioritySet_Common( void )
{
	GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );			///< priority
	GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
	GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
	GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 1 );		
	
	GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 1 );
	GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
	GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );
	GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 1 );
}



//--------------------------------------------------------------
/**
 * @brief	カーソルを、ON OFF 含めて 使用可能にする
 *
 * @param	wk	
 * @param	cur	
 * @param	x	
 * @param	y	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void HormingCursor_Init( BR_WORK* wk, CURSOR_WORK* cur, s16 x, s16 y )
{
	if ( wk->sys.touch == INPUT_MODE_KEY ){	
		Cursor_Visible( cur, TRUE );
	}
	Cursor_Active( cur, TRUE );
	{
		Cursor_PosSetFix( cur, x, y );
		Cursor_PosSet( cur, x, y );
	}
}


//--------------------------------------------------------------
/**
 * @brief	カーソルの位置設定
 *
 * @param	cur	
 * @param	x	
 * @param	y	
 * @param	event	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CurPosSet( CURSOR_WORK* cur, s16 x, s16 y, u32 event )
{
	if ( event == 0xFF ){ return; }
	
	Cursor_PosSet_Check( cur, x, y );
}

//--------------------------------------------------------------
/**
 * @brief	ユニット数を取得
 *
 * @param	num	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int Number_to_Unit_Get( u64 num )
{
	int unit = 1;
	u64 dat  = num;
	
	while ( dat > 1 ){
		
		dat /= 10;
		unit++;
	}
	
	OS_Printf( "桁 = %d\n", unit );
	
	return unit;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	id	
 * @param	frame	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ScrSet( BR_WORK* wk, int id, int frame )
{
	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, id, wk->sys.bgl, frame, 0, 0, 0, HEAPID_BR );
}

// =============================================================================
//
//
//	■エラー処理
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Error_Init( BR_WORK* wk )
{
	int frame = GF_BGL_FRAME2_M;
	
	if ( wk->sys.err_wk.bErrorInit ){ return; }	
	wk->sys.err_wk.bErrorInit = TRUE;

	if ( sys.disp3DSW == DISP_3D_TO_SUB ){	///< 下がmainなら上に出す
		frame = GF_BGL_FRAME2_S;
	}

	GF_BGL_BmpWinAdd( wk->sys.bgl, &wk->sys.err_wk.err_win, frame, 
					  ERR_WIN_X, ERR_WIN_Y, ERR_WIN_SX, ERR_WIN_SY, ERR_WIN_PAL, ERR_WIN_OFS );
	GF_BGL_BmpWinOff( &wk->sys.err_wk.err_win );
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Error_Exit( BR_WORK* wk )
{
	if ( wk->sys.err_wk.bErrorInit == FALSE ){ return; }
	wk->sys.err_wk.bErrorInit = FALSE;

	GF_BGL_BmpWinOff( &wk->sys.err_wk.err_win );
	GF_BGL_BmpWinDel( &wk->sys.err_wk.err_win );
}

//--------------------------------------------------------------
/**
 * @brief	エラーメッセージ
 *
 * @param	work	
 * @param	error_msg	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Error_CallBack( void* work, STRBUF* error_msg )
{
	BR_WORK* wk = work;
	
	if( error_msg != NULL ){
	//	GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_LOG], 15 );
	//	testsys->msg_index = GF_STR_PrintSimple( &testsys->win[TEST_BMPWIN_LOG], FONT_TALK,	error_msg, 0, 0, MSG_ALLPUT, NULL);		
	}
	else {
	//	GF_BGL_BmpWinDataFill(&testsys->win[TEST_BMPWIN_LOG], 15 );	
	}
}


// =============================================================================
//
//
//	■タッチアクション
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	タッチアクションTCB
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void TouchAction_TCB( TCB_PTR tcb, void* work )
{
	BR_WORK* wk = work;
	s16 ofs_r;
	
	if ( (++wk->touch_work.cnt) >= 7 ){
		TCB_Delete( tcb );
		wk->touch_work.cnt = 0;
		wk->touch_work.bAction = FALSE;
		Cursor_SizeS( wk->touch_work.cur );
		Cursor_R_Set( wk->touch_work.cur, 0, 0 );
		Cursor_Active( wk->touch_work.cur, FALSE );
		Cursor_Visible( wk->touch_work.cur, FALSE );
		return;
	}
	ofs_r = TOUCH_ACTION_CUR_R( wk->touch_work.cnt );
	Cursor_R_Set( wk->touch_work.cur, ofs_r, ofs_r );
}

//--------------------------------------------------------------
/**
 * @brief	タッチアクション初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void TouchActionInit( BR_WORK* wk )
{
	u32 x, y;
	s16 ofs = 0;

	if ( wk->touch_work.bAction == TRUE ){
		if ( wk->touch_work.tcb ){
			TCB_Delete( wk->touch_work.tcb );
		}
		wk->touch_work.tcb = NULL;
	}	
	if ( sys.disp3DSW == DISP_3D_TO_MAIN ){
		wk->touch_work.cur = wk->cur_wk_top;
	}
	else {
		wk->touch_work.cur = wk->cur_wk;
	}	
	wk->touch_work.bAction = TRUE;
	wk->touch_work.cnt = 0;
	
	GF_TP_GetPointTrg( &x, &y );
	
#ifdef BR_AUTO_SERCH
	x = 128;
	y =  96;
#endif

	Cursor_SizeL( wk->touch_work.cur );
	Cursor_Active( wk->touch_work.cur, TRUE );
	Cursor_Visible( wk->touch_work.cur, TRUE );
	Cursor_PosSet( wk->touch_work.cur, x, y + ofs );
	Cursor_PosSetFix( wk->touch_work.cur, x, y + ofs );
	wk->touch_work.tcb = TCB_Add( TouchAction_TCB, wk, eTCB_PRI_TOUCH_ACTION );

#ifdef PM_DEBUG
	{
		BOOL bCheck = DebugTCBSys_FuncEntryCheck( sys.mainTCBSys, TouchAction_TCB );
		GF_ASSERT_MSG( bCheck, "TouchAction TCB Add Error = %d", bCheck );
	}
#endif

	if ( wk->touch_work.tcb != NULL ){
		TouchAction_TCB( wk->touch_work.tcb, wk );
	}
	else {
		wk->touch_work.bAction = FALSE;
		Cursor_Visible( wk->touch_work.cur, FALSE );
		GF_ASSERT_MSG( 0, "TouchAction Error\n" );
	}
}

//--------------------------------------------------------------
/**
 * @brief	タッチアクション呼び出し
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void TouchAction( BR_WORK* wk )
{
	TouchActionInit( wk );
	Snd_SePlay( eBR_SND_TOUCH );
}


//--------------------------------------------------------------
/**
 * @brief	CATS が動いているか
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL IsCATS_SystemActive( BR_WORK* wk )
{
	return wk->sys.bCATS;
}

//--------------------------------------------------------------
/**
 * @brief	CATS の動作を指定
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void CATS_SystemActiveSet( BR_WORK* wk, BOOL flag )
{
	wk->sys.bCATS = flag;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	obtn	
 * @param	draw_area	
 * @param	msg_id	
 * @param	x	
 * @param	y	
 * @param	ox	
 * @param	oy	
 * @param	anime	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tool_FontButton_Create( BR_WORK* wk, OAM_BUTTON* obtn, int draw_area, int msg_id, s16 x, s16 y, s16 ox, s16 oy, int anime, BOOL flag )
{
	obtn->cap = ExTag_Add( wk, draw_area );	
	FontOam_Create( obtn, &wk->sys, draw_area );	
	FontOam_MsgSet( obtn, &wk->sys, msg_id );
	
	CATS_ObjectPosSetCap( obtn->cap, x, y );
	FONTOAM_SetMat( obtn->font_obj, ox, oy );
	FONTOAM_SetDrawFlag( obtn->font_obj, flag );
	CATS_ObjectAnimeSeqSetCap( obtn->cap, anime );
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	obtn	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Tool_FontButton_Delete( OAM_BUTTON* obtn )
{
	FontOam_Delete( obtn );
	CATS_ActorPointerDelete_S( obtn->cap );	
}

// =============================================================================
//
//
//	■エラー処理
//
//
// =============================================================================
//--------------------------------------------------------------
/**
 * @brief	エラーメッセージだけもらう
 *
 * @param	wk	
 * @param	mode	
 * @param	result	
 *
 * @retval	int	
 *
 */
//--------------------------------------------------------------
int BR_ErrorMsgGet( BR_WORK* wk, int mode, int result, int type )
{
	wk->sys.comm_err_wk.main_seq	= wk->main_seq;
	wk->sys.comm_err_wk.sub_seq		= wk->sub_seq;
	wk->sys.comm_err_wk.proc_seq	= *wk->proc_seq;
	wk->sys.comm_err_wk.bError		= TRUE;
	
	OS_Printf( "err result = %d\n", result );
	OS_Printf( "err type   = %d\n", type );

//	GF_ASSERT_MSG( 0, "mode = %d, result = %d, type = %d\n", mode, result, type );

	{
		int msg = msg_err_000;
		
		switch ( type ){
		case GDS_ERROR_TYPE_LIB:
			///< GDSライブラリのエラー
			switch ( result ){
			case POKE_NET_GDS_LASTERROR_NONE:
				msg = msg_lib_err_000;
				break;
			case POKE_NET_GDS_LASTERROR_NOTINITIALIZED:
				msg = msg_lib_err_001;
				break;
			case POKE_NET_GDS_LASTERROR_ILLEGALREQUEST:
				msg = msg_lib_err_002;
				break;
			case POKE_NET_GDS_LASTERROR_CREATESOCKET:
				msg = msg_lib_err_003;
				break;
			case POKE_NET_GDS_LASTERROR_IOCTRLSOCKET:
				msg = msg_lib_err_004;
				break;
			case POKE_NET_GDS_LASTERROR_NETWORKSETTING:
				msg = msg_lib_err_005;
				break;
			case POKE_NET_GDS_LASTERROR_CREATETHREAD:
				msg = msg_lib_err_006;
				break;
			case POKE_NET_GDS_LASTERROR_CONNECT:
				msg = msg_lib_err_007;
				break;
			case POKE_NET_GDS_LASTERROR_SENDREQUEST:
				msg = msg_lib_err_008;
				break;
			case POKE_NET_GDS_LASTERROR_RECVRESPONSE:
				msg = msg_lib_err_009;
				break;
			case POKE_NET_GDS_LASTERROR_CONNECTTIMEOUT:
				msg = msg_lib_err_010;
				break;
			case POKE_NET_GDS_LASTERROR_SENDTIMEOUT:
				msg = msg_lib_err_011;
				break;
			case POKE_NET_GDS_LASTERROR_RECVTIMEOUT:
				msg = msg_lib_err_012;
				break;
			case POKE_NET_GDS_LASTERROR_ABORTED:
				msg = msg_lib_err_013;
				break;
			case POKE_NET_GDS_LASTERROR_GETSVL:
				msg = msg_lib_err_014;
				break;
			default:						
				GF_ASSERT_MSG( 0, "result = %d\n", result );
				break;
			}
			break;
		
		case GDS_ERROR_TYPE_STATUS:
			///< GDSの状態エラー
			switch ( result ){
			case POKE_NET_GDS_STATUS_INACTIVE:
				msg = msg_st_err_000;
				break;
			case POKE_NET_GDS_STATUS_INITIALIZED:
				msg = msg_st_err_001;
				break;
			case POKE_NET_GDS_STATUS_REQUEST:
				msg = msg_st_err_002;
				break;
			case POKE_NET_GDS_STATUS_NETSETTING:
				msg = msg_st_err_003;
				break;
			case POKE_NET_GDS_STATUS_CONNECTING:
				msg = msg_st_err_004;
				break;
			case POKE_NET_GDS_STATUS_SENDING:
				msg = msg_st_err_005;
				break;
			case POKE_NET_GDS_STATUS_RECEIVING:
				msg = msg_st_err_006;
				break;
			case POKE_NET_GDS_STATUS_ABORTED:
				msg = msg_st_err_007;
				break;
			case POKE_NET_GDS_STATUS_FINISHED:
				msg = msg_st_err_008;
				break;
			case POKE_NET_GDS_STATUS_ERROR:
				msg = msg_st_err_009;
				break;
			default:						
				GF_ASSERT_MSG( 0, "result = %d\n", result );
				break;
			}
			break;
		
		case GDS_ERROR_TYPE_APP:
			///< 各アプリケーションのエラー
			{		
				switch ( mode ){
				case eERR_DRESS_SEND:
					///< ドレス送信時
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_SUCCESS:
						msg = msg_err_000;
						break;				
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_AUTH:
						msg = msg_err_001;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ALREADY:
						msg = msg_err_002;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGAL:
						msg = msg_err_003;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_ILLEGALPROFILE:
						msg = msg_err_004;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_ERROR_UNKNOWN:
						msg = msg_err_005;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}
					break;
				
				case eERR_DRESS_RECV:
					///< ドレス受信時
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_SUCCESS:
						msg = msg_err_006;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_AUTH:
						msg = msg_err_007;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_ILLEGALPOKEMON:
						msg = msg_err_008;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_GET_ERROR_UNKNOWN:
						msg = msg_err_009;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}
					break;
				
				case eERR_BOX_SEND:
					///< ボックスショット送信時
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_SUCCESS:
						msg = msg_err_010;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_AUTH:
						msg = msg_err_011;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ALREADY:
						msg = msg_err_012;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGAL:
						msg = msg_err_013;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALPROFILE:
						msg = msg_err_014;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_ILLEGALGROUP:
						msg = msg_err_015;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_REGIST_ERROR_UNKNOWN:
						msg = msg_err_016;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}			
					break;
					
				case eERR_BOX_RECV:
					///< ボックスショット受信時
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_SUCCESS:
						msg = msg_err_017;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_AUTH:
						msg = msg_err_018;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_ILLEGALGROUP:
						msg = msg_err_019;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BOXSHOT_GET_ERROR_UNKNOWN:
						msg = msg_err_020;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}			
					break;
				
				case eERR_RANKING_TYPE_RECV:
					///< ランキングデータの受信
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_SUCCESS:
						msg = msg_err_021;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_AUTH:
						msg = msg_err_022;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_GETTYPE_ERROR_UNKNOWN:
						msg = msg_err_023;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}						
					break;
					
				case eERR_RANKING_SEND:
					///< ランキングデータの送信
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_SUCCESS:
						msg = msg_err_024;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_AUTH:
						msg = msg_err_025;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALTYPE:
						msg = msg_err_026;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALDATA:
						msg = msg_err_027;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_ILLEGALPROFILE:
						msg = msg_err_028;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_RANKING_UPDATE_ERROR_UNKNOWN:
						msg = msg_err_029;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}						
					break;
				
				case eERR_VIDEO_SEND:
					///< ビデオの送信
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS:
						msg = msg_err_030;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH:
						msg = msg_err_031;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY:
						msg = msg_err_032;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL:
						msg = msg_err_033;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE:
						msg = msg_err_034;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN:
						msg = msg_err_035;
						break;					
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_MINE:
						msg = msg_err_036_0;
						break;					
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_OTHER:	
						msg = msg_err_036_1;
						break;						
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}						
					break;

				case eERR_VIDEO_SEARCH:
					///< ビデオの検索
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS:
						msg = msg_err_036;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_AUTH:
						msg = msg_err_037;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALPARAM:
						msg = msg_err_038;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_UNKNOWN:
						msg = msg_err_039;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}
					break;

				case eERR_VIDEO_DATA_GET:
					///< ビデオの受信
					switch ( result ){
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS:
						msg = msg_err_040;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH:
						msg = msg_err_041;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE:
						msg = msg_err_042;
						break;
					case POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN:
						msg = msg_err_043;
						break;
					default:						
						GF_ASSERT_MSG( 0, "result = %d\n", result );
						break;
					}
					break;
					
				default:
					GF_ASSERT_MSG( 0, "result = %d\n", result );
					break;
				}
			}
			break;
		}
		wk->sys.comm_err_wk.err_msg_id	= msg;
	}
	
	OS_Printf( "msg id = %d\n", wk->sys.comm_err_wk.err_msg_id );
	
	return wk->sys.comm_err_wk.err_msg_id;	
}

//--------------------------------------------------------------
/**
 * @brief	エラーを受け取った
 *
 * @param	mode	起動モード
 * @param	result	受け取った結果
 * @param	type	エラータイプ GDS_ERROR_TYPE_LIB　GDS_ERROR_TYPE_STATUS GDS_ERROR_TYPE_APP
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ErrorSet( BR_WORK* wk, int mode, int result, int type )
{
	wk->sys.comm_err_wk.main_seq	= wk->main_seq;
	wk->sys.comm_err_wk.sub_seq		= wk->sub_seq;
	wk->sys.comm_err_wk.proc_seq	= *wk->proc_seq;
	wk->sys.comm_err_wk.err_seq		= 0;
	wk->sys.comm_err_wk.err_count	= 0;
	wk->sys.comm_err_wk.err_msg_id	= BR_ErrorMsgGet( wk, mode, result, type );
	
	wk->sys.comm_err_wk.bError		= TRUE;	

	*wk->proc_seq = eMAIN_Error;
}

//--------------------------------------------------------------
/**
 * @brief	エラー処理のメイン
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Error_Main( BR_WORK* wk )
{
	switch ( wk->sys.comm_err_wk.err_seq ){
	case 0:
		if ( wk->sys.comm_err_wk.err_count < 8 ){
			wk->sys.comm_err_wk.err_count++;
		//	GX_SetMasterBrightness( -wk->sys.comm_err_wk.err_count );
		//	GXS_SetMasterBrightness( -wk->sys.comm_err_wk.err_count );
		//	G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, -wk->sys.comm_err_wk.err_count );
		//	G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, -wk->sys.comm_err_wk.err_count );
		}
		else {
			Tool_InfoMessageMain( wk, wk->sys.comm_err_wk.err_msg_id );	
			wk->sys.comm_err_wk.err_seq++;
		}
		break;
	
	case 1:
		if ( GF_TP_GetTrg() ){
			Tool_InfoMessageMainDel( wk );	
			wk->sys.comm_err_wk.err_seq++;
		}
	#ifdef PM_DEBUG
		#ifdef BR_AUTO_SERCH
			Tool_InfoMessageMainDel( wk );	
			wk->sys.comm_err_wk.err_seq++;
			break;
		#endif
	#endif
		break;
	
	default:
		if ( wk->sys.comm_err_wk.err_count > 0 ){
			wk->sys.comm_err_wk.err_count--;
		//	GX_SetMasterBrightness( -wk->sys.comm_err_wk.err_count );
		//	GXS_SetMasterBrightness( -wk->sys.comm_err_wk.err_count );
		//	G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, -wk->sys.comm_err_wk.err_count );
		//	G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3 |GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD, -wk->sys.comm_err_wk.err_count );
		}
		else {
			*wk->proc_seq = wk->sys.comm_err_wk.proc_seq;
		}		
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	保存されてる？
 *
 * @param	wk	
 * @param	video_no	
 *
 * @retval	BOOL	YES
 *
 */
//--------------------------------------------------------------
BOOL BattleVideo_Preserved( BR_WORK* wk, u64 video_no )
{
	u64 no;
	int i;
	
	///< 自分のセーブは比較しない
	for ( i = 1; i < 4; i++ ){		
		if ( wk->br_head[ i ] == NULL ){ continue; }
		
		no = RecHeader_ParamGet( wk->br_head[ i ], RECHEAD_IDX_DATA_NUMBER, 0 );
		
		if ( no == video_no ){ return TRUE; }	///< もってたらブッブー
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	エラー文字列を規定の文字列に置き換える
 *
 * @param	wk	
 * @param	strbuf	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_ErrorStrChange( BR_WORK* wk, STRBUF *strbuf )
{
	GDS_RAP_WORK* gdsrap = BR_GDSRAP_WorkGet( wk );
	STRBUF *temp_str;
	int strcode_size = 64;	//適当に大きく
	
	temp_str = STRBUF_Create(strcode_size, gdsrap->heap_id);

	//不明文字コードチェック
	if(FontProc_ErrorStrCheck(FONT_SYSTEM, strbuf, temp_str) == FALSE){
		OS_TPrintf("不明な文字コードが入っている\n");
		ErrorNameSet(strbuf, gdsrap->heap_id);
	}

	STRBUF_Delete(temp_str);
}

//--------------------------------------------------------------
/**
 * @brief   エラー文字だった場合に表示する代理名をバッファにセット
 *
 * @param   dest_str		代入先
 * @param   heap_id			テンポラリで使用するヒープ
 */
//--------------------------------------------------------------
static void ErrorNameSet(STRBUF *dest_str, int heap_id)
{
	MSGDATA_MANAGER * msgman;

	STRBUF_Clear(dest_str);
	
	msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_battle_rec_dat, heap_id );
	MSGMAN_GetString( msgman, msg_error_trainer_name, dest_str );
	MSGMAN_Delete( msgman );
}
