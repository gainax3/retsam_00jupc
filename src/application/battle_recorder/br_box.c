//==============================================================================
/**
 * @file	br_box.c
 * @brief	簡単な説明を書く
 * @author	goto
 * @date	2007.12.17(月)
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

#include "poketool/boxdata.h"
#include "include/savedata/misc.h"

//BOXDAT_GetDaisukiKabegamiFlag

/*
	上画面をメイン
	下画面をサブ
	
	に入れ替えて使用しています。
*/

#define BOX_SHOT_RECV_DATA_NUM	( 20 )
#define BOX_SHOT_BOX_POKE_NUM	( GT_PLW_BOX_MAX_POS )

typedef struct {
	
	int category;
	GDS_PROFILE_PTR gds_profile_ptr;
	
	int	download_max;
	
} PREVIEW;


// ボックスのワーク
typedef struct {

	BOX_DATA*		box_data;	///< box data
	GT_BOX_SHOT		box_shot;	///< box shot
	int				now_tray;	///< 現在のトレイ
	
	int				eva;
	int				evb;
	int				color;
	int				box_cur_pos;	
	
	GF_BGL_BMPWIN	win_m[ 8 ];
	GF_BGL_BMPWIN	win_s[ 8 ];
	
	CATS_ACT_PTR	cap[ GT_PLW_BOX_MAX_POS ];
	SOFT_SPRITE*	ssp_poke;	
	
	BUTTON_MAN*		btn_man[ 5 ];
	BUTTON_MAN*		btn;
	OAM_BUTTON		obtn[ 2 ];		///< 送る　戻る
	
	PREVIEW			prev;
	
	GT_BOX_SHOT_RECV*	recv_data[ BOX_SHOT_RECV_DATA_NUM ];		///< 0番は送信データにも使う
	GT_BOX_SHOT_RECV	recv_data_tmp[ BOX_SHOT_RECV_DATA_NUM ];
	STRBUF*				box_str[ BOX_SHOT_RECV_DATA_NUM ][ 2 ];		///< メモリ的に大丈夫か？　ボックス名　名前
	TOUCH_LIST_HEADER	tl_box_shot_head;
	
	GPP_WORK		gpp_work;
	
	///< カーソルの位置
	int				view_mode;
	int				poke_x;
	int				poke_y;
	
} BOX_WORK;

#define WP_OFS( n )	( ( n ) * 3 )
#define BR_BOX_WP_NCGR( n )	( NARC_batt_rec_gra_batt_rec_box_wp01_NCGR + WP_OFS( n ) )
#define BR_BOX_WP_NSCR( n )	( NARC_batt_rec_gra_batt_rec_box_wp01_NSCR + WP_OFS( n ) )
#define BR_BOX_WP_NCLR( n )	( NARC_batt_rec_gra_batt_rec_box_wp01_NCLR + WP_OFS( n ) )

//==============================================================
// Prototype
//==============================================================
static void BoxShot_GppDataCreate( BR_WORK* wk, int no );
static void BoxShot_GppDataDelete( BR_WORK* wk );
static BOOL BoxShot_SendCheck( GT_BOX_SHOT* data );
static void BoxShot_BoxCurPosSetNext( BOX_WORK* bwk );
static BOOL BoxShot_FirstSendCheck( BR_WORK* wk );
static void Btn_CallBack_01( u32 button, u32 event, void* work );
static void Btn_CallBack_02( u32 button, u32 event, void* work );
static void Btn_CallBack_03( u32 button, u32 event, void* work );
static void Btn_CallBack_04( u32 button, u32 event, void* work );
static void Btn_CallBack_05( u32 button, u32 event, void* work );
static void BoxShot_FontButton_Create( BR_WORK* wk );
static void BoxShot_FontButton_Delete( BR_WORK* wk );
static void BoxShot_BoxCurPosSet( BOX_WORK* bwk );
static void BoxShot_SendDataMake( SAVEDATA *sv, const BOX_DATA* box_data, int tray_no, GT_BOX_SHOT* box_shot );
static BOOL BoxShot_CommonInit( BR_WORK* wk );
static BOOL BoxShot_SetUp( BR_WORK* wk );
static BOOL BoxShot_TrayChange( BR_WORK* wk );
static void ButtonOAM_PosSet( BR_WORK* wk, int mode );
static BOOL BoxShot_CategoryIn( BR_WORK* wk );
static BOOL BoxShot_CategoryMain( BR_WORK* wk );
static BOOL BoxShot_SendOut( BR_WORK* wk );
static BOOL BoxShot_CategoryOut( BR_WORK* wk );
static BOOL BoxShot_EndFade( BR_WORK* wk );
static BOOL BoxShot_R_EndFade( BR_WORK* wk );
static void BoxShot_CommonExit( BR_WORK* wk );
static void BoxShot_BoxView_Init( BR_WORK* wk );
static void BoxShot_CategoryWin_Add( BR_WORK* wk );
static void BoxShot_CategoryWin_Del( BR_WORK* wk );
static void BoxShot_BoxView_Delete( BR_WORK* wk );
static void BoxShot_InfoMessage( BR_WORK* wk, int no );
static void BoxShot_InfoMessageEx( BR_WORK* wk, int no );
static void BoxShot_BoxName( BR_WORK* wk );
static void BoxShot_PokeName_BoxName_OFF( BR_WORK* wk );
static BOOL IsEggCheck( int bit, int check );
static void BoxShot_PokemonName( BR_WORK* wk, int pos );
static void BoxShot_PokeIconAdd( BR_WORK* wk );
static void BoxShot_PokeIconEnable( BR_WORK* wk, int flag );
static void BoxShot_PokeIconDel( BR_WORK* wk );
static void BoxShot_PokeSoftSpriteAdd( BR_WORK* wk );
static void BoxShot_PokeSoftSpriteEnable( BR_WORK* wk, int flag );
static void BoxShot_PokeSoftSpriteDel( BR_WORK* wk );
static void BoxShot_BoxView( BR_WORK* wk );
static void BoxShot_BoxView_NoMessage( BR_WORK* wk );
static BOOL BR_BoxS_Init( BR_WORK* wk );
static BOOL BR_BoxS_Main( BR_WORK* wk );
static BOOL BR_BoxS_Exit( BR_WORK* wk );
static BOOL BR_BoxR_Init( BR_WORK* wk );
static BOOL BR_BoxR_Main( BR_WORK* wk );
static void BoxShot_SelectWinAdd( BR_WORK* wk );
static void ProfWin_MsgSet( BR_WORK* wk, int id );
static void BoxShot_BoxWinAdd( BR_WORK* wk );
static void BoxShot_BoxWinDel( BR_WORK* wk );
static void BoxShot_SelectWinDel( BR_WORK* wk );
static void BoxShot_New20_WinAdd( BR_WORK* wk );
static void BoxShot_New20_WinDel( BR_WORK* wk );
static BOOL BoxShot_SelectIn( BR_WORK* wk );
static void BoxName_and_PlayerName_Create( BR_WORK* wk );
static void BoxName_and_PlayerName_Delete( BR_WORK* wk );
static BOOL BoxShot_Select( BR_WORK* wk );
static BOOL BoxShot_SelectOut( BR_WORK* wk );
static BOOL BoxShot_BoxIn( BR_WORK* wk );
static BOOL BoxShot_Box( BR_WORK* wk );
static BOOL BoxShot_BoxOut( BR_WORK* wk );
static BOOL BoxShot_ProfileIn( BR_WORK* wk );
static BOOL BoxShot_ProfileOut( BR_WORK* wk );
static BOOL BR_BoxR_Exit( BR_WORK* wk );
extern void BR_Response_BoxRegist( void *work, const GDS_RAP_ERROR_INFO *error_info );
extern void BR_Response_BoxGet( void *work, const GDS_RAP_ERROR_INFO *error_info );



//--------------------------------------------------------------
/**
 * @brief	
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_GppDataCreate( BR_WORK* wk, int no )
{
	BOX_WORK* bwk = wk->sub_work;
	
	bwk->gpp_work.gpp = GDS_Profile_AllocMemory( HEAPID_BR );
	
	GDS_CONV_GTGDSProfile_to_GDSProfile( &bwk->recv_data[ no ]->profile, bwk->gpp_work.gpp );
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
static void BoxShot_GppDataDelete( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	GDS_Profile_FreeMemory( bwk->gpp_work.gpp );
}

//--------------------------------------------------------------
/**
 * @brief	送信できるかチェック
 *
 * @param	data	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_SendCheck( GT_BOX_SHOT* data )
{
	int i;
	int monsno;

	for ( i = 0; i < GT_PLW_BOX_MAX_POS; i++ ){
		
		monsno = data->monsno[ i ];
		
		if ( monsno != 0 ){ return TRUE; }
	}
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	次のポケモンまで移動
 *
 * @param	bwk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxCurPosSetNext( BOX_WORK* bwk )
{
	int i;
	
	for ( i = bwk->box_cur_pos; i < GT_PLW_BOX_MAX_POS; i++ ){
		
		if ( bwk->box_shot.monsno[ i ] != 0 ){
			
			 bwk->box_cur_pos = i;

			 return;
		}
	}

	for ( i = 0; i < bwk->box_cur_pos; i++ ){
		
		if ( bwk->box_shot.monsno[ i ] != 0 ){
			
			 bwk->box_cur_pos = i;

			 return;
		}
	}
}


//--------------------------------------------------------------
/**
 * @brief	送信できるかすべてチェック
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_FirstSendCheck( BR_WORK* wk )
{	
	return ExParam2_Get( wk );
}

///< ボックスショット
static const RECT_HIT_TBL hit_table_01[] = {	
	{ DTC( 4 ), DTC( 8 ), DTC(  3 ), DTC(  9 ) },
	{ DTC( 4 ), DTC( 8 ), DTC( 23 ), DTC( 28 ) },	
	{ 152, 152 + 32,  32,  32 + 96 },
	{ 152, 152 + 32, 128, 128 + 96 },
};

///< カテゴリ選択
static const RECT_HIT_TBL hit_table_02[] = {
	{ DTC( 3 ), DTC( 5 ), DTC(  3 ), DTC( 13 ) },{ DTC( 3 ), DTC( 5 ), DTC( 18 ), DTC( 26 ) },
	{ DTC( 8 ), DTC(10 ), DTC(  3 ), DTC( 13 ) },{ DTC( 8 ), DTC(10 ), DTC( 18 ), DTC( 26 ) },
	{ DTC(13 ), DTC(15 ), DTC(  3 ), DTC( 13 ) },{ DTC(13 ), DTC(15 ), DTC( 18 ), DTC( 26 ) },

	{ 152, 184, 80, 176 },	///< 戻る
};

///< さいしん２０件
static const RECT_HIT_TBL hit_table_04[] = {
	{ DTC( 2 ), DTC(  6 ), DTC( 11 ), DTC( 21 ) },	///< プロフィール見る
	{ DTC( 8 ), DTC( 14 ), DTC( 11 ), DTC( 21 ) },	///< みる
	{ 152, 184, 80, 176 },	///< 戻る
};

static const RECT_HIT_TBL hit_table_05[] = {
//	{ DTC( 4 ), DTC( 8 ), DTC(  3 ), DTC(  9 ) },	///< ←
	{ DTC( 4 ), DTC( 12 ), DTC( 9 ), DTC( 23 ) },	///< くわしく
//	{ DTC( 4 ), DTC( 8 ), DTC( 23 ), DTC( 29 ) },	///< →
	{ 152, 184, 80, 176 },	///< 戻る
};

static void Btn_CallBack_01( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	BOX_WORK* bwk = wk->sub_work;

	switch ( button ){
	case 0:
		if ( event != BMN_EVENT_HOLD ){ return; }
		TouchAction( wk );
		if ( bwk->now_tray > 0 ){
			bwk->now_tray--;
		}
		else {
			bwk->now_tray = BOX_MAX_TRAY - 1;
		}
		BR_Main_SeqChange( wk, eBOX_TRAY_CHANGE );
		break;
	
	case 1:
		if ( event != BMN_EVENT_HOLD ){ return; }
		TouchAction( wk );
		bwk->now_tray++;
		bwk->now_tray %= BOX_MAX_TRAY;
		BR_Main_SeqChange( wk, eBOX_TRAY_CHANGE );
		break;
		
	case 2:
		if ( event != BMN_EVENT_TOUCH ){ return; }
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBOX_END_FADE );
		break;

	case 3:
		if ( event != BMN_EVENT_TOUCH ){ return; }
		TouchAction( wk );
		if ( BoxShot_SendCheck( &bwk->box_shot ) ){
			#if 0
			if ( MISC_GetGdsSend_Box( SaveData_GetMisc( wk->save ), bwk->now_tray ) ){
				BoxShot_InfoMessage( wk, msg_info_005 );
			}
			else {
				BR_Main_SeqChange( wk, eBOX_CATEGORY_IN );
			}
			#endif
			BR_Main_SeqChange( wk, eBOX_CATEGORY_IN );	///< とりあえず送る
		}
		else {
			BoxShot_InfoMessage( wk, msg_info_002 );
		}
		break;
	}
}

static void Btn_CallBack_02( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	BOX_WORK* bwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }

	switch ( button ){
	default:
		TouchAction( wk );
		bwk->prev.category = button;	
		BR_Main_SeqChange( wk, eBOX_SEND_OUT );
		break;
			
	case 6:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBOX_CATEGORY_OUT );
		break;
	}
}

static void Btn_CallBack_03( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	BOX_WORK* bwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }

	switch ( button ){
	default:		
		TouchAction( wk );
		bwk->prev.category = button;			
		BR_Main_SeqChange( wk, eBOX2_SELECT_IN );
		break;
			
	case 6:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBOX2_END_FADE );
		break;
	}
}

//--------------------------------------------------------------
/**
 * @brief	プロフィール　ボックス見る　もどる　の当たり判定
 *
 * @param	button	
 * @param	event	
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void Btn_CallBack_04( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	BOX_WORK* bwk = wk->sub_work;
	
	if ( event != BMN_EVENT_TOUCH ){ return; }

	switch ( button ){
	default:
	
	case 0:
		TouchAction( wk );
		if ( bwk->view_mode == 0 ){
			BR_Main_SeqChange( wk, eBOX2_PROF_IN );
		}
		else {
			BR_Main_SeqChange( wk, eBOX2_PROF_OUT );
		}
		break;
	
	case 1:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBOX2_BOX_IN );
		break;
			
	case 2:
		TouchAction( wk );
		BR_Main_SeqChange( wk, eBOX2_SELECT_OUT );
		break;
	}
}

static void Btn_CallBack_05( u32 button, u32 event, void* work )
{
	BR_WORK*  wk = work;
	BOX_WORK* bwk = wk->sub_work;
		
	if ( event != BMN_EVENT_TOUCH ){ return; }
	
	switch ( button ){	
	case 0:
		TouchAction( wk );
		bwk->box_cur_pos++;	
		bwk->box_cur_pos %= BOX_SHOT_BOX_POKE_NUM;	
		BoxShot_BoxCurPosSetNext( bwk );
		bwk->poke_x = bwk->box_cur_pos % 6;
		bwk->poke_y = bwk->box_cur_pos / 6;
		Cursor_PosSet( wk->cur_wk, 110 + ( bwk->poke_x * 24 ), 52 + ( bwk->poke_y * 22 ) );
		
		///< ポケモン切り替え
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_PokeSoftSpriteDel( wk );
		BoxShot_PokeSoftSpriteAdd( wk );
		CATS_SystemActiveSet( wk, FALSE );
		BoxShot_PokemonName( wk, bwk->box_cur_pos );		
		if ( bwk->box_shot.monsno[ bwk->box_cur_pos ] != 0 
		&&	 IsEggCheck( bwk->box_shot.egg_bit, ( 1 << bwk->box_cur_pos ) ) != TRUE ){
			Snd_PMVoicePlay( bwk->box_shot.monsno[ bwk->box_cur_pos ], bwk->box_shot.form_no[ bwk->box_cur_pos ] );
		}
		break;
		
	case 1:
		TouchAction( wk );		
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		BR_Main_SeqChange( wk, eBOX2_BOX_OUT );
		break;
	}
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
static void BoxShot_FontButton_Create( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	ExTag_ResourceLoad_Box( wk, NNS_G2D_VRAM_TYPE_2DSUB );	
	BR_Tool_FontButton_Create( wk, &bwk->obtn[ 0 ], NNS_G2D_VRAM_TYPE_2DSUB,  msg_05,  32, 232, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY, 0, TRUE );
	BR_Tool_FontButton_Create( wk, &bwk->obtn[ 1 ], NNS_G2D_VRAM_TYPE_2DSUB, msg_302, 128, 232, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY, 1, TRUE );
}


//--------------------------------------------------------------
/**
 * @brief	ボタン削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_FontButton_Delete( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	BR_Tool_FontButton_Delete( &bwk->obtn[ 0 ] );
	BR_Tool_FontButton_Delete( &bwk->obtn[ 1 ] );
	
	ExTag_ResourceDelete( wk );			
}


//--------------------------------------------------------------
/**
 * @brief	カーソル位置を決める ( ポケモンが最初に見つかった場所)
 *
 * @param	bwk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxCurPosSet( BOX_WORK* bwk )
{
	int i;
	
	for ( i = 0; i < GT_PLW_BOX_MAX_POS; i++ ){
		
		if ( bwk->box_shot.monsno[ i ] != 0 ){
			
			 bwk->box_cur_pos = i;

			 break;
		}
	}
}

//--------------------------------------------------------------
/**
 * @brief	送信データ作成 ( 描画にも使う )
 *
 * @param	boxdata	
 * @param	tray	
 * @param	gt_box	
 * @param	heap_id	
 * @param	move	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_SendDataMake( SAVEDATA *sv, const BOX_DATA* box_data, int tray_no, GT_BOX_SHOT* box_shot )
{
	memset( box_shot, 0, sizeof( GT_BOX_SHOT ) );	///< TAMAGOのBITのONOFFが、タマゴのときだけONになっているので初期化
	GDS_CONV_Box_to_GTBox( sv, box_data, tray_no, box_shot, HEAPID_BR );
}


//--------------------------------------------------------------
/**
 * @brief	共通の初期化処理
 *
 * @param	wk	
 *
 * @retval	BOOL	なんでも	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_CommonInit( BR_WORK* wk )
{
	BOX_WORK* bwk = sys_AllocMemory( HEAPID_BR, sizeof( BOX_WORK ) );
	
	memset( bwk, 0, sizeof( BOX_WORK ) );
	
	wk->sub_work = bwk;
	
	///< BG2 3 をまさらにするよ
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
	GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
	
	
	///< box data make
	bwk->box_data = SaveData_GetBoxData( wk->save );
	BoxShot_SendDataMake( wk->save, bwk->box_data, bwk->now_tray, &bwk->box_shot );
	BoxShot_BoxCurPosSet( bwk );
	
	///< alpha
	Plate_AlphaInit_Default( &bwk->eva, &bwk->evb, ePLANE_ALL );
	
	bwk->btn_man[ 0 ] = BMN_Create( hit_table_01, NELEMS( hit_table_01 ), Btn_CallBack_01, wk, HEAPID_BR );
	bwk->btn_man[ 1 ] = BMN_Create( hit_table_02, NELEMS( hit_table_02 ), Btn_CallBack_02, wk, HEAPID_BR );
	bwk->btn_man[ 2 ] = BMN_Create( hit_table_02, NELEMS( hit_table_02 ), Btn_CallBack_03, wk, HEAPID_BR );
	bwk->btn_man[ 3 ] = BMN_Create( hit_table_04, NELEMS( hit_table_04 ), Btn_CallBack_04, wk, HEAPID_BR );
	bwk->btn_man[ 4 ] = BMN_Create( hit_table_05, NELEMS( hit_table_05 ), Btn_CallBack_05, wk, HEAPID_BR );
	
	///< 送信用プロフィール
	bwk->prev.gds_profile_ptr = GDS_Profile_AllocMemory( HEAPID_BR );
	GDS_Profile_MyDataSet( bwk->prev.gds_profile_ptr, wk->save );
	
	BR_Main_SeqChange( wk, eBOX_SETUP );
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	setup
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_SetUp( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_OUT ) ){ wk->sub_seq++; }	
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 			 bwk->color, wk->sys.logo_color );	///< main	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	 LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );	///< sub	oam
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE,			 bwk->color, wk->sys.logo_color );	///< sub	bg
		break;
	
	case 1:
		CATS_SystemActiveSet( wk, TRUE );
		NormalTag_DeleteAllOp( wk );						///< tag delete
		CATS_SystemActiveSet( wk, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_MAIN );		///< main = top
		Cursor_Visible( wk->cur_wk, FALSE );
		wk->sub_seq++;
		break;
	
	case 2:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_FontButton_Create( wk );					///< ex tag cleate
		CATS_SystemActiveSet( wk, FALSE );
		PrioritySet_Common();
		wk->sub_seq++;
		break;
	
	default:		
		BR_Main_SeqChange( wk, eBOX_INIT );
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	トレイ切り替え
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_TrayChange( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_PokeSoftSpriteDel( wk );
		BoxShot_PokeIconDel( wk );
		BoxShot_PokeName_BoxName_OFF( wk );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
		
	case 1:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			BoxShot_SendDataMake( wk->save, bwk->box_data, bwk->now_tray, &bwk->box_shot );
			BoxShot_BoxCurPosSet( bwk );
						
			wk->sub_seq++;
		}
		break;
	
	case 2:
		{
			CATS_SystemActiveSet( wk, TRUE );
			BoxShot_BoxView_NoMessage( wk );
			BoxShot_PokeSoftSpriteEnable( wk, TRUE );
			BoxShot_PokeIconEnable( wk, FALSE );
			CATS_SystemActiveSet( wk, FALSE );
			
			wk->sub_seq++;
		}
		break;
		
	case 3:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
					
			BoxShot_PokemonName( wk, bwk->box_cur_pos );
			BoxShot_BoxName( wk );
			BoxShot_InfoMessage( wk, msg_info_001 );

			BoxShot_PokeSoftSpriteEnable( wk, FALSE );
			BoxShot_PokeIconEnable( wk, TRUE );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
			G2_BlendNone();
			
			BR_Main_SeqChange( wk, eBOX_MAIN );
		}
		break;

	default:		
		break;
	}	
	return FALSE;
}

static void ButtonOAM_PosSet( BR_WORK* wk, int mode )
{
	BOX_WORK* bwk = wk->sub_work;
	
	if ( mode == 0 ){
		///< 2個
		CATS_ObjectEnableCap( bwk->obtn[ 1 ].cap, TRUE );
		FONTOAM_SetDrawFlag( bwk->obtn[ 1 ].font_obj, TRUE );
		CATS_ObjectPosSetCap( bwk->obtn[ 0 ].cap, 32, 232 );
	}
	else {
		///< 1個
		CATS_ObjectEnableCap( bwk->obtn[ 1 ].cap, FALSE );
		FONTOAM_SetDrawFlag( bwk->obtn[ 1 ].font_obj, FALSE );
		CATS_ObjectPosSetCap( bwk->obtn[ 0 ].cap, 80, 232 );
	}
	FONTOAM_SetMat( bwk->obtn[ 0 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
	FONTOAM_SetMat( bwk->obtn[ 1 ].font_obj, FONT_OAM_TAG_OX, FONT_OAM_TAG_OY );
}

static BOOL BoxShot_CategoryIn( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Cursor_Visible( wk->cur_wk_top, FALSE );
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box1_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			BoxShot_CategoryWin_Add( wk );
			ButtonOAM_PosSet( wk, 1 );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
		
	case 1:
		BR_PaletteFade( &bwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			BR_Main_SeqChange( wk, eBOX_CATEGORY_MAIN );
			BoxShot_InfoMessage( wk, msg_info_003 );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	default:
		break;
	}	
	return FALSE;
}

static BOOL BoxShot_CategoryMain( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	BMN_Main( bwk->btn_man[ 1 ] );	
	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	送信部分？
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_SendOut( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GDS_RAP_ERROR_INFO *error_info;
	
	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
	
	switch( wk->sub_seq ){
	case 0:
		BoxShot_CategoryWin_Del( wk );
		Cursor_Visible( wk->cur_wk_top, FALSE );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;

	case 1:
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		
		BoxShot_PokeSoftSpriteDel( wk );
		
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_PokeIconDel( wk );
		CATS_SystemActiveSet( wk, FALSE );
		
		BoxShot_PokeName_BoxName_OFF( wk );

		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
		
	case 2:
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
						
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	case 3:
		wk->sub_seq++;
	
	case 4:
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		Tool_InfoMessageMain( wk, msg_info_019 );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
		
	case 5:
		///< 送信
		if( GDSRAP_Tool_Send_BoxshotUpload( BR_GDSRAP_WorkGet( wk ), 
											bwk->prev.category,
											bwk->prev.gds_profile_ptr,
											bwk->box_data, bwk->now_tray ) == TRUE ){			
			wk->sub_seq++;
		}
		break;
		
	case 6:
		///< 送信 check
		Tool_InfoMessageMainDel( wk );
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			//エラー発生時の処理
			//エラーメッセージの表示等はコールバックで行われるので、
			//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorSet( wk, eERR_BOX_SEND, error_info->result, error_info->type );
			wk->sub_seq++;
		}
		else{			
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_AddScore( wk, SCORE_ID_GDS_BOX_UPLOAD );
			ExParam2_Set( wk, TRUE );
			Snd_SePlay( eBR_SND_SEND_OK );
			wk->sub_seq++;
		}
		break;
		
	case 7:
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		wk->sub_seq++;
		break;

	default:
		BR_Main_SeqChange( wk, eBOX_EXIT );
		break;
	}
	return FALSE;	
}

static BOOL BoxShot_CategoryOut( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Cursor_Visible( wk->cur_wk_top, FALSE );
		BoxShot_CategoryWin_Del( wk );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;

	case 1:
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_SUB ) ){
			ButtonOAM_PosSet( wk, 0 );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box3_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		BR_PaletteFade( &bwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			BoxShot_InfoMessage( wk, msg_info_001 );
			BR_Main_SeqChange( wk, eBOX_MAIN );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	default:
		break;
	}	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	終了用フェード
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_EndFade( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_PokeSoftSpriteDel( wk );
		BoxShot_PokeIconDel( wk );
		BoxShot_PokeName_BoxName_OFF( wk );
		CATS_SystemActiveSet( wk, FALSE );

		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		
	case 1:
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
						
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		{
//			BoxShot_BoxView_NoMessage( wk );
//			BoxShot_PokeSoftSpriteEnable( wk, TRUE );
//			BoxShot_PokeIconEnable( wk, FALSE );			
			BR_Main_SeqChange( wk, eBOX_EXIT );
		}
		break;

	default:		
		break;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	終了用フェード
 *
 * @param	wk	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL BoxShot_R_EndFade( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		BoxShot_CategoryWin_Del( wk );
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		
	case 1:
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
						
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	case 2:
		{			
			BR_Main_SeqChange( wk, eBOX2_EXIT );
		}
		break;

	default:		
		break;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	共通の終了処理
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_CommonExit( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
}


// -----------------------------------------
//
//	送信用ボックス
//
// -----------------------------------------
static BOOL BR_BoxS_Init( BR_WORK* wk );
static BOOL BR_BoxS_Main( BR_WORK* wk );
static BOOL BR_BoxS_Exit( BR_WORK* wk );

BOOL (* const BR_BoxS_MainTable[])( BR_WORK* wk ) = {
	BoxShot_CommonInit,
	BoxShot_SetUp,
	BR_BoxS_Init,
	BR_BoxS_Main,
	BoxShot_TrayChange,
	BoxShot_CategoryIn,
	BoxShot_CategoryMain,
	BoxShot_CategoryOut,
	BoxShot_SendOut,
	BoxShot_EndFade,
	BR_BoxS_Exit,
};

// -----------------------------------------
//
//	受信用ボックス
//
// -----------------------------------------
static BOOL BR_BoxR_Init( BR_WORK* wk );
static BOOL BR_BoxR_Main( BR_WORK* wk );
static BOOL BR_BoxR_Exit( BR_WORK* wk );

static BOOL BoxShot_SelectIn( BR_WORK* wk );
static BOOL BoxShot_Select( BR_WORK* wk );
static BOOL BoxShot_SelectOut( BR_WORK* wk );

static BOOL BoxShot_ProfileIn( BR_WORK* wk );
static BOOL BoxShot_ProfileOut( BR_WORK* wk );

BOOL (* const BR_BoxR_MainTable[])( BR_WORK* wk ) = {
	BoxShot_CommonInit,
	BoxShot_SetUp,
	BR_BoxR_Init,
	BR_BoxR_Main,
	
	BoxShot_SelectIn,
	BoxShot_Select,
	BoxShot_SelectOut,

	BoxShot_ProfileIn,
	BoxShot_ProfileOut,
	
	BoxShot_BoxIn,
	BoxShot_Box,
	BoxShot_BoxOut,
	
	BoxShot_R_EndFade,
	BR_BoxR_Exit,
};

static const u8 win_data[][ 4 ] = {
	{  1,  2,  9, 3 },
	{ 15,  1, 13, 3 },
	{  1, 21, 30, 2 },
};

static const GF_PRINTCOLOR print_color[] = {
	PRINT_COL_BOX_SHOT,
	PRINT_COL_BOX_NAME,
	PRINT_COL_BOX_SHOT,
};

static const u32 pos_y[] = { 6, 4, 0, };
static const u32 col_c[] = { 0x00, 0x00, BR_MSG_CLEAR_CODE };


//--------------------------------------------------------------
/**
 * @brief	ボックスビューの初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxView_Init( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int ofs = 256 + 1;
	int i;
	
	for ( i = 0; i < 3; i++ ){
		win = &bwk->win_m[ i ];
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, win_data[ i ][ 0 ], win_data[ i ][ 1 ], win_data[ i ][ 2 ], win_data[ i ][ 3 ], eBG_PAL_FONT, ofs );
		GF_BGL_BmpWinDataFill( win, col_c[ i ] );
		GF_BGL_BmpWinOnVReq( win );

		ofs += ( win_data[ i ][ 2 ] * win_data[ i ][ 3 ] );
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボックスビューの初期化
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static const u8 win_data_c[][ 4 ] = {
    // MatchComment: all 10s here -> 11s
	{  3,  3, 11,  2 },{ 18,  3, 11,  2 },
	{  3,  8, 11,  2 },{ 18,  8, 11,  2 },
	{  3, 13, 11,  2 },{ 18, 13, 11,  2 },
};
static void BoxShot_CategoryWin_Add( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int ofs = 1;
	int i;	
	int x;
	STRBUF* str1;
	
	for ( i = 0; i < NELEMS( win_data_c ); i++ ){
		win = &bwk->win_s[ i ];	
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, win_data_c[ i ][ 0 ], win_data_c[ i ][ 1 ], win_data_c[ i ][ 2 ], win_data_c[ i ][ 3 ], eBG_PAL_FONT, ofs );
		
		str1 = MSGMAN_AllocString( wk->sys.man, msg_402 + i );
		x	 = BR_print_x_Get( win, str1 );
		
		GF_BGL_BmpWinDataFill( win, col_c[ 0 ] );
		
		GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, print_color[ 0 ], NULL );
				
		GF_BGL_BmpWinOnVReq( win );
		
		STRBUF_Delete( str1 );

		ofs += ( win_data_c[ i ][ 2 ] * win_data_c[ i ][ 3 ] );
	}
}

static void BoxShot_CategoryWin_Del( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int i;
	
	for ( i = 0; i < NELEMS( win_data_c ); i++ ){
		win = &bwk->win_s[ i ];
		
		GF_BGL_BmpWinOff( win );
		GF_BGL_BmpWinDel( win );
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボックスビューの削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxView_Delete( BR_WORK* wk )
{
	int i;
	BOX_WORK* bwk = wk->sub_work;
	
	for ( i = 0; i < 3; i++ ){
		GF_BGL_BmpWinOff( &bwk->win_m[ i ] );
		GF_BGL_BmpWinDel( &bwk->win_m[ i ] );
	}
}


//--------------------------------------------------------------
/**
 * @brief	メッセージの表示
 *
 * @param	wk	
 * @param	no	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_InfoMessage( BR_WORK* wk, int no )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	
	win  = &bwk->win_m[ 2 ];
	str1 = MSGMAN_AllocString( wk->sys.man, no );
	
	GF_BGL_BmpWinDataFill( win, col_c[ 2 ] );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, 0, pos_y[ 2 ], MSG_NO_PUT, print_color[ 2 ], NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
}

///< ○○○の○○○○ボックス
static void BoxShot_InfoMessageEx( BR_WORK* wk, int no )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	STRBUF* str2;
	STRBUF* tmp;
	WORDSET*	wset;
	int msg = msg_423 + bwk->prev.category;
	
	win  = &bwk->win_m[ 2 ];
	str1 = MSGMAN_AllocString( wk->sys.man, msg );
	str2 = STRBUF_Create( 255, HEAPID_BR );
	tmp  = STRBUF_Create( 255, HEAPID_BR );
	wset = BR_WORDSET_Create( HEAPID_BR );	

	STRBUF_SetStringCode( str2, bwk->recv_data[ wk->sys.touch_list.view.this ]->profile.name );
	BR_ErrorStrChange( wk, str2 );		///< no:0465

	WORDSET_RegisterWord( wset, 0, str2, 0, TRUE, PM_LANG );	
	WORDSET_ExpandStr( wset, tmp, str1 );
	
	GF_BGL_BmpWinDataFill( win, col_c[ 2 ] );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, tmp, 0, pos_y[ 2 ], MSG_NO_PUT, print_color[ 2 ], NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
	STRBUF_Delete( str2 );
	STRBUF_Delete( tmp );
	
	WORDSET_Delete( wset );
}


//--------------------------------------------------------------
/**
 * @brief	ボックスの名前表示
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxName( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	int x;
	
	win  = &bwk->win_m[ 1 ];
	str1 = STRBUF_Create( 255, HEAPID_BR );	
	STRBUF_SetStringCode( str1, bwk->box_shot.box_name );
	
	GF_BGL_BmpWinDataFill( win, col_c[ 1 ] );
	
	x	 = BR_print_x_Get( win, str1 );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, pos_y[ 1 ], MSG_NO_PUT, print_color[ 1 ], NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );
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
static void BoxShot_PokeName_BoxName_OFF( BR_WORK* wk )
{
	int i;
	GF_BGL_BMPWIN* win;
	BOX_WORK* bwk = wk->sub_work;
	
	///< info は消さない
	for ( i = 0; i < 2; i++ ){
		win  = &bwk->win_m[ i ];
		GF_BGL_BmpWinDataFill( win, col_c[ i ] );
		GF_BGL_BmpWinOnVReq( win );
	}
}


//--------------------------------------------------------------
/**
 * @brief	たまごチェック
 *
 * @param	bit	
 * @param	check	
 *
 * @retval	static BOOL	
 *
 */
//--------------------------------------------------------------
static BOOL IsEggCheck( int bit, int check )
{
	return ( ( bit & check ) == check );
}


//--------------------------------------------------------------
/**
 * @brief	ポケモンの名前表示
 *
 * @param	wk	
 * @param	pos	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokemonName( BR_WORK* wk, int pos )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	STRBUF* str1;
	int x;
	int monsno;
	STRCODE code[ 255 ];
	
	win  = &bwk->win_m[ 0 ];
	GF_BGL_BmpWinDataFill( win, col_c[ 0 ] );
	
	monsno = bwk->box_shot.monsno[ pos ];
	
	if ( monsno == 0 ){
		GF_BGL_BmpWinOnVReq( win );
		return;
	}
	
	str1 = STRBUF_Create( 255, HEAPID_BR );
	
	if ( IsEggCheck( bwk->box_shot.egg_bit, ( 1 << bwk->box_cur_pos ) ) == TRUE ){
		monsno = MONSNO_TAMAGO;
	}
		
	MSGDAT_MonsNameGet( monsno, HEAPID_BR, code );
	STRBUF_SetStringCode( str1, code );
	
	x	 = BR_print_x_Get( win, str1 );
	
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, pos_y[ 0 ], MSG_NO_PUT, print_color[ 0 ], NULL );
	
	GF_BGL_BmpWinOnVReq( win );
	
	STRBUF_Delete( str1 );	
}


//--------------------------------------------------------------
/**
 * @brief	ポケモンアイコン登録
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeIconAdd( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	CATS_SYS_PTR csp;
	CATS_RES_PTR crp;
	PALETTE_FADE_PTR pfd;
	TCATS_OBJECT_ADD_PARAM_S coap;
	
	csp = wk->sys.csp;
	crp = wk->sys.crp;
	pfd = wk->sys.pfd;
	
	CATS_LoadResourcePlttWorkArc( pfd, FADE_MAIN_OBJ, csp, crp,	ARC_POKEICON, PokeIconPalArcIndexGet(), 0, POKEICON_PAL_MAX, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON );
	CATS_LoadResourceCellArc( csp, crp, ARC_POKEICON, PokeIcon64kCellArcIndexGet(), 0, eID_OAM_POKE_ICON );
	CATS_LoadResourceCellAnmArc( csp, crp, ARC_POKEICON, PokeIcon64kCellAnmArcIndexGet(), 0, eID_OAM_POKE_ICON );
	
	{
		int i;
		int form;
		int monsno;
		int palno;
		int egg;
		
		for ( i = 0; i < GT_PLW_BOX_MAX_POS; i++ ){
			
			monsno = bwk->box_shot.monsno[ i ];
			form   = bwk->box_shot.form_no[ i ];
			egg	   = IsEggCheck( bwk->box_shot.egg_bit, ( 1 << i ) );
			
			if ( monsno == 0 ){ continue; }
			
			CATS_LoadResourceCharArcModeAdjustAreaCont(	csp, crp, ARC_POKEICON, PokeIconCgxArcIndexGetByMonsNumber( monsno, egg, form ), 0, NNS_G2D_VRAM_TYPE_2DMAIN, eID_OAM_POKE_ICON + i );
				
			coap.x		= 110 + ( ( i % 6 ) * 24 );
			coap.y		=  48 + ( ( i / 6 ) * 22 );
			coap.z		= 0;
			coap.anm	= 0;
			coap.pri	= 0;
			coap.pal	= 0;
			coap.d_area = CATS_D_AREA_MAIN;
			coap.bg_pri = 0;
			coap.vram_trans = 0;

			coap.id[0] = eID_OAM_POKE_ICON + i;
			coap.id[1] = eID_OAM_POKE_ICON;
			coap.id[2] = eID_OAM_POKE_ICON;
			coap.id[3] = eID_OAM_POKE_ICON;
			coap.id[4] = CLACT_U_HEADER_DATA_NONE;
			coap.id[5] = CLACT_U_HEADER_DATA_NONE;
			
			bwk->cap[ i ] = CATS_ObjectAdd_S( csp, crp, &coap );
				
			palno  = PokeIconPalNumGet( monsno, form, egg );
			CATS_ObjectPaletteOffsetSetCap( bwk->cap[ i ], eOAM_PAL_POKE_ICON + palno );
			CATS_ObjectAnimeSeqSetCap( bwk->cap[ i ], POKEICON_ANM_HPMAX );
			CATS_ObjectPriSetCap( bwk->cap[ i ], GT_PLW_BOX_MAX_POS - i );
		}			
	}
}


//--------------------------------------------------------------
/**
 * @brief	ポケモンアイコンの表示切替
 *
 * @param	wk	
 * @param	flag	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeIconEnable( BR_WORK* wk, int flag )
{
	BOX_WORK* bwk = wk->sub_work;
	
	int i;
	int monsno;
	
	for ( i = 0; i < GT_PLW_BOX_MAX_POS; i++ ){
		
		monsno = bwk->box_shot.monsno[ i ];
			
		if ( monsno == 0 ){ continue; }
		
		CATS_ObjectEnableCap( bwk->cap[ i ], flag );
	}
}


//--------------------------------------------------------------
/**
 * @brief	ポケモンアイコン削除
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeIconDel( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	int i;
	int monsno;

	CATS_FreeResourcePltt( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCell( wk->sys.crp, eID_OAM_POKE_ICON );
	CATS_FreeResourceCellAnm( wk->sys.crp, eID_OAM_POKE_ICON );
	
	for ( i = 0; i < GT_PLW_BOX_MAX_POS; i++ ){
		
		monsno = bwk->box_shot.monsno[ i ];
			
		if ( monsno == 0 ){ continue; }
		if ( bwk->cap[ i ] == NULL ){ continue; }
		
		CATS_FreeResourceChar( wk->sys.crp, eID_OAM_POKE_ICON + i );
		CATS_ActorPointerDelete_S( bwk->cap[ i ] );
		bwk->cap[ i ] = NULL;
	}
}


//--------------------------------------------------------------
/**
 * @brief	ソフトスプライトの登録
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeSoftSpriteAdd( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	POKEMON_PERSONAL_DATA* ppd;
	SOFT_SPRITE_ARC ssa;
	u8 sex;
	u8 rea;
	u32 id	   = bwk->box_shot.id_no[ bwk->box_cur_pos ];
	u32 form   = bwk->box_shot.form_no[ bwk->box_cur_pos ];
	u16 monsno = bwk->box_shot.monsno[ bwk->box_cur_pos ];
	u32 p_rand = bwk->box_shot.personal_rnd[ bwk->box_cur_pos ];
	u32 height;
	int dir	   = PARA_FRONT;
	
	if ( monsno == 0 ){ 
		bwk->ssp_poke = NULL;
		return;
	}
	
	if ( IsEggCheck( bwk->box_shot.egg_bit, ( 1 << bwk->box_cur_pos ) ) == TRUE ){
		if ( monsno == MONSNO_MANAFI ){
			form = 1;
		}
		else {
			form = 0;
		}
		monsno = MONSNO_TAMAGO;
	}
	
	ppd = PokePersonalPara_Open( monsno, HEAPID_BR );
	sex = PokeSexGetMonsNo( monsno, p_rand );
//	id  = PokeFormNoPersonalParaGet( monsno, form, ID_PARA_id_no );
	rea = PokeRareGetPara( id, p_rand );
	height = PokeHeightGet( monsno, sex, dir, form, p_rand );
	height = 0;	///< box で使ってないから 0 初期化 使うときのためにのこす
	
	PokeGraArcDataGet( &ssa, monsno, sex, dir, rea, form, p_rand );
	
	bwk->ssp_poke = SoftSpriteAdd( wk->sys.ssm_p, &ssa, 42, 91 + height, 0, 0, NULL, NULL);
	
	PokePersonalPara_Close( ppd );
}

//--------------------------------------------------------------
/**
 * @brief	ソフトスプライトの破棄
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeSoftSpriteEnable( BR_WORK* wk, int flag )
{
	BOX_WORK* bwk = wk->sub_work;
	
	if ( bwk->ssp_poke ){
		SoftSpriteParaSet( bwk->ssp_poke, SS_PARA_VANISH, flag );
	}
}

//--------------------------------------------------------------
/**
 * @brief	ソフトスプライトの破棄
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_PokeSoftSpriteDel( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	if ( bwk->ssp_poke ){
		SoftSpriteDel( bwk->ssp_poke );
	}
}


//--------------------------------------------------------------
/**
 * @brief	ボックスビュー
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxShot_BoxView( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	{
		BOX_DATA* boxdata = SaveData_GetBoxData( wk->save );
		
		///< ダイヤ・パールの壁紙は０番
		if ( bwk->box_shot.wallPaper >= BOX_NORMAL_WALLPAPER_MAX
		&&   bwk->box_shot.wallPaper < ( BOX_NORMAL_WALLPAPER_MAX + BOX_EX_WALLPAPER_MAX ) ){
			bwk->box_shot.wallPaper = 0;
		}
		
		///< プラチナの壁紙でも、もって無い場合は０番
		if ( bwk->box_shot.wallPaper >= ( BOX_NORMAL_WALLPAPER_MAX + BOX_EX_WALLPAPER_MAX )
		&&	 bwk->box_shot.wallPaper < BOX_TOTAL_WALLPAPER_MAX_PL ){			
			if( BOXDAT_GetDaisukiKabegamiFlag( boxdata, bwk->box_shot.wallPaper - BOX_TOTAL_WALLPAPER_MAX ) == FALSE ){
			//	bwk->box_shot.wallPaper = 0;
			}
		}
	}
	if(bwk->box_shot.wallPaper >= BOX_TOTAL_WALLPAPER_MAX_PL){
		bwk->box_shot.wallPaper = 0;
	}

	ArcUtil_HDL_BgCharSet( wk->sys.p_handle, BR_BOX_WP_NCGR( bwk->box_shot.wallPaper ),  wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, BR_BOX_WP_NSCR( bwk->box_shot.wallPaper ), wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	PaletteWorkSetEx_Arc( wk->sys.pfd, ARC_BR_GRA, BR_BOX_WP_NCLR( bwk->box_shot.wallPaper ), HEAPID_BR, FADE_MAIN_BG, 0x40, eBG_PAL_BOX * 16, eBG_PAL_BOX * 16 );

	BoxShot_PokeSoftSpriteAdd( wk );
	BoxShot_PokeIconAdd( wk );
	BoxShot_PokemonName( wk, bwk->box_cur_pos );
	BoxShot_BoxName( wk );
	BoxShot_InfoMessage( wk, msg_info_001 );
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
static void BoxShot_BoxView_NoMessage( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	{
		BOX_DATA* boxdata = SaveData_GetBoxData( wk->save );
		
		///< ダイヤ・パールの壁紙は０番
		if ( bwk->box_shot.wallPaper >= BOX_NORMAL_WALLPAPER_MAX
		&&   bwk->box_shot.wallPaper < ( BOX_NORMAL_WALLPAPER_MAX + BOX_EX_WALLPAPER_MAX ) ){
			bwk->box_shot.wallPaper = 0;
		}
		
		///< プラチナの壁紙でも、もって無い場合は０番
		if ( bwk->box_shot.wallPaper >= ( BOX_NORMAL_WALLPAPER_MAX + BOX_EX_WALLPAPER_MAX )
		&&	 bwk->box_shot.wallPaper < BOX_TOTAL_WALLPAPER_MAX_PL ){
			
			if( BOXDAT_GetDaisukiKabegamiFlag( boxdata, bwk->box_shot.wallPaper - BOX_TOTAL_WALLPAPER_MAX ) == FALSE ){
				bwk->box_shot.wallPaper = 0;
			}
		}
	}
//	if(bwk->box_shot.wallPaper >= BOX_TOTAL_WALLPAPER_MAX_PL){
//		bwk->box_shot.wallPaper = 0;
//	}
	
	ArcUtil_HDL_BgCharSet( wk->sys.p_handle, BR_BOX_WP_NCGR( bwk->box_shot.wallPaper ),  wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	ArcUtil_HDL_ScrnSet( wk->sys.p_handle, BR_BOX_WP_NSCR( bwk->box_shot.wallPaper ), wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
	PaletteWorkSetEx_Arc( wk->sys.pfd, ARC_BR_GRA, BR_BOX_WP_NCLR( bwk->box_shot.wallPaper ), HEAPID_BR, FADE_MAIN_BG, 0x40, eBG_PAL_BOX * 16, eBG_PAL_BOX * 16 );
	BoxShot_PokeSoftSpriteAdd( wk );
	BoxShot_PokeIconAdd( wk );
}


///< ----- 送信ボックス -----
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
static BOOL BR_BoxS_Init( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< 上下画面の転送
		{
			///< 上
			BR_disp_BG_InfoWindowLoad_Single( wk, GF_BGL_FRAME2_M );
			CATS_SystemActiveSet( wk, TRUE );
			BoxShot_BoxView_Init( wk );
			BoxShot_BoxView( wk );
			BoxShot_PokeSoftSpriteEnable( wk, TRUE );
			BoxShot_PokeIconEnable( wk, FALSE );
			CATS_SystemActiveSet( wk, FALSE );

			///< 下
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box3_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		}
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 1:
		///< alpha
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			BoxShot_PokeSoftSpriteEnable( wk, FALSE );
			BoxShot_PokeIconEnable( wk, TRUE );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			G2_BlendNone();
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< フェード		
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_IN ) ){
			bwk->btn = bwk->btn_man[ 0 ];
			BR_Main_SeqChange( wk, eBOX_MAIN );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	default:
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
static BOOL BR_BoxS_Main( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	BMN_Main( bwk->btn );
	
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
static BOOL BR_BoxS_Exit( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		BoxShot_PokeIconDel( wk );
		BoxShot_PokeSoftSpriteDel( wk );
		BoxShot_BoxView_Delete( wk );
		BoxShot_FontButton_Delete( wk );
		Cursor_Active( wk->cur_wk_top, FALSE );
		Cursor_Visible( wk->cur_wk_top, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	case 1:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		{ 
			int i;			
			for ( i = 0; i < 5; i++ ){
				BMN_Delete( bwk->btn_man[ i ] );
			}
		}
		BR_PaletteFadeIn_Init( &bwk->color );
		NormalTag_RecoverAllOp( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &bwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );
			GDS_Profile_FreeMemory( bwk->prev.gds_profile_ptr );
			sys_FreeMemoryEz( bwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, bwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}		
	
	return FALSE;
}


///< ----- 受信ボックス -----
static BOOL BR_BoxR_Init( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch ( wk->sub_seq ){
	case 0:
		///< 上下画面の転送
		{
			///< 上
			CATS_SystemActiveSet( wk, TRUE );
			BR_disp_BG_InfoWindowLoad_Single( wk, GF_BGL_FRAME2_M );
			BoxShot_BoxView_Init( wk );
			BoxShot_InfoMessage( wk, msg_info_006 );

			///< 下
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box1_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			BoxShot_CategoryWin_Add( wk );
			ButtonOAM_PosSet( wk, 1 );
			CATS_SystemActiveSet( wk, FALSE );
		}
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		break;
	
	case 1:
		///< alpha
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			G2_BlendNone();
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< フェード		
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_IN ) ){
			bwk->btn = bwk->btn_man[ 2 ];
			BR_Main_SeqChange( wk, eBOX_MAIN );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	default:
		break;
	}	
	return FALSE;
}

static BOOL BR_BoxR_Main( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	BMN_Main( bwk->btn_man[ 2 ] );	
	
	return FALSE;
}


static const u8 win_data_sel[][ 4 ] = {
	{  8,  3, 16, 2 },
	{  9, 10, 14, 2 },
};

static void BoxShot_SelectWinAdd( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int ofs = 1;
	int i;
	int x;
	int mes[] = { msg_718, msg_401 };
	STRBUF* str1;
	
	for ( i = 0; i < 2; i++ ){
		win = &bwk->win_s[ i ];
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, win_data_sel[ i ][ 0 ], win_data_sel[ i ][ 1 ], win_data_sel[ i ][ 2 ], win_data_sel[ i ][ 3 ], eBG_PAL_FONT, ofs );
		GF_BGL_BmpWinDataFill( win, 0x00 );
		
		str1 = MSGMAN_AllocString( wk->sys.man, mes[ i ] );
		
		x	 = BR_print_x_Get( win, str1 );
		
		GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_BOX_SHOT, NULL );
		
		GF_BGL_BmpWinOnVReq( win );
		ofs += ( win_data_sel[ i ][ 2 ] * win_data_sel[ i ][ 3 ] );
		
		STRBUF_Delete( str1 );
	}
}

static void ProfWin_MsgSet( BR_WORK* wk, int id )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN*	win;
	STRBUF*			str1;
	int x;
	
	win = &bwk->win_s[ 0 ];
	
	GF_BGL_BmpWinDataFill( win, 0x00 );
	str1 = MSGMAN_AllocString( wk->sys.man, id );
	x = BR_print_x_Get( win, str1 );
	GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, 0, MSG_NO_PUT, PRINT_COL_BOX_SHOT, NULL );		
	STRBUF_Delete( str1 );		
	
	GF_BGL_BmpWinOnVReq( win );
}

static void BoxShot_BoxWinAdd( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int ofs = 1;
	int i;
	int x;
	STRBUF* str1;
	
	for ( i = 0; i < 1; i++ ){
		win = &bwk->win_s[ i ];
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_S, 12, 6, 8, 4, eBG_PAL_FONT, ofs );
		GF_BGL_BmpWinDataFill( win, 0x00 );
		
		str1 = MSGMAN_AllocString( wk->sys.man, msg_429 );
		
        {
            // MatchComment: localization change for new plat feature (inclusion of FontProc_GetPrintMaxLineWidth)
            u32 width = (64 - FontProc_GetPrintMaxLineWidth(FONT_SYSTEM, str1, 0)) / 2;
            GF_STR_PrintColor( win, FONT_SYSTEM, str1, width, 0, MSG_NO_PUT, PRINT_COL_BOX_SHOT, NULL );
		}

		GF_BGL_BmpWinOnVReq( win );
		
		STRBUF_Delete( str1 );
	}
}


static void BoxShot_BoxWinDel( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BmpWinOff( &bwk->win_s[ 0 ] );
	GF_BGL_BmpWinDel( &bwk->win_s[ 0 ] );		
}

static void BoxShot_SelectWinDel( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	int i;
	
	for ( i = 0; i < 2; i++ ){
		GF_BGL_BmpWinOff( &bwk->win_s[ i ] );
		GF_BGL_BmpWinDel( &bwk->win_s[ i ] );
	}	
}

static const u8 win_data_new20[][ 4 ] = {
	{  4, 3, 24, 2 },	///< たいとる
};

static void BoxShot_New20_WinAdd( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int ofs = 1;
	int i;
	int x;
	int y;
	int mes[] = { msg_422, msg_19, msg_420 };
	STRBUF* str1;
	
	for ( i = 0; i < NELEMS( win_data_new20 ); i++ ){
		win = &bwk->win_m[ i ];
		GF_BGL_BmpWinInit( win );
		GF_BGL_BmpWinAdd( wk->sys.bgl, win, GF_BGL_FRAME2_M, win_data_new20[ i ][ 0 ], win_data_new20[ i ][ 1 ], win_data_new20[ i ][ 2 ], win_data_new20[ i ][ 3 ], eBG_PAL_FONT, ofs );
		
		GF_BGL_BmpWinDataFill( win, 0x00 );
		
		if ( i == 0 ){
			str1 = MSGMAN_AllocString( wk->sys.man, msg_box_index_000 + bwk->prev.category );
			x = BR_print_x_Get( win, str1 );
			y = 0;
		}
		
		if ( str1 ){
			GF_STR_PrintColor( win, FONT_SYSTEM, str1, x, y, MSG_NO_PUT, PRINT_COL_BOX_SHOT, NULL );
			STRBUF_Delete( str1 );
		}
		
		GF_BGL_BmpWinOnVReq( win );
		ofs += ( win_data_new20[ i ][ 2 ] * win_data_new20[ i ][ 3 ] );
	}
}

static void BoxShot_New20_WinDel( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GF_BGL_BMPWIN* win;
	int i;
	
	for ( i = 0; i < NELEMS( win_data_new20 ); i++ ){
		GF_BGL_BmpWinOff( &bwk->win_m[ i ] );
		GF_BGL_BmpWinDel( &bwk->win_m[ i ] );		
	}
}


static const TOUCH_LIST_HEADER tl_head_boxshot = {
	NULL,
	0,
	3,					///< 1行幅 = 2
	
	 3, 7,
	26,15,
	64,					///< 見出し考慮
	GF_BGL_FRAME2_M,
	
	5,					///< 表示件数
	NULL,				///< 当たり判定
};

static BOOL BoxShot_SelectIn( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	GDS_RAP_ERROR_INFO *error_info;

	if ( BR_GDSRAP_MoveStatusAllCheck( wk ) == FALSE ){ return FALSE; }
		
	switch( wk->sub_seq ){
	case 0:
		Cursor_Visible( wk->cur_wk_top, FALSE );
		BoxShot_BoxView_Delete( wk );
		BoxShot_CategoryWin_Del( wk );		
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		
	case 1:
		BR_PaletteFade( &bwk->color, eFADE_MODE_OUT );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box_u_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box2_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
		
	case 2:
		///< 受信
		HormingCursor_Init( wk, wk->cur_wk, COMM_HORMING_CUR_CX, COMM_HORMING_CUR_CY );
		Cursor_Visible( wk->cur_wk, TRUE );
		Cursor_R_Set( wk->cur_wk, COMM_HORMING_R, COMM_HORMING_R );
		Tool_InfoMessageMain( wk, msg_info_020 );
		Snd_SePlay( eBR_SND_SEARCH );
		wk->sub_seq++;
		break;
	
	case 3:		
		if( GDSRAP_Tool_Send_BoxshotDownload( BR_GDSRAP_WorkGet( wk ), bwk->prev.category ) == TRUE ){
			wk->sub_seq++;
		}
		break;
		
	case 4:
	//	Tool_InfoMessageMainSet( wk, msg_915 );
		Tool_InfoMessageMainDel( wk );
		if( GDSRAP_ErrorInfoGet( BR_GDSRAP_WorkGet( wk ), &error_info ) == TRUE ){
			//エラー発生時の処理
			//エラーメッセージの表示等はコールバックで行われるので、
			//ここでは表示後の処理。(アプリ終了とか特定のメニューに戻すとか)
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			BR_ErrorSet( wk, eERR_BOX_RECV, error_info->result, error_info->type );
			bwk->prev.download_max = 0;
			bwk->view_mode = 0;
			CATS_SystemActiveSet( wk, TRUE );
			BoxShot_BoxView_Init( wk );			
			Cursor_Visible( wk->cur_wk_top, FALSE );
			Cursor_Active( wk->cur_wk, FALSE );
			Cursor_Visible( wk->cur_wk, FALSE );
			Cursor_R_Set( wk->cur_wk, 0, 0 );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq = 7;
		}
		else {
			Snd_SeStopBySeqNo( eBR_SND_SEARCH, 0 );
			Snd_SePlay( eBR_SND_RECV_OK );
			wk->sub_seq++;
		}
		break;
	
	case 5:
	//	if ( !GF_TP_GetTrg() ){ break; }
	//	Tool_InfoMessageMainDel( wk );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		if ( bwk->prev.download_max == 0 ){
			wk->sub_seq++;
			wk->wait = 0;
			Tool_InfoMessageMain( wk, msg_info_025 );
		}
		else {
			wk->sub_seq = 0xFF;
		}
		break;
	
	case 6:
		///< 0件の処理		
		if ( (++wk->wait) >= BB_INFO_WAIT 
		||	 GF_TP_GetTrg() == TRUE ){			
			wk->wait = 0;
			Tool_InfoMessageMainDel( wk );
			bwk->view_mode = 0;
			CATS_SystemActiveSet( wk, TRUE );
			BoxShot_BoxView_Init( wk );
		//	BoxShot_InfoMessageEx( wk, 0xFF );
			
			Cursor_Visible( wk->cur_wk_top, FALSE );
			Cursor_Active( wk->cur_wk, FALSE );
			Cursor_Visible( wk->cur_wk, FALSE );
			Cursor_R_Set( wk->cur_wk, 0, 0 );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
			CATS_SystemActiveSet( wk, FALSE );
			wk->sub_seq++;
		}
		break;
	
	case 7:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_OFF );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box1_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		break;
		
	case 8:
		BR_PaletteFade( &bwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			BR_disp_BG_InfoWindowLoad_Single( wk, GF_BGL_FRAME2_M );
			BoxShot_InfoMessage( wk, msg_info_006 );
			BoxShot_CategoryWin_Add( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
			BR_Main_SeqChange( wk, eBOX2_MAIN );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	
	default:
		BR_PaletteFade( &bwk->color, eFADE_MODE_IN );
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			///< 最新20件のボックス描画処理
			BoxShot_New20_WinAdd( wk );
			BoxName_and_PlayerName_Create( wk );
			{
				CATS_SystemActiveSet( wk, TRUE );
				bwk->tl_box_shot_head = tl_head_boxshot;
				bwk->tl_box_shot_head.data_max = bwk->prev.download_max;			///< ヘッダー更新
				Slider_InitEx( &wk->sys.slider, wk, eTAG_EX_BOX_SLIDER );			///< スライダーの初期化
				TouchList_WorkClear( &wk->sys.touch_list );							///< リストワークの初期化
				TouchList_InitBoxShot( &wk->sys.touch_list, wk, &bwk->tl_box_shot_head, &bwk->box_str[ 0 ]  );	///< リストの初期化
				Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
				TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_MAIN );	///< リストのカーソル登録
				TouchList_CursorPosSet( &wk->sys.touch_list, 56, 176 );				///< カーソルの位置変更
				CATS_SystemActiveSet( wk, FALSE );
			}
			BoxShot_SelectWinAdd( wk );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			{
				HormingCursor_Init( wk, wk->cur_wk, 16, 68 + ( wk->sys.touch_list.view.l_pos * 24 ) );
				Cursor_Visible( wk->cur_wk, TRUE );
				Cursor_R_Set( wk->cur_wk, LIST_HORMING_R, LIST_HORMING_R );
			}
			BR_Main_SeqChange( wk, eBOX2_SELECT );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, bwk->color, wk->sys.logo_color );		///< 上下カーソルがタグパレットなので
		break;
	}	
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief	プレイヤーとボックス名をメモリ確保する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxName_and_PlayerName_Create( BR_WORK* wk )
{
	int i;
	BOX_WORK* bwk = wk->sub_work;
	
	for ( i = 0; i < bwk->prev.download_max; i++ ){				
		bwk->box_str[ i ][ 1 ] = STRBUF_Create( 20, HEAPID_BR );
		STRBUF_SetStringCode( bwk->box_str[ i ][ 1 ], bwk->recv_data[ i ]->profile.name );
		BR_ErrorStrChange( wk, bwk->box_str[ i ][ 1 ] );

		bwk->box_str[ i ][ 0 ] = STRBUF_Create( 30, HEAPID_BR );
		STRBUF_SetStringCode( bwk->box_str[ i ][ 0 ], bwk->recv_data[ i ]->box_shot.box_name );
	}
}

//--------------------------------------------------------------
/**
 * @brief	プレイヤーとボックス名を破棄する
 *
 * @param	wk	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void BoxName_and_PlayerName_Delete( BR_WORK* wk )
{
	int i;
	BOX_WORK* bwk = wk->sub_work;	
	
	for ( i = 0; i < bwk->prev.download_max; i++ ){		
		if ( bwk->box_str[ i ][ 0 ] ){
			STRBUF_Delete( bwk->box_str[ i ][ 0 ] );
			bwk->box_str[ i ][ 0 ] = NULL;
		}		
		if ( bwk->box_str[ i ][ 1 ] ){
			STRBUF_Delete( bwk->box_str[ i ][ 1 ] );
			bwk->box_str[ i ][ 1 ] = NULL;
		}
	}
}

static BOOL BoxShot_Select( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	if ( bwk->view_mode == 0 ){
		Slider_HitCheck_Main( &wk->sys.slider );
		TouchList_ViewMode_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
		TouchList_MainBoxShot(  &wk->sys.touch_list, wk, &bwk->box_str[ 0 ] );
		Cursor_PosSet_Check( wk->cur_wk, 16, 68 + ( wk->sys.touch_list.view.l_pos * 24 ) );
	}
			
	BMN_Main( bwk->btn_man[ 3 ] );

	return FALSE;
}

static BOOL BoxShot_SelectOut( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:	
		BoxName_and_PlayerName_Delete( wk );
		BoxShot_SelectWinDel( wk );		
		if ( bwk->view_mode == 0 ){
			BoxShot_New20_WinDel( wk );
			
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
			TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
			TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
			CATS_SystemActiveSet( wk, FALSE );
		}
		else {
			CATS_SystemActiveSet( wk, TRUE );
			GppDataFree( &bwk->gpp_work, wk );
			BoxShot_GppDataDelete( wk );
			CATS_SystemActiveSet( wk, FALSE );
		}
		bwk->view_mode = 0;
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_BoxView_Init( wk );
		BoxShot_InfoMessageEx( wk, 0xFF );
		
		Cursor_Visible( wk->cur_wk_top, FALSE );
		Cursor_Active( wk->cur_wk, FALSE );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
	
	case 1:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box1_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		break;
		
	case 2:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_SUB ) ){
			BR_disp_BG_InfoWindowLoad_Single( wk, GF_BGL_FRAME2_M );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			BoxShot_InfoMessage( wk, msg_info_006 );
			BoxShot_CategoryWin_Add( wk );
			BR_Main_SeqChange( wk, eBOX2_MAIN );
		}
		break;
	
	default:
		break;
	}	
	return FALSE;
}

static BOOL BoxShot_BoxIn( BR_WORK* wk )
{	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		BoxShot_SelectWinDel( wk );		
		if ( bwk->view_mode == 0 ){
			BoxShot_New20_WinDel( wk );
			CATS_SystemActiveSet( wk, TRUE );
			Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
			TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
			TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
			CATS_SystemActiveSet( wk, FALSE );
		}
		else {
			CATS_SystemActiveSet( wk, TRUE );
			GppDataFree( &bwk->gpp_work, wk );
			BoxShot_GppDataDelete( wk );
			CATS_SystemActiveSet( wk, FALSE );
		}
//		bwk->view_mode = 1;
		Cursor_Visible( wk->cur_wk_top, FALSE );
		Cursor_Active( wk->cur_wk, FALSE );
		Cursor_Visible( wk->cur_wk, FALSE );
		Cursor_R_Set( wk->cur_wk, 0, 0 );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
	
	case 1:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			wk->sub_seq++;
		}
		break;
	
	case 2:
		///< 上下画面の転送
		{
			///< 上
			CATS_SystemActiveSet( wk, TRUE );
			bwk->box_shot = bwk->recv_data[ wk->sys.touch_list.view.this ]->box_shot;
			BoxShot_BoxCurPosSet( bwk );
			BR_disp_BG_InfoWindowLoad_Single( wk, GF_BGL_FRAME2_M );

			BoxShot_BoxView_Init( wk );			
			BoxShot_BoxView( wk );
			BoxShot_InfoMessageEx( wk, 0xFF );

			BoxShot_PokeSoftSpriteEnable( wk, TRUE );
			BoxShot_PokeIconEnable( wk, FALSE );
			CATS_SystemActiveSet( wk, FALSE );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );

			///< 下
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box5_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
		}
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
		GF_Disp_GXS_VisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		wk->sub_seq++;
		break;
	
	case 3:
		///< alpha
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			BoxShot_PokeSoftSpriteEnable( wk, FALSE );
			BoxShot_PokeIconEnable( wk, TRUE );
			BoxShot_BoxWinAdd( wk );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			G2_BlendNone();
			wk->sub_seq++;
		}
		break;
	
	default:
		///< フェード		
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_IN ) ){
			bwk->btn = bwk->btn_man[ 0 ];
			BoxShot_BoxCurPosSet( bwk );		
			bwk->poke_x = bwk->box_cur_pos % 6;
			bwk->poke_y = bwk->box_cur_pos / 6;
			HormingCursor_Init( wk, wk->cur_wk, 110 + ( bwk->poke_x * 24 ), 52 + ( bwk->poke_y * 22 ) );
			Cursor_Visible( wk->cur_wk, TRUE );
			Cursor_R_Set( wk->cur_wk, LIST_HORMING_R, LIST_HORMING_R );
			if ( bwk->box_shot.monsno[ bwk->box_cur_pos ] != 0 
			&&	 IsEggCheck( bwk->box_shot.egg_bit, ( 1 << bwk->box_cur_pos ) ) != TRUE ){
				Snd_PMVoicePlay( bwk->box_shot.monsno[ bwk->box_cur_pos ], bwk->box_shot.form_no[ bwk->box_cur_pos ] );
			}		
			BR_Main_SeqChange( wk, eBOX2_BOX );
		}
		ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ,	LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );
		break;
	}	
	return FALSE;	
}

static BOOL BoxShot_Box( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	BMN_Main( bwk->btn_man[ 4 ] );	
	
	return FALSE;
}

static BOOL BoxShot_BoxOut( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		BoxShot_PokeSoftSpriteDel( wk );
		BoxShot_PokeIconDel( wk );
		BoxShot_PokeName_BoxName_OFF( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		BoxShot_BoxView_Delete( wk );
		BoxShot_BoxWinDel( wk );
		wk->sub_seq++;
		
	case 1:
		bwk->view_mode = 0;
		wk->sub_seq++;
	
	case 2:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_ALL ) ){
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box_u_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box2_NSCR, wk->sys.bgl, GF_BGL_FRAME3_S, 0, 0, 0, HEAPID_BR );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			wk->sub_seq++;
		}
		break;
		
	default:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_ALL ) ){
			CATS_SystemActiveSet( wk, TRUE );
			Cursor_Visible( wk->cur_wk, TRUE );
			BoxShot_New20_WinAdd( wk );
			BoxShot_SelectWinAdd( wk );
			{
				Slider_InitEx( &wk->sys.slider, wk, eTAG_EX_BOX_SLIDER );			///< スライダーの初期化
				TouchList_InitBoxShot( &wk->sys.touch_list, wk, &bwk->tl_box_shot_head, &bwk->box_str[ 0 ]  );	///< リストの初期化
				Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
				TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_MAIN );	///< リストのカーソル登録
				TouchList_CursorPosSet( &wk->sys.touch_list, 56, 176 );				///< カーソルの位置変更
			}
			{
				HormingCursor_Init( wk, wk->cur_wk, 16, 68 + ( wk->sys.touch_list.view.l_pos * 24 ) );
				Cursor_Visible( wk->cur_wk, TRUE );
				Cursor_R_Set( wk->cur_wk, LIST_HORMING_R, LIST_HORMING_R );
			}
			if ( bwk->view_mode == 0 ){
				Slider_HitCheck_Main( &wk->sys.slider );
				TouchList_ViewMode_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
				TouchList_MainBoxShot(  &wk->sys.touch_list, wk, &bwk->box_str[ 0 ] );
				Cursor_PosSet_Check( wk->cur_wk, 16, 68 + ( wk->sys.touch_list.view.l_pos * 24 ) );
			}
			CATS_SystemActiveSet( wk, FALSE );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			BR_Main_SeqChange( wk, eBOX2_SELECT );
		}
		break;
	}
	return FALSE;
}

static BOOL BoxShot_ProfileIn( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		Cursor_Visible( wk->cur_wk, FALSE );
		if ( bwk->view_mode == 0 ){
			BoxShot_New20_WinDel( wk );
		}
		{
			Slider_Exit( &wk->sys.slider );					///< スライダーの破棄
			TouchList_Exit( &wk->sys.touch_list );			///< リストの破棄
			TouchList_CursorDel( &wk->sys.touch_list, wk );	///< リストのカーソル破棄
		}
		bwk->view_mode = 1;
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		Plate_AlphaSetPlane( &bwk->eva, &bwk->evb, DEF_MP1, DEF_MP2, DEF_SP1, DEF_SP2, ePLANE_MAIN );
		GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
		wk->sub_seq++;
		
	case 1:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			BoxShot_GppDataCreate( wk, wk->sys.touch_list.view.this );
			CATS_SystemActiveSet( wk, TRUE );
			GppDataMake( &bwk->gpp_work, wk );
			GppData_IconEnable( &bwk->gpp_work, FALSE );
			CATS_SystemActiveSet( wk, FALSE );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_bg1uc_data_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		break;
		
	case 2:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			GppData_IconEnable( &bwk->gpp_work, TRUE );
			ProfWin_MsgSet( wk, msg_718 + bwk->view_mode ); 
			GF_Disp_GXS_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			BR_Main_SeqChange( wk, eBOX2_SELECT );
		}
		break;
	
	default:
		break;
	}	
	return FALSE;	
}


static BOOL BoxShot_ProfileOut( BR_WORK* wk )
{	
	BOX_WORK* bwk = wk->sub_work;
	
	switch( wk->sub_seq ){
	case 0:
		bwk->view_mode = 0;
		wk->sub_seq++;
		break;
	
	case 1:
		CATS_SystemActiveSet( wk, TRUE );
		GppDataFree( &bwk->gpp_work, wk );
		BoxShot_GppDataDelete( wk );
		CATS_SystemActiveSet( wk, FALSE );
		wk->sub_seq++;
	
	case 2:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_OUT, ePLANE_MAIN ) ){
			GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );
			ArcUtil_HDL_BgCharSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_data_NCGR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			ArcUtil_HDL_ScrnSet( wk->sys.p_handle, NARC_batt_rec_gra_batt_rec_box_u_NSCR, wk->sys.bgl, GF_BGL_FRAME3_M, 0, 0, 0, HEAPID_BR );
			wk->sub_seq++;
		}
		break;
		
	default:
		if ( Plate_AlphaFade( &bwk->eva, &bwk->evb, eFADE_MODE_IN, ePLANE_MAIN ) ){
			CATS_SystemActiveSet( wk, TRUE );
			ProfWin_MsgSet( wk, msg_718 + bwk->view_mode ); 
			BoxShot_New20_WinAdd( wk );
//			BoxShot_SelectWinAdd( wk );
			{
				Slider_InitEx( &wk->sys.slider, wk, eTAG_EX_BOX_SLIDER );			///< スライダーの初期化
				TouchList_InitBoxShot( &wk->sys.touch_list, wk, &bwk->tl_box_shot_head, &bwk->box_str[ 0 ]  );	///< リストの初期化
				Slider_AnimeCheck( &wk->sys.slider, &wk->sys.touch_list );			///< アニメチェック
				TouchList_CursorAdd( &wk->sys.touch_list, wk, CATS_D_AREA_MAIN );	///< リストのカーソル登録
				TouchList_CursorPosSet( &wk->sys.touch_list, 56, 176 );				///< カーソルの位置変更
			}
			if ( bwk->view_mode == 0 ){
				Slider_HitCheck_Main( &wk->sys.slider );
				TouchList_ViewMode_ValueSet( &wk->sys.touch_list, wk->sys.slider.value );
				TouchList_MainBoxShot(  &wk->sys.touch_list, wk, &bwk->box_str[ 0 ] );
				Cursor_PosSet_Check( wk->cur_wk, 16, 68 + ( wk->sys.touch_list.view.l_pos * 24 ) );
			}
			Cursor_Visible( wk->cur_wk, TRUE );
//			ProfWin_MsgSet( wk, msg_718 + bwk->view_mode ); 
//			BoxShot_New20_WinAdd( wk );
			GF_Disp_GX_VisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
			CATS_SystemActiveSet( wk, FALSE );
			BR_Main_SeqChange( wk, eBOX2_SELECT );
		}
		break;
	}
	return FALSE;
}

static BOOL BR_BoxR_Exit( BR_WORK* wk )
{
	BOX_WORK* bwk = wk->sub_work;

	switch ( wk->sub_seq ){
	case 0:
		CATS_SystemActiveSet( wk, TRUE );
		BoxShot_BoxView_Delete( wk );
		BoxShot_FontButton_Delete( wk );
		Cursor_Active( wk->cur_wk_top, FALSE );
		Cursor_Visible( wk->cur_wk_top, FALSE );
		Cursor_Active( wk->cur_wk, TRUE );
		Cursor_Visible( wk->cur_wk, FALSE );
		CATS_SystemActiveSet( wk, FALSE );
		BR_ChangeDisplayVIntr( wk, DISP_3D_TO_SUB );				///< main = top
		wk->sub_seq++;
		break;
	
	case 1:
		if ( BR_IsChangeDisplay( wk ) == FALSE ){ break; }
		{ 
			int i;			
			for ( i = 0; i < 5; i++ ){
				BMN_Delete( bwk->btn_man[ i ] );
			}
		}
		BR_PaletteFadeIn_Init( &bwk->color );
		NormalTag_RecoverAllOp( wk );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME2_S );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_M );
		GF_BGL_ScrClear( wk->sys.bgl, GF_BGL_FRAME3_S );
		wk->sub_seq++;
		break;
	
	default:
		if ( BR_PaletteFade( &bwk->color, eFADE_MODE_IN ) ){					
			PaletteFadeClear( wk );
			BR_PaletteFadeIn_Init( &bwk->color );
			ColorConceChangePfd( wk->sys.pfd, FADE_MAIN_OBJ, TAG_PALETTE, 16, wk->sys.logo_color );	///< main	oam
			
			BR_Main_ProcSeqChange( wk, eMAIN_Input, TRUE, wk->proc_seq );
			BR_Main_SeqChange( wk, eINPUT_MENU_IN );
			GDS_Profile_FreeMemory( bwk->prev.gds_profile_ptr );
			sys_FreeMemoryEz( bwk );
		}
		else {
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_BG,	 LOGO_PALETTE, bwk->color, wk->sys.logo_color );	///< main	oam
			ColorConceChangePfd( wk->sys.pfd, FADE_SUB_OBJ, LINE_OTHER_SUB_PALETTE, bwk->color, wk->sys.logo_color );	///< main	oam
		}
		break;
	}		
	
	return FALSE;
}


// =============================================================================
//
//
//	■通信周り
//
//
// =============================================================================
void BR_Response_BoxRegist( void *work, const GDS_RAP_ERROR_INFO *error_info )
{
	BOX_WORK *bwk = work;
	
	OS_TPrintf("ボックスショットのアップロードレスポンス取得\n");
	if( error_info->occ == TRUE ){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし
	}
}

void BR_Response_BoxGet( void *work, const GDS_RAP_ERROR_INFO *error_info )
{
	BR_WORK* wk = work;
	BOX_WORK* bwk = wk->sub_work;
	
	OS_TPrintf("ボックスショットのダウンロードレスポンス取得\n");

	if( error_info->occ == TRUE ){
		//TRUEならばエラー発生しているので、ここでメニューを戻すとかアプリ終了モードへ移行とかする
		bwk->prev.download_max = 0;
	}
	else{
		//正常時ならば受信バッファからデータ取得などを行う
		//アップロードの場合は特に必要なし

		bwk->prev.download_max = GDS_RAP_RESPONSE_Boxshot_Download_RecvPtr_Set( BR_GDSRAP_WorkGet( wk ), bwk->recv_data, BOX_SHOT_RECV_DATA_NUM );
		MI_CpuCopy8( bwk->recv_data, &bwk->recv_data_tmp, sizeof( GT_BOX_SHOT_RECV ) * BOX_SHOT_RECV_DATA_NUM );	
	}
}

