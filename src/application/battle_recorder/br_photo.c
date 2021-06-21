//==============================================================================
/**
 * @file	br_photo.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.12.11(火)
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
static BOOL DressInit( BR_WORK* wk );
static BOOL DressSetup( BR_WORK* wk );
static BOOL DressSendIn( BR_WORK* wk );
static BOOL DressSendMain( BR_WORK* wk );
static BOOL DressSendOut( BR_WORK* wk );
static BOOL DressSendMainOut( BR_WORK* wk );
static BOOL Dress_PokemonInit( BR_WORK* wk );
static BOOL Dress_PokemonAIUEO( BR_WORK* wk );
static BOOL Dress_PokemonSelect( BR_WORK* wk );
static BOOL Dress_PokemonOut( BR_WORK* wk );
static BOOL Dress_RecvMain( BR_WORK* wk );
static BOOL Dress_MainIn( BR_WORK* wk );
static BOOL Dress_Main( BR_WORK* wk );
static BOOL Dress_MainOut( BR_WORK* wk );
static BOOL Dress_ProfChange( BR_WORK* wk );
static BOOL Dress_DataChange( BR_WORK* wk );
static void ProfWin_Del( BR_WORK* wk );
static void ProfWin_MsgSet( BR_WORK* wk, int id );
static void ProfWin_Add( BR_WORK* wk );
static void Photo_VBlank( void* work );
static void WindowMaskSet( void );
static void WindowMaskReSet( void );
static void ButtonOAM_PosSetSub( BR_WORK* wk, int mode );
static void FontButton_CreateSub( BR_WORK* wk );
static void FontButton_Delete( BR_WORK* wk );
static void WinAdd_Send( BR_WORK* wk );
static void WinAdd_SendEx( BR_WORK* wk, int no );
static BOOL IsPokeNameList( BR_WORK* wk, int word );
static void PokemonNameList_DataMake( BR_WORK* wk, int word );
static void PokemonNameList_DataFree( BR_WORK* wk );
static void Btn_CallBack_AIUEO( u32 button, u32 event, void* work );
static void WinAdd_AIUEO( BR_WORK* wk );
static void WinDel_AIUEO( BR_WORK* wk );
static void WinDel_Send( BR_WORK* wk );
extern void BR_Response_DressRegist(void *work, const GDS_RAP_ERROR_INFO *error_info);
extern void BR_Response_DressGet(void *work, const GDS_RAP_ERROR_INFO *error_info);


#define PRINT_COL_VIDEO		( PRINT_COL_NORMAL )
#define PRINT_COL_VIDEO2	( GF_PRINTCOLOR_MAKE( 12, 11,  0 ) )
#define IMC_CX				( 128 - 56 )
#define IMC_CY				(  96 - 64 - 16 )

/*
	上画面をメイン
	下画面をサブ	
	に入れ替えて使用しています。
*/

FS_EXTERN_OVERLAY( ol_imageclip );

///< 名前
static u16 NameHeadTable[]={
	ZKN_AKSTNHMYRW_IDX_A,
	ZKN_AKSTNHMYRW_IDX_K,
	ZKN_AKSTNHMYRW_IDX_S,
	ZKN_AKSTNHMYRW_IDX_T,
	ZKN_AKSTNHMYRW_IDX_N,
	ZKN_AKSTNHMYRW_IDX_H,
	ZKN_AKSTNHMYRW_IDX_M,
	ZKN_AKSTNHMYRW_IDX_Y,
	ZKN_AKSTNHMYRW_IDX_R,
	ZKN_AKSTNHMYRW_IDX_W,
	ZKN_AKSTNHMYRW_IDX_END,
};

///< 送信用
BOOL (* const BR_photo_send_MainTable[])( BR_WORK* wk ) = {
	DressInit,
	DressSetup,
	DressSendIn,
	DressSendMain,
	DressSendOut,
	DressSendMainOut,
};

///< 受信用
BOOL (* const BR_photo_recv_MainTable[])( BR_WORK* wk ) = {
	DressInit,
	DressSetup,	
	Dress_PokemonInit,
	Dress_PokemonAIUEO,
	Dress_PokemonSelect,
	Dress_PokemonOut,
	Dress_RecvMain,	
	Dress_MainIn,
	Dress_Main,
	Dress_MainOut,
	Dress_ProfChange,
	Dress_DataChange,
};

///< 当たり判定
static const RECT_HIT_TBL hit_table_send[] = {
	{ 152, 152 + 32,  32,  32 + 96 },	///< 戻る
	{ 152, 152 + 32, 128, 128 + 96 },	///< 送る
};

static const RECT_HIT_TBL hit_table_back[] = {	
	{ 152, 184, 80, 176 },	///< 戻る
};

static const RECT_HIT_TBL hit_tbl_pokemon[] = {
	{ DTC(  3 ), DTC(  5 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  5 ), DTC(  7 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  7 ), DTC(  9 ), DTC( 7 ), DTC( 24 ) },
	{ DTC(  9 ), DTC( 11 ), DTC( 7 ), DTC( 24 ) },
	{ DTC( 11 ), DTC( 13 ), DTC( 7 ), DTC( 24 ) },
	{ DTC( 13 ), DTC( 15 ), DTC( 7 ), DTC( 24 ) },
};

static const RECT_HIT_TBL hit_tbl_main[] = {
	{ DTC(  2 ), DTC(  6 ), DTC(  7 ), DTC( 25 ) },	///< プロフィール
	{ DTC(  9 ), DTC( 13 ), DTC(  3 ), DTC(  9 ) },
	{ DTC(  9 ), DTC( 13 ), DTC( 23 ), DTC( 29 ) },
};

static const TOUCH_LIST_HEADER tl_head_pokemon_dmy = {
	NULL,
	0,
	1,					///< 2行幅
		
	9,3,
	14,12,
	32,
	GF_BGL_FRAME2_S,
	
	6,					///< 表示件数
	hit_tbl_pokemon,	///< 当たり判定
};

//< 受信データ
typedef struct {
	
	int mode;					/// 0 = photo
	int page;					/// page
	IMC_TELEVISION_SAVEDATA*	data;
	GT_GDS_DRESS_RECV*			gds_data[ BR_DRESS_VIEW_MAX ];
	
} POKE_RECV_DATA;

//< 本体
typedef struct {
	
	int				eva;
	int				evb;
	int				color;
	int				view_page;

	GF_BGL_BMPWIN	win_m[ 12 ];
	GF_BGL_BMPWIN	win_s[ 12 ];
	int				win_s_num;
	int				win_m_num;

	OAM_BUTTON		obtn[ 2 ];				///< 送る　戻る	
	BUTTON_MAN*		btn_man[ 1 ];

	///< リスト用
	POKE_SEL_WORK		poke_select;
	TL_INFO*			tl_poke_list_info;	///< どーしても可変になるので ポケモンの個数Alloc
	TOUCH_LIST_HEADER	tl_poke_list_head;	///< どーしても可変になるので	
	
	IMC_PLAYERSYS_INIT			imc_param;		///< 渡すパラメータ
	IMC_PLAYERSYS_PTR			imc_sys;		///< 描画用システム*		
	IMC_TELEVISION_SAVEDATA*	dress;			///< 描画用

	POKE_RECV_DATA				poke_recv;		///< データ受信用ワーク
	GPP_WORK					gpp_work;		///< GPPワーク
	
	
	GDS_PROFILE_PTR				gds_profile_ptr;///< プロフィールポインタ
	
	
	int							monsno;			///< もんすNo
	int							download_max;	///< 受信件数
	
} DRESS_WORK;


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
static BOOL DressInit( BR_WORK* wk )
{
	DRESS_WORK* dwk = sys_AllocMemory( HEAPID_BR, sizeof( DRESS_WORK ) );
	
	memset( dwk, 0, sizeof( DRESS_WORK ) );
	
	wk->sub_work = dwk;
	
	Overlay_Load( FS_OVERLAY_ID( ol_imageclip ), OVERLAY_LOAD_NOT_SYNCHRONIZE );
	
	{	
		dwk->imc_param.p_bg_ini= wk->sys.bgl;
		dwk->imc_param.x	   = IMC_CX;
		dwk->imc_param.y	   = IMC_CY;
		dwk->imc_param.heap_id = HEAPID_BR;
	}			
	{
		dwk->poke_recv.data = ImcSaveData_TelevisionAllocWork( HEAPID_BR );
		dwk->gpp_work.gpp	= GDS_Profile_AllocMemory( HEAPID_BR );
	}
	
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	Plate_AlphaInit_Default( &dwk->eva, &dwk->evb, ePLANE_ALL );

	BR_Main_SeqChange( wk, eDRESS_Setup );
	
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
static BOOL DressSetup( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &dwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 dwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 dwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, 2 );
		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, 0 );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, 1 );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, 3 );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, 2 );
		Cursor_Visible( wk->cur_wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 2:
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );				///< main = top	
		CATS_SystemActiveSet( wk, TRUE );
		wk->sub_seq++;
		break;

	default:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		CATS_SystemActiveSet( wk, FALSE );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete
		FontButton_CreateSub( wk );
		CATS_SystemActiveSet( wk, FALSE );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
		ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );

		BR_Main_SeqChange( wk, DRESS_SepSeq );
		break;
	}
	
	return FALSE;		
}


//--------------------------------------------------------------
/**
 * @brief	送信初期化
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL DressSendIn( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		WindowMaskSet();
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_u_NSCR,	 GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_d2_NSCR, GF_BGL_FRAME3_S );		
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );	
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		
		CATS_SystemActiveSet( wk, TRUE );
		dwk->dress = ImcSaveData_GetTelevisionSaveData( SaveData_GetImcSaveData( wk->save ), IMC_SAVEDATA_TELEVISION_MYDATA );			
		if ( dwk->imc_sys == NULL ){
			dwk->imc_sys = IMC_TelevisionPlayerInit( &dwk->imc_param, dwk->dress );		
			IMC_Player_SetDrawFlag( dwk->imc_sys, TRUE );
		}
		CATS_SystemActiveSet( wk, FALSE );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		sys_VBlankFuncChange( Photo_VBlank, wk );
		wk->sub_seq++;
		break;
		
	case 1:
		PaletteWorkSet_VramCopy( wk->sys.pfd, FADE_MAIN_BG, 0 * 16, 16 * 0x20 );
		wk->sub_seq++;
		break;
	
	case 2:
		BR_PaletteFade( &dwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			WinAdd_Send( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eDRESS_SendMain );
		break;
	}
	if ( dwk->imc_sys ){
		GF_G3X_Reset();	
		IMC_PlayerMain( dwk->imc_sys );
		GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	送信メイン
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL DressSendMain( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( GF_TP_SingleHitTrg( &hit_table_send[ 0 ] ) ){	///< 戻る
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_SendMainOut );
	}
	if ( GF_TP_SingleHitTrg( &hit_table_send[ 1 ] ) ){	///< 送信
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_SendOut );
	}
	if ( dwk->imc_sys ){
		GF_G3X_Reset();	
		IMC_PlayerMain( dwk->imc_sys );
		GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	送信抜ける
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL DressSendOut( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	GDS_RAP_ERROR_INFO *error_info;
	
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
	
	switch( wk->sub_seq ){
	case 0:		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, eBR_BG3_PRI );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, eBR_BG3_PRI );
		{
			sys_FreeMemoryEz( dwk->poke_recv.data );
			GDS_Profile_FreeMemory( dwk->gpp_work.gpp );
		}
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		WinDel_Send( wk );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_Delete( wk );
			CATS_SystemActiveSet( wk, FALSE );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			GF_BGL_ScrollSet( wk->sys.bgl, GF_BGL_FRAME2_M,	GF_BGL_SCROLL_X_SET, 0);
			GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;

	case 2:
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		Tool_InfoMessageMain( wk, msg_info_017 );	
		dwk->gds_profile_ptr = GDS_Profile_AllocMemory( HEAPID_BR );
		GDS_Profile_MyDataSet( dwk->gds_profile_ptr, wk->save );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;

	case 3:
		if( GDSRAP_Tool_Send_DressupUpload( BR_GDSRAP_WorkGet( wk ),
											dwk->gds_profile_ptr,
											dwk->dress ) == TRUE){			
			GDS_Profile_FreeMemory( dwk->gds_profile_ptr );
			wk->sub_seq++;
		}
		break;

	case 4:
		///< 送信
		Tool_InfoMessageMainDel( wk );
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			//エラー発生時の処理
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorSet( wk, eERR_DRESS_SEND, error_info->result, error_info->type );
			wk->sub_seq++;
		}
		else{
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_AddScore( wk, SCORE_ID_GDS_DRESS_UPLOAD );
			Snd_SePlay( eBR_SND_SEND_OK );
		//	Tool_InfoMessageMainDel( wk );
			wk->sub_seq++;
		}		
		break;
		
	case 5:
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		wk->sub_seq++;
		break;
	
	case 6:
		CATS_SystemActiveSet( wk, TRUE );
		BR_PaletteFadeIn_Init( &dwk->color );
		NormalTag_RecoverAllOp( wk );	
		CATS_SystemActiveSet( wk, FALSE );	
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
			if ( dwk->imc_sys ){
				IMC_PlayerEnd( dwk->imc_sys );
				BG2_Recover( wk );
				GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
				WindowMaskReSet();
			}
		
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &dwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( dwk );
					
			Overlay_UnloadID( FS_OVERLAY_ID( ol_imageclip ) );
			sys_VBlankFuncChange( BR_VBlank, wk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	LOGO_PALETTE, dwk->color, wk->sys.logo_color );			///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	メインアウト
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL DressSendMainOut( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, eBR_BG3_PRI );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, eBR_BG3_PRI );
		{
			sys_FreeMemoryEz( dwk->poke_recv.data );
			GDS_Profile_FreeMemory( dwk->gpp_work.gpp );
		}
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		WinDel_Send( wk );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_Delete( wk );
			CATS_SystemActiveSet( wk, FALSE );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		BR_PaletteFadeIn_Init( &dwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );	
		CATS_SystemActiveSet( wk, FALSE );	
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
			CATS_SystemActiveSet( wk, TRUE );
			if ( dwk->imc_sys ){
				IMC_PlayerEnd( dwk->imc_sys );
				BG2_Recover( wk );
				GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
				WindowMaskReSet();
			}
			CATS_SystemActiveSet( wk, FALSE );
		
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &dwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( dwk );
					
			Overlay_UnloadID( FS_OVERLAY_ID( ol_imageclip ) );
			sys_VBlankFuncChange( BR_VBlank, wk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	LOGO_PALETTE, dwk->color, wk->sys.logo_color );			///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}

static BOOL Dress_PokemonInit( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		ButtonOAM_PosSetSub( wk, 1 );
		wk->sub_seq++;
		break;

	default:
		BR_Main_SeqChange( wk, eDRESS_PokemonAIUEO );
		break;
	}
	
	return FALSE;
}

static BOOL Dress_PokemonAIUEO( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d2_NSCR, GF_BGL_FRAME3_S );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
		dwk->btn_man[ 0 ] = BMN_Create( hit_rect_AIUEO, NELEMS( hit_rect_AIUEO ), Btn_CallBack_AIUEO, wk, HEAPID_BR );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &dwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			Tool_InfoMessageMain( wk, msg_711 );
			WinAdd_AIUEO( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;

	case 2:
		BMN_Main( dwk->btn_man[ 0 ] );
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) 
		||	 dwk->poke_select.bInit == TRUE ){
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		WinDel_AIUEO( wk );					///< あいうえお　消す
		BMN_Delete( dwk->btn_man[ 0 ] );
		Tool_InfoMessageMainDel( wk );		///< info消す
		wk->sub_seq++;
	
	case 4:
		if ( dwk->poke_select.bInit == TRUE ){
			if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				wk->sub_seq++;
			}
		}
		else {				
			BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
			if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
				wk->sub_seq++;
			}
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		}
		break;

	default:
		if ( dwk->poke_select.bInit == TRUE ){
			BR_Main_SeqChange( wk, eDRESS_PokemonSelect );
		}
		else {
			BR_Main_SeqChange( wk, eDRESS_PokemonOut );
		}
		break;
	}
	
	return FALSE;	
}

static BOOL Dress_PokemonSelect( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		ButtonOAM_PosSetSub( wk, 1 );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_find_d4_NSCR, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			Tool_InfoMessageMain( wk, msg_711 );
			
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Init( &wk->sys.slider, wk );															///< スライダーの初期化			
			dwk->tl_poke_list_head = tl_head_pokemon_dmy;												///< リストヘッダーのベースはコピーする
			dwk->tl_poke_list_head.info = dwk->tl_poke_list_info;										///< infoは書き換え
			dwk->tl_poke_list_head.data_max = dwk->poke_select.poke_list_num;							///< データ個数も書き換え
			TouchList_WorkClear( &wk->sys.touch_list );
			TouchList_InitEx( &wk->sys.touch_list, wk, dwk->poke_select.man, &dwk->tl_poke_list_head );	///< リストの初期化
			Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
			TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_SUB );							///< リストのカーソル登録
			CATS_SystemActiveSet( wk, FALSE );
			
			dwk->monsno = 0;
			wk->sub_seq++;
		}
		break;
	
	case 2:
		Slider_HitCheck_Main( &wk->sys.slider );		
		TouchList_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		{		
			int param = TouchList_Main( &wk->sys.touch_list, wk );
			if ( param ){
				dwk->monsno = param;
				TouchAction( wk );
				wk->sub_seq++;
			}
		}
		if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){
			TouchAction( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		Tool_InfoMessageMainDel( wk );					///< info消す
		CATS_SystemActiveSet( wk, TRUE );
		Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
		TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
		TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
	
	case 4:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );	
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;

	default:
		PokemonNameList_DataFree( wk );
		
		if ( dwk->monsno ){
			BR_Main_SeqChange( wk, eDRESS_RecvMain );
		}
		else {
			BR_Main_SeqChange( wk, eDRESS_PokemonAIUEO );
		}
		break;
	}
	return FALSE;
}

static BOOL Dress_PokemonOut( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_M, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_M, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_M, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_M, eBR_BG3_PRI );		
		GF_BGL_PrioritySet( GF_BGL_FRAME0_S, eBR_BG0_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME1_S, eBR_BG1_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME2_S, eBR_BG2_PRI );
		GF_BGL_PrioritySet( GF_BGL_FRAME3_S, eBR_BG3_PRI );
		{
			sys_FreeMemoryEz( dwk->poke_recv.data );
			GDS_Profile_FreeMemory( dwk->gpp_work.gpp );
		}
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		wk->sub_seq++;
		break;
	
	case 1:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			FontButton_Delete( wk );
			CATS_SystemActiveSet( wk, FALSE );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		BR_PaletteFadeIn_Init( &dwk->color );
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_RecoverAllOp( wk );		
		CATS_SystemActiveSet( wk, FALSE );
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
			CATS_SystemActiveSet( wk, TRUE );
			if ( dwk->imc_sys ){
				IMC_PlayerEnd( dwk->imc_sys );
				BG2_Recover( wk );
				GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
				WindowMaskReSet();
			}
			CATS_SystemActiveSet( wk, FALSE );
		
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &dwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );		
			sys_FreeMemoryEz( dwk );

			Overlay_UnloadID( FS_OVERLAY_ID( ol_imageclip ) );
			sys_VBlankFuncChange( BR_VBlank, wk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	LOGO_PALETTE, dwk->color, wk->sys.logo_color );			///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}
	
	return FALSE;
}

static BOOL Dress_RecvMain( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	GDS_RAP_ERROR_INFO *error_info;

	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
	
	switch ( wk->sub_seq ){
	case 0:
	//	OS_Printf( "検索するmonsno = %3d\n", dwk->monsno );
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		Tool_InfoMessageMain( wk, msg_info_018 );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;			
		break;
	
	case 1:		
		if( GDSRAP_Tool_Send_DressupDownload( BR_GDSRAP_WorkGet( wk ), dwk->monsno ) == TRUE ){
			wk->sub_seq++;	
		}
		break;
	
	case 2:
		Tool_InfoMessageMainDel( wk );
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );			
			BR_ErrorSet( wk, eERR_DRESS_RECV, error_info->result, error_info->type );
			dwk->download_max = 0;		
			wk->wait = 0;
			Cursor_Visible( wk->cur_wk, FALSE );			
			BR_Main_SeqChange( wk, eDRESS_PokemonAIUEO );
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_RECV_OK );
		//	Tool_InfoMessageMainDel( wk );
			wk->sub_seq++;
		}
		break;
	
	case 3:
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		wk->sub_seq++;
		break;
	
	case 4:
		if ( dwk->download_max ){
			dwk->poke_recv.page = 0;
			dwk->poke_recv.mode = 0;
			BR_Main_SeqChange( wk, eDRESS_MainIn );
		}
		else {
			Tool_InfoMessageMain( wk, msg_info_024 );
			wk->sub_seq++;
			wk->wait = 0;
		}
		break;

	default:
		if ( (++wk->wait) >= BB_INFO_WAIT 
		||	 GF_TP_GetTrg() == TRUE ){			
			wk->wait = 0;
			Tool_InfoMessageMainDel( wk );
			BR_Main_SeqChange( wk, eDRESS_PokemonAIUEO );
		}
		break;
	}
	
	return FALSE;	
}

static BOOL Dress_MainIn( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< ドレス描画
		dwk->view_page = 0;	///< 最初はドレスを見てる
		GDS_CONV_GTDress_to_Dress( &dwk->poke_recv.gds_data[ dwk->poke_recv.page ]->dressup, dwk->poke_recv.data );
		dwk->imc_sys = IMC_TelevisionPlayerInit( &dwk->imc_param, dwk->poke_recv.data );
		IMC_Player_SetDrawFlag( dwk->imc_sys, FALSE );
		WindowMaskSet();
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_u_NSCR,	 GF_BGL_FRAME3_M );
		BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_d_NSCR, GF_BGL_FRAME3_S );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );	
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );	
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		sys_VBlankFuncChange( Photo_VBlank, wk );
		wk->sub_seq++;
		break;

	case 1:
		PaletteWorkSet_VramCopy( wk->sys.pfd, FADE_MAIN_BG, 0 * 16, 16 * 0x20 );
		wk->sub_seq++;
		break;	

	case 2:
		BR_PaletteFade( &dwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			IMC_Player_SetDrawFlag( dwk->imc_sys, TRUE );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			WinAdd_SendEx( wk, dwk->poke_recv.page );
			ProfWin_Add( wk );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;

	default:
		BR_Main_SeqChange( wk, eDRESS_Main );
		break;
	}

	if ( dwk->imc_sys ){
		GF_G3X_Reset();	
		IMC_PlayerMain( dwk->imc_sys );
		GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
	}
	return FALSE;
}

static BOOL Dress_Main( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( GF_TP_SingleHitTrg( &hit_table_back[ 0 ] ) ){	///< 戻る
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_MainOut );
	}	
	if ( GF_TP_SingleHitTrg( &hit_tbl_main[ 0 ] ) ){	///< プロフィール切り替え
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_ProfChange );
	}
	if ( GF_TP_SingleHitTrg( &hit_tbl_main[ 1 ] ) ){	///< ←
		if ( dwk->poke_recv.page != 0 ){
			dwk->poke_recv.page--;
		}
		else {
			dwk->poke_recv.page = dwk->download_max - 1;
		}
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_DataChange );
	}	
	if ( GF_TP_SingleHitTrg( &hit_tbl_main[ 2 ] ) ){	///< →
		dwk->poke_recv.page++;
		dwk->poke_recv.page %= dwk->download_max;
		TouchAction( wk );
		BR_Main_SeqChange( wk, eDRESS_DataChange );
	}	
	
	///< 3D描画
	if ( dwk->imc_sys ){
		GF_G3X_Reset();	
		IMC_PlayerMain( dwk->imc_sys );
		GF_G3_RequestSwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	メインアウト
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Dress_MainOut( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		if ( dwk->view_page == 0 ){
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			WinDel_Send( wk );
		//	WindowMaskReSet();
		}
		else {
			CATS_SystemActiveSet( wk, TRUE );
			GppDataFree( &dwk->gpp_work, wk );			
			CATS_SystemActiveSet( wk, FALSE );
		}
		ProfWin_Del( wk );
		wk->sub_seq++;
		break;

	case 1:
		BR_PaletteFade( &dwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			if ( dwk->imc_sys ){
				IMC_PlayerEnd( dwk->imc_sys );
				BG2_Recover( wk );
				GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
				dwk->imc_sys = NULL;
				WindowMaskReSet();
			}
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, dwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_Main_SeqChange( wk, eDRESS_PokemonAIUEO );
		break;
	}	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	プロフィール切り替え
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Dress_ProfChange( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	///< dwk->view_page == 0 = ImageClip
	switch( wk->sub_seq ){
	case 0:
		if ( dwk->view_page == 0 ){
			///< イメージクリップさくじょ
			WinDel_Send( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		}
		else {
			///< プロフィール削除
			CATS_SystemActiveSet( wk, TRUE );
			GppDataFree( &dwk->gpp_work, wk );			
			CATS_SystemActiveSet( wk, FALSE );
		}
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){			
			if ( dwk->view_page == 0 ){
				///< イメージクリップデータ削除
				if ( dwk->imc_sys ){
					IMC_PlayerEnd( dwk->imc_sys );
					BG2_Recover( wk );
					GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
					dwk->imc_sys = NULL;
					WindowMaskReSet();
				}
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			}
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< 切り替え
		if ( dwk->view_page == 0 ){
			///< プロフィールデータ取得
			CATS_SystemActiveSet( wk, TRUE );
			GDS_CONV_GTGDSProfile_to_GDSProfile( &dwk->poke_recv.gds_data[ dwk->poke_recv.page ]->profile, dwk->gpp_work.gpp );
			GppDataMake( &dwk->gpp_work, wk );
			GppData_IconEnable( &dwk->gpp_work, FALSE );
			CATS_SystemActiveSet( wk, FALSE );
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, GF_BGL_FRAME3_M );
		}
		else {
			///< イメージクリップ生成
			GDS_CONV_GTDress_to_Dress( &dwk->poke_recv.gds_data[ dwk->poke_recv.page ]->dressup, dwk->poke_recv.data );
			dwk->imc_sys = IMC_TelevisionPlayerInit( &dwk->imc_param, dwk->poke_recv.data );
			IMC_Player_SetDrawFlag( dwk->imc_sys, FALSE );
			WindowMaskSet();
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_u_NSCR,	 GF_BGL_FRAME3_M );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );	
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
		wk->sub_seq++;
		break;
	
	case 3:
		if ( dwk->view_page == 0 ){
		}
		else {
			PaletteWorkSet_VramCopy( wk->sys.pfd, FADE_MAIN_BG, 0 * 16, 16 * 0x20 );
		}
		wk->sub_seq++;
		break;
	
	case 4:
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			if ( dwk->view_page == 0 ){
				GppData_IconEnable( &dwk->gpp_work, TRUE );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			}
			else {
				WinAdd_SendEx( wk, dwk->poke_recv.page );
				IMC_Player_SetDrawFlag( dwk->imc_sys, TRUE );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			}
			wk->sub_seq++;
		}		
		break;

	default:
		dwk->view_page ^= 1;
		ProfWin_MsgSet( wk, msg_718 + dwk->view_page );
		BR_Main_SeqChange( wk, eDRESS_Main );
		break;
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	データ切り替え
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL Dress_DataChange( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	///< dwk->view_page == 0 = ImageClip
	switch( wk->sub_seq ){
	case 0:
		if ( dwk->view_page == 0 ){
			///< イメージクリップさくじょ
			WinDel_Send( wk );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		}
		else {
			///< プロフィール削除
			CATS_SystemActiveSet( wk, TRUE );
			GppDataFree( &dwk->gpp_work, wk );			
			CATS_SystemActiveSet( wk, FALSE );
		}
		wk->sub_seq++;
		break;
	
	case 1:
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){			
			if ( dwk->view_page == 0 ){
				///< イメージクリップデータ削除
				if ( dwk->imc_sys ){
					IMC_PlayerEnd( dwk->imc_sys );
					BG2_Recover( wk );
					GF_BGL_PrioritySet( GF_BGL_FRAME2_M, 0 );
					dwk->imc_sys = NULL;
					WindowMaskReSet();
				}
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
				GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			}
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< 切り替え
		CATS_SystemActiveSet( wk, TRUE );
		if ( dwk->view_page == 0 ){
			///< イメージクリップ生成
			GDS_CONV_GTDress_to_Dress( &dwk->poke_recv.gds_data[ dwk->poke_recv.page ]->dressup, dwk->poke_recv.data );
			dwk->imc_sys = IMC_TelevisionPlayerInit( &dwk->imc_param, dwk->poke_recv.data );
			IMC_Player_SetDrawFlag( dwk->imc_sys, FALSE );
			WindowMaskSet();
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_photo_u_NSCR,	 GF_BGL_FRAME3_M );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );	
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
		}
		else {
			///< プロフィールデータ取得
			GDS_CONV_GTGDSProfile_to_GDSProfile( &dwk->poke_recv.gds_data[ dwk->poke_recv.page ]->profile, dwk->gpp_work.gpp );
			GppDataMake( &dwk->gpp_work, wk );
			GppData_IconEnable( &dwk->gpp_work, FALSE );
			BR_ScrSet( wk, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, GF_BGL_FRAME3_M );
			GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
		}
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 3:
		if ( dwk->view_page == 0 ){
			PaletteWorkSet_VramCopy( wk->sys.pfd, FADE_MAIN_BG, 0 * 16, 16 * 0x20 );
		}
		else {
		}
		wk->sub_seq++;
		break;
	
	case 4:
		if ( Plate_AlphaFade( &dwk->eva, &dwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			if ( dwk->view_page == 0 ){
				WinAdd_SendEx( wk, dwk->poke_recv.page );
				IMC_Player_SetDrawFlag( dwk->imc_sys, TRUE );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
			}
			else {
				GppData_IconEnable( &dwk->gpp_work, TRUE );
				GF_Disp_GX_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			}
			wk->sub_seq++;
		}		
		break;

	default:
		BR_Main_SeqChange( wk, eDRESS_Main );
		break;
	}
	
	return FALSE;
}


// =============================================================================
//
//
//	■ツール関係
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
static void ProfWin_Del( BR_WORK* wk )
{	
	DRESS_WORK* dwk = wk->sub_work;

	GF_BGL_BmpWinOff( &dwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &dwk->win_s[ 0 ] );	
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
	DRESS_WORK* dwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int x;
	
	win = &dwk->win_s[ 0 ];
	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, id );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );		
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
	DRESS_WORK* dwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	int				ofs = 1;
	
	win = &dwk->win_s[ 0 ];	
	GF_BGL_BmpWinInit( win );	
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 8, 3, 16, 2, eBG_PAL_FONT, ofs );
	
	ProfWin_MsgSet( wk, msg_718 );
}

//--------------------------------------------------------------
/**
 * @brief	IMC込みの割り込み
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Photo_VBlank( void* work )
{
	BR_WORK* wk = work;
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( dwk->imc_sys ){
		IMC_PlayerVblank( dwk->imc_sys );
	}
	
	DoVramTransferManager();			///< Vram転送マネージャー実行
	
//	CATS_RenderOamTrans();

	PaletteFadeTrans( wk->sys.pfd );
	
	GF_BGL_VBlankFunc( wk->sys.bgl );
	
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------
/**
 * @brief	マスク設定
 *
 * @param	none	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WindowMaskSet( void )
{		
	GX_SetVisibleWnd( GX_WNDMASK_W0 );
	G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	G2_SetWnd0Position( 9 * 8, 2 * 8, 9 * 8 + 14 * 8, 2 * 8 + 16 * 8 );	
}

//--------------------------------------------------------------
/**
 * @brief	マスク解除
 *
 * @param	none	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WindowMaskReSet( void )
{
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );	
}

//--------------------------------------------------------------
/**
 * @brief	ボタンの座標変更
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
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( dwk->obtn[ 1 ].cap, TRUE );
		FONTOAM_SetDrawFlag( dwk->obtn[ 1 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( dwk->obtn[ 0 ].cap, 32, 232 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( dwk->obtn[ 1 ].cap, FALSE );
		FONTOAM_SetDrawFlag( dwk->obtn[ 1 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( dwk->obtn[ 0 ].cap,  80, 232 );
	}
	FONTOAM_SetMat( dwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( dwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}

//--------------------------------------------------------------
/**
 * @brief	ボタン作成
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void FontButton_CreateSub( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;

	ExTag_ResourceLoad( wk, NNS_G2D_VRAM_TYPE_2DSUB );		
	dwk->obtn[ 0 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	dwk->obtn[ 1 ].cap = ExTag_Add( wk, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &dwk->obtn[ 0 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );
	FontOam_Create( &dwk->obtn[ 1 ], &wk->sys, NNS_G2D_VRAM_TYPE_2DSUB );

	FontOam_MsgSet( &dwk->obtn[ 0 ], &wk->sys, msg_05 );
	FontOam_MsgSet( &dwk->obtn[ 1 ], &wk->sys, msg_302 );
	CATS_ObjectAnimeSeqSetCap( dwk->obtn[ 0 ].cap, eTAG_EX_BACK );
	CATS_ObjectAnimeSeqSetCap( dwk->obtn[ 1 ].cap, eTAG_EX_SEND );

	CATS_ObjectPosSetCap( dwk->obtn[ 0 ].cap,  32, 232 );
	CATS_ObjectPosSetCap( dwk->obtn[ 1 ].cap, 128, 232 );
	FONTOAM_SetMat( dwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( dwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetDrawFlag( dwk->obtn[ 0 ].font_obj, TRUE );
	FONTOAM_SetDrawFlag( dwk->obtn[ 1 ].font_obj, TRUE );
		
	ButtonOAM_PosSetSub( wk, 0 );	
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
static void FontButton_Delete( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	FontOam_Delete( &dwk->obtn[ 0 ] );
	FontOam_Delete( &dwk->obtn[ 1 ] );
	CATS_ActorPointerDelete_S( dwk->obtn[ 0 ].cap );
	CATS_ActorPointerDelete_S( dwk->obtn[ 1 ].cap );
	
	ExTag_ResourceDelete( wk );			
}

//--------------------------------------------------------------
/**
 * @brief	名前＋作品名
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WinAdd_Send( BR_WORK* wk )
{
	GF_BGL_BMPWIN* win;
	STRBUF* 	str1;
	STRBUF* 	str2;
	STRBUF*		tmp;
	WORDSET*	wset;
	DRESS_WORK* dwk = wk->sub_work;
	PMS_WORD	word;
	
	if ( GF_BGL_BmpWinAddCheck( &dwk->win_m[ 0 ] ) == TRUE ){ return; }
	
	win = &dwk->win_m[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 16, 19, 16, 4, eBG_PAL_FONT, 300 );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	
	wset = BR_WORDSET_Create( HEAPID_BR );
	tmp  = STRBUF_Create( 255, HEAPID_BR );						///< テンポラリ
	str2 = MSGMAN_AllocString( wk->sys.man, msg_305 );			///< ～ の記録
	
	str1 = STRBUF_Create( IMC_SAVEDATA_STRBUF_NUM, HEAPID_BR );		///< 親名
	ImcSaveData_GetTelevisionPokeOyaName( dwk->dress, str1 );
	BR_ErrorStrChange( wk, str1 );
	word = ImcSaveData_GetTelevisionTitlePmsWord( dwk->dress );		///< 作品名
	
	WORDSET_RegisterWord( wset, 0, str1, 0, TRUE, PM_LANG );
	WORDSET_RegisterPMSWord( wset, 1, word );
	WORDSET_ExpandStr( wset, tmp, str2 );
	GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );	
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );
	STRBUF_Delete( tmp );
	WORDSET_Delete( wset );
}


//--------------------------------------------------------------
/**
 * @brief	受信データ表示
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WinAdd_SendEx( BR_WORK* wk, int no )
{
	GF_BGL_BMPWIN* win;
	STRBUF* 	str1;
	STRBUF* 	str2;
	STRBUF*		tmp;
	WORDSET*	wset;
	DRESS_WORK* dwk = wk->sub_work;
	PMS_WORD	word;
	
	if ( GF_BGL_BmpWinAddCheck( &dwk->win_m[ 0 ] ) == TRUE ){ return; }
	
	win = &dwk->win_m[ 0 ];
	GF_BGL_BmpWinInit( win );
	GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, 16, 19, 16, 4, eBG_PAL_FONT, 300 );
	GF_BGL_BmpWinDataFill( win, 0x00 );
	
	wset = BR_WORDSET_Create( HEAPID_BR );
	tmp  = STRBUF_Create( 255, HEAPID_BR );										///< テンポラリ
	str2 = MSGMAN_AllocString( wk->sys.man, msg_305 );							///< ～ の記録
	
	str1 = STRBUF_Create( IMC_SAVEDATA_STRBUF_NUM, HEAPID_BR );					///< 親名
	ImcSaveData_GetTelevisionPokeOyaName( dwk->poke_recv.data, str1 );	
	BR_ErrorStrChange( wk, str1 );
	word = ImcSaveData_GetTelevisionTitlePmsWord( dwk->poke_recv.data );		///< 作品名

	WORDSET_RegisterWord( wset, 0, str1, 0, TRUE, PM_LANG );
	WORDSET_RegisterPMSWord( wset, 1, word );
	WORDSET_ExpandStr( wset, tmp, str2 );
	GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, 0, MSG_NO_PUT, PRINT_COL_PHOTO, NULL );	
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );
	STRBUF_Delete( tmp );
	WORDSET_Delete( wset );
}

//--------------------------------------------------------------
/**
 * @brief	ポケモンいるかチェック（リストにできるか）
 *
 * @param	wk	
 * @param	word	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL IsPokeNameList( BR_WORK* wk, int word )
{
	BOOL bOK = FALSE;
	
	
	u16* pdata;
	int  num;
	int  min;
	int  max;
	int  poke_num;
	DRESS_WORK* dwk = wk->sub_work;
	
	max = NameHeadTable[ word + 1 ];
	min = NameHeadTable[ word ];
	num = max - min;	
	pdata = ZukanSortDataGet_50on( HEAPID_BR, 0, &poke_num );
	
	{
		int  i;
		int  no = 0;
		BOOL bFound;		
		ZUKAN_WORK* zw = SaveData_GetZukanWork( wk->save );

		for ( i = min; i < max; i++ ){			
			bFound = ZukanWork_GetPokeSeeFlag( zw, pdata[ i ] );			
			if ( bFound == FALSE ){
				pdata[ i ] = NOTFOUND_MONSNO;
			}
		}
		
		no = 0;		
		for ( i = min; i < max; i++ ){			
			if ( pdata[ i ] != NOTFOUND_MONSNO ){
				bOK = TRUE;
				break;
			}			
		}
		sys_FreeMemoryEz( pdata );
	}
	
	return bOK;
}


//--------------------------------------------------------------
/**
 * @brief	データ作成
 *
 * @param	wk	
 * @param	word	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void PokemonNameList_DataMake( BR_WORK* wk, int word )
{
	u16* pdata;
	int  num;
	int  min;
	int  max;
	DRESS_WORK* dwk = wk->sub_work;
	
	max = NameHeadTable[ word + 1 ];
	min = NameHeadTable[ word ];
	num = max - min;
	
	dwk->poke_select.poke_data = ZukanSortDataGet_50on( HEAPID_BR, 0, &dwk->poke_select.poke_num );
	pdata = ZukanSortDataGet_50on( HEAPID_BR, 0, &dwk->poke_select.poke_num );	
	{
		int  i;
		int  no = 0;
		BOOL bFound;		
		ZUKAN_WORK* zw = SaveData_GetZukanWork( wk->save );
		
		dwk->poke_select.poke_list_num = 0;
		for ( i = min; i < max; i++ ){			
			bFound = ZukanWork_GetPokeSeeFlag( zw, pdata[ i ] );
			
			if ( bFound == FALSE ){
				pdata[ i ] = NOTFOUND_MONSNO;
			}
			else {
				dwk->poke_select.poke_list_num++;
			}
		}
		
		no = 0;		
		for ( i = min; i < max; i++ ){			
			if ( pdata[ i ] == NOTFOUND_MONSNO ){ continue; }			
			dwk->poke_select.poke_data[ no ] = pdata[ i ];
			no++;
		}
		for ( i = no; i < dwk->poke_select.poke_num; i++ ){
			dwk->poke_select.poke_data[ i ] = pdata[ i ];
		}
		sys_FreeMemoryEz( pdata );
		
		dwk->tl_poke_list_info = sys_AllocMemory( HEAPID_BR, sizeof( TL_INFO ) * dwk->poke_select.poke_list_num );
		MI_CpuFill8( dwk->tl_poke_list_info, 0, sizeof( TL_INFO ) * dwk->poke_select.poke_list_num );
		for ( i = 0; i < dwk->poke_select.poke_list_num; i++ ){
			dwk->tl_poke_list_info[ i ].msg   = dwk->poke_select.poke_data[ i ];
			dwk->tl_poke_list_info[ i ].param = dwk->poke_select.poke_data[ i ];
		}
	}
	dwk->poke_select.man = MSGMAN_Create( MSGMAN_TYPE_NORMAL, ARC_MSG,NARC_msg_monsname_dat, HEAPID_BR );
	dwk->poke_select.bInit = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief	リスト削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void PokemonNameList_DataFree( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;

	MSGMAN_Delete( dwk->poke_select.man );
	sys_FreeMemoryEz( dwk->tl_poke_list_info );
	sys_FreeMemoryEz( dwk->poke_select.poke_data );
	
	dwk->poke_select.bInit = FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	あいうえおボタンのコールバック
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_AIUEO( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }

	if ( IsPokeNameList( wk, button ) == FALSE ){
		Snd_SePlay( eBR_SND_NG );
		return;
	}	
//	OS_Printf( "button = %d\n", button );

	PokemonNameList_DataMake( wk, button );
}

//--------------------------------------------------------------
/**
 * @brief	あいうえおウィンドウ
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WinAdd_AIUEO( BR_WORK* wk )
{
	STRBUF* 			str1;
	GF_BGL_BMPWIN*		win;
	DRESS_WORK*	dwk = wk->sub_work;
	
	{
		int i;
		int ofs = 1;
		u8  x, y;
		int	px;
		
		for ( i = 0; i < 10; i++ ){		
			win = &dwk->win_s[ i ];			
			x = hit_rect_AIUEO[ i ].rect.left / 8;
			y = hit_rect_AIUEO[ i ].rect.top  / 8;		
			GF_BGL_BmpWinInit( win );
			GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, x, y, 2, 2, eBG_PAL_FONT, ofs );
			GF_BGL_BmpWinDataFill( win, 0x00 );
			str1 = MSGMAN_AllocString( wk->sys.man, msg_900 + i );
			px = BR_print_x_Get( win, str1 );
			
			if ( IsPokeNameList( wk, i ) == TRUE ){
				GF_STR_PrintColor( win, FONT_SYSTEM, str1, px, 0, MSG_NO_PUT, PRINT_COL_VIDEO, NULL );
			}
			else {
				GF_STR_PrintColor( win, FONT_SYSTEM, str1, px, 0, MSG_NO_PUT, PRINT_COL_VIDEO2, NULL );
			}
			GF_BGL_BmpWinOnVReq( win );			
			STRBUF_Delete( str1 );
			ofs += 4;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	あいうえお削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WinDel_AIUEO( BR_WORK* wk )
{
	int i;
	GF_BGL_BMPWIN*		win;
	DRESS_WORK*	dwk = wk->sub_work;

	for ( i = 0; i < 10; i++ ){		
		win = &dwk->win_s[ i ];					
		GF_BGL_BmpWinOff( win );
		GF_BGL_BmpWinDel( win );	
	}
}

//--------------------------------------------------------------
/**
 * @brief	送信用の名前ウィンドウ削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void WinDel_Send( BR_WORK* wk )
{
	DRESS_WORK* dwk = wk->sub_work;
	
	if ( GF_BGL_BmpWinAddCheck( &dwk->win_m[ 0 ] ) == TRUE ){
		GF_BGL_BmpWinOff( &dwk->win_m[ 0 ] );
		GF_BGL_BmpWinDel( &dwk->win_m[ 0 ] );
	}
}

//--------------------------------------------------------------
/**
 * @brief	送受信部分
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_DressRegist(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	BR_WORK* wk = work;
	DRESS_WORK* dwk = wk->sub_work;	
	
	OS_TPrintf("ドレスアップショットのアップロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
	}
}


//--------------------------------------------------------------
/**
 * @brief	送受信部分
 *
 * @param	work	
 * @param	error_info	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void BR_Response_DressGet(void *work, const GDS_RAP_ERROR_INFO *error_info)
{
	BR_WORK* wk = work;
	DRESS_WORK* dwk = wk->sub_work;
	
	OS_TPrintf("ドレスアップショットのダウンロードレスポンス取得\n");
	if(error_info->occ == TRUE){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
		dwk->download_max = GDS_RAP_RESPONSE_DressupShot_Download_RecvPtr_Set( BR_GDSRAP_WorkGet( wk ), dwk->poke_recv.gds_data, BR_DRESS_VIEW_MAX );
	}
}
