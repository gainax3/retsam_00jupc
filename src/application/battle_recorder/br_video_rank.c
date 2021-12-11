//==============================================================================
/**
 * @file	br_weekly_rank.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2008.01.08(火)
 *
 * msg_br_rank_501,msg_br_rank_501,に色々な解説等を書いてもよい
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

/*
	上画面をメイン
	下画面をサブ	
	に入れ替えて使用しています。
*/

typedef struct {

	int				eva;
	int				evb;
	int				color;

	int				ranking_mode;
	int				view_page;

	GF_BGL_BMPWIN	win_m[ 8 ];
	GF_BGL_BMPWIN	win_s[ 8 ];

	OAM_BUTTON		obtn[ 4 ];				///< 送る　戻る　送るは常にビジブル　使いまわしで ３人のデータ＋もどる
	
	GT_BATTLE_REC_RECV*	rec_data;
	
	BUTTON_MAN*		btn_man[ 3 ];
	
	GPP_WORK		gpp_work;	
	VIDEO_WORK		vtr_work;
	
	GT_BATTLE_REC_RECV	save_data;
	
	int				save_no;
	int				win_s_num;
	int				win_m_num;
	int				tmp_seq;
	
	BOOL			bData;

	///< リスト用
	POKE_SEL_WORK		poke_select;
	TL_INFO*			tl_poke_list_info;	///< どーしても可変になるので ポケモンの個数Alloc
	TOUCH_LIST_HEADER	tl_poke_list_head;	///< どーしても可変になるので
	
	POKE_ICON_WORK		icon;

} RANK_WORK;

static const TOUCH_LIST_HEADER tl_head_video_list_dmy = {
	NULL,
	6,
	3,					///< 2行幅

	2,8,
	10,15,
	128,
	GF_BGL_FRAME2_M,

	5,					///< 表示件数
	NULL,				///< 当たり判定
};

static const RECT_HIT_TBL hit_table_000[] = {
	{ DTC(  2 ), DTC(  6 ), DTC(  8 ), DTC( 25 ) },	///< バトルビデオ	
	{ DTC(  8 ), DTC( 14 ), DTC( 11 ), DTC( 21 ) },	///< 再生	
	{ 152, 152 + 32, 128, 128 + 96 },	///< 戻る
	{ 152, 152 + 32,  32,  32 + 96 },	///< 保存
};

///< 保存先
static const RECT_HIT_TBL hit_table_001[] = {	
	{ TAG_HIT_Y( 0 ) - 16, TAG_HIT_Y( 0 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 1 ) - 16, TAG_HIT_Y( 1 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 2 ) - 16, TAG_HIT_Y( 2 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },
	{ TAG_HIT_Y( 3 ) - 16, TAG_HIT_Y( 3 ) + 16, TAG_HIT_X, TAG_HIT_X + 160 },	
};

///< 上書きしますか？はい・いいえ スクリーン
static const RECT_HIT_TBL hit_table_002[] = {
	{ DTC(  5 ), DTC( 9 ), DTC(  4 ), DTC( 15 ) },	///< はい
	{ DTC(  5 ), DTC( 9 ), DTC( 18 ), DTC( 29 ) },	///< いいえ	
};

static const RECT_HIT_TBL hit_table_back[] = {	
	{ 152, 184, 80, 176 },	///< 戻る
};

static const RECT_HIT_TBL hit_table_view[] = {
	{ DTC( 4 ), DTC( 12 ), DTC( 10 ), DTC( 22 ) },	///< ビデオを見る
};

static void ListPokeIcon_PosSet( BR_WORK* wk )
{
	const s16 icon_pos[][ 2 ] = {
		{ 13 * 8, 9 * 8 },{ 16 * 8, 9 * 8 },{ 19 * 8, 9 * 8 },{ 22 * 8, 9 * 8 },{ 25 * 8, 9 * 8 },{ 28 * 8, 9 * 8 },
		{ 13 * 8,12 * 8 },{ 16 * 8,12 * 8 },{ 19 * 8,12 * 8 },{ 22 * 8,12 * 8 },{ 25 * 8,12 * 8 },{ 28 * 8,12 * 8 },
		{ 13 * 8,15 * 8 },{ 16 * 8,15 * 8 },{ 19 * 8,15 * 8 },{ 22 * 8,15 * 8 },{ 25 * 8,15 * 8 },{ 28 * 8,15 * 8 },
		{ 13 * 8,18 * 8 },{ 16 * 8,18 * 8 },{ 19 * 8,18 * 8 },{ 22 * 8,18 * 8 },{ 25 * 8,18 * 8 },{ 28 * 8,18 * 8 },
		{ 13 * 8,21 * 8 },{ 16 * 8,21 * 8 },{ 19 * 8,21 * 8 },{ 22 * 8,21 * 8 },{ 25 * 8,21 * 8 },{ 28 * 8,21 * 8 },
	};
	int i,j;
	RANK_WORK* rwk = wk->sub_work;
	
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( rwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectPosSetCap( rwk->icon.poke[ i ][ j ].cap, icon_pos[ ( i * 6 ) + j ][ 0 ], icon_pos[ ( i * 6 ) + j ][ 1 ] );
			}			
		}
	}
}


//==============================================================
// Prototype
//==============================================================
static void ListPokeIcon_PosSet( BR_WORK* wk );
static void FontButton_CreateSub( BR_WORK* wk );
static void InfoMessageSet( BR_WORK* wk, int no, int data_no );
static BOOL VRANK_BrsDataGet( BR_WORK* wk );
static void Video_GppDataDelete( BR_WORK* wk );
static BOOL VRANK_CheckMain( BR_WORK* wk );
static BOOL VRANK_CheckExit( BR_WORK* wk );
static BOOL VRANK_CheckFinish( BR_WORK* wk );
static BOOL VRANK_CheckIn( BR_WORK* wk );
static BOOL VRANK_SaveMain( BR_WORK* wk );
static BOOL VRANK_SaveExit( BR_WORK* wk );
static BOOL VRANK_PlayRecover( BR_WORK* wk );
static BOOL VRANK_ProfExit( BR_WORK* wk );
static void ButtonOAM_PosSet( BR_WORK* wk, int mode );
static void FontButton_Create( BR_WORK* wk );
static void FontOam_SaveData_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no );
static void FontOam_Message_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no );
static void SaveFontButton_Create( BR_WORK* wk );
static void SaveFontButton_Delete( BR_WORK* wk );
static void FontButton_Delete( BR_WORK* wk );
static void ButtonOAM_PosSetSub( BR_WORK* wk, int mode );
static void BRV_WinDel_Main( BR_WORK* wk );
static void BRV_WinDel_Sub( BR_WORK* wk );
static BOOL VRANK_Init( BR_WORK* wk );
static BOOL VRANK_Setting( BR_WORK* wk );
static BOOL VRANK_Send_Recv( BR_WORK* wk );
static void BRV_WinAdd_VideoList( BR_WORK* wk );
static BOOL VRANK_MainIn( BR_WORK* wk );
static BOOL VRANK_Main( BR_WORK* wk );
static BOOL VRANK_Exit( BR_WORK* wk );
static BOOL VRANK_ProfIn( BR_WORK* wk );
static BOOL VRANK_ProfMain( BR_WORK* wk );
static BOOL VRANK_ProfChange( BR_WORK* wk );
static BOOL VRANK_Play( BR_WORK* wk );
static BOOL VRANK_RecoverIn( BR_WORK* wk );
static void InfoMessage( BR_WORK* wk, int no, BOOL flag );
static BOOL VRANK_SaveIn( BR_WORK* wk );
static void ProfWin_Del( BR_WORK* wk );
static void ProfWin_MsgSet( BR_WORK* wk, int id );
static void ProfWin_Add( BR_WORK* wk );
static void SaveWin_Add( BR_WORK* wk );
static void SaveWin_Del( BR_WORK* wk );
static void Btn_CallBack_200( u32 button, u32 event, void* work );
static void Btn_CallBack_201( u32 button, u32 event, void* work );
static void Btn_CallBack_202( u32 button, u32 event, void* work );
static void Video_GppDataCreate_from_OutLine( BR_WORK* wk );
static void ListPokeIcon_Add( BR_WORK* wk );
static void ListPokeIcon_Enable( BR_WORK* wk, BOOL flag );
static void ListPokeIcon_Del( BR_WORK* wk );
static void ListPokeIcon_ReAdd( BR_WORK* wk );
static void FontButton_DeleteSub( BR_WORK* wk );
static void BR_Prof_Save( BR_WORK* wk, int no );


static void FontButton_CreateSub( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	rwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	rwk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &rwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &rwk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );

	FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
	FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
	CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
	CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );

	CATS_ObjectPosSetCap( rwk->obtn[ 0 ].cap,  32, 232 );
	CATS_ObjectPosSetCap( rwk->obtn[ 1 ].cap, 128, 232 );
	FONTOAM_SetMat( rwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( rwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( rwk->obtn[ 0 ].font_obj, TRUE );
	FONTOAM_SetDrawFlag( rwk->obtn[ 1 ].font_obj, TRUE );
		
	ButtonOAM_PosSetSub( wk, 0 );	
}


static void InfoMessageSet( BR_WORK* wk, int no, int data_no )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	RANK_WORK* rwk = wk->sub_work;

	if ( no == msg_608 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = rwk->gpp_work.gpp;
		
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
	else if ( no == msg_610 ){
		
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


static BOOL VRANK_BrsDataGet( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;
	
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }

	switch ( wk->sub_seq ){
	case 0:
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		if ( rwk->view_page == 0 ){
			VideoData_IconEnable( &rwk->vtr_work, FALSE );
		}
		else {
			GppData_IconEnable( &rwk->gpp_work, FALSE );
		}
		wk->sub_seq++;
		
	case 1:		
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		///< ビデオを探しています。
		Tool_InfoMessageMain( wk, msg_714 );
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		rwk->bData = FALSE;
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 3:		
		if ( GDSRAP_Tool_Send_BattleVideo_DataDownload( BR_GDSRAP_WorkGet( wk ), 
			 wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->head.data_number ) == TRUE ){
			wk->sub_seq++;
		}				
		break;
		
	case 4:		
		Tool_InfoMessageMainDel( wk );
		if ( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorSet( wk, eERR_VIDEO_DATA_GET, error_info->result, error_info->type );
			wk->sub_seq++;
			rwk->bData = FALSE;
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			wk->sub_seq = 0xFF;
			rwk->bData  = TRUE;
			wk->secure_addrs  = &wk->recv_data.outline_secure_flag[ wk->sys.touch_list.view.this ];
			///< その後再生するかもしれないので、テンポラリにコピー
			rwk->rec_data = wk->recv_data.rec_data_tmp;
			MI_CpuCopy8( rwk->rec_data, &wk->recv_data.rec_data, sizeof( GT_BATTLE_REC_RECV ) );
		}
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		break;

	default:
		if ( rwk->bData ){
			///< ビデオが見つかった場合は、先へ進む
			Snd_SePlay( eBR_SND_RECV_OK );
			BR_Main_SeqChange( wk, rwk->tmp_seq );
			RecData_RecStopFlagSet( wk, TRUE );
		}
		else {
			///< 見つからない場合は、プロフィールへ復帰　たぶんセーブからの復帰で問題ないはず･･･
			BR_Main_SeqChange( wk, eVRANK_PlayRecover );
		}
		break;
	}

	return FALSE;
}

static void Video_GppDataDelete( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	BattleRec_Header_FreeMemory( rwk->vtr_work.br_head );
	GDS_Profile_FreeMemory( rwk->gpp_work.gpp );	
}

static BOOL VRANK_CheckMain( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	BMN_Main( rwk->btn_man[ 2 ] );

	return FALSE;
}

static BOOL VRANK_CheckExit( BR_WORK* wk )
{	
	RANK_WORK* rwk = wk->sub_work;

	switch( wk->sub_seq ){
	case 0:
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			SaveWin_Del( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			wk->sub_seq++;
		}
		break;
	
	case 2:
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			InfoMessageSet( wk, msg_608, 0 );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eVRANK_SaveMain );
		break;
		return TRUE;
	}
	return FALSE;
}

static BOOL VRANK_CheckFinish( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	GDS_RAP_ERROR_INFO* error_info;

	switch( wk->sub_seq ){
	case 0:
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
		//	SaveWin_Del( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			InfoMessageSet( wk, msg_info_009, 0 );
			BR_SaveWork_Clear( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;

	case 2:
		OS_Printf( "rec secure = %d\n", wk->recv_data.outline_secure_flag[ wk->sys.touch_list.view.this ] );
		if ( RecData_Save( wk, rwk->save_no, wk->recv_data.outline_secure_flag[ wk->sys.touch_list.view.this ] ) ){
			wk->sub_seq++;
		}
		break;
	
	case 3:
	//	InfoMessageSet( wk, msg_1003, 0 );
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		wk->sub_seq++;
		Snd_SePlay( eBR_SND_SEARCH );
		break;
	
	case 4:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		InfoMessageSet( wk, msg_info_011, 0 );	
		{
			u64 data_no = RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 );
				
			if ( GDSRAP_Tool_Send_BattleVideo_FavoriteUpload( BR_GDSRAP_WorkGet( wk ), data_no ) == TRUE ){
				wk->sub_seq++;
			}
		}
		break;
	
	case 5:
		if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		
		if ( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			//エラー処理はいらない
			wk->sub_seq++;
		}
		else{
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_RECV_OK );
			wk->sub_seq++;
		}
		InfoMessageSet( wk, msg_info_012, 0 );	
		break;
	
	case 6:
		///< ここから終了へ向かう
		if ( !GF_TP_GetTrg() ){ break; }
		InfoMessageDel( wk );
		CATS_SystemActiveSet( wk, TRUE );
		SaveFontButton_Delete( wk );	
		CATS_SystemActiveSet( wk, FALSE );	
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );

		BR_SaveData_GppHead_Delete( wk );						///< GPP データ破棄
		BR_SaveData_GppHead_Load( wk );							///< GPP データの再読み込み					
		TouchList_WorkClear( &wk->sys.touch_list );	
		wk->sub_seq++;
		break;
	
	case 7:
		BMN_Delete( rwk->btn_man[ 0 ] );
		BMN_Delete( rwk->btn_man[ 1 ] );
		BMN_Delete( rwk->btn_man[ 2 ] );
		Video_GppDataDelete( wk );
		BR_PaletteFadeIn_Init( &rwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );	
		CATS_SystemActiveSet( wk, FALSE );	
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_BR );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &rwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &rwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );				
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			sys_FreeMemoryEz( rwk );
			if ( BattleRec_DataExistCheck() == TRUE ){
				BattleRec_Exit();
			}
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}

	return FALSE;
}

static BOOL VRANK_CheckIn( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_answer_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			InfoMessageSet( wk, msg_610, rwk->save_no );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	case 1:
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			SaveWin_Add( wk );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );			
			wk->sub_seq++;
		}
		break;

	default:
		BR_Main_SeqChange( wk, eVRANK_CheckMain );
		break;
	}	
	
	return FALSE;
}

static BOOL VRANK_PlayRecover( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	case 2:
	//	FontButton_CreateSub( wk );
		if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
			///< この時点だとボタンのタグ情報が変わってるので入れ替える
			FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		}
		ProfWin_Del( wk );
		ProfWin_Add( wk );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		CATS_SystemActiveSet( wk, TRUE );
		if ( rwk->view_page == 0 ){
			///< プロフィール削除 概要作成
			VideoDataFree( &rwk->vtr_work, wk );
			VideoDataMake( &rwk->vtr_work, wk );
			VideoData_IconEnable( &rwk->vtr_work, FALSE );
//			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール作成
			GppDataFree( &rwk->gpp_work, wk );
			GppDataMake( &rwk->gpp_work, wk );
			GppData_IconEnable( &rwk->gpp_work, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 3:
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			if ( rwk->view_page == 0 ){
				VideoData_IconEnable( &rwk->vtr_work, TRUE );
			}
			else {
				GppData_IconEnable( &rwk->gpp_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );		
		break;

	default:
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		BR_Main_SeqChange( wk, eVRANK_ProfMain );
		break;
	}
	return FALSE;
}


static BOOL VRANK_SaveMain( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	BMN_Main( rwk->btn_man[ 1 ] );

	return FALSE;
}

static BOOL VRANK_SaveExit( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
	//	InfoMessageDel( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			SaveFontButton_Delete( wk );
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		FontButton_CreateSub( wk );
		if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
			///< この時点だとボタンのタグ情報が変わってるので入れ替える
			FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		}
		ProfWin_Add( wk );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		CATS_SystemActiveSet( wk, TRUE );
		if ( rwk->view_page == 0 ){
			///< プロフィール削除 概要作成
			VideoDataMake( &rwk->vtr_work, wk );
			VideoData_IconEnable( &rwk->vtr_work, FALSE );
//			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール作成
			GppDataMake( &rwk->gpp_work, wk );
			GppData_IconEnable( &rwk->gpp_work, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		wk->sub_seq++;
		break;
	
	case 3:
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			if ( rwk->view_page == 0 ){
				VideoData_IconEnable( &rwk->vtr_work, TRUE );
			}
			else {
				GppData_IconEnable( &rwk->gpp_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );		
		break;

	default:
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		BR_Main_SeqChange( wk, eVRANK_ProfMain );
		break;
	}
	return FALSE;
}

static BOOL VRANK_ProfExit( BR_WORK* wk )
{	
	RANK_WORK* rwk = wk->sub_work;
	switch ( wk->sub_seq ){
	case 0:
		BMN_Delete( rwk->btn_man[ 0 ] );
		BMN_Delete( rwk->btn_man[ 1 ] );
		BMN_Delete( rwk->btn_man[ 2 ] );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		Video_GppDataDelete( wk );
		CATS_SystemActiveSet( wk, TRUE );
		if ( rwk->view_page == 0 ){
			VideoDataFree( &rwk->vtr_work, wk );
		}
		else {			
			GppDataFree( &rwk->gpp_work, wk );
		}
		CATS_SystemActiveSet( wk, FALSE );
		ProfWin_Del( wk );
		wk->sub_seq++;
		break;

	case 1:	
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
		//	FontButton_DeleteSub( wk );				
			FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
			FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_708 );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
			CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_OK );		
			ButtonOAM_PosSetSub( wk, 1 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eVRANK_MainIn );
		break;
	}
	return FALSE;
}

BOOL (* const BR_VideoRanking_MainTable[])( BR_WORK* wk ) = {
	VRANK_Init,
	VRANK_Setting,
	VRANK_Send_Recv,
	VRANK_MainIn,
	VRANK_Main,
	VRANK_Exit,

	VRANK_ProfIn,
	VRANK_ProfMain,
	VRANK_ProfChange,	
	VRANK_Play,
	VRANK_ProfExit,
	
	VRANK_SaveIn,
	VRANK_SaveMain,
	VRANK_SaveExit,
	
	VRANK_CheckIn,
	VRANK_CheckMain,
	VRANK_CheckExit,
	VRANK_CheckFinish,
	
	VRANK_BrsDataGet,
	VRANK_RecoverIn,
	VRANK_PlayRecover,
};

#define PRINT_COL_RANK	( GF_PRINTCOLOR_MAKE( 15, 13, 0 ) )

/// font button
static void ButtonOAM_PosSet( BR_WORK* wk, int mode )
{
	RANK_WORK* rwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( rwk->obtn[ 0 ].cap, TRUE );
		FONTOAM_SetDrawFlag( rwk->obtn[ 0 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( rwk->obtn[ 1 ].cap, 128, 232 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( rwk->obtn[ 0 ].cap, FALSE );
		FONTOAM_SetDrawFlag( rwk->obtn[ 0 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( rwk->obtn[ 1 ].cap, 80, 232 );
	}
	FONTOAM_SetMat( rwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( rwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}

static void FontButton_Create( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	rwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	rwk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &rwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &rwk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_602 );
	FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_05 );

	CATS_ObjectPosSetCap( rwk->obtn[ 0 ].cap,  32, 232 );
	CATS_ObjectPosSetCap( rwk->obtn[ 1 ].cap, 128, 232 );
	FONTOAM_SetMat( rwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( rwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( rwk->obtn[ 0 ].font_obj, FALSE );
	FONTOAM_SetDrawFlag( rwk->obtn[ 1 ].font_obj, TRUE );
	CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, 3 );
	CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, 0 );
	
//	rwk->sbtn[ 0 ] = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
//	rwk->sbtn[ 1 ] = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
//	CATS_ObjectPosSetCap( rwk->sbtn[ 0 ],  36, 160 + 64 );
//	CATS_ObjectPosSetCap( rwk->sbtn[ 1 ], 220, 160 + 64 );
//	CATS_ObjectAnimeSeqSetCap( rwk->sbtn[ 0 ], 5 );
//	CATS_ObjectAnimeSeqSetCap( rwk->sbtn[ 1 ], 5 );
//	CATS_ObjectEnableCap( rwk->sbtn[ 0 ], FALSE );
//	CATS_ObjectEnableCap( rwk->sbtn[ 1 ], FALSE );
	
	ButtonOAM_PosSet( wk, 1 );	
}

static void FontOam_SaveData_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	
	WORDSET*			wset;
	STRBUF*				tmp1;
	STRBUF*				tmp2;
	
	man = wk->sys.man;
	
	if ( wk->br_gpp[ no ] == NULL ){
		str	= MSGMAN_AllocString( man, msg_10 );
	}
	else {
		wset = BR_WORDSET_Create( HEAPID_BR );
		tmp1 = MSGMAN_AllocString( wk->sys.man, msg_09 );
		tmp2 = GDS_Profile_CreateNameString( wk->br_gpp[ no ], HEAPID_BR );
		BR_ErrorStrChange( wk, tmp2 );
		str	 = STRBUF_Create( 255, HEAPID_BR );

		WORDSET_RegisterWord( wset, 0, tmp2, 0, TRUE, PM_LANG );	
		WORDSET_ExpandStr( wset, str, tmp1 );
		
		STRBUF_Delete( tmp1 );
		STRBUF_Delete( tmp2 );
		WORDSET_Delete( wset );	
	}
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( obtn->font_obj, obtn->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);	
}

static void FontOam_Message_Set( OAM_BUTTON* obtn, BR_WORK* wk, int no )
{
	STRBUF*				str;
	MSGDATA_MANAGER*	man;
	GF_BGL_BMPWIN		bmpwin;	
	
	WORDSET*			wset;
	STRBUF*				tmp1;
	STRBUF*				tmp2;
	
	man = wk->sys.man;
	
	str	= MSGMAN_AllocString( man, no );
	
	GF_BGL_BmpWinInit( &bmpwin );
	GF_BGL_BmpWinObjAdd( wk->sys.bgl, &bmpwin, BR_TAG_BMP_WIX_SX, BR_TAG_BMP_WIX_SY, 0, 0 );
	GF_STR_PrintColor( &bmpwin, FONT_SYSTEM, str, 0, 0, MSG_NO_PUT, PRINT_COL_BLACK, NULL );
	
	FONTOAM_OAMDATA_ResetBmp( obtn->font_obj, obtn->font_data, &bmpwin, HEAPID_BR );
		
	STRBUF_Delete(str);
	GF_BGL_BmpWinDel(&bmpwin);	
}

static void SaveFontButton_Create( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	int i;
	int mes[] = { msg_10, msg_10, msg_10, msg_05, };
	int anime[] = { eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_DATA, eTAG_GDS_SAVE_BACK };

	ExTag_ResourceLoad_Save( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	
	for ( i = 0; i < 4; i++ ){
		rwk->obtn[ i ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
		FontOam_Create( &rwk->obtn[ i ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
		FontOam_MsgSet( &rwk->obtn[ i ], &wk->sys, mes[ i ] );
		if ( i != 3 ){
			FontOam_SaveData_Set( &rwk->obtn[ i ], wk, i + 1 );
		}
		CATS_ObjectPosSetCap( rwk->obtn[ i ].cap,  TAG_MAIN_POS + 8, TAG_INIT_POS( i, 4 ) + 64 );
		CATS_ObjectAnimeSeqSetCap( rwk->obtn[ i ].cap, anime[ i ] );
		FONTOAM_SetMat( rwk->obtn[ i ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
		FONTOAM_SetDrawFlag( rwk->obtn[ i ].font_obj, TRUE );
	}
}

static void SaveFontButton_Delete( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	int i;
	
	for ( i = 0; i < 4; i++ ){
		FontOam_Delete( &rwk->obtn[ i ] );
		CATS_ActorPointerDelete_S( rwk->obtn[ i ].cap );
	}
	
	ExTag_ResourceDelete( wk );
}

static void FontButton_Delete( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	FontOam_Delete( &rwk->obtn[ 0 ] );
	FontOam_Delete( &rwk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( rwk->obtn[ 0 ].cap );
	CATS_ActorPointerDelete_S( rwk->obtn[ 1 ].cap );
	
	ExTag_ResourceDelete( wk );			
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
static void ButtonOAM_PosSetSub( BR_WORK* wk, int mode )
{
	RANK_WORK* rwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( rwk->obtn[ 1 ].cap, TRUE );
		FONTOAM_SetDrawFlag( rwk->obtn[ 1 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( rwk->obtn[ 0 ].cap, 32, 232 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( rwk->obtn[ 1 ].cap, FALSE );
		FONTOAM_SetDrawFlag( rwk->obtn[ 1 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( rwk->obtn[ 0 ].cap,  80, 232 );
	}
	FONTOAM_SetMat( rwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( rwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}

//--------------------------------------------------------------
/**
 * @brief	メイン削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinDel_Main( BR_WORK* wk )
{
	int i;
	RANK_WORK* rwk = wk->sub_work;
	
	for ( i = 0; i < rwk->win_m_num; i++ ){
		GF_BGL_BmpWinOff( &rwk->win_m[ i ] );
		GF_BGL_BmpWinDel( &rwk->win_m[ i ] );			
	}
	rwk->win_m_num = 0;
}

//--------------------------------------------------------------
/**
 * @brief	サブ削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BRV_WinDel_Sub( BR_WORK* wk )
{
	int i;
	RANK_WORK* rwk = wk->sub_work;
	
	for ( i = 0; i < rwk->win_s_num; i++ ){
		GF_BGL_BmpWinOff( &rwk->win_s[ i ] );
		GF_BGL_BmpWinDel( &rwk->win_s[ i ] );			
	}
	rwk->win_s_num = 0;
}

//--------------------------------------------------------------
/**
 * @brief	初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_Init( BR_WORK* wk )
{
	RANK_WORK* rwk = sys_AllocMemory( HEAPID_BR, sizeof( RANK_WORK ) );
	
	memset( rwk, 0, sizeof( RANK_WORK ) );
	
	wk->sub_work = rwk;
	
//	rwk->ranking_mode	= wk->ex_param1;

	if ( wk->ex_param1 == BR_EX_DATA_BATTLE_RANK ){
		rwk->ranking_mode = 0;
	}
	else if ( wk->ex_param1 == BR_EX_DATA_FRONTIER_RANK ){
		rwk->ranking_mode = 1;
	}
	else if ( wk->ex_param1 == BR_EX_DATA_NEW30 ){
		rwk->ranking_mode = 2;
	}
	
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	Plate_AlphaInit_Default( &rwk->eva, &rwk->evb, ePLANE_ALL );

	BR_Main_SeqChange( wk, eVRANK_Setting );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	準備
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_Setting( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &rwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 rwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 rwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 1 );			///< priority
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 1 );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );		
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );				///< main = top		
		Cursor_Visible( wk->cur_wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 2:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }

		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete			
		CATS_SystemActiveSet( wk, FALSE );

		CATS_SystemActiveSet( wk, TRUE );
		FontButton_CreateSub( wk );
		CATS_SystemActiveSet( wk, FALSE );

		wk->sub_seq++;
		break;

	default:
		if ( BR_RecoverCheck( wk ) == TRUE ){
			BR_Main_SeqChange( wk, eVRANK_RecoverIn );
		}
		else {
			BR_Main_SeqChange( wk, eVRANK_SendRecv );
		}
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	ランキング受信
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_Send_Recv( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	GDS_RAP_ERROR_INFO *error_info;
		
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
	
	switch ( wk->sub_seq ){
	case 0:
		///< 初期化
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		if ( wk->ex_param1 == BR_EX_DATA_BATTLE_RANK 
		||	 wk->ex_param1 == BR_EX_DATA_FRONTIER_RANK ){
			Tool_InfoMessageMain( wk, msg_info_010 );
		}
		else {
			Tool_InfoMessageMain( wk, msg_info_023 );
		}
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
		
	case 1:
		///< 受信
		if ( wk->ex_param1 == BR_EX_DATA_BATTLE_RANK ){
			rwk->ranking_mode = 0;
			OS_Printf( "通信対戦ランキング問い合わせ\n" );
			if ( GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Colosseum( BR_GDSRAP_WorkGet( wk ) ) == TRUE ){
				wk->sub_seq++;
			}
		}
		else if ( wk->ex_param1 == BR_EX_DATA_FRONTIER_RANK ){
			rwk->ranking_mode = 1;
			OS_Printf( "フロンティアランキング問い合わせ\n" );
			if ( GDSRAP_Tool_Send_BattleVideoFavoriteDownload_Frontier( BR_GDSRAP_WorkGet( wk ) ) == TRUE ){
		//		Snd_SePlay( eBR_SND_SEARCH );
				wk->sub_seq++;
			}
		}
		else if ( wk->ex_param1 == BR_EX_DATA_NEW30 ){
			rwk->ranking_mode = 2;
			if ( BR_IsGameClear( wk ) == TRUE ){
				OS_Printf( "最新30件問い合わせ --- クリア済み" );
				if ( GDSRAP_Tool_Send_BattleVideoNewDownload( BR_GDSRAP_WorkGet( wk ) ) == TRUE ){
		//			Snd_SePlay( eBR_SND_SEARCH );
					wk->sub_seq++;
				}
			}
			else {
				OS_Printf( "最新30件問い合わせ --- クリアしてない" );
				if ( GDSRAP_Tool_Send_BattleVideoNewDownload_ColosseumOnly( BR_GDSRAP_WorkGet( wk ) ) == TRUE ){
		//			Snd_SePlay( eBR_SND_SEARCH );
					wk->sub_seq++;
				}
			}
		}
		else {
			GF_ASSERT( 0 );
		}
		break;
	
	case 2:
		Tool_InfoMessageMainDel( wk );
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			//エラー発生時の処理
			BR_ErrorSet( wk, eERR_VIDEO_SEARCH, error_info->result, error_info->type );
			wk->sub_seq++;
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_RECV_OK );
			if ( wk->recv_data.outline_data_num == 0 ){
				BR_ErrorSet( wk, eERR_RANKING_SEND, 0, 0 );
				if ( rwk->ranking_mode == 2 ){
					wk->sys.comm_err_wk.err_msg_id = msg_715;
				}
				else {
					wk->sys.comm_err_wk.err_msg_id = msg_info_031;
				}
				Cursor_Visible( wk->cur_wk, FALSE );				
				wk->sub_seq++;				
			}
			else {
				wk->sub_seq = 0xFF;
			}
		}
		break;
	
	case 3:
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		wk->sub_seq++;
		break;
	
	case 4:
		///< フェードアウト
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){			
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	case 5:
		///< 復帰
		CATS_SystemActiveSet( wk, TRUE );
		FontButton_Delete( wk );
		NormalTag_RecoverAllOp( wk );
		CATS_SystemActiveSet( wk, FALSE );
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_BR );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	case 6:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &rwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &rwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );				
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			sys_FreeMemoryEz( rwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;

	default:
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		BR_Main_SeqChange( wk, eVRANK_MainIn );
		break;
	}
	
	return FALSE;
}

static void BRV_WinAdd_VideoList( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	int x = 0;
	int ofs = 32;
	STRBUF* str1;
	GF_BGL_BMPWIN* win;
	int msg_list[] = { msg_605, msg_606, msg_720 };
	
	///< 見出し
	win = &rwk->win_m[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 3, 3, 20, 4, eBG_PAL_FONT, ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_list[ rwk->ranking_mode ] );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, 0, MSG_NO_PUT, PRINT_COL_RANK, NULL );
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );

	///< ビデオを見る
	ofs = 256;
	win = &rwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 11, 6, 10, 4, eBG_PAL_FONT, ofs );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_603 );
    // MatchComment: new change in plat US
    {
        u32 width = (80 - FontProc_GetPrintMaxLineWidth(FONT_SYSTEM, str1, 0)) / 2;
        GF_STR_PrintColor( win, FONT_SYSTEM, str1, width, 0, MSG_NO_PUT, PRINT_COL_RANK, NULL );
    }
    // MatchCommentEnd -------------------
	GF_BGL_BmpWinOnVReq( win );
	STRBUF_Delete( str1 );
	
	rwk->win_s_num = 1;
	rwk->win_m_num = 1;
}

//--------------------------------------------------------------
/**
 * @brief	メイン IN
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_MainIn( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< データ一覧の準備
		if ( BattleRec_DataExistCheck() ){	///< 読み込んだ録画データはいったん破棄
			BattleRec_Exit();
		}
		rwk->tl_poke_list_head = tl_head_video_list_dmy;								///< リストヘッダーのベースはコピーする
		rwk->tl_poke_list_head.info = NULL;												///< infoは書き換え
		rwk->tl_poke_list_head.data_max = wk->recv_data.outline_data_num;				///< データ個数も書き換え
		PokemonIcon_ResLoad( wk );														///< ポケモンアイコンのリソース読み込み
		rwk->icon.handle = PokemonGraphic_ArcOpen( HEAPID_BR );							///< アイコンのアークを開く
		rwk->icon.global_gra_id = GLOBAL_POKE_ICON_ID;	///< 気持ちずらす
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_u_NSCR,  GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d1_NSCR, GF_BGL_FRAME3_S );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );	
		
		///< 先読み非表示
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		ButtonOAM_PosSetSub( wk, 1 );			///< OAMボタン1個
		BRV_WinAdd_VideoList( wk );
		
		CATS_SystemActiveSet( wk, TRUE );
		Slider_Init( &wk->sys.slider, wk );											///< スライダーの初期化	
		TouchList_InitEx( &wk->sys.touch_list, wk, NULL, &rwk->tl_poke_list_head );	///< リストの初期化
		wk->sys.touch_list.bView = TRUE;
		Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
		TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_MAIN );			///< リストのカーソル登録
		TouchList_CursorPosSet( &wk->sys.touch_list, 64, 184 );						///< カーソルの位置変更
		TouchList_ViewMode_Main( &wk->sys.touch_list, wk );							///< リスト更新を呼ぶ
		CATS_SystemActiveSet( wk, FALSE );

		CATS_SystemActiveSet( wk, TRUE );
		ListPokeIcon_Add( wk );														///< ポケモンアイコン登録
		ListPokeIcon_Enable( wk, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		
		wk->sub_seq++;
		break;
	
	case 1:
		///< 画面の表示
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );	
			ListPokeIcon_Enable( wk, TRUE );
			HormingCursor_Init( wk, wk->cur_wk, 16, 76 + ( wk->sys.touch_list.view.l_pos * 24 ) );
			Cursor_Visible( wk->cur_wk, TRUE );
			Cursor_R_Set( wk->cur_wk, LIST_HORMING_R, LIST_HORMING_R );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,  LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, rwk->color, wk->sys.logo_color );		///< 上下カーソルがタグパレットなので
		break;
	default:
		BR_Main_SeqChange( wk, eVRANK_Main );
		break;
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	メイン
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_Main( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< リスト処理
		Slider_HitCheck_Main( &wk->sys.slider );
		TouchList_ViewMode_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		TouchList_ViewMode_Main( &wk->sys.touch_list, wk );
		Cursor_PosSet_Check( wk->cur_wk, 16, 76 + ( wk->sys.touch_list.view.l_pos * 24 ) );
		ListPokeIcon_ReAdd( wk );														///< ポケモンアイコン再登録

		if ( GF_TP_SingleHitTrg( &hit_table_view[ 0 ] ) ){	///< ビデオを見る
			TouchAction( wk );
			if ( wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ] != NULL ){
				rwk->tmp_seq = eVRANK_ProfIn;
				rwk->view_page = 0;
				wk->sub_seq++;
			}
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){	///< 終わる
			TouchAction( wk );
			rwk->tmp_seq = eVRANK_Exit;
			wk->sub_seq++;
		}
#ifdef BR_AUTO_SERCH
	if ( g_debug_auto ){
		TouchAction( wk );
		rwk->tmp_seq = eVRANK_Exit;
		wk->sub_seq++;
	}
#endif
		break;
		
	case 1:
		///< 終了処理
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		PokemonGraphic_ArcClose( rwk->icon.handle );	///< ポケモンアイコンのアーカイブ閉じる
		ListPokeIcon_Del( wk );							///< ポケモンアイコン破棄
		PokemonIcon_ResFree( wk );						///< リソース解放
		BRV_WinDel_Main( wk );
		BRV_WinDel_Sub( wk );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );

		if ( rwk->tmp_seq == eVRANK_Exit ){		///< リストの位置は記憶しなくていい
			Slider_WorkClear( &wk->sys.slider );
			TouchList_WorkClear( &wk->sys.touch_list );
			rwk->icon.view_start = 0;
		}
		wk->sub_seq++;
	
	case 2:
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			ButtonOAM_PosSetSub( wk, 0 );
			if ( rwk->tmp_seq == eVRANK_ProfIn ){	///< 文字を書き換える
				FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
				FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
				CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
				CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );	
			}
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, rwk->color, wk->sys.logo_color );		///< 上下カーソルがタグパレットなので
		break;
	
	default:
		BR_Main_SeqChange( wk, rwk->tmp_seq );
		break;
	}	
		
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	終了
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL VRANK_Exit( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< OAM とか 文字とか消す
		BRV_WinDel_Main( wk );
		BRV_WinDel_Sub( wk );
		Tool_InfoMessageMainDel( wk );
		wk->sub_seq++;
		break;
	
	case 1:
		///< フェードアウト
		BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
            CATS_SystemActiveSet( wk, TRUE );
            FontButton_Delete( wk );
            CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		///< 復帰
		BR_PaletteFadeIn_Init( &rwk->color );
		NormalTag_RecoverAllOp( wk );
		WirelessIconEasy_HoldLCD( TRUE, HEAPID_BR );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		if ( BR_PaletteFade( &rwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &rwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );				
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			sys_FreeMemoryEz( rwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}

static BOOL VRANK_ProfIn( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		Video_GppDataCreate_from_OutLine( wk );
		rwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_200, wk, HEAPID_BR );
		rwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_201, wk, HEAPID_BR );
		rwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_202, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &rwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &rwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &rwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eVRANK_ProfMain );
		break;
	}
	
	return FALSE;
}

static BOOL VRANK_ProfMain( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	BMN_Main( rwk->btn_man[ 0 ] );
	
	return FALSE;
}

static BOOL VRANK_ProfChange( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		CATS_SystemActiveSet( wk, TRUE );
		if ( rwk->view_page == 0 ){
			///< 概要削除 プロフィール作成
			VideoDataFree( &rwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除 概要作成
			GppDataFree( &rwk->gpp_work, wk );						
		}
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			wk->sub_seq++;
		}
		break;
	
	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		if ( rwk->view_page == 0 ){
			///< プロフィール作成
			GppDataMake( &rwk->gpp_work, wk );
			GppData_IconEnable( &rwk->gpp_work, FALSE );
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, GF_BGL_FRAME3_M );
		}
		else {
			///< プロフィール削除 概要作成
			VideoDataMake( &rwk->vtr_work, wk );
			VideoData_IconEnable( &rwk->vtr_work, FALSE );
		//	BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
			BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
		}
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 3:
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			if ( rwk->view_page == 0 ){
				GppData_IconEnable( &rwk->gpp_work, TRUE );
			}
			else {
				VideoData_IconEnable( &rwk->vtr_work, TRUE );
			}
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		break;

	default:
		rwk->view_page ^= 1;
		ProfWin_MsgSet( wk, msg_718 + rwk->view_page );
		BR_Main_SeqChange( wk, eVRANK_ProfMain );
		break;
	}
	
	return FALSE;
}

static BOOL VRANK_Play( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
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
		CATS_SystemActiveSet( wk, TRUE );	
		if ( rwk->view_page == 0 ){
			///< 概要削除
			VideoDataFree( &rwk->vtr_work, wk );
		}
		else {
			///< プロフィール削除
			GppDataFree( &rwk->gpp_work, wk );						
		}
		BMN_Delete( rwk->btn_man[ 0 ] );
		BMN_Delete( rwk->btn_man[ 1 ] );
		BMN_Delete( rwk->btn_man[ 2 ] );

		Video_GppDataDelete( wk );
		ProfWin_Del( wk );
		FontButton_DeleteSub( wk );
		CATS_SystemActiveSet( wk, FALSE );

		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp_Simple( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BR_RecoverSeqSet( wk, *(wk->proc_seq) );
		BR_PostMessage( wk->message, TRUE, 0 );
		
		wk->sub_seq++;
		break;
	
	default:
		sys_FreeMemoryEz( rwk );
		return TRUE;
	}
	return FALSE;	
}


static BOOL VRANK_RecoverIn( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		PrioritySet_Common();
		Plate_AlphaInit_Default( &rwk->eva, &rwk->evb, ePLANE_ALL );
		{
			int i;
			for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
				wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
			}
		}
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR,   wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
//		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_video_d2_NSCR,   GF_BGL_FRAME3_S );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
//		FontButton_CreateSub( wk );
		FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
		FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
		CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
		CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
		rwk->icon.global_gra_id = GLOBAL_POKE_ICON_ID;	///< 気持ちずらす
		rwk->icon.view_start = wk->sys.touch_list.now;
		wk->sub_seq++;
		break;
	
	case 1:
		Video_GppDataCreate_from_OutLine( wk );
		rwk->btn_man[ 0 ] = BMN_Create( hit_table_000, NELEMS( hit_table_000 ), Btn_CallBack_200, wk, HEAPID_BR );
		rwk->btn_man[ 1 ] = BMN_Create( hit_table_001, NELEMS( hit_table_001 ), Btn_CallBack_201, wk, HEAPID_BR );
		rwk->btn_man[ 2 ] = BMN_Create( hit_table_002, NELEMS( hit_table_002 ), Btn_CallBack_202, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 2:
		CATS_SystemActiveSet( wk, TRUE );
		VideoDataMake( &rwk->vtr_work, wk );
		BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
		VideoData_IconEnable( &rwk->vtr_work, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;

	case 3:
		BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			VideoData_IconEnable( &rwk->vtr_work, TRUE );
			ProfWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
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
		BR_Main_SeqChange( wk, eVRANK_ProfMain );
		break;
	}
	
	return FALSE;
}

static void InfoMessage( BR_WORK* wk, int no, BOOL flag )
{
	STRBUF* str1;
	GF_BGL_BMPWIN* win = &wk->info_win;
	RANK_WORK* rwk = wk->sub_work;

	BR_disp_BG_InfoWindowLoad( wk, GF_BGL_FRAME2_M, flag );
	GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
	
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 1, 19, 30, 4, eBG_PAL_FONT, 300 );

	if ( no == msg_608 ){
		
		WORDSET*	wset;
		STRBUF*		str2;
		STRBUF*		str3;
		GDS_PROFILE_PTR	gpp = rwk->gpp_work.gpp;
		
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

static BOOL VRANK_SaveIn( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	if ( BattleVideo_Preserved( wk, RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 ) ) ){			
		switch ( wk->sub_seq ){
		case 0:
			CATS_SystemActiveSet( wk, TRUE );
			if ( rwk->view_page == 0 ){
				///< 概要削除 プロフィール作成
				VideoDataFree( &rwk->vtr_work, wk );
			}
			else {
				///< プロフィール削除 概要作成
				GppDataFree( &rwk->gpp_work, wk );						
			}
			ProfWin_Del( wk );
			CATS_SystemActiveSet( wk, FALSE );
			CATS_SystemActiveSet( wk, TRUE );
            FontButton_DeleteSub( wk );
            CATS_SystemActiveSet( wk, FALSE );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;
		
		case 1:
			BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				CATS_SystemActiveSet( wk, TRUE );
				SaveFontButton_Create( wk );
				CATS_SystemActiveSet( wk, FALSE );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
			break;
		
		case 2:		
			Tool_InfoMessageMain( wk, msg_info_008 );			
			wk->sub_seq++;
			break;
		
		case 3:
			if ( !GF_TP_GetTrg() ){ break; }
			Tool_InfoMessageMainDel( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;

		case 4:
			BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				CATS_SystemActiveSet( wk, TRUE );
				SaveFontButton_Delete( wk );
				CATS_SystemActiveSet( wk, FALSE );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
			break;
		
		case 5:
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_CreateSub( wk );
			CATS_SystemActiveSet( wk, FALSE );
			if ( wk->ex_param1 == BR_EX_DATA_KUWASIKU ){	
				///< この時点だとボタンのタグ情報が変わってるので入れ替える
				FontOam_MsgSet( &rwk->obtn[ 0 ], &wk->sys, msg_05 );
				FontOam_MsgSet( &rwk->obtn[ 1 ], &wk->sys, msg_602 );
				CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 0 ].cap, eTAG_EX_BACK );
				CATS_ObjectAnimeSeqSetCap( rwk->obtn[ 1 ].cap, eTAG_EX_SAVE );
			}
			ProfWin_Add( wk );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			CATS_SystemActiveSet( wk, TRUE );
			if ( rwk->view_page == 0 ){
				///< プロフィール削除 概要作成
				VideoDataMake( &rwk->vtr_work, wk );
				VideoData_IconEnable( &rwk->vtr_work, FALSE );
//				ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1ua_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
				BR_ScrSet( wk, ProfileBGGet( &rwk->vtr_work ), GF_BGL_FRAME3_M );
			}
			else {
				///< プロフィール作成
				GppDataMake( &rwk->gpp_work, wk );
				GppData_IconEnable( &rwk->gpp_work, FALSE );
				ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			}
			CATS_SystemActiveSet( wk, FALSE );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle,	 NARC_batt_rec_gra_batt_rec_video_d2_NSCR, 	 wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
			break;
		
		case 6:
			BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
			if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
				if ( rwk->view_page == 0 ){
					VideoData_IconEnable( &rwk->vtr_work, TRUE );
				}
				else {
					GppData_IconEnable( &rwk->gpp_work, TRUE );
				}
				GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
				GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );		
			break;

		default:
			{
				int i;
				for ( i = 0; i < wk->recv_data.outline_data_num; i++ ){
					wk->recv_data.outline_data_tmp[ i ] = &wk->recv_data.outline_data[ i ];	
				}
			}
			BR_Main_SeqChange( wk, eVRANK_ProfMain );
			break;
		}
	}
	else {			
		switch ( wk->sub_seq ){
		case 0:
			CATS_SystemActiveSet( wk, TRUE );
			if ( rwk->view_page == 0 ){
				///< 概要削除 プロフィール作成
				VideoDataFree( &rwk->vtr_work, wk );
			}
			else {
				///< プロフィール削除 概要作成
				GppDataFree( &rwk->gpp_work, wk );						
			}
			CATS_SystemActiveSet( wk, FALSE );
			CATS_SystemActiveSet( wk, TRUE );
            FontButton_DeleteSub( wk );
			CATS_SystemActiveSet( wk, FALSE );
			ProfWin_Del( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
			break;
		
		case 1:
			BR_PaletteFade( &rwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
				CATS_SystemActiveSet( wk, TRUE );
				SaveFontButton_Create( wk );
				CATS_SystemActiveSet( wk, FALSE );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );
			break;
		
		case 2:
			BR_PaletteFade( &rwk->color, eFADE_MODE_IN );
			if ( Plate_AlphaFade( &rwk->eva, &rwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
				InfoMessage( wk, msg_608, FALSE );
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, rwk->color, wk->sys.logo_color );		
			break;

		default:
			BR_Main_SeqChange( wk, eVRANK_SaveMain );
			break;
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
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ProfWin_Del( BR_WORK* wk )
{	
	RANK_WORK* rwk = wk->sub_work;

	GF_BGL_BmpWinOff( &rwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &rwk->win_s[ 0 ] );	
}


//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	id	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void ProfWin_MsgSet( BR_WORK* wk, int id )
{
	RANK_WORK* rwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int x;
	
	win = &rwk->win_s[ 0 ];
	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, id );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_RANK, NULL );		
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
static void ProfWin_Add( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	int				ofs = 1;
	
	win = &rwk->win_s[ 0 ];	
	GF_BGL_BmpWinInit( win );	
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 8, 3, 16, 2, eBG_PAL_FONT, ofs );
	
	ProfWin_MsgSet( wk, msg_718 + rwk->view_page );
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
static void SaveWin_Add( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int				ofs = 1;
	int x;
	
	win = &rwk->win_s[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 4, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1000 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_RANK, NULL );		
	STRBUF_Delete( str1 );	
	GF_BGL_BmpWinOnVReq( win );
	
	ofs += ( 10 * 2 );
	
	win = &rwk->win_s[ 1 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 18, 6, 10, 2, eBG_PAL_FONT, ofs );	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, msg_1001 );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_RANK, NULL );		
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
	RANK_WORK* rwk = wk->sub_work;

	GF_BGL_BmpWinOff( &rwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &rwk->win_s[ 0 ] );
	GF_BGL_BmpWinOff( &rwk->win_s[ 1 ] );
	GF_BGL_BmpWinDel( &rwk->win_s[ 1 ] );
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_200( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	RANK_WORK* rwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVRANK_ProfChange );
		break;

	case 1:
		TouchAction( wk );
		if ( BattleRec_DataExistCheck() ){
			BR_Main_SeqChange( wk, eVRANK_Play );
		}
		else {
			rwk->tmp_seq = eVRANK_Play;
			BR_Main_SeqChange( wk, eVRANK_BrsDataGet );
		}
		break;

	case 2:
		TouchAction( wk );
		if ( BattleRec_DataExistCheck() ){
			BR_Main_SeqChange( wk, eVRANK_SaveIn );
		}
		else {
			rwk->tmp_seq = eVRANK_SaveIn;
			BR_Main_SeqChange( wk, eVRANK_BrsDataGet );
		}
		break;

	case 3:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVRANK_ProfExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_201( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	RANK_WORK* rwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
	case 1:
	case 2:
		rwk->save_no = button + 1;
		if ( wk->br_gpp[ rwk->save_no ] != NULL ){
			BR_Main_SeqChange( wk, eVRANK_CheckIn );
		}
		else {
			BR_Main_SeqChange( wk, eVRANK_CheckFinish );
		}
		TouchAction( wk );
		break;

	case 3:
		TouchAction( wk );
		InfoMessageDel( wk );
		BR_Main_SeqChange( wk, eVRANK_SaveExit );
		break;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボタンコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_202( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	RANK_WORK* rwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	OS_Printf( "button = %d\n", button );
	
	switch ( button ){
	case 0:
		TouchAction( wk );
		SaveWin_Del( wk );
		BR_Main_SeqChange( wk, eVRANK_CheckFinish );
		break;

	case 1:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eVRANK_CheckExit );
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief	GPP生成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Video_GppDataCreate_from_OutLine( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	rwk->gpp_work.gpp	  = GDS_Profile_AllocMemory( HEAPID_BR );															///< 確保して
	GDS_CONV_GTGDSProfile_to_GDSProfile( &wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->profile, rwk->gpp_work.gpp );	///< ダウンロードデータコピーして
	rwk->vtr_work.gpp	  = rwk->gpp_work.gpp;																				///< ビデオデータのプロフにもＧＰＰのポインタ当てる
	
	rwk->vtr_work.br_head = BattleRec_Header_AllocMemory( HEAPID_BR );
	MI_CpuCopy8( &wk->recv_data.outline_data_tmp[ wk->sys.touch_list.view.this ]->head, rwk->vtr_work.br_head, sizeof( GT_BATTLE_REC_HEADER ) );
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
static void ListPokeIcon_Add( BR_WORK* wk )
{
	int i,j;
	int index = 0;
	RANK_WORK* rwk = wk->sub_work;
	
	int monsno;
	int form;
	int egg;
	int now = wk->sys.touch_list.now;
	int tbl[][ 6 ] = {
		{ 0,1,2,3,4,5 },
		{ 0,1,2,3,4,5 },	///< 2vs2  --味味敵敵
	//	{ 0,1,2,6,7,8 },
	};
	int monsno_tbl[ 6 ] = { 0,0,0,0,0,0 };
	int form_tbl[ 6 ]	= { 0,0,0,0,0,0 };
	int tbl_index = 0;	
	
//	OS_Printf( "now = %d\n", now );
	
	for ( i = 0; i < 5; i++ ){
		if ( wk->recv_data.outline_data_tmp[ now + i ] == NULL ){ continue; }
		if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ now + i ]->head ) ){
			tbl_index = 1;
		}
		else {
			tbl_index = 0;
		}
		
		///< たまごを抜いたソートをする 05.14
		{
			int n,m;
			int idx = 0;
			
			for ( n = 0; n < 6; n++ ){
				monsno_tbl[ n ] = 0;
				form_tbl[ n ]	= 0;				
			}
			
			for ( n = 0; n < 3; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ now + i ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
			if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ now + i ]->head ) ){
				idx = 3;
			}
			for ( n = 3; n < 6; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ now + i ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
		}
		
		for ( j = 0; j < 6; j++ ){
			monsno = monsno_tbl[ j ];//wk->recv_data.outline_data_tmp[ now + i ]->head.monsno[ tbl[ tbl_index ][ j ] ];
		//	OS_Printf( "monsno = %3d\n", monsno );
			form   = form_tbl[ j ];//wk->recv_data.outline_data_tmp[ now + i ]->head.form[ tbl[ tbl_index ][ j ] ];
			egg	   = 0;
		//	OS_Printf( "add pokemon = %d\n",monsno );
			rwk->icon.poke[ i ][ j ].gra_id = rwk->icon.global_gra_id;
			rwk->icon.poke[ i ][ j ].cap	= PokemonIcon_AddHDL( wk, rwk->icon.handle, rwk->icon.poke[ i ][ j ].gra_id, monsno, form, egg );
			if ( rwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectPriSetCap( rwk->icon.poke[ i ][ j ].cap, 6 - j );
			}
			rwk->icon.global_gra_id++;
		//	OS_Printf( " global graphics id = %d\n", rwk->icon.global_gra_id );
		}
	}
	ListPokeIcon_PosSet( wk );
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
static void ListPokeIcon_Enable( BR_WORK* wk, BOOL flag )
{
	int i,j;
	RANK_WORK* rwk = wk->sub_work;
	
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( rwk->icon.poke[ i ][ j ].cap ){
				CATS_ObjectEnableCap( rwk->icon.poke[ i ][ j ].cap, flag );
			}			
		}
	}
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
static void ListPokeIcon_Del( BR_WORK* wk )
{
	int i,j;
	RANK_WORK* rwk = wk->sub_work;
	
	for ( i = 0; i < 5; i++ ){
		for ( j = 0; j < 6; j++ ){			
			if ( rwk->icon.poke[ i ][ j ].cap ){
				PokemonIcon_Del( wk, rwk->icon.poke[ i ][ j ].gra_id, rwk->icon.poke[ i ][ j ].cap );
				rwk->icon.poke[ i ][ j ].cap = NULL;
			}			
		}
	}
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
static void ListPokeIcon_ReAdd( BR_WORK* wk )
{
	int i,j;
	int index = 0;
	int old_index;
	int gra_id[ 6 ] = { 0, 0, 0, 0, 0, 0 };
	RANK_WORK* rwk = wk->sub_work;
	
	if ( rwk->icon.view_start == wk->sys.touch_list.now ){ return; }
	
	CATS_SystemActiveSet( wk, TRUE );
	
	///< どっちへリストが動いたか
	if ( rwk->icon.view_start > wk->sys.touch_list.now ){
		///< ↑ = 最下段のアイコンの破棄
		index = 4;
	}
	
	///< 最上段か、最下段のアイコンを削除
	for ( i = 0; i < 6; i++ ){
		if ( rwk->icon.poke[ index ][ i ].cap ){
			PokemonIcon_DelActor( wk, rwk->icon.poke[ index ][ i ].gra_id, rwk->icon.poke[ index ][ i ].cap );
			gra_id[ i ] = rwk->icon.poke[ index ][ i ].gra_id;
			rwk->icon.poke[ index ][ i ].cap = NULL;
		}
	}

	if ( index ){
		///< 下へ詰めて、最上段をフリーにする
		for ( i = 4; i >= 1; i-- ){
			for ( j = 0; j < 6; j++ ){
				rwk->icon.poke[ i ][ j ].cap	= rwk->icon.poke[ i - 1 ][ j ].cap;
				rwk->icon.poke[ i ][ j ].gra_id = rwk->icon.poke[ i - 1 ][ j ].gra_id;
			}
		}
		index = 0;	///< index を 空けた箇所へ
	}
	else {
		///< 上へ詰めて、最下段をフリーにする
		for ( i = 1; i < 5; i++ ){
			for ( j = 0; j < 6; j++ ){
				rwk->icon.poke[ i - 1 ][ j ].cap	= rwk->icon.poke[ i ][ j ].cap;
				rwk->icon.poke[ i - 1 ][ j ].gra_id = rwk->icon.poke[ i ][ j ].gra_id;
			}
		}
		index = 4;	///< index を 空けた箇所へ
	}
	
	{
		int monsno;
		int form;
		int	egg;
		int pos = wk->sys.touch_list.now + index;
		int tbl[][ 6 ] = {
			{ 0,1,2,3,4,5 },
			{ 0,1,2,3,4,5 },
		//	{ 0,1,2,6,7,8 },
		};
		int monsno_tbl[ 6 ] = { 0,0,0,0,0,0 };
		int form_tbl[ 6 ]	= { 0,0,0,0,0,0 };
		int tbl_index = 0;

		if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ pos ]->head ) ){
			tbl_index = 1;
		}		
				
		///< たまごを抜いたソートをする 05.14
		{
			int n,m;
			int idx = 0;
			
			for ( n = 0; n < 6; n++ ){
				monsno_tbl[ n ] = 0;
				form_tbl[ n ]	= 0;				
			}
			
			for ( n = 0; n < 3; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ pos ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
			if ( VideoCheck2vs2_Head( ( BATTLE_REC_HEADER_PTR )&wk->recv_data.outline_data_tmp[ pos ]->head ) ){
				idx = 3;
			}
			for ( n = 3; n < 6; n++ ){
				monsno = wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ n ] ];				
				if ( monsno == 0 ){ continue; }
				monsno_tbl[ idx ] = monsno;
				form_tbl[ idx ] = wk->recv_data.outline_data_tmp[ pos ]->head.form_no[ tbl[ tbl_index ][ n ] ];
				idx++;
			}
		}
		
		for ( j = 0; j < 6; j++ ){
			monsno = monsno_tbl[ j ];//wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ j ] ];
			form   = form_tbl[ j ];//wk->recv_data.outline_data_tmp[ pos ]->head.monsno[ tbl[ tbl_index ][ j ] ];
			egg	   = 0;
			rwk->icon.poke[ index ][ j ].gra_id = rwk->icon.global_gra_id;
			rwk->icon.poke[ index ][ j ].cap	= PokemonIcon_AddHDL( wk, rwk->icon.handle, rwk->icon.poke[ index ][ j ].gra_id, monsno, form, egg ); 
			if ( rwk->icon.poke[ index ][ j ].cap ){
				CATS_ObjectPriSetCap( rwk->icon.poke[ index ][ j ].cap, 6 - j );
			}
			rwk->icon.global_gra_id++;
		//	OS_Printf( " global graphics id = %d\n", rwk->icon.global_gra_id );
		}
	}
	ListPokeIcon_PosSet( wk );	
	rwk->icon.view_start = wk->sys.touch_list.now;
	
	for ( i = 0; i < 6; i++ ){
		if ( gra_id[ i ] == 0 ){ continue; }
		CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON + gra_id[ i ] );
	}
	
	CATS_SystemActiveSet( wk, FALSE );
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
static void FontButton_DeleteSub( BR_WORK* wk )
{
	RANK_WORK* rwk = wk->sub_work;
	
	FontOam_Delete( &rwk->obtn[ 0 ] );
	FontOam_Delete( &rwk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( rwk->obtn[ 0 ].cap );
	CATS_ActorPointerDelete_S( rwk->obtn[ 1 ].cap );
	ExTag_ResourceDelete( wk );
}


//--------------------------------------------------------------
/**
 * @brief	プロフィールのセーブ
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BR_Prof_Save( BR_WORK* wk, int no )
{
	SAVE_RESULT result;
	RANK_WORK* rwk = wk->sub_work;
	
//	result = BattleRec_GTRec_Save( wk->save, &rwk->save_data, no );
	result = BattleRec_GDS_RecvData_Save( wk->save, no, 0, NULL, NULL);
	
	OS_Printf( " result = %d\n", result );
}
