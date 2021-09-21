//==============================================================================
/**
 * @file	br_delete.c
 * @brief	バトルビデオ削除画面
 * @author	goto
 * @date	2008.03.10(月)
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
#include "system/window.h"
#include "msgdata/msg.naix"
#include "msgdata/msg_fightmsg_dp.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "system/fontproc.h"
#include "system/lib_pack.h"
#include "system/fontoam.h"
#include "system/msg_ds_icon.h"
#include "gflib/msg_print.h"
#include "gflib/touchpanel.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "poketool/boxdata.h"
#include "system/bmp_menu.h"
#include "system/snd_tool.h"
#include "gflib/strbuf_family.h"
#include "system/pm_overlay.h"

#include "br_private.h"


typedef struct {
	
	int					eva;
	int					evb;
	int					color;
	int					select;
	
	GF_BGL_BMPWIN		win_s[ 2 ];

} DEL_WORK;

enum {
	eDEL_Init = 0,
	eDEL_FadeOut,
	eDEL_Main,
	eDEL_Exit,
};

//==============================================================
// Prototype
//==============================================================
static void InfoMessageAdd( BR_WORK* wk, BOOL flag );
static void InfoMessageDel( BR_WORK* wk );
static void InfoMessageSet( BR_WORK* wk, int no, int data_no );
static void SaveWin_Add( BR_WORK* wk );
static void SaveWin_Del( BR_WORK* wk );
static BOOL DEL_Init( BR_WORK* wk );
static BOOL DEL_FadeOut( BR_WORK* wk );
static BOOL DEL_Main( BR_WORK* wk );
static BOOL DEL_Exit( BR_WORK* wk );



static void InfoMessageAdd( BR_WORK* wk, BOOL flag )
{
	GF_BGL_BMPWIN* win = &wk->info_win;

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_M, flag );	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 1, 19, 30, 4, eBG_PAL_FONT, 300 );
}

static void InfoMessageDel( BR_WORK* wk )
{
	GF_BGL_BmpWinOff( &wk->info_win );
	GF_BGL_BmpWinDel( &wk->info_win );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );	
}

static void InfoMessageSet( BR_WORK* wk, int no, int data_no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;

	if ( no == msg_info_013 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = wk->br_gpp[ data_no ];
		
		wset = BR_WORDSET_Create( HEAPID_BR );
		str1 = STRBUF_Create( 255, HEAPID_BR );
		str2 = GDS_Profile_CreateNameString( gpp, HEAPID_BR );
		BR_ErrorStrChange( wk, str2 );
		str3 = MSGMAN_AllocString( wk->sys.man, no );

		WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str1, str3 );
		
		STRBUF_Delete( str2 );
		STRBUF_Delete( str3 );
		WORDSET_Delete( wset );
	}
	else {
		str1 = MSGMAN_AllocString( wk->sys.man, no );	
	}

	GF_BGL_BmpWinDataFill( win, BR_MSG_CLEAR_CODE );	
		
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_INFO, NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

static void SaveWin_Add( BR_WORK* wk )
{
	DEL_WORK* dwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int				ofs = 1;
	int x;
	
	win = &dwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 4, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1000 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );
	
	ofs += ( 10 * 2 );
	
	win = &dwk->win_s[ 1 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 18, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1001 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );	
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
static void SaveWin_Del( BR_WORK* wk )
{
	DEL_WORK* dwk = wk->sub_work;

	GF_BGL_BmpWinOff( &dwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &dwk->win_s[ 0 ] );
	GF_BGL_BmpWinOff( &dwk->win_s[ 1 ] );
	GF_BGL_BmpWinDel( &dwk->win_s[ 1 ] );
}

static BOOL DEL_Init( BR_WORK* wk )
{
	DEL_WORK* dwk = sys_AllocMemory( HEAPID_BR, sizeof( DEL_WORK ) );
	
	memset( dwk, 0, sizeof( DEL_WORK ) );
	
	wk->sub_work = dwk;
	
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	Plate_AlphaInit_Default( &dwk->eva, &dwk->evb, ePLANE_ALL );
	
	BR_Main_SeqChange( wk, eDEL_FadeOut );

	return FALSE;
}

static BOOL DEL_FadeOut( BR_WORK* wk )
{
	DEL_WORK* dwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &dwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }

		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 dwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 dwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 2 );

		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		
		CATS_SystemActiveSet( wk, TRUE );		
		NormalTag_DeleteAllOp( wk );						///< tag delete
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;

	default:
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
		BR_Main_SeqChange( wk, eDEL_Main );
		break;
	}	
	return FALSE;
}

static const RECT_HIT_TBL hit_table[] = {
	{ DTC(  5 ), DTC( 9 ), DTC(  4 ), DTC( 15 ) },	///< はい
	{ DTC(  5 ), DTC( 9 ), DTC( 18 ), DTC( 29 ) },	///< いいえ	
};
static BOOL DEL_Main( BR_WORK* wk )
{
	DEL_WORK* dwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		SaveWin_Add( wk );
		InfoMessageAdd( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_answer_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;		
		break;
	
	case 1:
		///< ○○の記録を～
		BR_PaletteFade( &dwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			InfoMessageSet( wk, msg_info_013, wk->ex_param1 );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );		
		break;

	case 2:
		///< 最初　はい・いいえ
		if ( GF_TP_SingleHitTrg( &hit_table[ 0 ] ) ){
			TouchAction( wk );
			InfoMessageSet( wk, msg_info_014, 0 );
			wk->sub_seq++;
		}
		else if ( GF_TP_SingleHitTrg( &hit_table[ 1 ] ) ){
			TouchAction( wk );			
			dwk->select = 1;
			InfoMessageDel( wk );
			wk->sub_seq = 4;
		}
		break;
	
	case 3:
		///< 再度　はい・いいえ
		if ( GF_TP_SingleHitTrg( &hit_table[ 0 ] ) ){
			TouchAction( wk );
			BR_SaveWork_Clear( wk );
			dwk->select = 0;
			wk->sub_seq++;
		}
		else if ( GF_TP_SingleHitTrg( &hit_table[ 1 ] ) ){
			TouchAction( wk );
			InfoMessageDel( wk );
			dwk->select = 1;
			wk->sub_seq++;
		}
		break;
	
	case 4:
		SaveWin_Del( wk );
		wk->sub_seq++;
		break;
	
	case 5:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			if ( dwk->select == 1 ){
				BR_Main_SeqChange( wk, eDEL_Exit );
			}
			else {
				wk->sub_seq++;
			}
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;
		
	case 6:
		///< 消す処理
		if ( BR_Executed_RecDelete( wk ) ){
			BR_SaveData_GppHead_Delete( wk );						///< GPP データ破棄
			BR_SaveData_GppHead_Load( wk );							///< GPP データの再読み込み	
			InfoMessageSet( wk, msg_info_016, 0 );
			wk->sub_seq++;
		}
		else {
			BR_Main_SeqChange( wk, eDEL_Exit );
		}
		break;

	default:
		if ( !GF_TP_GetTrg() ){ break; }
		InfoMessageDel( wk );
		BR_Main_SeqChange( wk, eDEL_Exit );
		break;
	}	
	return FALSE;
}

static BOOL DEL_Exit( BR_WORK* wk )
{
	DEL_WORK* dwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		wk->sub_seq++;
	
	case 1:
		wk->sub_seq++;
	
	case 2:
		NormalTag_RecoverAllOp( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;

	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &dwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &dwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( dwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	LOGO_PALETTE, dwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}

	return FALSE;
}

BOOL (* const BR_delete_MainTable[])( BR_WORK* wk ) = {
	DEL_Init,
	DEL_FadeOut,
	DEL_Main,
	DEL_Exit,
};
