//==============================================================================
/**
 * @file	br_profile.c
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
#include "system/pm_overlay.h"

#include "br_private.h"

/*

	上画面をメイン
	下画面をサブ
	
	に入れ替えて使用しています。

*/

enum {
	
	eVC_NONE	= 0,
	eVC_YES,
	eVC_NO,	
};

typedef struct {
	
	int					eva;
	int					evb;
	int					color;
	int					view_page;
	
	GPP_WORK			gpp_work;	
	VIDEO_WORK			vtr_work;
	
	OAM_BUTTON			obtn[ 1 ];			///< もどる
	BUTTON_MAN*			btn_man[ 1 ];
	GF_BGL_BMPWIN		win_s[ 2 ];
	
	int					video_check_state;

} PROF_WORK;


//==============================================================
// Prototype
//==============================================================
static BOOL BR_Init( BR_WORK* wk );
static BOOL BR_FadeOut( BR_WORK* wk );
static BOOL BR_ProfIn( BR_WORK* wk );
static BOOL BR_ProfMain( BR_WORK* wk );
static BOOL BR_ProfChange( BR_WORK* wk );
static BOOL BR_PlayVideoCheck( BR_WORK* wk );
static BOOL BR_PlayVideoFlagSave( BR_WORK* wk );
static BOOL BR_ProfPlay( BR_WORK* wk );
static BOOL BR_ProfExit( BR_WORK* wk );
static BOOL BR_RecoverIn( BR_WORK* wk );
static void Btn_CallBack_000( u32 button, u32 event, void* work );
static void GppDataCreate( BR_WORK* wk );
static void ProfWin_Add( BR_WORK* wk );
static void ProfWin_Del( BR_WORK* wk );
static void ProfWin_MsgSet( BR_WORK* wk, int id );
static void FontButton_CreateSub( BR_WORK* wk );
static void FontButton_DeleteSub( BR_WORK* wk );


BOOL (* const BR_prof_MainTable[])( BR_WORK* wk ) = {
	BR_Init,
	BR_FadeOut,
	BR_ProfIn,
	BR_ProfMain,
	BR_ProfChange,
	BR_PlayVideoCheck,
	BR_PlayVideoFlagSave,
	BR_ProfPlay,
	BR_ProfExit,
	BR_RecoverIn,
};

enum {
	eBR_Init = 0,
	eBR_FadeOut,
	eBR_ProfIn,
	eBR_ProfMain,
	eBR_ProfChange,
	eBR_VideoCheck,
	eBR_VideoFlagSave,
	eBR_ProfPlay,
	eBR_ProfExit,
	eBR_RecoverIn,
};

static const RECT_HIT_TBL hit_table_000[] = {
	{ DTC(  2 ), DTC(  6 ), DTC(  8 ), DTC( 25 ) },	///< バトルビデオ
	{ DTC(  8 ), DTC( 14 ), DTC( 11 ), DTC( 21 ) },	///< 再生
	{ 152, 184, 80, 176 },	///< 戻る
};

static BOOL BR_Init( BR_WORK* wk )
{
	
	PROF_WORK* pwk = sys_AllocMemory( HEAPID_BR, sizeof( PROF_WORK ) );
	
	memset( pwk, 0, sizeof( PROF_WORK ) );
	
	wk->sub_work = pwk;
	
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	Plate_AlphaInit_Default( &pwk->eva, &pwk->evb, ePLANE_ALL );
	
	BR_Main_SeqChange( wk, eBR_FadeOut );
	
	return FALSE;
}

static BOOL BR_FadeOut( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &pwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 pwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 pwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete
		CATS_SystemActiveSet( wk, FALSE );
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 2 );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
		
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;

	default:
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );
		if ( BR_RecoverCheck( wk ) == TRUE ){
			
			BOOL bOk = FALSE;
			
			if ( wk->ex_param1 == 0 ){
				bOk = TRUE;	
			}
			else if ( RecHeader_ParamGet( wk->br_head[ wk->ex_param1 ], RECHEAD_IDX_SECURE, 0 ) == TRUE ){
				bOk = TRUE;
			}
			else if ( wk->ex_param3 == 1 ){
				bOk = TRUE;
			}
			
			if ( bOk ){
				FontButton_CreateSub( wk );
				pwk->video_check_state = eVC_NONE;
				BR_Main_SeqChange( wk, eBR_RecoverIn );
			}
			else {
				pwk->video_check_state = eVC_YES;
				BR_Main_SeqChange( wk, eBR_VideoFlagSave );
			}
		}
		else {
			BR_Main_SeqChange( wk, eBR_ProfIn );
		}
		break;
	}
	
	return FALSE;
}

static BOOL BR_ProfIn( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		FontButton_CreateSub( wk );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		GppDataCreate( wk );
		pwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_000, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &pwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &pwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &pwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &pwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &pwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eBR_ProfMain );
		break;
	}
	
	return FALSE;
}

static BOOL BR_ProfMain( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;	

	BMN_Main( pwk->btn_man[ 0 ] );
	
	return FALSE;
}

static BOOL BR_ProfChange( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( pwk->view_page == 0 ){
			///< 概要削除 プロフィール作成
			VideoDataFree( &pwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除 概要作成
			GppDataFree( &pwk->gpp_work, wk );						
		}
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			wk->sub_seq++;
		}
		break;
	
	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		if ( pwk->view_page == 0 ){
			///< プロフィール作成
			GppDataMake( &pwk->gpp_work, wk );
			GppData_IconEnable( &pwk->gpp_work, FALSE );
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール削除 概要作成
			VideoDataMake( &pwk->vtr_work, wk );
			VideoData_IconEnable( &pwk->vtr_work, FALSE );
			BR_ScrSet( wk, ProfileBGGet( &pwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 3:
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			if ( pwk->view_page == 0 ){
				GppData_IconEnable( &pwk->gpp_work, TRUE );
			}
			else {
				VideoData_IconEnable( &pwk->vtr_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		break;

	default:
		pwk->view_page ^= 1;
		ProfWin_MsgSet( wk, msg_718 + pwk->view_page );
		BR_Main_SeqChange( wk, eBR_ProfMain );
		break;
	}
	
	return FALSE;
}


static const RECT_HIT_TBL hit_table[] = {
	{ DTC(  5 ), DTC( 9 ), DTC(  4 ), DTC( 15 ) },	///< はい
	{ DTC(  5 ), DTC( 9 ), DTC( 18 ), DTC( 29 ) },	///< いいえ	
};
static void SaveWin_Add( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int				ofs = 1;
	int x;
	
	win = &pwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 4, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1000 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );
	
	ofs += ( 10 * 2 );
	
	win = &pwk->win_s[ 1 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 18, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1001 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );	
}

static void SaveWin_Del( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;

	GF_BGL_BmpWinOff( &pwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &pwk->win_s[ 0 ] );
	GF_BGL_BmpWinOff( &pwk->win_s[ 1 ] );
	GF_BGL_BmpWinDel( &pwk->win_s[ 1 ] );
}

//--------------------------------------------------------------
/**
 * @brief	一度見た事あるビデオか？
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_PlayVideoCheck( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< 不要なものをフェードさせる
		BMN_Delete( pwk->btn_man[ 0 ] );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );	
		CATS_SystemActiveSet( wk, TRUE );	
		if ( pwk->view_page == 0 ){
			VideoDataFree( &pwk->vtr_work, wk );
		}
		else {			
			GppDataFree( &pwk->gpp_work, wk );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ProfWin_Del( wk );
		pwk->video_check_state = eVC_NONE;
		wk->sub_seq++;
		break;
	case 1:	
		BR_PaletteFade( &pwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_DeleteSub( wk );
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );
		break;


	case 2:
		///< はい・いいえの呼び出し		
		SaveWin_Add( wk );
		Tool_InfoMessageMain( wk, msg_info_027 );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_answer_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
//		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
//		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 3:
		BR_PaletteFade( &pwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );		
		break;
	
	case 4:
		///< 最初　はい・いいえ
		if ( GF_TP_SingleHitTrg( &hit_table[ 0 ] ) ){
			pwk->video_check_state = eVC_YES;
			TouchAction( wk );
			if ( SaveData_GetTotalSaveFlag( wk->save ) ){
				Tool_InfoMessageMainSet( wk, msg_info_029 );			///< たくさん
			}
			else {
				Tool_InfoMessageMainSet( wk, msg_info_030 );			///< そーでもない
			}
		//	sys_SoftResetNG( SOFTRESET_TYPE_SAVELOAD );					///< ソフトリセット禁止
			wk->sub_seq++;
		}
		else if ( GF_TP_SingleHitTrg( &hit_table[ 1 ] ) ){
			pwk->video_check_state = eVC_NO;
			TouchAction( wk );
		//	BR_Main_SeqChange( wk, eDEL_Exit );
			wk->sub_seq = 6;
		}
		break;
	
	case 5:
		///< レポートを書く
		SaveData_Save( wk->save );
	//	sys_SoftResetOK( SOFTRESET_TYPE_SAVELOAD );						///< ソフトリセット許可
		wk->sub_seq++;
		break;
	
	case 6:
		Tool_InfoMessageMainDel( wk );
		SaveWin_Del( wk );
		wk->sub_seq++;
		break;
	
	case 7:
		BR_PaletteFade( &pwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){		
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );		
		break;

	default:
		if ( pwk->video_check_state == eVC_YES ){
			BR_Main_SeqChange( wk, eBR_ProfPlay );
		}
		else {
			pwk->video_check_state = eVC_NONE;
			pwk->view_page = 0;
			BR_Main_SeqChange( wk, eBR_ProfIn );
		}
		break;
	}
	
	return FALSE;
}

static BOOL BR_PlayVideoFlagSave( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		Tool_InfoMessageMain( wk, msg_info_030 );
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 1:	
		if ( WIPE_SYS_EndCheck() == TRUE ){	
		//	sys_SoftResetNG( SOFTRESET_TYPE_SAVELOAD );						///< ソフトリセット禁止
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< フラグセーブ
		if ( wk->ex_param3 == FALSE ){
			{
				LOAD_RESULT ret;
				SAVE_RESULT result;

				//対戦録画データのロード
				BattleRec_Load( wk->save, HEAPID_BR, &ret, NULL, wk->ex_param1 );

				result = BattleRec_SecureSetSave( wk->save, wk->ex_param1 );

				OS_Printf( " result = %d\n", result );

				BattleRec_Exit();

				BR_SaveData_GppHead_Delete( wk );	///< GPP データ破棄
				BR_SaveData_GppHead_Load( wk );		///< GPP データの再読み込み	
				
			//	sys_SoftResetOK( SOFTRESET_TYPE_SAVELOAD );					///< ソフトリセット許可
			}
		}
		wk->sub_seq++;
		break;

	default:
		Tool_InfoMessageMainDel( wk );
		FontButton_CreateSub( wk );
		BR_Main_SeqChange( wk, eBR_RecoverIn );
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	再生
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BR_ProfPlay( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:		
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 1:		
		if ( WIPE_SYS_EndCheck() == TRUE ){	
			wk->sub_seq++;
		}
		break;
	
	case 2:
		if ( pwk->video_check_state == eVC_NONE ){		///< ビデオチェックして、はいいいえを選んで無いときのみ
			CATS_SystemActiveSet( wk, TRUE );
			if ( pwk->view_page == 0 ){
				///< 概要削除
				VideoDataFree( &pwk->vtr_work, wk );
			}
			else {
				///< プロフィール削除
				GppDataFree( &pwk->gpp_work, wk );						
			}
			CATS_SystemActiveSet( wk, FALSE );
			BMN_Delete( pwk->btn_man[ 0 ] );
			ProfWin_Del( wk );
			FontButton_DeleteSub( wk );
		}
		
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp_Simple( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_RecoverSeqSet( wk, *(wk->proc_seq) );
		BR_PostMessage( wk->message, TRUE, 0 );
		
		wk->sub_seq++;
		break;
	
	default:
		sys_FreeMemoryEz( pwk );
		return TRUE;
	}
	return FALSE;
}

static BOOL BR_ProfExit( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	switch ( wk->sub_seq ){
	case 0:
		BMN_Delete( pwk->btn_man[ 0 ] );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );		
		if ( pwk->view_page == 0 ){
			VideoDataFree( &pwk->vtr_work, wk );
		}
		else {			
			GppDataFree( &pwk->gpp_work, wk );
		}
		ProfWin_Del( wk );
		wk->sub_seq++;
		break;

	case 1:	
		BR_PaletteFade( &pwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			FontButton_DeleteSub( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &pwk->color, eFADE_MODE_IN ) ){
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &pwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( pwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,  LOGO_PALETTE, pwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}	
	return FALSE;
}

static BOOL BR_RecoverIn( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 1:
		GppDataCreate( wk );
		pwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_000, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &pwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &pwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &pwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &pwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &pwk->eva, &pwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &pwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			if ( pwk->video_check_state == eVC_YES ){
				wk->sub_seq = 0xFF;
			}
			else {
				wk->sub_seq++;
			}
			pwk->video_check_state = eVC_NONE;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, pwk->color, wk->sys.logo_color );
		break;

	case 4:
		WIPE_SYS_Start( WIPE_PATTERN_WMS,
			    WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
			    WIPE_FADE_BLACK,  WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 5:		
		if ( WIPE_SYS_EndCheck() == TRUE ){	
			wk->sub_seq++;
		}
		break;

	default:
		wk->tag_man.stack_s += 1;				///< proc init で Menu_Backが呼ばれ1段階下がってしまうので応急処置
		BR_RecoverWorkClear( wk );		
		BR_Main_SeqChange( wk, eBR_ProfMain );
		break;
	}
	return FALSE;
}

// =============================================================================
//
//
//	■ツール的な
//
//
// =============================================================================
static void Btn_CallBack_000( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	PROF_WORK* pwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
// 	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBR_ProfChange );
		break;

	case 1:
		TouchAction( wk );
		
		///< 自分のビデオの場合と、一度再生して最後まで見ることのできたビデオに関してはセーブ無し
		OS_Printf( "ex_param1 = %d\n", wk->ex_param1 );
		OS_Printf( "secure flag = %d\n", RecHeader_ParamGet( pwk->vtr_work.br_head, RECHEAD_IDX_SECURE, 0 ) );
		if ( wk->ex_param1 == 0 
		||	 RecHeader_ParamGet( pwk->vtr_work.br_head, RECHEAD_IDX_SECURE, 0 ) == TRUE ){
			BR_Main_SeqChange( wk, eBR_ProfPlay );		///< 自分のビデオor再生の安全が確認されたもの
		}
		else {
			BR_Main_SeqChange( wk, eBR_VideoCheck );	///< ダウンロードしただけで、まだ再生確認ができていない
		}
		break;

	case 2:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBR_ProfExit );
		break;
	}
}

static void GppDataCreate( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;	
	pwk->gpp_work.gpp	  = wk->br_gpp[ wk->ex_param1 ];
	pwk->vtr_work.gpp	  = pwk->gpp_work.gpp;											///< ビデオデータのプロフにもＧＰＰのポインタ当てる
	pwk->vtr_work.br_head = wk->br_head[ wk->ex_param1 ];
}


static void ProfWin_Add( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	int				ofs = 1;
	
	win = &pwk->win_s[ 0 ];	
	GF_BGL_BmpWinInit( win );	
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 8, 3, 16, 2, eBG_PAL_FONT, ofs );
	
	ProfWin_MsgSet( wk, msg_718 );
}

static void ProfWin_Del( BR_WORK* wk )
{	
	PROF_WORK* pwk = wk->sub_work;

	GF_BGL_BmpWinOff( &pwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &pwk->win_s[ 0 ] );	
}

static void ProfWin_MsgSet( BR_WORK* wk, int id )
{
	PROF_WORK* pwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int x;
	
	win = &pwk->win_s[ 0 ];
	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, id );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_NORMAL, NULL );		
	STRBUF_Delete( str1 );		
	
	GF_BGL_BmpWinOnVReq( win );
}

static void FontButton_CreateSub( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	pwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &pwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	
	FontOam_MsgSet( &pwk->obtn[ 0 ], &wk->sys, msg_05 );
	CATS_ObjectAnimeSeqSetCap( pwk->obtn[ 0 ].cap, eTAG_EX_BACK );

	CATS_ObjectPosSetCap( pwk->obtn[ 0 ].cap,  80, 232 );
	FONTOAM_SetMat( pwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( pwk->obtn[ 0 ].font_obj, TRUE );
}

static void FontButton_DeleteSub( BR_WORK* wk )
{
	PROF_WORK* pwk = wk->sub_work;
	
	FontOam_Delete( &pwk->obtn[ 0 ] );
	CATS_ActorPointerDelete_S( pwk->obtn[ 0 ].cap );
	ExTag_ResourceDelete( wk );
}
