//==============================================================================
/**
 * @file	br_start.c
 * @brief	バトルレコーダー
 * @author	goto
 * @date	2007.07.26(木)
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

#include "br_private.h"

//==============================================================
// Prototype
//==============================================================
static void TouchHere_Add( BR_WORK* wk );
static void TouchHere_Del( BR_WORK* wk );
static BOOL BR_start_Boot( BR_WORK* wk );
static BOOL BR_start_Fade( BR_WORK* wk );
static BOOL BR_start_On( BR_WORK* wk );
static BOOL BR_start_pv_MenuIn( BR_WORK* wk );
extern BOOL BR_start_MenuIn( BR_WORK* wk );

static void TouchHere_Add( BR_WORK* wk )
{
	STRBUF* str;
	int x;
	
	GF_BGL_BMPWIN* win = &wk->info_win;
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 8, 16, 16, 2, eBG_PAL_FONT, 300 );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	
	str = MSGMAN_AllocString( wk->sys.man, msg_10000 );	
	x = BR_print_x_Get( win, str );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str, x, 0, MSG_NO_PUT, PRINT_COL_BOOT, NULL );		
	STRBUF_Delete( str );	
	GF_BGL_BmpWinOnVReq( win );

	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
}

static void TouchHere_Del( BR_WORK* wk )
{	
	GF_BGL_BmpWinOff( &wk->info_win );
	GF_BGL_BmpWinDel( &wk->info_win );

	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
}

static BOOL BR_start_Boot( BR_WORK* wk )
{
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_RecoverCheck( wk ) == TRUE ){
			BR_Main_SeqChange( wk, eSTART_SEQ_FADE );
			break;
		}
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
					    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
					    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFE, 16, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xBFFF, 16, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	bg
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0x3FFE, 16, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 0xFFFF, 16, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	bg
		Cursor_SizeL( wk->cur_wk_top );
		Cursor_SizeL( wk->cur_wk );
		wk->sub_seq++;
		break;

	case 1:
		if ( WIPE_SYS_EndCheck() == FALSE ){ break; }
		Cursor_PosSetFix( wk->cur_wk_top, 256/2, 16 );
		Cursor_PosSet( wk->cur_wk_top, 256/2, 192 + 24 );
		Cursor_Visible( wk->cur_wk_top, TRUE );
		Cursor_Active( wk->cur_wk_top, TRUE );
		Snd_SePlay( eBR_SND_TOUCH_HERE );
		wk->sub_seq++;
		break;

	case 2:
		if ( (++wk->wait ) < 25 ){ break; }
		wk->wait = 0;		
		Cursor_PosSetFix( wk->cur_wk, 256/2, -16 );
		Cursor_PosSet( wk->cur_wk, 256/2, 192 / 2 );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_Active( wk->cur_wk, TRUE );
		wk->sub_seq++;
		break;
	
	case 3:
		if ( (++wk->wait ) < 18 ){ break; }
		TouchHere_Add( wk );
		Cursor_Visible( wk->cur_wk_top, FALSE );
	//	Snd_SePlay( eBR_SND_TOUCH_HERE );
		wk->wait = 0;
		wk->sub_seq++;
		break;
	
	case 4:
		if ( wk->wait < 16 ){
			wk->wait += 4;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG, 0xF000, wk->wait, 0xFFFF );	///< main	bg
		}
		else {
			wk->sub_seq++;
		}
		break;
		
	case 5:
		if ( wk->wait > 0 ){
			wk->wait -= 4;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG, 0xF000, wk->wait, 0xFFFF );	///< main	bg
		}
		else {
			wk->sub_seq++;
		}
		break;

	case 6:
		if ( GF_TP_GetCont() == TRUE ){
			TouchHere_Del( wk );
		//	TouchAction( wk );
			Cursor_Visible( wk->cur_wk, FALSE );
			Cursor_SizeS( wk->cur_wk_top );
			Cursor_SizeS( wk->cur_wk );
			wk->sub_seq++;
		}
		break;
		
	default:
		BR_Main_SeqChange( wk, eSTART_SEQ_FADE );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	フェード
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_start_Fade( BR_WORK* wk )
{
	switch ( wk->sub_seq ){
		
	case 0:
		///< ここでの作成は絶対動かせない
		CATS_SystemActiveSet( wk, TRUE );
		BR_SideBar_Load( wk );
		BR_SideBar_Add( wk );
		BR_Tag_Load( wk );
		BR_Tag_Add( wk );		
		BR_disp_FontOam_Create( wk );
		BR_Tag_ReSet( wk );		
		BR_Tag_SubRecover( wk );
		CATS_SystemActiveSet( wk, FALSE );

		///< リカバー状態の時、OAM等を作成後各シーケンスに移行		
		if ( BR_RecoverCheck( wk ) == TRUE ){
			if ( wk->mode == BR_MODE_BROWSE ){
				ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_browse_bg0u_NSCR, wk->sys.bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
			}
			else {
				ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_gds_bg0u_NSCR, wk->sys.bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
			}
			Cursor_PosSetFix( wk->cur_wk, 256/2, 192+32 );
			Cursor_PosSet( wk->cur_wk, 256/2, 192+32 );
			Cursor_Visible( wk->cur_wk, FALSE );
			Cursor_Active( wk->cur_wk, TRUE );
			BR_Main_ProcSeqChange( wk, wk->recover_wk.last_seq, TRUE, wk->proc_seq );
			break;
		}
//		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,  LOGO_PALETTE, 16, wk->sys.logo_color );
//		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );
		///< ここからは通常の起動の流れ
//		WIPE_SYS_Start( WIPE_PATTERN_WMS,
//					    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
//					    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );

		Snd_SePlay( eBR_SND_START3 );
#if 0
#if 1
		if( sys.cont & PAD_BUTTON_L ){
			Snd_SePlay( eBR_SND_START );
		}else if( sys.cont & PAD_BUTTON_R ){
			Snd_SePlay( eBR_SND_START2 );
		}else{
			Snd_SePlay( eBR_SND_START3 );
		}
#else
		Snd_SePlay( eBR_SND_START );
#endif
#endif
		wk->wait = 16;
		wk->sub_seq++;
		break;
		
	case 1:
		if ( BR_RecoverCheck( wk ) == TRUE ){
			if ( WIPE_SYS_EndCheck() == TRUE ){
				wk->sub_seq++;
			}
		}
		else {
			if ( wk->wait != 0 ){
				wk->wait -= 2;
				ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFE, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	oam
				ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	bg
				ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0x3FFE, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	oam
				ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 0xFFFF, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	bg
			}
			else {
				ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,  LOGO_PALETTE, 16, wk->sys.logo_color );
				ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );
				wk->sub_seq++;
			}
		}
		break;
	
	case 2:
		if ( wk->mode == BR_MODE_BROWSE ){
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_browse_bg0u_NSCR, wk->sys.bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
		}
		else {
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_gds_bg0u_NSCR, wk->sys.bgl, GF_BGL_FRAME1_S, 0, 0, 0, HEAPID_BR );
		}
		wk->sub_seq++;
		break;
		
	default:
		if ( wk->bar[ 0 ].state < eSB_TCB_STATE_STAY ){ break; }
		BR_Main_SeqChange( wk, eSTART_SEQ_ON );
		break;
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	起動
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_start_On( BR_WORK* wk )
{
	switch ( wk->sub_seq ){
	case 0:
		///< ちらみ
		Cursor_PosSetFix( wk->cur_wk, 256/2, 192/2 );
		Cursor_PosSet( wk->cur_wk, 256/2, 192/2 );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_Active( wk->cur_wk, TRUE );
		wk->wait++;
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, LOGO_PALETTE, 16 - wk->wait, wk->sys.logo_color );
		if ( wk->wait == 2 ){
			wk->wait = 15;
			wk->sub_seq++;
		}
		break;
	
	case 1:
		///< 見えなくして
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, LOGO_PALETTE, wk->wait, wk->sys.logo_color );
		wk->sub_seq++;
		break;
	
	case 2:
		///< フェード
		wk->wait--;
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, LOGO_PALETTE, wk->wait, wk->sys.logo_color );		
		if ( wk->wait == 0 ){
			wk->sub_seq++;
		}
		break;
			
	default:
		BR_Main_SeqChange( wk, eSTART_SEQ_MENU_IN );
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	メニューイン
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_start_pv_MenuIn( BR_WORK* wk )
{
	BOOL bActive = BR_start_MenuIn( wk );
	
	{
		s16 x, y;
		
		BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ 0 ].cap, &x, &y );
		
		Cursor_PosSet( wk->cur_wk, x + TAG_CIRCLE_CX, y );
	}
	
	return bActive;
}


//--------------------------------------------------------------
/**
 * @brief	br_input.cでもcall
 *
 * @param	wk	
 *
 * @retval	BOOL	
 *
 */
//--------------------------------------------------------------
BOOL BR_start_MenuIn( BR_WORK* wk )
{
	BR_TAG_WORK* swk;
	
	switch ( wk->sub_seq ){
	case 0:	
		///< メモリ確保
		swk = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_WORK ) );		
		MI_CpuFill8( swk, 0, sizeof( BR_TAG_WORK ) );
		wk->sub_work = swk;	
		swk->color = 16;
	//	Cursor_Visible( wk->cur_wk, TRUE );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, swk->color, wk->sys.logo_color );
//		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  LINE_OTHER_SUB_PALETTE, swk->color, wk->sys.logo_color );
		{
			int	 i;
			fx32 oy;
			
			for ( i = 0; i < wk->tag_man.item_max; i++ ){
				swk->mv_wk[ i ].wait	= TAG_IN_WAIT * i;
				swk->mv_wk[ i ].rad		= TAG_INIT_RAD;
				swk->mv_wk[ i ].ty		= TAG_INIT_POS( i, wk->tag_man.item_max );
				
				BR_CATS_ObjectPosSetCap( wk->tag_man.tag_m[ i ].cap, TAG_MAIN_POS, TAG_INIT_POS( TAG_MAX - 1, wk->tag_man.item_max ) );				
				CATS_ObjectPosGetCapFx32( wk->tag_man.tag_m[ i ].cap, &swk->mv_wk[ i ].ox, &oy );
				CATS_ObjectPosSetCapFx32( wk->tag_man.tag_m[ i ].cap, swk->mv_wk[ i ].ox, TAG_INIT_POS( TAG_MAX - 1, wk->tag_man.item_max ) * FX32_ONE );
				FONTOAM_SetMat( wk->tag_man.tag_m[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
				FONTOAM_SetDrawFlag( wk->tag_man.tag_m[ i ].font_obj, TRUE );
			}
			wk->sub_seq++;
		}
		break;

	case 1:
		///< なんやかんや
		swk = wk->sub_work;
		
		if ( swk->color != 0 ){
			swk->color -= TAG_FADE_SPPED;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, swk->color, wk->sys.logo_color );
//			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  LINE_OTHER_SUB_PALETTE, swk->color, wk->sys.logo_color );
		}
		{
			int i;
			int num = 0;
			s16 x, y;
			fx32 px, py;
			for ( i = 0; i < wk->tag_man.item_max; i++ ){
				
				if ( swk->mv_wk[ i ].wait != 0 ){
					 swk->mv_wk[ i ].wait--;
					 continue;
				}
				
				BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ i ].cap, &x, &y );
				
				if ( y == swk->mv_wk[ i ].ty ){	
					num++;
					continue;
				}
				else if ( y - TAG_MOVE_VALUE_U > swk->mv_wk[ i ].ty ){
					y -= TAG_MOVE_VALUE_U;
				}
				else {
					y = swk->mv_wk[ i ].ty;
				}
				py = y * FX32_ONE;
				px = swk->mv_wk[ i ].ox + ( Sin360R( FX_GET_ROTA_NUM( swk->mv_wk[ i ].rad ) ) * 16 );
				swk->mv_wk[ i ].rad += TAG_RAD_VALUE_U;
				CATS_ObjectPosSetCapFx32( wk->tag_man.tag_m[ i ].cap, px, py );
				FONTOAM_SetMat( wk->tag_man.tag_m[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
			}
			if ( num == wk->tag_man.item_max ){
				wk->sub_seq++;			
			}
		}
		break;
		
	default:		
		swk = wk->sub_work;
		{
			s16 x, y;
			BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ wk->tag_man.cur_pos ].cap, &x, &y );
			Cursor_PosSetFix( wk->cur_wk, x + TAG_CIRCLE_CX, y );
			Cursor_PosSet( wk->cur_wk, x + TAG_CIRCLE_CX, y );
			Cursor_Visible( wk->cur_wk, FALSE );
			
			BR_TouchPanel_TagHRT_Set( wk );
			
			sys_FreeMemoryEz( swk );
		}
		return TRUE;
	}
	
	return FALSE;
}


BOOL (* const BR_start_MainTable[])( BR_WORK* wk ) = {
	BR_start_Boot,
	BR_start_Fade,
	BR_start_On,
	BR_start_pv_MenuIn,
};

