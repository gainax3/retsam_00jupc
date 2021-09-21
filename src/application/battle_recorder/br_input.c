//==============================================================================
/**
 * @file	br_input.c
 * @brief	バトルレコーダー
 * @author	goto
 * @date	2007.07.31
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
#include "system/wipe.h"

#include "br_private.h"

//==============================================================
// Prototype
//==============================================================
static BOOL BR_input_Touch( BR_WORK* wk );
static BOOL BR_input_Key( BR_WORK* wk );
static BOOL BR_input_End( BR_WORK* wk );
static BOOL BR_input_MenuIn( BR_WORK* wk );
static BOOL BR_input_TagSelect( BR_WORK* wk );
static BOOL BR_input_TagBack( BR_WORK* wk );

static BOOL BR_input_ColorChangeL( BR_WORK* wk );
static BOOL BR_input_ColorChangeR( BR_WORK* wk );

static int SideBer_HitCheck( void );


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_Touch( BR_WORK* wk )
{
	if ( GF_TP_GetTrg() && wk->bWiFiEnd == FALSE ){
		Tool_InfoMessageDel( wk );
	}
#if 0
	if ( sys.trg ){		
		wk->sys.touch = INPUT_MODE_KEY;		
		BR_Main_SeqChange( wk, eINPUT_KEY );		
		Cursor_Visible( wk->cur_wk, TRUE );		
	//	OS_Printf( "key mode\n" );		
		return FALSE;
	}
#endif	
	if ( wk->mode == BR_MODE_BROWSE )
	{
		int no = SideBer_HitCheck();
		
		switch ( no ){
		case 1:
			Snd_SePlay( eBR_SND_COL_CHG );
			BR_Main_SeqChange( wk, eINPUT_COLOR_CHANGE_L );
			return FALSE;
		
		case 2:
			Snd_SePlay( eBR_SND_COL_CHG );
			BR_Main_SeqChange( wk, eINPUT_COLOR_CHANGE_R );
			return FALSE;
		
		default:
			break;
		}
	}
	
	BR_TouchPanel_Update( wk );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	サイドバー触ってるー？
 *
 * @param	none	
 *
 * @retval	static int	
 *
 */
//--------------------------------------------------------------
static int SideBer_HitCheck( void )
{
	static const TP_HIT_TBL color_hit_tbl[] = {
		{ 0, 192,   0 +  0,   0 + 8 },
		{ 0, 192, 256 - 8, 256 - 0 },
	};
	if ( GF_TP_SingleHitCont( &color_hit_tbl[ 0 ] ) ){
		return 1;
	}

	if ( GF_TP_SingleHitCont( &color_hit_tbl[ 1 ] ) ){
		return 2;
	}
	
	return 0;
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_Key( BR_WORK* wk )
{
	BOOL bMove = FALSE;
	
	GF_ASSERT( 0 );					///< ここにはもうこない
	
	wk->sys.touch = INPUT_MODE_TOUCH;
	BR_Main_SeqChange( wk, eINPUT_TOUCH );
	
	return BR_input_Touch( wk );	///< とりあえず、touchのふりする

#if 0
	if ( GF_TP_GetTrg() ){
		Tool_InfoMessageDel( wk );
	}
	
	if ( GF_TP_GetCont() == TRUE ){
		///< タッチされたら、キーモードを終了する
			
		wk->sys.touch = INPUT_MODE_TOUCH;
		
		BR_Main_SeqChange( wk, eINPUT_TOUCH );
		
		Cursor_Visible( wk->cur_wk, FALSE );
		
	//	OS_Printf( "touch mode\n" );
		
		return FALSE;
	}
	
	if ( sys.trg & PAD_BUTTON_A ){
		///< 決定
		
		///< 判定しない処理
		{
			BOOL bLock = BR_UnlockMenuCheck( wk );
			
			if ( bLock == FALSE ){
				
				return FALSE;
			}	
		}
		
		wk->ex_param1 = wk->menu_data[ wk->tag_man.cur_pos ].ex_param1;
		BR_Main_SeqChange( wk, wk->menu_data[ wk->tag_man.cur_pos ].tag_seq );
		
		if ( wk->menu_data[ wk->tag_man.cur_pos ].tag_seq == eINPUT_END ){
			BR_PostMessage( wk->message, FALSE, 0 );
		}
	}
	else if ( sys.trg & PAD_BUTTON_B ){
		///< キャンセル		
		if ( wk->tag_man.stack_s == 0 ){
			BR_PostMessage( wk->message, FALSE, 0 );
			BR_Main_SeqChange( wk, eINPUT_END );
		}
		else {
			BR_Main_SeqChange( wk, eINPUT_TAG_BACK );
		}
	}
	else if ( sys.trg & PAD_KEY_UP ){
		///< ↑ キー
		
		if ( wk->tag_man.cur_pos > 0 ){
			wk->tag_man.cur_pos--;
		}
		else {
			wk->tag_man.cur_pos = wk->tag_man.item_max - 1;
		}
		
		bMove = TRUE;
	}
	else if ( sys.trg & PAD_KEY_DOWN ){
		///< ↓ キー
		
		wk->tag_man.cur_pos++;
		wk->tag_man.cur_pos %= wk->tag_man.item_max;
		
		bMove = TRUE;
	}
	
	if ( bMove ){		
		s16 x, y;
		BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ wk->tag_man.cur_pos ].cap, &x, &y );
		Cursor_PosSet( wk->cur_wk, x + TAG_CIRCLE_CX, y );
	}
		
	return FALSE;
#endif
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_End( BR_WORK* wk )
{	
	switch ( wk->sub_seq ){
	case 0:
		wk->sys.master_color = 0;
		wk->wait = 0;
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_Visible( wk->cur_wk_top, FALSE );
		CATS_SystemActiveSet( wk, TRUE );			///< true にした時点で、Visibleも効かなくなる↑2行
		wk->sub_seq++;
		break;
	
	case 1:
		if ( wk->wait != 16 ){
			wk->wait += 2;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFE, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< main	bg
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0x3FFE, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG, 	 0xFFFF, wk->wait, BR_FadeColorCode_Get_BootLine( wk ) );	///< sub	bg
		}
		else {
			wk->wait = 16;
			wk->sub_seq++;
		}
		break;
	
	case 2:
		if ( wk->wait != 0 ){
			 wk->wait -= 4;
		}
		else {
			WIPE_SYS_Start( WIPE_PATTERN_WMS,
				    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
				    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
			wk->sub_seq++;
		}
		break;

	default:
		if (WIPE_SYS_EndCheck() == TRUE){
			CATS_SystemActiveSet( wk, FALSE );
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_MenuIn( BR_WORK* wk )
{
	BOOL bActive;
	
	bActive = BR_start_MenuIn( wk );
	
	if ( bActive ){
		
		if ( wk->sys.touch == INPUT_MODE_TOUCH ){
			
			Cursor_Visible( wk->cur_wk, FALSE );
			BR_Main_SeqChange( wk, eINPUT_TOUCH );
		}
		else {
			
			BR_Main_SeqChange( wk, eINPUT_KEY );
		}
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_Executed( BR_WORK* wk )
{
	BOOL bActive;
	
	switch( wk->menu_data[ wk->tag_man.cur_pos ].seq ){
//	case eMENU_DATA_REC_DELETE:
//		///< セーブを消す
//		bActive = BR_input_Executed_RecDelete( wk );
//		break;
		
	default:
		///< ダミー
		bActive = TRUE;
		break;
	}
	
	if ( bActive ){
		if ( wk->sys.touch == INPUT_MODE_TOUCH ){
			
			Cursor_Visible( wk->cur_wk, FALSE );
			BR_Main_SeqChange( wk, eINPUT_TOUCH );
		}
		else {
			
			BR_Main_SeqChange( wk, eINPUT_KEY );
		}
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	タグ移動用TCB
 *
 * @param	tcb	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_TagMove_TCB( TCB_PTR tcb, void* work )
{
	BR_TAG_TCB_WORK* wk = work;
	
	switch ( wk->seq ){
	case 0:
		wk->mx = ( wk->t_px - wk->s_px ) / wk->wait;
		wk->my = ( wk->t_py - wk->s_py ) / wk->wait;
		wk->seq++;
	
	case 1:
		{
			s16 x, y;
			BR_CATS_ObjectPosGetCap( wk->cap, &x, &y );
			
			wk->wait--;
			if ( wk->wait == 0 ){
				x = wk->t_px;
				y = wk->t_py;
				wk->seq++;
			}
			else {
				x += wk->mx;
				y += wk->my;
			}
			
			if ( wk->cur_wk ){
				Cursor_PosSet( wk->cur_wk, x + TAG_CIRCLE_CX, y );
				
				if ( wk->wait == 2 ){
					
					*(wk->bTagMove_U) = TRUE;
				}
			}
			
			BR_CATS_ObjectPosSetCap( wk->cap, x, y );
			FONTOAM_SetMat( wk->font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
			
			*(wk->bEnd) = TRUE;
		}
		break;
		
	default:
		///< 解放
		sys_FreeMemoryEz( wk );
		TCB_Delete( tcb );		
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief	タグの移動
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_TagSelect( BR_WORK* wk )
{
	BR_TAG_SEL_WORK* tsw;
	
	switch ( wk->sub_seq ){
	case 0:
		///< 動作TCB登録
//		BR_TouchPanel_TagHRT_ReSet( wk );
		{
			int i;			
			BR_TAG_TCB_WORK* tag;
			BR_TAG_SEL_WORK* tsw = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_SEL_WORK ) );
			MI_CpuFill8( tsw, 0, sizeof( BR_TAG_SEL_WORK ) );
			wk->sub_work = tsw;
			tsw->bInit = FALSE;
			
			for ( i = 0; i < wk->tag_man.item_max; i++ ){
				
				tag = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_TCB_WORK ) );
				memset( tag, 0, sizeof( BR_TAG_TCB_WORK ) );
				
				BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ i ].cap, &tag->s_px, &tag->s_py );
				
				tag->cap		= wk->tag_man.tag_m[ i ].cap;
				tag->font_obj	= wk->tag_man.tag_m[ i ].font_obj;
				tag->bEnd		= &tsw->bEnd;
				tag->bTagMove_U	= &tsw->bTagMove_U;
				
				if ( i == wk->tag_man.cur_pos ){
					tag->cur_wk = wk->cur_wk;
					tag->t_px = tag->s_px;
					tag->t_py = TAG_VANISH_TOP;
					tag->wait = TAG_UP_MOVE_F;
				}
				else {
					tag->cur_wk = NULL;					
					tag->t_px = tag->s_px;
					tag->t_py = TAG_INIT_POS( TAG_MAX, wk->tag_man.item_max );
				//	OS_Printf( "タグの移動 = %d\n", tag->t_py );
					if ( tag->t_py >= TAG_INIT_POS_2( TAG_MAX, 3 ) ){
						 tag->t_py = TAG_INIT_POS_2( TAG_MAX, 3 );
					}
					tag->wait = TAG_DOWN_MOVE_F;
				}
				
				TCB_Add( BR_TagMove_TCB, tag, eTCB_PRI_TAG_MOVE );
			}
			
			tsw->color = 0;			
		}
		wk->sub_seq++;
		break;
	
	case 1:
		///< TCB 待ち
		tsw = wk->sub_work;
		if ( tsw->color != 16 ){
			tsw->color += 2;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, tsw->color, wk->sys.logo_color );
		}
		///< 上画面
		{
			int i;		
			BR_TAG_TCB_WORK* tag;
			if ( tsw->bTagMove_U == TRUE ){
				
				i = 0;
				
				if ( tsw->bInit == FALSE ){
					tsw->bInit = TRUE;
					do {
						tag = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_TCB_WORK ) );
						memset( tag, 0, sizeof( BR_TAG_TCB_WORK ) );
						
						BR_CATS_ObjectPosGetCap( wk->tag_man.tag_s[ i ].cap, &tag->s_px, &tag->s_py );
						
						tag->cap			= wk->tag_man.tag_s[ i ].cap;
						tag->font_obj		= wk->tag_man.tag_s[ i ].font_obj;
						tag->bEnd			= &tsw->bEnd;
						
						tag->cur_wk = NULL;
						tag->t_px = tag->s_px;
						if ( wk->tag_man.stack_s == i ){
							tag->t_px = tag->s_px;
							tag->t_py = TAG_INIT_POS_U( TAG_MAX - 1 );
							CATS_ObjectAnimeSeqSetCap( wk->tag_man.tag_s[ i ].cap, wk->menu_data[ wk->tag_man.cur_pos ].ID );
							BR_disp_FontOAM_MegWrite( wk, &wk->tag_man.tag_s[ wk->tag_man.stack_s ], wk->menu_data[ wk->tag_man.cur_pos ].msgID, &wk->menu_data[ wk->tag_man.cur_pos ] );
							FONTOAM_SetDrawFlag( wk->tag_man.tag_s[ wk->tag_man.stack_s ].font_obj, TRUE );
							CATS_ObjectEnableCap( wk->tag_man.tag_s[ i ].cap, TRUE );
						}
						else {
							tag->t_px = tag->s_px - 4;
							tag->t_py = ( 16 * ( wk->tag_man.stack_s - i ) ) + TAG_INIT_POS_U( TAG_MAX - ( wk->tag_man.stack_s - i ) - 1 );
						}
						tag->wait = TAG_UP_SUB_MOVE_F;
						
						TCB_Add( BR_TagMove_TCB, tag, eTCB_PRI_TAG_MOVE );
						
						i++;
					} while ( i <= wk->tag_man.stack_s );
				}
				
				tsw->bEnd = tsw->bTagMove_U;
				tsw->bTagMove_U = FALSE;
			}
			
			///< タグの色切り替え	
			for ( i = 0; i < wk->tag_man.stack_s; i++ ){
				if ( i == wk->tag_man.stack_s ){
					CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_1 );	
				}
				else {
					CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_2 );	
				}
			}			
		}
		if ( tsw->bEnd == FALSE ){
			wk->sub_seq++;
		}
		tsw->bEnd = FALSE;
		break;
		
	case 2:
		{		
			if ( wk->menu_data[ wk->tag_man.cur_pos ].data == NULL ){		///< 次のメニューデータが無い場合
			
				BR_MenuData_Analysis( wk );
			}
			else {

				BR_Main_SeqChange( wk, eINPUT_MENU_IN );
			}
			
			BR_Main_MenuChangeNext( wk );					///< 次のメニュー設定			
		//	BR_TouchPanel_TagHRT_Set( wk );					///< タグの当たり判定更新
		
			tsw = wk->sub_work;								///< subワークは削除
			sys_FreeMemoryEz( tsw );
		}
		break;
	
	default:
	//	BR_PostMessage( wk->message, TRUE, 0 );
	//	BR_Main_SeqChange( wk, eINPUT_END );
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	タグ戻す
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_input_TagBack( BR_WORK* wk )
{
	BR_TAG_SEL_WORK* tsw;
	
	switch ( wk->sub_seq ){
	case 0:
		///< 動作TCB登録
		{
			int i;
			BR_TAG_TCB_WORK* tag;
			
			tsw = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_SEL_WORK ) );
			MI_CpuFill8( tsw, 0, sizeof( BR_TAG_SEL_WORK ) );
			wk->sub_work = tsw;
			
			i = 0;
			do {
				tag = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_TCB_WORK ) );
				memset( tag, 0, sizeof( BR_TAG_TCB_WORK ) );

				BR_CATS_ObjectPosGetCap( wk->tag_man.tag_s[ i ].cap, &tag->s_px, &tag->s_py );
				
				tag->cap		= wk->tag_man.tag_s[ i ].cap;
				tag->font_obj	= wk->tag_man.tag_s[ i ].font_obj;
				tag->bEnd		= &tsw->bEnd;					
				tag->bTagMove_U	= &tsw->bTagMove_U;
				
				tag->cur_wk = NULL;
				tag->t_px = tag->s_px;
				if ( wk->tag_man.stack_s - 1 == i ){
					tag->cur_wk = wk->cur_wk;
					tag->t_px = tag->s_px;
					tag->t_py = TAG_INIT_POS_U( TAG_MAX - 1 ) + TAG_VANISH_BOTTOM;
				}
				else {
					tag->t_px = tag->s_px + 4;
					tag->t_py = TAG_INIT_POS_U( TAG_MAX - ( wk->tag_man.stack_s - i ) ) + ( 16 * ( wk->tag_man.stack_s - i ) ) + 4;
				}
				tag->wait = TAG_UP_SUB_MOVE_F;
				
				TCB_Add( BR_TagMove_TCB, tag, eTCB_PRI_TAG_MOVE );
				
				i++;
			} while ( i <= ( wk->tag_man.stack_s - 1 ) );
		}
		wk->sub_seq++;
		break;

	case 1:
		tsw = wk->sub_work;
		{
			int i;			
			BR_TAG_TCB_WORK* tag;
						
			if ( tsw->bTagMove_U == TRUE ){				
			
				for ( i = 0; i < wk->tag_man.item_max; i++ ){
					
					tag = sys_AllocMemory( HEAPID_BR, sizeof( BR_TAG_TCB_WORK ) );
					memset( tag, 0, sizeof( BR_TAG_TCB_WORK ) );
					
					BR_CATS_ObjectPosGetCap( wk->tag_man.tag_m[ i ].cap, &tag->s_px, &tag->s_py );
					
					tag->cap		= wk->tag_man.tag_m[ i ].cap;
					tag->font_obj	= wk->tag_man.tag_m[ i ].font_obj;
					tag->bEnd		= &tsw->bEnd;
					tag->bTagMove_U	= &tsw->bTagMove_U;
					
					tag->cur_wk = NULL;					
					tag->t_px = tag->s_px;
					tag->t_py = TAG_INIT_POS( TAG_MAX, wk->tag_man.item_max );
					if ( tag->t_py >= TAG_INIT_POS_2( TAG_MAX, 3 ) ){
						 tag->t_py = TAG_INIT_POS_2( TAG_MAX, 3 );
					}
					tag->wait = TAG_UP_MOVE_F;
					
					TCB_Add( BR_TagMove_TCB, tag, eTCB_PRI_TAG_MOVE );
				}					
				tsw->color = 0;
				wk->sub_seq++;
				
				///< 色切り替え
				wk->tag_man.stack_s--;
				for ( i = 0; i < wk->tag_man.stack_s; i++ ){				
					if ( i == wk->tag_man.stack_s - 1 ){
						CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_1 );	
					}
					else {
						CATS_ObjectPaletteOffsetSetCap( wk->tag_man.tag_s[ i ].cap, dOAM_PAL_TAG_2 );	
					}
				}
			}
		}
		break;
	
	case 2:		
		tsw = wk->sub_work;
		if ( tsw->color != 16 ){
			tsw->color += 2;
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, tsw->color, wk->sys.logo_color );
		}
		if ( tsw->bEnd == FALSE ){
			wk->sub_seq++;
		}
		tsw->bEnd = FALSE;
		break;

	default:
		{
			BOOL state;
			
			state = BR_Main_MenuChangeBack( wk );		///< 前のメニュー設定
			
			tsw = wk->sub_work;
			sys_FreeMemoryEz( tsw );
			
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );
		}
		break;
	}
	
	return FALSE;
}

BOOL (* const BR_input_MainTable[])( BR_WORK* wk ) = {
	BR_input_Touch,
	BR_input_Key,
	BR_input_End,
	BR_input_TagSelect,
	BR_input_TagBack,
	BR_input_MenuIn,
	BR_input_Executed,	
	BR_input_ColorChangeL,
	BR_input_ColorChangeR,
};


static BOOL BR_input_ColorChangeL( BR_WORK* wk )
{
	switch ( wk->sub_seq ){ 
	case 0:
		wk->sys.master_color = 0;
		if ( wk->sys.color_type > 0 ){
			wk->sys.color_type--;
		}
		else {
			wk->sys.color_type = CHANGE_COLOR_MAX - 1;
		}
		wk->sys.logo_color = BR_FadeColorCode_Get( wk );
		wk->sub_seq++;
		break;

	case 1:
		if ( BR_PaletteFade( &wk->sys.master_color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		break;
		
	case 2:
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
		BR_ColorPaletteChange( wk, wk->sys.color_type );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		wk->sub_seq++;
		break;
		
	case 3:
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 4:
		if ( BR_PaletteFade( &wk->sys.master_color, eFADE_MODE_IN ) ){ wk->sub_seq++; }
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eINPUT_TOUCH );
		break;
	}
	return FALSE;
}

static BOOL BR_input_ColorChangeR( BR_WORK* wk )
{
	switch ( wk->sub_seq ){ 
	case 0:
		wk->sys.master_color = 0;
		wk->sys.color_type++;
		wk->sys.color_type %= CHANGE_COLOR_MAX;		
		wk->sys.logo_color = BR_FadeColorCode_Get( wk );
		wk->sub_seq++;
		break;

	case 1:
		if ( BR_PaletteFade( &wk->sys.master_color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		break;
		
	case 2:
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
		BR_ColorPaletteChange( wk, wk->sys.color_type );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		wk->sub_seq++;
		break;
		
	case 3:
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 4:
		if ( BR_PaletteFade( &wk->sys.master_color, eFADE_MODE_IN ) ){ wk->sub_seq++; }
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_BG,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 0xFFFF, wk->sys.master_color, wk->sys.logo_color );
		break;
	
	default:
		wk->sys.master_color = 0;
		BR_Main_SeqChange( wk, eINPUT_TOUCH );
		break;
	}
	return FALSE;
}
